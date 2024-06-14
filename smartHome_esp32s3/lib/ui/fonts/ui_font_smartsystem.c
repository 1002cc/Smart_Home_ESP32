/*******************************************************************************
 * Size: 16 px
 * Bpp: 1
 * Opts: --bpp 1 --size 16 --font E:/Smart_Home/ESP32/smartHome_espidf/gui/assets/SourceHanSansCN-Normal-2.otf -o E:/Smart_Home/ESP32/smartHome_espidf/gui/assets\ui_font_smartsystem.c --format lvgl -r 0x20-0x7f --symbols 智能家居 --no-compress --no-prefilter
 ******************************************************************************/

#include "../ui.h"

#ifndef UI_FONT_SMARTSYSTEM
#define UI_FONT_SMARTSYSTEM 1
#endif

#if UI_FONT_SMARTSYSTEM

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xaa, 0xaa, 0xf,

    /* U+0022 "\"" */
    0x99, 0x99,

    /* U+0023 "#" */
    0x24, 0x48, 0x97, 0xf4, 0x48, 0x92, 0x7f, 0x48,
    0x91, 0x22, 0x40,

    /* U+0024 "$" */
    0x10, 0x47, 0xb1, 0x82, 0xc, 0x1c, 0x18, 0x30,
    0x41, 0x8d, 0xe1, 0x4,

    /* U+0025 "%" */
    0x70, 0x46, 0xc4, 0x22, 0x21, 0x12, 0x8, 0x97,
    0x6d, 0x6d, 0xca, 0x20, 0x91, 0x4, 0x88, 0x44,
    0x44, 0x36, 0x20, 0xe0,

    /* U+0026 "&" */
    0x18, 0x12, 0x9, 0x4, 0x82, 0x80, 0x81, 0xc3,
    0x93, 0x8d, 0x43, 0x31, 0xcf, 0x10,

    /* U+0027 "'" */
    0xf0,

    /* U+0028 "(" */
    0x29, 0x29, 0x24, 0x92, 0x44, 0x91,

    /* U+0029 ")" */
    0x89, 0x22, 0x49, 0x24, 0x94, 0x94,

    /* U+002A "*" */
    0x21, 0x1e, 0xe5, 0x0,

    /* U+002B "+" */
    0x10, 0x20, 0x47, 0xf1, 0x2, 0x4, 0x8,

    /* U+002C "," */
    0x6c, 0xa4,

    /* U+002D "-" */
    0xf0,

    /* U+002E "." */
    0xf0,

    /* U+002F "/" */
    0x4, 0x10, 0x82, 0x8, 0x41, 0x4, 0x20, 0x82,
    0x10, 0x41, 0x8, 0x20,

    /* U+0030 "0" */
    0x38, 0x8b, 0x1c, 0x18, 0x30, 0x60, 0xc1, 0x83,
    0x89, 0x11, 0xc0,

    /* U+0031 "1" */
    0x31, 0xc1, 0x4, 0x10, 0x41, 0x4, 0x10, 0x41,
    0x3f,

    /* U+0032 "2" */
    0x79, 0x88, 0x8, 0x10, 0x20, 0xc1, 0x4, 0x18,
    0x61, 0x87, 0xf0,

    /* U+0033 "3" */
    0x3c, 0x46, 0x2, 0x2, 0x4, 0x38, 0x6, 0x1,
    0x1, 0x1, 0xc2, 0x3c,

    /* U+0034 "4" */
    0xc, 0xc, 0x1c, 0x14, 0x24, 0x24, 0x44, 0xc4,
    0xff, 0x4, 0x4, 0x4,

    /* U+0035 "5" */
    0x7e, 0x81, 0x2, 0x7, 0xc8, 0x80, 0x81, 0x2,
    0x7, 0x13, 0xc0,

    /* U+0036 "6" */
    0x3c, 0xc5, 0x4, 0xb, 0xd8, 0xa0, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+0037 "7" */
    0xfe, 0x8, 0x10, 0x40, 0x81, 0x4, 0x8, 0x10,
    0x20, 0x40, 0x80,

    /* U+0038 "8" */
    0x3c, 0xcd, 0xa, 0x16, 0x26, 0x93, 0x43, 0x83,
    0x7, 0x1b, 0xe0,

    /* U+0039 "9" */
    0x78, 0x8a, 0xc, 0x18, 0x38, 0xde, 0x81, 0x2,
    0xa, 0x33, 0xc0,

    /* U+003A ":" */
    0xf0, 0x3, 0xc0,

    /* U+003B ";" */
    0xf0, 0x3, 0xda,

    /* U+003C "<" */
    0x1, 0x7, 0x38, 0xc0, 0xe0, 0x38, 0x7, 0x1,

    /* U+003D "=" */
    0xff, 0x0, 0x0, 0x0, 0xff,

    /* U+003E ">" */
    0x0, 0xe0, 0x38, 0x7, 0x7, 0x38, 0xe0, 0x0,

    /* U+003F "?" */
    0x7b, 0x30, 0x41, 0x8, 0x61, 0xc, 0x0, 0x3,
    0xc,

    /* U+0040 "@" */
    0x7, 0xc0, 0xc3, 0x8, 0x4, 0x80, 0x34, 0x38,
    0xc2, 0x46, 0x22, 0x31, 0x11, 0x88, 0x8c, 0x4c,
    0xa1, 0xb8, 0x80, 0x6, 0x0, 0x18, 0x0, 0x3e,
    0x0,

    /* U+0041 "A" */
    0xc, 0x3, 0x1, 0x40, 0x58, 0x12, 0x8, 0x82,
    0x30, 0xfc, 0x41, 0x10, 0x64, 0xb, 0x2,

    /* U+0042 "B" */
    0xfc, 0x86, 0x82, 0x82, 0x86, 0xfc, 0x82, 0x81,
    0x81, 0x81, 0x82, 0xfc,

    /* U+0043 "C" */
    0x1e, 0x63, 0x40, 0x80, 0x80, 0x80, 0x80, 0x80,
    0x80, 0x40, 0x63, 0x1e,

    /* U+0044 "D" */
    0xf8, 0x86, 0x82, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x82, 0x86, 0xf8,

    /* U+0045 "E" */
    0xfd, 0x2, 0x4, 0x8, 0x1f, 0xa0, 0x40, 0x81,
    0x2, 0x7, 0xf0,

    /* U+0046 "F" */
    0xfe, 0x8, 0x20, 0x82, 0xf, 0xe0, 0x82, 0x8,
    0x20,

    /* U+0047 "G" */
    0x1f, 0x10, 0xd0, 0x10, 0x8, 0x4, 0x2, 0x1f,
    0x1, 0xc0, 0xa0, 0x48, 0x23, 0xe0,

    /* U+0048 "H" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0xff, 0x81, 0x81,
    0x81, 0x81, 0x81, 0x81,

    /* U+0049 "I" */
    0xff, 0xf0,

    /* U+004A "J" */
    0x4, 0x10, 0x41, 0x4, 0x10, 0x41, 0x4, 0x1c,
    0xde,

    /* U+004B "K" */
    0x82, 0x84, 0x8c, 0x98, 0xb0, 0xb8, 0xc8, 0xcc,
    0x84, 0x86, 0x82, 0x83,

    /* U+004C "L" */
    0x82, 0x8, 0x20, 0x82, 0x8, 0x20, 0x82, 0x8,
    0x3f,

    /* U+004D "M" */
    0xc0, 0xf0, 0x3e, 0xe, 0x85, 0xa1, 0x6c, 0xd9,
    0x26, 0x49, 0x8c, 0x63, 0x18, 0xc6, 0x1,

    /* U+004E "N" */
    0x81, 0xc1, 0xe1, 0xa1, 0x91, 0x91, 0x89, 0x89,
    0x85, 0x87, 0x83, 0x81,

    /* U+004F "O" */
    0x1e, 0x18, 0x64, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x80, 0x50, 0x26, 0x10, 0x78,

    /* U+0050 "P" */
    0xf9, 0xe, 0xc, 0x18, 0x30, 0xbe, 0x40, 0x81,
    0x2, 0x4, 0x0,

    /* U+0051 "Q" */
    0x1e, 0x18, 0x44, 0xa, 0x1, 0x80, 0x60, 0x18,
    0x6, 0x1, 0x80, 0x50, 0x26, 0x18, 0xfc, 0xc,
    0x1, 0x80, 0x3c,

    /* U+0052 "R" */
    0xfd, 0xe, 0xc, 0x18, 0x30, 0xbf, 0x4c, 0x89,
    0xa, 0x1c, 0x10,

    /* U+0053 "S" */
    0x3e, 0x63, 0x40, 0x40, 0x60, 0x3c, 0xe, 0x3,
    0x1, 0x1, 0x62, 0x3c,

    /* U+0054 "T" */
    0xff, 0x84, 0x2, 0x1, 0x0, 0x80, 0x40, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+0055 "U" */
    0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81, 0x81,
    0x81, 0x81, 0x42, 0x3c,

    /* U+0056 "V" */
    0xc0, 0xe0, 0xd0, 0x48, 0x26, 0x31, 0x10, 0x88,
    0x64, 0x14, 0xa, 0x7, 0x1, 0x0,

    /* U+0057 "W" */
    0x43, 0xa, 0x18, 0x50, 0xc2, 0x87, 0x16, 0x49,
    0x92, 0x48, 0x92, 0x44, 0x92, 0x28, 0x51, 0xc3,
    0x86, 0x18, 0x30, 0xc0,

    /* U+0058 "X" */
    0x41, 0x31, 0x88, 0x86, 0xc1, 0x40, 0x60, 0x70,
    0x28, 0x36, 0x11, 0x10, 0xc8, 0x20,

    /* U+0059 "Y" */
    0x41, 0x30, 0x88, 0x84, 0x41, 0x40, 0xa0, 0x20,
    0x10, 0x8, 0x4, 0x2, 0x1, 0x0,

    /* U+005A "Z" */
    0x7f, 0x3, 0x6, 0x4, 0xc, 0x8, 0x18, 0x30,
    0x20, 0x60, 0xc0, 0xff,

    /* U+005B "[" */
    0xf2, 0x49, 0x24, 0x92, 0x49, 0x27,

    /* U+005C "\\" */
    0x81, 0x4, 0x10, 0x20, 0x82, 0x4, 0x10, 0x40,
    0x82, 0x8, 0x20, 0x41,

    /* U+005D "]" */
    0xe4, 0x92, 0x49, 0x24, 0x92, 0x4f,

    /* U+005E "^" */
    0x10, 0x50, 0xa3, 0x44, 0x48, 0xa1, 0x80,

    /* U+005F "_" */
    0xff, 0x80,

    /* U+0060 "`" */
    0x46, 0x20,

    /* U+0061 "a" */
    0x7d, 0x8c, 0x8, 0x37, 0xb0, 0x60, 0xc3, 0x7a,

    /* U+0062 "b" */
    0x81, 0x2, 0x4, 0xb, 0x98, 0xa0, 0xc1, 0x83,
    0x6, 0xe, 0x2b, 0x80,

    /* U+0063 "c" */
    0x3d, 0x18, 0x20, 0x82, 0x8, 0x11, 0x3c,

    /* U+0064 "d" */
    0x2, 0x4, 0x8, 0x13, 0xa8, 0xe0, 0xc1, 0x83,
    0x6, 0xa, 0x33, 0xa0,

    /* U+0065 "e" */
    0x38, 0x8a, 0xc, 0x1f, 0xf0, 0x20, 0x20, 0x3c,

    /* U+0066 "f" */
    0x3a, 0x10, 0x8f, 0x21, 0x8, 0x42, 0x10, 0x84,
    0x0,

    /* U+0067 "g" */
    0x3f, 0xc6, 0x82, 0x82, 0xc6, 0x78, 0x80, 0x80,
    0x7e, 0x81, 0x81, 0xc2, 0x7c,

    /* U+0068 "h" */
    0x81, 0x2, 0x4, 0xb, 0xd8, 0xe0, 0xc1, 0x83,
    0x6, 0xc, 0x18, 0x20,

    /* U+0069 "i" */
    0xf0, 0xaa, 0xaa, 0x80,

    /* U+006A "j" */
    0x33, 0x0, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22,
    0xc0,

    /* U+006B "k" */
    0x81, 0x2, 0x4, 0x8, 0xd1, 0x24, 0x58, 0xf1,
    0xb2, 0x24, 0x68, 0x40,

    /* U+006C "l" */
    0xaa, 0xaa, 0xaa, 0xc0,

    /* U+006D "m" */
    0xb9, 0xd9, 0xce, 0x10, 0xc2, 0x18, 0x43, 0x8,
    0x61, 0xc, 0x21, 0x84, 0x20,

    /* U+006E "n" */
    0xbd, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xc1, 0x82,

    /* U+006F "o" */
    0x3c, 0x42, 0x83, 0x81, 0x81, 0x81, 0x81, 0x42,
    0x3c,

    /* U+0070 "p" */
    0xb9, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xe2, 0xb9,
    0x2, 0x4, 0x8, 0x0,

    /* U+0071 "q" */
    0x3a, 0x8e, 0xc, 0x18, 0x30, 0x60, 0xa3, 0x3a,
    0x4, 0x8, 0x10, 0x20,

    /* U+0072 "r" */
    0xbc, 0x88, 0x88, 0x88, 0x80,

    /* U+0073 "s" */
    0x7a, 0x8, 0x30, 0x38, 0x30, 0x61, 0x78,

    /* U+0074 "t" */
    0x21, 0x9, 0xf2, 0x10, 0x84, 0x21, 0x8, 0x30,

    /* U+0075 "u" */
    0x83, 0x6, 0xc, 0x18, 0x30, 0x60, 0xe3, 0x7a,

    /* U+0076 "v" */
    0xc1, 0x42, 0x42, 0x62, 0x24, 0x24, 0x34, 0x18,
    0x18,

    /* U+0077 "w" */
    0x42, 0x14, 0x61, 0x45, 0x36, 0x52, 0x25, 0x22,
    0x92, 0x28, 0xe3, 0x8c, 0x18, 0xc0,

    /* U+0078 "x" */
    0x42, 0x64, 0x24, 0x18, 0x18, 0x38, 0x24, 0x46,
    0x42,

    /* U+0079 "y" */
    0xc1, 0x42, 0x42, 0x22, 0x24, 0x24, 0x14, 0x18,
    0x18, 0x8, 0x10, 0x10, 0x60,

    /* U+007A "z" */
    0xfc, 0x21, 0x84, 0x20, 0x84, 0x20, 0xfc,

    /* U+007B "{" */
    0x34, 0x44, 0x44, 0x44, 0x84, 0x44, 0x44, 0x47,

    /* U+007C "|" */
    0xff, 0xff, 0xc0,

    /* U+007D "}" */
    0xc2, 0x22, 0x22, 0x22, 0x12, 0x22, 0x22, 0x2e,

    /* U+007E "~" */
    0x61, 0x24, 0x30,

    /* U+5BB6 "家" */
    0x0, 0x0, 0x8, 0xf, 0xff, 0xa0, 0x2, 0x80,
    0x8, 0xff, 0x80, 0x40, 0xf, 0x8c, 0xc6, 0x40,
    0x27, 0x7, 0x34, 0x21, 0x48, 0x39, 0x33, 0x4,
    0x70, 0xe0, 0x0,

    /* U+5C45 "居" */
    0x3f, 0xf8, 0x40, 0x10, 0x80, 0x21, 0xff, 0xc2,
    0x8, 0x4, 0x10, 0xf, 0xff, 0x90, 0x40, 0x20,
    0x80, 0x5f, 0xf0, 0xa0, 0x22, 0x40, 0x44, 0x80,
    0x89, 0xff, 0x0,

    /* U+667A "智" */
    0x40, 0x3, 0xf7, 0xf4, 0x23, 0xfd, 0x11, 0x8,
    0x96, 0x7d, 0x98, 0x18, 0x0, 0x3f, 0xf1, 0x0,
    0x8f, 0xfc, 0x40, 0x22, 0x1, 0x1f, 0xf8,

    /* U+80FD "能" */
    0x10, 0x80, 0x92, 0x24, 0x2f, 0x3f, 0xa0, 0x0,
    0x84, 0x2, 0x17, 0xe7, 0xd0, 0xa0, 0x7e, 0x89,
    0xa, 0xe7, 0xe8, 0x10, 0xa1, 0x42, 0x85, 0x39,
    0xf0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 57, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 80, .box_w = 2, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 4, .adv_w = 117, .box_w = 4, .box_h = 4, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 6, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 17, .adv_w = 141, .box_w = 6, .box_h = 16, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 29, .adv_w = 234, .box_w = 13, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 49, .adv_w = 171, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 63, .adv_w = 69, .box_w = 1, .box_h = 4, .ofs_x = 1, .ofs_y = 9},
    {.bitmap_index = 64, .adv_w = 85, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 70, .adv_w = 85, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 76, .adv_w = 118, .box_w = 5, .box_h = 5, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 80, .adv_w = 141, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 87, .adv_w = 69, .box_w = 3, .box_h = 5, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 89, .adv_w = 88, .box_w = 4, .box_h = 1, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 90, .adv_w = 69, .box_w = 2, .box_h = 2, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 91, .adv_w = 100, .box_w = 6, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 103, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 114, .adv_w = 141, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 123, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 141, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 146, .adv_w = 141, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 158, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 169, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 191, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 202, .adv_w = 141, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 213, .adv_w = 69, .box_w = 2, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 216, .adv_w = 69, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 219, .adv_w = 141, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 227, .adv_w = 141, .box_w = 8, .box_h = 5, .ofs_x = 1, .ofs_y = 4},
    {.bitmap_index = 232, .adv_w = 141, .box_w = 8, .box_h = 8, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 240, .adv_w = 120, .box_w = 6, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 249, .adv_w = 239, .box_w = 13, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 274, .adv_w = 154, .box_w = 10, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 289, .adv_w = 167, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 301, .adv_w = 162, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 313, .adv_w = 175, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 325, .adv_w = 150, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 140, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 345, .adv_w = 175, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 359, .adv_w = 185, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 371, .adv_w = 73, .box_w = 1, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 373, .adv_w = 136, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 382, .adv_w = 164, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 394, .adv_w = 137, .box_w = 6, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 403, .adv_w = 206, .box_w = 10, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 418, .adv_w = 184, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 430, .adv_w = 189, .box_w = 10, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 445, .adv_w = 160, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 456, .adv_w = 189, .box_w = 10, .box_h = 15, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 475, .adv_w = 161, .box_w = 7, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 486, .adv_w = 152, .box_w = 8, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 498, .adv_w = 152, .box_w = 9, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 512, .adv_w = 183, .box_w = 8, .box_h = 12, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 524, .adv_w = 145, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 538, .adv_w = 223, .box_w = 13, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 558, .adv_w = 144, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 572, .adv_w = 134, .box_w = 9, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 586, .adv_w = 154, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 598, .adv_w = 85, .box_w = 3, .box_h = 16, .ofs_x = 2, .ofs_y = -3},
    {.bitmap_index = 604, .adv_w = 100, .box_w = 6, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 616, .adv_w = 85, .box_w = 3, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 622, .adv_w = 141, .box_w = 7, .box_h = 7, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 629, .adv_w = 143, .box_w = 9, .box_h = 1, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 631, .adv_w = 154, .box_w = 4, .box_h = 4, .ofs_x = 2, .ofs_y = 10},
    {.bitmap_index = 633, .adv_w = 143, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 157, .box_w = 7, .box_h = 13, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 653, .adv_w = 130, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 660, .adv_w = 157, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 672, .adv_w = 141, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 680, .adv_w = 81, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 689, .adv_w = 143, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 702, .adv_w = 154, .box_w = 7, .box_h = 13, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 714, .adv_w = 69, .box_w = 2, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 718, .adv_w = 69, .box_w = 4, .box_h = 17, .ofs_x = -1, .ofs_y = -4},
    {.bitmap_index = 727, .adv_w = 139, .box_w = 7, .box_h = 13, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 739, .adv_w = 71, .box_w = 2, .box_h = 13, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 743, .adv_w = 235, .box_w = 11, .box_h = 9, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 756, .adv_w = 155, .box_w = 7, .box_h = 9, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 764, .adv_w = 154, .box_w = 8, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 773, .adv_w = 157, .box_w = 7, .box_h = 13, .ofs_x = 2, .ofs_y = -4},
    {.bitmap_index = 785, .adv_w = 157, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = -4},
    {.bitmap_index = 797, .adv_w = 97, .box_w = 4, .box_h = 9, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 802, .adv_w = 119, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 809, .adv_w = 94, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 817, .adv_w = 154, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 825, .adv_w = 131, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 834, .adv_w = 202, .box_w = 12, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 848, .adv_w = 124, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 857, .adv_w = 131, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = -4},
    {.bitmap_index = 870, .adv_w = 120, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 877, .adv_w = 85, .box_w = 4, .box_h = 16, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 885, .adv_w = 68, .box_w = 1, .box_h = 18, .ofs_x = 2, .ofs_y = -5},
    {.bitmap_index = 888, .adv_w = 85, .box_w = 4, .box_h = 16, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 896, .adv_w = 141, .box_w = 7, .box_h = 3, .ofs_x = 1, .ofs_y = 5},
    {.bitmap_index = 899, .adv_w = 256, .box_w = 14, .box_h = 15, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 926, .adv_w = 256, .box_w = 15, .box_h = 14, .ofs_x = 0, .ofs_y = -1},
    {.bitmap_index = 953, .adv_w = 256, .box_w = 13, .box_h = 14, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 976, .adv_w = 256, .box_w = 14, .box_h = 14, .ofs_x = 1, .ofs_y = -1}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_1[] = {
    0x0, 0x8f, 0xac4, 0x2547
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 95, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 23478, .range_length = 9544, .glyph_id_start = 96,
        .unicode_list = unicode_list_1, .glyph_id_ofs_list = NULL, .list_length = 4, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
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
    0, 0, 0, 0
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
    0, 0, 0, 0
};

/*Kern values between classes*/
static const int8_t kern_class_values[] =
{
    0, 0, 0, 0, -32, 0, -32, 0,
    0, 0, 0, -15, 0, -26, -3, 0,
    0, 0, 0, -3, 0, 0, 0, 0,
    -9, 0, 0, 0, 0, 0, -6, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 23, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -26, 0, -38,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -28, -5, -18, -9, 0,
    -26, 0, 0, 0, -3, 0, 0, 0,
    7, 0, 0, -12, 0, -9, -6, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    -5, -13, 0, -5, -3, -7, -18, -6,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -8, 0, -2, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -12, -3, -23, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -7,
    -9, 0, -3, 8, 8, 0, 0, 3,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, -15, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, 0, 0, 0, 0,
    0, 0, 1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -15, 0, -27,
    0, 0, 0, 0, 0, 0, -7, -2,
    -3, 0, 0, -15, -5, -4, 0, 2,
    -4, -2, -12, 7, 0, -3, 0, 0,
    0, 0, 7, -4, -2, -2, -1, -1,
    -2, 0, 0, 0, 0, -9, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -4,
    -4, -7, 0, -1, -1, -1, -4, -1,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -4, -3, -3, -4, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, 0, 0, 0, -8, -3, -7, -5,
    -4, -1, -1, -1, -2, -3, 0, 0,
    0, 0, -6, 0, 0, 0, 0, -7,
    -3, -4, -3, 0, -4, 0, 0, 0,
    0, -10, 0, 0, 0, -5, 0, 0,
    0, -3, 0, -11, 0, -7, 0, -3,
    -2, -5, -6, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, -2, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, -7, 0, -3, 0, -8,
    -3, 0, 0, 0, 0, 0, -20, 0,
    -20, -21, 0, 0, 0, -11, -3, -40,
    -5, 0, 0, 2, 2, -7, 1, -9,
    0, -9, -4, 0, -7, 0, 0, -6,
    -5, -3, -4, -5, -4, -7, -4, -7,
    0, 0, 0, -8, 0, 0, 0, 0,
    0, 0, 0, -1, 0, 0, 0, -6,
    0, -4, -1, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -7, 0, -7, 0, 0, 0,
    0, 0, 0, -12, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -5, 0, -12,
    0, -7, 0, 0, 0, 0, -2, -3,
    -5, 0, -2, -5, -4, -3, -3, 0,
    -4, 0, 0, 0, -2, 0, 0, 0,
    -3, 0, 0, -9, -4, -5, -4, -4,
    -5, -4, 0, -23, 0, -43, 0, -16,
    0, 0, 0, 0, -8, 1, -7, 0,
    -6, -34, -8, -22, -16, 0, -21, 0,
    -23, 0, -3, -4, -1, 0, 0, 0,
    0, -5, -3, -9, -9, 0, -9, 0,
    0, 0, 0, 0, -31, -10, -31, -23,
    0, 0, 0, -14, 0, -42, -3, -7,
    0, 0, 0, -7, -3, -23, 0, -13,
    -7, 0, -9, 0, 0, 0, -3, 0,
    0, 0, 0, -4, 0, -6, 0, 0,
    0, -3, 0, -9, 0, 0, 0, 0,
    0, -1, 0, -5, -4, -4, 0, 2,
    2, -1, 0, -3, 0, -1, -3, 0,
    -1, 0, 0, 0, 0, 0, 0, 0,
    0, -2, 0, -2, 0, 0, 0, -5,
    0, 4, 0, 0, 0, 0, 0, 0,
    0, -4, -4, -6, 0, 0, 0, 0,
    -4, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -30, -21,
    -30, -26, -6, -6, 0, -12, -7, -36,
    -12, 0, 0, 0, 0, -6, -4, -16,
    0, -21, -18, -5, -21, 0, 0, -13,
    -17, -5, -13, -10, -10, -12, -10, -22,
    0, 0, 0, 0, -4, 0, -4, -9,
    0, 0, 0, -5, 0, -13, -3, 0,
    0, -1, 0, -3, -4, 0, 0, -1,
    0, 0, -3, 0, 0, 0, -1, 0,
    0, 0, 0, -2, 0, 0, 0, 0,
    0, 0, -18, -5, -18, -14, 0, 0,
    0, -4, -3, -21, -3, 0, -3, 3,
    0, 0, 0, -5, 0, -6, -4, 0,
    -6, 0, 0, -6, -3, 0, -9, -3,
    -3, -4, -3, -7, 0, 0, 0, 0,
    -9, -3, -9, -9, 0, 0, 0, 0,
    -2, -19, -2, 0, 0, 0, 0, 0,
    0, -2, 0, -5, 0, 0, -4, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -3, 0, -3, 0, -3, 0, -7,
    0, 0, 0, 0, 0, 1, -5, 0,
    -4, -6, -3, 0, 0, 0, 0, 0,
    0, -3, -2, -4, 0, 0, 0, 0,
    0, -4, -3, -4, -4, -3, -4, -4,
    0, 0, 0, 0, -25, -18, -25, -20,
    -7, -7, -2, -4, -4, -28, -4, -4,
    -3, 0, 0, 0, 0, -8, 0, -19,
    -11, 0, -17, 0, 0, -12, -11, -7,
    -9, -4, -7, -9, -4, -13, 0, 0,
    0, 0, 0, -10, 0, 0, 0, 0,
    0, -2, -6, -10, -9, 0, -3, -2,
    -2, 0, -4, -5, 0, -5, -6, -6,
    -4, 0, 0, 0, 0, -4, -7, -5,
    -5, -7, -5, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -25, -9, -15, -9, 0,
    -21, 0, 0, 0, 0, 0, 10, 0,
    21, 0, 0, 0, 0, -6, -3, 0,
    4, 0, 0, 0, 0, -16, 0, 0,
    0, 0, 0, 0, -3, 0, 0, 0,
    0, -7, 0, -4, -1, 0, -7, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, -4, 0, 0, 0, 0, 0, 0,
    0, -9, 0, -8, -3, 2, -3, 0,
    0, 0, -3, 0, 0, 0, 0, -16,
    0, -5, 0, -1, -13, 0, -7, -4,
    0, 0, 0, 0, 0, 0, 0, -5,
    0, -1, -1, -5, -1, -1, 0, 0,
    0, 0, 0, -5, 0, 0, 0, 0,
    0, 0, 0, 0, 0, -6, 0, -4,
    0, 0, -7, 0, 0, -3, -6, 0,
    -3, 0, 0, 0, 0, -3, 0, 2,
    2, 3, 2, 0, 0, 0, 0, -10,
    0, 3, 0, 0, 0, 0, -2, 0,
    0, -6, -6, -7, 0, -4, -3, 0,
    -7, 0, -5, -4, 0, 0, -3, 0,
    0, 0, 0, -3, 0, 2, 2, -2,
    2, 0, 4, 12, 15, 0, -14, -4,
    -14, -4, 0, 0, 8, 0, 0, 0,
    0, 13, 0, 19, 13, 10, 17, 0,
    19, -6, -3, 0, -4, 0, -3, 0,
    -1, 0, 0, 4, 0, -1, 0, -4,
    0, 0, 4, -10, 0, 0, 0, 14,
    0, 0, -10, 0, 0, 0, 0, -7,
    0, 0, 0, 0, -4, 0, 0, -4,
    -4, 0, 0, 0, 10, 0, 0, 0,
    0, -1, -1, 0, 5, -4, 0, 0,
    0, -10, 0, 0, 0, 0, 0, 0,
    -2, 0, 0, 0, 0, -7, 0, -3,
    0, 0, -4, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    4, -11, 4, 0, 4, 4, -3, 0,
    0, 0, 0, -9, 0, 0, 0, 0,
    -3, 0, 0, -3, -5, 0, -3, 0,
    -3, 0, 0, -5, -4, 0, 0, -2,
    0, -2, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 2, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -6,
    0, -4, 0, 0, -9, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, -15, -7, -15, -10, 8, 8,
    0, -4, 0, -15, 0, 0, 0, 0,
    0, 0, 0, -3, 4, -7, -3, 0,
    -3, 0, 0, 0, -1, 0, 0, 8,
    6, 0, 8, -1, 0, 0, 0, -15,
    0, 3, 0, 0, 0, 0, -3, 0,
    0, 0, 0, -7, 0, -3, 0, 0,
    -6, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, -6, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 3, -7,
    3, 3, 4, 4, -7, 0, 0, 0,
    0, -4, 0, 0, 0, 0, -1, 0,
    0, -6, -4, 0, -3, 0, 0, 0,
    -3, -6, 0, 0, 0, -5, 0, 0,
    0, 0, 0, -3, -9, -2, -9, -6,
    0, 0, 0, -3, 0, -12, 0, -6,
    0, -3, 0, 0, -4, -3, 0, -6,
    -1, 0, 0, 0, -3, 0, 0, 0,
    0, 0, 0, 0, 0, -7, 0, 0,
    0, -3, -10, 0, -10, -2, 0, 0,
    0, -1, 0, -9, 0, -7, 0, -3,
    0, -4, -7, 0, 0, -3, -1, 0,
    0, 0, -3, 0, 0, 0, 0, 0,
    0, 0, 0, -5, -4, 0, 0, -6,
    2, -4, -2, 0, 0, 2, 0, 0,
    -3, 0, -1, -10, 0, -4, 0, -3,
    -9, 0, 0, -3, -5, 0, 0, 0,
    0, 0, 0, -7, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, -9, 0,
    -9, -4, 0, 0, 0, 0, 0, -12,
    0, -6, 0, -1, 0, -1, -2, 0,
    0, -6, -1, 0, 0, 0, -3, 0,
    0, 0, 0, 0, 0, -4, 0, -7,
    0, 0, 0, 0, 0, -4, 0, 0,
    0, 0, 0, 0, 0, 0, 0, -8,
    0, 0, 0, 0, -9, 0, 0, -7,
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
const lv_font_t ui_font_smartsystem = {
#else
lv_font_t ui_font_smartsystem = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 19,          /*The maximum line height required by the font*/
    .base_line = 5,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -2,
    .underline_thickness = 1,
#endif
    .dsc = &font_dsc           /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
};



#endif /*#if UI_FONT_SMARTSYSTEM*/

