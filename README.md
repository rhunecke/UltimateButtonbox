# Ultimate Button Box
This repository contains an .INO sketch for an Arduino Leonardo microcontroller board to be used as a HID Game Controller with a custom 3D printed case. All printing files can be found on [Makerworld](https://makerworld.com/en/models/2670201-ultimate-sim-racing-flight-button-box-arduino).

## Description
This **Button Box** or **Sim-Panel** is a high-performance, compact controller designed for flight simulation (MSFS, DCS, X-Plane etc.) and sim-racing. It leverages an intelligent **Dual-Layer (Shift) system** to provide a staggering **84 Buttons** (82 mappable digital functions + 2 mode states with LED indicator) alongside 3 high-precision analog axes, all powered by a single Arduino Leonardo.
 

Whether you need to manage complex autopilot systems, engine mixtures, or landing gear, this panel provides the tactile feedback and reliability required for a professional setup.

## 🚀 Key Features
- **Dual-Layer Logic:** A dedicated Mode button toggles all matrix inputs and encoders into a second command layer.
- **82 Digital Commands:** 64 button functions, 6 encoder push-buttons (dual-layered), and 12 encoder rotation commands split into two banks.
- **3 Analog Axes:** 2x Rotary Potentiometers and 1x 60mm Linear Fader (ideal for Throttle, Flaps, or Mixture).
- **Zero-Ghosting Matrix:** A full 6x6 matrix equipped with 36 diodes (1N4148) to ensure every input is registered accurately.
- **Industrial Design:** Robust housing using M3 and M2 brass threaded inserts for a professional, screw-together assembly.
- **Plug & Play:** Recognized as a standard HID Game Controller (Joystick) in Windows - no external drivers required.
- **Toggle/Pulse Mode:** Press _EN01 + EN03 + BN02_ to change _SW07, SW08, SW09_ and _SW10_ between _Toggle_ and _Pulse_ mode. While _Toggle_ will keep the key pressed, _Pulse_ will send only a short button press when flipping the switch. Refer to wiring schematics for more details.

## 🖥️ 3D Printing
- The case and lid can be printed in PLA, I recommend PETG HF for durability though.
- Printing requires supports
- I include a profile for printing PETG with a PLA interface layer as well as a No-AMS profile for printing a single material
- Clearances were designed around the components I used (see shopping list). Might require adjustments when using other components.
- The case can be printed with 2 walls and 15% infill. Parts for threaded inserts should be set to 100% infill.
- Out of vanity I printed a case featuring my logo on the inside, this version is also included, as well as a test piece for testing clearances.

**Printing times**
- When printing PETG + PLA interface layer for supports (P2S + AMS)
  - Case: 6h31m
  - Lid: 3h16m
  - Clearance test piece: 1h26m
- When printing single material (P2S)
  - Case: 5h59m
  - Lid: 3h16m
  - Clearance test piece: 48m

## 🔌 Wiring
- [Cirkit Designer Wiring Schematics](https://app.cirkitdesigner.com/project/6d7af60f-560e-4cac-84b6-958b4d1232e1)
 
## 🛒 Shopping List
- The shopping list includes all the components I used. Depending on your location, these may be available from other retailers or can be replaced with equivalent components.

## </> INO Sketch for Flashing your Arduino Leonardo
- Open buttonbox.ino in [Arduino IDE](https://docs.arduino.cc/software/ide/)
- Add the Encoder and Keypad libraries via the Library Manager. Also add Matthew Heironimus' [ArduinoJoystickLibrary](https://github.com/mheironimus/arduinojoysticklibrary) to the sketch via _Sketch/Include Library/Add .ZIP Library_.
- Make sure your Arduino Leonardo is connected and the COM port selected.
- Upload the sketch to your Leonardo.
- Be aware that Windows will not show more than 32 buttons when calibrating the device. Nonetheless they are recognized and working. For testing I created [HID Tester - A free Joystick Testing App](https://github.com/rhunecke/HIDTester). Any other modern tools work as well.

## 📝 License
**3D Print Files**
- The 3D models for the enclosure are hosted exclusively on MakerWorld. You can download the print files here: [Ultimate Sim-Racing & Flight Button Box | Arduino](https://makerworld.com/en/models/2670201-ultimate-sim-racing-flight-button-box-arduino)

**Code**
- The Arduino code and documentation provided in this GitHub repository are licensed under the GNU General Public License 3.0