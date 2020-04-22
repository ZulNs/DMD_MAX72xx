/*
 * MAX7219_test.ino
 *
 * DMD_MAX72xx library demo
 * Platform: AVR, ESP8266, STM32, and others.
 *
 * Created by ZulNs, @Gorontalo, Indonesia, 15 October 2019
 * Released into the public domain.
 */

#include <DMD_MAX72xx.h>

#define DELAY() delay(30)
#define DELAY2() delay(60)

DMD_MAX72xx dmd;
DMD_MAX72xx::font_info_t fi;
DMD_MAX72xx::area_t da;
const char mytext[] PROGMEM = "DMD_MAX72xx Arduino Library Demo by ZulNs, tested on Arduino AVRs, ESP8266 (prevents breaking by WDT), and STM32duino";

void setup() {
  Serial.begin(115200);
#if defined(__AVR__)
  dmd.begin(DMD_MAX72xx::TYPE_LRBT, 4, 2, 10);
#elif defined(ESP8266)
  dmd.begin(DMD_MAX72xx::TYPE_LRBT, 4, 2, 15);
#elif defined(_STM32_DEF_) || defined(_VARIANT_ARDUINO_STM32_)
  dmd.begin(DMD_MAX72xx::TYPE_LRBT, 4, 2, PA4);
#else
#error "Undefined development board...!!!"
#endif
  //dmd.setBrightness(0);
  randomSeed(analogRead(0));
  
  //ESP.wdtDisable();
  //*((volatile uint32_t*) 0x60000900) &= ~(1); // Hardware WDT OFF
  
  /*while (!Serial) {
    ESP.wdtFeed();
  }*/
}

void loop() {
  uint8_t i, rnd, strw(0);
  uint32_t timer;
  uint8_t cw[5];
  
  dmd.marquee_P(dmd.getArea(0, 4, 31, 11), mytext, 30, 3);
  
  bool sd = false, su = false;
  anime5('Z', sd);
  anime5('u', sd);
  anime5('l', sd);
  anime5('N', sd);
  anime5('s', sd);
  anime6(33, sd, su);
  anime7(7, 8, su);
  anime7(11, 14, su);
  anime7(7, 18, su);
  anime7(7, 24, su);
  
  /*cw[0] = anime1('Z', strw);
  strw += cw[0] + dmd.getFontSpacing();
  cw[1] = anime1('u', strw);
  strw += cw[1] + dmd.getFontSpacing();
  cw[2] = anime1('l', strw);
  strw += cw[2] + dmd.getFontSpacing();
  cw[3] = anime1('N', strw);
  strw += cw[3] + dmd.getFontSpacing();
  cw[4] = anime1('s', strw);
  strw += cw[4];

  anime2(cw[0], cw[0]);
  anime2(cw[1], cw[0]+cw[1]+                    dmd.getFontSpacing());
  anime2(cw[2], cw[0]+cw[1]+cw[2]+            2*dmd.getFontSpacing());
  anime2(cw[3], cw[0]+cw[1]+cw[2]+cw[3]+      3*dmd.getFontSpacing());
  anime2(cw[4], cw[0]+cw[1]+cw[2]+cw[3]+cw[4]+4*dmd.getFontSpacing());*/
  
  for (i = 0; i < 4; i++) {
    dmd.scrollDown(dmd.getArea(0, 0, 31, 15));
    dmd.refresh();
    DELAY();
  }
  
  timer = millis();
  
  while (1) {
    rnd = random(0, 7);
    for (i = 0; i < 32; i++) {
      switch (rnd) {
        case 0:
          dmd.scrollLeft(dmd.getDisplayArea(), true);
          break;
        case 1:
          dmd.scrollRight(dmd.getDisplayArea(), true);
          break;
        case 2:
          dmd.scrollUp(dmd.getDisplayArea(), true);
          break;
        case 3:
          dmd.scrollDown(dmd.getDisplayArea(), true);
          break;
        case 4:
          dmd.scrollRotateTopLeft(dmd.getDisplayArea());
          break;
        case 5:
          dmd.scrollRotateTopRight(dmd.getDisplayArea());
          break;
        case 6:
          dmd.scrollRotateBottomLeft(dmd.getDisplayArea());
          break;
        case 7:
          dmd.scrollRotateBottomRight(dmd.getDisplayArea());
      }
      dmd.refresh();
      DELAY();
      //if (millis() >= timer + 20000) break;
    }
    if (millis() >= timer + 20000) break;
  }
  
  uint8_t *clip1 = dmd.copy(dmd.getArea(2, 4, 15, 11));
  uint8_t *clip2 = dmd.copy(dmd.getArea(16, 4, 29, 11));
  uint8_t *clip3;
  for (i = 0; i < 7; i++) {
    dmd.clearBuffer();
    dmd.paste(i+3, 4, clip1);
    dmd.paste(15-i, 4, clip2, true); //orMode
    dmd.refresh();
    DELAY2();
  }
  for (i = 0; i < 6; i++) {
    clip3 = dmd.copy(dmd.getArea(9+i, 4, 9+i, 11), true); //cutMode
    dmd.paste(10+i, 4, clip3, true); //orMode
    free(clip3);
    clip3 = dmd.copy(dmd.getArea(22-i, 4, 22-i, 11), true); //cutMode
    dmd.paste(21-i, 4, clip3, true); //orMode
    free(clip3);
    dmd.refresh();
    DELAY2();
  }
  dmd.clearBuffer();
  dmd.refresh();
  DELAY();
  for (i = 0; i < 14; i++) {
    dmd.setDisplayArea(dmd.getArea(15-i, 4, 16+i, 11));
    dmd.paste(2, 4, clip1);
    dmd.paste(16, 4, clip2);
    dmd.flipHorizontal(dmd.getArea(15-i, 4, 16+i, 11));
    dmd.refresh();
    DELAY2();
  }
  for (i = 0; i < 14; i++) {
    dmd.scrollRight(dmd.getArea(2, 4, 15, 11));
    dmd.scrollLeft(dmd.getArea(16, 4, 29, 11));
    dmd.refresh();
    DELAY2();
  }
  for (i = 0; i < 14; i++) {
    dmd.setDisplayArea(dmd.getArea(15-i, 4, 16+i, 11));
    dmd.paste(2, 4, clip1);
    dmd.paste(16, 4, clip2);
    dmd.refresh();
    DELAY2();
  }
  free(clip2);
  free(clip1);
  clip1 = dmd.copy(dmd.getArea(2, 4, 29, 11));
  for (i = 0; i < 8; i++) {
    dmd.drawHorizontalLine(2, 11-i, 27, false);
    dmd.refresh();
    DELAY2();
  }
  dmd.paste(2, 4, clip1);
  dmd.flipVertical(dmd.getArea(2, 4, 29, 11));
  free(clip1);
  clip1 = dmd.copy(dmd.getArea(2, 4, 29, 11), true); //cutMode
  for (i = 0; i < 8; i++) {
    dmd.setDisplayArea(dmd.getArea(2, 4, 29, 4+i));
    dmd.paste(2, 4, clip1);
    dmd.refresh();
    DELAY2();
  }
  for (i = 0; i < 8; i++) {
    dmd.drawHorizontalLine(2, 4+i, 27, false);
    dmd.refresh();
    DELAY2();
  }
  dmd.paste(2, 4, clip1);
  dmd.flipVertical(dmd.getArea(2, 4, 29, 11));
  free(clip1);
  clip1 = dmd.copy(dmd.getArea(2, 4, 29, 11), true); //cutMode
  for (i = 0; i < 8; i++) {
    dmd.setDisplayArea(dmd.getArea(2, 11-i, 29, 11));
    dmd.paste(2, 4, clip1);
    dmd.refresh();
    DELAY2();
  }
  free(clip1);
  dmd.setDisplayAreaMax();
  
  for (i = 0; i < 15; i++) {
    dmd.inverseBuffer();
    dmd.refresh();
    DELAY2();
  }
  
  /*for (i = 0; i < 16; i++) {
    dmd.setBrightness(i);
    DELAY2();
  }
  for (i = 14; i < 16; i--) {
    dmd.setBrightness(i);
    DELAY2();
  }*/
  
  for (i = 0; i < 8; i++) {
    dmd.drawRect(dmd.getArea(i, i, 31-i, 15-i), false);
    dmd.refresh();
    DELAY2();
  }
  anime3(15, 7);
}

/*uint8_t anime1(unsigned char chr, uint8_t strw) {
  uint8_t i, chrw(dmd.drawChr(31, 0, chr));
  dmd.refresh();
  DELAY();
  for (i = 30; i > 31 - chrw; i--) {
    dmd.drawChr(i, 0, chr);
    dmd.refresh();
    DELAY();
  }
  for (i = 0; i < 32 - chrw; i++) {
    dmd.scrollLeft(dmd.getArea(0, 0, 31, 7));
    dmd.refresh();
    DELAY();
  }
  for (i = 0; i < 8; i++) {
    dmd.scrollDown(dmd.getArea(0, 0, chrw, 15));
    dmd.refresh();
    DELAY();
  }
  for (i = 0; i < 32 - chrw - strw; i++) {
    dmd.scrollRight(dmd.getArea(0, 8, 31 - strw, 15));
    dmd.refresh();
    DELAY();
  }
  return chrw;
}

void anime2(uint8_t chrw, uint8_t pos) {
  uint8_t i;
  for (i = 0; i < pos - chrw; i++) {
    dmd.scrollRight(dmd.getArea(32 - pos, 8, 31, 15));
    dmd.refresh();
    DELAY();
  }
  for (i = 0; i < 8; i++) {
    dmd.scrollUp(dmd.getArea(32 - chrw, 0, 31, 15));
    dmd.refresh();
    DELAY();
  }
  for (i = 0; i < 30 - pos; i++) {
    dmd.scrollLeft(dmd.getArea(2 + pos - chrw, 0, 31, 7));
    dmd.refresh();
    DELAY();
  }
}*/

void anime3(int16_t xc, int16_t yc) {
  uint16_t i;
  for (i = 1; i < 8; i++)
    anime4(xc, yc, i);
  for (i = 6; i > 1; i--)
    anime4(xc, yc, i);
}

void anime4(int16_t x, int16_t y, uint16_t r) {
  float rad(PI/2.0), stp(PI/8.0);
  for (uint8_t i = 0; i < 16; i++) {
    dmd.drawLine_R(x, y, r, rad);
    dmd.refresh();
    DELAY();
    dmd.drawLine_R(x, y, r, rad, false);
    rad -= stp;
  }
}

void anime5(unsigned char chr, bool &sd) {
  uint8_t t = (chr == 'l') ? 15 : 13;
  for (uint8_t i = 0; i < t; i++) {
    if (sd) {
      dmd.scrollDown(dmd.getArea(0,0, 4,15));
      dmd.scrollRight(dmd.getArea(5,8, 31,15));
      dmd.scrollLeft(dmd.getArea(5,0, 31,7));
      sd = !(dmd.getPixel(0, 14) || dmd.getPixel(1, 14));
    }
    else {
      dmd.scrollRight(dmd.getArea(0,8, 31,15));
      dmd.scrollLeft(dmd.getArea(0,0, 31,7));
      sd = dmd.getPixel(0, 6) || dmd.getPixel(0, 5);
    }
    dmd.drawChr(31-i, 0, chr);
    dmd.refresh();
    DELAY();
  }
}

void anime6(uint8_t t, bool &sd, bool &su) {
  su = dmd.getPixel(31, 14) || dmd.getPixel(31, 13);
  for (uint8_t i = 0; i < t; i++) {
    if (su) {
      dmd.scrollUp(dmd.getArea(27,0, 31,15));
      if (sd) {
        dmd.scrollDown(dmd.getArea(0,0, 4,15));
        dmd.scrollRight(dmd.getArea(5,8, 26,15));
        dmd.scrollLeft(dmd.getArea(5,0, 26,7));
        sd = !(dmd.getPixel(0, 14) || dmd.getPixel(1, 14));
      }
      else {
        dmd.scrollRight(dmd.getArea(0,8, 26,15));
        dmd.scrollLeft(dmd.getArea(0,0, 26,7));
        sd = dmd.getPixel(0, 6) || dmd.getPixel(0, 5);
      }
      su = !(dmd.getPixel(31, 0) || dmd.getPixel(30, 0) || dmd.getPixel(31, 2) && dmd.getPixel(29, 6));
    }
    else {
      if (sd) {
        dmd.scrollDown(dmd.getArea(0,0, 4,15));
        dmd.scrollRight(dmd.getArea(5,8, 31,15));
        dmd.scrollLeft(dmd.getArea(5,0, 31,7));
        sd = !(dmd.getPixel(0, 14) || dmd.getPixel(1, 14));
      }
      else {
        dmd.scrollRight(dmd.getArea(0,8, 31,15));
        dmd.scrollLeft(dmd.getArea(0,0, 31,7));
        sd = dmd.getPixel(0, 6) || dmd.getPixel(0, 5);
      }
      su = dmd.getPixel(31, 14) || dmd.getPixel(31, 13);
    }
    dmd.refresh();
    DELAY();
  }
}

void anime7(uint8_t t, uint8_t x, bool &su) {
  for (uint8_t i = 0; i < t; i++) {
    if (su) {
      dmd.scrollUp(dmd.getArea(27,0, 31,15));
      dmd.scrollRight(dmd.getArea(0,8, 26,15));
      dmd.scrollLeft(dmd.getArea(x,0, 26,7));
      su = !(dmd.getPixel(31, 0) || dmd.getPixel(30, 0) || dmd.getPixel(31, 2) && dmd.getPixel(29, 6));
    }
    else {
      dmd.scrollRight(dmd.getArea(0,8, 31,15));
      dmd.scrollLeft(dmd.getArea(x,0, 31,7));
      su = dmd.getPixel(31, 14) || dmd.getPixel(31, 13);
    }
    dmd.refresh();
    DELAY();
  }
}
