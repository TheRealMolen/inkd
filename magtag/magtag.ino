#include "EPaper.h"


EPaperDisplay paper;


void setup() {
  Serial.begin(115200);
  
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  pinMode(BUTTON_D, INPUT_PULLUP);
}


void loop() {
    if (!digitalRead(BUTTON_A))
      paper.setScreenMode(magtag::Grey4);
    else if (!digitalRead(BUTTON_B))
      paper.setScreenMode(magtag::Mono);
    
    paper.clear();
    paper.redrawScreen();
    delay(2000);

    paper.drawBlart();
    paper.redrawScreen();
    delay(2000);
}
