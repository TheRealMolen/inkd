#include "EPaper.h"


template<typename T>
inline T clamp(const T val, const T lo, const T hi)
{
  if (val < lo)
    return lo;
  if (val > hi)
    return hi;
  return val;
}



EPaperDisplay paper;


void setup() {
  Serial.begin(115200);
  
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(BUTTON_D, INPUT_PULLUP);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, 0);
  
  paper.setScreenMode(magtag::Grey4);
  
  paper.clear();
  paper.redrawScreen();
}


float t = 0.f;


void loop() {
  if (!digitalRead(BUTTON_A))
    paper.setScreenMode(magtag::Grey4);
  else if (!digitalRead(BUTTON_B))
    paper.setScreenMode(magtag::Mono);

  if (!digitalRead(BUTTON_D))
  {
    paper.clear();
    paper.redrawScreen();
  }


  for (int i = 0; i <800; ++i)
  {
    float x = sin(t);
    float y = sin(2.1f * t);
    t += 0.005f;
    
    const float scale = 0.8f * float(magtag::ScreenWidth / 2);
    int sx = int(x * scale) + (magtag::ScreenWidth / 2);
    int sy = int(y * scale) + (magtag::ScreenHeight / 2);
    sx = clamp(sx, 0, magtag::ScreenWidth-1);
    sy = clamp(sy, 0, magtag::ScreenHeight-1);
  
    paper.plot(uint8_t(sx), uint16_t(sy), 3);
  }

/*
  for (int i = 11; i < 128-11; ++i)
  {
    paper.plot(uint8_t(i), uint16_t(i + 95), 3);
    paper.plot(uint8_t(128 - i), uint16_t(i + 95), 1);
  }
*/
  digitalWrite(LED_BUILTIN, 1);
  paper.redrawScreen();
  digitalWrite(LED_BUILTIN, 0);
  
  delay(3500);
}
