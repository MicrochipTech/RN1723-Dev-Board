/* 
 * File:   self_test.h
 * Author: c13839
 *
 * Created on August 27, 2015, 2:13 PM
 */

#ifndef SELF_TEST_H
#define	SELF_TEST_H

void self_test(void);
BOOL EEPROM_selfTest(void);
BOOL Flash_selfTest(void);
BOOL RN1723_selfTest(void);
BOOL LEDs_selfTest(void);
BOOL Potentiometer_selfTest(void);
BOOL TempSensor_selfTest(void);
BOOL TiltVibe_selfTest(void);
void ConsoleMsgCallback_selfTest(void);
BOOL Configure_RN1723(void);
#endif	/* SELF_TEST_H */

