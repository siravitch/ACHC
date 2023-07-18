#include "PMS.h"

String PMmessage[] = { "สามารถทำกิจกรรมได้", "ควรลดกิจกรรมกลางแจ้ง", "ควนสวมหน้ากาก N95 และลดกิจกรรมกลางแจ้ง", "ยกเลิกกิจกรรมกลางแจ้ง" };

#define RXD2 16
#define TXD2 17

PMS pms(Serial2);
PMS::DATA data;


int pm25, pm10;
float Rpm25, Rpm10;

int Countpms = 0;

void setpms() {
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
}

void getpms() {
  do {

    pms.requestRead();
    if (pms.readUntil(data)) {
      Rpm25 = data.PM_AE_UG_2_5;
      Rpm10 = data.PM_AE_UG_10_0;
    }
  } while (Rpm25 == 0 && Rpm10 == 0);


  if (Rpm25 > 0.00 && Rpm10 > 0.00) {
    pm25 += Rpm25;
    pm10 += Rpm10;
    Countpms += 1;
  }

  if (pm25 == 0 || pm10 == 0) {
    pm25 = Rpm25;
    pm10 = Rpm10;
  }
}
