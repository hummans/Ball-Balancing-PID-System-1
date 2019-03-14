# Ball Balancing PID Systems 

Hardware:
- microcontroller: Atmega2560
- PC
- Servos:
- Battery pack: 6V, 2850maAh

Language: C/C++
Additional libs: OpenCV 3.4.1

***Those modules are designed to be modularly.
In every module there is a testing unit ***

[1] Computer Vision Module: acquire ball position using OpenCV libs.

[2] PID Module: filter and compute signal.

[3] Serial Communication Module: provide beetwen PC and AVR device.

[4] Actuation Module: this code run on avr, wait for incoming packet and move servos.

