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

class FreetypeFace {
public:
	FT_Face face = 0;
	uint32_t lineheight;
	uint32_t descender;
	FT_GlyphSlot slot;

	FreetypeFace(FreetypeLib& library, const char *fontfile, FT_UInt ftpx);
	~FreetypeFace();

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

class TextDims {
public:
	long startX, startY;
	long sizeX, sizeY;

	TextDims(long startX_, long startY_,
		   long sizeX_, long sizeY_) :
		startX(startX_), startY(startY_),
		sizeX(sizeX_), sizeY(sizeY_)
	{}

	TextDims operator += (const TextDims& dims)
	{
		long minX = std::min(startX, dims.startX);
		long minY = std::min(startY, dims.startY);

		long maxX = std::max(startX + sizeX, dims.startX + dims.sizeX);
		long maxY = std::max(startY + sizeY, dims.startY + dims.sizeY);

		startX = minX;
		startX = minY;
		sizeX = maxX - minX;
		sizeY = maxY - minY;

		return *this;
	}

	~TextDims(){}
};

TextDims drawText(HarfbuzzText& hb, FreetypeFace& face, unsigned int line, Image& image);

class Pen{
	unsigned int line = 0;
	Image& image;
	FreetypeFace& freetypeFace;
	HarfbuzzFont& harfbuzzFont;

public:

	Pen(Image& image_,
		FreetypeFace& freetypeFace_,
		HarfbuzzFont& harfbuzzFont_)
	:
		image(image_),
		freetypeFace(freetypeFace_),
		harfbuzzFont(harfbuzzFont_)
	{}

	TextDims draw(const char *text){
		HarfbuzzText harfbuzzText(text, harfbuzzFont);
		TextDims ret = drawText(harfbuzzText, freetypeFace, line, image);
		line++;
		return ret;
	}
};



}


#endif // FREEFONT_H
