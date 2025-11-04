// =========================================================================
//  MINIMAL TOUCH TEST for 480x320 ST7796 Display with Shared SPI
// =========================================================================
//
//  PURPOSE:
//  This sketch uses ONLY the TFT_eSPI library to test the touch screen.
//  It does not use the external XPT2046_Touchscreen library.
//  This isolates the problem to either hardware/setup or the main sketch.
//
//  INSTRUCTIONS:
//  1. Ensure your User_Setup.h has #define TOUCH_CS 33
//  2. Upload this sketch.
//  3. Open the Serial Monitor at 115200 baud.
//  4. Touch the colored buttons on the screen.
//
//  EXPECTED RESULT:
//  - You will see the touch coordinates printed in the Serial Monitor.
//  - You will see the coordinates and which button was pressed printed on the screen.
//  - The button you touch will flash a white border.
//
// =========================================================================

#include <SPI.h>
#include <TFT_eSPI.h> // We only need this one library!

// Create an instance of the TFT_eSPI class
TFT_eSPI tft = TFT_eSPI();

// Define a simple structure for our buttons
struct Button {
  int x, y, w, h;
  const char* label;
  uint16_t color;
};

// Create three test buttons
Button buttons[3] = {
  { 50,  80, 150, 80, "Button 1", TFT_RED   },
  { 280, 80, 150, 80, "Button 2", TFT_GREEN },
  { 165, 190, 150, 80, "Button 3", TFT_BLUE  }
};


void setup() {
  Serial.begin(115200);
  Serial.println("\n\nStarting Touch Test Sketch...");

  // Initialise the display
  tft.init();
  tft.setRotation(1); // Set landscape mode (480 wide x 320 high)
  // Load touch screen calibration data. This ensures touch coordinates are accurate.
  // This data is usually found by running a calibration sketch once.
  // To get the calibration data run the tft generic Touch_calibrate example sketch and open the serial monitor
  uint16_t calData[5] = { 252, 3676, 180, 3648, 7 };
  tft.setTouch(calData);

  tft.fillScreen(TFT_BLACK);

  // --- Draw the UI ---
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextDatum(MC_DATUM); // Middle-Center text alignment
  tft.setTextFont(4);
  tft.drawString("Touch a button", tft.width() / 2, 40);

  // Draw all the buttons
  for (int i = 0; i < 3; i++) {
    tft.fillRoundRect(buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h, 10, buttons[i].color);
    tft.drawString(buttons[i].label, buttons[i].x + buttons[i].w / 2, buttons[i].y + buttons[i].h / 2);
  }

  // Draw a line to separate the feedback area
  tft.drawFastHLine(0, 280, tft.width(), TFT_DARKGREY);
  Serial.println("Setup complete. Ready for touch input.");
}


void loop() {
  uint16_t tx, ty; // To store the touch coordinates

  // This is the key function from TFT_eSPI.
  // It returns true if a touch is detected and fills tx, ty with the coordinates.
  bool is_touched = tft.getTouch(&tx, &ty);

  // If the screen is touched
  if (is_touched) {
    // 1. Print coordinates to Serial Monitor for raw debugging
    Serial.print("Touch detected at -> X: ");
    Serial.print(tx);
    Serial.print(", Y: ");
    Serial.println(ty);

    // 2. Clear the previous text feedback on the screen
    tft.fillRect(0, 285, tft.width(), 35, TFT_BLACK);

    // 3. Display the current coordinates on the screen
    char buf[50];
    sprintf(buf, "Touch at: %d, %d", tx, ty);
    tft.setTextFont(2);
    tft.drawString(buf, tft.width() / 2, 300);

    // 4. Check if the touch was inside any of our buttons
    for (int i = 0; i < 3; i++) {
      if (tx > buttons[i].x && tx < (buttons[i].x + buttons[i].w) &&
          ty > buttons[i].y && ty < (buttons[i].y + buttons[i].h)) {

        Serial.print(">>> Button pressed: ");
        Serial.println(buttons[i].label);

        // Give visual feedback by drawing a white border
        tft.drawRoundRect(buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h, 10, TFT_WHITE);

        // Wait a little bit so we can see the flash
        delay(150);

        // Remove the border by drawing over it with the original color
        tft.drawRoundRect(buttons[i].x, buttons[i].y, buttons[i].w, buttons[i].h, 10, buttons[i].color);
      }
    }

    // A small delay to prevent overwhelming the Serial output
    delay(100);
  }
}
