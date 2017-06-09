
#include <Time.h>  


int bufferpins[] = {2,3,4,5,6,7,8,13};
int demuxpins[] = {9,10,11,12};


class Headers{
  public:
    static const byte TIME_REQUEST=1;
    static const byte TIME_HEADER=2;
    static const byte TIME_SET=3;
};


void setup() {
  Serial.begin(9600); //Init serial port
  setSyncProvider( requestSync);  //set function to call when sync required
  for (int i=0; i>8 ;i++) {
    pinMode(bufferpins[i], INPUT_PULLUP);
  }
  for (int i=0; i<4 ;i++) {
    pinMode(demuxpins[i], OUTPUT);
  }
}

void loop() {
  if (Serial.available()) {
    if (timeStatus()!=timeSet) { 
      processSyncMessage();
     
      if (timeStatus()==timeSet) {
        Serial.write(Headers::TIME_SET);  
      }
    }
    else {
      for (int i=0; i<16 ;i++) {
        for (int j=0; j<4 ;j++) {
          digitalWrite(demuxpins[j], HIGH && (i & (1 << (3 - j))));
        }
        for (int j=0; j<8 ;j++) {
          sensorDisplay(getHoleNumber(i,j), digitalRead(bufferpins[j]), digitalRead(bufferpins[j+1]));
          j++;
        }
      }
    }
  }
}

int getSensorNumber(int i, int j) {
  return j + (i << 3);
}

int getHoleNumber(int i, int j) {
  
  return (int)(getSensorNumber(i,j)/2) ;
    
}

void sensorDisplay(int n, int val1, int val2) {
  if (n < 10) Serial.print("0");
  Serial.print(n); //2
  Serial.print((val1 << 1) + val2); // 1
  Serial.print(now()); // 10
  Serial.print("\n");
}


void processSyncMessage() {
  unsigned long pctime = 0;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013
  byte header = Serial.read();
  if(header == Headers::TIME_HEADER) {
     pctime = Serial.parseInt();
     if( pctime >= DEFAULT_TIME) { // check the integer is a valid time (greater than Jan 1 2013)
       setTime(pctime); // Sync Arduino clock to the time received on the serial port
     }
  }
}

time_t requestSync()
{
  Serial.write(Headers::TIME_REQUEST);  
  return 0; // the time will be sent later in response to serial mesg
}




