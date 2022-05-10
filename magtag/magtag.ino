#include "EPaper.h"


EPaperDisplay paper;


void setup() {
  Serial.begin(115200);
}


void loop() {
    paper.clear();
    paper.redrawScreen();
    delay(2000);

    paper.drawBlart();
    paper.redrawScreen();
    delay(2000);
}