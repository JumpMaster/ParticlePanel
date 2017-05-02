#ifndef Display_h
#define Display_h

#include "application.h"

enum Modes {
    show_text,
    show_clock,
    show_rainbow
};

class Display
{
  public:
    Display();
    void begin();
    void setText(String text, bool repeat);
    void show();
    void copyColumn(int column, int newColumn);
    void clearColumn(int column);
    int writeText(String word, int start);
    void setColor(int color);
    void setBrightness(int brightness);
    void displayLetter(char letter, int *col, int charNum);
    int getBrightness();
    int getColor();
    void showRainbow();
    void setMode(enum Modes mode);
    void setUpdateTimer(unsigned int callbackTime);
    int getUpdateTimer();
    void kevtest(int a);
  private:
    void mainLoop();
    int masterCallbackTime = 50;
    Timer masterTimer;
    enum Modes mMode = show_clock;
    bool mRepeat;
    String mText;
    int mBrightness = 10;
    int mColor = 255;
    int8_t hPosition = 32;
    int8_t vPosition[100];
    
    const uint8_t ADDR_BRIGHTNESS = 10;
    const uint8_t ADDR_COLOR = 20;
    const uint8_t ADDR_UPDATE_INTERVAL = 30;
};
#endif
