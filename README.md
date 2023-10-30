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

## Homework #1 - RGB
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

### Electrical scheme
![Electrical Circuit Scheme](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/LedRGB/electrical.png)

### Hardware Setup
![Setup Image](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/LedRGB/LedRGBsetup.jpg)

### Video showcasing functionality
<a href="https://youtu.be/jgkgB0jxtuw" target="_blank">youtube link here</a>



## Homework #2 - Elevator Simulator
This  assignment  involves  simulating  a  3-floor  elevator  control  system  usingLEDs, buttons, and a buzzer with Arduino.
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

### Electrical scheme
![Electrical Circuit Scheme](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/ElevatorSim/ElevatorSimElectrical.png)

### Hardware Setup
![Setup Image](https://github.com/MariusAlexandru358/IntroductionToRobotics/blob/main/ElevatorSim/ElevatorSimSetup.jpg)

### Video showcasing functionality
<a href="https://youtu.be/h7vUpx4tfUE" target="_blank">youtube link here</a>







