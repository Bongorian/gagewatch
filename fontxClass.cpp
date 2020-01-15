//https://garretlab.web.fc2.com/arduino/lab/fontx/

#include "fontxClass.h"

fontxClass::fontxClass()
{
    numFonts = 0;
}

bool fontxClass::begin(FS *fs, const char *hfile, const char *zfile)
{
    if (!numFonts && openFontx(fs, hfile, 0) && openFontx(fs, zfile, 1))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void fontxClass::end()
{
    numFonts = 0;
    for (int i = 0; i < 2; i++)
    {
        fonts[i].fs.close();
    }
}

bool fontxClass::openFontx(FS *fs, const char *filepath, int codeFlag)
{
    char buf[18];

    if (fonts[codeFlag].fs = fs->open(filepath, "r"))
    {
        fonts[codeFlag].fs.readBytes(buf, sizeof(buf));
        fonts[codeFlag].width = buf[14];
        fonts[codeFlag].height = buf[15];
        fonts[codeFlag].codeFlag = buf[16];
        fonts[codeFlag].numCodeBlocks = buf[17]; // for multibyte character set
        fonts[codeFlag].glyphSize = (fonts[codeFlag].width + 7) / 8 * fonts[codeFlag].height;

        if (fonts[codeFlag].codeFlag != codeFlag)
        {
            end();
            return false;
        }

        numFonts++;
        return true;
    }

    return false;
}

bool fontxClass::getGlyph(uint16_t scode, uint8_t *width, uint8_t *height, uint8_t *glyph)
{
    uint8_t ncb;
    uint16_t startCode, endCode;
    uint8_t fontIndex;
    uint16_t numChars = 0;
    uint32_t offset = 0;
    uint8_t index;

    if (scode < 0x100)
    { // single-byte character
        index = 0;
        offset = 17 + scode * fonts[index].glyphSize;
    }
    else
    { // multibyte character
        index = 1;
        ncb = fonts[index].numCodeBlocks;

        fonts[index].fs.seek(18);
        while (ncb--)
        {
            fonts[index].fs.readBytes((char *)&startCode, 2);
            fonts[index].fs.readBytes((char *)&endCode, 2);
            if ((scode >= startCode) && (scode <= endCode))
            { // found the code
                numChars += scode - startCode;
                offset = 18 + 4 * fonts[index].numCodeBlocks + numChars * fonts[index].glyphSize;
                break;
            }
            numChars += endCode - startCode + 1; // add the number of characters in the code block
        }
    }

    if (offset)
    {
        fonts[index].fs.seek(offset);
        fonts[index].fs.readBytes((char *)glyph, fonts[index].glyphSize);
        *width = fonts[index].width;
        *height = fonts[index].height;
        return true;
    }

    return false;
}