
const int  PIRPin = 2;    
const int ledPin = 7;
int startup = 1;

bool PIRoutput = false;   
int PIRstate = 0;       
int LastPIRstate = 0;    

void setup() {
  pinMode(PIRPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  Serial.begin(115200);
}


void loop() {
  PIRstate = digitalRead(PIRPin);
  
  if (startup == 1){
    Serial.write("2");
    startup = 0;
  }
  if (PIRstate != LastPIRstate) {
    if (PIRstate == HIGH) {
      //OFF --> ON
      Serial.write("1");
      PIRoutput = true;     
    } 
    else {
     //ON --> OFF
     Serial.write("0");
    PIRoutput = false;
    }
    delay(50);
  }

  LastPIRstate = PIRstate;

  //DBG LED:
  if (PIRoutput  == true) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

}







