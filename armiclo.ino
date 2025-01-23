// Include libraries
#include <LiquidCrystal.h>

// Definition of MIDI messages
#define MIDI_TIMING_CLOCK 0xF8

// Definition of tempo LED pin
#define TEMPO_LED_PIN 2

// Definition of user input delay (en usecs)
#define USER_INPUT_DELAY 100000

// Analog input definition
const int btnsPin = A0;
const int potarPin = A1;

// Global LCD buttons and potentiometer values
int btnsVal;
int rotaryVal;

// Global tempo value
int bpm;

long now;

// Definition of LCD display
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Button values enum
enum {
  BUTTON_NONE,
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_LEFT,
  BUTTON_RIGHT,
  BUTTON_SELECT,
};

// Setup section
void setup() {
  
  //Init Serial USB communication for sending MIDI messages
  Serial.begin(250000);

  // Set tempo led digital pin to output mode
  pinMode(TEMPO_LED_PIN, OUTPUT);

  // Init LCD in 2x16 mode
  lcd.begin(16, 2);

  // Display splash title for 2s
  lcd.print(F("  ARDUINO MIDI  "));
  lcd.setCursor(2, 2);
  lcd.print(F("SIMPLE CLOCK"));
  delay(3000);
  lcd.clear();
}

// Looping section
void loop() {
  bpm = rotaryListener();
  int blink = int(30000 / bpm);
  digitalWrite(TEMPO_LED_PIN, HIGH);
  delay(blink);
  digitalWrite(TEMPO_LED_PIN, LOW);
  delay(blink);
  

  // btnListener(getBtnPressed());
  // Serial.write(MIDI_TIMING_CLOCK);
}

int rotaryListener(void) {
  int rotaryVal = analogRead(potarPin);
  if(rotaryVal < 3) rotaryVal = 0;
  if(rotaryVal > 1020) rotaryVal = 1023;
  int new_bpm = map(rotaryVal, 0, 1023, 56, 240);
  if(new_bpm != bpm) {
    lcd.setCursor(0, 1);
    lcd.print("BPM: " + String(new_bpm) + "  ");
  }
  return new_bpm;
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
