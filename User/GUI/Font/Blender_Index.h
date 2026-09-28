#ifndef BLENDER_INDEX_H
#define BLENDER_INDEX_H

#include "Blender_18.h"
#include "Blender_24.h"
#include "Blender_72.h"
#include "Blender_34.h"
#include "Blender_144.h"

static inline const LCD_FontGlyph* Blender_FindGlyph(char ch, uint16_t size)
{
    switch(size)
    {
        case 18:
            switch(ch)
            {
                case '0': return &glyph_18_0030;
                case '1': return &glyph_18_0031;
                case '2': return &glyph_18_0032;
                case '3': return &glyph_18_0033;
                case '4': return &glyph_18_0034;
                case '5': return &glyph_18_0035;
                case '6': return &glyph_18_0036;
                case '7': return &glyph_18_0037;
                case '8': return &glyph_18_0038;
                case '9': return &glyph_18_0039;
                case '.': return &glyph_18_002E;
                case '-': return &glyph_18_002D;
                case ':': return &glyph_18_003A;
                case '%': return &glyph_18_0025;
                case '/': return &glyph_18_002F;
                case 'A': return &glyph_18_0041;
                case 'B': return &glyph_18_0042;
                case 'C': return &glyph_18_0043;
                case 'D': return &glyph_18_0044;
                case 'E': return &glyph_18_0045;
                case 'F': return &glyph_18_0046;
                case 'G': return &glyph_18_0047;
                case 'H': return &glyph_18_0048;
                case 'I': return &glyph_18_0049;
                case 'J': return &glyph_18_004A;
                case 'K': return &glyph_18_004B;
                case 'L': return &glyph_18_004C;
                case 'M': return &glyph_18_004D;
                case 'N': return &glyph_18_004E;
                case 'O': return &glyph_18_004F;
                case 'P': return &glyph_18_0050;
                case 'Q': return &glyph_18_0051;
                case 'R': return &glyph_18_0052;
                case 'S': return &glyph_18_0053;
                case 'T': return &glyph_18_0054;
                case 'U': return &glyph_18_0055;
                case 'V': return &glyph_18_0056;
                case 'W': return &glyph_18_0057;
                case 'X': return &glyph_18_0058;
                case 'Y': return &glyph_18_0059;
                case 'Z': return &glyph_18_005A;
                default: return 0;
            }
        case 24:
            switch(ch)
            {
                case '0': return &glyph_24_0030;
                case '1': return &glyph_24_0031;
                case '2': return &glyph_24_0032;
                case '3': return &glyph_24_0033;
                case '4': return &glyph_24_0034;
                case '5': return &glyph_24_0035;
                case '6': return &glyph_24_0036;
                case '7': return &glyph_24_0037;
                case '8': return &glyph_24_0038;
                case '9': return &glyph_24_0039;
                case '.': return &glyph_24_002E;
                case '-': return &glyph_24_002D;
                case ':': return &glyph_24_003A;
                case '%': return &glyph_24_0025;
                case '/': return &glyph_24_002F;
                case 'A': return &glyph_24_0041;
                case 'B': return &glyph_24_0042;
                case 'C': return &glyph_24_0043;
                case 'D': return &glyph_24_0044;
                case 'E': return &glyph_24_0045;
                case 'F': return &glyph_24_0046;
                case 'G': return &glyph_24_0047;
                case 'H': return &glyph_24_0048;
                case 'I': return &glyph_24_0049;
                case 'J': return &glyph_24_004A;
                case 'K': return &glyph_24_004B;
                case 'L': return &glyph_24_004C;
                case 'M': return &glyph_24_004D;
                case 'N': return &glyph_24_004E;
                case 'O': return &glyph_24_004F;
                case 'P': return &glyph_24_0050;
                case 'Q': return &glyph_24_0051;
                case 'R': return &glyph_24_0052;
                case 'S': return &glyph_24_0053;
                case 'T': return &glyph_24_0054;
                case 'U': return &glyph_24_0055;
                case 'V': return &glyph_24_0056;
                case 'W': return &glyph_24_0057;
                case 'X': return &glyph_24_0058;
                case 'Y': return &glyph_24_0059;
                case 'Z': return &glyph_24_005A;
                default: return 0;
            }
        case 72:
            switch(ch)
            {
                case '0': return &glyph_72_0030;
                case '1': return &glyph_72_0031;
                case '2': return &glyph_72_0032;
                case '3': return &glyph_72_0033;
                case '4': return &glyph_72_0034;
                case '5': return &glyph_72_0035;
                case '6': return &glyph_72_0036;
                case '7': return &glyph_72_0037;
                case '8': return &glyph_72_0038;
                case '9': return &glyph_72_0039;
                case '.': return &glyph_72_002E;
                case '-': return &glyph_72_002D;
                case ':': return &glyph_72_003A;
                case '%': return &glyph_72_0025;
                case '/': return &glyph_72_002F;
                case 'A': return &glyph_72_0041;
                case 'B': return &glyph_72_0042;
                case 'C': return &glyph_72_0043;
                case 'D': return &glyph_72_0044;
                case 'E': return &glyph_72_0045;
                case 'F': return &glyph_72_0046;
                case 'G': return &glyph_72_0047;
                case 'H': return &glyph_72_0048;
                case 'I': return &glyph_72_0049;
                case 'J': return &glyph_72_004A;
                case 'K': return &glyph_72_004B;
                case 'L': return &glyph_72_004C;
                case 'M': return &glyph_72_004D;
                case 'N': return &glyph_72_004E;
                case 'O': return &glyph_72_004F;
                case 'P': return &glyph_72_0050;
                case 'Q': return &glyph_72_0051;
                case 'R': return &glyph_72_0052;
                case 'S': return &glyph_72_0053;
                case 'T': return &glyph_72_0054;
                case 'U': return &glyph_72_0055;
                case 'V': return &glyph_72_0056;
                case 'W': return &glyph_72_0057;
                case 'X': return &glyph_72_0058;
                case 'Y': return &glyph_72_0059;
                case 'Z': return &glyph_72_005A;
                default: return 0;
            }
        case 34:
            switch(ch)
            {
                case '0': return &glyph_34_0030;
                case '1': return &glyph_34_0031;
                case '2': return &glyph_34_0032;
                case '3': return &glyph_34_0033;
                case '4': return &glyph_34_0034;
                case '5': return &glyph_34_0035;
                case '6': return &glyph_34_0036;
                case '7': return &glyph_34_0037;
                case '8': return &glyph_34_0038;
                case '9': return &glyph_34_0039;
                case '.': return &glyph_34_002E;
                case '-': return &glyph_34_002D;
                case ':': return &glyph_34_003A;
                case '%': return &glyph_34_0025;
                case '/': return &glyph_34_002F;
                case 'A': return &glyph_34_0041;
                case 'B': return &glyph_34_0042;
                case 'C': return &glyph_34_0043;
                case 'D': return &glyph_34_0044;
                case 'E': return &glyph_34_0045;
                case 'F': return &glyph_34_0046;
                case 'G': return &glyph_34_0047;
                case 'H': return &glyph_34_0048;
                case 'I': return &glyph_34_0049;
                case 'J': return &glyph_34_004A;
                case 'K': return &glyph_34_004B;
                case 'L': return &glyph_34_004C;
                case 'M': return &glyph_34_004D;
                case 'N': return &glyph_34_004E;
                case 'O': return &glyph_34_004F;
                case 'P': return &glyph_34_0050;
                case 'Q': return &glyph_34_0051;
                case 'R': return &glyph_34_0052;
                case 'S': return &glyph_34_0053;
                case 'T': return &glyph_34_0054;
                case 'U': return &glyph_34_0055;
                case 'V': return &glyph_34_0056;
                case 'W': return &glyph_34_0057;
                case 'X': return &glyph_34_0058;
                case 'Y': return &glyph_34_0059;
                case 'Z': return &glyph_34_005A;
                default: return 0;
            }
        case 144:
            switch(ch)
            {
                case '0': return &glyph_144_0030;
                case '1': return &glyph_144_0031;
                case '2': return &glyph_144_0032;
                case '3': return &glyph_144_0033;
                case '4': return &glyph_144_0034;
                case '5': return &glyph_144_0035;
                case '6': return &glyph_144_0036;
                case '7': return &glyph_144_0037;
                case '8': return &glyph_144_0038;
                case '9': return &glyph_144_0039;
                case '.': return &glyph_144_002E;
                case '-': return &glyph_144_002D;
                case ':': return &glyph_144_003A;
                case '%': return &glyph_144_0025;
                case '/': return &glyph_144_002F;
                case 'A': return &glyph_144_0041;
                case 'B': return &glyph_144_0042;
                case 'C': return &glyph_144_0043;
                case 'D': return &glyph_144_0044;
                case 'E': return &glyph_144_0045;
                case 'F': return &glyph_144_0046;
                case 'G': return &glyph_144_0047;
                case 'H': return &glyph_144_0048;
                case 'I': return &glyph_144_0049;
                case 'J': return &glyph_144_004A;
                case 'K': return &glyph_144_004B;
                case 'L': return &glyph_144_004C;
                case 'M': return &glyph_144_004D;
                case 'N': return &glyph_144_004E;
                case 'O': return &glyph_144_004F;
                case 'P': return &glyph_144_0050;
                case 'Q': return &glyph_144_0051;
                case 'R': return &glyph_144_0052;
                case 'S': return &glyph_144_0053;
                case 'T': return &glyph_144_0054;
                case 'U': return &glyph_144_0055;
                case 'V': return &glyph_144_0056;
                case 'W': return &glyph_144_0057;
                case 'X': return &glyph_144_0058;
                case 'Y': return &glyph_144_0059;
                case 'Z': return &glyph_144_005A;
                default: return 0;
            }
        default: return 0;
    }
}

#endif
