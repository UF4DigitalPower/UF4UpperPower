/**
  ******************************************************************************
  * @file    setpoint_input.h
  * @author  UF4
  * @brief   Encoder driven VSET/ISET input.
  ******************************************************************************
  */
#ifndef SETPOINT_INPUT_H
#define SETPOINT_INPUT_H

#include <stdint.h>

void SetpointInput_Init(void);
void SetpointInput_Update(void);

float SetpointInput_GetVset(void);
float SetpointInput_GetIset(void);

uint8_t SetpointInput_GetVsetDigit(void);
uint8_t SetpointInput_GetIsetDigit(void);

#endif /* SETPOINT_INPUT_H */
