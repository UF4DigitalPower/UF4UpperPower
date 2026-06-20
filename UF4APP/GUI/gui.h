#ifndef UF4GUI_GUI_H
#define UF4GUI_GUI_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GUI_SCREEN_WIDTH
#define GUI_SCREEN_WIDTH  640U
#endif

#ifndef GUI_SCREEN_HEIGHT
#define GUI_SCREEN_HEIGHT 480U
#endif

#define GUI_MAX_DIRTY_RECTS     16U
#define GUI_MAX_PAGE_COUNT      8U
#define GUI_MAX_ANIM_COUNT      12U
#define GUI_MAX_CHILDREN        16U
#define GUI_WAVE_MAX_POINTS     256U
#define GUI_MENU_MAX_ITEMS      8U

typedef uint16_t GUI_Color;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
} GUI_Rect;

typedef enum {
    GUI_EVENT_NONE = 0,
    GUI_EVENT_DRAW,
    GUI_EVENT_UPDATE,
    GUI_EVENT_TOUCH_DOWN,
    GUI_EVENT_TOUCH_MOVE,
    GUI_EVENT_TOUCH_UP,
    GUI_EVENT_KEY,
    GUI_EVENT_VALUE_CHANGED,
    GUI_EVENT_CLICKED
} GUI_EventType;

typedef struct {
    GUI_EventType type;
    int16_t x;
    int16_t y;
    int16_t value;
    uint32_t tick;
    void *target;
} GUI_Event;

typedef enum {
    GUI_WIDGET_LABEL = 0,
    GUI_WIDGET_BUTTON,
    GUI_WIDGET_SWITCH,
    GUI_WIDGET_SLIDER,
    GUI_WIDGET_PROGRESS,
    GUI_WIDGET_WAVE,
    GUI_WIDGET_MENU,
    GUI_WIDGET_WINDOW
} GUI_WidgetType;

typedef struct GUI_Widget GUI_Widget;

typedef void (*GUI_WidgetDrawFn)(GUI_Widget *widget, const GUI_Rect *clip);
typedef void (*GUI_WidgetEventFn)(GUI_Widget *widget, const GUI_Event *event);

typedef struct {
    GUI_Rect rect;             /* Local rectangle in parent coordinate space. */
    uint8_t visible;           /* 0 hides object and skips hit-test/draw. */
    uint8_t enabled;           /* 0 keeps drawing but ignores input events. */
    uint8_t invalid;           /* Set when object needs redraw. */
    void *user_data;           /* Application-owned pointer, never allocated by GUI. */
} GUI_Object;

struct GUI_Widget {
    GUI_Object obj;            /* Base geometry and common state. */
    GUI_WidgetType type;       /* Runtime widget discriminator. */
    GUI_WidgetDrawFn draw;     /* Widget renderer. */
    GUI_WidgetEventFn event;   /* Optional input/update handler. */
    GUI_Widget *children[GUI_MAX_CHILDREN];
    uint8_t child_count;
    GUI_Widget *parent;
    union {
        struct {
            const char *text;
            GUI_Color color;
            uint8_t scale;
        } label;
        struct {
            const char *text;
            GUI_Color bg;
            GUI_Color fg;
            uint8_t pressed;
            void (*on_click)(GUI_Widget *widget);
        } button;
        struct {
            uint8_t checked;
            void (*on_change)(GUI_Widget *widget, uint8_t checked);
        } sw;
        struct {
            int16_t min;
            int16_t max;
            int16_t value;
            uint8_t dragging;
            void (*on_change)(GUI_Widget *widget, int16_t value);
        } slider;
        struct {
            int16_t min;
            int16_t max;
            int16_t value;
            GUI_Color bar_color;
        } progress;
        struct {
            int16_t data_v[GUI_WAVE_MAX_POINTS];
            int16_t data_i[GUI_WAVE_MAX_POINTS];
            int16_t data_t[GUI_WAVE_MAX_POINTS];
            int16_t data_p[GUI_WAVE_MAX_POINTS];
            uint16_t head;
            uint16_t count;
            int16_t min;
            int16_t max;
            uint8_t show_voltage;
            uint8_t show_current;
            uint8_t show_temp;
            uint8_t show_power;
        } wave;
        struct {
            const char *items[GUI_MENU_MAX_ITEMS];
            uint8_t count;
            uint8_t selected;
            void (*on_select)(GUI_Widget *widget, uint8_t index);
        } menu;
        struct {
            const char *title;
            GUI_Color bg;
        } window;
    } data;
};

typedef struct {
    uint8_t id;                /* Stable page id used by GUI_Page_Switch. */
    const char *name;          /* Debug/readability name. */
    GUI_Widget *root;          /* Root widget tree. */
    void (*on_enter)(void);
    void (*on_leave)(void);
    void (*on_update)(uint32_t elapsed_ms);
    void (*on_draw)(const GUI_Rect *clip);
} GUI_Page;

typedef struct {
    GUI_Color bg;
    GUI_Color panel;
    GUI_Color text;
    GUI_Color muted;
    GUI_Color border;
    GUI_Color accent;
    GUI_Color accent2;
    GUI_Color danger;
    GUI_Color warning;
    GUI_Color success;
} GUI_Theme;

typedef enum {
    GUI_THEME_DARK = 0,
    GUI_THEME_LIGHT,
    GUI_THEME_INDUSTRIAL
} GUI_ThemeId;

typedef enum {
    GUI_ANIM_FADE = 0,
    GUI_ANIM_SLIDE_X,
    GUI_ANIM_SLIDE_Y,
    GUI_ANIM_SCALE,
    GUI_ANIM_NUMBER,
    GUI_ANIM_PROGRESS
} GUI_AnimType;

typedef struct {
    uint8_t active;
    GUI_AnimType type;
    GUI_Widget *target;
    int32_t from;
    int32_t to;
    int32_t value;
    uint32_t duration_ms;
    uint32_t elapsed_ms;
    void (*on_step)(GUI_Widget *target, int32_t value);
    void (*on_done)(GUI_Widget *target);
} GUI_Anim;

void GUI_Init(void);
void GUI_Tick(uint32_t elapsed_ms);
void GUI_DispatchEvent(const GUI_Event *event);
void GUI_InvalidateRect(const GUI_Rect *rect);
void GUI_InvalidateWidget(GUI_Widget *widget);
void GUI_Refresh(void);
void GUI_SetTheme(GUI_ThemeId id);
const GUI_Theme *GUI_GetTheme(void);

GUI_Color GUI_RGB565(uint8_t r, uint8_t g, uint8_t b);
GUI_Color GUI_Blend565(GUI_Color fg, GUI_Color bg, uint8_t alpha);

void GUI_DrawPixel(int16_t x, int16_t y, GUI_Color color);
void GUI_DrawSetClip(const GUI_Rect *clip);
void GUI_DrawResetClip(void);
void GUI_DrawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, GUI_Color color);
void GUI_DrawRect(const GUI_Rect *rect, GUI_Color color);
void GUI_FillRect(const GUI_Rect *rect, GUI_Color color);
void GUI_DrawCircle(int16_t x0, int16_t y0, int16_t r, GUI_Color color);
void GUI_DrawString(int16_t x, int16_t y, const char *text, GUI_Color color, uint8_t scale);
void GUI_DrawImage(int16_t x, int16_t y, uint16_t w, uint16_t h, const GUI_Color *pixels);

void GUI_Widget_Init(GUI_Widget *widget, GUI_WidgetType type, const GUI_Rect *rect);
void GUI_Widget_AddChild(GUI_Widget *parent, GUI_Widget *child);
void GUI_Widget_DrawTree(GUI_Widget *widget, const GUI_Rect *clip);
void GUI_Widget_DispatchTree(GUI_Widget *widget, const GUI_Event *event);
void GUI_Label_Create(GUI_Widget *w, const GUI_Rect *r, const char *text);
void GUI_Button_Create(GUI_Widget *w, const GUI_Rect *r, const char *text, void (*on_click)(GUI_Widget *));
void GUI_Switch_Create(GUI_Widget *w, const GUI_Rect *r, uint8_t checked);
void GUI_Slider_Create(GUI_Widget *w, const GUI_Rect *r, int16_t min, int16_t max, int16_t value);
void GUI_Progress_Create(GUI_Widget *w, const GUI_Rect *r, int16_t min, int16_t max, int16_t value);
void GUI_Menu_Create(GUI_Widget *w, const GUI_Rect *r, const char **items, uint8_t count);
void GUI_Window_Create(GUI_Widget *w, const GUI_Rect *r, const char *title);

void GUI_WaveView_Create(GUI_Widget *w, const GUI_Rect *r, int16_t min, int16_t max);
void GUI_WaveView_Push(GUI_Widget *w, int16_t voltage, int16_t current, int16_t temp, int16_t power);

uint8_t GUI_Page_Register(GUI_Page *page);
uint8_t GUI_Page_Switch(uint8_t id);
void GUI_Page_Update(uint32_t elapsed_ms);
void GUI_Page_Draw(const GUI_Rect *clip);
GUI_Page *GUI_Page_Current(void);

GUI_Anim *GUI_Anim_Start(GUI_AnimType type, GUI_Widget *target, int32_t from, int32_t to,
                         uint32_t duration_ms, void (*on_step)(GUI_Widget *, int32_t),
                         void (*on_done)(GUI_Widget *));
void GUI_Anim_Update(uint32_t elapsed_ms);

void GUI_DemoPower_Init(void);
void GUI_DemoPower_Update(int32_t mv, int32_t ma, int32_t mw, int32_t temp_c10, uint8_t cc_mode);

#ifdef __cplusplus
}
#endif

#endif
