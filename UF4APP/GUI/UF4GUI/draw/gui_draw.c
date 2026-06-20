#include "gui.h"
#include "bsp_lcd.h"

static GUI_Rect g_clip = {0, 0, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT};

static const uint8_t g_font5x7[96][5] = {
    {0,0,0,0,0},{0,0,0x5F,0,0},{0,7,0,7,0},{0x14,0x7F,0x14,0x7F,0x14},{0x24,0x2A,0x7F,0x2A,0x12},{0x23,0x13,8,0x64,0x62},{0x36,0x49,0x55,0x22,0x50},{0,5,3,0,0},{0,0x1C,0x22,0x41,0},{0,0x41,0x22,0x1C,0},{0x14,8,0x3E,8,0x14},{8,8,0x3E,8,8},{0,0x50,0x30,0,0},{8,8,8,8,8},{0,0x60,0x60,0,0},{0x20,0x10,8,4,2},
    {0x3E,0x51,0x49,0x45,0x3E},{0,0x42,0x7F,0x40,0},{0x42,0x61,0x51,0x49,0x46},{0x21,0x41,0x45,0x4B,0x31},{0x18,0x14,0x12,0x7F,0x10},{0x27,0x45,0x45,0x45,0x39},{0x3C,0x4A,0x49,0x49,0x30},{1,0x71,9,5,3},{0x36,0x49,0x49,0x49,0x36},{6,0x49,0x49,0x29,0x1E},{0,0x36,0x36,0,0},{0,0x56,0x36,0,0},{8,0x14,0x22,0x41,0},{0x14,0x14,0x14,0x14,0x14},{0,0x41,0x22,0x14,8},{2,1,0x51,9,6},
    {0x32,0x49,0x79,0x41,0x3E},{0x7E,0x11,0x11,0x11,0x7E},{0x7F,0x49,0x49,0x49,0x36},{0x3E,0x41,0x41,0x41,0x22},{0x7F,0x41,0x41,0x22,0x1C},{0x7F,0x49,0x49,0x49,0x41},{0x7F,9,9,9,1},{0x3E,0x41,0x49,0x49,0x7A},{0x7F,8,8,8,0x7F},{0,0x41,0x7F,0x41,0},{0x20,0x40,0x41,0x3F,1},{0x7F,8,0x14,0x22,0x41},{0x7F,0x40,0x40,0x40,0x40},{0x7F,2,0x0C,2,0x7F},{0x7F,4,8,0x10,0x7F},{0x3E,0x41,0x41,0x41,0x3E},
    {0x7F,9,9,9,6},{0x3E,0x41,0x51,0x21,0x5E},{0x7F,9,0x19,0x29,0x46},{0x46,0x49,0x49,0x49,0x31},{1,1,0x7F,1,1},{0x3F,0x40,0x40,0x40,0x3F},{0x1F,0x20,0x40,0x20,0x1F},{0x3F,0x40,0x38,0x40,0x3F},{0x63,0x14,8,0x14,0x63},{7,8,0x70,8,7},{0x61,0x51,0x49,0x45,0x43},{0,0x7F,0x41,0x41,0},{2,4,8,0x10,0x20},{0,0x41,0x41,0x7F,0},{4,2,1,2,4},{0x40,0x40,0x40,0x40,0x40},
    {0,1,2,4,0},{0x20,0x54,0x54,0x54,0x78},{0x7F,0x48,0x44,0x44,0x38},{0x38,0x44,0x44,0x44,0x20},{0x38,0x44,0x44,0x48,0x7F},{0x38,0x54,0x54,0x54,0x18},{8,0x7E,9,1,2},{0x0C,0x52,0x52,0x52,0x3E},{0x7F,8,4,4,0x78},{0,0x44,0x7D,0x40,0},{0x20,0x40,0x44,0x3D,0},{0x7F,0x10,0x28,0x44,0},{0,0x41,0x7F,0x40,0},{0x7C,4,0x18,4,0x78},{0x7C,8,4,4,0x78},{0x38,0x44,0x44,0x44,0x38},
    {0x7C,0x14,0x14,0x14,8},{8,0x14,0x14,0x18,0x7C},{0x7C,8,4,4,8},{0x48,0x54,0x54,0x54,0x20},{4,0x3F,0x44,0x40,0x20},{0x3C,0x40,0x40,0x20,0x7C},{0x1C,0x20,0x40,0x20,0x1C},{0x3C,0x40,0x30,0x40,0x3C},{0x44,0x28,0x10,0x28,0x44},{0x0C,0x50,0x50,0x50,0x3C},{0x44,0x64,0x54,0x4C,0x44},{0,8,0x36,0x41,0},{0,0,0x7F,0,0},{0,0x41,0x36,8,0},{0x10,8,8,0x10,8},{0}
};

GUI_Color GUI_RGB565(uint8_t r, uint8_t g, uint8_t b)
{
    return (GUI_Color)(((r & 0xF8U) << 8) | ((g & 0xFCU) << 3) | (b >> 3));
}

GUI_Color GUI_Blend565(GUI_Color fg, GUI_Color bg, uint8_t alpha)
{
    uint32_t fr = (fg >> 11) & 0x1FU;
    uint32_t fg6 = (fg >> 5) & 0x3FU;
    uint32_t fb = fg & 0x1FU;
    uint32_t br = (bg >> 11) & 0x1FU;
    uint32_t bg6 = (bg >> 5) & 0x3FU;
    uint32_t bb = bg & 0x1FU;
    uint32_t r = (fr * alpha + br * (255U - alpha)) / 255U;
    uint32_t g = (fg6 * alpha + bg6 * (255U - alpha)) / 255U;
    uint32_t b = (fb * alpha + bb * (255U - alpha)) / 255U;
    return (GUI_Color)((r << 11) | (g << 5) | b);
}

void GUI_DrawSetClip(const GUI_Rect *clip)
{
    g_clip = *clip;
    if (g_clip.x < 0) {
        g_clip.w = (int16_t)(g_clip.w + g_clip.x);
        g_clip.x = 0;
    }
    if (g_clip.y < 0) {
        g_clip.h = (int16_t)(g_clip.h + g_clip.y);
        g_clip.y = 0;
    }
    if ((int32_t)g_clip.x + g_clip.w > (int32_t)GUI_SCREEN_WIDTH) {
        g_clip.w = (int16_t)(GUI_SCREEN_WIDTH - (uint16_t)g_clip.x);
    }
    if ((int32_t)g_clip.y + g_clip.h > (int32_t)GUI_SCREEN_HEIGHT) {
        g_clip.h = (int16_t)(GUI_SCREEN_HEIGHT - (uint16_t)g_clip.y);
    }
}

void GUI_DrawResetClip(void)
{
    g_clip.x = 0;
    g_clip.y = 0;
    g_clip.w = GUI_SCREEN_WIDTH;
    g_clip.h = GUI_SCREEN_HEIGHT;
}

void GUI_DrawPixel(int16_t x, int16_t y, GUI_Color color)
{
    if ((x < 0) || (y < 0) || (x >= (int16_t)GUI_SCREEN_WIDTH) || (y >= (int16_t)GUI_SCREEN_HEIGHT)) {
        return;
    }
    if ((x < g_clip.x) || (y < g_clip.y) || (x >= (g_clip.x + g_clip.w)) || (y >= (g_clip.y + g_clip.h))) {
        return;
    }
    POINT_COLOR = color;
    LCD_DrawPoint((uint16_t)x, (uint16_t)y);
}

void GUI_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, GUI_Color color)
{
    int16_t dx = (x1 < x2) ? (int16_t)(x2 - x1) : (int16_t)(x1 - x2);
    int16_t sx = (x1 < x2) ? 1 : -1;
    int16_t dy = (y1 < y2) ? (int16_t)(y1 - y2) : (int16_t)(y2 - y1);
    int16_t sy = (y1 < y2) ? 1 : -1;
    int16_t err = (int16_t)(dx + dy);

    for (;;) {
        GUI_DrawPixel(x1, y1, color);
        if ((x1 == x2) && (y1 == y2)) {
            break;
        }
        if ((int16_t)(2 * err) >= dy) {
            err = (int16_t)(err + dy);
            x1 = (int16_t)(x1 + sx);
        }
        if ((int16_t)(2 * err) <= dx) {
            err = (int16_t)(err + dx);
            y1 = (int16_t)(y1 + sy);
        }
    }
}

void GUI_DrawRect(const GUI_Rect *rect, GUI_Color color)
{
    if ((rect->w <= 0) || (rect->h <= 0)) {
        return;
    }
    POINT_COLOR = color;
    LCD_DrawRectangle((uint16_t)rect->x, (uint16_t)rect->y,
                      (uint16_t)(rect->x + rect->w - 1), (uint16_t)(rect->y + rect->h - 1));
}

void GUI_FillRect(const GUI_Rect *rect, GUI_Color color)
{
    int16_t x = rect->x;
    int16_t y = rect->y;
    int16_t w = rect->w;
    int16_t h = rect->h;
    int16_t x2;
    int16_t y2;

    if (x < 0) { w = (int16_t)(w + x); x = 0; }
    if (y < 0) { h = (int16_t)(h + y); y = 0; }
    if ((int32_t)x + w > (int32_t)GUI_SCREEN_WIDTH) { w = (int16_t)(GUI_SCREEN_WIDTH - (uint16_t)x); }
    if ((int32_t)y + h > (int32_t)GUI_SCREEN_HEIGHT) { h = (int16_t)(GUI_SCREEN_HEIGHT - (uint16_t)y); }
    x2 = (int16_t)(x + w);
    y2 = (int16_t)(y + h);
    if (x < g_clip.x) { x = g_clip.x; }
    if (y < g_clip.y) { y = g_clip.y; }
    if (x2 > (g_clip.x + g_clip.w)) { x2 = (int16_t)(g_clip.x + g_clip.w); }
    if (y2 > (g_clip.y + g_clip.h)) { y2 = (int16_t)(g_clip.y + g_clip.h); }
    w = (int16_t)(x2 - x);
    h = (int16_t)(y2 - y);
    if ((w <= 0) || (h <= 0)) { return; }
    LCD_Rect_Fill((uint16_t)x, (uint16_t)y, (uint16_t)(x + w - 1), (uint16_t)(y + h - 1), color);
}

void GUI_DrawCircle(int16_t x0, int16_t y0, int16_t r, GUI_Color color)
{
    POINT_COLOR = color;
    LCD_Draw_Circle((uint16_t)x0, (uint16_t)y0, (uint8_t)r);
}

void GUI_DrawString(int16_t x, int16_t y, const char *text, GUI_Color color, uint8_t scale)
{
    int16_t cursor = x;
    uint8_t s = (scale == 0U) ? 1U : scale;

    while ((text != 0) && (*text != '\0')) {
        uint8_t ch = (uint8_t)*text++;
        const uint8_t *glyph = (ch >= 32U && ch < 128U) ? g_font5x7[ch - 32U] : g_font5x7[0];
        uint8_t col;
        for (col = 0U; col < 5U; ++col) {
            uint8_t bits = glyph[col];
            uint8_t row;
            for (row = 0U; row < 7U; ++row) {
                if ((bits & (1U << row)) != 0U) {
                    GUI_Rect p = {(int16_t)(cursor + col * s), (int16_t)(y + row * s), s, s};
                    GUI_FillRect(&p, color);
                }
            }
        }
        cursor = (int16_t)(cursor + 6 * s);
    }
}

void GUI_DrawImage(int16_t x, int16_t y, uint16_t w, uint16_t h, const GUI_Color *pixels)
{
    if ((pixels == 0) || (w == 0U) || (h == 0U)) {
        return;
    }
    if ((x < g_clip.x) || (y < g_clip.y) || ((x + (int16_t)w) > (g_clip.x + g_clip.w)) ||
        ((y + (int16_t)h) > (g_clip.y + g_clip.h))) {
        uint16_t row;
        uint16_t col;
        for (row = 0U; row < h; ++row) {
            for (col = 0U; col < w; ++col) {
                GUI_DrawPixel((int16_t)(x + col), (int16_t)(y + row), pixels[(uint32_t)row * w + col]);
            }
        }
        return;
    }
    LCD_Color_Fill((uint16_t)x, (uint16_t)y, (uint16_t)(x + w - 1U), (uint16_t)(y + h - 1U), (uint16_t *)pixels);
}
