#define stepU 2
#define dirU 3
#define stepD 4
#define dirD 5
#define shoot 8
#define servoPin 9
#define xSen 6
#define ms1 11
#define ms2 12
#define ms3 13


#include <Servo.h>
#include <MPU9255.h>
float y=0, nextY=0;
float x=0, nextX=0;
char serialBuffer[10];
Servo serwomechanizm;
MPU9255 mpu;
float krok = 0.25 * 0.9;
int stepDelay = 500;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  serwomechanizm.attach(servoPin);
  mpu.init();
  pinMode(shoot, OUTPUT);
  pinMode(stepU,OUTPUT);
  pinMode(dirU,OUTPUT);
  pinMode(stepD,OUTPUT);
  pinMode(dirD,OUTPUT);
  pinMode(ms1,OUTPUT);
  pinMode(ms2,OUTPUT);
  pinMode(ms3,OUTPUT);
  pinMode(xSen,INPUT_PULLUP);

  digitalWrite(ms2,HIGH);
  digitalWrite(ms1,LOW);
  digitalWrite(ms3,LOW);
  
  digitalWrite(shoot,HIGH);
  
  digitalWrite(stepU,LOW);
  digitalWrite(stepD,LOW);
  digitalWrite(dirU,LOW);
  digitalWrite(dirD,LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available()>=10) {
    Serial.readBytes(serialBuffer,10);
    
    if(serialBuffer[0] == 0)
    {
      ((uint8_t*)&nextX)[0]=serialBuffer[1];
      ((uint8_t*)&nextX)[1]=serialBuffer[2];
      ((uint8_t*)&nextX)[2]=serialBuffer[3];
      ((uint8_t*)&nextX)[3]=serialBuffer[4];
      ((uint8_t*)&nextY)[0]=serialBuffer[5];
      ((uint8_t*)&nextY)[1]=serialBuffer[6];
      ((uint8_t*)&nextY)[2]=serialBuffer[7];
      ((uint8_t*)&nextY)[3]=serialBuffer[8];
      if(nextX<-100 || nextX>100 || nextY<-100 || nextY>100)
      {
        nextX = x;
        nextY = y;
      }
    }
    else if(serialBuffer[0] == 1)
    {
      digitalWrite(shoot,LOW);
      delay(225);
      digitalWrite(shoot,HIGH);
    }
    else if(serialBuffer[0] == 2)
    {
      if(checkReady())
      {
        Serial.write(255);
      }
      else
      {
        Serial.write(0);
      }
    }
    else if(serialBuffer[0] == 3)
    {
      float offsetX, offsetY;
      ((uint8_t*)&offsetX)[0]=serialBuffer[1];
      ((uint8_t*)&offsetX)[1]=serialBuffer[2];
      ((uint8_t*)&offsetX)[2]=serialBuffer[3];
      ((uint8_t*)&offsetX)[3]=serialBuffer[4];
      ((uint8_t*)&offsetY)[0]=serialBuffer[5];
      ((uint8_t*)&offsetY)[1]=serialBuffer[6];
      ((uint8_t*)&offsetY)[2]=serialBuffer[7];
      ((uint8_t*)&offsetY)[3]=serialBuffer[8];
      if(offsetX>-100 && offsetX<100 && offsetY>-100 && offsetY<100)
      {
        x-=offsetX;
        y-=offsetY;
      }
    }
    else if(serialBuffer[0] == 4)
    {
      int nextDelay;
      ((uint8_t*)&nextDelay)[0]=serialBuffer[1];
      ((uint8_t*)&nextDelay)[1]=serialBuffer[2];
      if(nextDelay>0)
      {
        stepDelay = nextDelay;
      }
    }
    else if(serialBuffer[0] == 5)
    {
      if(serialBuffer[1] == 1)
      {
        digitalWrite(ms1,LOW);
        digitalWrite(ms2,LOW);
        digitalWrite(ms3,LOW);
        krok = 0.9;
      }
      else if(serialBuffer[1] == 2)
      {
        digitalWrite(ms1,HIGH);
        digitalWrite(ms2,LOW);
        digitalWrite(ms3,LOW);
        krok = 0.9 * 0.5;
      }
      else if(serialBuffer[1] == 4)
      {
        digitalWrite(ms1,LOW);
        digitalWrite(ms2,HIGH);
        digitalWrite(ms3,LOW);
        krok = 0.9 * 0.25;
      }
      else if(serialBuffer[1] == 8)
      {
        digitalWrite(ms1,HIGH);
        digitalWrite(ms2,HIGH);
        digitalWrite(ms3,LOW);
        krok = 0.9 * 0.125;
      }
      else if(serialBuffer[1] == 16)
      {
        digitalWrite(ms1,HIGH);
        digitalWrite(ms2,HIGH);
        digitalWrite(ms3,HIGH);
        krok = 0.9 * 0.0625;
      }
    }
    else if(serialBuffer[0] == 6)
    {
      configuration();
    }
  }

  if(x-krok > nextX)
  {
    digitalWrite(dirD,HIGH);
    digitalWrite(stepD,HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepD,LOW);
    delayMicroseconds(stepDelay);
    x-=krok;
  }
  if(y-krok > nextY)
  {
    digitalWrite(dirU,LOW);
    digitalWrite(stepU,HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepU,LOW);
    delayMicroseconds(stepDelay);
    y-=krok;
  }
  if(x+krok < nextX)
  {
    digitalWrite(dirD,LOW);
    digitalWrite(stepD,HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepD,LOW);
    delayMicroseconds(stepDelay);
    x+=krok;
  }
  if(y+krok < nextY)
  {
    digitalWrite(dirU,HIGH);
    digitalWrite(stepU,HIGH);
    delayMicroseconds(stepDelay);
    digitalWrite(stepU,LOW);
    delayMicroseconds(stepDelay);
    y+=krok;
  }  
}

bool checkReady()
{
  if(abs(x-nextX)<krok && abs(y-nextY)<krok) return true;
  return false;
}


void configuration()
{
  digitalWrite(ms1,HIGH);
  digitalWrite(ms2,HIGH);
  digitalWrite(ms3,HIGH);
  delay(1000);
  serwomechanizm.write(180);
  delay(1000);
  while(digitalRead(xSen)==HIGH)
  {
    digitalWrite(dirD,HIGH);
    digitalWrite(stepD,HIGH);
    delayMicroseconds(2000);
    digitalWrite(stepD,LOW);
    delayMicroseconds(2000);
  }
  serwomechanizm.write(0);
  delay(1000);
  mpu.read_acc();
  while(mpu.ay/16384.0>0)
  {
    digitalWrite(dirU,HIGH);
    digitalWrite(stepU,HIGH);
    delayMicroseconds(12000);
    digitalWrite(stepU,LOW);
    delayMicroseconds(12000);
    mpu.read_acc();
  }
  delay(1000);
  digitalWrite(ms1,LOW);
  digitalWrite(ms2,HIGH);
  digitalWrite(ms3,LOW);
  krok = 0.9 * 0.25;
  stepDelay = 500;
  x=0;
  y=0; 
}
