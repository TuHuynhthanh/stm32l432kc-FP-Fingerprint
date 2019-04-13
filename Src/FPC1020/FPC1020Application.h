/*
 * FPC1020Application.h
 *
 *  Created on: Mar 18, 2019
 *      Author: tuhuy
 */

#ifndef FPC1020_FPC1020APPLICATION_H_
#define FPC1020_FPC1020APPLICATION_H_


#include "stm32l4xx_hal.h"
#include "string.h"
#include "Define.h"
#include "stdbool.h"

#define DEBUG(a) HAL_UART_Transmit(debugTerminal,(uint8_t*)a,strlen(a),1000)


extern UART_HandleTypeDef *debugTerminal;
extern int g_nMaxFpCount;
extern volatile bool rx2Flag;
void appEnroll();
void deleteID(int userID);
void deleteAllID();
void upTemplate(int m_nUserID); //up template from fingerboard to array [m_abyTemplate1]
void downTemplate(int m_nUserID); //down template from array [m_abyTemplate1]  to finger board

int  getEmptyId();
int fingerIdentify();

#endif /* FPC1020_FPC1020APPLICATION_H_ */
