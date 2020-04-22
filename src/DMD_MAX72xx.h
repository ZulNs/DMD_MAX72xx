/*
 * DMD_MAX72xx.h
 *
 * Arduino Library for MAX7219/MAX7221 dot matrix display module as pixel addressable display.
 * Supports multi columns and rows (like Freetronics DMD).
 * Supported platforms: AVR, ESP8266, STM32, and others.
 *
 * Created by ZulNs, @Gorontalo, Indonesia, 15 October 2019
 * Released into the public domain.
 */

#ifndef DMD_MAX72xx_h
#define DMD_MAX72xx_h

#define REG_DECODE_MODE  0x09
#define REG_INTENSITY    0x0A
#define REG_SCAN_LIMIT   0x0B
#define REG_SHUTDOWN     0x0C
#define REG_DISPLAY_TEST 0x0F

#include <Arduino.h>

extern const uint8_t PROGMEM _stdFont[];

class DMD_MAX72xx {
public:
  enum dev_type_t {
    TYPE_TBLR,
    TYPE_TBRL,
    TYPE_BTLR,
    TYPE_BTRL,
    TYPE_LRTB,
    TYPE_LRBT,
    TYPE_RLTB,
    TYPE_RLBT
  };
  
  typedef struct {
    int16_t x;
    int16_t y;
  } point_t;
  
  typedef struct {
    int16_t left;
    int16_t top;
    int16_t right;
    int16_t bottom;
  } area_t;
  
  typedef const uint8_t font_t;
  
  typedef struct {
    uint8_t firstAscii;
    uint8_t lastAscii;
    uint8_t width;
    uint8_t height;
    font_t *data;
  } font_info_t;
  
  DMD_MAX72xx() {}
  
  DMD_MAX72xx(dev_type_t devType, uint8_t colCount, uint8_t rowCount, uint8_t csPin) {
    begin(devType, colCount, rowCount, csPin);
  }
  
  DMD_MAX72xx(dev_type_t devType, uint8_t colCount, uint8_t rowCount, uint8_t csPin, uint8_t dataPin, uint8_t clkPin) {
    begin(devType, colCount, rowCount, csPin, dataPin, clkPin);
  }
  
  ~DMD_MAX72xx();
  
  void begin(dev_type_t devType, uint8_t colCount, uint8_t rowCount, uint8_t csPin);
  void begin(dev_type_t devType, uint8_t colCount, uint8_t rowCount, uint8_t csPin, uint8_t dataPin, uint8_t clkPin);
  void init(void);
  
  void writeReg(uint8_t reg, uint8_t data);
  void writeEachSegment(uint8_t reg, uint8_t *data);
  void writeAllSegments(uint8_t reg, uint8_t data);
  void writeSegment(uint8_t block, uint8_t reg, uint8_t data);
  
  inline uint8_t getColCount(void) { return _colCount; }
  inline uint8_t getRowCount(void) { return _rowCount; }
  inline area_t getDisplayArea(void) { return _displayArea; }
  inline uint8_t getFontSpacing(void) { return _fontSpacing; }
  inline font_info_t getFontInfo(void) { return _fontInfo; }
  inline uint8_t getBrightness(void) { return _brightness; }
  
  inline void setDisplayArea(area_t a) { _displayArea = a; }
  inline void setDisplayAreaMax(void) { _displayArea = getArea(0, 0, (_colCount << 3) - 1, (_rowCount << 3) - 1); }
  inline void setFontSpacing(uint8_t fspc) { _fontSpacing = fspc; }
  
  inline void setBrightness(uint8_t br) {
    _brightness = br & 15;
    writeAllSegments(REG_INTENSITY, _brightness);
  }
  
  inline point_t getPoint(int16_t x, int16_t y) {
    point_t p; p.x = x; p.y = y; return p;
  }
  
  area_t getArea(point_t p0, point_t p1);
  
  inline area_t getArea(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    return getArea(getPoint(x0, y0), getPoint(x1, y1));
  }
  
  void refresh(void);
  
  bool setFont(font_t *f);
  font_t *getChrPtr(unsigned char chr);
  uint8_t getChrWidth(unsigned char chr);
  uint8_t *copyChr(unsigned char chr);
  uint8_t drawChr(int16_t x, int16_t y, unsigned char chr);
  uint16_t getStrWidth(unsigned char *str);
  
  inline uint16_t getStrWidth(const char *str) {
    return getStrWidth((unsigned char *)str);
  }
  
  inline uint16_t getStrWidth(const __FlashStringHelper *fstr) {
    return getStrWidth_P((const char *)fstr);
  }
  
  uint16_t getStrWidth_P(const char *str);
  void drawStr(int16_t x, int16_t y, unsigned char *str);
  
  void inline drawStr(int16_t x, int16_t y, const char *str) {
    return drawStr(x, y, (unsigned char *)str);
  }
  
  inline void drawStr(int16_t x, int16_t y, const __FlashStringHelper *fstr) {
    return drawStr_P(x, y, (const char *)fstr);
  }
  
  void drawStr_P(int16_t x, int16_t y, const char *pstr);
  void drawFontSpacing(int16_t x, int16_t y);
  
  inline bool marquee(area_t a, unsigned char *str, uint16_t delayTime, uint8_t times = 1, bool (*callback)(void) = nullptr) {
    return marquee(a, str, nullptr, delayTime, times, callback);
  }
  
  inline bool marquee(area_t a, const char *str, uint16_t delayTime, uint8_t times = 1, bool (*callback)(void) = nullptr) {
    return marquee(a, (unsigned char *)str, nullptr, delayTime, times, callback);
  }
  
  inline bool marquee(area_t a, const __FlashStringHelper *fstr, uint16_t delayTime, uint16_t times = 1, bool (*callback)(void) = nullptr) {
    return marquee(a, nullptr, (const char *)fstr, delayTime, times, callback);
  }
  
  inline bool marquee_P(area_t a, const char *pstr, uint16_t delayTime, uint16_t times = 1, bool (*callback)(void) =  nullptr) {
    return marquee(a, nullptr, pstr, delayTime, times, callback);
  }
  
  void clearDisplay(void) {
    for (uint8_t i = 1; i <= 8; i++) writeAllSegments(i, 0);
  }
  
  inline void clearArea(area_t a, bool fill = false) {
    drawFilledRect(a, fill);
  }
  
  inline void clearBuffer(bool fill = false) {
    memset(_displayBuffer, fill ? 0xFF : 0, _rowCount*(_colCount << 3));
  }
  
  void inverseBuffer(void) {
    uint16_t s = _rowCount*(_colCount << 3);
    for (uint16_t i = 0; i < s; i++) _displayBuffer[i] = ~_displayBuffer[i];
  }
  
  bool getPixel(int16_t x, int16_t y);
  void putPixel(int16_t x, int16_t y, bool dot = true);
  uint8_t *copy(area_t a, bool cutMode = false);
  void paste(int16_t x, int16_t y, uint8_t *src, bool orMode = false);
  void pasteRotate(int16_t x, int16_t y, uint8_t *src, bool isCW = true);
  void drawVerticalLine(int16_t x, int16_t y, int16_t d, bool dot = true);
  void drawHorizontalLine(int16_t x, int16_t y, int16_t d, bool dot = true);
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, bool dot = true);
  void drawLine_R(int16_t x, int16_t y, uint16_t r, float rad, bool dot = true);
  void drawRect(area_t a, bool dot = true);
  void drawFilledRect(area_t a, bool fill = true);
  void drawCircle(int16_t cx, int16_t cy, uint16_t r, bool dot = true);
  void drawEllipse(int16_t cx, int16_t cy, uint16_t rx, uint16_t ry, bool dot = true);
  void flipVertical(area_t a);
  void flipHorizontal(area_t a);
  void scrollLeft(area_t a, bool rotated = false);
  void scrollRight(area_t a, bool rotated = false);
  void scrollUp(area_t a, bool rotated = false);
  void scrollDown(area_t a, bool rotated = false);
  void scrollTopLeft(area_t a);
  void scrollRotateTopLeft(area_t a);
  void scrollBottomLeft(area_t a);
  void scrollRotateBottomLeft(area_t a);
  void scrollTopRight(area_t a);
  void scrollRotateTopRight(area_t a);
  void scrollBottomRight(area_t a);
  void scrollRotateBottomRight(area_t a);

private:
  dev_type_t  _devType;
  uint8_t     _csPin,
              _dataPin,
              _clkPin,
              _colCount,
              _rowCount,
              _fontSpacing,
              _brightness;
  bool        _useHwSpi;
  font_info_t _fontInfo;
  area_t      _displayArea;
  uint8_t    *_displayBuffer;
  
  template<typename T> static inline void swap(T &a, T &b) {
    T tmp(a); a = b; b = tmp;
  }

  template<typename T> static inline void ensureOrder(T &a, T &b) {
    if (b < a) swap(a, b);
  }

  bool marquee(area_t a, unsigned char *str, const char *pstr, uint16_t delayTime, uint8_t times = 1, bool (*callback)(void) = nullptr);
};

#endif /*DMD_MAX72xx_h*/