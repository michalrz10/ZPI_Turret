#define stepU 2
#define dirU 3
#define stepD 4
#define dirD 5
#define shoot 7
#define servoPin 9
#define servoPower 10
#define xSen 6
#define ms1 11
#define ms2 12
#define ms3 13


#include <Servo.h>
#include <MPU9255.h>
#include <Kalman.h>
float y=0, nextY=0;
float x=0, nextX=0;
char serialBuffer[10];
Servo serwomechanizm;
MPU9255 mpu;
float krok = 0.9 * 0.0625;
int stepDelay = 250;
bool actionshoot = false;
bool actionconfig = false;
Kalman kalmanX;
Kalman kalmanY;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  serwomechanizm.attach(servoPin);
  mpu.init();
  mpu.set_acc_scale(scale_2g);
  mpu.set_gyro_scale(scale_250dps);
  pinMode(shoot, OUTPUT);
  pinMode(servoPower,OUTPUT);
  pinMode(stepU,OUTPUT);
  pinMode(dirU,OUTPUT);
  pinMode(stepD,OUTPUT);
  pinMode(dirD,OUTPUT);
  pinMode(ms1,OUTPUT);
  pinMode(ms2,OUTPUT);
  pinMode(ms3,OUTPUT);
  pinMode(xSen,INPUT_PULLUP);

  digitalWrite(ms2,HIGH);
  digitalWrite(ms1,HIGH);
  digitalWrite(ms3,HIGH);
  
  digitalWrite(shoot,HIGH);
  
  digitalWrite(stepU,LOW);
  digitalWrite(stepD,LOW);
  digitalWrite(dirU,LOW);
  digitalWrite(dirD,LOW);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  readData();

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
  if(actionconfig)
  {
    configuration();
    actionconfig = false;
  }
  if(actionshoot)
  {
    digitalWrite(shoot,LOW);
    delay(300);
    digitalWrite(shoot,HIGH);
    actionshoot = false;
  }
}

void readData()
{
  while (Serial.available()>=16) {
    Serial.readBytes(serialBuffer,16);
    
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
      actionshoot = true;
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
      actionconfig = true;
    }
  }
}

bool checkReady()
{
  if(abs(x-nextX)<krok && abs(y-nextY)<krok) return true;
  return false;
}


void configuration()
{
  digitalWrite(servoPower,HIGH);
  digitalWrite(ms1,HIGH);
  digitalWrite(ms2,HIGH);
  digitalWrite(ms3,HIGH);
  for(int i=0; i<800; i++)
  {
    digitalWrite(dirD,LOW);
    digitalWrite(stepD,HIGH);
    delayMicroseconds(300);
    digitalWrite(stepD,LOW);
    delayMicroseconds(300);
  }
  readData();
  serwomechanizm.write(180);
  delay(1000);
  while(digitalRead(xSen)==HIGH)
  {
    digitalWrite(dirD,HIGH);
    digitalWrite(stepD,HIGH);
    delayMicroseconds(300);
    digitalWrite(stepD,LOW);
    delayMicroseconds(300);
  }
  serwomechanizm.write(0);
  delay(1000);
  readData();
  double accX, accY, accZ;
  double gyroX, gyroY, gyroZ;
  
  double gyroXangle, gyroYangle; // Angle calculate using the gyro only
  double compAngleX, compAngleY; // Calculated angle using a complementary filter
  double kalAngleX, kalAngleY; // Calculated angle using a Kalman filter

  uint32_t timer;
  mpu.read_acc();
  accX = mpu.ax;
  accY = mpu.ay;
  accZ = mpu.az;
  double roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
  double pitch = atan2(-accX, accZ) * RAD_TO_DEG;
  kalmanX.setAngle(roll); // Set starting angle
  kalmanY.setAngle(pitch);
  gyroXangle = roll;
  gyroYangle = pitch;
  compAngleX = roll;
  compAngleY = pitch;
  timer = micros();
  while(abs(compAngleX)>0.01)
  {
    mpu.read_acc();
    mpu.read_gyro();
    accX = mpu.ax;
    accY = mpu.ay;
    accZ = mpu.az;
    gyroX = mpu.gx;
    gyroY = mpu.gy;
    gyroZ = mpu.gz;
    double dt = (double)(micros() - timer) / 1000000;
    timer = micros();
    roll  = atan(accY / sqrt(accX * accX + accZ * accZ)) * RAD_TO_DEG;
    pitch = atan2(-accX, accZ) * RAD_TO_DEG;
    double gyroXrate = gyroX / 131.0; // Convert to deg/s
    double gyroYrate = gyroY / 131.0; // Convert to deg/s
    if ((pitch < -90 && kalAngleY > 90) || (pitch > 90 && kalAngleY < -90)) {
    kalmanY.setAngle(pitch);
    compAngleY = pitch;
    kalAngleY = pitch;
    gyroYangle = pitch;
  } else
    kalAngleY = kalmanY.getAngle(pitch, gyroYrate, dt); // Calculate the angle using a Kalman filter

  if (abs(kalAngleY) > 90)
    gyroXrate = -gyroXrate; // Invert rate, so it fits the restriced accelerometer reading
  kalAngleX = kalmanX.getAngle(roll, gyroXrate, dt); // Calculate the angle using a Kalman filter
    gyroXangle += gyroXrate * dt; // Calculate gyro angle without any filter
  gyroYangle += gyroYrate * dt;
  //gyroXangle += kalmanX.getRate() * dt; // Calculate gyro angle using the unbiased rate
  //gyroYangle += kalmanY.getRate() * dt;

  compAngleX = 0.93 * (compAngleX + gyroXrate * dt) + 0.07 * roll;
  compAngleY = 0.93 * (compAngleY + gyroYrate * dt) + 0.07 * pitch;

  // Reset the gyro angle when it has drifted too much
  if (gyroXangle < -180 || gyroXangle > 180)
    gyroXangle = kalAngleX;
  if (gyroYangle < -180 || gyroYangle > 180)
    gyroYangle = kalAngleY;
    
    if(compAngleX>0)
    {
      digitalWrite(dirU,HIGH);
      digitalWrite(stepU,HIGH);
      delayMicroseconds(250);
      digitalWrite(stepU,LOW);
      delayMicroseconds(250);
    }
    else
    {
      digitalWrite(dirU,LOW);
      digitalWrite(stepU,HIGH);
      delayMicroseconds(250);
      digitalWrite(stepU,LOW);
      delayMicroseconds(250);
    }
  }
  
  /*delay(1000);
  mpu.read_acc();
  while(mpu.ay/16384.0>0)
  {
    digitalWrite(dirU,HIGH);
    digitalWrite(stepU,HIGH);
    delayMicroseconds(12000);
    digitalWrite(stepU,LOW);
    delayMicroseconds(12000);
    mpu.read_acc();
  }*/
  
  krok = 0.9 * 0.0625;
  x=0;
  y=0;
  digitalWrite(servoPower,LOW);
  Serial.write(255); 
}
