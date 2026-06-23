/**
  ******************************************************************************
  * @file    app_controller.h
  * @brief   Top-level application scheduler for panel, telemetry and GUI.
  ******************************************************************************
  */
#ifndef APP_CONTROLLER_H
#define APP_CONTROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief Initialize application services after all MCU peripherals are ready.
  */
void AppController_Init(void);

/**
  * @brief Run one foreground scheduler iteration from the main loop.
  */
void AppController_Run(void);

/**
  * @brief Update 1 ms panel input tasks from TIM6 interrupt context.
  */
void AppController_TimerTick(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_CONTROLLER_H */
