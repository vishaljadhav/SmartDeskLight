#include <FastLED.h>

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "";
char pass[] = "";

// define timers to read
BlynkTimer wipeOutTimer;

// onboard led to check status
#define DEFAULT_LED 2

// neo pixel setup
#define LED_PIN 13
#define LED_COUNT 60
#define LED_BRIGHTNESS 60
#define CLOCK_PIN

CRGB leds[LED_COUNT];

// Animation Type
// 1 = Wipeout
// 2 = Rainbow
int animationType = 1;

// Animations
int wipeOutAnimation = 0;
int rainbowAnimation = 0;

void setup()
{
    Blynk.begin(auth, ssid, pass);

    FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, LED_COUNT);
    FastLED.setBrightness(LED_BRIGHTNESS);
    FastLED.clear();
    FastLED.show();

    wipeOutTimer.setInterval(9000, playWipeOutAnimation);
}

void loop()
{
    Blynk.run();
    // You can inject your own code or combine it with other sketches.
    // Check other examples on how to communicate with Blynk. Remember
    // to avoid delay() function!

    wipeOutTimer.run();
}

void setAnimationState(int animationType)
{
    // reset all animations
    wipeOutAnimation = 0;
    rainbowAnimation = 0;

    switch (animationType)
    {
    case 1:
        wipeOutAnimation = 1;
        break;

    case 2:
        rainbowAnimation = 1;
        break;
    }
}

// Fill strip pixels one after another with a color. Strip is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// strip.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.
void colorWipe(int r, int g, int b, int wait)
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds[i] = CRGB(r, g, b);
        FastLED.show();
        delay(wait);
    }
}

void playWipeOutAnimation()
{
    colorWipe(255, 0, 0, 50);
    delay(500);

    colorWipe(0, 255, 0, 50);
    delay(500);

    colorWipe(0, 0, 255, 50);
    delay(500);
}

BLYNK_CONNECTED()
{
    printConsole("Sync in Progress");
    Blynk.syncAll();
    printConsole("Sync Done");
}

// V4 Color Wipe
BLYNK_WRITE(V4)
{
    int pinVal = param.asInt();

    if (pinVal == 1)
    {
        setAnimationState(1)
    }
}

void checkD2Status()
{
    int ledState = digitalRead(DEFAULT_LED);
    Blynk.virtualWrite(V2, ledState == 1 ? 255 : 0);
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
// void theaterChase(uint32_t color, int wait)
// {
//     for (int a = 0; a < 10; a++)
//     { // Repeat 10 times...
//         for (int b = 0; b < 3; b++)
//         {                 //  'b' counts from 0 to 2...
//             leds.clear(); //   Set all pixels in RAM to 0 (off)
//             // 'c' counts up from 'b' to end of strip in steps of 3...
//             for (int c = b; c < strip.numPixels(); c += 3)
//             {
//                 leds[c] = CRGB(color); // Set pixel 'c' to value 'color'
//             }
//             leds.show(); // Update strip with new contents
//             delay(wait); // Pause for a moment
//         }
//     }
// }
//
//// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
//void rainbow(int wait) {
//  // Hue of first pixel runs 5 complete loops through the color wheel.
//  // Color wheel has a range of 65536 but it's OK if we roll over, so
//  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
//  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
//  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
//    for (int i = 0; i < leds.numPixels(); i++) { // For each pixel in strip...
//      // Offset pixel hue by an amount to make one full revolution of the
//      // color wheel (range of 65536) along the length of the strip
//      // (strip.numPixels() steps):
//      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
//      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
//      // optionally add saturation and value (brightness) (each 0 to 255).
//      // Here we're using just the single-argument hue variant. The result
//      // is passed through strip.gamma32() to provide 'truer' colors
//      // before assigning to each pixel:
//      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
//    }
//    strip.show(); // Update strip with new contents
//    delay(wait);  // Pause for a moment
//  }
//}
//
//// Rainbow-enhanced theater marquee. Pass delay time (in ms) between frames.
//void theaterChaseRainbow(int wait) {
//  int firstPixelHue = 0;     // First pixel starts at red (hue 0)
//  for (int a = 0; a < 30; a++) { // Repeat 30 times...
//    for (int b = 0; b < 3; b++) { //  'b' counts from 0 to 2...
//      strip.clear();         //   Set all pixels in RAM to 0 (off)
//      // 'c' counts up from 'b' to end of strip in increments of 3...
//      for (int c = b; c < strip.numPixels(); c += 3) {
//        // hue of pixel 'c' is offset by an amount to make one full
//        // revolution of the color wheel (range 65536) along the length
//        // of the strip (strip.numPixels() steps):
//        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
//        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); // hue -> RGB
//        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
//      }
//      strip.show();                // Update strip with new contents
//      delay(wait);                 // Pause for a moment
//      firstPixelHue += 65536 / 90; // One cycle of color wheel over 90 frames
//    }
//  }
//}

// V3 Brightness slider
BLYNK_WRITE(V3)
{
    int pinVal = param.asInt();

    FastLED.setBrightness(pinVal);
    FastLED.show();
}

// V6 Brightness slider
BLYNK_WRITE(V6)
{
    FastLED.clear();
    FastLED.show();
}

// V5 zRGB
BLYNK_WRITE(V5)
{
    // get a RED channel value9o98koh6ryesw
    int r = param[0].asInt();
    // get a GREEN channel value
    int g = param[1].asInt();
    // get a BLUE channel value
    int b = param[2].asInt();

    for (int i = 0; i < LED_COUNT; i++)
    {
        leds[i] = CRGB(r, g, b);
    }
    FastLED.show();
}

// V1 Button
BLYNK_WRITE(V1)
{
    int pinVal = param.asInt();
    int led_state = digitalRead(DEFAULT_LED);

    if (pinVal == 1)
    {
        digitalWrite(DEFAULT_LED, HIGH);
        Blynk.virtualWrite(V2, 255); //sending to Blynk
    }
    else
    {
        digitalWrite(DEFAULT_LED, LOW);
        Blynk.virtualWrite(V2, 0); //sending to Blynk
    }
}

void printConsole(String message)
{
    Blynk.virtualWrite(V0, message);
}

void printConsole(int message)
{
    Blynk.virtualWrite(V0, message);
}