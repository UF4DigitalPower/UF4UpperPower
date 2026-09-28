/**   ******************************************************************************   * @file    gui_render.c   * @brief   UF4 GUI module.   ******************************************************************************   */
#include "gui_internal.h"
#include "g474_remote.h"
#include "uf4com.h"

typedef struct
{
  uint8_t active;
  ui_page_t from_page;
  ui_page_t to_page;
  int8_t direction;
  uint16_t step;
  uint32_t to_signature;
} ui_page_transition_t;

static ui_page_transition_t s_page_transition;

static uint32_t signature_mix(uint32_t signature, uint32_t value)
{
  return (signature * 16777619UL) ^ value;
}

static uint32_t signature_mix_float(uint32_t signature, float value)
{
  uint32_t value_bits = 0U;

  memcpy(&value_bits, &value, sizeof(value_bits));
  return signature_mix(signature, value_bits);
}

static uint32_t signature_mix_param(uint32_t signature, uint16_t id)
{
  float value = 0.0f;

  (void)GUI_ParamGet(id, &value);
  return signature_mix_float(signature, value);
}

static uint32_t page_render_signature(ui_page_t page)
{
  static const uint16_t status_ids[] = {
    UF4_ID_POWER_STATE,
    UF4_ID_CC_CV_MODE,
    UF4_ID_POWER_CONVERTER_MODE,
    UF4_ID_POWER_DIRECTION_STATUS,
    UF4_ID_FAULT_STATE
  };
  static const uint16_t home_ids[] = {
    UF4_ID_INPUT_VOLTAGE,
    UF4_ID_INPUT_CURRENT,
    UF4_ID_OUTPUT_VOLTAGE,
    UF4_ID_OUTPUT_CURRENT,
    UF4_ID_CORE_TEMPERATURE,
    UF4_ID_TEMP1_TEMPERATURE,
    UF4_ID_TEMP2_TEMPERATURE,
    UF4_ID_FAN_SET_VALUE
  };
  uint32_t signature = signature_mix(2166136261UL, (uint32_t)page);
  uint16_t i;

  signature = signature_mix(signature, g_ui.output_on);
  signature = signature_mix(signature, G474_Remote_IsOnline());

  for (i = 0U; i < (uint16_t)(sizeof(status_ids) / sizeof(status_ids[0])); ++i)
  {
    signature = signature_mix_param(signature, status_ids[i]);
  }

  if (page == UI_PAGE_HOME)
  {
    signature = signature_mix(signature, g_ui.step_index);
    signature = signature_mix_float(signature, g_ui.voltage_set);
    signature = signature_mix_float(signature, g_ui.current_limit);
    for (i = 0U; i < (uint16_t)(sizeof(home_ids) / sizeof(home_ids[0])); ++i)
    {
      signature = signature_mix_param(signature, home_ids[i]);
    }
  }
  else if (page == UI_PAGE_PARAMS)
  {
    uint16_t first = (uint16_t)(g_ui.param_page * UI_PARAMS_PER_PAGE);

    signature = signature_mix(signature, g_ui.param_page);
    signature = signature_mix(signature, g_ui.param_selected);
    for (i = first; i < UI_PARAM_COUNT && i < (uint16_t)(first + UI_PARAMS_PER_PAGE); ++i)
    {
      signature = signature_mix_float(signature, g_params[i].value);
    }
  }
  else if (page == UI_PAGE_SETTINGS)
  {
    signature = signature_mix(signature, g_ui.setting_page);
    signature = signature_mix(signature, g_ui.setting_selected);
    for (i = 0U; i < UI_SETTING_COUNT; ++i)
    {
      signature = signature_mix(signature, g_settings[i].current);
    }
  }
  else if (page == UI_PAGE_LOG)
  {
    signature = signature_mix(signature, g_ui.heartbeat_led);
    signature = signature_mix(signature, GUI_LogGeneration());
  }

  return signature;
}

void draw_content(void)
{
  switch (g_ui.page)
  {
  case UI_PAGE_HOME: draw_home_page(); break;
  case UI_PAGE_PARAMS: draw_params_page(); break;
  case UI_PAGE_SETTINGS: draw_settings_page(); break;
  case UI_PAGE_LOG: draw_log_page(); break;
  default: break;
  }
}

static uint32_t page_cache_addr(ui_page_t page)
{
  switch (page)
  {
  case UI_PAGE_PARAMS: return LCD_PAGE_CACHE1_ADDR;
  case UI_PAGE_SETTINGS: return LCD_PAGE_CACHE2_ADDR;
  case UI_PAGE_LOG: return LCD_PAGE_CACHE3_ADDR;
  case UI_PAGE_HOME:
  default: return LCD_PAGE_CACHE0_ADDR;
  }
}

void gui_draw_current_frame(void)
{
  LCD_Clear(UI_COLOR_BG);
  draw_header();
  draw_status_bar();
  draw_content();
  draw_nav();
}

static void render_page_cache(ui_page_t page)
{
  ui_page_t saved_page = g_ui.page;
  uint32_t saved_front;
  uint32_t saved_draw;

  LCD_WaitForIdle();
  saved_front = LCD_GetFrontBufferAddress();
  saved_draw = LCD_GetDrawBufferAddress();

  g_ui.page = page;
  LCD_SetFrameBuffers(saved_front, page_cache_addr(page));
  gui_draw_current_frame();
  LCD_SetFrameBuffers(saved_front, saved_draw);
  g_ui.page = saved_page;
}

static void snapshot_front_to_page_cache(ui_page_t page)
{
  uint32_t saved_front;
  uint32_t saved_draw;

  LCD_WaitForIdle();
  saved_front = LCD_GetFrontBufferAddress();
  saved_draw = LCD_GetDrawBufferAddress();

  LCD_SetFrameBuffers(saved_front, page_cache_addr(page));
  LCD_SyncRectFromFront(0U, 0U, UI_W, UI_H);
  LCD_SetFrameBuffers(saved_front, saved_draw);
}

static void refresh_page_cache_dynamic(ui_page_t page)
{
  ui_page_t saved_page = g_ui.page;
  uint32_t saved_front;
  uint32_t saved_draw;

  LCD_WaitForIdle();
  saved_front = LCD_GetFrontBufferAddress();
  saved_draw = LCD_GetDrawBufferAddress();

  g_ui.page = page;
  LCD_SetFrameBuffers(saved_front, page_cache_addr(page));
  draw_header_output();
  draw_status_bar();
  switch (page)
  {
  case UI_PAGE_HOME: draw_home_live_content(); break;
  case UI_PAGE_PARAMS: draw_params_page(); break;
  case UI_PAGE_SETTINGS: draw_settings_page(); break;
  case UI_PAGE_LOG: draw_log_page(); break;
  default: break;
  }
  LCD_SetFrameBuffers(saved_front, saved_draw);
  g_ui.page = saved_page;
}

void gui_render_all_page_caches(void)
{
  render_page_cache(UI_PAGE_HOME);
  render_page_cache(UI_PAGE_PARAMS);
  render_page_cache(UI_PAGE_SETTINGS);
  render_page_cache(UI_PAGE_LOG);
}

static void blit_page_slice(uint32_t page_addr, uint16_t src_x, uint16_t dst_x, uint16_t w)
{
  uint32_t src_addr;

  if (w == 0U)
  {
    return;
  }

  src_addr = page_addr + LCD_BYTES_PER_PIXEL * (uint32_t)src_x;
  LCD_BlitRGB565FromAddr(dst_x, 0U, w, UI_H, src_addr, UI_W);
}

static void blit_content_slice(uint32_t page_addr, uint16_t src_x, uint16_t dst_x, uint16_t w)
{
  uint32_t src_addr;

  if (w == 0U)
  {
    return;
  }

  src_addr = page_addr + LCD_BYTES_PER_PIXEL * ((uint32_t)UI_CONTENT_Y * UI_W + src_x);
  LCD_BlitRGB565FromAddr(dst_x, UI_CONTENT_Y, w, UI_CONTENT_H, src_addr, UI_W);
}

static void blit_top_bands(uint32_t page_addr)
{
  LCD_BlitRGB565FromAddr(0U, 0U, UI_W, UI_CONTENT_Y, page_addr, UI_W);
}

static void compose_content_slide_addr(uint32_t from_addr, uint32_t to_addr,
                                       uint16_t offset, int8_t dir)
{
  if (offset >= UI_W)
  {
    blit_content_slice(to_addr, 0U, 0U, UI_W);
    return;
  }

  if (dir > 0)
  {
    blit_content_slice(from_addr, offset, 0U, (uint16_t)(UI_W - offset));
    blit_content_slice(to_addr, 0U, (uint16_t)(UI_W - offset), offset);
  }
  else
  {
    blit_content_slice(to_addr, (uint16_t)(UI_W - offset), 0U, offset);
    blit_content_slice(from_addr, 0U, offset, (uint16_t)(UI_W - offset));
  }
}

static void compose_content_slide(ui_page_t from_page, ui_page_t to_page,
                                  uint16_t offset, int8_t dir)
{
  compose_content_slide_addr(page_cache_addr(from_page), page_cache_addr(to_page),
                             offset, dir);
}

void gui_present_cached_page(ui_page_t page)
{
  blit_page_slice(page_cache_addr(page), 0U, 0U, UI_W);
  LCD_CommitFrameFromDraw();
}

#define UI_SLIDE_STEPS       16U
#define UI_SLIDE_EASE_SCALE 1024U

static uint16_t slide_offset(uint16_t step)
{
  uint32_t t = (uint32_t)step * UI_SLIDE_EASE_SCALE / UI_SLIDE_STEPS;
  uint32_t eased = t * t * (3U * UI_SLIDE_EASE_SCALE - 2U * t) /
                   (UI_SLIDE_EASE_SCALE * UI_SLIDE_EASE_SCALE);

  return (uint16_t)(eased * UI_W / UI_SLIDE_EASE_SCALE);
}

static void draw_nav_sliding(ui_page_t from_page, ui_page_t to_page, uint16_t offset)
{
  static const char *labels[4] = { "HOME", "PARAMS", "SETTINGS", "LOG" };
  uint16_t from_x = (uint16_t)((uint16_t)from_page * 120U);
  uint16_t to_x   = (uint16_t)((uint16_t)to_page * 120U);
  uint16_t block_x;
  uint8_t i;

  block_x = (uint16_t)(from_x + (uint16_t)(((int32_t)to_x - (int32_t)from_x) * (int32_t)offset / (int32_t)UI_W));

  /* Paint all backgrounds first so the moving block never erases part of a label. */
  for (i = 0U; i < 4U; ++i)
  {
    uint16_t bx = (uint16_t)(i * 120U);

    draw_cell(bx, UI_NAV_Y, 120U, UI_NAV_H, UI_COLOR_BG);
  }

  /* Sliding dark selection block. */
  fill(block_x, UI_NAV_Y, 120U, UI_NAV_H, UI_COLOR_INK);

  /* Draw each label once after the block. */
  for (i = 0U; i < 4U; ++i)
  {
    uint16_t bx = (uint16_t)(i * 120U);
    uint16_t center = (uint16_t)(bx + 60U);
    uint16_t fg = (center >= block_x && center < (uint16_t)(block_x + 120U)) ?
                  UI_COLOR_BG : UI_COLOR_MUTED;

    draw_text_center(bx, UI_NAV_Y, 120U, UI_NAV_H, labels[i], 2U, fg);
  }
}

static void slide_to_page(ui_page_t next_page)
{
  ui_page_t from_page = g_ui.page;

  /* The visible frame is already current.  Snapshot it with DMA2D instead of
     rebuilding the outgoing page and all of its text before every slide. */
  snapshot_front_to_page_cache(from_page);
  render_page_cache(next_page);

  s_page_transition.active = 1U;
  s_page_transition.from_page = from_page;
  s_page_transition.to_page = next_page;
  s_page_transition.direction = (next_page > from_page) ? 1 : -1;
  s_page_transition.step = 0U;
  s_page_transition.to_signature = page_render_signature(next_page);
  g_ui.page = next_page;
}

uint8_t gui_page_transition_active(void)
{
  return s_page_transition.active;
}

uint8_t gui_page_transition_tick(void)
{
  uint32_t to_signature;
  uint16_t offset;

  if (s_page_transition.active == 0U)
  {
    return 0U;
  }

  to_signature = page_render_signature(s_page_transition.to_page);

  if (to_signature != s_page_transition.to_signature)
  {
    refresh_page_cache_dynamic(s_page_transition.to_page);
    s_page_transition.to_signature = page_render_signature(s_page_transition.to_page);
  }

  ++s_page_transition.step;
  offset = slide_offset(s_page_transition.step);

  /* Header / status stay fixed; the content and bottom nav slide together. */
  blit_top_bands(page_cache_addr(s_page_transition.to_page));
  compose_content_slide(s_page_transition.from_page, s_page_transition.to_page,
                        offset, s_page_transition.direction);
  draw_nav_sliding(s_page_transition.from_page, s_page_transition.to_page, offset);
  if (s_page_transition.step < UI_SLIDE_STEPS)
  {
    LCD_CommitFrameFromDrawNoSync();
    return 0U;
  }

  LCD_CommitFrameFromDraw();
  s_page_transition.active = 0U;
  return 1U;
}

void gui_draw_full(void)
{
  gui_draw_current_frame();
  LCD_CommitFrameFromDraw();
}

void redraw_content_nav(void)
{
  fill(0U, UI_CONTENT_Y, UI_W, (uint16_t)(UI_H - UI_CONTENT_Y), UI_COLOR_BG);
  draw_content();
  draw_nav();
  LCD_CommitRectFromDraw(0U, UI_CONTENT_Y, UI_W, (uint16_t)(UI_H - UI_CONTENT_Y));
}

void redraw_header_content_nav(void)
{
  fill(0U, 0U, UI_W, UI_HEADER_H, UI_COLOR_BG);
  fill(0U, UI_CONTENT_Y, UI_W, (uint16_t)(UI_H - UI_CONTENT_Y), UI_COLOR_BG);
  draw_header();
  draw_content();
  draw_nav();
  {
    const lcd_rect_t rects[2] = {
      {0U, 0U, UI_W, UI_HEADER_H},
      {0U, UI_CONTENT_Y, UI_W, (uint16_t)(UI_H - UI_CONTENT_Y)}
    };
    LCD_CommitRectsFromDraw(rects, 2U);
  }
}

void sync_dirty(uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
  LCD_CommitRectFromDraw(x, y, w, h);
}

void sync_dirty_rects(const ui_rect_t *rects, uint8_t count)
{
  uint8_t i;
  uint8_t dirty_count = 0U;
  lcd_rect_t dirty[LCD_MAX_DIRTY_RECTS];

  if (rects == 0)
  {
    return;
  }

  if (count > LCD_MAX_DIRTY_RECTS)
  {
    LCD_CommitFrameFromDraw();
    return;
  }

  for (i = 0U; i < count; ++i)
  {
    if (rects[i].w != 0U && rects[i].h != 0U)
    {
      dirty[dirty_count].x = rects[i].x;
      dirty[dirty_count].y = rects[i].y;
      dirty[dirty_count].w = rects[i].w;
      dirty[dirty_count].h = rects[i].h;
      ++dirty_count;
    }
  }
  LCD_CommitRectsFromDraw(dirty, dirty_count);
}

void sync_dirty_union(uint16_t x1, uint16_t y1, uint16_t w1, uint16_t h1,
                      uint16_t x2, uint16_t y2, uint16_t w2, uint16_t h2)
{
  uint16_t left = (x1 < x2) ? x1 : x2;
  uint16_t top = (y1 < y2) ? y1 : y2;
  uint16_t right1 = (uint16_t)(x1 + w1);
  uint16_t right2 = (uint16_t)(x2 + w2);
  uint16_t bottom1 = (uint16_t)(y1 + h1);
  uint16_t bottom2 = (uint16_t)(y2 + h2);
  uint16_t right = (right1 > right2) ? right1 : right2;
  uint16_t bottom = (bottom1 > bottom2) ? bottom1 : bottom2;

  sync_dirty(left, top, (uint16_t)(right - left), (uint16_t)(bottom - top));
}

void sync_dirty_union3(uint16_t x1, uint16_t y1, uint16_t w1, uint16_t h1,
                       uint16_t x2, uint16_t y2, uint16_t w2, uint16_t h2,
                       uint16_t x3, uint16_t y3, uint16_t w3, uint16_t h3)
{
  uint16_t left = (x1 < x2) ? x1 : x2;
  uint16_t top = (y1 < y2) ? y1 : y2;
  uint16_t right1 = (uint16_t)(x1 + w1);
  uint16_t right2 = (uint16_t)(x2 + w2);
  uint16_t bottom1 = (uint16_t)(y1 + h1);
  uint16_t bottom2 = (uint16_t)(y2 + h2);
  uint16_t right = (right1 > right2) ? right1 : right2;
  uint16_t bottom = (bottom1 > bottom2) ? bottom1 : bottom2;
  uint16_t right3 = (uint16_t)(x3 + w3);
  uint16_t bottom3 = (uint16_t)(y3 + h3);

  if (x3 < left) { left = x3; }
  if (y3 < top) { top = y3; }
  if (right3 > right) { right = right3; }
  if (bottom3 > bottom) { bottom = bottom3; }
  sync_dirty(left, top, (uint16_t)(right - left), (uint16_t)(bottom - top));
}

void draw_header(void)
{
  const char *small = "UF4 DIGITAL POWER";
  const char *large = "DC POWER SUPPLY";
  if (g_ui.page == UI_PAGE_PARAMS)
  {
    small = "LOOP PARAMETERS";
    large = "PID / TRANSFER";
  }
  else if (g_ui.page == UI_PAGE_SETTINGS)
  {
    small = "SYSTEM SETTINGS";
    large = "DEVICE CONFIG";
  }
  else if (g_ui.page == UI_PAGE_LOG)
  {
    small = "RUNTIME MONITOR";
    large = "EVENT LOG";
  }

  draw_cell(0U, 0U, UI_W, UI_HEADER_H, UI_COLOR_BG);
  draw_text(16U, 14U, small, 2U, UI_COLOR_MUTED);
  draw_text(16U, 40U, large, 3U, UI_COLOR_TEXT);
  draw_header_output();
}

void draw_header_output(void)
{
  draw_cell(362U, 0U, 118U, UI_HEADER_H, g_ui.output_on ? UI_COLOR_INK : UI_COLOR_DISABLED);
  draw_text_center(362U, 0U, 118U, UI_HEADER_H, g_ui.output_on ? "ON" : "OFF", 2U,
                   g_ui.output_on ? UI_COLOR_BG : UI_COLOR_MUTED);
}

void draw_status_bar(void)
{
  const char *items[5];
  float power_state = 0.0f;
  float cc_cv_mode = 0.0f;
  float converter_mode = 0.0f;
  float direction_status = 0.0f;
  float fault_state = 0.0f;
  uint8_t i;

  (void)GUI_ParamGet(UF4_ID_POWER_STATE, &power_state);
  (void)GUI_ParamGet(UF4_ID_CC_CV_MODE, &cc_cv_mode);
  (void)GUI_ParamGet(UF4_ID_POWER_CONVERTER_MODE, &converter_mode);
  (void)GUI_ParamGet(UF4_ID_POWER_DIRECTION_STATUS, &direction_status);
  (void)GUI_ParamGet(UF4_ID_FAULT_STATE, &fault_state);

  switch ((uint16_t)power_state)
  {
  case 0U: items[0] = "BOOT"; break;
  case 1U: items[0] = "HW INIT"; break;
  case 2U: items[0] = "PARAM"; break;
  case 3U: items[0] = "CHECK"; break;
  case 4U: items[0] = "IDLE"; break;
  case 5U: items[0] = "START"; break;
  case 6U: items[0] = "RUN"; break;
  case 7U: items[0] = "STOP"; break;
  case 8U: items[0] = "SWITCH"; break;
  case 9U: items[0] = "FAULT"; break;
  default: items[0] = "FAULT"; break;
  }
  items[1] = (cc_cv_mode != 0.0f) ? "CC" : "CV";
  switch ((uint16_t)converter_mode)
  {
  case 0U: items[2] = "BUCK"; break;
  case 1U: items[2] = "MIX"; break;
  default: items[2] = "BOOST"; break;
  }
  items[3] = (direction_status != 0.0f) ? "REVERSE" : "FORWARD";
  if (G474_Remote_IsOnline() == 0U) { items[4] = "COMM"; }
  else if (((uint16_t)fault_state & 0x0001U) != 0U) { items[4] = "OCP"; }
  else if (((uint16_t)fault_state & 0x0002U) != 0U) { items[4] = "VIN OVP"; }
  else if (((uint16_t)fault_state & 0x0004U) != 0U) { items[4] = "VOUT OVP"; }
  else if (((uint16_t)fault_state & 0x0008U) != 0U) { items[4] = "OTP"; }
  else if (((uint16_t)fault_state & 0x0010U) != 0U) { items[4] = "HW FLT"; }
  else if (((uint16_t)fault_state & 0x0020U) != 0U) { items[4] = "VIN UVP"; }
  else if (((uint16_t)fault_state & 0x0040U) != 0U) { items[4] = "ISR OVR"; }
  else { items[4] = "NORMAL"; }

  for (i = 0U; i < 5U; ++i)
  {
    uint16_t bg = ((i == 0U && g_ui.output_on) || (i == 4U && fault_state != 0.0f)) ?
                  UI_COLOR_ACCENT : UI_COLOR_PANEL;
    uint16_t fg = ((i == 0U && g_ui.output_on) || (i == 4U && fault_state != 0.0f)) ?
                  UI_COLOR_INK : UI_COLOR_MUTED;
    draw_cell((uint16_t)(i * 96U), UI_STATUS_Y, 96U, UI_STATUS_H, bg);
    draw_text_center((uint16_t)(i * 96U), UI_STATUS_Y, 96U, UI_STATUS_H, items[i], 1U, fg);
  }
}

void draw_nav(void)
{
  draw_button(0U, UI_NAV_Y, 120U, UI_NAV_H, "HOME", (uint8_t)(g_ui.page == UI_PAGE_HOME), 0U);
  draw_button(120U, UI_NAV_Y, 120U, UI_NAV_H, "PARAMS", (uint8_t)(g_ui.page == UI_PAGE_PARAMS), 0U);
  draw_button(240U, UI_NAV_Y, 120U, UI_NAV_H, "SETTINGS", (uint8_t)(g_ui.page == UI_PAGE_SETTINGS), 0U);
  draw_button(360U, UI_NAV_Y, 120U, UI_NAV_H, "LOG", (uint8_t)(g_ui.page == UI_PAGE_LOG), 0U);
}

uint8_t handle_nav_touch(uint16_t x, uint16_t y)
{
  ui_page_t next_page = g_ui.page;
  if (y < UI_NAV_Y)
  {
    return 0U;
  }
  if (x < 120U) { next_page = UI_PAGE_HOME; }
  else if (x < 240U) { next_page = UI_PAGE_PARAMS; }
  else if (x < 360U) { next_page = UI_PAGE_SETTINGS; }
  else { next_page = UI_PAGE_LOG; }

  if (next_page != g_ui.page)
  {
    GUI_LogPrintf("PAGE %u -> %u", (unsigned)g_ui.page, (unsigned)next_page);
    slide_to_page(next_page);
  }
  return 1U;
}

void draw_log_page(void)
{
  char line[GUI_LOG_LINE_LEN];
  uint16_t count = GUI_LogCount();
  uint16_t first = (count > 22U) ? (uint16_t)(count - 22U) : 0U;
  uint16_t i;

  draw_cell(0U, UI_CONTENT_Y, UI_W, UI_CONTENT_H, UI_COLOR_BG);
  draw_text(16U, 132U, "LIVE EVENTS", 2U, UI_COLOR_TEXT);
  draw_cell(428U, 130U, 20U, 20U, g_ui.heartbeat_led ? UI_COLOR_ACCENT : UI_COLOR_DISABLED);
  border(428U, 130U, 20U, 20U, UI_COLOR_BORDER);
  for (i = first; i < count; ++i)
  {
    if (GUI_LogGetLine(i, line, sizeof(line)) != 0U)
    {
      draw_text(12U, (uint16_t)(164U + (i - first) * 26U), line, 1U, UI_COLOR_TEXT);
    }
  }
  if (count == 0U) { draw_text(16U, 190U, "NO EVENTS", 2U, UI_COLOR_MUTED); }
}

