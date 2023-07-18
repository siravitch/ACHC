#include "Wi_Fi.h"
#include "localtime.h"
#include "pms_senser.h"
#include "dht22.h"
#include "line.h"
#include "led.h"
#include <Ticker.h>  //import ticker timer

String GOOGLE_SCRIPT_ID = "AKfycbwQjtwg7hKJbqkfaHR8LSwKJjKD1d2JUW2gnx4oB0lePglAe0Tml2kiUW5Cargm689F";
//https://script.google.com/macros/s/AKfycbwQjtwg7hKJbqkfaHR8LSwKJjKD1d2JUW2gnx4oB0lePglAe0Tml2kiUW5Cargm689F/exec


//updated 04.12.2019
const char* root_ca =
  "-----BEGIN CERTIFICATE-----\n"
  "MIIDujCCAqKgAwIBAgILBAAAAAABD4Ym5g0wDQYJKoZIhvcNAQEFBQAwTDEgMB4G\n"
  "A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjIxEzARBgNVBAoTCkdsb2JhbFNp\n"
  "Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDYxMjE1MDgwMDAwWhcNMjExMjE1\n"
  "MDgwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEG\n"
  "A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n"
  "hvcNAQEBBQADggEPADCCAQoCggEBAKbPJA6+Lm8omUVCxKs+IVSbC9N/hHD6ErPL\n"
  "v4dfxn+G07IwXNb9rfF73OX4YJYJkhD10FPe+3t+c4isUoh7SqbKSaZeqKeMWhG8\n"
  "eoLrvozps6yWJQeXSpkqBy+0Hne/ig+1AnwblrjFuTosvNYSuetZfeLQBoZfXklq\n"
  "tTleiDTsvHgMCJiEbKjNS7SgfQx5TfC4LcshytVsW33hoCmEofnTlEnLJGKRILzd\n"
  "C9XZzPnqJworc5HGnRusyMvo4KD0L5CLTfuwNhv2GXqF4G3yYROIXJ/gkwpRl4pa\n"
  "zq+r1feqCapgvdzZX99yqWATXgAByUr6P6TqBwMhAo6CygPCm48CAwEAAaOBnDCB\n"
  "mTAOBgNVHQ8BAf8EBAMCAQYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUm+IH\n"
  "V2ccHsBqBt5ZtJot39wZhi4wNgYDVR0fBC8wLTAroCmgJ4YlaHR0cDovL2NybC5n\n"
  "bG9iYWxzaWduLm5ldC9yb290LXIyLmNybDAfBgNVHSMEGDAWgBSb4gdXZxwewGoG\n"
  "3lm0mi3f3BmGLjANBgkqhkiG9w0BAQUFAAOCAQEAmYFThxxol4aR7OBKuEQLq4Gs\n"
  "J0/WwbgcQ3izDJr86iw8bmEbTUsp9Z8FHSbBuOmDAGJFtqkIk7mpM0sYmsL4h4hO\n"
  "291xNBrBVNpGP+DTKqttVCL1OmLNIG+6KYnX3ZHu01yiPqFbQfXf5WRDLenVOavS\n"
  "ot+3i9DAgBkcRcAtjOj4LaR0VknFBbVPFd5uRHg5h6h+u/N5GJG79G+dwfCMNYxd\n"
  "AfvDbbnvRG15RjF+Cv6pgsH/76tuIMRQyV+dTZsXjAzlAcmgQWpzU/qlULRuJQ/7\n"
  "TBj0/VLZjmmx6BEP3ojY+x1J96relc8geMJgEtslQIxq/H5COEBkEveegeGTLg==\n"
  "-----END CERTIFICATE-----\n";


WiFiClientSecure client;

Ticker Tic10m;             //Ticker 10 m
boolean Ticker10m = true;  //ticker every 10 m

Ticker Tic1m;             //Ticker 1 m
boolean Ticker1m = true;  //ticker every 1 m

String datasend = "";

void setup() {
  Serial.begin(115200);
  configWiFi();

  setuptime();
  looptime();

  if (year() < 2000) {
    delay(3000);
    ESP.restart();
  }


  Line_Notify("START : [ " + digitalClockShow() + " ]");

  setpms();
  setupdht();

  Tic1m.attach(60, tick1m);     //Interrupt every 5m
  Tic10m.attach(600, tick10m);  //Interrupt every 5m

  setupled();
  
}

void loop() {

  if (Ticker1m == true) {  //setting timer 5 m
    loopdht();
    getpms();

    datasend = "Temperature=" + String(RTemperature) + "&Humidity=" + String(RHumidity) + "&Heatindex=" + String(RHeatindex) + "&Pm25=" + String(Rpm25) + "&Pm10=" + String(Rpm10) + "";
    Serial.println(datasend);

    Ticker1m = false;
  }

  if (Ticker10m == true) {  //setting timer 5 m

    if (Countdht > 0) {
      Temperature = Temperature / Countdht;
      Humidity = Humidity / Countdht;
      Heatindex = Heatindex / Countdht;
      Countdht = 0;
    }

    if (Countpms > 0) {
      pm25 = pm25 / Countpms;
      pm10 = pm10 / Countpms;
      Countpms = 0;
    }





    datasend = "Temperature=" + String(Temperature) + "&Humidity=" + String(Humidity) + "&Heatindex=" + String(Heatindex) + "&Pm25=" + String(pm25) + "&Pm10=" + String(pm10) + "";
    sendData(datasend);

    String message = "วันที่-เวลา :" + String(digitalClockShow());

    message += "\n\nอุณณหภูมิในพื้นที่เท่ากับ : " + String(Temperature) + " ºC\nความชื้นสัมพัทธ์เท่ากับ : " + String(Humidity) + " ％ \nดัชนีความร้อนเท่ากับ : " + String(Heatindex) + " ºC";

    if (Heatindex < 27) {
      message += "\n ข้อแนะนำ : " + DHTmessage[0];
    } else if (Heatindex >= 27 && Heatindex < 33) {
      message += "\n ข้อแนะนำ : " + DHTmessage[1];
    } else if (Heatindex >= 33 && Heatindex < 40) {
      message += "\n ข้อแนะนำ : " + DHTmessage[2];
    } else if (Heatindex >= 40 && Heatindex < 52) {
      message += "\n ข้อแนะนำ : " + DHTmessage[3];
    } else {
      message += "\n ข้อแนะนำ : " + DHTmessage[4];
    }

    message += "\n\nค่าฝุ่น PM 2.5 : " + String(pm25) + " µg/m³";

    if (pm25 < 50) {
      message += "\n ข้อแนะนำ : " + PMmessage[0];
    } else if (pm25 >= 50 && pm25 < 101) {
      message += "\n ข้อแนะนำ : " + PMmessage[1];
    } else if (pm25 >= 101 && pm25 < 151) {
      message += "\n ข้อแนะนำ : " + PMmessage[2];
    } else {
      message += "\n ข้อแนะนำ : " + PMmessage[3];
    }

    message += "\n\nค่าฝุ่น PM 10 : " + String(pm10) + " µg/m³";

    if (pm10 < 150) {
      message += "\n ข้อแนะนำ : " + PMmessage[0];
    } else if (pm10 >= 150 && pm10 < 251) {
      message += "\n ข้อแนะนำ : " + PMmessage[1];
    } else if (pm10 >= 251 && pm10 < 351) {
      message += "\n ข้อแนะนำ : " + PMmessage[2];
    } else {
      message += "\n ข้อแนะนำ : " + PMmessage[3];
    }

    Serial.println(message);
    Line_Notify(message);


    if (pm10 < 100) {
      setled(0, 0, 22.9);
    } else if (pm10 < 200) {
      setled(0, 22.9, 0);
    } else if (pm10 < 350) {
      setled(25.5, 25.5, 1);
    } else if (pm10 < 420) {
      setled(25.5, 12.7, 0);
    } else if (pm10 < 550) {
      setled(22.9, 0, 0);
    } else if (pm10 < 650) {
      setled(15.3, 1, 7.8);
    } else {
      setled(12.7, 2, 3.6);
    }


    Ticker10m = false;
  }
  delay(50);
}

void sendData(String params) {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + params;
  Serial.println(url);
  Serial.print("Making a request");
  http.begin(url, root_ca);  //Specify the URL and certificate
  int httpCode = http.GET();
  http.end();
  Serial.println(": done " + httpCode);
}

void tick1m() {
  Ticker1m = true;
}

void tick10m() {
  Ticker10m = true;
}