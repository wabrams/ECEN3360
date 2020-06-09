# [ECEN 3360] Digital Design Lab

Coursework for University of Colorado Boulder's ECEN 3360, Spring 2020.
Good, low energy embedded practices done using a Silicon Labs devkit.

## Getting Started

These projects all rely on the following from Silicon Labs:
* Pearl Gecko 12 (devkit)
* Simplicity Studio (v4, devenv)

### Installing

Install [Simplicity Studio](https://www.silabs.com/products/development-tools/software/simplicity-studio "silabs sstudio")

TODO: install needed tools for PG12 and Programmer

## Importing / Running Projects

I have included .zip exports of the projects with each release.
I recommend downloaded these archives, and importing them into Simplicity, like so:
File -> Import -> Existing Project into Workspace -> Archive File

### Versioning

These are indexed with version numbers:
* v1.x indicates Lab 1
* v2.x indicates Lab 2

Normally, x = 0, but occasionally corrections were made after labs were submitted, in which case x > 0.

## Lab Breakdowns

### Lab 1 - Blinky

Function: simple LED blink on/off

This lab is a simple way to gain familiarity with the IDE and devkit. 
It introduces the basic structure of the project.

### Lab 2 - PWM

Function: simple PWM heartbeat / LED blink

This lab establishes a heartbeat we can use throughout the system.

### Lab 3 - EM

Function: cycle through energy modes (power conservation)

This lab establishes the sleep modes and blocks, to let us sit in the lowest possible energy mode.

### Lab 4 - I2C

Function: communicate with Si7021 temperature sensor over I2C

This lab enables basic data readings, over I2C.

### Lab 5 - LEUART

Function: communicate temperature readings to bluetooth device.

This lab enables communication with the HM10 bluetooth module.

### Lab 6 - CB

Function: allow back-to-back writes without string errors.

This lab introduces the circular buffer data structure, to buffer bluetooth writes.

### Lab 7 - FP

Function: allow user interaction.

This lab finishes setting up LEUART RX capabilities, allowing us to receive commands from the bluetooth module.

## Authors

* William Abrams - [wabrams](https://github.com/wabrams)
