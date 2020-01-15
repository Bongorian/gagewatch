//https://garretlab.web.fc2.com/arduino/lab/fontx/

#pragma once

#include <FS.h>

typedef struct
{
    uint8_t width;         // Font width
    uint8_t height;        // Font height
    uint8_t codeFlag;      // Code flag: 0 -> 1byte font, 1 -> 2byte font
    uint8_t numCodeBlocks; // Number of code block
    uint8_t glyphSize;     // Glyph size: per character size
    fs::File fs;           // File structure for font file
} fontxFile_t;

class fontxClass
{
public:
    fontxClass();
    bool begin(FS *fs, const char *hfile, const char *zfile);
    void end();
    bool getGlyph(uint16_t code, uint8_t *width, uint8_t *height, uint8_t *glyph);

private:
    fontxFile_t fonts[2];
    bool openFontx(FS *fs, const char *filepath, int codeFlag);
    int numFonts;
};