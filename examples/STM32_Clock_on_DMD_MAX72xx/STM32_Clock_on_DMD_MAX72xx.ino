/*
 * STM32_Clock_on_DMD_MAX72xx.ino
 *
 * STM32F1_RTC and DMD_MAX72xx libraries demo
 * Platform: STM32F1 series.
 *
 * Created by ZulNs, @Gorontalo, Indonesia, 15 October 2019
 * Released into the public domain.
 */

#include <stm32f1_rtc.h>
#include <DMD_MAX72xx.h>

const char * weekdayName[] = { "Minggu", "Senin", "Selasa", "Rabu", "Kamis", "Jum'at", "Sabtu" };
const char * monthName[] = { "Januari", "Februari", "Maret", "April", "Mei", "Juni", "Juli", "Agustus", "September", "Oktober", "November", "Desember" };
const char * hijriMonthName[] = { "Muharram", "Safar", "Rabi'ul-Awwal", "Rabi'ul-Akhir", "Jumadil-Awwal", "Jumadil-Akhir",
                                  "Rajab", "Sya'ban", "Ramadhan", "Syawwal", "Dzhul-Qo'dah", "Dzhul-Hijjah" };

STM32F1_RTC rtc;
DMD_MAX72xx dmd;

DateVar date;
TimeVar time;

char decStr[5];
char dateStr[32];
uint32_t secondTimer;

void setup() {
  //Serial.begin(115200);
  rtc.begin();
  dmd.begin(DMD_MAX72xx::TYPE_LRBT, 4, 2, PA4);
  randomSeed(analogRead(0));
  dmd.setBrightness(0);
  dmd.marquee(dmd.getArea(0,4, 31, 11), F("Real Time Clock and Calendar by ZulNs"), 30);
  dmd.clearBuffer();
  dmd.refresh();
  rtc.epochToTime(rtc.getTime(), time);
  printHours();
  dmd.drawChr(15, 0, ':');
  printMinutes();
}

void loop() {
  dmd.clearArea(dmd.getArea(0, 8, 31, 15));
  rtc.epochToDate(rtc.getTime(), date);
  strcpy(dateStr, weekdayName[date.weekday]);
  strcat(dateStr, ", ");
  binToDecStr(date.day);
  strcat(dateStr, decStr);
  strcat(dateStr, " ");
  strcat(dateStr, monthName[date.month - 1]);
  strcat(dateStr, " ");
  binToDecStr(date.year);
  strcat(dateStr, decStr);
  dmd.marquee(dmd.getArea(0, 8, 31, 15), dateStr, random(20, 40), 1, marqueeCheck);
  
  dmd.clearArea(dmd.getArea(0, 8, 31, 15));
  rtc.epochToHijriDate(rtc.getTime(), date);
  strcpy(dateStr, weekdayName[date.weekday]);
  strcat(dateStr, ", ");
  binToDecStr(date.day);
  strcat(dateStr, decStr);
  strcat(dateStr, " ");
  strcat(dateStr, hijriMonthName[date.month - 1]);
  strcat(dateStr, " ");
  binToDecStr(date.year);
  strcat(dateStr, decStr);
  strcat(dateStr, " H");
  dmd.marquee(dmd.getArea(0, 8, 31, 15), dateStr, random(20, 40), 1, marqueeCheck);
  
  dmd.clearArea(dmd.getArea(0, 8, 31, 15));
  for (uint8_t i = 0; i < 5; i++) {
    while (!checkSecond());
    printSeconds();
  }
  while (!checkSecond());
}

bool marqueeCheck() {
  bool chk(checkSecond());
  if (chk && time.hours == 0 && time.minutes == 0 && time.seconds == 0)
    return true;
  else if (millis() >= secondTimer + 500)
    dmd.drawChr(15, 0, ' ');
  return false;
}

bool checkSecond() {
  if (rtc.isCounterUpdated()) {
    rtc.clearSecondFlag();
    secondTimer = millis();
    dmd.drawChr(15, 0, ':');
    rtc.epochToTime(rtc.getTime(), time);
    if (time.seconds == 0) {
      printMinutes();
      if (time.minutes == 0)
        printHours();
    }
    return true;
  }
  return false;
}

void printHours() {
  uint8_t strw(binToTwoDigDecStr(time.hours));
  dmd.clearArea(dmd.getArea(2, 0, 12, 7));
  dmd.drawStr(13 - strw, 0, decStr);
  dmd.refresh();
}

void printMinutes() {
  binToTwoDigDecStr(time.minutes);
  dmd.clearArea(dmd.getArea(18, 0, 28, 7));
  dmd.drawStr(18, 0, decStr);
  dmd.refresh();
}

void printSeconds() {
  uint8_t strw(binToTwoDigDecStr(time.seconds));
  dmd.clearArea(dmd.getArea(0, 8, 31, 15));
  dmd.drawStr(10 + (11 - strw)/2, 8, decStr);
  dmd.refresh();
}

uint8_t binToTwoDigDecStr(int16_t bin) {
  uint16_t dec(binToTwoDigDec(bin));
  decStr[0] = highByte(dec);
  decStr[1] = lowByte(dec);
  decStr[2] = 0;
  return dmd.getStrWidth(decStr);
}

uint16_t binToTwoDigDec(int16_t bin) {
  uint16_t dec(bin + bin/10*6);
  dec = (dec << 4) & 0x0F00 | dec & 0x000F | 0x3030;
  return dec;
}

void binToDecStr(int16_t bin) {
  uint16_t bcd = binToBcd(bin);
  uint8_t ctr(0);
  if (bcd >= 0x1000)
    decStr[ctr++] = (bcd >> 12) | 0x30;
  if (bcd >= 0x100)
    decStr[ctr++] = (bcd >> 8) & 0x0F | 0x30;
  if (bcd >= 0x10)
    decStr[ctr++] = (bcd >> 4) & 0x0F | 0x30;
  decStr[ctr++] = bcd & 0x0F | 0x30;
  decStr[ctr] = 0;
}

uint16_t binToBcd(int16_t bin) {
  uint16_t bcd(bin/1000*0x1000);
  bcd += (bin%1000)/100*0x100;
  bcd += (bin%1000%100)/10*0x10;
  bcd += bin%1000%100%10;
  return bcd;
}
