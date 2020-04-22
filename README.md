# DMD_MAX72xx Arduino Library API Documentation

## Overview
This library allows the interfacing of any Arduino board with one or more LED matrix modules controlled by MAX7219/MAX7221.
Each module has 64 individual LEDs arranged in an 8 x 8 matrix.
The maximum number of modules that can be controlled is 255.
These can be arranged in the form of one row of 255 modules (matrix 8 x 2040 = 16320 pixels),
or in two rows of 127 modules (matrix 16 x 1016 = 16256 pixels),
or in three rows of 85 modules (matrix 24 x 680 = 16320 pixels),
or whatever number of rows you want.
Each LED in the matrix display can be addressed individually.

## MAX7219/MAX7221 LED Matrix Module

An example image of a MAX7219/MAX7221 Matrix Module commonly sold on the market can be seen in ***Figure 1***.
On the right side, there are 3 input pins (CLK, DS, and DIN respectively) to be connected to the controller and two more pins (GND and VCC) for the power supply.
On the left side, there are 3 output pins (also CLK, CS, and DOUT) to be connected to other module inputs if the number of LEDs/pixels is to be increased.
There are also two power supply pins (GND and VCC) to make connection easier.

![](images/MAX72xx_LED_matrix.png?raw=true)

***Figure 1. MAX72xx LED Matrix Module***

&nbsp;

- ### `DMD_MAX72xx::TYPE_TBLR`

***Table 1. MAX72xx LED Matrix Module configured as `DMD_MAX72xx::TYPE_TBLR`***

| &#8681;&#8680; | Byte-0 | Byte-1 | Byte-2 | Byte-3 | Byte-4 | Byte-5 | Byte-6 | Byte-7 |
| :-------: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| **bit-0** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-1** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-2** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-3** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-4** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-5** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-6** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-7** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |

&nbsp;

- ### `DMD_MAX72xx::TYPE_TBRL`

***Table 2. MAX72xx LED Matrix Module configured as `DMD_MAX72xx::TYPE_TBRL`***

| &#8681;&#8678; | Byte-7 | Byte-6 | Byte-5 | Byte-4 | Byte-3 | Byte-2 | Byte-1 | Byte-0 |
| :-------: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| **bit-0** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-1** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-2** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-3** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-4** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-5** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-6** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-7** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |

&nbsp;

- ### `DMD_MAX72xx::TYPE_BTLR`

***Table 3. MAX72xx LED Matrix Module configured as `DMD_MAX72xx::TYPE_BTLR`***

| &#8679;&#8680; | Byte-0 | Byte-1 | Byte-2 | Byte-3 | Byte-4 | Byte-5 | Byte-6 | Byte-7 |
| :-------: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| **bit-7** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-6** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-5** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-4** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-3** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-2** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-1** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-0** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |

&nbsp;

- ### `DMD_MAX72xx::TYPE_BTRL`

***Table 4. MAX72xx LED Matrix Module configured as `DMD_MAX72xx::TYPE_BTRL`***

| &#8679;&#8678; | Byte-7 | Byte-6 | Byte-5 | Byte-4 | Byte-3 | Byte-2 | Byte-1 | Byte-0 |
| :-------: | :----: | :----: | :----: | :----: | :----: | :----: | :----: | :----: |
| **bit-7** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-6** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-5** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-4** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-3** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-2** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-1** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |
| **bit-0** |   O    |   O    |   O    |   O    |   O    |   O    |   O    |   O    |

&nbsp;

- ### `DMD_MAX72xx::TYPE_LRTB`

***Table 5. MAX72xx LED Matrix Module configured as `DMD_MAX72xx::TYPE_LRTB`***

| &#8680;&#8681; | bit-0 | bit-1 | bit-2 | bit-3 | bit-4 | bit-5 | bit-6 | bit-7 |
| :--------: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Byte-0** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-1** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-2** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-3** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-4** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-5** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-6** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-7** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |

&nbsp;

- ### `DMD_MAX72xx::TYPE_LRBT`

***Table 6. MAX72xx LED Matrix Module configured as `DMD_MAX72xx::TYPE_LRBT`***

| &#8680;&#8679; | bit-0 | bit-1 | bit-2 | bit-3 | bit-4 | bit-5 | bit-6 | bit-7 |
| :--------: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Byte-7** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-6** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-5** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-4** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-3** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-2** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-1** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-0** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |

&nbsp;

- ### `DMD_MAX72xx::TYPE_RLTB`

***Table 7. MAX72xx LED Matrix Module configured as `DMD_MAX72xx::TYPE_RLTB`***

| &#8678;&#8681; | bit-7 | bit-6 | bit-5 | bit-4 | bit-3 | bit-2 | bit-1 | bit-0 |
| :--------: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Byte-0** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-1** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-2** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-3** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-4** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-5** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-6** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-7** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |

&nbsp;

- ### `DMD_MAX72xx::TYPE_RLBT`

***Table 8. MAX72xx LED Matrix Module configured as `DMD_MAX72xx::TYPE_RLBT`***

| &#8678;&#8679; | bit-7 | bit-6 | bit-5 | bit-4 | bit-3 | bit-2 | bit-1 | bit-0 |
| :--------: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: |
| **Byte-7** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-6** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-5** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-4** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-3** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-2** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-1** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |
| **Byte-0** |   O   |   O   |   O   |   O   |   O   |   O   |   O   |   O   |

&nbsp;

## How to interfacing MAX72xx LED Matrix Module with AVR board

&nbsp;

![](images/interfacing-MAX72xx_LED_matrix-with-AVR.png?raw=true)

***Figure 2. Interfacing MAX72xx LED Matrix Module with AVR board***

&nbsp;

## How to interfacing MAX72xx LED Matrix Module with ESP8266 board

&nbsp;

![](images/interfacing-MAX72xx_LED_matrix-with-ESP8266.png?raw=true)

***Figure 3. Interfacing MAX72xx LED Matrix Module with ESP8266 board***

&nbsp;

## How to interfacing MAX72xx LED Matrix Module with STM32 board

&nbsp;

![](images/interfacing-MAX72xx_LED_matrix-with-STM32.png?raw=true)

***Figure 4. Interfacing MAX72xx LED Matrix Module with STM32 board***

&nbsp;

## How to cascading MAX72xx LED Matrix Module in one row

&nbsp;

![](images/cascading-MAX72xx_LED_matrix.png?raw=true)

***Figure 5. Cascading MAX72xx LED Matrix Module in one row***

&nbsp;

## How to cascading MAX72xx LED Matrix Module in multi rows

&nbsp;

![](images/cascading-multi-rows-MAX72xx_LED_matrix.png?raw=true)

***Figure 6. Cascading MAX72xx LED Matrix Module in multi rows***

&nbsp;

&nbsp;

---
### Created by: ZulNs
#### Gorontalo, 15 October 2019
---
