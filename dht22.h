#include "DHT.h"


#define DHTTYPE DHT22  // DHT 22  (AM2302), AM2321

const int DHT22PIN = 4;

DHT dht(DHT22PIN, DHTTYPE);

float Temperature = 0.00;
float Humidity = 0.00;
float Heatindex = 0.00;

float RTemperature = 0.00;
float RHumidity = 0.00;
float RHeatindex = 0.00;

int Countdht = 0;

String DHTmessage[] = { "ทำกิจกรรมได้ปกติ ดื่มน้ำ 500 ml ต่อ ชม.", "ทำกิจกรรมได้ 40 นาที พัก 20 นาที ดื่มน้ำ 750 ml ต่อ ชม.", "ทำกิจกรรมได้ 40 นาที พัก 20 นาที ดื่มน้ำ 1 L ต่อ ชม.", "ทำกิจกรรมได้ 30 นาที พัก 30 นาที ดื่มน้ำ 1 L ต่อ ชม.", "ทำกิจกรรมได้ 20 นาที พัก 40 นาที ดื่มน้ำ 1 L ต่อ ชม." };


void setupdht() {
  dht.begin();
}

void loopdht() {

  // Check if any reads failed and exit early (to try again).
  if (isnan(dht.readHumidity()) || isnan(dht.readTemperature())) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  RHumidity = dht.readHumidity();
  RTemperature = dht.readTemperature();
  RHeatindex = dht.computeHeatIndex(RTemperature, RHumidity, false);



  if (RTemperature > 0.00 && RHumidity > 0.00 && RHeatindex > 0.00) {
    Temperature += RTemperature;
    Humidity += RHumidity;
    Heatindex += RHeatindex;
    Countdht += 1;
  }

  if (Temperature == 0 || Humidity == 0 || Heatindex == 0) {
    Temperature = RTemperature;
    Humidity = RHumidity;
    Heatindex = RHeatindex;
  }
}