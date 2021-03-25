#include <SoftwareSerial.h> // Includem libraria serialului Bluetooth HC-06
#include <Servo.h> // Includem libraria motorului Servo
//#include <NewPing.h>

Servo myservo; // Declaram variabila pentru a determina unghiul de miscare
/*
#define TRIG_PIN 5 // Pin A0 on the Motor Drive Shield soldered to the ultrasonic sensor
#define ECHO_PIN 4 // Pin A1 on the Motor Drive Shield soldered to the ultrasonic sensor
#define MAX_DISTANCE 100 // sets maximum useable sensor measuring distance to 300cm
#define COLL_DIST 30 // sets distance at which robot stops and reverses to 30cm
#define TURN_DIST COLL_DIST+20 // sets distance at which robot veers away from object
*/

int trigPin = 13;
int echoPin = 12;
long duration;
long distance;

//NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

int leftDistance, rightDistance; //distances on either side
int curDist = 0;
String motorSet = "";
int speedSet = 0;
int obj =1;

char data, operatie  ;  // Declaram variabila pentru stocarea datelor primite prin Bluetooth                 
int Motor1 = 6 ; // Setam pinul digital 6
int EnableMotor1 = 7 ; // Setam pinul digital 7
int EnableMotor2 = 8; // Setam pinul digital 8
int Motor2 = 3; // Setam pinul digital 3
int value = 2; // 
int servoLeft = 230; //Setam unghiul de deplasare la stanga
int servoRight = 100; //Setam unghiul de deplasare la dreapta
int servoForward = 150; //Setam unghiul de deplasare in fata

int motorValue = 0;  // Valoarea motorului cand este oprit

bool motor_on = false; // Variabila boolean pentru a tine evidenta daca motoarele siiiunt oprite sau pornite
String  s1, s2, s, n, p, t, sw, sa, sd, ss;
//SoftwareSerial BT(10, 11); // RX, TX // Setam pinii pentru Bluetooth

/*
 Functia StartMotors() pentru a porni motoarele.
 */
 
void StartMotors(){
  StopMotors();
  digitalWrite(EnableMotor1, HIGH);  
  analogWrite(Motor1,128 - motorValue);
  digitalWrite(EnableMotor2, HIGH);
  analogWrite(Motor2,128 -  motorValue);
  Serial.println(motorValue);
  motor_on = true;
}

/*
 Functia StopMotors() pentru a porni motoarele.
 */

void StopMotors(){
  digitalWrite(EnableMotor1, LOW);
  digitalWrite(EnableMotor2, LOW);
  motor_on = false;
//  motorValue = 120;
}

void StartMotorsReverse(){
   StopMotors();
  digitalWrite(EnableMotor1, HIGH);
  analogWrite(Motor1, 128 + motorValue);
  digitalWrite(EnableMotor2, HIGH);
  analogWrite(Motor2, 128 + motorValue);
  Serial.println(motorValue);
  motor_on = true;
}

void Left(){
   StopMotors();
digitalWrite(EnableMotor1, LOW);  
 // analogWrite(Motor1, motorValue);
  digitalWrite(EnableMotor2, HIGH);
 // analogWrite(Motor2, 128 - motorValue); 
  motor_on = true;
}

void Right(){
   StopMotors();
  digitalWrite(EnableMotor1, HIGH);  
//  analogWrite(Motor1, 128 - motorValue);
   digitalWrite(EnableMotor2, LOW);
//  analogWrite(Motor2, motorValue);
  motor_on = true;
}

int CalculeazaDistanta(){
 digitalWrite(trigPin, LOW);  // Added this line
  delayMicroseconds(2); // Added this line
  digitalWrite(trigPin, HIGH);
//  delayMicroseconds(1000); - Removed this line
  delayMicroseconds(10); // Added this line
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration/2) / 29.1;
 Serial.print("Distance: ");
 Serial.println(distance);
}

void Autonom() 
{
  CalculeazaDistanta();

StartMotors();
if (distance >= 20) {
  Serial.print(distance);
  Serial.print("cm");
    Serial.println();
    Serial.println("GOOD");
    StartMotors();
    delay(1);
  } else
  {
  Serial.print(distance);
  Serial.print("cm");
      Serial.println();
      Serial.println("DANGER");
      switch (obj) {
        case 1:
       StopMotors();
          delay(1);
          myservo.write(servoLeft);
          delay(1000);
          StartMotorsReverse();
          delay(2000);  
          CalculeazaDistanta();
          if (CalculeazaDistanta() > 10 ){
          Left();     
          delay(1500);
          myservo.write(servoForward);
          obj = 1;
          }
          if (CalculeazaDistanta() < 10) {obj  = 2; }   
          break;
        case 2:
          StopMotors();
          delay(1);
          myservo.write(servoRight);
          delay(1000);
          CalculeazaDistanta();
          if (CalculeazaDistanta() > 10) {
            Right();
            delay(1500);
            myservo.write(servoForward);
            obj = 1;
          }
          if (CalculeazaDistanta() < 10) { 
          obj = 3;}
          break;
        case 3:
          StopMotors();
          delay(1);
          myservo.write(servoForward);
          StartMotorsReverse();
          delay(2000);
          Left();
          delay(1500 );
          obj = 1;
          break;
          default : 
          break;
      }
        }
}


/*
 Functia Setup() pentru a seta pinii de intrare, respectiv iesire.
 */
 
void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);         //Seteaza timpul de primirii a datelor in biti pe secunda      
  pinMode(Motor1, OUTPUT);
  pinMode(EnableMotor1, OUTPUT);
  pinMode(Motor2, OUTPUT);
  pinMode(EnableMotor2, OUTPUT);
  pinMode(5, OUTPUT);
  myservo.attach(9); // Setam pinul digital pentru Servo
  myservo.write(servoForward);
  operatie = '2';
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}


void loop()
{

     if(Serial1.available() > 0){  // Trimite informatie doar cand primesti informatie
       data = Serial1.read();      // Citim informatia primita si o stocam intr-o varibila
       Serial.println(data);
        s =String(data);
 //       BT.println(s);
           
    switch(data){
     
        
      //   * Cazurile 'i' si 'k' sunt folosite pentru incrementarea si decrementarea vitezei cu ajutorul functiei analogWrite() care ne permite sa setam viteza motorului
         
         
      case 'i':
            if(motor_on && motorValue >=0){
            motorValue -= 10;
            //analogWrite(Motor1,128 - motorValue);
            //analogWrite(Motor2,128 - motorValue);
            Serial.println(motorValue);
   //         n =String(motorValue);
  //          BT.println(n);
  data = operatie;
  Serial.println(data);

            }

 // String t = String(s + ' ' + n );
  
        break;
        
      case 'k':
            if(motor_on &&  motorValue <120)
            {
            motorValue += 10;
            //analogWrite(Motor1,128 - motorValue);
            //analogWrite(Motor2,128 - motorValue);
            Serial.println(motorValue);
         //  p =String(motorValue);
        //    BT.println(p);
        data = operatie;
        Serial.println(data);
        
        }
    }

 switch(data){    
        case '1':
        {operatie = '1';
        }
        break;

      case '2':
       {operatie = '2';
        }
        break;
        
      case 'w':
        {operatie = 'w';
        }
        break;  
        
      case 'a':
      
      {operatie = 'a';
      }
        break;
      case 'd':
        {operatie = 'd';
        }
        break;

      case 's':
       {operatie = 's';
        }
        break;
        case '3':
        do
        Autonom();
        while (Serial1.available() == 0);
        break;
       
    }
    Serial.println(operatie);
    switch(operatie){   
 case '1':
//        if(!motor_on){
            StartMotors();
            Serial.println(motorValue);
//            s1 =String('1');
        
        break;

      case '2':
        {StopMotors();
//        s2 =String('2');
        }   
        break;
      case 'w':
        myservo.write(servoForward);
     //   sw =String('w');
        break;  
      case 'a':
        Left();
        Serial.println(motorValue);
        break;
      case 'd':
        Right();
        Serial.println(motorValue);
        break;
/*      case '0':
        StopMotors();
        break;  */
      case 's':
        StartMotorsReverse();
        Serial.println(motorValue);
break;
//        ss =String('s');
//        break;
//      case 'b':
//      default:
//        break;
      }
         
  }
  String t1 = String(s1 + ' ' + s2 + ' ' + s);
//  String t2 = String(n + ' ' + p);
//  String t3 = String(sw + ' ' + sa + ' ' + sd + ' ' + ss);
String t4 = String(s1 + ' ');
//  BT.println(motorValue);
/* delay(1000);
  BT.println(t2);
  delay(1000);
  BT.println(t3); */
}   
