// Requires Arduino Joystick Library: https://github.com/mheironimus/arduinojoysticklibrary

#include <Keypad.h>
#include <Joystick.h>
#include <Encoder.h>

// CONFIGURATION
bool pulseModeActive = true; // Initial startup state (true: Pulse Mode; false: Toggle Mode)
const int pulseDuration = 80; // Duration of the pulse in milliseconds

// IDs of the physical toggle switches that should use the pulse mode:
const int pulseTargets[] = {25, 26, 27, 28, 29, 30, 31, 32};  // RawIDs of both states of SW7, SW8, SW9, SW10
const int numPulseTargets = sizeof(pulseTargets) / sizeof(pulseTargets[0]);

// Button Combo to switch between Pulse/Toggle Mode
const int secretKey1 = 33; // First button to hold (rawID)
const int secretKey2 = 35; // Second button to hold (rawID)
const int secretKey3 = 2;  // Third button to hold (rawID)
bool comboFired = false;  // Prevents multiple triggers while holding the combo
// ---------------------

// Matrix Config
const byte ROWS = 6; 
const byte COLS = 6; 
byte rowPins[ROWS] = {2, 3, 4, 5, 6, 7}; 
byte colPins[COLS] = {16, 15, 14, 10, 9, 8}; 
const int PIN_LED = 11;

char keys[ROWS][COLS] = {
  {0,1,2,3,4,5},{6,7,8,9,10,11},{12,13,14,15,16,17},
  {18,19,20,21,22,23},{24,25,26,27,28,29},{30,31,32,33,34,35}
};

const byte buttonMap[36] = {
  3,2,1,0,28,26, 6,5,4,33,27,25, 18,16,14,34,32,30,
  17,15,13,35,31,29, 24,22,20,12,8,7, 23,21,19,11,10,9
};

Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Joystick Config
Joystick_ Joystick(0x08, 0x04, 128, 0, 
  false, false, true,      // Deactivates X & Y, Activates Z (Slider Potentiometer)
  true, true, false,       // Activates Rx & Ry (Potentiometers), Deactivates Rz
  false, false,            // Deactivates Rudder & Throttle
  false, false,            // Deactivates Accelerator & Brake
  false);                  // Deactivates Slider

// Encoder & Variables
Encoder enc1(0, 1); Encoder enc2(A0, A1); Encoder enc3(A2, A3); 
long oldPos[] = {0, 0, 0};
bool modeActive = false;

// Previous axis values used for change detection
int lastRx = 0, lastRy = 0, lastZ = 0;

// Helper to check if a button ID is in our pulse target list
bool isPulseTarget(int id) {
  for(int i = 0; i < numPulseTargets; i++) {
    if(id == pulseTargets[i]) return true;
  }
  return false;
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  Joystick.begin(true);

  // Visual Feedback: LED blinks 3 times on startup
  for(int i=0; i<3; i++) {
    analogWrite(PIN_LED, 255); delay(100);
    analogWrite(PIN_LED, 0); delay(100);
  }

  // Restore LED state just in case the Mode switch is already physically pressed during boot
  kpd.getKeys();
  for (int i=0; i<10; i++) {
    if (buttonMap[(int)kpd.key[i].kchar] == 0) {
      if (kpd.key[i].kstate == PRESSED || kpd.key[i].kstate == HOLD) {
        modeActive = true;
        analogWrite(PIN_LED, 150);
      }
    }
  }
}

void loop() {
  if (kpd.getKeys()) {
    
    // Pulse/Toggle Switch Combo Logic
    bool k1_Active = false;
    bool k2_Active = false;
    bool k3_Active = false;
    
    // Check all currently active keys
    for (int i=0; i<10; i++) {
      if (kpd.key[i].kstate == PRESSED || kpd.key[i].kstate == HOLD) {
        int rawID = buttonMap[(int)kpd.key[i].kchar];
        if (rawID == secretKey1) k1_Active = true;
        if (rawID == secretKey2) k2_Active = true;
        if (rawID == secretKey3) k3_Active = true;
      }
    }

    // If all secret keys are held down simultaneously
    if (k1_Active && k2_Active && k3_Active) {
      if (!comboFired) { // Only trigger ONCE per hold
        pulseModeActive = !pulseModeActive; // Flip the mode!
        comboFired = true;
        
        // Fast LED strobe to confirm mode change
        for(int b=0; b<5; b++) {
          analogWrite(PIN_LED, 255); delay(40);
          analogWrite(PIN_LED, 0); delay(40);
        }
        analogWrite(PIN_LED, modeActive ? 150 : 0); // Restore correct LED state
        
        // Apply the new mode instantly to all switches
        refreshAllButtons();
      }
    } else {
      // Reset the trigger lock once the keys are released
      comboFired = false; 
    }

    // REGULAR MATRIX & MODE SWITCH LOGIC
    for (int i=0; i<10; i++) { 
      int rawID = buttonMap[(int)kpd.key[i].kchar];

      if (rawID == 0 && kpd.key[i].stateChanged) {
        if (kpd.key[i].kstate == PRESSED) {
          modeActive = true;
          analogWrite(PIN_LED, 150);
          refreshAllButtons(); 
        }
        else if (kpd.key[i].kstate == RELEASED) {
          modeActive = false;
          analogWrite(PIN_LED, 0);
          refreshAllButtons(); 
        }
      } 
      else if (rawID > 0 && kpd.key[i].stateChanged) {
        updateSingleButton(i);
      }
    }
  }

  // Encoder (Base IDs: 37, 39, 41) ---
  checkEncoder(enc1, 0, 37); 
  checkEncoder(enc2, 1, 39); 
  checkEncoder(enc3, 2, 41);

  // Analog Axes
  // 4x Oversampling
  auto readSmooth = [](int pin) {
    long sum = 0;
    for(int i=0; i<4; i++) sum += analogRead(pin);
    return (int)(sum / 4);
  };

  int valL = readSmooth(A4); 
  int valR = readSmooth(A5); 
  int valF = readSmooth(A11);

  // Threshold of > 1 causes value change, increase if input starts to jitter
  if (abs(valL - lastRx) > 1) { Joystick.setRxAxis(valL); lastRx = valL; }
  if (abs(valR - lastRy) > 1) { Joystick.setRyAxis(valR); lastRy = valR; }
  if (abs(valF - lastZ) > 1)  { Joystick.setZAxis(valF);  lastZ = valF; }
}

// Helper Functions

void updateSingleButton(int i) {
  int rawID = buttonMap[(int)kpd.key[i].kchar];
  int btn = modeActive ? (rawID + 42) : rawID;
  
  // Clear both Banks (Bank 1: rawID, Bank 2: rawID + 42)
  Joystick.setButton(rawID - 1, false);
  Joystick.setButton(rawID + 42 - 1, false);

  // Check if this button is a toggle switch AND if the live pulse mode is active
  if (pulseModeActive && isPulseTarget(rawID)) {
    // PULSE MODE: 
    // Triggering only once for a set duration if pressed (Pulse VS Toggle)
    if (kpd.key[i].kstate == PRESSED) {
      Joystick.setButton(btn - 1, true);
      delay(pulseDuration);
      Joystick.setButton(btn - 1, false);
    }
  } 
  else {
    // TOGGLE / MOMENTARY MODE (Original Behavior for all other buttons)
    bool isPressed = (kpd.key[i].kstate == PRESSED || kpd.key[i].kstate == HOLD);
    if (isPressed) {
      Joystick.setButton(btn - 1, true);
    }
  }
}

void refreshAllButtons() {
  for (int i=0; i<10; i++) {
    int rawID = buttonMap[(int)kpd.key[i].kchar];
    if (rawID > 0) {
      updateSingleButton(i);
    }
  }
}

void checkEncoder(Encoder &encoder, int id, int baseBtn) {
  long newPos = encoder.read() / 4; 
  if (newPos != oldPos[id]) {
    // Offset Encoder when Mode Switch is active
    int currentBase = modeActive ? (baseBtn + 42) : baseBtn;
    
    int btn = (newPos > oldPos[id]) ? currentBase : (currentBase + 1);
    int otherBtn = (newPos > oldPos[id]) ? (currentBase + 1) : currentBase;
    
    Joystick.setButton(otherBtn - 1, 0); 
    Joystick.setButton(btn - 1, 1);
    delay(7); 
    Joystick.setButton(btn - 1, 0);
    
    oldPos[id] = newPos;
  }
}