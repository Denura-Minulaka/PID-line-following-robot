# PID Line Following Supermarket Assistance Robot

A **PID-controlled line following robot** designed to assist customers in supermarkets by guiding them to product locations. The robot combines robotics, embedded systems, and automation to create a smart shopping assistant.

---

## Project Overview
This project introduces a smart supermarket assistant robot that helps users quickly navigate to item locations.  
Users enter an item number using a keypad, and the robot automatically travels to the selected location using a **PID line-following algorithm** with obstacle detection.

The system improves shopping efficiency by reducing the time spent searching for products.

---

## Hardware Components

- Arduino Uno
- L298N Motor Driver
- 5 IR Sensors (Line detection)
- Ultrasonic Sensor (Obstacle detection)
- 4x4 Keypad with PCF8574T I/O Expander
- 16x2 LCD Display with I2C Module
- 2 DC Motors + Caster Wheel
- 7.4V 18650 Li-ion Battery Pack
- Buck Converter (Voltage regulation)
- Robot Chassis

---

## Technologies Used
- Arduino (Embedded C++)
- PID Control Algorithm
- I2C Communication
- Robotics & Embedded Systems

---

## Key Features

- **PID Line Following**
  - Smooth and stable movement without zigzag behavior
  - Real-time error correction using PID (Proportional, Integral, Derivative)

- **Destination Navigation**
  - Users select item location using keypad
  - Robot navigates through junctions to reach selected destination

- **Obstacle Avoidance**
  - Ultrasonic sensor detects obstacles
  - Robot stops and resumes automatically once the path is clear

- **User Interface**
  - LCD displays selected item and robot status
  - Keypad used to input destination numbers

---

## How the Robot Works

1. Power ON the robot.
2. LCD displays **"Select Your Item"**.
3. User enters item number via keypad.
4. Robot follows the path using PID control.
5. At junctions:
   - 1 → Turn Left (Milk)
   - 2 → Turn Right (Cake)
   - 3 → Go Forward (Apple)
6. Robot stops when the destination is reached.
7. If an obstacle is detected, robot pauses until cleared.

---

## PID Control Explanation

Traditional line followers move in a zigzag pattern.  
This robot uses a **PID controller** to calculate error and adjust motor speed dynamically.

- **P (Proportional):** Corrects current error  
- **I (Integral):** Corrects past errors  
- **D (Derivative):** Predicts future error and reduces overshoot  

This results in **smooth and accurate line tracking**.

---

## Challenges Faced

- Limited Arduino pins → Solved using **I2C Module** and **PCF8574T I/O Expander**
- Robot weight changes affected PID tuning → Re-tuned Kp, Ki, Kd values for stability

---

## Future Improvements

- Voice guidance system
- Voice command navigation
- Upgrade to **SLAM navigation** using LiDAR and cameras
- Mobile app integration

---


## Demo Video
https://drive.google.com/file/d/16eQtQpZiDaIyX-p6CYWEGZdYw9ZP80vb/view?usp=sharing
