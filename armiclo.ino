// Include libraries
#include <LiquidCrystal.h>

// Definition of MIDI messages
#define MIDI_TIMING_CLOCK 0xF8
#define MIDI_START 0xFA
#define MIDI_CONTINUE 0xFB
#define MIDI_STOP 0xFC

// Definition of buttons analog thresholds
#define BUTTON_RIGHT 50
#define BUTTON_UP 250
#define BUTTON_DOWN 350
#define BUTTON_LEFT 550
#define BUTTON_SELECT 750
#define BUTTON_NONE 1023

// Definition of tempo LED pin
#define TEMPO_LED_PIN 2

// Definition of LCD backlighting pin
#define BACKLIGHTING_PIN 10

// Definition of LED blink time (en usecs)
#define LED_BLINK_TIME 50000

// Definition of LCD backlight ON time (en usecs)
#define LCD_BACKLIGHT_TIME 10000000

// Definition of user input delay (en usecs)
#define USER_INPUT_DELAY 100000

// Definition of custom char references
#define MODE_DIRECT 0
#define MODE_SEND 1
#define STATUS_PLAY 2
#define STATUS_PAUSE 3
#define STATUS_STOP 4

// Analog input definition
const int buttonPin = A0;
const int rotaryPin = A1;

// Global potentiometer value
int rotaryVal = -2;

// Global triggers to follow up
int midiTicks;
long lastMidiCheck;
long lastLEDCheck;
long lastUICheck;
long lastBacklightCheck;

// MIDI delay between each 24 ticks / beat
long midiDelay;

// LED blink config
bool ledOnState = false;

// LCD ON config
bool lcdOnState = true;

// Definition of LCD display
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// Playing states enum
enum {
  IS_PLAYING,
  IS_PAUSED,
  IS_STOPPED
};

// Last button pressed
int buttonVal;

// Playing state
byte playState = IS_STOPPED;

// Definition of custom char bitmaps
byte mode_direct[8] = {
  B00000,
  B00100,
  B00110,
  B11111,
  B00110,
  B00100,
  B00000,
  B00000
};

byte mode_send[8] = {
  B00000,
  B01110,
  B10001,
  B10101,
  B10001,
  B01110,
  B00000,
  B00000
};

byte status_play[8] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01100,
  B01000,
  B00000,
  B00000
};

byte status_pause[8] = {
  B00000,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B00000,
  B00000
};

byte status_stop[8] = {
  B00000,
  B01110,
  B01110,
  B01110,
  B01110,
  B01110,
  B00000,
  B00000
};

// By default, we directly send MIDI BPM changes
bool modeDirect = true;

// Setup section
void setup() {
  
  //Init Serial USB communication for sending MIDI messages
  Serial.begin(31250);

  // Set tempo led digital pin to output mode
  pinMode(TEMPO_LED_PIN, OUTPUT);

  // Set LCD backlighting digital pin to output mode
  pinMode(BACKLIGHTING_PIN, OUTPUT);

  // Init LCD in 2x16 mode
  lcd.begin(16, 2);

  // Create custom chars
  lcd.createChar(MODE_DIRECT, mode_direct);
  lcd.createChar(MODE_SEND, mode_send);
  lcd.createChar(STATUS_PLAY, status_play);
  lcd.createChar(STATUS_PAUSE, status_pause);
  lcd.createChar(STATUS_STOP, status_stop);

  // Display splash title for 3s
  lcd.clear();
  backlightOn(0);
  lcd.print(" ARDUINO SIMPLE ");
  lcd.setCursor(3, 1);
  lcd.print("MIDI CLOCK");
  delay(3000);
  lcd.clear();

  // Display default interface
  lcd.print("    STATUS:");
  lcd.write(byte(STATUS_STOP));
  lcd.setCursor(0, 1);
  lcd.print("BPM:      MODE:");
  lcd.write(byte(MODE_DIRECT));
  
  // Reset all triggers before we start
  resetTriggers();

  // Default state for MIDI clock is off
  Serial.write(MIDI_STOP);
}

// Looping section
void loop() {
  dispatcher();
}

void resetTriggers(void) {
  midiTicks = 0;
  lastMidiCheck = 0;
  lastLEDCheck = 0;
  lastUICheck = 0;
  lastBacklightCheck = micros();
}

// The dispatcher allows to check when new events have to be fired without blocking the main loop
void dispatcher() {

  // Get present time in microseconds since Arduino has been started
  long now = micros();

  // Time to read user input
  if(now - lastUICheck > USER_INPUT_DELAY) {
    lastUICheck = now;
    rotaryVal = rotaryListener(now);
    buttonListener(now);
  }

  // Time to send a MIDI clock signal
  if(now - lastMidiCheck >= midiDelay) {
    lastMidiCheck = now;
    if(playState == IS_PLAYING) {
      if(midiTicks == 0 && playState == IS_PLAYING) ledOn(now);
      midiTicks++;
      if(midiTicks > 23) midiTicks = 0;
    }
    Serial.write(MIDI_TIMING_CLOCK);
  }

  // Time to turn OFF LED
  if(ledOnState && now - lastLEDCheck > LED_BLINK_TIME) ledOff();

  // Time to turn OFF LCD
  if(lcdOnState && now - lastBacklightCheck > LCD_BACKLIGHT_TIME) backlightOff();
}

// Reads the rotary resistor value and adjust BPM value if it has changed
int rotaryListener(long now) {
  int newRotaryVal = analogRead(rotaryPin);

  // We have set a small error margin of 1 when reading rotary resistor value
  // This value is not 100% accurate in time and can slightly drift
  if(newRotaryVal < rotaryVal - 1 || newRotaryVal > rotaryVal + 1) {
    backlightOn(now);
    int bpm = map(newRotaryVal, 0, 1020, 40, 300);
    lcd.setCursor(4, 1);
    lcd.print(String(bpm) + " ");
    if(modeDirect) midiDelay = long(round(60000000 / bpm / 24));
  }
  return newRotaryVal;
}

// Set BPM LED On
void ledOn(long now) {
  lastLEDCheck = now;
  ledOnState = true;
  digitalWrite(TEMPO_LED_PIN, HIGH);
}

// Set BPM LED Off
void ledOff(void) {
  ledOnState = false;
  digitalWrite(TEMPO_LED_PIN, LOW);
}

// Set LCD backlighting On
void backlightOn(long now) {
  lastBacklightCheck = now;
  lcdOnState = true;
  digitalWrite(BACKLIGHTING_PIN, HIGH);
}

// Set LCD backlighting Off
void backlightOff(void) {
  lcdOnState = true;
  digitalWrite(BACKLIGHTING_PIN, LOW);
}


// Listen to button presses from LCD buttons
// To avoid multiple false presses, we store last pressed button
// and only fire the event if it is different from previous one
void buttonListener(long now) {
  int newButtonVal = analogRead(buttonPin);
  if (newButtonVal < BUTTON_RIGHT) {}
  else if (newButtonVal < BUTTON_UP) {
    if(buttonVal != BUTTON_UP) {
      backlightOn(now);
      buttonVal = BUTTON_UP;
      lcd.setCursor(11, 0);
      if(playState == IS_PLAYING) {
        playState = IS_PAUSED;
        lcd.write(byte(STATUS_PAUSE));
        Serial.write(MIDI_STOP);
      }
      else if(playState == IS_PAUSED) {
        playState = IS_PLAYING;
        lcd.write(byte(STATUS_PLAY));
        Serial.write(MIDI_CONTINUE);
      }
      else {
        playState = IS_PLAYING;
        lcd.write(byte(STATUS_PLAY));
        Serial.write(MIDI_START);
      }
    }
  }
  else if (newButtonVal < BUTTON_DOWN) {
    if(buttonVal != BUTTON_DOWN) {
      backlightOn(now);
      buttonVal = BUTTON_DOWN;
      midiTicks = 0;
      playState = IS_PLAYING;
      lcd.setCursor(11, 0);
      lcd.write(byte(STATUS_PLAY));
      Serial.write(MIDI_START);
    }
  }
  else if (newButtonVal < BUTTON_LEFT) {
    if(buttonVal != BUTTON_LEFT) {
      backlightOn(now);
      buttonVal = BUTTON_LEFT;
      modeDirect = !modeDirect;
      lcd.setCursor(15, 1);
      if(modeDirect) lcd.write(byte(MODE_DIRECT));
      else lcd.write(byte(MODE_SEND));
    }
 }
  else if (newButtonVal < BUTTON_SELECT) {}
  else buttonVal = BUTTON_NONE;
}