# DAI_Firebase_Proj
A simple firebase ESP32-C3 project to record the amount of calcium carbonate formed on a cathode over time

# Things you need for the Project
| Hardware Components  | Software |
| ------------- | ------------- |
| ESP32-C3 Dev Kit  | Arduino IDE  |
| LED Diode x 1  |   |
| Light Dependent Resistor (LDR) x 1  |   |
| Resistor x 2  |   |
| Jumper Cables  |   |

# Background
I am currently working with a company that aims to build electric reefs in Singapore to grow corals and hit Singapore's target of 100,000 corals. The electric reef pillars make use of electrolysis to deposit calcium carbonate on the cathode which then facilitates coral growth.

<p align="center">
  <img width="460" height="300" src="https://github.com/skywillow/DAI_Firebase_Proj/assets/154501032/a4655e67-031e-40d1-a96b-125b39dac5b6">
</p>

There is currently much testing being conducted with different set up variations. The material used for the anode and cathode, distance between them as well as shape of the cathode are being tested to find the most effective way for calcium carbonate to form. I aim to reduce the manual labour involved in this process by automating the data collection and tracking process. I will use the ESP32-C3 Dev Kit to create a simple project to track the rough amount of calcium carbonate being formed in order to notify the user when a significant amount of calcium carbonate has been formed.

<p align="center">
  <img width="460" height="400" src="https://github.com/skywillow/DAI_Firebase_Proj/assets/154501032/f7ea1225-e608-412b-935d-ae4a605ee929">
</p>

Usually readings are taken over 5 days. But for this demonstration we will assume readings are taken over 2 hours with 15mins interval.
The readings of the LDR is taken every 15mins and the value is given a corresponding tag (Very Little, Little, Some, More, A lot), the time stamp will also be taken 
Time lapsed since testing started will be calculated. 

The data will then be compiled in a google sheet using Two Minute Reports for easier tracking and reference. The data includes the exact voltage recorded by the Light Dependent Resistor, the corresponding tag as well as the time stamp. With these data, users can then estimate the speed taken for the calcium carbonate to form as well as monitor the formation and can head down to check on it when necessary.

This is a base for similar functions, accurate calibration of the sensors is still required to reflect the amount of calcium carbonate that is being formed accurately. Slight modification of the circuitry would also be needed for actual application as it would have to be submerged underwater. Else it can also be attached outside the tank.

# Basic Specifications

#


# Schematic Diagram

# Code

# Application
