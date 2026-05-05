// PID Controlling Line Following Robot

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad_I2C.h>
#include <Keypad.h>

// IR and Motor setup
int IR_val[5];
int IR_weight[5] = {-20, -10, 0, 10, 20};
int error = 0, previousError = 0;
int P, I = 0, D;
int speedAdjust;
int LMotorSpeed, RMotorSpeed;
const int MotorBasespeed = 110; // base motor speed
const int MAX_SPEED = 255;      // max motor speed

// IR pins
#define IR1 8
#define IR2 9
#define IR3 10
#define IR4 11
#define IR5 12

// motor pins
#define LMotorA 2
#define LMotorB 4
#define LMotorPWM 3
#define RMotorA 6
#define RMotorB 7
#define RMotorPWM 5

// ultrasonic pins
#define trigPin A0
#define echoPin A1
const int obstacle_distance = 15;

// PID values
float Kp = 2.0;
float Ki = 0.0;
float Kd = 4.5;

// LCD and Keypad 
LiquidCrystal_I2C lcd(0x27, 16, 2);
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {0,1,2,3};
byte colPins[COLS] = {4,5,6,7};
Keypad_I2C keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS, 0x20);

// global state
int selected = 0;
bool commandGiven = false;
bool junctionMeet = false;
String currentLocation = "start";


void setup() {
  Serial.begin(9600);
  
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);
  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);

  pinMode(LMotorA, OUTPUT);
  pinMode(LMotorB, OUTPUT);
  pinMode(LMotorPWM, OUTPUT);
  pinMode(RMotorA, OUTPUT);
  pinMode(RMotorB, OUTPUT);
  pinMode(RMotorPWM, OUTPUT);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lcd.begin(16, 2);
  lcd.backlight();
  keypad.begin();

  lcd.clear();
  lcd.print("Select Your Item");
  set_forward();
  delay(1000);

}


void loop() {

  char key = keypad.getKey();
  if (key) {
    if (key >= '0' && key <='9') {
      selected = key - '0';
      lcd.clear();
      lcd.print("Selected: ");
      lcd.print(selected);
    } else if (key == '#') {
      lcd.clear();
      if (selected == 1) lcd.print("Goint To Milk");
      else if (selected == 2) lcd.print("Goint To Cake");
      else if (selected == 3) lcd.print("Goint To Apple");
      else lcd.print("Invalid Choice");
      commandGiven = true;
      delay(1000);
    }
  }

  if (commandGiven) {
    obstacleRead();
  }
  
  if (commandGiven && currentLocation == "start") {
    read_IR();

    // junction detect
    if (IR_val[1] == 1 && IR_val[2] == 1 && IR_val[3] == 1) {
      junctionMeet = true;

      if (selected == 1 && currentLocation == "start") {
        bool turnLeftFull = false;
        unsigned long leftTurnStart = millis();

        // slowly turn when IR[2] meet the line
        unsigned long startTurn = millis();
        while (true) {
          turnLeft();
          delay(150);
          read_IR();

          //ignore ir for 300ms
          if (millis() - startTurn < 300) continue;
          if (IR_val[2] == 1 && IR_val[0] == 0 && IR_val[1] == 0 && IR_val[3] == 0 && IR_val[4] == 0) {
            turnLeftFull = true;
            break;
          }

          //timeout
          if (millis() - leftTurnStart > 1000) {
            stop();
            break;
          }
        }

        //after turn, follow the line
        bool stopped = false;

        while (true) {
          //obstacle check
          obstacleRead();

          read_IR();
          PID_control();
          set_forward();
          set_speed();

          if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
            stop();
            junctionMeet = false;
            currentLocation = "milk";
            lcd.clear();
            lcd.print("Reached to Milk");
            commandGiven = false;
            delay(1000);
            stopped = true;
            break;
          }
          delay(10);
        }
        return; 
      }

      else if (selected == 2 && currentLocation == "start") {
        bool turnRightFull = false;
        unsigned long RightTurnStart = millis();

        // slowly turn when IR[2] meet the line
        unsigned long startTurn = millis();
        while (true) {
          turnRight();
          delay(150);
          read_IR();

          //ignore ir for 300ms
          if (millis() - startTurn < 300) continue;
          if (IR_val[2] == 1 && IR_val[0] == 0 && IR_val[1] == 0 && IR_val[3] == 0 && IR_val[4] == 0) {
            turnRightFull = true;
            break;
          }

          //timeout
          if (millis() - RightTurnStart > 1000) {
            stop();
            break;
          }
        }

        //after turn, follow the line
        bool stopped = false;

        while (true) {
          //obstacle check
          obstacleRead();

          read_IR();
          PID_control();
          set_forward();
          set_speed();

          if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
            stop();
            junctionMeet = false;
            currentLocation = "cake";
            lcd.clear();
            lcd.print("Reached to Cake");
            commandGiven = false;
            delay(1000);
            stopped = true;
            break;
          }
          delay(10);
        }
        return; 
      }
      else if (selected == 3 && currentLocation == "start") {
        
        set_forward();
        analogWrite(LMotorPWM, 100);
        analogWrite(RMotorPWM, 100);
        delay(300);

        //after turn, follow the line
        bool stopped = false;

        while (true) {
          //obstacle check
          obstacleRead();

          read_IR();
          PID_control();
          set_forward();
          set_speed();

          if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
            stop();
            junctionMeet = false;
            currentLocation = "apple";
            lcd.clear();
            lcd.print("Reached to Apple");
            commandGiven = false;
            delay(1000);
            stopped = true;
            break;
          }
          delay(10);
        }
        return; 
      }
    }

    // if no junction, PID line follow normal
    PID_control();
    set_forward();
    set_speed();

  } else if (commandGiven && currentLocation == "milk") {
    stop();
    delay(200);

    bool turnFullRound = false;
    unsigned long turnStart = millis();
    unsigned long startTurn = millis();

    // turn full round
    while (true) {
      turnFull();
      delay(200);
      read_IR();

      if (millis() - startTurn < 300) continue;
      if (IR_val[2] == 1) {
        turnFullRound = true;
        break;
      }
      
      // avoid infinite loop
      if (millis() - turnStart > 1000) {
        stop();
        break;
      }
      delay(10);
    }

    // when junction meet, follow line
    junctionMeet = false;
    while (!junctionMeet) {
      //obstacle check
      obstacleRead();

      read_IR();
      PID_control();
      set_forward();
      set_speed();

      if (IR_val[1] == 1 && IR_val[2] == 1 && IR_val[3] == 1) {
        junctionMeet = true;
        break;
      }
      delay(10);
    }

    if (selected == 3) {
      bool turnLeftFull = false;
      unsigned long leftTurnStart = millis();

      // slowly turn when IR[2] meet the line
      unsigned long startTurn = millis();
      while (true) {
        turnLeft();
        delay(150);
        read_IR();

        //if center see the line, all others are off
        //ignore ir for 300ms
        if (millis() - startTurn < 300) continue;
        if (IR_val[2] == 1 && IR_val[0] == 0 && IR_val[1] == 0 && IR_val[3] == 0 && IR_val[4] == 0) {
          turnLeftFull = true;
          break;
        }

        //timeout
        if (millis() - leftTurnStart > 1000) {
          stop();
          break;
        }
      }

      // line follow to apple
      while (true) {
        //obstacle check
        obstacleRead();

        read_IR();
        PID_control();
        set_forward();
        set_speed();

        if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
          stop();
          junctionMeet = false;
          currentLocation = "apple";
          lcd.clear();
          lcd.print("Reached to Apple");
          commandGiven = false;
          delay(1000);
          break;
        }
          delay(10);
      }
        return; 
    } else if (selected == 2) {
      set_forward();
      analogWrite(LMotorPWM, 100);
      analogWrite(RMotorPWM, 100);
      delay(300);

      // line follow to apple
      while (true) {
        //obstacle check
        obstacleRead();

        read_IR();
        PID_control();
        set_forward();
        set_speed();

        if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
          stop();
          junctionMeet = false;
          currentLocation = "cake";
          lcd.clear();
          lcd.print("Reached to Cake");
          commandGiven = false;
          delay(1000);
          break;
        }
          delay(10);
      }
      return;
    }
  }

   else if (commandGiven && currentLocation == "cake") {
    stop();
    delay(200);

    bool turnFullRound = false;
    unsigned long turnStart = millis();
    unsigned long startTurn = millis();

    // turn full round
    while (true) {
      turnFull();
      delay(200);
      read_IR();

      if (millis() - startTurn < 300) continue;
      if (IR_val[2] == 1) {
        turnFullRound = true;
        break;
      }
      
      // avoid infinite loop
      if (millis() - turnStart > 1000) {
        stop();
        break;
      }
      delay(10);
    }

    // when junction meet, follow line
    junctionMeet = false;
    while (!junctionMeet) {
      //obstacle check
      obstacleRead();

      read_IR();
      PID_control();
      set_forward();
      set_speed();

      if (IR_val[1] == 1 && IR_val[2] == 1 && IR_val[3] == 1) {
        junctionMeet = true;
        break;
      }
      delay(10);
    }

    if (selected == 3) {
      bool turnRightFull = false;
      unsigned long RightTurnStart = millis();

      // slowly turn when IR[2] meet the line
      unsigned long startTurn = millis();
      while (true) {
        turnRight();
        delay(150);
        read_IR();

        //if center see the line, all others are off
        //ignore ir for 300ms
        if (millis() - startTurn < 300) continue;
        if (IR_val[2] == 1 && IR_val[0] == 0 && IR_val[1] == 0 && IR_val[3] == 0 && IR_val[4] == 0) {
          turnRightFull = true;
          break;
        }

        //timeout
        if (millis() - RightTurnStart > 1000) {
          stop();
          break;
        }
      }

      // line follow to apple
      while (true) {
        //obstacle check
        obstacleRead();

        read_IR();
        PID_control();
        set_forward();
        set_speed();

        if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
          stop();
          junctionMeet = false;
          currentLocation = "apple";
          lcd.clear();
          lcd.print("Reached to Apple");
          commandGiven = false;
          delay(1000);
          break;
        }
          delay(10);
      }
        return; 
    } else if (selected == 1) {
      set_forward();
      analogWrite(LMotorPWM, 100);
      analogWrite(RMotorPWM, 100);
      delay(300);

      // line follow to apple
      while (true) {
        //obstacle check
        obstacleRead();

        read_IR();
        PID_control();
        set_forward();
        set_speed();

        if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
          stop();
          junctionMeet = false;
          currentLocation = "milk";
          lcd.clear();
          lcd.print("Reached to Milk");
          commandGiven = false;
          delay(1000);
          break;
        }
          delay(10);
      }
      return;
    }
  }

   else if (commandGiven && currentLocation == "apple") {
    stop();
    delay(200);

    bool turnFullRound = false;
    unsigned long turnStart = millis();
    unsigned long startTurn = millis();

    // turn full round
    while (true) {
      turnFull();
      delay(200);
      read_IR();

      if (millis() - startTurn < 300) continue;
      if (IR_val[2] == 1) {
        turnFullRound = true;
        break;
      }
      
      // avoid infinite loop
      if (millis() - turnStart > 1000) {
        stop();
        break;
      }
      delay(10);
    }

    // when junction meet, follow line
    junctionMeet = false;
    while (!junctionMeet) {
      //obstacle check
      obstacleRead();

      read_IR();
      PID_control();
      set_forward();
      set_speed();

      if (IR_val[1] == 1 && IR_val[2] == 1 && IR_val[3] == 1) {
        junctionMeet = true;
        break;
      }
      delay(10);
    }

    if (selected == 2) {
      bool turnLeftFull = false;
      unsigned long leftTurnStart = millis();

      // slowly turn when IR[2] meet the line
      unsigned long startTurn = millis();
      while (true) {
        turnLeft();
        delay(150);
        read_IR();

        //if center see the line, all others are off
        //ignore ir for 300ms
        if (millis() - startTurn < 300) continue;
        if (IR_val[2] == 1 && IR_val[0] == 0 && IR_val[1] == 0 && IR_val[3] == 0 && IR_val[4] == 0) {
          turnLeftFull = true;
          break;
        }

        //timeout
        if (millis() - leftTurnStart > 1000) {
          stop();
          break;
        }
      }

      // line follow to apple
      while (true) {
        //obstacle check
        obstacleRead();

        read_IR();
        PID_control();
        set_forward();
        set_speed();

        if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
          stop();
          junctionMeet = false;
          currentLocation = "cake";
          lcd.clear();
          lcd.print("Reached to Cake");
          commandGiven = false;
          delay(1000);
          break;
        }
          delay(10);
      }
        return; 
    } else if (selected == 1) {
      bool turnRightFull = false;
      unsigned long RightTurnStart = millis();

      // slowly turn when IR[2] meet the line
      unsigned long startTurn = millis();
      while (true) {
        turnRight();
        delay(150);
        read_IR();

        //if center see the line, all others are off
        //ignore ir for 300ms
        if (millis() - startTurn < 300) continue;
        if (IR_val[2] == 1 && IR_val[0] == 0 && IR_val[1] == 0 && IR_val[3] == 0 && IR_val[4] == 0) {
          turnRightFull = true;
          break;
        }

        //timeout
        if (millis() - RightTurnStart > 1000) {
          stop();
          break;
        }
      }

      // line follow to apple
      while (true) {
        //obstacle check
        obstacleRead();

        read_IR();
        PID_control();
        set_forward();
        set_speed();

        if (IR_val[0] == 1 && IR_val[1] == 1 && junctionMeet == true) {
          stop();
          junctionMeet = false;
          currentLocation = "milk";
          lcd.clear();
          lcd.print("Reached to Milk");
          commandGiven = false;
          delay(1000);
          break;
        }
          delay(10);
      }
      return;
    }
  }
}



// read ultrasonic distance
long read_distance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);

  return duration * 0.034 / 2;
}

void obstacleRead() {
  if (read_distance() < obstacle_distance) {
    stop();
    lcd.setCursor(0, 1);
    lcd.print("Obstacle! ");
    while (read_distance() < obstacle_distance) {
      delay(50);
    }
    lcd.setCursor(0, 1);
    lcd.print("Path Clear! ");
    delay(500);
  }
}

// read IR sensors values
void read_IR() {
  IR_val[0] = digitalRead(IR1);
  IR_val[1] = digitalRead(IR2);
  IR_val[2] = digitalRead(IR3);
  IR_val[3] = digitalRead(IR4);
  IR_val[4] = digitalRead(IR5);
}

void PID_control() {
  error = 0;

  for (int i = 0; i < 5; i++) {
    error += IR_weight[i] * IR_val[i];
  }

  P = error;
  I += error;
  D = error - previousError;
  previousError = error;

  speedAdjust = Kp * P + Ki * I + Kd * D;

  LMotorSpeed = MotorBasespeed - speedAdjust;
  RMotorSpeed = MotorBasespeed + speedAdjust;

  if (LMotorSpeed < 0) {
    LMotorSpeed = 0;
  }
  if (RMotorSpeed < 0) {
    RMotorSpeed = 0;
  }
  if (LMotorSpeed > MAX_SPEED) {
    LMotorSpeed = MAX_SPEED;
  }
  if (RMotorSpeed > MAX_SPEED) {
    RMotorSpeed = MAX_SPEED;
  }
}

void set_forward() {
  digitalWrite(LMotorA, HIGH);
  digitalWrite(LMotorB, LOW);
  digitalWrite(RMotorA, HIGH);
  digitalWrite(RMotorB, LOW);
}

void turnLeft() {
  digitalWrite(LMotorA, LOW);
  digitalWrite(LMotorB, HIGH);
  digitalWrite(RMotorA, HIGH);
  digitalWrite(RMotorB, LOW);

  analogWrite(LMotorPWM, 50);
  analogWrite(RMotorPWM, 120);
}

void turnRight() {
  digitalWrite(LMotorA, HIGH);
  digitalWrite(LMotorB, LOW);
  digitalWrite(RMotorA, LOW);
  digitalWrite(RMotorB, HIGH);

  analogWrite(LMotorPWM, 120);
  analogWrite(RMotorPWM, 50);
}

void stop() {
  digitalWrite(LMotorA, LOW);
  digitalWrite(LMotorB, LOW);
  digitalWrite(RMotorA, LOW);
  digitalWrite(RMotorB, LOW);

  analogWrite(LMotorPWM, 0);
  analogWrite(RMotorPWM, 0);
}

void turnFull() {
  digitalWrite(LMotorA, LOW);
  digitalWrite(LMotorB, HIGH);
  digitalWrite(RMotorA, HIGH);
  digitalWrite(RMotorB, LOW);

  analogWrite(LMotorPWM, 160);
  analogWrite(RMotorPWM, 160);
}

void set_speed() {
  analogWrite(LMotorPWM, LMotorSpeed);
  analogWrite(RMotorPWM, RMotorSpeed);  
}
