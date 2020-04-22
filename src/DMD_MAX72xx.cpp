/*
 * DMD_MAX72xx.cpp
 *
 * DMD_MAX72xx library implementation for basic controlling
 * MAX7219/MAX7221 dot matrix module, pixel manipulation,
 * and geometric transformation.
 *
 * Created by ZulNs, @Gorontalo, Indonesia, 15 October 2019
 * Released into the public domain.
 */

#include <Arduino.h>
#include "DMD_MAX72xx.h"
#include <SPI.h>

#define SPI_SPEED 10000000

#define CS_ON()  digitalWrite(_csPin, LOW)
#define CS_OFF() digitalWrite(_csPin, HIGH)

DMD_MAX72xx::~DMD_MAX72xx() {
  if (_useHwSpi) SPI.end();
  free(_displayBuffer);
}

void DMD_MAX72xx::begin(dev_type_t devType, uint8_t colCount, uint8_t rowCount, uint8_t csPin) {
  _devType  = devType;
  _colCount = colCount;
  _rowCount = rowCount;
  _csPin    = csPin;
  _useHwSpi = true;
  init();
}

void DMD_MAX72xx::begin(dev_type_t devType, uint8_t colCount, uint8_t rowCount, uint8_t csPin, uint8_t dataPin, uint8_t clkPin) {
  _devType  = devType;
  _colCount = colCount;
  _rowCount = rowCount;
  _csPin    = csPin;
  _dataPin  = dataPin;
  _clkPin   = clkPin;
  _useHwSpi = false;
  pinMode(_dataPin, OUTPUT);
  pinMode(_clkPin, OUTPUT);
  digitalWrite(clkPin, LOW);
  init();
}

void DMD_MAX72xx::init() {
  if (_colCount == 0) _colCount = 1;
  if (_rowCount == 0) _rowCount = 1;
  if (_rowCount*_colCount > 255) {
    _rowCount = 255/_colCount;
  }
  pinMode(_csPin, OUTPUT);
  CS_OFF();
  if (_useHwSpi) SPI.begin();
  _fontSpacing = 1;
  setFont(_stdFont);
  setDisplayAreaMax();
  _displayBuffer = (uint8_t *)calloc(_rowCount*_colCount*8, sizeof(uint8_t));
  writeAllSegments(REG_SHUTDOWN, 1);
  writeAllSegments(REG_DECODE_MODE, 0);
  writeAllSegments(REG_SCAN_LIMIT, 7);
  writeAllSegments(REG_DISPLAY_TEST, 0);
  setBrightness(0);
  refresh();
}

void DMD_MAX72xx::writeReg(uint8_t reg, uint8_t data) {
  if (_useHwSpi) {
    SPI.transfer(reg);
    SPI.transfer(data);
  }
  else {
    shiftOut(_dataPin, _clkPin, MSBFIRST, reg);
    shiftOut(_dataPin, _clkPin, MSBFIRST, data);
  }
}

void DMD_MAX72xx::writeEachSegment(uint8_t reg, uint8_t *data) {
  if (_useHwSpi) SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  CS_ON();
  for (uint8_t i = 0; i < _rowCount*_colCount; i++)
    writeReg(reg, data[i]);
  CS_OFF();
  if (_useHwSpi) SPI.endTransaction();
}

void DMD_MAX72xx::writeAllSegments(uint8_t reg, uint8_t data) {
  if (_useHwSpi) SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  CS_ON();
  for (uint8_t i = 0; i < _rowCount*_colCount; i++)
    writeReg(reg, data);
  CS_OFF();
  if (_useHwSpi) SPI.endTransaction();
}

void DMD_MAX72xx::writeSegment(uint8_t block, uint8_t reg, uint8_t data) {
  if (_useHwSpi) SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  CS_ON();
  for (uint8_t i = 0; i < _rowCount*_colCount; i++)
    if (i == block) writeReg(reg, data);
    else            writeReg(0, 0);
  CS_OFF();
  if (_useHwSpi) SPI.endTransaction();
}

DMD_MAX72xx::area_t DMD_MAX72xx::getArea(point_t p0, point_t p1) {
  area_t a;
  ensureOrder(p0.x, p1.x);
  ensureOrder(p0.y, p1.y);
  a.left   = p0.x;
  a.top    = p0.y;
  a.right  = p1.x;
  a.bottom = p1.y;
  return a;
}

void DMD_MAX72xx::refresh(void) {
  uint8_t r, i, b, n, s = _colCount*_rowCount;
  if (_useHwSpi) SPI.beginTransaction(SPISettings(SPI_SPEED, MSBFIRST, SPI_MODE0));
  for (r = 1; r <= 8; r++) {
    CS_ON();
    for (i = 0; i < s; i++) {
      switch (_devType) {
        case TYPE_TBLR:
          n = _displayBuffer[i*8 + r - 1];
          break;
        case TYPE_TBRL:
          n = _displayBuffer[i*8 + 8 - r];
          break;
        case TYPE_BTLR:
          for (b = 0; b < 8; b++)
            bitWrite(n, b, bitRead(_displayBuffer[i*8 + r - 1], 7 - b));
          break;
        case TYPE_BTRL:
          for (b = 0; b < 8; b++)
            bitWrite(n, b, bitRead(_displayBuffer[i*8 + 8 - r], 7 - b));
          break;
        case TYPE_LRTB:
          for (b = 0; b < 8; b++)
            bitWrite(n, b, bitRead(_displayBuffer[i*8 + b], r - 1));
          break;
        case TYPE_LRBT:
          for (b = 0; b < 8; b++)
            bitWrite(n, b, bitRead(_displayBuffer[i*8 + b], 8 - r));
          break;
        case TYPE_RLTB:
          for (b = 0; b < 8; b++)
            bitWrite(n, b, bitRead(_displayBuffer[i*8 + 7 - b], r - 1));
          break;
        case TYPE_RLBT:
          for (b = 0; b < 8; b++)
            bitWrite(n, b, bitRead(_displayBuffer[i*8 + 7 - b], 8 - r));
      }
      writeReg(r, n);
    }
    CS_OFF();
  }
  if (_useHwSpi) SPI.endTransaction();
}

bool DMD_MAX72xx::getPixel(int16_t x, int16_t y) {
  return (x < _displayArea.left || _displayArea.right < x || y < _displayArea.top || _displayArea.bottom < y) ?
    false : bitRead(_displayBuffer[(y/8)*(_colCount*8) + x], y%8);
}

void DMD_MAX72xx::putPixel(int16_t x, int16_t y, bool dot) {
  if (_displayArea.left <= x && x <= _displayArea.right && _displayArea.top <= y && y <= _displayArea.bottom)
    bitWrite(_displayBuffer[(y/8)*(_colCount*8) + x], y%8, dot);
}

uint8_t *DMD_MAX72xx::copy(area_t a, bool cutMode) {
  int16_t x, y;
  uint16_t dx, dy;
  uint8_t iy, b;
  dx = 1 + a.right - a.left;
  dy = 1 + a.bottom - a.top;
  uint8_t *buf = (uint8_t *)calloc(4 + (1 + (dy - 1)/8)*dx, sizeof(uint8_t));
  buf[0] = dx; buf[1] = dx >> 8;
  buf[2] = dy; buf[3] = dy >> 8;
  for (y = a.top; y <= a.bottom; y++) {
    iy = (y - a.top)/8;
    b = (y - a.top)%8;
    for (x = a.left; x <= a.right; x++) {
      bitWrite(buf[4 + iy*dx + x - a.left], b, getPixel(x, y));
      if (cutMode) putPixel(x, y, false);
    }
  }
  return buf;
}

void DMD_MAX72xx::paste(int16_t x, int16_t y, uint8_t *src, bool orMode) {
  int16_t cx, cy;
  uint16_t dx, dy;
  uint8_t iy, b;
  bool dot;
  dx = src[0] + (src[1] << 8);
  dy = src[2] + (src[3] << 8);
  for (cy = 0; cy < dy; cy++) {
    iy = cy/8;
    b = cy%8;
    for (cx = 0; cx < dx; cx++) {
      dot = bitRead(src[4 + iy*dx + cx], b);
      if (!orMode)
        putPixel(x + cx, y + cy, dot);
      else if (dot)
        putPixel(x + cx, y + cy, true);
    }
  }
}

void DMD_MAX72xx::pasteRotate(int16_t x, int16_t y, uint8_t *src, bool isCW) {
  int16_t cx, cy;
  uint16_t dx, dy;
  uint8_t iy, b;
  bool dot;
  dy = src[0] + (src[1] << 8);
  dx = src[2] + (src[3] << 8);
  for (cx = 0; cx < dx; cx++) {
    if (isCW) {
      iy = (dx - cx - 1)/8;
      b = (dx - cx - 1)%8;
    }
    else {
      iy = cx/8;
      b = cx%8;
    }
    for (cy = 0; cy < dy; cy++) {
      dot = bitRead(src[(isCW) ? iy*dy + cy + 4: iy*dy + dy - cy + 3], b);
      putPixel(x + cx, y + cy, dot);
    }
  }
}

void DMD_MAX72xx::drawVerticalLine(int16_t x, int16_t y, int16_t d, bool dot) {
  if (d == 0) return;
  while (d != 0) {
    putPixel(x, y, dot);
    if (d > 0) { y++; d--; }
    else       { y--; d++; }
  }
  putPixel(x, y, dot);
}

void DMD_MAX72xx::drawHorizontalLine(int16_t x, int16_t y, int16_t d, bool dot) {
  if (d == 0) return;
  while (d != 0) {
    putPixel(x, y, dot);
    if (d > 0) { x++; d--; }
    else       { x--; d++; }
  }
  putPixel(x, y, dot);
}

void DMD_MAX72xx::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool dot) {
  int16_t dx, dy, p, x, y;
  dx = (x1 > x0) ? x1 - x0 : x0 - x1;
  dy = (y1 > y0) ? y1 - y0 : y0 - y1;
  x = x0; y = y0;
  if (dx > dy) {
    p = 2*dy - dx;
    while (x != x1) {
      putPixel(x, y, dot);
      if (p >= 0) {
        p += 2*dy - 2*dx;
        y += (y1 > y0) ? 1 : - 1;
      }
      else p += 2*dy;
      x += (x1 > x0) ? 1 : -1;
    }
  }
  else {
    p = 2*dx - dy;
    while (y != y1) {
      putPixel(x, y, dot);
      if (p >= 0) {
        p += 2*dx - 2*dy;
        x += (x1 > x0) ? 1 : -1;
      }
      else p += 2*dx;
      y += (y1 > y0) ? 1 : -1;
    }
  }
  putPixel(x1, y1, dot);
}

void DMD_MAX72xx::drawLine_R(int16_t x, int16_t y, uint16_t r, float rad, bool dot) {
  int16_t x2, y2;
  x2 = round(r*cos(rad));
  y2 = round(r*sin(rad));
  drawLine(x, y, x + x2, y - y2, dot);
}

void DMD_MAX72xx::drawRect(area_t a, bool dot) {
  int16_t dx, dy;
  dx = a.right - a.left;
  dy = a.bottom - a.top;
  drawHorizontalLine(a.left,  a.top,     dx, dot);
  drawVerticalLine  (a.right, a.top,     dy, dot);
  drawHorizontalLine(a.right, a.bottom, -dx, dot);
  drawVerticalLine  (a.left,  a.bottom, -dy, dot);
}

void DMD_MAX72xx::drawFilledRect(area_t a, bool fill) {
  int16_t y, dx;
  dx = a.right - a.left;
  for (y = a.top; y <= a.bottom; y++)
    drawHorizontalLine(a.left, y, dx, fill);
}

void DMD_MAX72xx::drawCircle(int16_t cx, int16_t cy, uint16_t r, bool dot) {
  int16_t x(r), y(0), x0(x);
  while(x > 0) {
    putPixel(cx-x, cy-y, dot);
    putPixel(cx+y, cy-x, dot);
    putPixel(cx+x, cy+y, dot);
    putPixel(cx-y++, cy+x, dot);
    x = round(sqrt(r*r - y*y));
    if (x0-x > 1) {
      x = x0 - 1;
      y = round(sqrt(r*r - x*x));
    }
    x0 = x;
  }
}

void DMD_MAX72xx::drawEllipse(int16_t cx, int16_t cy, uint16_t rx, uint16_t ry, bool dot) {
  int16_t x(rx), y(1), x0(x);
  putPixel(cx-rx, cy,    dot);
  putPixel(cx,    cy-ry, dot);
  putPixel(cx+rx, cy,    dot);
  putPixel(cx,    cy+ry, dot);
  while(x > 0) {
    putPixel(cx-x, cy-y,   dot);
    putPixel(cx+x, cy-y,   dot);
    putPixel(cx+x, cy+y,   dot);
    putPixel(cx-x, cy+y++, dot);
    x = round(sqrt(rx*rx * (ry*ry - y*y)/(float)(ry*ry)));
    if (x0-x > 1) {
      x = x0 - 1;
      y = round(sqrt(ry*ry * (rx*rx - x*x)/(float)(rx*rx)));
    }
    x0 = x;
  }
}

void DMD_MAX72xx::flipVertical(area_t a) {
  int16_t x, y, hy, cy;
  bool dot;
  hy = (a.top + a.bottom)/2;
  for (x = a.left; x <= a.right; x++) {
    cy = 0;
    for (y = a.top; y <= hy; y++) {
      dot = getPixel(x, y);
      putPixel(x, y, getPixel(x, a.bottom - cy));
      putPixel(x, a.bottom - cy, dot);
      cy++;
    }
  }
}

void DMD_MAX72xx::flipHorizontal(area_t a) {
  int16_t x, y, hx, cx;
  bool dot;
  hx = (a.left + a.right)/2;
  for (y = a.top; y <= a.bottom; y++) {
    cx = 0;
    for (x = a.left; x <= hx; x++) {
      dot = getPixel(x, y);
      putPixel(x, y, getPixel(a.right - cx, y));
      putPixel(a.right - cx, y, dot);
      cx++;
    }
  }
}

void DMD_MAX72xx::scrollLeft(area_t a, bool rotated) {
  int16_t x, y;
  uint8_t *buf = copy(getArea(a.left, a.top, a.left, a.bottom));
  for (y = a.top; y <= a.bottom; y++) {
    for (x = a.left; x < a.right; x++)
      putPixel(x, y, getPixel(x + 1, y));
    if (!rotated) putPixel(x, y, false);
  }
  if (rotated) paste(a.right, a.top, buf);
  free(buf);
}

void DMD_MAX72xx::scrollRight(area_t a, bool rotated) {
  int16_t x, y;
  uint8_t *buf = copy(getArea(a.right, a.top, a.right, a.bottom));
  for (y = a.top; y <= a.bottom; y++) {
    for (x = a.right; x > a.left; x--)
      putPixel(x, y, getPixel(x - 1, y));
    if (!rotated) putPixel(x, y, false);
  }
  if (rotated) paste(a.left, a.top, buf);
  free(buf);
}

void DMD_MAX72xx::scrollUp(area_t a, bool rotated) {
  int16_t x, y;
  uint8_t *buf = copy(getArea(a.left, a.top, a.right, a.top));
  for (x = a.left; x <= a.right; x++) {
    for (y = a.top; y < a.bottom; y++)
      putPixel(x, y, getPixel(x, y + 1));
    if (!rotated) putPixel(x, y, false);
  }
  if (rotated) paste(a.left, a.bottom, buf);
  free(buf);
}

void DMD_MAX72xx::scrollDown(area_t a, bool rotated) {
  int16_t x, y;
  uint8_t *buf = copy(getArea(a.left, a.bottom, a.right, a.bottom));
  for (x = a.left; x <= a.right; x++) {
    for (y = a.bottom; y > a.top; y--)
      putPixel(x, y, getPixel(x, y - 1));
    if (!rotated) putPixel(x, y, false);
  }
  if (rotated) paste(a.left, a.top, buf);
  free(buf);
}

void DMD_MAX72xx::scrollTopLeft(area_t a) {
  int16_t x, y;
  for (x = a.left; x < a.right; x++) {
    for (y = a.top; y < a.bottom; y++)
      putPixel(x, y, getPixel(x + 1, y + 1));
    putPixel(x, y, false);
  }
  for (y = a.top; y <= a.bottom; y++)
    putPixel(x, y, false);
}

void DMD_MAX72xx::scrollRotateTopLeft(area_t a) {
  uint8_t *bx = copy(getArea(a.left + 1, a.top, a.right, a.top));
  uint8_t *by = copy(getArea(a.left, a.top + 1, a.left, a.bottom));
  bool dot = getPixel(a.left, a.top);
  scrollTopLeft(a);
  putPixel(a.right, a.bottom, dot);
  paste(a.left, a.bottom, bx);
  paste(a.right, a.top, by);
  free(by);
  free(bx);
}

void DMD_MAX72xx::scrollBottomLeft(area_t a) {
  int16_t x, y;
  for (x = a.left; x < a.right; x++) {
    putPixel(x, a.top, false);
    for (y = a.top; y < a.bottom; y++)
      putPixel(x, y + 1, getPixel(x + 1, y));
  }
  for (y = a.top; y <= a.bottom; y++)
    putPixel(x, y, false);
}

void DMD_MAX72xx::scrollRotateBottomLeft(area_t a) {
  uint8_t *bx = copy(getArea(a.left + 1, a.bottom, a.right, a.bottom));
  uint8_t *by = copy(getArea(a.left, a.top, a.left, a.bottom - 1));
  bool dot = getPixel(a.left, a.bottom);
  scrollBottomLeft(a);
  putPixel(a.right, a.top, dot);
  paste(a.left, a.top, bx);
  paste(a.right, a.top + 1, by);
  free(by);
  free(bx);
}

void DMD_MAX72xx::scrollTopRight(area_t a) {
  int16_t x, y;
  for (x = a.right; x > a.left; x--) {
    for (y = a.top; y < a.bottom; y++)
      putPixel(x, y, getPixel(x - 1, y + 1));
    putPixel(x, y, false);
  }
  for (y = a.top; y <= a.bottom; y++)
    putPixel(x, y, false);
}

void DMD_MAX72xx::scrollRotateTopRight(area_t a) {
  uint8_t *bx = copy(getArea(a.left, a.top, a.right - 1, a.top));
  uint8_t *by = copy(getArea(a.right, a.top + 1, a.right, a.bottom));
  bool dot = getPixel(a.right, a.top);
  scrollTopRight(a);
  putPixel(a.left, a.bottom, dot);
  paste(a.left + 1, a.bottom, bx);
  paste(a.left, a.top, by);
  free(by);
  free(bx);
}

void DMD_MAX72xx::scrollBottomRight(area_t a) {
  int16_t x, y;
  for (x = a.right; x > a.left; x--) {
    putPixel(x, a.top, false);
    for (y = a.top; y < a.bottom; y++)
      putPixel(x, y + 1, getPixel(x - 1, y));
  }
  for (y = a.top; y <= a.bottom; y++)
    putPixel(x, y, false);
}

void DMD_MAX72xx::scrollRotateBottomRight(area_t a) {
  uint8_t *bx = copy(getArea(a.left, a.bottom, a.right - 1, a.bottom));
  uint8_t *by = copy(getArea(a.right, a.top, a.right, a.bottom - 1));
  bool dot = getPixel(a.right, a.bottom);
  scrollBottomRight(a);
  putPixel(a.left, a.top, dot);
  paste(a.left + 1, a.top, bx);
  paste(a.left, a.top + 1, by);
  free(by);
  free(bx);
}
