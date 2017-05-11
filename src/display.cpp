#include "display.h"
#include "letters.h"
#include "neopixel.h"

const int PIXEL_COUNT = 256;
const int PIXEL_PIN = D2;
const int PIXEL_TYPE = WS2812B;

Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);

Display::Display() : masterTimer(masterCallbackTime, &Display::mainLoop, *this, false) { }

void Display::begin() {
  int brightness;
  EEPROM.get(ADDR_BRIGHTNESS, brightness);
  setBrightness(brightness);
  int color;
  EEPROM.get(ADDR_COLOR, color);
  setColor(color);
  int updateInterval;
  EEPROM.get(ADDR_UPDATE_INTERVAL, updateInterval);
  setUpdateTimer(updateInterval);

  Particle.variable("color", mColor);
  Particle.variable("brightness", mBrightness);
  Particle.variable("text", mText);
  Particle.variable("scrollspeed", masterCallbackTime);

  strip.begin();
  strip.setBrightness(mBrightness);
  strip.show();
  masterTimer.start();
}

void Display::show() {
  strip.show();
}

void Display::displayLetter(char letter, int *col, int charNum) {
  int l = letter - '!';
  int num = *col * 8;

  for (int c = 0; c < letterSize[l]; c++) {

    if (*col >= 0 && *col < 32) {
      bool up = *col % 2 == 1;

      if ((vPosition[charNum] < 0 && !up) || (vPosition[charNum] > 0 && up))
        for (int i = 7; i > 0; i--)
          strip.setPixelColor(num+i, strip.getPixelColor(num+i-1) > 0 ? mColor : 0);

      if ((vPosition[charNum] < 0 && up) || (vPosition[charNum] > 0 && !up))
        for (int i = 0; i < 7; i++)
          strip.setPixelColor(num+i, strip.getPixelColor(num+i+1) > 0 ? mColor : 0);

      if (!up) {
        for (int r = 0; r < 8; r++) {
          int t = vPosition[charNum] + r;
          if (t >= 0 && t < 8)
            strip.setPixelColor(num+vPosition[charNum], letters[l][r][c] == 1 ? mColor : 0);
          num++;
        }
      } else {
        for (int r = 7; r >= 0; r--) {
          int t = vPosition[charNum] + r;
          if (t >= 0 && t < 8)
            strip.setPixelColor(num-vPosition[charNum], letters[l][r][c] == 1 ? mColor : 0);
          num++;
        }
      }
    }
    else
      num += 8;
    (*col)++;
  }
}

int Display::writeText(String word, int start) {
  int col = start;
  for (int i = 0; i < word.length(); i++) {
    if (word.charAt(i) == ' ')
      clearColumn(col++);
    else
      displayLetter(word.charAt(i), &col, i);
    clearColumn(col++); // space between letters
    if (col >= 32)
    return col;
  }
  return col;
}

void Display::clearColumn(int column) {
  for (int i = 0; i < 8; i++)
    strip.setPixelColor((column*8)+i, 0);
}

void Display::copyColumn(int column, int newColumn) {
  bool reverse = (column % 2) != (newColumn % 2);

  for (int i = 0; i < 8; i++) {
    int num = (column * 8) + i;
    int newNum = reverse ? ((newColumn * 8)+7) - i : (newColumn * 8) + i;
    strip.setPixelColor(newNum, strip.getPixelColor(num) == 0 ? 0 : mColor);
  }
}

void Display::setColor(int color) {
  if (color > 16777215)
    mColor = 16777215;
  else if (color < 0)
    mColor = 0;
  else
    mColor = color;

  EEPROM.put(ADDR_COLOR, color);

  for (int i = 0; i < 256; i++)
    strip.setPixelColor(i, (strip.getPixelColor(i) == 0 ? 0 : mColor));
  strip.show();
}

void Display::setBrightness(int brightness) {
  if (brightness < 0)
    mBrightness = 0;
  else if (brightness > 255)
    mBrightness = 255;
  else
    mBrightness = brightness;

  strip.setBrightness(brightness);
  writeText(mText, hPosition);
  strip.show();

  EEPROM.put(ADDR_BRIGHTNESS, brightness);
}

int Display::getBrightness() {
  return mBrightness;
}

int Display::getColor() {
  return mColor;
}

void Display::showRainbow() {
  int r = 255;
  int g = 0;
  int b = 0;
  int stage = 0;
  for (int i = 0; i < 256; i++) {
    if (stage == 0) {
      g+=5;
      if (g == 255)
      stage++;
    } else if (stage == 1) {
      r-=5;
      if (r == 0)
      stage++;
    } else if (stage == 2) {
      b+=5;
      if (b == 255)
      stage++;
    } else if (stage == 3) {
      g-=5;
      if (g == 0)
      stage++;
    } else if (stage == 4) {
      r+=5;
    }
    strip.setPixelColor(i, r, g, b);
  }
  strip.show();
}

void Display::setMode(enum Modes mode) {
  mMode = mode;
  hPosition = 32;
}

void Display::setUpdateTimer(unsigned int callbackTime) {
  if (callbackTime <= 0)
    masterCallbackTime = 1;
  else if (callbackTime > 1000)
    masterCallbackTime = 1000;
  else
    masterCallbackTime = callbackTime;

  EEPROM.put(ADDR_UPDATE_INTERVAL, masterCallbackTime);
  masterTimer.changePeriod(masterCallbackTime);
}

void Display::setText(String text, bool repeat) {
  mText = text;
  mRepeat = repeat;
}

int Display::getUpdateTimer() {
  return masterCallbackTime;
}

void Display::kevtest(int a) {
  int intBrightness = sizeof(mBrightness);
  int intColor = sizeof(mColor);
  int intUpdateTime = sizeof(masterCallbackTime);
  Particle.publish("LOG", String::format("Color = %d : Brightness = %d : UpdateTime = %d", intColor, intBrightness, intUpdateTime), PRIVATE);
}

void Display::mainLoop() {
  if (mMode == show_text) {
    for (int8_t i = 0; i < hPosition; i++)
    copyColumn(i+1, i);

    int output = writeText(mText, hPosition);
    show();
    hPosition--;

    for (int i = 0; i < 100; i++) {
      if (vPosition[i] > 0)
        vPosition[i]--;
      else if (vPosition[i] < 0)
        vPosition[i]++;
    }

    if (output < 0 && mRepeat)
      hPosition = 32;
    else if (output < 0 && !mRepeat)
      setMode(show_clock);
  } else if (mMode == show_clock) {
      String newText = Time.format(Time.now(), "%H:%M");

    for (int i = 0; i < 100; i++) {
      if (vPosition[i] > 0)
        vPosition[i]--;
      else if (vPosition[i] < 0)
        vPosition[i]++;
    }

    if (hPosition != 2)
      hPosition--;
    else {
      bool b;
      for (int i = 0; i < 5; i++) {
        if (newText.charAt(i) != mText.charAt(i)) {
          if (b) {
            vPosition[i] = -8;
            b = false;
          } else {
            vPosition[i] = 8;
            b = true;
          }
        }
      }
    }

    mText = newText;
    writeText(mText, hPosition);
    show();
  } else if (mMode == show_rainbow) {
    showRainbow();
  }
}
