/**   ***************************
 *   * @file    gui_tim.c
 *   * @brief   UF4 GUI module.
 *   ***************************  */

#include "gui_internal.h"
#include "g474_remote.h"

/* TIM7: 90 MHz / 5000 / 180 = 100 Hz (10 ms). */
#define GUI_TIMER_PERIOD_MS    10U
#define GUI_HANDLER_PERIOD_MS  10U
#define GUI_TELEMETRY_REFRESH_MS  30U  // GUI遥测刷新毫秒
#define GUI_TOUCH_IRQ_WINDOW_MS 150U
#define GUI_HEARTBEAT_PERIOD_MS 500U

static uint32_t g_boot_ready_tick;
static uint8_t g_boot_popup_done;
static volatile uint32_t g_gui_tick_ms;
static volatile uint8_t g_gui_handler_pending;
static volatile uint8_t g_gui_handler_elapsed_ms;
static volatile uint8_t g_touch_irq_pending;
static volatile uint8_t g_touch_irq_seen;
static volatile uint32_t g_touch_irq_tick;
static uint32_t g_home_status_signature;
static uint8_t g_home_output_state;
static uint32_t g_home_telemetry_signature;
static uint32_t g_heartbeat_tick;
static uint32_t g_log_generation_seen;

static uint32_t home_telemetry_signature(void)
{
  static const uint16_t ids[] = {
    UF4_ID_INPUT_VOLTAGE,
    UF4_ID_INPUT_CURRENT,
    UF4_ID_OUTPUT_VOLTAGE,
    UF4_ID_OUTPUT_CURRENT,
    UF4_ID_CORE_TEMPERATURE,
    UF4_ID_TEMP1_TEMPERATURE,
    UF4_ID_TEMP2_TEMPERATURE,
    UF4_ID_FAN_SET_VALUE,
    UF4_ID_FAULT_STATE,
    UF4_ID_POWER_STATE,
    UF4_ID_CC_CV_MODE,
    UF4_ID_POWER_CONVERTER_MODE
  };
  uint32_t signature = 2166136261UL;
  uint8_t i;

  for (i = 0U; i < (uint8_t)(sizeof(ids) / sizeof(ids[0])); ++i)
  {
    float value = 0.0f;
    (void)GUI_ParamGet(ids[i], &value);
    signature = (signature * 16777619UL) ^ (uint16_t)(value * 100.0f);
  }
  return (signature * 16777619UL) ^ G474_Remote_IsOnline();
}

static uint32_t home_status_signature(void)
{
  static const uint16_t ids[] = {
    UF4_ID_POWER_STATE,
    UF4_ID_CC_CV_MODE,
    UF4_ID_POWER_CONVERTER_MODE,
    UF4_ID_POWER_DIRECTION_STATUS,
    UF4_ID_FAULT_STATE
  };
  uint32_t signature = 2166136261UL;
  uint8_t i;

  for (i = 0U; i < (uint8_t)(sizeof(ids) / sizeof(ids[0])); ++i)
  {
    float value = 0.0f;
    (void)GUI_ParamGet(ids[i], &value);
    signature = (signature * 16777619UL) ^ (uint16_t)value;
  }
  return (signature * 16777619UL) ^ G474_Remote_IsOnline();
}

static void home_banner_tracking_sync(void)
{
  g_home_output_state = g_ui.output_on;
  g_home_status_signature = home_status_signature();
}

static void handle_touch_down(uint16_t x, uint16_t y)
{
  g_ui.repeat_target = 0U;
  if (pt_in(x, y, 362U, 0U, 118U, UI_HEADER_H))
  {
    /* This is the F429's local desired state.  G474 telemetry is display
       feedback only and must never decide the next output command. */
    const uint8_t output_on = (uint8_t)!G474_Remote_GetOutputCommanded();

    if (UI_Esp32ModeIsActive() != 0U)
    {
      GUI_PopupShow("CONTROL LOCKED", "ESP32 SERVICE ACTIVE", 1500U);
      return;
    }

    if(G474_Remote_Write(UF4_ID_OUTPUT_ENABLE, output_on) == 0U)
    {
      GUI_LogWrite("OUTPUT COMMAND FAILED");
      GUI_PopupShow("OUTPUT", "COMMAND NOT SENT", 1500U);
      return;
    }
    GUI_LogPrintf("OUTPUT %s REQUEST", output_on != 0U ? "ON" : "OFF");
    return;
  }
  if (handle_nav_touch(x, y) != 0U)
  {
    if (g_ui.page == UI_PAGE_HOME)
    {
      home_banner_tracking_sync();
    }
    return;
  }
  switch (g_ui.page)
  {
  case UI_PAGE_HOME: handle_home_touch(x, y, 1U); break;
  case UI_PAGE_PARAMS: handle_params_touch(x, y, 1U); break;
  case UI_PAGE_SETTINGS: handle_settings_touch(x, y, 1U); break;
  case UI_PAGE_LOG: break;
  default: break;
  }
}

static void handle_repeat(uint32_t now)
{
  if (g_ui.repeat_target == 0U || (uint32_t)(now - g_ui.down_tick) < 450U ||
      (uint32_t)(now - g_ui.last_repeat_tick) < 90U)
  {
    return;
  }

  g_ui.last_repeat_tick = now;
  if (g_ui.repeat_target >= 1U && g_ui.repeat_target <= 4U)
  {
    handle_home_repeat(g_ui.repeat_target);
  }
  else if (g_ui.repeat_target == 5U)
  {
    handle_params_repeat(g_ui.repeat_target);
  }
  else if (g_ui.repeat_target == 6U)
  {
    handle_params_repeat(g_ui.repeat_target);
  }
}

static void clear_hover(void)
{
  if (g_ui.hover_target == UI_HOVER_HOME_VDEC || g_ui.hover_target == UI_HOVER_HOME_VINC ||
      g_ui.hover_target == UI_HOVER_HOME_IDEC || g_ui.hover_target == UI_HOVER_HOME_IINC)
  {
    handle_home_button_hover(g_ui.hover_target, 0U);
  }
  else if (g_ui.hover_target == UI_HOVER_HOME_PRESET_SELECT || g_ui.hover_target == UI_HOVER_HOME_PRESET_SAVE ||
           g_ui.hover_target == UI_HOVER_HOME_PRESET_APPLY)
  {
    handle_home_preset_hover(g_ui.hover_target, 0U);
  }
  else if (g_ui.hover_target == UI_HOVER_PARAM_DEC || g_ui.hover_target == UI_HOVER_PARAM_INC)
  {
    handle_params_button_hover(g_ui.hover_target, 0U);
  }
  else if (g_ui.hover_target == UI_HOVER_SETTING_ACTION)
  {
    handle_settings_action_hover(0U);
  }

  g_ui.hover_target = UI_HOVER_NONE;
}

void GUI_Init(void)
{
  gui_draw_init();
  g_gui_tick_ms = 0U;
  g_gui_handler_pending = 0U;
  g_gui_handler_elapsed_ms = 0U;
  g_touch_irq_pending = 0U;
  g_touch_irq_seen = 0U;
  g_touch_irq_tick = 0U;
  g_boot_ready_tick = 0U;
  g_boot_popup_done = 0U;
  GUI_LogInit();
  GUI_LogWrite("SYSTEM READY");
  g_heartbeat_tick = 0U;
  g_log_generation_seen = GUI_LogGeneration();
  ui_load_color_preset();
  FT5X16_Init();
  gui_render_all_page_caches();
  gui_present_cached_page(UI_PAGE_HOME);
  home_banner_tracking_sync();
  g_home_telemetry_signature = home_telemetry_signature();
}

void GUI_TouchIrqNotify(void)
{
  /* The controller's INT may be a pulse instead of a level held for the
     entire contact.  Timestamp it in the ISR and do the I2C transaction from
     GUI_Handler(). */
  g_touch_irq_tick = g_gui_tick_ms;
  g_touch_irq_seen = 1U;
  g_touch_irq_pending = 1U;
}

void GUI_Tick(void)
{
  g_gui_tick_ms += GUI_TIMER_PERIOD_MS;
  g_gui_handler_elapsed_ms = (uint8_t)(g_gui_handler_elapsed_ms + GUI_TIMER_PERIOD_MS);
  if (g_gui_handler_elapsed_ms >= GUI_HANDLER_PERIOD_MS)
  {
    g_gui_handler_elapsed_ms = (uint8_t)(g_gui_handler_elapsed_ms - GUI_HANDLER_PERIOD_MS);
    g_gui_handler_pending = 1U;
  }
}

static uint8_t gui_take_handler_tick(uint32_t *now)
{
  uint32_t primask;

  if (now == 0)
  {
    return 0U;
  }

  primask = __get_PRIMASK();
  __disable_irq();
  if (g_gui_handler_pending == 0U)
  {
    if (primask == 0U)
    {
      __enable_irq();
    }
    return 0U;
  }

  *now = g_gui_tick_ms;
  g_gui_handler_pending = 0U;
  if (primask == 0U)
  {
    __enable_irq();
  }

  return 1U;
}

void GUI_Handler(void)
{
  FT5X16_Point_t points[FT5X16_MAX_POINTS];
  uint8_t count = 0U;
  uint8_t touch_sample_due;
  uint32_t now;
  static uint32_t last_touch_reinit_tick = 0U;
  static uint32_t last_telemetry_refresh_tick = 0U;

  if (gui_take_handler_tick(&now) == 0U)
  {
    return;
  }

  if ((uint32_t)(now - g_heartbeat_tick) >= GUI_HEARTBEAT_PERIOD_MS)
  {
    g_heartbeat_tick = now;
    g_ui.heartbeat_led = (uint8_t)!g_ui.heartbeat_led;
    if (g_ui.page == UI_PAGE_LOG && gui_page_transition_active() == 0U)
    {
      draw_cell(428U, 130U, 20U, 20U,
                g_ui.heartbeat_led ? UI_COLOR_ACCENT : UI_COLOR_DISABLED);
      border(428U, 130U, 20U, 20U, UI_COLOR_BORDER);
      sync_dirty(428U, 130U, 20U, 20U);
    }
  }

  if (gui_page_transition_active() != 0U)
  {
    if (gui_page_transition_tick() != 0U)
    {
      if (g_ui.page == UI_PAGE_HOME)
      {
        home_banner_tracking_sync();
        g_home_telemetry_signature = home_telemetry_signature();
      }
      if (g_ui.page == UI_PAGE_LOG)
      {
        g_log_generation_seen = GUI_LogGeneration();
      }
    }
    return;
  }

  handle_params_save_result();
  UI_Params_RefreshRemote();

  /* Do not infer a new touch from TD_STATUS alone: the panel can retain stale
     point bytes while idle.  A falling-edge notification opens a bounded
     sampling window.  The active-low level remains a fallback for panels
     which hold INT for the duration of contact. */
  touch_sample_due = (uint8_t)(g_ui.touch_down != 0U ||
                               g_touch_irq_pending != 0U ||
                               FT5X16_IsTouched() != 0U ||
                               (g_touch_irq_seen != 0U &&
                                (uint32_t)(now - g_touch_irq_tick) < GUI_TOUCH_IRQ_WINDOW_MS));
  g_touch_irq_pending = 0U;

  if (touch_sample_due == 0U)
  {
    clear_hover();
    g_ui.touch_down = 0U;
    g_ui.repeat_target = 0U;
    goto popup_tick;
  }

  if (FT5X16_ReadPoints(points, &count) != HAL_OK)
  {
    if ((uint32_t)(now - last_touch_reinit_tick) >= 500U)
    {
      (void)FT5X16_Init();
      last_touch_reinit_tick = now;
    }
    clear_hover();
    g_ui.touch_down = 0U;
    g_ui.repeat_target = 0U;
    goto popup_tick;
  }

  if (count == 0U)
  {
    clear_hover();
    g_ui.touch_down = 0U;
    g_ui.repeat_target = 0U;
    goto popup_tick;
  }

  g_ui.touch_x = points[0].x;
  g_ui.touch_y = points[0].y;

  /* FT5X16 reports a final point with EVENT_UP.  Treating that point as a
     new press was enough to turn an intermittent I2C/touch event into an
     output-enable toggle. */
  if (points[0].event == FT5X16_EVENT_UP)
  {
    clear_hover();
    g_ui.touch_down = 0U;
    g_ui.repeat_target = 0U;
    goto popup_tick;
  }

  if (g_ui.touch_down == 0U)
  {
    g_ui.touch_down = 1U;
    g_ui.down_tick = now;
    g_ui.last_repeat_tick = now;
    handle_touch_down(g_ui.touch_x, g_ui.touch_y);
  }
  else
  {
    handle_repeat(now);
  }

popup_tick:
  if (gui_page_transition_active() != 0U)
  {
    return;
  }

  if (g_boot_popup_done == 0U && (uint32_t)(now - g_boot_ready_tick) >= 2000U &&
      ui_popup_is_active() == 0U)
  {
    g_boot_popup_done = 1U;
    GUI_PopupShow("BOOT OK", "SYSTEM READY", 3000U);
  }
  ui_popup_tick(now);

  if (g_ui.page == UI_PAGE_LOG && GUI_LogGeneration() != g_log_generation_seen)
  {
    g_log_generation_seen = GUI_LogGeneration();
    draw_log_page();
    sync_dirty(0U, UI_CONTENT_Y, UI_W, UI_CONTENT_H);
  }

  /* The G474 telemetry updates g_ui asynchronously.  Keep this to the live
     regions so the LCD update cannot starve the 50 ms serial polling cycle. */
  if (g_ui.page == UI_PAGE_HOME && g_ui.touch_down == 0U &&
      (uint32_t)(now - last_telemetry_refresh_tick) >= GUI_TELEMETRY_REFRESH_MS)
  {
    ui_rect_t telemetry_rects[4];
    uint8_t rect_count = 0U;
    uint32_t status_signature = home_status_signature();
    uint32_t telemetry_signature = home_telemetry_signature();

    last_telemetry_refresh_tick = now;
    if (g_ui.output_on != g_home_output_state)
    {
      draw_header_output();
      telemetry_rects[rect_count++] = (ui_rect_t){362U, 0U, 118U, UI_HEADER_H};
      g_home_output_state = g_ui.output_on;
    }
    if (status_signature != g_home_status_signature)
    {
      draw_status_bar();
      telemetry_rects[rect_count++] = (ui_rect_t){0U, UI_STATUS_Y, UI_W, UI_STATUS_H};
      g_home_status_signature = status_signature;
    }
    if (telemetry_signature != g_home_telemetry_signature)
    {
      draw_home_telemetry();
      telemetry_rects[rect_count++] = (ui_rect_t){0U, UI_CONTENT_Y, UI_W, 268U};
      telemetry_rects[rect_count++] = (ui_rect_t){0U, 586U, UI_W, 162U};
      g_home_telemetry_signature = telemetry_signature;
    }
    sync_dirty_rects(telemetry_rects, rect_count);
  }
}
