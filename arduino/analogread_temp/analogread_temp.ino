

int sensorPin = A0;   
int sensorValue = 0; 
char incomingByte = 0;
String stringOne;

void setup() {
  
  Serial.begin(115200); 
}

void loop() {
   if (Serial.available() > 0) {
         incomingByte = Serial.read();
         if (incomingByte == 'r' ){           
            stringOne =  String(analogRead(sensorPin), DEC);
            Serial.print(stringOne);          
        }     
    }
    // delay(100);           
}
