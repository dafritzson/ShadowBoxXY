#include <FastLED.h>
#include <boarddefs.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <ir_Lego_PF_BitStreamEncoder.h>

/*RGB LEDs definitions*/
#define LED_PIN     5
#define NUM_LEDS    150
#define BRIGHTNESS  64
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
CRGB leds[NUM_LEDS];
CRGB backgroundColor = CRGB(0, 0, 0);

uint8_t numRows = 10;
uint8_t numCols = 13;
uint8_t locationNum = 0;

/*IR definitions*/
int IR_PIN = 11;
double last_value = 0;

/*Hotspot Globals*/
uint8_t damLEDS[][2] = {{1, 4}, {1, 5}};                                                                                    /*Location 1*/
uint8_t houseLEDS[][2] = {{3, 4}, {4, 4}};                                                                                  /*Location 2*/
uint8_t wispLEDS[][2] = {{2, 6}, {3, 6}, {2, 7}, {3, 7}};                                                                   /*Location 3*/
uint8_t brendasLEDS[][2] = {{4, 5}, {5, 5}, {6, 5}};                                                                        /*Location 4*/
uint8_t parkLEDS[][2] = {{8, 5}, {8, 6}, {9, 5}, {9, 6}};                                                                   /*Location 5*/
uint8_t walmartLEDS[][2] = {{0, 0}, {0, 1}, {1, 0}, {1, 1}, {2, 0}, {2, 1}};                                                /*Location 6*/
uint8_t southeastLEDS[][2] = {{7, 2}, {7, 3}, {8, 2}, {8, 3}, {8, 4}, {9, 2}, {9, 3}, {10, 2}, {11, 2}};                    /*Location 7*/


IRrecv irrecv(IR_PIN);
decode_results results;

void setup(){
  Serial.begin(9600);
  irrecv.enableIRIn(); // Start the receiver
  
  delay( 3000 ); // power-up safety delay
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(  BRIGHTNESS );
}

uint8_t XYtoIndex ( uint8_t x, uint8_t y, uint8_t LEDindex){
  if ((x % 2) == 0){
    LEDindex = x*numRows + y;
  }
  else{
    LEDindex = (x + 1)*numRows - y - 1;
  }

  return LEDindex;
}

double scanIR(){
  double button_value = 0;
  if (irrecv.decode(&results))
    {
     Serial.println(results.value, HEX);
     Serial.println(results.value);
     irrecv.resume(); // Receive the next value
    }
  button_value = results.value;
  results.value = 0;
  return button_value;
}

uint8_t parseLEDGroup ( uint8_t XYarray[][2], uint8_t numRows, uint8_t LEDindex, byte *LEDarray){
  uint8_t arrayIndex = 0;
  for (uint8_t i = 0; i < numRows; i++){
      LEDindex = XYtoIndex(XYarray[i][0], XYarray[i][1], LEDindex);
      LEDarray[arrayIndex] = LEDindex;
      arrayIndex++;
    }
  return arrayIndex;
}

void setBackgroundColor(){
  fill_solid(leds, NUM_LEDS, backgroundColor);
}

double interpretIR( double button_value, bool new_IR){
  uint8_t LEDindex = 0;
  byte LEDarray[150] = {};
  uint8_t arrayLength = 0;
  uint8_t numRows = 0;

  if (button_value == 16732845){
    /*White button. Scrolls through locations as you press it*/
    locationNum++;
    if (locationNum > 7){
      locationNum = 1;
    }
    Serial.println(locationNum);
    switch (locationNum){
      case 1: 
        button_value = 16769055;
        break;
      case 2: 
        button_value = 16720605;
        break;
      case 3: 
        button_value = 16712445;
        break;
      case 4: 
        button_value = 16754775;
        break;
      case 5: 
        button_value = 16730805;
        break;
      case 6:
        /*SE portion of the lake. Doesn't have an independent button assigned to it.*/
        numRows = sizeof(southeastLEDS)/sizeof(southeastLEDS[0]);
        arrayLength = parseLEDGroup(southeastLEDS, numRows, LEDindex, &LEDarray[0]);
        /*Reset all lights to background color*/
        setBackgroundColor();
        /*Light up location*/
        for (uint8_t k = 0; k < 100; k++){
          FastLED.setBrightness(k);
          for (uint8_t i = 0; i < arrayLength; i++){
            leds[LEDarray[i]] = CRGB(255, 150, 20);
          }
          FastLED.show();
          delay(10);
        } 
        break;
     case 7: 
      button_value = 16728765;
      break;
    }
  }
  
  if (button_value == 16753245){
    /*On button*/
    backgroundColor = CRGB(255, 150, 20);
    setBackgroundColor();
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      FastLED.show();
      delay(10);  
    }
  } 
  
  else if (button_value == 16769565){
    /*Off button*/
    for (uint8_t i = 0; i < NUM_LEDS; i++){
      leds[i] = CRGB(0, 0, 0);
    }
    FastLED.show();
    FastLED.clear();
    locationNum = 0;
    backgroundColor = CRGB(0, 0, 0);
  }  
  
  else if (button_value == 16712445){
    /*M+ button*/
    numRows = sizeof(wispLEDS)/sizeof(wispLEDS[0]);
    arrayLength = parseLEDGroup(wispLEDS, numRows, LEDindex, &LEDarray[0]);
    /*Reset all lights to background color*/
    setBackgroundColor();
    /*Light up location*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      for (uint8_t i = 0; i < arrayLength; i++){
        leds[LEDarray[i]] = CRGB(255, 150, 20);
      }
      FastLED.show();
      delay(10);
    }  
  }
  
  else if (button_value == 16754775){
    /*M- button*/
    numRows = sizeof(brendasLEDS)/sizeof(brendasLEDS[0]);
    arrayLength = parseLEDGroup(brendasLEDS, numRows, LEDindex, &LEDarray[0]);
    /*Reset all lights to background color*/
    setBackgroundColor();
    /*Light up location*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      for (uint8_t i = 0; i < arrayLength; i++){
        leds[LEDarray[i]] = CRGB(255, 150, 20);
      }
      FastLED.show();
      delay(10);
    }  
  }
  else if (button_value == 16720605){
    /*S+ button*/
    numRows = sizeof(houseLEDS)/sizeof(houseLEDS[0]);
    arrayLength = parseLEDGroup(houseLEDS, numRows, LEDindex, &LEDarray[0]);
    /*Reset all lights to background color*/
    setBackgroundColor();
    /*Light up location*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      for (uint8_t i = 0; i < arrayLength; i++){
        leds[LEDarray[i]] = CRGB(255, 150, 20);
      }
      FastLED.show();
      delay(10);
    }  
  }
  else if (button_value == 16769055){
    /*S- button*/
    numRows = sizeof(damLEDS)/sizeof(damLEDS[0]);
    arrayLength = parseLEDGroup(damLEDS, numRows, LEDindex, &LEDarray[0]);
    /*Reset all lights to background color*/
    setBackgroundColor();
    /*Light up location*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      for (uint8_t i = 0; i < arrayLength; i++){
        leds[LEDarray[i]] = CRGB(255, 150, 20);
      }
      FastLED.show();
      delay(10);
    } 
  }
  else if (button_value == 16761405){
    /*Brightness up button*/
    uint8_t brightness = FastLED.getBrightness();
    if (brightness < 100){
      FastLED.setBrightness(brightness + 10);
    }

    if (brightness > 100){
      FastLED.setBrightness(100);
    }
    FastLED.show();
  }
  else if (button_value == 16748655){
    /*Brightness down button*/
    uint8_t brightness = FastLED.getBrightness();
    if (brightness > 0 ){
      FastLED.setBrightness(brightness - 10);
    }
    /*unsigned integer so check for rollover*/
    if (brightness > 100){
      brightness = 0;
    }
    FastLED.show();
  }
  else if (button_value == 16738455){
    /*Red button*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(255, 0, 0);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }
  }
  else if (button_value == 16750695){
    /*Green button*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(0, 255, 0);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }
  }
  else if (button_value == 16756815){
    /*Blue button*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(0, 0, 255);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }
  }
  else if (button_value == 16724175){
    /*Yellow button*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(127, 127, 0);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }
  }
  else if (button_value == 16718055){
    /*Mustard button*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(127, 82, 0);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }
  }
  else if (button_value == 16743045){
    /*Orange button*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(200, 82, 0);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }
  }
  else if (button_value == 16716015){
    /*Peach button*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(145, 82, 0);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }
  }
  else if (button_value == 16726215){
    /*Cyan button*/
    int hueMin = 140;
    int hueMax = 162;
    int hueAvg = (hueMin + hueMax)/2;
    int satMin = 10;
    int satMax = 200;
    int satAvg = (satMin + satMax)/2;
    int brightMin = 20;
    int brightMax = 100;
    int brightAvg = (brightMin + brightMax)/2;

    /*This button would simulate a water effect, but relies upon HSV instead of RGB mode, so the rest of the functionality would have to be rewritten*/
//    if (new_IR){
//      for (uint8_t i = 0; i < NUM_LEDS; i++){
//        long randBlue = random(hueMin, hueMax);
//        long randSat = random(satMin, satMax);
//        long randBright = random(brightMin, brightMax);
//        leds[i] = CHSV(randBlue, randSat, randBright);
//      }
//    }
//    
//    for (uint8_t i = 0; i < NUM_LEDS; i++){
//      long randChange = random(0, 10);
//
//      /*Randomly tweak the hue*/
//      if (leds[i].hue > hueAvg){
//        leds[i].hue = leds[i].hue - randChange;
//      }
//      else {
//        leds[i].hue = leds[i].hue + randChange;
//      }
//
//      /*Randomly tweak the saturation*/
//      randChange = random(0, 80);
//      if (leds[i].sat > satAvg){
//        leds[i].sat = leds[i].sat - randChange;
//      }
//      else{
//        leds[i].sat = leds[i].sat + randChange;
//      }
//
//      /*Randomly tweak the brightness*/
//      randChange = random(0, 40);
//      if (leds[i].val > brightAvg){
//        leds[i].val = leds[i].val - randChange;
//      }
//      else {
//        leds[i].val = leds[i].val + randChange;
//      }
//    }
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(0, 127, 127);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }

    
    
  }
  else if (button_value == 16734885){
    /*Violet button*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      backgroundColor = CRGB(119, 65, 150);
      setBackgroundColor();
      FastLED.show();
      delay(10);  
    }
  }
  else if (button_value == 16728765){
    /*Magenta button*/
    numRows = sizeof(walmartLEDS)/sizeof(walmartLEDS[0]);
    arrayLength = parseLEDGroup(walmartLEDS, numRows, LEDindex, &LEDarray[0]);
    /*Reset all lights to background color*/
    setBackgroundColor();
    /*Light up location*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      for (uint8_t i = 0; i < arrayLength; i++){
        leds[LEDarray[i]] = CRGB(255, 150, 20);
      }
      FastLED.show();
      delay(10);
    } 
  }
  else if (button_value == 16730805){
    /*Forest button*/
    numRows = sizeof(parkLEDS)/sizeof(parkLEDS[0]);
    arrayLength = parseLEDGroup(parkLEDS, numRows, LEDindex, &LEDarray[0]);
    /*Reset all lights to background color*/
    setBackgroundColor();
    /*Light up location*/
    for (uint8_t k = 0; k < 100; k++){
      FastLED.setBrightness(k);
      for (uint8_t i = 0; i < arrayLength; i++){
        leds[LEDarray[i]] = CRGB(255, 150, 20);
      }
      FastLED.show();
      delay(10);
    } 
  }
  else if (button_value == 16736925){ 
    /*Auto button*/
    int locationNum = 1;
    for (int n = 1; n < 8; n++){
      backgroundColor = CRGB(0, 0, 0);
      setBackgroundColor();
      switch (locationNum){
        case 1:
          numRows = sizeof(damLEDS)/sizeof(damLEDS[0]);
          arrayLength = parseLEDGroup(damLEDS, numRows, LEDindex, &LEDarray[0]);
          break;
        case 2:
          numRows = sizeof(houseLEDS)/sizeof(houseLEDS[0]);
          arrayLength = parseLEDGroup(houseLEDS, numRows, LEDindex, &LEDarray[0]);
          break;
        case 3:
          numRows = sizeof(wispLEDS)/sizeof(wispLEDS[0]);
          arrayLength = parseLEDGroup(wispLEDS, numRows, LEDindex, &LEDarray[0]);
          break;
        case 4:
          numRows = sizeof(brendasLEDS)/sizeof(brendasLEDS[0]);
          arrayLength = parseLEDGroup(brendasLEDS, numRows, LEDindex, &LEDarray[0]);
          break;
        case 5:
          numRows = sizeof(parkLEDS)/sizeof(parkLEDS[0]);
          arrayLength = parseLEDGroup(parkLEDS, numRows, LEDindex, &LEDarray[0]);
          break;
        case 6:
          numRows = sizeof(southeastLEDS)/sizeof(southeastLEDS[0]);
          arrayLength = parseLEDGroup(southeastLEDS, numRows, LEDindex, &LEDarray[0]);
          break;
        case 7:
          numRows = sizeof(walmartLEDS)/sizeof(walmartLEDS[0]);
          arrayLength = parseLEDGroup(walmartLEDS, numRows, LEDindex, &LEDarray[0]);
          break;
          
      locationNum++;   
      }
      for (uint8_t k = 0; k < 100; k++){
        FastLED.setBrightness(k);
        for (uint8_t i = 0; i < arrayLength; i++){
          leds[LEDarray[i]] = CRGB(255, 150, 20);
        }
        FastLED.show();
        delay(10);
      }
      delay(3000);
    }
    
  }

  /*Want the button_value to be passed back out to the main loop if in Auto or Cyan mode*/
  if (button_value != 16726215 && button_value != 16736925){
    button_value = 0;
  }
  
  return button_value;
}

void loop(){
  double IR_value = 0;
  double button_value;
  bool new_IR = false;
  
  IR_value = scanIR();
  if (IR_value > 0 && IR_value < 20000000){
    button_value = IR_value;
    new_IR = true;
  }
  if (button_value > 0){
    setBackgroundColor();
    button_value = interpretIR(button_value, new_IR);
  }
}
