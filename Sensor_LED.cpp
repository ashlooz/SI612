// Include Particle Device OS APIs
#include "Particle.h"
#include "../lib/neopixel/src/neopixel.h"
//#include "neopixel.h" // Include the NeoPixel library

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// NeoPixel setup
// Important: For Particle P2, we need to use a pin number, not D0
#define PIXEL_PIN SPI1    // Using pin D0, will be properly resolved
#define PIXEL_COUNT 1   // For a single 5mm LED
#define PIXEL_TYPE WS2812 // Common type for Adafruit diffused LEDs

// Initialize NeoPixel strip - use a proper pin number
Adafruit_NeoPixel pixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
//Adafruit_NeoPixel pixel(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Color variables
int red = 255;
int green = 255;
int blue = 255;
String colorName = "spring"; // Default color
bool powerOn = true;  // Track if power is on or off

// Function prototypes
void setColor(String color);
int changeColor(String command);
int setPower(String command);  // New function to control power

// setup() runs once, when the device is first turned on
void setup() {
  // Initialize NeoPixel
  pixel.begin();
  pixel.show(); // Initialize all pixels to 'off'
  
  // Register cloud functions
  Particle.function("changeColor", changeColor);
  Particle.function("setPower", setPower);  // Register new function
  
  Serial.begin(9600);
  setColor(colorName);
}

// loop() runs over and over again, as quickly as it can execute.
void loop() {
  // Example: Check if there's serial input to change the color or power
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    
    if (input == "spring" || input == "summer" || 
        input == "fall" || input == "winter" || input == "pink") {
      setColor(input);
      Serial.println("Changed to " + input);
    }
    else if (input == "on") {
      setPower("on");
    }
    else if (input == "off") {
      setPower("off");
    }
  }
  delay(100);
}

// Function to control power
int setPower(String command) {
  if (command == "off" || command == "OFF") {
    powerOn = false;
    pixel.setPixelColor(0, pixel.Color(0, 0, 0));  // Turn off the LED
    pixel.show();
    Log.info("LED power OFF");
    return 1;
  }
  else if (command == "on" || command == "ON") {
    powerOn = true;
    setColor(colorName);  // Restore previous color
    Log.info("LED power ON");
    return 1;
  }
  return -1;  // Invalid command
}

// Function to set LED color
void setColor(String color) {
  colorName = color;
  
  if (colorName == "spring") { // neutral white
    red = 255; 
    green = 255; 
    blue = 255;
  } else if (colorName == "summer") { // bright yellow hue
    red = 255; 
    green = 120; 
    blue = 30;
  } else if (colorName == "fall") { // warm soft white
    red = 255; 
    green = 150; 
    blue = 80;
  } else if (colorName == "winter") { // cool blue hue
    red = 100; 
    green = 180; 
    blue = 255;
  } else if (colorName == "pink") { // cool pink hue
    red = 255; 
    green = 105; 
    blue = 180;
  }
  
  // Only update LED if power is on
  if (powerOn) {
    // Set the LED color
    pixel.setPixelColor(0, pixel.Color(green, red, blue));
    pixel.setBrightness(180); // Set brightness (0-255)
    pixel.show(); // Update the LED
  }
  
  Log.info("Color set to %s (R:%d,G:%d,B:%d)", 
           colorName.c_str(), red, green, blue);
}

// Cloud function to change color
int changeColor(String command) {
  if (command == "spring" || command == "summer" || 
      command == "fall" || command == "winter" || command == "pink") {
    setColor(command);
    return 1; // Success
  }
  return -1; // Invalid command
}