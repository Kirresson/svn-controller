// to implement
// 1 long beep, no access
// 3 beeps, no response

// Libraries
#include <WiegandMulti.h>

// Define readers
WIEGANDMULTI wg_checkin;
WIEGANDMULTI wg_checkout;

String device = "";
int buzzerPin;
String api_response = "";
int ledPin_in = 8;
int ledPin_out = 9;
bool waitForResponse;
int response; // 1 = true, 2 = false, 3 = error
bool messageSent = false;
String Token;

void playBuzzer(String device, int count, int freq){

  if (device == "Check_IN") {
    buzzerPin = 6;
  } else if (device == "Check_OUT"){
    buzzerPin = 7;
  } else {
    buzzerPin = 10;
  }

  // Set pin mode for buzzer
  pinMode(buzzerPin, OUTPUT);

  // play negative tone, 100Hz 
  for (int i = 0; i <= count; i++){
    tone(buzzerPin, freq);
    delay(300);
    noTone(buzzerPin);
    delay(300);
  }

  // set pin back to input for protection
  pinMode(buzzerPin, INPUT);

}


void receiveResponse() {

  // Read the message
  api_response = Serial.readString();
  //Serial.println("Arduino received:" + api_response);
  waitForResponse = false;

  if (api_response.indexOf("heck_IN") > 0) {
    device = "Check_IN";
  } else if (api_response.indexOf("heck_OUT") > 0) {
    device = "Check_OUT";
  } else {
    device = "";
  }

  if (api_response.indexOf("True") > 0) {
    response = 1;
    printLCD2("Zugang gew\xE1hrt.");
  } else if (api_response.indexOf("False") > 0) {
    response = 2;
    printLCD2("Zugang verwehrt.");
  } else {
    response = 3;
    printLCD2("Error.          ");
  }

  if (device == ""){
    // do nothing
  } else {
    reactToResponse(device, response);
  }
  
}


void reactToResponse(String device, int response){

    if (response == 2) {
      playBuzzer(device, 1, 100);
    } else if (response == 3) {
      playBuzzer(device, 3, 100);
    } else if (response == 1) {
      playBuzzer(device, 0, 2000);
    }

  // reset device
  device = "";

  // set leds to green
  digitalWrite(ledPin_in, LOW);
  digitalWrite(ledPin_out, LOW);

}

void Reader1D0Interrupt(void)
{
  wg_checkin.ReadD0();
}

void Reader1D1Interrupt(void)
{
  wg_checkin.ReadD1();
}

void Reader2D0Interrupt(void)
{
  wg_checkout.ReadD0();
}

void Reader2D1Interrupt(void)
{
  wg_checkout.ReadD1();
}


void setup() {
  // Start serial
	Serial.begin(9600);

  // Start reader checkin on pin 2 and 3
	wg_checkin.begin(2,3,Reader1D0Interrupt,Reader1D1Interrupt);

  // Start reader checkout on pin 4 and 5
	wg_checkout.begin(4,5,Reader2D0Interrupt,Reader2D1Interrupt);

  // Set pin to green
  pinMode(ledPin_in, OUTPUT);
  pinMode(ledPin_out, OUTPUT);
  digitalWrite(ledPin_in, LOW);
  digitalWrite(ledPin_out, LOW);
  setup_lcd();
  printLCD1("Keine Karte     ");
  printLCD2("gescannt.       ");
}


void loop() {
  // Read checkin
	if(wg_checkin.available()) {
    // Turn off readers
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);

    // Set LED to red
    digitalWrite(ledPin_in, HIGH);
    digitalWrite(ledPin_out, HIGH);

    device = "Check_IN";
    Token = String(wg_checkin.getCode(),HEX);

		// Print to python
    Serial.println("Card;"+device+";"+Token);

    // Print to LCD
    printLCD1("Scanned:" +Token);
    clearRow(2);

    messageSent = true;
	}
	
  // Read checkout
	if(wg_checkout.available())	{
    // Turn off readers
    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);

    // Set LED to red
    digitalWrite(ledPin_out, HIGH);
    digitalWrite(ledPin_in, HIGH);
    
    device = "Check_OUT";
    Token = String(wg_checkout.getCode(),HEX);

		// Print to python
    Serial.println("Card;"+device+";"+Token);

		// Print to LCD
    printLCD1("Scanned:" +Token);
    clearRow(2);

    messageSent = true;
	}

  // If the python script responded, give the user feedback with the LED and buzzer
  if (messageSent == true){

      //Wait for response
      waitForResponse = true;
      printLCD2("ID wird gepr\365ft.");

      unsigned long startingMillis = millis();

        while (waitForResponse == true) {
          if (millis() - startingMillis >= 5000){

            waitForResponse = false;
            playBuzzer(device, 3, 100);
            printLCD2("Keine Antwort. ");

          } else {

            if (Serial.available() > 0) {
            
            receiveResponse();
            
            }
          }
        }
        messageSent = false;
        device = "";
        // turn led green
        digitalWrite(ledPin_in, LOW);
        digitalWrite(ledPin_out, LOW);
        
        //Start RFID reader again
        // Serial.println("Finished");
        pinMode(2, INPUT);
        pinMode(3, INPUT);
        pinMode(4, INPUT);
        pinMode(5, INPUT);
        delay(10);
        wg_checkin.begin(2,3,Reader1D0Interrupt,Reader1D1Interrupt);
        wg_checkout.begin(4,5,Reader2D0Interrupt,Reader2D1Interrupt);
        delay(10);
  }
}
