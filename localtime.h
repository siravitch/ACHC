#include <time.h>
#include <TimeLib.h>

// NTP Servers:
static const char ntpServerName[] = "th.pool.ntp.org";
const int timeZone = 7;  // Central European Time

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
String digitalClockDisplay();
String printDigits(int digits);
void sendNTPpacket(IPAddress &address);

String readtime = "";

const int NTP_PACKET_SIZE = 48;      // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE];  //buffer to hold incoming & outgoing packets

int limitbackup = 48;    //limit data backup [192 record/(60/5=12)]=16 Hr
String namebk[49] = "";  //backup filename
int pvhr = 0, cuhr = 0;  //
/////////////////////////////////////////////////////////////////////////////////////////
void setuptime() {
  while (!Serial)
    ;  // Needed for Leonardo only
  delay(250);
  Serial.println("waiting for sync");
  Udp.begin(localPort);
  setSyncProvider(getNtpTime);
  setSyncInterval(100000);
}

time_t prevDisplay = 0;  // when the digital clock was displayed

void looptime() {
  if (timeStatus() != timeNotSet) {
    if (now() != prevDisplay) {  //update the display only if time has changed
      prevDisplay = now();
      digitalClockDisplay();
    }
  }
}

String digitalClockDisplay() {
  readtime = String(year());
  readtime += "-";
  readtime += String(printDigits(month()));
  readtime += "-";
  readtime += String(printDigits(day()));
  readtime += " ";
  readtime += String(printDigits(hour()));
  readtime += ":";
  readtime += String(printDigits(minute()));
  // readtime += ":";
  // readtime += String(printDigits(second()));
  return readtime;
}

String digitalClockShow() {
  readtime = String(printDigits(day()));
  readtime += "-";
  readtime += String(printDigits(month()));
  readtime += "-";
  readtime += String(year());
  readtime += " ";
  readtime += String(printDigits(hour()));
  readtime += ":";
  readtime += String(printDigits(minute()));
  readtime += ":";
  readtime += String(printDigits(second()));
  return readtime;
}

String printDigits(int digits) {
  if (digits < 10) {
    return "0" + String(digits);
  } else {
    return String(digits);
  }
}

/*-------- NTP code ----------*/
time_t getNtpTime() {
  IPAddress ntpServerIP;  // NTP server's ip address

  while (Udp.parsePacket() > 0)
    ;  // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 = (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + timeZone * SECS_PER_HOUR;
    }
  }
  Serial.println("No NTP Response :-(");
  return 0;  // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address) {
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;  // LI, Version, Mode
  packetBuffer[1] = 0;           // Stratum, or type of clock
  packetBuffer[2] = 6;           // Polling Interval
  packetBuffer[3] = 0xEC;        // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123);  //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}
