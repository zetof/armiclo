//Libraries
#include <LiquidCrystal.h>

#define MIDI_TIMING_CLOCK 0xF8

//Parameters
const int btnsPin  = A0;
const int potarPin  = A1;
int btnsVal  = 0;
int potarVal = 0;
int ledPin = 2;

//Objects
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);;

//Enums
enum {
  BUTTON_NONE,
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_SELECT,
};

void setup() {
  //Init Serial USB
  Serial.begin(250000);
  Serial.println(F("Initialize System"));
  pinMode(ledPin, OUTPUT);
  //Init LCD16x2 Shield
  lcd.begin(16, 2);
  lcd.print(F("Hello World "));
  delay(2000);
  lcd.clear();
  lcd.print(F("POTAR VAL: "));
}

void loop() {
  potarVal = analogRead(potarPin);
  if(potarVal < 2) potarVal = 0;
  if(potarVal > 1021) potarVal = 1023;
  int bpm = map(potarVal, 0, 1023, 56, 240);
  int blink = int(30000 / bpm);
  digitalWrite(ledPin, HIGH);
  delay(blink);
  digitalWrite(ledPin, LOW);
  delay(blink);
  lcd.setCursor(0, 1);
  lcd.print(String(bpm) + "    ");

  // btnListener(getBtnPressed());
  // Serial.write(MIDI_TIMING_CLOCK);
}

void btnListener(byte btnStatus) { /* function btnListener */
  //// Get button value when pressed
  lcd.setCursor(0, 1);
  switch (btnStatus) {
    case BUTTON_UP:
      lcd.print(F("UP     "));
      lcd.print(btnsVal);
      break;

    case BUTTON_DOWN:
      lcd.print(F("DOWN   "));
      lcd.print(btnsVal);
      lcd.print("   ");
      break;

    case BUTTON_LEFT:
      lcd.print(F("LEFT   "));
      lcd.print(btnsVal);
      lcd.print("   ");
      break;

    case BUTTON_RIGHT:
      lcd.print(F("RIGHT  "));
      lcd.print(btnsVal);
      lcd.print("   ");
      break;

    case BUTTON_SELECT:
      lcd.print(F("SELECT "));
      lcd.print(btnsVal);
      lcd.print("   ");
      break;

    default://case BUTTON_NONE:
      //lcd.print(F("       "));
      break;
  }
  delay(100);
}

byte getBtnPressed() { /* function getBtnPressed */
  //// Get button value when pressed
  btnsVal = analogRead(btnsPin);
  if (btnsVal < 50)
    return BUTTON_RIGHT;
  else if (btnsVal < 250)
    return BUTTON_UP;
  else if (btnsVal < 350)
    return BUTTON_DOWN;
  else if (btnsVal < 450)
    return BUTTON_LEFT;
  else if (btnsVal < 650)
    return BUTTON_SELECT;
  else
    return BUTTON_NONE;
}
