# IntroductionToRobotics
    Introduction to Robotics Homework Repository

## About
This is my personal GitHub repository, a dedicated space for all the engaging laboratory homework assignments I've tackled during my third year at the Faculty of Mathematics and Computer Science, University of Bucharest. Here, you'll find a collection of captivating robotics challenges that have enriched my understanding of this dynamic field.

## What's Inside?
For each homework assignment I've compiled the clear task requirements and/or description, implementation details, a photo of my hardware setup and a link to a video showcasing functionality. This has been a voyage of discovery, where I've combined theoretical insights with hands-on coding and experimentation. 

## Key Features

- **Theory and Practice**: Approaching each assignment with utmost professionalism has helped me deepen my theoretical knowledge and sharpen my practical skills.

- **Code and Images**: Well explained code and visual exemplification are paramount to one's better understanding of the task at hand.

# Homework

<details>
<summary>

## Homework #1 - RGB

</summary><br>
This assignment focuses on controlling each channel (Red, Green, and Blue) of  an  RGB  LED  using  individual  potentiometers.

[Assignment folder](https://github.com/MariusAlexandru358/IntroductionToRobotics/tree/main/LedRGB)

### Components:
- Arduino UNO
- Breadbord
- RGB LED (1)
- Potentiometers (3)
- 330&#x2126; Resistors (3)
- Wires as needed

### Technical Task
Use a separate potentiometer for controlling each color of the RGB LED: Red, Green and Blue. The control must leverage digital electronics.

### Breakdown
This is a straightforward task. All we have to do is read the values from the potentiometers (these are analog values), map them to the 0-255 interval, and write the resulting value to the LED pin. Of course, each potentiometer corresponds to one color of the RGB LED.

### Electrical schematic
![Electrical Circuit Scheme](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/LedRGB/electrical.png)

### Hardware Setup
![Setup Image](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/LedRGB/LedRGBsetup.jpg)

### Video showcasing functionality
<a href="https://youtu.be/jgkgB0jxtuw" target="_blank">youtube link here</a>

</details>

---

<details>
<summary>

## Homework #2 - Elevator Simulator

</summary><br>
This assignment involves simulating a 3-floor elevator control system using LEDs, buttons, and a buzzer with Arduino.

[Assignment folder](https://github.com/MariusAlexandru358/IntroductionToRobotics/tree/main/ElevatorSim)

### Components:
- Arduino UNO
- Breadbord
- Pushbuttons (At least 3 for floor calls)
- LEDs (At least 4: 3 for the floors and 1 for the elevator's operational state)
- 330&#x2126; or 220&#x2126; Resistors (4)
- Buzzer (1)
- 100&#x2126; Resistor (1)
- Wires as needed

### Technical Task
Design a control system that simulates a 3-floor elevator using the Arduino platform. Specific requirements:
- **LED Indicators:** Each of the 3 LEDs should represent one of the 3 floors. The LED corresponding to the current floor should light up.  Additionally, another LED should represent the elevator’s operational state. It should blink when the elevator is moving and remain static when stationary.
- **Buttons:** Implement 3 buttons that represent the call buttons from the 3 floors.  When pressed, the elevator should simulate movement towards the floor after a short interval (2-3 seconds).
- **Buzzer:** The buzzer should sound briefly when the elevator arrives at the desire floor (something resembling a ”cling”), when the elevator doors are closing and during movement.
- **State Change & Timers:** If the elevator is already at the desired floor, pressing the button for that floor should have no effect. Otherwise, after a button press, the elevator should ”wait for the doors to close” and then ”move” to the corresponding floor. If the elevator is in movement it should either do nothing or it should stack its decision (get to the first programmed floor, open the doors, wait, close them and then go to the next desired floor).
- **Debounce:** Implement debounce for the buttons to avoid unintentional repeated button presses.

### Breakdown
One way to implement this is by using the function millis() to get the timestamp of when a button push is confirmed by the debounce logic and then with the help of the same function we can calculate how much time has passed since that event. Let's say it takes 3 seconds for the elevator to move up a floor, then after 3000 milliseconds we simulate moving up to that floor by turning on the corresponding LED. We can apply the same logic for all the actions we must implement. 

### Electrical schematic
![Electrical Circuit Scheme](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/ElevatorSim/ElevatorSimElectrical.png)

### Hardware Setup
![Setup Image](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/ElevatorSim/ElevatorSimSetup.jpg)

### Video showcasing functionality
<a href="https://youtu.be/h7vUpx4tfUE" target="_blank">youtube link here</a>

</details>

---

<details>
<summary>

## Homework #3 - 7 Segment Display Drawing

</summary><br>
This assignment involves using the joystick in order to control the position of the segment and ”draw” on the display. The movement between segments should be natural, meaning they should jump from the current position only to neighbors, but without passing through ”walls”.

[Assignment folder](https://github.com/MariusAlexandru358/IntroductionToRobotics/tree/main/Drawing_7SegmentDisplay)

### Components:
- Arduino UNO
- Breadbord
- 7-Segment-Display (1)
- Joystick (1)
- 330&#x2126; or 220&#x2126; Resistors (8)
- Wires as needed

### Technical Task
The initial position should be on the DP. The current position always blinks (irrespective of the fact that the segment is on or off). Use the joystick to move from one position to neighbors (see table for corresponding movement). Short pressing the button toggles the segment state from ON to OFF or from OFF to ON. Long pressing the button resets the entire display by turning all the segments OFF and moving the current position to the decimal point.

| Current segment | UP | DOWN | LEFT | RIGHT |
| --------------- | -- | ---- | ---- | ----- |
| **a**           |N/A |  g   |  f   |   b   |
| **b**           | a  |  g   |  f   |  N/A  | 
| **c**           | g  |  d   |  e   |  dp   |
| **d**           | g  | N/A  |  e   |   c   | 
| **e**           | g  |  d   | N/A  |   c   |
| **f**           | a  |  g   | N/A  |   b   | 
| **g**           | a  |  d   | N/A  |  N/A  |
| **dp**          |N/A |  N/A |  c   |  N/A  | 

### Breakdown
This assignment can be broken down into multiple simpler tasks. 
- **Input detection:** The push-button needs debouncing and to be able to also detect long pushes. For the movements on the joystick we will set thresholds it needs to pass in order to be registered.
- **Calculating the next position and moving to it**
- **Updating the blinking** of the selected LED
- **Toggling the state** of a LED or **reseting** the display and position


### Electrical schematic
![Electrical Circuit Scheme](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/Drawing_7SegmentDisplay/Drawing_7SegmentDisplay_Electrical.png)

### Hardware Setup
![Setup Image](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/Drawing_7SegmentDisplay/Drawing_7SegmentDisplay_Setup.jpg)

### Video showcasing functionality
<a href="https://youtu.be/JfM9QyAPTl4" target="_blank">youtube link here</a>

</details>

---

<details>
<summary>

## Homework #4 - Stopwatch Timer

</summary><br>
Using the 4 digit 7 segment display and 3 buttons, implement a stopwatch timer that counts in 10ths of a second and has a save lap functionality (similar to most basic stopwatch functions on most phones).

[Assignment folder](https://github.com/MariusAlexandru358/IntroductionToRobotics/tree/main/StopwatchTimer)

### Components:
- Arduino UNO
- Breadbord
- SN74HC595N Shift Register (1)
- 4-Digit 7-Segment-Display (1)
- 330&#x2126; or 220&#x2126; Resistors (8)
- Pushbuttons (3)
- Wires as needed

### Technical Task
The starting value of the 4 digit 7-segment display should be "000.0". The buttons should have the following functionalities:
- Button 1: Start / Pause
- Button 2: Reset (if paused). Reset saved laps (if in lap viewing mode)
- Button 3: Save lap (if counting). Cycle through last saved laps (up to 4 laps)

Workflow:
1. Display shows "000.0" When pressing the **Start** button, the timer should start.
2. During the counting, each time you press the lap button, that value should be saved, up to 4 laps; pressing the 5th time should override the first saved one. Pressing the Reset button while counting should do nothing. Pressing the Pause button should stop the timer.
3. In **Pause** Mode, the Lap button shouldnt work anymore. Pressing the Reset button should reset the timer to "000.0".
4. After a reset, the Lap button should cycle through the saved lap times. Pressing it continuosly should cycle through the memory continously. Pressing the reset button while in this state should reset everything and set the timer back to "000.0".

### Breakdown
- In order to display the current time, we will cycle through the display digits and write the values for each digit to the shift register in serial mode, which will then send it to the 7-segment display. Because we can turn the LEDs on and off faster than your eyes can "see", we will creat the ilusion of a 4 digit number. Multiplexing logic is used to select which digit of the display we are writing on.
- The logic behind the buttons is relatively simple, it can be done with a few flags. Each button needs debouncing of course.
- In order to implement an extra functionality, I've decided to use a vector for the memory. The first position will always be a starting position ("000.0") from which we can start counting again, while the following positions are the memory spaces for the saved lap times. This way, we can start a timer, count one or more laps, see the data, and count again without losing the previous laps. For a practical use, we can increase the memory size (the vector size). Of course, the Reset button when pressed while in the lap viewing mode still works as intended.

### Electrical schematic
![Electrical Circuit Scheme](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/StopwatchTimer/StopwatchTimer.png)

### Hardware Setup
![Setup Image](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/StopwatchTimer/StopwatchTimer.jpg)

### Video showcasing functionality
<a href="https://youtu.be/cyFr2RE60oo" target="_blank">youtube link here</a>

</details>

---







