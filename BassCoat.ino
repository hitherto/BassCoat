#include <FastLED.h>

//
// Global config!
//
// Unless you want to tinker, you can probably set this stuff up, build your
// coat, and everything will be shiny without diving into the amateur-hour
// code below.
//
#define LED_PIN 6 // which digital out pin are your neopixels plugged into?
#define MICROPHONE_PIN 9 // which analog in pin is your microphone plugged into?
#define BUTTON_PIN 0 // which digital in pin is your mode-switch button plugged into?
#define LED_COUNT 50 // how many lights did you sew into your coat, you crazy buffoon?
#define DEFAULT_BRIGHTNESS 60 // what brightness value do we want by default?
#define STATIC_BRIGHTNESS 80 // default brightness for non-bass rainbow mode.
#define GOT_ME_SOME_BASS 150 // how bright do we want to shine when the bass hits?
#define SAMPLE_PERIOD_US 100 // Our audio sampling rate, Advised: 100 (2500hz) for 
                             // Flora boards, 200 (5000hz) for more capable Arduinos

// defines for setting and clearing register bits
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

// Global variables for light control

// LED buffer to track color
CRGB leds[LED_COUNT];

// Current color (rgb)
byte LEDColor[3];

// Initialize our global mode indicator
byte Mode = 0;

// Set Brightness (for pulsing with the bass)
byte Brightness = STATIC_BRIGHTNESS;

// Track our rainbow fade
byte WheelPos = 0;
unsigned long LastChange = micros(); // track when we last changed color
unsigned long WheelPeriod = 40000; // microseconds before moving to the next wheel color

// Track twinkles
float TwinkleState[LED_COUNT];

// Direction for glow state
byte GlowDirection = 1;

// Register when we last got a button interrupt, for debouncing
static unsigned long LastInterruptTime = 0;

void setup() {
  // Set ADC to 77khz, max for 10bit
  sbi(ADCSRA,ADPS2);
  cbi(ADCSRA,ADPS1);
  cbi(ADCSRA,ADPS0);

  // Start the lights
  FastLED.addLeds<WS2811, LED_PIN>(leds, LED_COUNT);
  FastLED.show(); // Initialize all pixels to 'off'

  // Initialize the button interrupt
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(2, buttonPushed, FALLING);

  // Initialize our twinkle state tracker
  for (uint16_t i = 0; i < LED_COUNT; i++){
    TwinkleState[i] = 1;
  }
}

void loop() {
  uint16_t i;

  // Sample audio for dynamic modes only
  if (Mode == 1 or Mode == 2){
    SampleAudio();
  }

  if (Mode == 0 or Mode == 1){
    LightsRainbow();
  } else if (Mode == 2){
    LightsTwinkle();
  } else if (Mode == 3){
    LightsGlow();
  }

  // The micros() counter overflows after 70 minutes or so. Given we're
  // potentially hooking this thing up to a huuuge battery and running it
  // for hours, we need to cope with the timer reset or our coat will
  // become distressingly dull after the clock runs over.
  //
  // This basic approach won't save us on the cycle where the reset
  // happens, but the microsecond-level pause shouldn't be noticeable to
  // anyone except Superman.
  if (micros() < LastChange) LastChange = 0;

  // Send our new values out to the neopixels
  if (Mode < 4){
    FastLED.show();
  }
}

void LightsRainbow(){
    // Use the internal clock to cycle colors at a constant rate, whatever
    // is happening with audio sampling/brightness values
    if (micros() > WheelPeriod+LastChange){
      WheelPos++;
      if (WheelPos > 255) WheelPos = 0;
      LastChange = micros();
    }
    // Set all the pixels to the latest and greatest values
    Wheel(WheelPos & 255, LEDColor);
    FastLED.setBrightness(Brightness);
    fill_solid(leds, LED_COUNT, CRGB(LEDColor[0], LEDColor[1], LEDColor[2]));
}

// Twinkle mode. If you set your lights up in a white fur coat this'll make
// you look like a thundercloud! Basically, each time we detect a bass note,
// we'll set a random light to max brightness, then slowly dim it back to
// the base state over 4 cycles.
void LightsTwinkle(){
  if (Brightness==GOT_ME_SOME_BASS) {
      TwinkleState[random(LED_COUNT)] = 4;
  }

  if (micros() > (WheelPeriod*2)+LastChange){
    for(uint16_t i = 0; i < LED_COUNT; i++){
      if (TwinkleState[i] > 1) TwinkleState[i]--;
    }
    LastChange = micros();
  }
  for(uint16_t i = 0; i < LED_COUNT; i++){
    uint16_t b = 0;
    if (TwinkleState[i] == 1) { 
      b = 65;
    } else {
      b = (63*TwinkleState[i])+2;
    }
    leds[i].setRGB(b, b, b);
  }
}

void LightsGlow(){
  if (micros() > (WheelPeriod/4)+LastChange){
    Brightness+=GlowDirection;
    if (Brightness >= 200){
      GlowDirection = -1;
    } else if (Brightness <= 30){
      GlowDirection = 1;
    }
    LastChange = micros();
  }
  FastLED.setBrightness(Brightness);
}

// Button-push handling with basic debounce. This routine does whatever initial
// setup is needed for each mode (some modes are static, so this is all that
// actually happens until the next button-push)
void buttonPushed(){
  unsigned long InterruptTime = millis();
  // Simple signal debounce
  // The or is to cope with the millis() clock overflowing after 70 mins
  if (InterruptTime - LastInterruptTime > 200 or InterruptTime < LastInterruptTime){
    Mode++;
    if (Mode > 7) Mode = 0;

    // Initial setups for certain modes
    if (Mode == 0){
      Brightness == DEFAULT_BRIGHTNESS;
    } else if (Mode == 3 || Mode == 6){
      LEDColor[0] = 0;
      LEDColor[1] = 0;
      LEDColor[2] = 255;
      fill_solid(leds, LED_COUNT, CRGB(LEDColor[0], LEDColor[1], LEDColor[2]));
      if (Mode == 6){
        FastLED.setBrightness(STATIC_BRIGHTNESS);
        FastLED.show();
      }
    } else if (Mode == 4){
      for (int led = 0; led < 13; led++){
        leds[led] = CRGB::White;
      }
      for (int led = 27; led < 38; led++){
        leds[led] = CRGB::White;
      }
      for (int led = 13; led < 27; led++){
        leds[led] = CRGB::Red;
      }
      for (int led = 38; led < 50; led++){
        leds[led] = CRGB::Red;
      }
      FastLED.setBrightness(STATIC_BRIGHTNESS);
      FastLED.show();
    } else if (Mode == 5){
      for (int led = 0; led < 20; led++){
        leds[led] = CRGB::Green;
      }
      for (int led = 20; led < 44; led++){
        leds[led] = CRGB::Red;
      }
      for (int led = 44; led < 50; led++){
        leds[led] = CRGB::Green;
      }
      FastLED.show();
    } else if (Mode == 7){
      LEDColor[0] = 100;
      LEDColor[1] = 10;
      LEDColor[2] = 123;
      fill_solid(leds, LED_COUNT, CRGB(LEDColor[0], LEDColor[1], LEDColor[2]));
      FastLED.show();
    }
    LastInterruptTime = InterruptTime;
  }
}

// Input a value 0 to 255 to set LEDColor with a color value.
// The colours are a transition r - g - b - back to r.
void Wheel(byte Pos, byte pLEDColor[]) {
  Pos = 255 - Pos;
  if(Pos < 85) {
    pLEDColor[0] = 255 - Pos * 3;
    pLEDColor[1] = 0;
    pLEDColor[2] = Pos * 3;
  }
  else if(Pos < 170) {
    Pos -= 85;
    pLEDColor[0] = 0;
    pLEDColor[1] = Pos * 3;
    pLEDColor[2] = 255 - Pos * 3;
  } else {
    Pos -= 170;
    pLEDColor[0] = Pos * 3;
    pLEDColor[1] = 255 - Pos * 3;
    pLEDColor[2] = 0;
  }
}

// For dynamic modes, this is where the magic happens, sampling
// the envelope from the mic input and determining if it involved
// any crunchy bass.
void SampleAudio(){
  unsigned long time = micros();
  float sample, value, envelope, beat, thresh;
  unsigned char i;
  for(i = 0;i<=SAMPLE_PERIOD_US;++i){
    // Read ADC and center so +-512
    sample = (float)analogRead(MICROPHONE_PIN)-503.f;
    // Filter only bass component
    value = bassFilter(sample);
    
    // Take signal amplitude and filter
    if(value < 0)value=-value;
    envelope = envelopeFilter(value);
  }

  // Filter out repeating bass sounds 100 - 180bpm
  beat = beatFilter(envelope);

  // Threshold it
  thresh = 0.02f * 40;

  // If we are above threshold, light up LEDs
  if(beat > thresh) Brightness=GOT_ME_SOME_BASS;
  else Brightness=DEFAULT_BRIGHTNESS;

  // If necessary, consume excess clock cycles, to keep at 5000 hz
  for(unsigned long up = time+SAMPLE_PERIOD_US; time > 20 && time < up; time = micros());
}


//// Audio filtering routines

// 20 - 200hz Single Pole Bandpass IIR Filter
float bassFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 9.1f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7960060012f * yv[0]) + (1.7903124146f * yv[1]);
    return yv[2];
}

// 10hz Single Pole Lowpass IIR Filter
float envelopeFilter(float sample) { //10hz low pass
    static float xv[2] = {0,0}, yv[2] = {0,0};
    xv[0] = xv[1]; 
    xv[1] = sample / 160.f;
    yv[0] = yv[1]; 
    yv[1] = (xv[0] + xv[1]) + (0.9875119299f * yv[0]);
    return yv[1];
}

// 1.7 - 3.0hz Single Pole Bandpass IIR Filter
float beatFilter(float sample) {
    static float xv[3] = {0,0,0}, yv[3] = {0,0,0};
    xv[0] = xv[1]; xv[1] = xv[2]; 
    xv[2] = sample / 7.015f;
    yv[0] = yv[1]; yv[1] = yv[2]; 
    yv[2] = (xv[2] - xv[0])
        + (-0.7169861741f * yv[0]) + (1.4453653501f * yv[1]);
    return yv[2];
}
