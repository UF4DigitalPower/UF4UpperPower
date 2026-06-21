#ifndef TEKO_SEMIBOLD_INDEX_H
#define TEKO_SEMIBOLD_INDEX_H

#include "Teko_SemiBold_24.h"
#include "Teko_SemiBold_72.h"
#include "Teko_SemiBold_196.h"

static inline const LCD_FontGlyph* Teko_SemiBold_FindGlyph(char ch, uint16_t size)
{
    switch(size)
    {
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
                case '%': return &glyph_24_0025;
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
                case '%': return &glyph_72_0025;
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
        case 196:
            switch(ch)
            {
                case '0': return &glyph_196_0030;
                case '1': return &glyph_196_0031;
                case '2': return &glyph_196_0032;
                case '3': return &glyph_196_0033;
                case '4': return &glyph_196_0034;
                case '5': return &glyph_196_0035;
                case '6': return &glyph_196_0036;
                case '7': return &glyph_196_0037;
                case '8': return &glyph_196_0038;
                case '9': return &glyph_196_0039;
                case '.': return &glyph_196_002E;
                case '%': return &glyph_196_0025;
                case 'A': return &glyph_196_0041;
                case 'B': return &glyph_196_0042;
                case 'C': return &glyph_196_0043;
                case 'D': return &glyph_196_0044;
                case 'E': return &glyph_196_0045;
                case 'F': return &glyph_196_0046;
                case 'G': return &glyph_196_0047;
                case 'H': return &glyph_196_0048;
                case 'I': return &glyph_196_0049;
                case 'J': return &glyph_196_004A;
                case 'K': return &glyph_196_004B;
                case 'L': return &glyph_196_004C;
                case 'M': return &glyph_196_004D;
                case 'N': return &glyph_196_004E;
                case 'O': return &glyph_196_004F;
                case 'P': return &glyph_196_0050;
                case 'Q': return &glyph_196_0051;
                case 'R': return &glyph_196_0052;
                case 'S': return &glyph_196_0053;
                case 'T': return &glyph_196_0054;
                case 'U': return &glyph_196_0055;
                case 'V': return &glyph_196_0056;
                case 'W': return &glyph_196_0057;
                case 'X': return &glyph_196_0058;
                case 'Y': return &glyph_196_0059;
                case 'Z': return &glyph_196_005A;
                default: return 0;
            }
        default: return 0;
    }
}

#endif
