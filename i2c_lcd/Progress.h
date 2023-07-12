#ifndef __PROGRESS_H__
#define __PROGRESS_H__
#include <Adafruit_SSD1306.h>

class Progress {
  public:
    static const char _chars[];
    Progress(Adafruit_SSD1306);
    void Start();
    void Next();
    void End();
  private:
    Adafruit_SSD1306 _oled;
    int _current;
};

#endif