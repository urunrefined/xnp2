#ifndef FREEFONT_H
#define FREEFONT_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#include <stdexcept>
#include <string>
#include <hb.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig.h>

#include "Image.h"

namespace BR {

class FontconfigLib {
public:
	FcConfig *config = 0;

	FontconfigLib();
	~FontconfigLib();
};

class FontList {
	FcFontSet *fs = 0;

public:
	FontList(FontconfigLib& FontconfigLib);
	~FontList();

	void print();
	std::string getFirst();
};

class FreetypeLib {
public:
	FT_Library  library = 0;

	FreetypeLib();
	~FreetypeLib();
};

class FreetypeFace{
public:
	FT_Face face = 0;
	FT_GlyphSlot slot;

	FreetypeFace(FreetypeLib& library, const char *fontfile);

};


class HarfbuzzBlob {
public:
	hb_blob_t *blob;

	HarfbuzzBlob(const char *fontfile);

	~HarfbuzzBlob();
};

class HarfbuzzFace {
public:
	hb_face_t *face;

	HarfbuzzFace(hb_blob_t *blob);

	~HarfbuzzFace();
};

class HarfbuzzFont {
public:
	hb_font_t *font;

	HarfbuzzFont(hb_face_t *face);

	~HarfbuzzFont();
};

class HarfbuzzText {
	hb_buffer_t *buf = 0;
	unsigned int glyph_count = 0;
	hb_glyph_info_t *glyph_info = 0;
	hb_glyph_position_t *glyph_pos = 0;

public:
	HarfbuzzText(const char *text, HarfbuzzFont& font);
	~HarfbuzzText();

	void print();

	unsigned int getGlyphCount() const;

	hb_codepoint_t getGlyph(unsigned int i);
	hb_position_t getAdvanceX(unsigned int i);
	hb_position_t getAdvanceY(unsigned int i);
};

void drawText(HarfbuzzText& hb, FreetypeFace& face, int target_height, Image& image);

}


#endif // FREEFONT_H
