/*******************************************************************************
 * Size: 15 px
 * Bpp: 1
 * Opts: --bpp 1 --size 15 --font E:/Smart_Home/ESP32/smartHome_esp32/guiproject/assets/SourceHanSansCN-Normal-2.otf -o E:/Smart_Home/ESP32/smartHome_esp32/guiproject/assets\ui_font_misc.c --format lvgl -r 0x20-0x7f --symbols 音量 --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_MISC
#define UI_FONT_MISC 1
#endif

#if UI_FONT_MISC

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xaa, 0xa8, 0x3c,

    /* U+0022 "\"" */
    0x99, 0x99,

    /* U+0023 "#" */
    0x24, 0x89, 0x27, 0xf4, 0x89, 0x12, 0x7e, 0x48,
    0x92, 0x20,

    /* U+0024 "$" */
    0x21, 0xcc, 0xe0, 0x83, 0x7, 0x6, 0xc, 0x10,
    0x62, 0x70, 0x80,

    /* U+0025 "%" */
    0x70, 0x46, 0xc4, 0x22, 0x21, 0x12, 0x8, 0xa7,
    0x6d, 0x6d, 0xd2, 0x20, 0x91, 0x8, 0x88, 0x46,
    0xc4, 0x1c,

    /* U+0026 "&" */
    0x38, 0x22, 0x11, 0x9, 0x7, 0x3, 0x7, 0x47,
    0x12, 0x86, 0x63, 0x9e, 0x20,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x29, 0x29, 0x24, 0x92, 0x24, 0x88,

    /* U+0029 ")" */
    0x89, 0x22, 0x49, 0x24, 0xa4, 0xa0,

    /* U+002A "*" */
    0x25, 0x5c, 0xa0, 0x0,

    /* U+002B "+" */
    0x10, 0x20, 0x47, 0xf1, 0x2, 0x4, 0x0,

    /* U+002C "," */
    0xd8,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x4, 0x20, 0x82, 0x10, 0x41, 0x8, 0x20, 0x84,
    0x10, 0x42, 0x0,

    /* U+0030 "0" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x86,
    0x88, 0xe0,

    /* U+0031 "1" */
    0x67, 0x8, 0x42, 0x10, 0x84, 0x21, 0x3e,

    /* U+0032 "2" */
    0x79, 0x98, 0x10, 0x20, 0x41, 0x2, 0x8, 0x20,
    0x83, 0xf8,

    /* U+0033 "3" */
    0x79, 0x98, 0x10, 0x20, 0x86, 0x3, 0x1, 0x3,
    0xd, 0xf0,

    /* U+0034 "4" */
    0xc, 0xc, 0x14, 0x34, 0x24, 0x44, 0xc4, 0xff,
    0x4, 0x4, 0x4,

    /* U+0035 "5" */
    0x7c, 0x81, 0x2, 0x7, 0xc8, 0xc0, 0x81, 0x3,
    0x9, 0xe0,

    /* U+0036 "6" */
    0x3c, 0xc1, 0x4, 0xb, 0xd8, 0xe0, 0xc1, 0x82,
    0x88, 0xe0,

    /* U+0037 "7" */
    0xfe, 0x8, 0x20, 0x41, 0x2, 0x8, 0x10, 0x20,
    0x40, 0x80,

    /* U+0038 "8" */
    0x38, 0x89, 0x12, 0x22, 0x87, 0x11, 0x41, 0x83,
    0x8d, 0xf0,

    /* U+0039 "9" */
    0x78, 0x8a, 0xc, 0x18, 0x38, 0xde, 0x81, 0x4,
    0x19, 0xe0,

    /* U+003A ":" */
    0xf0, 0xf,

    /* U+003B ";" */
    0xf0, 0x3, 0x70,

    /* U+003C "<" */
    0x0, 0x1c, 0xc6, 0x6, 0x3, 0x80, 0x80,

    /* U+003D "=" */
    0xfe, 0x0, 0x7, 0xf0,

    /* U+003E ">" */
    0x1, 0xc0, 0x60, 0x30, 0xce, 0x20, 0x0,

    /* U+003F "?" */
    0x7b, 0x10, 0x41, 0x8, 0x43, 0xc, 0x0, 0xc3,
    0x0,

    /* U+0040 "@" */
    0xf, 0x83, 0x6, 0x20, 0x24, 0x1, 0xc7, 0x18,
    0x91, 0x91, 0x19, 0x11, 0x93, 0x28, 0xdc, 0x40,
    0x2, 0x0, 0x1f, 0x0,

    /* U+0041 "A" */
    0x8, 0xa, 0x5, 0x2, 0x83, 0x61, 0x10, 0x88,
    0xfe, 0x41, 0x20, 0xb0, 0x60,

    /* U+0042 "B" */
    0xfd, 0xe, 0xc, 0x18, 0x7f, 0xa1, 0xc1, 0x83,
    0xf, 0xe0,

    /* U+0043 "C" */
    0x1e, 0x62, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x40, 0x63, 0x1e,

    /* U+0044 "D" */
    0xf8, 0x86, 0x82, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x82, 0x86, 0xf8,

    /* U+0045 "E" */
    0xfe, 0x8, 0x20, 0x83, 0xe8, 0x20, 0x82, 0xf,
    0xc0,

    /* U+0046 "F" */
    0xfe, 0x8, 0x20, 0x83, 0xe8, 0x20, 0x82, 0x8,
    0x0,

    /* U+0047 "G" */
    0x1e, 0x61, 0x40, 0x80, 0x80, 0x87, 0x81, 0x81,
    0x41, 0x61, 0x1e,

    /* U+0048 "H" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81,
    0x81, 0x81, 0x81,

    /* U+0049 "I" */
    0xff, 0xe0,

    /* U+004A "J" */
    0x4, 0x10, 0x41, 0x4, 0x10, 0x41, 0x7, 0x37,
    0x80,

    /* U+004B "K" */
    0x86, 0x8c, 0x88, 0x90, 0xb0, 0xf8, 0xc8, 0x8c,
    0x84, 0x86, 0x82,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0xf,
    0xc0,

    /* U+004D "M" */
    0xc1, 0xe0, 0xf0, 0x74, 0x5a, 0x2d, 0x96, 0x53,
    0x29, 0x88, 0xc4, 0x60, 0x20,

    /* U+004E "N" */
    0x81, 0xc1, 0xa1, 0xa1, 0x91, 0x99, 0x89, 0x85,
    0x85, 0x83, 0x81,

    /* U+004F "O" */
    0x3c, 0x31, 0x90, 0x50, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x41, 0x31, 0x87, 0x80,

    /* U+0050 "P" */
    0xf9, 0xe, 0xc, 0x18, 0x30, 0xbe, 0x40, 0x81,
    0x2, 0x0,

    /* U+0051 "Q" */
    0x3e, 0x31, 0x90, 0x50, 0x18, 0xc, 0x6, 0x3,
    0x1, 0x41, 0x1b, 0x6, 0x1, 0x80, 0x38,

    /* U+0052 "R" */
    0xfd, 0xe, 0xc, 0x18, 0x7f, 0xa6, 0x44, 0x85,
    0xe, 0x8,

    /* U+0053 "S" */
    0x3e, 0x63, 0x40, 0x40, 0x30, 0x1c, 0x3, 0x1,
    0x1, 0xc3, 0x3c,

    /* U+0054 "T" */
    0xff, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
    0x10, 0x10, 0x10,

    /* U+0055 "U" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x42, 0x3c,

    /* U+0056 "V" */
    0x81, 0xc1, 0x43, 0x42, 0x62, 0x22, 0x24, 0x34,
    0x14, 0x18, 0x18,

    /* U+0057 "W" */
    0x42, 0x1a, 0x18, 0x91, 0x44, 0x8a, 0x26, 0x51,
    0x12, 0xd0, 0xa2, 0x85, 0x14, 0x28, 0xa1, 0xc7,
    0x4, 0x10,

    /* U+0058 "X" */
    0x43, 0x62, 0x26, 0x34, 0x18, 0x18, 0x1c, 0x24,
    0x26, 0x42, 0x41,

    /* U+0059 "Y" */
    0x83, 0x8d, 0x13, 0x62, 0x85, 0x4, 0x8, 0x10,
    0x20, 0x40,

    /* U+005A "Z" */
    0x7f, 0x2, 0x6, 0x4, 0xc, 0x18, 0x10, 0x30,
    0x60, 0x40, 0xff,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x49, 0xc0,

    /* U+005C "\\" */
    0x81, 0x4, 0x10, 0x20, 0x82, 0x4, 0x10, 0x40,
    0x82, 0x8, 0x10,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x93, 0xc0,

    /* U+005E "^" */
    0x30, 0xc6, 0x92, 0x46, 0x10,

    /* U+005F "_" */
    0xff,

    /* U+0060 "`" */
    0xcc, 0x0,

    /* U+0061 "a" */
    0x78, 0x30, 0x4f, 0xc6, 0x18, 0xdd,

    /* U+0062 "b" */
    0x81, 0x2, 0x5, 0xcc, 0x50, 0x60, 0xc1, 0x83,
    0xb, 0xe0,

    /* U+0063 "c" */
    0x39, 0x8, 0x20, 0x82, 0x4, 0x4e,

    /* U+0064 "d" */
    0x2, 0x4, 0x9, 0xd4, 0x70, 0x60, 0xc1, 0x82,
    0x8c, 0xe8,

    /* U+0065 "e" */
    0x3c, 0x8e, 0xf, 0xf8, 0x10, 0x10, 0x1e,

    /* U+0066 "f" */
    0x34, 0x44, 0xf4, 0x44, 0x44, 0x44,

    /* U+0067 "g" */
    0x7f, 0x1a, 0x16, 0x67, 0x90, 0x20, 0x3e, 0x83,
    0xd, 0xf0,

    /* U+0068 "h" */
    0x82, 0x8, 0x2e, 0xc6, 0x18, 0x61, 0x86, 0x18,
    0x40,

    /* U+0069 "i" */
    0xc2, 0xaa, 0xa8,

    /* U+006A "j" */
    0x30, 0x2, 0x22, 0x22, 0x22, 0x22, 0x2c,

    /* U+006B "k" */
    0x82, 0x8, 0x23, 0x9a, 0x4a, 0x34, 0x9a, 0x28,
    0x40,

    /* U+006C "l" */
    0xaa, 0xaa, 0xac,

    /* U+006D "m" */
    0xb9, 0xd8, 0xc6, 0x10, 0xc2, 0x18, 0x43, 0x8,
    0x61, 0xc, 0x21,

    /* U+006E "n" */
    0xbb, 0x18, 0x61, 0x86, 0x18, 0x61,

    /* U+006F "o" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x51, 0x1c,

    /* U+0070 "p" */
    0xb9, 0x8a, 0xc, 0x18, 0x30, 0x71, 0x5c, 0x81,
    0x2, 0x0,

    /* U+0071 "q" */
    0x3a, 0x8e, 0xc, 0x18, 0x30, 0x51, 0x9d, 0x2,
    0x4, 0x8,

    /* U+0072 "r" */
    0xbc, 0x88, 0x88, 0x88,

    /* U+0073 "s" */
    0x74, 0x20, 0xc1, 0x6, 0x2e,

    /* U+0074 "t" */
    0x44, 0xf4, 0x44, 0x44, 0x43,

    /* U+0075 "u" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0xdd,

    /* U+0076 "v" */
    0xc2, 0x85, 0x1b, 0x22, 0x45, 0x6, 0xc,

    /* U+0077 "w" */
    0x46, 0x28, 0xc5, 0x18, 0xb4, 0xb2, 0x94, 0x52,
    0x8a, 0x50, 0x84,

    /* U+0078 "x" */
    0x46, 0xc8, 0xa0, 0xc3, 0x85, 0x11, 0x23,

    /* U+0079 "y" */
    0xc2, 0x85, 0x19, 0x22, 0x43, 0x6, 0xc, 0x10,
    0x21, 0x80,

    /* U+007A "z" */
    0xfc, 0x21, 0x84, 0x21, 0x84, 0x3f,

    /* U+007B "{" */
    0x74, 0x44, 0x44, 0x48, 0x44, 0x44, 0x47,

    /* U+007C "|" */
    0xff, 0xff,

    /* U+007D "}" */
    0xe2, 0x22, 0x22, 0x21, 0x22, 0x22, 0x2e,

    /* U+007E "~" */
    0xe0, 0x38,

    /* U+91CF "量" */
    0x3f, 0xe1, 0xff, 0x8, 0x8, 0x3f, 0x8f, 0xff,
    0x80, 0x1, 0xff, 0xc7, 0xfe, 0x42, 0x11, 0xff,
    0x1f, 0xfc, 0x4, 0xf, 0xff, 0x80,

    /* U+97F3 "音" */
    0x0, 0x0, 0x10, 0x1f, 0xfc, 0x20, 0x80, 0x8c,
    0x7f, 0xfc, 0x0, 0x0, 0x0, 0x3f, 0xe1, 0x1,
    0xf, 0xf8, 0x40, 0x42, 0x2, 0x1f, 0xf0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 54, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 75, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 110, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 6, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 16, .adv_w = 132, .box_w = 6, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 27, .adv_w = 219, .box_w = 13, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 45, .adv_w = 161, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 65, .box_w = 1, .box_h = 4, .ofs_x = 1, .ofs_y = 7},
    {.bitmap_index = 59, .adv_w = 79, .box_w = 3, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 65, .adv_w = 79, .box_w = 3, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 71, .adv_w = 110, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 75, .adv_w = 132, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 82, .adv_w = 65, .box_w = 2, .box_h = 4, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 83, .adv_w = 82, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 84, .adv_w = 65, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 85, .adv_w = 94, .box_w = 6, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 96, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 106, .adv_w = 132, .box_w = 5, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 113, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 123, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 133, .adv_w = 132, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 144, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 154, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 164, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 174, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 184, .adv_w = 132, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 194, .adv_w = 65, .box_w = 2, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 196, .adv_w = 65, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 199, .adv_w = 132, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 206, .adv_w = 132, .box_w = 7, .box_h = 4, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 210, .adv_w = 132, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 217, .adv_w = 112, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 226, .adv_w = 224, .box_w = 12, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 246, .adv_w = 144, .box_w = 9, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 259, .adv_w = 156, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 269, .adv_w = 152, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 280, .adv_w = 164, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 291, .adv_w = 140, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 300, .adv_w = 131, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 309, .adv_w = 164, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 320, .adv_w = 173, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 331, .adv_w = 69, .box_w = 1, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 333, .adv_w = 127, .box_w = 6, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 342, .adv_w = 153, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 353, .adv_w = 129, .box_w = 6, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 362, .adv_w = 193, .box_w = 9, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 375, .adv_w = 172, .box_w = 8, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 386, .adv_w = 177, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 399, .adv_w = 150, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 409, .adv_w = 177, .box_w = 9, .box_h = 13, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 424, .adv_w = 150, .box_w = 7, .box_h = 11, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 434, .adv_w = 142, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 445, .adv_w = 143, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 456, .adv_w = 172, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 467, .adv_w = 136, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 478, .adv_w = 209, .box_w = 13, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 496, .adv_w = 135, .box_w = 8, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 507, .adv_w = 126, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 517, .adv_w = 144, .box_w = 8, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 528, .adv_w = 79, .box_w = 3, .box_h = 14, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 534, .adv_w = 94, .box_w = 6, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 545, .adv_w = 79, .box_w = 3, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 551, .adv_w = 132, .box_w = 6, .box_h = 6, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 556, .adv_w = 134, .box_w = 8, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 557, .adv_w = 144, .box_w = 3, .box_h = 3, .ofs_x = 2, .ofs_y = 9},
    {.bitmap_index = 559, .adv_w = 134, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 565, .adv_w = 147, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 575, .adv_w = 121, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 581, .adv_w = 148, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 591, .adv_w = 132, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 598, .adv_w = 76, .box_w = 4, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 604, .adv_w = 134, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 614, .adv_w = 144, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 623, .adv_w = 65, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 626, .adv_w = 65, .box_w = 4, .box_h = 14, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 633, .adv_w = 130, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 642, .adv_w = 67, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 645, .adv_w = 221, .box_w = 11, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 656, .adv_w = 145, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 662, .adv_w = 144, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 669, .adv_w = 148, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 679, .adv_w = 148, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 689, .adv_w = 91, .box_w = 4, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 693, .adv_w = 111, .box_w = 5, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 698, .adv_w = 89, .box_w = 4, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 703, .adv_w = 144, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 709, .adv_w = 123, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 716, .adv_w = 190, .box_w = 11, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 727, .adv_w = 117, .box_w = 7, .box_h = 8, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 734, .adv_w = 123, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 744, .adv_w = 112, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 750, .adv_w = 79, .box_w = 4, .box_h = 14, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 757, .adv_w = 64, .box_w = 1, .box_h = 16, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 759, .adv_w = 79, .box_w = 4, .box_h = 14, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 766, .adv_w = 132, .box_w = 7, .box_h = 2, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 768, .adv_w = 240, .box_w = 13, .box_h = 13, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 790, .adv_w = 240, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x624
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 37327, .range_length = 1573, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 2, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    }
};

/*-----------------
 *    KERNING
 *----------------*/


/*Map glyph_ids to kern left classes*/
static const uint8_t kern_left_class_mapping[] =
{
    0, 0, 0, 1, 0, 0, 0, 0,
    1, 2, 0, 0, 0, 3, 4, 3,
    5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 6, 6, 0, 0, 0,
    0, 0, 7, 8, 9, 10, 11, 12,
    13, 0, 0, 14, 15, 16, 0, 0,
    10, 17, 10, 18, 19, 20, 21, 22,
    23, 24, 25, 26, 2, 27, 0, 0,
    0, 0, 28, 29, 30, 0, 31, 32,
    33, 34, 0, 0, 35, 36, 34, 34,
    29, 29, 37, 38, 39, 40, 37, 41,
    42, 43, 44, 45, 2, 0, 0, 0,
    0, 0
};

/*Map glyph_ids to kern right classes*/
static const uint8_t kern_right_class_mapping[] =
{
    0, 0, 1, 2, 0, 0, 0, 0,
    2, 0, 3, 4, 0, 5, 6, 7,
    8, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 9, 10, 0, 0, 0,
    11, 0, 12, 0, 13, 0, 0, 0,
    13, 0, 0, 14, 0, 0, 0, 0,
    13, 0, 13, 0, 15, 16, 17, 18,
    19, 20, 21, 22, 0, 23, 3, 0,
    0, 0, 24, 0, 25, 25, 25, 26,
    27, 0, 28, 29, 0, 0, 30, 30,
    25, 30, 25, 30, 31, 32, 33, 34,
    35, 36, 37, 38, 0, 0, 3, 0,
    0, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, -30, 0, -30, 0,
    0, 0, 0, -14, 0, -25, -3, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    -8, 0, 0, 0, 0, 0, -5, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 21, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -24, 0, -36,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -26, -5, -17, -9, 0,
    -24, 0, 0, 0, -3, 0, 0, 0,
    7, 0, 0, -12, 0, -8, -6, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    -4, -12, 0, -5, -3, -7, -17, -5,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -7, 0, -2, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -11, -3, -21, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    -9, 0, -3, 7, 7, 0, 0, 3,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, -14, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -14, 0, -25,
    0, 0, 0, 0, 0, 0, -7, -2,
    -3, 0, 0, -14, -4, -4, 0, 1,
    -4, -2, -11, 6, 0, -3, 0, 0,
    0, 0, 6, -4, -2, -2, -1, -1,
    -2, 0, 0, 0, 0, -8, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    -4, -6, 0, -1, -1, -1, -4, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -4, -3, -3, -4, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, 0, 0, 0, -7, -3, -6, -5,
    -4, -1, -1, -1, -2, -3, 0, 0,
    0, 0, -5, 0, 0, 0, 0, -7,
    -3, -4, -3, 0, -4, 0, 0, 0,
    0, -9, 0, 0, 0, -5, 0, 0,
    0, -3, 0, -10, 0, -6, 0, -3,
    -2, -4, -5, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, -6, 0, -3, 0, -8,
    -3, 0, 0, 0, 0, 0, -18, 0,
    -18, -19, 0, 0, 0, -10, -3, -37,
    -5, 0, 0, 1, 1, -6, 1, -8,
    0, -9, -4, 0, -6, 0, 0, -5,
    -5, -3, -4, -5, -4, -7, -4, -7,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, 0, -5,
    0, -4, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -6, 0, -6, 0, 0, 0,
    0, 0, 0, -11, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, -11,
    0, -7, 0, 0, 0, 0, -2, -3,
    -5, 0, -2, -4, -4, -3, -3, 0,
    -4, 0, 0, 0, -2, 0, 0, 0,
    -3, 0, 0, -8, -4, -5, -4, -4,
    -5, -4, 0, -22, 0, -40, 0, -15,
    0, 0, 0, 0, -8, 1, -7, 0,
    -6, -32, -7, -20, -15, 0, -20, 0,
    -21, 0, -3, -4, -1, 0, 0, 0,
    0, -5, -3, -9, -9, 0, -9, 0,
    0, 0, 0, 0, -29, -9, -29, -21,
    0, 0, 0, -13, 0, -39, -3, -7,
    0, 0, 0, -6, -3, -22, 0, -12,
    -6, 0, -9, 0, 0, 0, -3, 0,
    0, 0, 0, -4, 0, -5, 0, 0,
    0, -3, 0, -8, 0, 0, 0, 0,
    0, -1, 0, -5, -4, -4, 0, 2,
    2, -1, 0, -3, 0, -1, -3, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, -2, 0, 0, 0, -5,
    0, 4, 0, 0, 0, 0, 0, 0,
    0, -4, -4, -5, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -28, -20,
    -28, -25, -5, -5, 0, -11, -6, -34,
    -11, 0, 0, 0, 0, -5, -4, -15,
    0, -20, -17, -5, -20, 0, 0, -12,
    -16, -5, -12, -9, -9, -11, -9, -20,
    0, 0, 0, 0, -4, 0, -4, -9,
    0, 0, 0, -5, 0, -12, -3, 0,
    0, -1, 0, -3, -4, 0, 0, -1,
    0, 0, -3, 0, 0, 0, -1, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, -17, -5, -17, -13, 0, 0,
    0, -4, -3, -20, -3, 0, -3, 3,
    0, 0, 0, -5, 0, -6, -4, 0,
    -6, 0, 0, -5, -3, 0, -8, -2,
    -2, -4, -2, -7, 0, 0, 0, 0,
    -9, -3, -9, -9, 0, 0, 0, 0,
    -2, -18, -2, 0, 0, 0, 0, 0,
    0, -2, 0, -5, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -3, 0, -3, 0, -7,
    0, 0, 0, 0, 0, 1, -4, 0,
    -4, -5, -3, 0, 0, 0, 0, 0,
    0, -3, -2, -4, 0, 0, 0, 0,
    0, -4, -3, -4, -4, -3, -4, -4,
    0, 0, 0, 0, -24, -17, -24, -18,
    -6, -6, -2, -4, -4, -27, -4, -4,
    -3, 0, 0, 0, 0, -7, 0, -18,
    -11, 0, -16, 0, 0, -11, -11, -7,
    -9, -4, -6, -9, -4, -12, 0, 0,
    0, 0, 0, -9, 0, 0, 0, 0,
    0, -2, -5, -9, -8, 0, -3, -2,
    -2, 0, -4, -5, 0, -5, -6, -5,
    -4, 0, 0, 0, 0, -4, -6, -5,
    -5, -6, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -24, -8, -14, -8, 0,
    -20, 0, 0, 0, 0, 0, 9, 0,
    20, 0, 0, 0, 0, -5, -3, 0,
    4, 0, 0, 0, 0, -15, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, -6, 0, -4, -1, 0, -6, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, -8, 0, -7, -3, 2, -3, 0,
    0, 0, -3, 0, 0, 0, 0, -15,
    0, -5, 0, -1, -12, 0, -7, -4,
    0, 0, 0, 0, 0, 0, 0, -4,
    0, -1, -1, -4, -1, -1, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, -4,
    0, 0, -6, 0, 0, -3, -6, 0,
    -3, 0, 0, 0, 0, -3, 0, 2,
    2, 3, 2, 0, 0, 0, 0, -9,
    0, 3, 0, 0, 0, 0, -2, 0,
    0, -5, -5, -6, 0, -4, -3, 0,
    -7, 0, -5, -4, 0, 0, -3, 0,
    0, 0, 0, -3, 0, 2, 2, -2,
    2, 0, 4, 11, 14, 0, -13, -4,
    -13, -4, 0, 0, 7, 0, 0, 0,
    0, 12, 0, 18, 12, 9, 16, 0,
    18, -5, -3, 0, -4, 0, -3, 0,
    -1, 0, 0, 4, 0, -1, 0, -4,
    0, 0, 4, -9, 0, 0, 0, 13,
    0, 0, -9, 0, 0, 0, 0, -7,
    0, 0, 0, 0, -4, 0, 0, -4,
    -4, 0, 0, 0, 10, 0, 0, 0,
    0, -1, -1, 0, 4, -4, 0, 0,
    0, -9, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, -6, 0, -3,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    4, -10, 4, 0, 4, 4, -3, 0,
    0, 0, 0, -9, 0, 0, 0, 0,
    -3, 0, 0, -3, -5, 0, -3, 0,
    -3, 0, 0, -5, -4, 0, 0, -2,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, -4, 0, 0, -8, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -14, -6, -14, -9, 7, 7,
    0, -4, 0, -14, 0, 0, 0, 0,
    0, 0, 0, -3, 4, -6, -3, 0,
    -3, 0, 0, 0, -1, 0, 0, 7,
    6, 0, 7, -1, 0, 0, 0, -14,
    0, 3, 0, 0, 0, 0, -2, 0,
    0, 0, 0, -6, 0, -3, 0, 0,
    -5, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, -7,
    2, 3, 4, 4, -7, 0, 0, 0,
    0, -4, 0, 0, 0, 0, -1, 0,
    0, -6, -4, 0, -3, 0, 0, 0,
    -3, -5, 0, 0, 0, -5, 0, 0,
    0, 0, 0, -3, -8, -2, -8, -5,
    0, 0, 0, -3, 0, -11, 0, -5,
    0, -2, 0, 0, -4, -3, 0, -5,
    -1, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, 0,
    0, -3, -9, 0, -9, -2, 0, 0,
    0, -1, 0, -8, 0, -6, 0, -2,
    0, -4, -6, 0, 0, -3, -1, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, -4, -4, 0, 0, -6,
    2, -4, -2, 0, 0, 2, 0, 0,
    -3, 0, -1, -9, 0, -4, 0, -3,
    -9, 0, 0, -3, -4, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -8, 0,
    -8, -4, 0, 0, 0, 0, 0, -11,
    0, -5, 0, -1, 0, -1, -2, 0,
    0, -5, -1, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, -4, 0, -6,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    0, 0, 0, 0, -8, 0, 0, -6,
    -3, 0, -2, 0, 0, 0, 0, 0,
    -3, -1, 0, 0, -1, 0
};


/*Collect the kern class' data in one place*/
static const lv_font_fmt_txt_kern_classes_t kern_classes =
{
    .class_pair_values   = kern_class_values,
    .left_class_mapping  = kern_left_class_mapping,
    .right_class_mapping = kern_right_class_mapping,
    .left_class_cnt      = 45,
    .right_class_cnt     = 38,
};

/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LV_VERSION_CHECK(8, 0, 0)
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = &kern_classes,
    .kern_scale = 16,
    .cmap_num = 2,
    .bpp = 1,
    .kern_classes = 1,
    .bitmap_format = 0,
#if LV_VERSION_CHECK(8, 0, 0)
    .cache = &cache
#endif
};


/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LV_VERSION_CHECK(8, 0, 0)
const lv_font_t ui_font_misc = {
#else
lv_font_t ui_font_misc = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 17,          /*The maximum line height required by the font*/
    .base_line = 4,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if UI_FONT_MISC*/

