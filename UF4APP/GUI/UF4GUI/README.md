# UF4GUI

UF4GUI is a small production-oriented C GUI layer for STM32H743ZI, RGB565 TFT LCD, double buffering and industrial instruments such as digital power supplies, battery testers, electronic loads and oscilloscopes.

## Directory

```text
UF4GUI/
├── core/        # init, tick, dirty rectangles, refresh, theme selection
├── draw/        # RGB565 primitives, text, images, optional alpha blend helpers
├── font/        # bitmap font adapters; project font assets live in UF4APP/GUI/Font
├── widget/      # Label, Button, Switch, Slider, ProgressBar, WaveView, Menu, Window
├── animation/   # fixed-slot animation engine
├── page/        # static page registration and switching
└── theme/       # theme module boundary
```

## Core Object Model

- `GUI_Color`: RGB565 pixel value.
- `GUI_Rect`: signed `x/y/w/h` rectangle used by layout, clipping and dirty refresh.
- `GUI_Event`: input/update event package. `type` identifies touch/key/click/value change, `x/y` carry pointer position, `value` carries key/value payload, `tick` carries caller time, `target` is optional.
- `GUI_Object`: base object with rectangle, visibility, enable state, invalid flag and application-owned `user_data`.
- `GUI_Widget`: drawable/control object. It embeds `GUI_Object`, widget type, draw callback, event callback, parent/children links and a fixed union payload for all built-in controls.
- `GUI_Page`: screen-level unit with id, name, root widget tree and enter/leave/update/draw callbacks.

## Rendering Pipeline

1. Application updates model values and calls widget setters or `GUI_InvalidateRect`.
2. `GUI_Refresh` iterates dirty rectangles.
3. For each dirty rectangle, front buffer content is copied to draw buffer through `LCD_CopyRectFromFrontToDraw`.
4. Current page and widget tree redraw only the invalid area.
5. `LCD_Present` swaps draw/front buffers during vertical blanking.

The existing BSP provides the RGB565 framebuffer, DMA2D fill/copy and LTDC presentation. UF4GUI deliberately does not allocate memory and does not require RTOS services.

## Controls

- Label: draws static or caller-owned dynamic text.
- Button: pressed state, touch-up click callback.
- Switch: binary output control.
- Slider: integer range and drag updates.
- ProgressBar: integer range display, suitable for animated progress.
- WaveView: voltage/current/temperature/power curves in fixed ring buffers.
- Menu: fixed item list with selected index.
- Window: framed industrial panel with title bar.

## Pages

Supported APIs:

- `GUI_Page_Register`
- `GUI_Page_Switch`
- `GUI_Page_Update`
- `GUI_Page_Draw`

The power application interface implements:

- Home page
- Parameter setup page
- Waveform page
- System information page

## Fonts

`GUI_DrawString` uses the project `Bender_24x24` bitmap font for large scale text (`scale >= 3`) and the compact built-in 5x7 font for small labels and controls.

## Animation

`GUI_Anim_Start` and `GUI_Anim_Update` use `GUI_MAX_ANIM_COUNT` static slots. Use `on_step` to map values to alpha, x/y position, scale, numeric text or progress bar values.

Supported animation categories:

- Fade
- Slide
- Scale
- Number rolling
- Progress bar animation

## Theme

Built-in themes:

- Dark
- Light
- Industrial

The industrial theme uses restrained high-contrast colors inspired by Rohde & Schwarz, Keysight, Rigol, Siglent and EA Elektro-Automatik equipment styling.

## Main Loop Example

```c
GUI_PowerApp_Init();

while (1) {
    GUI_PowerApp_Update(meas_mv, meas_ma, meas_mw, temp_c10, cc_mode);
    GUI_Tick(10);
    GUI_Refresh();
}
```

For a 200 kHz power-control loop, keep GUI calls in a lower-rate cooperative UI task, typically 20-100 Hz. The control ISR/main loop can write measurements to a lock-free snapshot; the UI consumes the latest snapshot.
