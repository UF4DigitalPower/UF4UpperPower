# Copyright 2026 NXP
# NXP Proprietary. This software is owned or controlled by NXP and may only be used strictly in
# accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
# activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
# comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
# terms, then you may not retain, install, activate or otherwise use the software.

import SDL
import utime as time
import usys as sys
import lvgl as lv
import lodepng as png
import ustruct
import fs_driver

lv.init()
SDL.init(w=480,h=640)

# Register SDL display driver.
disp_buf1 = lv.disp_draw_buf_t()
buf1_1 = bytearray(480*640*4)
disp_buf1.init(buf1_1, None, len(buf1_1)//4)
disp_drv = lv.disp_drv_t()
disp_drv.init()
disp_drv.draw_buf = disp_buf1
disp_drv.flush_cb = SDL.monitor_flush
disp_drv.hor_res = 480
disp_drv.ver_res = 640
disp_drv.register()

# Regsiter SDL mouse driver
indev_drv = lv.indev_drv_t()
indev_drv.init()
indev_drv.type = lv.INDEV_TYPE.POINTER
indev_drv.read_cb = SDL.mouse_read
indev_drv.register()

fs_drv = lv.fs_drv_t()
fs_driver.fs_register(fs_drv, 'Z')

# Below: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

COLOR_SIZE = lv.color_t.__SIZE__
COLOR_IS_SWAPPED = hasattr(lv.color_t().ch,'green_h')

class lodepng_error(RuntimeError):
    def __init__(self, err):
        if type(err) is int:
            super().__init__(png.error_text(err))
        else:
            super().__init__(err)

# Parse PNG file header
# Taken from https://github.com/shibukawa/imagesize_py/blob/ffef30c1a4715c5acf90e8945ceb77f4a2ed2d45/imagesize.py#L63-L85

def get_png_info(decoder, src, header):
    # Only handle variable image types

    if lv.img.src_get_type(src) != lv.img.SRC.VARIABLE:
        return lv.RES.INV

    data = lv.img_dsc_t.__cast__(src).data
    if data == None:
        return lv.RES.INV

    png_header = bytes(data.__dereference__(24))

    if png_header.startswith(b'\211PNG\r\n\032\n'):
        if png_header[12:16] == b'IHDR':
            start = 16
        # Maybe this is for an older PNG version.
        else:
            start = 8
        try:
            width, height = ustruct.unpack(">LL", png_header[start:start+8])
        except ustruct.error:
            return lv.RES.INV
    else:
        return lv.RES.INV

    header.always_zero = 0
    header.w = width
    header.h = height
    header.cf = lv.img.CF.TRUE_COLOR_ALPHA

    return lv.RES.OK

def convert_rgba8888_to_bgra8888(img_view):
    for i in range(0, len(img_view), lv.color_t.__SIZE__):
        ch = lv.color_t.__cast__(img_view[i:i]).ch
        ch.red, ch.blue = ch.blue, ch.red

# Read and parse PNG file

def open_png(decoder, dsc):
    img_dsc = lv.img_dsc_t.__cast__(dsc.src)
    png_data = img_dsc.data
    png_size = img_dsc.data_size
    png_decoded = png.C_Pointer()
    png_width = png.C_Pointer()
    png_height = png.C_Pointer()
    error = png.decode32(png_decoded, png_width, png_height, png_data, png_size)
    if error:
        raise lodepng_error(error)
    img_size = png_width.int_val * png_height.int_val * 4
    img_data = png_decoded.ptr_val
    img_view = img_data.__dereference__(img_size)

    if COLOR_SIZE == 4:
        convert_rgba8888_to_bgra8888(img_view)
    else:
        raise lodepng_error("Error: Color mode not supported yet!")

    dsc.img_data = img_data
    return lv.RES.OK

# Above: Taken from https://github.com/lvgl/lv_binding_micropython/blob/master/driver/js/imagetools.py#L22-L94

decoder = lv.img.decoder_create()
decoder.info_cb = get_png_info
decoder.open_cb = open_png

def anim_x_cb(obj, v):
    obj.set_x(v)

def anim_y_cb(obj, v):
    obj.set_y(v)

def anim_width_cb(obj, v):
    obj.set_width(v)

def anim_height_cb(obj, v):
    obj.set_height(v)

def anim_img_zoom_cb(obj, v):
    obj.set_zoom(v)

def anim_img_rotate_cb(obj, v):
    obj.set_angle(v)

global_font_cache = {}
def test_font(font_family, font_size):
    global global_font_cache
    if font_family + str(font_size) in global_font_cache:
        return global_font_cache[font_family + str(font_size)]
    if font_size % 2:
        candidates = [
            (font_family, font_size),
            (font_family, font_size-font_size%2),
            (font_family, font_size+font_size%2),
            ("montserrat", font_size-font_size%2),
            ("montserrat", font_size+font_size%2),
            ("montserrat", 16)
        ]
    else:
        candidates = [
            (font_family, font_size),
            ("montserrat", font_size),
            ("montserrat", 16)
        ]
    for (family, size) in candidates:
        try:
            if eval(f'lv.font_{family}_{size}'):
                global_font_cache[font_family + str(font_size)] = eval(f'lv.font_{family}_{size}')
                if family != font_family or size != font_size:
                    print(f'WARNING: lv.font_{family}_{size} is used!')
                return eval(f'lv.font_{family}_{size}')
        except AttributeError:
            try:
                load_font = lv.font_load(f"Z:MicroPython/lv_font_{family}_{size}.fnt")
                global_font_cache[font_family + str(font_size)] = load_font
                return load_font
            except:
                if family == font_family and size == font_size:
                    print(f'WARNING: lv.font_{family}_{size} is NOT supported!')

global_image_cache = {}
def load_image(file):
    global global_image_cache
    if file in global_image_cache:
        return global_image_cache[file]
    try:
        with open(file,'rb') as f:
            data = f.read()
    except:
        print(f'Could not open {file}')
        sys.exit()

    img = lv.img_dsc_t({
        'data_size': len(data),
        'data': data
    })
    global_image_cache[file] = img
    return img

def calendar_event_handler(e,obj):
    code = e.get_code()

    if code == lv.EVENT.VALUE_CHANGED:
        source = e.get_current_target()
        date = lv.calendar_date_t()
        if source.get_pressed_date(date) == lv.RES.OK:
            source.set_highlighted_dates([date], 1)

def spinbox_increment_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.increment()
def spinbox_decrement_event_cb(e, obj):
    code = e.get_code()
    if code == lv.EVENT.SHORT_CLICKED or code == lv.EVENT.LONG_PRESSED_REPEAT:
        obj.decrement()

def digital_clock_cb(timer, obj, current_time, show_second, use_ampm):
    hour = int(current_time[0])
    minute = int(current_time[1])
    second = int(current_time[2])
    ampm = current_time[3]
    second = second + 1
    if second == 60:
        second = 0
        minute = minute + 1
        if minute == 60:
            minute = 0
            hour = hour + 1
            if use_ampm:
                if hour == 12:
                    if ampm == 'AM':
                        ampm = 'PM'
                    elif ampm == 'PM':
                        ampm = 'AM'
                if hour > 12:
                    hour = hour % 12
    hour = hour % 24
    if use_ampm:
        if show_second:
            obj.set_text("%d:%02d:%02d %s" %(hour, minute, second, ampm))
        else:
            obj.set_text("%d:%02d %s" %(hour, minute, ampm))
    else:
        if show_second:
            obj.set_text("%d:%02d:%02d" %(hour, minute, second))
        else:
            obj.set_text("%d:%02d" %(hour, minute))
    current_time[0] = hour
    current_time[1] = minute
    current_time[2] = second
    current_time[3] = ampm

def analog_clock_cb(timer, obj):
    datetime = time.localtime()
    hour = datetime[3]
    if hour >= 12: hour = hour - 12
    obj.set_time(hour, datetime[4], datetime[5])

def datetext_event_handler(e, obj):
    code = e.get_code()
    target = e.get_target()
    if code == lv.EVENT.FOCUSED:
        if obj is None:
            bg = lv.layer_top()
            bg.add_flag(lv.obj.FLAG.CLICKABLE)
            obj = lv.calendar(bg)
            scr = target.get_screen()
            scr_height = scr.get_height()
            scr_width = scr.get_width()
            obj.set_size(int(scr_width * 0.8), int(scr_height * 0.8))
            datestring = target.get_text()
            year = int(datestring.split('/')[0])
            month = int(datestring.split('/')[1])
            day = int(datestring.split('/')[2])
            obj.set_showed_date(year, month)
            highlighted_days=[lv.calendar_date_t({'year':year, 'month':month, 'day':day})]
            obj.set_highlighted_dates(highlighted_days, 1)
            obj.align(lv.ALIGN.CENTER, 0, 0)
            lv.calendar_header_arrow(obj)
            obj.add_event_cb(lambda e: datetext_calendar_event_handler(e, target), lv.EVENT.ALL, None)
            scr.update_layout()

def datetext_calendar_event_handler(e, obj):
    code = e.get_code()
    target = e.get_current_target()
    if code == lv.EVENT.VALUE_CHANGED:
        date = lv.calendar_date_t()
        if target.get_pressed_date(date) == lv.RES.OK:
            obj.set_text(f"{date.year}/{date.month}/{date.day}")
            bg = lv.layer_top()
            bg.clear_flag(lv.obj.FLAG.CLICKABLE)
            bg.set_style_bg_opa(lv.OPA.TRANSP, 0)
            target.delete()

# Create PAGE_MAIN
PAGE_MAIN = lv.obj()
PAGE_MAIN.set_size(480, 640)
PAGE_MAIN.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN.set_style_bg_color(lv.color_hex(0xcccccc), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_cont_1
PAGE_MAIN_cont_1 = lv.obj(PAGE_MAIN)
PAGE_MAIN_cont_1.set_pos(16, 57)
PAGE_MAIN_cont_1.set_size(220, 140)
PAGE_MAIN_cont_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN_cont_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_cont_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_radius(6, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create PAGE_MAIN_label_7
PAGE_MAIN_label_7 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_7.set_text("VIN:")
PAGE_MAIN_label_7.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_7.set_width(lv.pct(100))
PAGE_MAIN_label_7.set_pos(10, 10)
PAGE_MAIN_label_7.set_size(60, 32)
# Set style for PAGE_MAIN_label_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_8
PAGE_MAIN_label_8 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_8.set_text("IIN:")
PAGE_MAIN_label_8.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_8.set_width(lv.pct(100))
PAGE_MAIN_label_8.set_pos(10, 52)
PAGE_MAIN_label_8.set_size(60, 32)
# Set style for PAGE_MAIN_label_8, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_8.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_8.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_9
PAGE_MAIN_label_9 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_9.set_text("PIN:")
PAGE_MAIN_label_9.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_9.set_width(lv.pct(100))
PAGE_MAIN_label_9.set_pos(10, 94)
PAGE_MAIN_label_9.set_size(60, 32)
# Set style for PAGE_MAIN_label_9, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_9.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_9.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_cont_2
PAGE_MAIN_cont_2 = lv.obj(PAGE_MAIN_cont_1)
PAGE_MAIN_cont_2.set_pos(231, 0)
PAGE_MAIN_cont_2.set_size(220, 140)
PAGE_MAIN_cont_2.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN_cont_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_cont_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_radius(6, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create PAGE_MAIN_label_12
PAGE_MAIN_label_12 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_12.set_text("VOUT:")
PAGE_MAIN_label_12.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_12.set_width(lv.pct(100))
PAGE_MAIN_label_12.set_pos(10, 7)
PAGE_MAIN_label_12.set_size(80, 32)
# Set style for PAGE_MAIN_label_12, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_12.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_12.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_11
PAGE_MAIN_label_11 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_11.set_text("IOUT :")
PAGE_MAIN_label_11.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_11.set_width(lv.pct(100))
PAGE_MAIN_label_11.set_pos(10, 51)
PAGE_MAIN_label_11.set_size(80, 32)
# Set style for PAGE_MAIN_label_11, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_11.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_11.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_10
PAGE_MAIN_label_10 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_10.set_text("POUT:")
PAGE_MAIN_label_10.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_10.set_width(lv.pct(100))
PAGE_MAIN_label_10.set_pos(10, 95)
PAGE_MAIN_label_10.set_size(80, 32)
# Set style for PAGE_MAIN_label_10, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_10.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_10.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_16
PAGE_MAIN_label_16 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_16.set_text("00.00")
PAGE_MAIN_label_16.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_16.set_width(lv.pct(100))
PAGE_MAIN_label_16.set_pos(93, 7)
PAGE_MAIN_label_16.set_size(93, 32)
# Set style for PAGE_MAIN_label_16, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_16.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_16.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_17
PAGE_MAIN_label_17 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_17.set_text("00.00")
PAGE_MAIN_label_17.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_17.set_width(lv.pct(100))
PAGE_MAIN_label_17.set_pos(93, 51)
PAGE_MAIN_label_17.set_size(93, 32)
# Set style for PAGE_MAIN_label_17, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_17.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_17.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_18
PAGE_MAIN_label_18 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_18.set_text("00.00")
PAGE_MAIN_label_18.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_18.set_width(lv.pct(100))
PAGE_MAIN_label_18.set_pos(93, 95)
PAGE_MAIN_label_18.set_size(93, 32)
# Set style for PAGE_MAIN_label_18, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_18.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_18.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_22
PAGE_MAIN_label_22 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_22.set_text("V")
PAGE_MAIN_label_22.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_22.set_width(lv.pct(100))
PAGE_MAIN_label_22.set_pos(182, 7)
PAGE_MAIN_label_22.set_size(30, 32)
# Set style for PAGE_MAIN_label_22, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_22.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_22.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_23
PAGE_MAIN_label_23 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_23.set_text("A")
PAGE_MAIN_label_23.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_23.set_width(lv.pct(100))
PAGE_MAIN_label_23.set_pos(182, 52)
PAGE_MAIN_label_23.set_size(30, 32)
# Set style for PAGE_MAIN_label_23, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_23.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_23.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_24
PAGE_MAIN_label_24 = lv.label(PAGE_MAIN_cont_2)
PAGE_MAIN_label_24.set_text("W")
PAGE_MAIN_label_24.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_24.set_width(lv.pct(100))
PAGE_MAIN_label_24.set_pos(182, 97)
PAGE_MAIN_label_24.set_size(30, 32)
# Set style for PAGE_MAIN_label_24, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_24.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_24.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_13
PAGE_MAIN_label_13 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_13.set_text("00.00")
PAGE_MAIN_label_13.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_13.set_width(lv.pct(100))
PAGE_MAIN_label_13.set_pos(70, 10)
PAGE_MAIN_label_13.set_size(93, 32)
# Set style for PAGE_MAIN_label_13, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_13.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_13.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_14
PAGE_MAIN_label_14 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_14.set_text("00.00")
PAGE_MAIN_label_14.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_14.set_width(lv.pct(100))
PAGE_MAIN_label_14.set_pos(70, 52)
PAGE_MAIN_label_14.set_size(93, 32)
# Set style for PAGE_MAIN_label_14, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_14.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_14.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_15
PAGE_MAIN_label_15 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_15.set_text("00.00")
PAGE_MAIN_label_15.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_15.set_width(lv.pct(100))
PAGE_MAIN_label_15.set_pos(70, 94)
PAGE_MAIN_label_15.set_size(93, 32)
# Set style for PAGE_MAIN_label_15, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_15.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_15.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_19
PAGE_MAIN_label_19 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_19.set_text("V")
PAGE_MAIN_label_19.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_19.set_width(lv.pct(100))
PAGE_MAIN_label_19.set_pos(171, 9)
PAGE_MAIN_label_19.set_size(30, 32)
# Set style for PAGE_MAIN_label_19, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_19.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_19.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_20
PAGE_MAIN_label_20 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_20.set_text("A")
PAGE_MAIN_label_20.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_20.set_width(lv.pct(100))
PAGE_MAIN_label_20.set_pos(171, 52)
PAGE_MAIN_label_20.set_size(30, 32)
# Set style for PAGE_MAIN_label_20, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_20.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_20.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_21
PAGE_MAIN_label_21 = lv.label(PAGE_MAIN_cont_1)
PAGE_MAIN_label_21.set_text("W")
PAGE_MAIN_label_21.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_21.set_width(lv.pct(100))
PAGE_MAIN_label_21.set_pos(171, 95)
PAGE_MAIN_label_21.set_size(30, 32)
# Set style for PAGE_MAIN_label_21, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_21.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_21.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_cont_3
PAGE_MAIN_cont_3 = lv.obj(PAGE_MAIN)
PAGE_MAIN_cont_3.set_pos(16, 15)
PAGE_MAIN_cont_3.set_size(450, 32)
PAGE_MAIN_cont_3.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN_cont_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_cont_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_radius(6, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create PAGE_MAIN_label_25
PAGE_MAIN_label_25 = lv.label(PAGE_MAIN_cont_3)
PAGE_MAIN_label_25.set_text("UF4DigitalPower")
PAGE_MAIN_label_25.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_25.set_width(lv.pct(100))
PAGE_MAIN_label_25.set_pos(4, 3)
PAGE_MAIN_label_25.set_size(180, 24)
# Set style for PAGE_MAIN_label_25, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_25.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_text_font(test_font("blender", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_25.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_26
PAGE_MAIN_label_26 = lv.label(PAGE_MAIN_cont_3)
PAGE_MAIN_label_26.set_text("V1.0.0.112")
PAGE_MAIN_label_26.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_26.set_width(lv.pct(100))
PAGE_MAIN_label_26.set_pos(231, 3)
PAGE_MAIN_label_26.set_size(180, 24)
# Set style for PAGE_MAIN_label_26, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_26.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_text_font(test_font("blender", 24), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_26.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_chart_1
PAGE_MAIN_chart_1 = lv.chart(PAGE_MAIN)
PAGE_MAIN_chart_1.set_type(lv.chart.TYPE.LINE)
PAGE_MAIN_chart_1.set_div_line_count(3, 5)
PAGE_MAIN_chart_1.set_point_count(5)
PAGE_MAIN_chart_1.set_range(lv.chart.AXIS.PRIMARY_Y, 0, 100)
PAGE_MAIN_chart_1.set_range(lv.chart.AXIS.SECONDARY_Y, 0, 100)
PAGE_MAIN_chart_1.set_zoom_x(256)
PAGE_MAIN_chart_1.set_zoom_y(256)
PAGE_MAIN_chart_1_series_0 = PAGE_MAIN_chart_1.add_series(lv.color_hex(0x000000), lv.chart.AXIS.PRIMARY_Y)
PAGE_MAIN_chart_1.set_next_value(PAGE_MAIN_chart_1_series_0, 1)
PAGE_MAIN_chart_1.set_next_value(PAGE_MAIN_chart_1_series_0, 20)
PAGE_MAIN_chart_1.set_next_value(PAGE_MAIN_chart_1_series_0, 30)
PAGE_MAIN_chart_1.set_next_value(PAGE_MAIN_chart_1_series_0, 40)
PAGE_MAIN_chart_1.set_next_value(PAGE_MAIN_chart_1_series_0, 5)
PAGE_MAIN_chart_1.set_pos(16, 206)
PAGE_MAIN_chart_1.set_size(449, 168)
PAGE_MAIN_chart_1.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN_chart_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_chart_1.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_radius(6, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_line_width(2, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_line_color(lv.color_hex(0xe8e8e8), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_line_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for PAGE_MAIN_chart_1, Part: lv.PART.TICKS, State: lv.STATE.DEFAULT.
PAGE_MAIN_chart_1.set_style_text_color(lv.color_hex(0x151212), lv.PART.TICKS|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_text_font(test_font("montserratMedium", 12), lv.PART.TICKS|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_text_opa(255, lv.PART.TICKS|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_line_width(2, lv.PART.TICKS|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_line_color(lv.color_hex(0xe8e8e8), lv.PART.TICKS|lv.STATE.DEFAULT)
PAGE_MAIN_chart_1.set_style_line_opa(255, lv.PART.TICKS|lv.STATE.DEFAULT)

# Create PAGE_MAIN_cont_4
PAGE_MAIN_cont_4 = lv.obj(PAGE_MAIN)
PAGE_MAIN_cont_4.set_pos(16, 385)
PAGE_MAIN_cont_4.set_size(234, 90)
PAGE_MAIN_cont_4.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN_cont_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_cont_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_radius(6, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create PAGE_MAIN_label_27
PAGE_MAIN_label_27 = lv.label(PAGE_MAIN_cont_4)
PAGE_MAIN_label_27.set_text("ISET:")
PAGE_MAIN_label_27.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_27.set_width(lv.pct(100))
PAGE_MAIN_label_27.set_pos(3, 47)
PAGE_MAIN_label_27.set_size(93, 32)
# Set style for PAGE_MAIN_label_27, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_27.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_27.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_28
PAGE_MAIN_label_28 = lv.label(PAGE_MAIN_cont_4)
PAGE_MAIN_label_28.set_text("VSET:")
PAGE_MAIN_label_28.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_28.set_width(lv.pct(100))
PAGE_MAIN_label_28.set_pos(4, 10)
PAGE_MAIN_label_28.set_size(93, 32)
# Set style for PAGE_MAIN_label_28, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_28.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_28.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_cont_5
PAGE_MAIN_cont_5 = lv.obj(PAGE_MAIN_cont_4)
PAGE_MAIN_cont_5.set_pos(2, 99)
PAGE_MAIN_cont_5.set_size(110, 100)
PAGE_MAIN_cont_5.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN_cont_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_cont_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_radius(6, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create PAGE_MAIN_label_29
PAGE_MAIN_label_29 = lv.label(PAGE_MAIN_cont_5)
PAGE_MAIN_label_29.set_text("CC/CV")
PAGE_MAIN_label_29.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_29.set_width(lv.pct(100))
PAGE_MAIN_label_29.set_pos(53, 8)
PAGE_MAIN_label_29.set_size(43, 17)
# Set style for PAGE_MAIN_label_29, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_29.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_29.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_30
PAGE_MAIN_label_30 = lv.label(PAGE_MAIN_cont_5)
PAGE_MAIN_label_30.set_text("MODE:")
PAGE_MAIN_label_30.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_30.set_width(lv.pct(100))
PAGE_MAIN_label_30.set_pos(8, 8)
PAGE_MAIN_label_30.set_size(44, 17)
# Set style for PAGE_MAIN_label_30, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_30.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_30.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_31
PAGE_MAIN_label_31 = lv.label(PAGE_MAIN_cont_5)
PAGE_MAIN_label_31.set_text("TOPO :")
PAGE_MAIN_label_31.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_31.set_width(lv.pct(100))
PAGE_MAIN_label_31.set_pos(8, 30)
PAGE_MAIN_label_31.set_size(44, 17)
# Set style for PAGE_MAIN_label_31, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_31.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_31.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_32
PAGE_MAIN_label_32 = lv.label(PAGE_MAIN_cont_5)
PAGE_MAIN_label_32.set_text("FAULT:")
PAGE_MAIN_label_32.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_32.set_width(lv.pct(100))
PAGE_MAIN_label_32.set_pos(8, 52)
PAGE_MAIN_label_32.set_size(44, 17)
# Set style for PAGE_MAIN_label_32, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_32.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_32.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_33
PAGE_MAIN_label_33 = lv.label(PAGE_MAIN_cont_5)
PAGE_MAIN_label_33.set_text("FSM  :")
PAGE_MAIN_label_33.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_33.set_width(lv.pct(100))
PAGE_MAIN_label_33.set_pos(8, 74)
PAGE_MAIN_label_33.set_size(44, 17)
# Set style for PAGE_MAIN_label_33, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_33.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_33.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_34
PAGE_MAIN_label_34 = lv.label(PAGE_MAIN_cont_5)
PAGE_MAIN_label_34.set_text("BUCK\n")
PAGE_MAIN_label_34.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_34.set_width(lv.pct(100))
PAGE_MAIN_label_34.set_pos(53, 30)
PAGE_MAIN_label_34.set_size(43, 17)
# Set style for PAGE_MAIN_label_34, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_34.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_34.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_35
PAGE_MAIN_label_35 = lv.label(PAGE_MAIN_cont_5)
PAGE_MAIN_label_35.set_text("OTP\n")
PAGE_MAIN_label_35.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_35.set_width(lv.pct(100))
PAGE_MAIN_label_35.set_pos(53, 52)
PAGE_MAIN_label_35.set_size(43, 17)
# Set style for PAGE_MAIN_label_35, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_35.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_35.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_36
PAGE_MAIN_label_36 = lv.label(PAGE_MAIN_cont_5)
PAGE_MAIN_label_36.set_text("RUN\n")
PAGE_MAIN_label_36.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_36.set_width(lv.pct(100))
PAGE_MAIN_label_36.set_pos(53, 73)
PAGE_MAIN_label_36.set_size(43, 17)
# Set style for PAGE_MAIN_label_36, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_36.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_36.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_cont_6
PAGE_MAIN_cont_6 = lv.obj(PAGE_MAIN_cont_5)
PAGE_MAIN_cont_6.set_pos(119, 0)
PAGE_MAIN_cont_6.set_size(113, 100)
PAGE_MAIN_cont_6.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN_cont_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_cont_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_radius(6, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create PAGE_MAIN_label_42
PAGE_MAIN_label_42 = lv.label(PAGE_MAIN_cont_6)
PAGE_MAIN_label_42.set_text("CORE  :")
PAGE_MAIN_label_42.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_42.set_width(lv.pct(100))
PAGE_MAIN_label_42.set_pos(4, 31)
PAGE_MAIN_label_42.set_size(48, 17)
# Set style for PAGE_MAIN_label_42, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_42.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_42.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_41
PAGE_MAIN_label_41 = lv.label(PAGE_MAIN_cont_6)
PAGE_MAIN_label_41.set_text("TEMP1:")
PAGE_MAIN_label_41.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_41.set_width(lv.pct(100))
PAGE_MAIN_label_41.set_pos(3, 52)
PAGE_MAIN_label_41.set_size(49, 17)
# Set style for PAGE_MAIN_label_41, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_41.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_41.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_40
PAGE_MAIN_label_40 = lv.label(PAGE_MAIN_cont_6)
PAGE_MAIN_label_40.set_text("TEMP2:")
PAGE_MAIN_label_40.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_40.set_width(lv.pct(100))
PAGE_MAIN_label_40.set_pos(3, 73)
PAGE_MAIN_label_40.set_size(49, 17)
# Set style for PAGE_MAIN_label_40, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_40.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_40.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_39
PAGE_MAIN_label_39 = lv.label(PAGE_MAIN_cont_6)
PAGE_MAIN_label_39.set_text("25.00")
PAGE_MAIN_label_39.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_39.set_width(lv.pct(100))
PAGE_MAIN_label_39.set_pos(58, 31)
PAGE_MAIN_label_39.set_size(43, 17)
# Set style for PAGE_MAIN_label_39, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_39.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_39.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_43
PAGE_MAIN_label_43 = lv.label(PAGE_MAIN_cont_6)
PAGE_MAIN_label_43.set_text("25.00\n")
PAGE_MAIN_label_43.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_43.set_width(lv.pct(100))
PAGE_MAIN_label_43.set_pos(58, 73)
PAGE_MAIN_label_43.set_size(43, 17)
# Set style for PAGE_MAIN_label_43, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_43.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_43.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_44
PAGE_MAIN_label_44 = lv.label(PAGE_MAIN_cont_6)
PAGE_MAIN_label_44.set_text("25.00\n")
PAGE_MAIN_label_44.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_44.set_width(lv.pct(100))
PAGE_MAIN_label_44.set_pos(58, 52)
PAGE_MAIN_label_44.set_size(43, 17)
# Set style for PAGE_MAIN_label_44, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_44.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_44.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_45
PAGE_MAIN_label_45 = lv.label(PAGE_MAIN_cont_6)
PAGE_MAIN_label_45.set_text("Temperature")
PAGE_MAIN_label_45.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_45.set_width(lv.pct(100))
PAGE_MAIN_label_45.set_pos(23, 10)
PAGE_MAIN_label_45.set_size(83, 17)
# Set style for PAGE_MAIN_label_45, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_45.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_text_font(test_font("blender", 14), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_45.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_img_1
PAGE_MAIN_img_1 = lv.img(PAGE_MAIN_cont_6)
PAGE_MAIN_img_1.set_src("B:MicroPython/_ic_fluent_temperature_48_regular_alpha_24x24.bin")
PAGE_MAIN_img_1.add_flag(lv.obj.FLAG.CLICKABLE)
PAGE_MAIN_img_1.set_pivot(24,24)
PAGE_MAIN_img_1.set_angle(0)
PAGE_MAIN_img_1.set_pos(3, 6)
PAGE_MAIN_img_1.set_size(24, 24)
# Set style for PAGE_MAIN_img_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_img_1.set_style_img_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_img_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_img_1.set_style_clip_corner(True, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_61
PAGE_MAIN_label_61 = lv.label(PAGE_MAIN_cont_4)
PAGE_MAIN_label_61.set_text("V")
PAGE_MAIN_label_61.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_61.set_width(lv.pct(100))
PAGE_MAIN_label_61.set_pos(203, 10)
PAGE_MAIN_label_61.set_size(24, 32)
# Set style for PAGE_MAIN_label_61, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_61.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_61.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_62
PAGE_MAIN_label_62 = lv.label(PAGE_MAIN_cont_4)
PAGE_MAIN_label_62.set_text("A")
PAGE_MAIN_label_62.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_62.set_width(lv.pct(100))
PAGE_MAIN_label_62.set_pos(203, 47)
PAGE_MAIN_label_62.set_size(24, 31)
# Set style for PAGE_MAIN_label_62, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_62.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_62.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_ta_1
PAGE_MAIN_ta_1 = lv.textarea(PAGE_MAIN_cont_4)
PAGE_MAIN_ta_1.set_text("45.00")
PAGE_MAIN_ta_1.set_placeholder_text("")
PAGE_MAIN_ta_1.set_password_bullet("*")
PAGE_MAIN_ta_1.set_password_mode(False)
PAGE_MAIN_ta_1.set_one_line(True)
PAGE_MAIN_ta_1.set_accepted_chars("1234567890.")
PAGE_MAIN_ta_1.set_max_length(32)
PAGE_MAIN_ta_1.set_pos(97, 10)
PAGE_MAIN_ta_1.set_size(107, 35)
# Set style for PAGE_MAIN_ta_1, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_1.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_text_letter_space(3, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for PAGE_MAIN_ta_1, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_1.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_1.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create PAGE_MAIN_ta_2
PAGE_MAIN_ta_2 = lv.textarea(PAGE_MAIN_cont_4)
PAGE_MAIN_ta_2.set_text("45.00")
PAGE_MAIN_ta_2.set_placeholder_text("")
PAGE_MAIN_ta_2.set_password_bullet("*")
PAGE_MAIN_ta_2.set_password_mode(False)
PAGE_MAIN_ta_2.set_one_line(True)
PAGE_MAIN_ta_2.set_accepted_chars("")
PAGE_MAIN_ta_2.set_max_length(32)
PAGE_MAIN_ta_2.set_pos(97, 47)
PAGE_MAIN_ta_2.set_size(107, 31)
# Set style for PAGE_MAIN_ta_2, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_2.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_text_font(test_font("blender", 32), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_text_letter_space(3, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for PAGE_MAIN_ta_2, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_2.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_2.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create PAGE_MAIN_cont_7
PAGE_MAIN_cont_7 = lv.obj(PAGE_MAIN)
PAGE_MAIN_cont_7.set_pos(260, 385)
PAGE_MAIN_cont_7.set_size(205, 90)
PAGE_MAIN_cont_7.set_scrollbar_mode(lv.SCROLLBAR_MODE.OFF)
# Set style for PAGE_MAIN_cont_7, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_cont_7.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_radius(6, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_bg_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_bg_color(lv.color_hex(0xffffff), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_cont_7.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
# Create PAGE_MAIN_label_63
PAGE_MAIN_label_63 = lv.label(PAGE_MAIN_cont_7)
PAGE_MAIN_label_63.set_text("OTP:")
PAGE_MAIN_label_63.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_63.set_width(lv.pct(100))
PAGE_MAIN_label_63.set_pos(5, 7)
PAGE_MAIN_label_63.set_size(36, 17)
# Set style for PAGE_MAIN_label_63, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_63.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_63.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_64
PAGE_MAIN_label_64 = lv.label(PAGE_MAIN_cont_7)
PAGE_MAIN_label_64.set_text("OVP:")
PAGE_MAIN_label_64.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_64.set_width(lv.pct(100))
PAGE_MAIN_label_64.set_pos(5, 28)
PAGE_MAIN_label_64.set_size(36, 17)
# Set style for PAGE_MAIN_label_64, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_64.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_64.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_65
PAGE_MAIN_label_65 = lv.label(PAGE_MAIN_cont_7)
PAGE_MAIN_label_65.set_text("OCP:")
PAGE_MAIN_label_65.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_65.set_width(lv.pct(100))
PAGE_MAIN_label_65.set_pos(5, 49)
PAGE_MAIN_label_65.set_size(36, 17)
# Set style for PAGE_MAIN_label_65, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_65.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_65.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_label_66
PAGE_MAIN_label_66 = lv.label(PAGE_MAIN_cont_7)
PAGE_MAIN_label_66.set_text("UVP:")
PAGE_MAIN_label_66.set_long_mode(lv.label.LONG.WRAP)
PAGE_MAIN_label_66.set_width(lv.pct(100))
PAGE_MAIN_label_66.set_pos(4, 69)
PAGE_MAIN_label_66.set_size(36, 17)
# Set style for PAGE_MAIN_label_66, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_label_66.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_text_line_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_pad_bottom(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_label_66.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Create PAGE_MAIN_ta_3
PAGE_MAIN_ta_3 = lv.textarea(PAGE_MAIN_cont_7)
PAGE_MAIN_ta_3.set_text("80.00")
PAGE_MAIN_ta_3.set_placeholder_text("")
PAGE_MAIN_ta_3.set_password_bullet("*")
PAGE_MAIN_ta_3.set_password_mode(False)
PAGE_MAIN_ta_3.set_one_line(True)
PAGE_MAIN_ta_3.set_accepted_chars("1234567890.")
PAGE_MAIN_ta_3.set_max_length(32)
PAGE_MAIN_ta_3.set_pos(43, 7)
PAGE_MAIN_ta_3.set_size(41, 15)
# Set style for PAGE_MAIN_ta_3, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_3.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for PAGE_MAIN_ta_3, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_3.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_3.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create PAGE_MAIN_ta_4
PAGE_MAIN_ta_4 = lv.textarea(PAGE_MAIN_cont_7)
PAGE_MAIN_ta_4.set_text("45.00")
PAGE_MAIN_ta_4.set_placeholder_text("")
PAGE_MAIN_ta_4.set_password_bullet("*")
PAGE_MAIN_ta_4.set_password_mode(False)
PAGE_MAIN_ta_4.set_one_line(True)
PAGE_MAIN_ta_4.set_accepted_chars("1234567890.")
PAGE_MAIN_ta_4.set_max_length(32)
PAGE_MAIN_ta_4.set_pos(43, 27)
PAGE_MAIN_ta_4.set_size(41, 15)
# Set style for PAGE_MAIN_ta_4, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_4.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for PAGE_MAIN_ta_4, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_4.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_4.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create PAGE_MAIN_ta_5
PAGE_MAIN_ta_5 = lv.textarea(PAGE_MAIN_cont_7)
PAGE_MAIN_ta_5.set_text("10.00")
PAGE_MAIN_ta_5.set_placeholder_text("")
PAGE_MAIN_ta_5.set_password_bullet("*")
PAGE_MAIN_ta_5.set_password_mode(False)
PAGE_MAIN_ta_5.set_one_line(True)
PAGE_MAIN_ta_5.set_accepted_chars("1234567890.")
PAGE_MAIN_ta_5.set_max_length(32)
PAGE_MAIN_ta_5.set_pos(42, 47)
PAGE_MAIN_ta_5.set_size(41, 15)
# Set style for PAGE_MAIN_ta_5, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_5.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for PAGE_MAIN_ta_5, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_5.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_5.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

# Create PAGE_MAIN_ta_6
PAGE_MAIN_ta_6 = lv.textarea(PAGE_MAIN_cont_7)
PAGE_MAIN_ta_6.set_text("05.00")
PAGE_MAIN_ta_6.set_placeholder_text("")
PAGE_MAIN_ta_6.set_password_bullet("*")
PAGE_MAIN_ta_6.set_password_mode(False)
PAGE_MAIN_ta_6.set_one_line(True)
PAGE_MAIN_ta_6.set_accepted_chars("1234567890.")
PAGE_MAIN_ta_6.set_max_length(32)
PAGE_MAIN_ta_6.set_pos(42, 67)
PAGE_MAIN_ta_6.set_size(41, 15)
# Set style for PAGE_MAIN_ta_6, Part: lv.PART.MAIN, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_6.set_style_text_color(lv.color_hex(0x000000), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_text_font(test_font("blender", 16), lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_text_opa(255, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_text_letter_space(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_text_align(lv.TEXT_ALIGN.CENTER, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_bg_opa(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_border_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_shadow_width(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_pad_top(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_pad_right(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_pad_left(0, lv.PART.MAIN|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_radius(0, lv.PART.MAIN|lv.STATE.DEFAULT)

# Set style for PAGE_MAIN_ta_6, Part: lv.PART.SCROLLBAR, State: lv.STATE.DEFAULT.
PAGE_MAIN_ta_6.set_style_bg_opa(255, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_bg_color(lv.color_hex(0x2195f6), lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_bg_grad_dir(lv.GRAD_DIR.NONE, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)
PAGE_MAIN_ta_6.set_style_radius(0, lv.PART.SCROLLBAR|lv.STATE.DEFAULT)

PAGE_MAIN.update_layout()

# content from custom.py

# Load the default screen
lv.scr_load(PAGE_MAIN)

while SDL.check():
    time.sleep_ms(5)

