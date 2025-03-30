#include <IRremote.h>


#define FLOTING 777
#define TVMOTOR LOW
#define DOORMOTOR HIGH
#define BOXON LOW
#define BOXOFF HIGH
#define OPEN HIGH
#define CLOSE LOW
#define LIMITPRESSEDSTATE LOW
#define DEBOUNCELIMIT 120

//limits pins
const int dooropenl = 3;
const int doorclosel = 2;
const int tvopenl = 5;
const int tvclosel = 4;
const int buttonPin = 10;  //LOW on, HIGH, off  // the number of the pushbutton pin
const int openmaint = 6;   //buton for maintenance c
const int powerforopenmaint = 7;
const int IR_PIN = 8;
//constants
// motor pins
const int runbox = 14;    //HIGH for starting default HIGH
const int polarity = 15;  //HIGH for open
const int motorselect = 16;
const int security = 70;  // time inbetween the digitalWrite()s for stability

//IR receiver
IRrecv irrecv(IR_PIN);



// Variables will change:
int lastButtonState = LOW;
int lastIrState = LOW;
int lastWifiState = LOW;
int readingWifi;
int readingIr;
int reading = LOW;  //the current reading for the input
int maint;    //the current reading for the maintenance button



void setup() {
  //limits
  pinMode(dooropenl, INPUT_PULLUP);
  pinMode(doorclosel, INPUT_PULLUP);
  pinMode(tvopenl, INPUT_PULLUP);
  pinMode(tvclosel, INPUT_PULLUP);


  //inputs
  pinMode(openmaint, INPUT_PULLUP);
  pinMode(buttonPin, INPUT);

  //outputs
  pinMode(runbox, OUTPUT);
  pinMode(polarity, OUTPUT);
  pinMode(motorselect, OUTPUT);
  pinMode(powerforopenmaint, OUTPUT);  //used as a sink for current for the button openmaint


  digitalWrite(powerforopenmaint, LOW);  //used as a sink for current for the button openmaint


  digitalWrite(runbox, BOXOFF);  //turn the box off at the start for safety


  //Serial for testing and troubleshouting
  //Serial.begin(115200);
  Serial.begin(115200);
  irrecv.enableIRIn();
}


void opendoor() {
  return action(DOORMOTOR, OPEN, dooropenl);
}

void closedoor() {
  return action(DOORMOTOR, CLOSE, doorclosel);
}

void opentv() {
  return action(TVMOTOR, OPEN, tvopenl);
}

void closetv() {
  return action(TVMOTOR, CLOSE, tvclosel);
}

void action(int motor, int openclose, int limit) {  //motor LOW tv HIGH door, limit to check
  int state = !LIMITPRESSEDSTATE;
  //int state = HIGH;
  digitalWrite(runbox, BOXOFF);
  delay(security);
  digitalWrite(motorselect, motor);
  delay(security);
  digitalWrite(polarity, openclose);
  delay(security);
  digitalWrite(runbox, BOXON);
  while (state != LIMITPRESSEDSTATE) {  // wait until the limit is pressed
    state = debounce(limit, DEBOUNCELIMIT);
    //Serial.println(state);
  }
  digitalWrite(runbox, BOXOFF);

  return;
}

void stopall() {
  Serial.println("stoped");
  Serial.println();
  for (;;) {}
}

int debounce(int pin, int debouncelim) {
  int detect = 0;
  //int state = 0;
  for (int i = 0; i < debouncelim; i++) {
    if (digitalRead(pin) == HIGH) {
      detect = detect + 1;
    } else {
      detect = detect - 1;
    }
    // Serial.print(pin);
    // Serial.print("detected = ");
    //Serial.println(detect);

    delay(1);
  }

  if (detect > debouncelim / 2) {
    return 1;
  } else if (-detect >= debouncelim / 2) {
    return 0;
  }
}



// the debounce time; increase if the output flickers
/*
void loop1(){
if (irrecv.decode()) {
    // Print the HEX value of the button press
    Serial.println(irrecv.decodedIRData.decodedRawData, DEC);
    switch(irrecv.decodedIRData.decodedRawData){
      case 3091726080:
      Serial.println("key pressed: CH+");
      break;
      case 3108437760:
      Serial.println("key pressed: CH");
      break;
      case 3125149440:
      Serial.println("key pressed: CH-");
      break;
      default:
      break;
    }
    // Reset the IR receiver for the next signal
    irrecv.resume();
  }

}
*/
void loop() {

  if (irrecv.decode()) {

    Serial.println(irrecv.decodedIRData.decodedRawData, DEC);
    switch (irrecv.decodedIRData.decodedRawData) {
      case 3091726080:
        Serial.println("key pressed: CH+");
        readingIr = !readingIr;

        break;
      /*
      case 3108437760:
      Serial.println("key pressed: CH");
      break;
      case 3125149440:
      Serial.println("key pressed: CH-");
      break;
      */
      default:
        break;
    }




    irrecv.resume();

  } else {

    maint = debounce(openmaint, DEBOUNCELIMIT);
    readingWifi = debounce(buttonPin, DEBOUNCELIMIT);
  }
  if (maint == LOW) {  //if in maintement mode
    opendoor();
    opentv();

    while (maint == LOW) {  // keep itself in mainteneance mode as long as the button is on
      maint = debounce(openmaint, DEBOUNCELIMIT);
      //Serial.print("maintenace = ");
      //Serial.println(maint);
    }
    lastWifiState = !readingWifi;
    lastIrState = !readingIr;  // so  the next if statement is true and the door returns to the state it was previously in.
  }


  if(readingWifi != lastWifiState || readingIr != lastIrState){
    reading = !reading;

  }
  // if the button state has changed:
  if (reading != lastButtonState) {


    // only toggle the LED if the new button state is HIGH
    if (reading == LOW) {
      opendoor();
      opentv();
      closedoor();
      Serial.println("open");

    } else {
      opendoor();
      closetv();
      closedoor();
      Serial.println("closed");
    }
  }




  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
  lastWifiState = readingWifi;
  lastIrState = readingIr; 

  delay(1);
}
