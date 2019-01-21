/*
 * definitions.h
 *
 * Created: 01/11/2016 17:26:23
 *  Author: Muhammed Zia Dawood
 */ 


#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_




#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

//#include "WString.h"
//#define bit_is_set(reg,bit) (_SFR_BYTE(ref) & _BV(bit))

#define Y_A        23                     // the pin connected to the wire A of the coil A (or to the H-bridge pin controlling the same wire)
#define Y_A_bar    22                     // the pin connected to the wire A- of the coil A (or to the H-bridge pin controlling the same wire)
#define Y_B        21                    // the pin connected to the wire B of the coil A (or to the H-bridge pin controlling the same wire)
#define Y_B_bar    20                    // the pin connected to the wire B- of the coil A (or to the H-bridge pin controlling the same wire)
#define Y_Enable   19//14


#define X_A       31     // 3                      // the pin connected to the wire A of the coil A (or to the H-bridge pin controlling the same wire)
#define X_A_bar   30     //  4                     // the pin connected to the wire A- of the coil A (or to the H-bridge pin controlling the same wire)
#define X_B       29     //  5                     // the pin connected to the wire B of the coil A (or to the H-bridge pin controlling the same wire)
#define X_B_bar   28     // 6                     // the pin connected to the wire B- of the coil A (or to the H-bridge pin controlling the same wire)
#define X_Enable  27     //7
//#define X_Max_Pin  17 //  c1

#define X_Max_Pin  17
#define X_Max_Pin_Avr  PINC //  c1
#define X_Max_Pin_No 1   //  c1

#define X_Min_Pin  18
#define X_Min_Pin_Avr PINC //  c2
#define X_Min_Pin_No  2  //  c2

#define Y_Max_Pin  15     //d7
#define Y_Max_Pin_Avr  PIND //  d7
#define Y_Max_Pin_No 7   //  d7

#define Y_Min_Pin  16    //c0
#define Y_Min_Pin_Avr  PINC //  c0
#define Y_Min_Pin_No 0   //  c2

#define stepsPerRevolution 200

#define servo_up_value 270;
#define servo_down_value 130;
#define in_coms_size 64



#endif /* DEFINITIONS_H_ */