#include <WiFiClientSecure.h>
String LINE_TOKEN = "GiSV8ZnaDqwzZdZBD3NkwYWnkZvOG0P6gQgFm3mZ1py";


int Line_Notify(String message) {
  WiFiClientSecure client;
  if (!client.connect("notify-api.line.me", 443)) {
    Serial.println("connection failed");
    return 404;
  }
  String messg = "message=[" + nodename + "] \n" + message;

  String req = "";
  req += "POST /api/notify HTTP/1.1\r\n";
  req += "Host: notify-api.line.me\r\n";
  req += "Authorization: Bearer " + String(LINE_TOKEN) + "\r\n";
  req += "Cache-Control: no-cache\r\n";
  req += "User-Agent: ESP8266\r\n";
  req += "Content-Type: application/x-www-form-urlencoded\r\n";

  req += "Content-Length: " + String(messg.length()) + "\r\n";
  req += "\r\n";
  req += messg;
  req += "\r\n";

  client.print(req);
  delay(20);
  //Serial.println("-------------");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      break;
    }
    //Serial.println(line);
  }
  return 0;
}
