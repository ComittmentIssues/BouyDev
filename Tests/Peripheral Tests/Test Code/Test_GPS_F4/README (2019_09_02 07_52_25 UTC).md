# Buoy Dev

The official project for the development of the Antarctic sea ice measurement platform. All code/docs/development strategies are available here

## Getting Started

If you are developing software, please take note of the following conventions:

### Hardware Platform

The Microcontroller used for this project is the STM32F4 Discovery board. Please make sure you are familiar with the platform before starting

### Development STudio

The project is developed in C using Atollic TrueStudio version 7.1.1. We will be using the ARM GDB and ST-LINK to debug the controller and the OpenOCD tool chain. 

### Coding conventions

The code standard for this project is SPL. These drivers are included by default in new atollic projects.

To ensure ease of integration. Please make sure the modules are coded to interact with the subsystems using the following mapping:

1.  USART 1: GPS
2.  USART 2: <RESERVED>
3.  USART 3: Iridium
4.  SPI   1: SD Card Reader
5.  I2C   1: IMU

These are the conventions unless otherwise stated

Place All device specific code into separete Headers and Source Documents e.g. device.h device.c. 
These include

1. Functions and prototypes
2. Device specific structs
3. Device specific enums
3. Device specific Macros
4. private variables 

Any additional libraries used are to be descibed in full and credited where neccessary.

#### Only use the main functions for test cases

## Label System

Issues contain detais regarding tasks including due dates and descriptions. When you are working on a task please set the label to "doing". When you are finished, set the label to Q/A
We will review them at the next meeting. 
If an issue cannot be resolved, the label is set to block.
