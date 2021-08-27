
#include "FreeFont.h"
#include "../exception.h"

namespace BR {

FontconfigLib::FontconfigLib() {
	config = FcInitLoadConfigAndFonts();
	if(!config){
		throw Exception("Loading Fontconfig failed failed");
	}

	FcConfigSetRescanInterval(config, 0);
}

FontconfigLib::~FontconfigLib(){
	FcConfigDestroy(config);
	FcFini();
}

class FontconfigPattern {
public:
	FcPattern *pat;

	FontconfigPattern(){
		pat = FcPatternCreate();
		if(!pat){
			throw Exception("Loading Fontconfig failed failed");
		}
	}

	~FontconfigPattern(){
		FcPatternDestroy(pat);
	}
};

class FontconfigOS {
public:
	FcObjectSet *os;

	FontconfigOS(){
		os = FcObjectSetBuild (FC_FAMILY, FC_STYLE, FC_LANG, FC_FILE, (char *) 0);
	}

	~FontconfigOS(){
		FcObjectSetDestroy (os);
	}
};

class Fontname {
public:
	FcChar8 *s;

	Fontname(FcPattern *pattern){
		s = FcNameUnparse(pattern);
		if(!s){
			throw Exception("FcNameUnparse font failed");
		}
	}

	~Fontname(){
		free(s);
	}

};

FontList::FontList(FontconfigLib& fontconfigLib){
	FontconfigPattern pat;
	FontconfigOS os;

	fs = FcFontList(fontconfigLib.config, pat.pat, os.os);

	if(!fs){
		throw Exception("Obtaining fontlist failed");
	}
}

void FontList::print(){
	printf("Total fonts: %d\n", fs->nfont);
	for (int i=0; i < fs->nfont; i++) {
		FcPattern *font = fs->fonts[i];

		FcChar8 *style;
		FcPatternGetString(font, FC_STYLE, 0, &style);

		if(strcmp((const char *)style, "Regular") != 0){
			continue;
		}

		FcLangSet *lang = 0;
		FcPatternGetLangSet(font, FC_LANG, 0, &lang);

		if(FcLangSetHasLang (lang, (FcChar8 *)"en") == FcLangEqual &&
				FcLangSetHasLang (lang, (FcChar8 *)"ja") == FcLangEqual){

			Fontname fontname (font);
			printf("Font %s\n", fontname.s);
		}
	}
}

std::string FontList::getFirst(){
	for (int i=0; i < fs->nfont; i++) {
		FcPattern *font = fs->fonts[i];

		FcChar8 *style;
		FcPatternGetString(font, FC_STYLE, 0, &style);

		if(strcmp((const char *)style, "Regular") != 0){
			continue;
		}

		{
			FcLangSet *lang = 0;
			FcPatternGetLangSet(font, FC_LANG, 0, &lang);

			if(FcLangSetHasLang (lang, (FcChar8 *)"en") == FcLangEqual &&
					FcLangSetHasLang (lang, (FcChar8 *)"ja") == FcLangEqual){

				FcChar8 *file = 0;
				FcPatternGetString(font, FC_FILE, 0, &file);

				if(file){
					Fontname fontname (font);
					printf("Font %s\n", fontname.s);

					return (const char *)file;
				}
			}
		}
	}

	return "";
}

FontList::~FontList(){
	FcFontSetDestroy(fs);
}

FreetypeLib::FreetypeLib(){
	FT_Init_FreeType(&library);

	if(!library){
		throw Exception("Init freetype failed");
	}
}

FreetypeLib::~FreetypeLib(){
	FT_Done_FreeType(library);
}

FreetypeFace::FreetypeFace(FreetypeLib &library, const char *fontfile){
	FT_New_Face(library.library, fontfile, 0, &face);
	if(!face){
		throw Exception("No face");
	}

	//FT_Set_Pixel_Sizes(face, 64, 64);
	FT_Set_Char_Size(face, 192, 192, 1024, 1024);
	slot = face->glyph;
}

HarfbuzzText::HarfbuzzText(const char *text, HarfbuzzFont& font){

	buf = hb_buffer_create();
	hb_buffer_add_utf8(buf, text, -1, 0, -1);

	hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
	hb_buffer_set_script(buf, HB_SCRIPT_LATIN);
	hb_buffer_set_language(buf, hb_language_from_string("en", -1));

	hb_shape(font.font, buf, NULL, 0);

	glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
	glyph_pos  = hb_buffer_get_glyph_positions(buf, &glyph_count);
}

void HarfbuzzText::print(){
	hb_position_t cursor_x = 0;
	hb_position_t cursor_y = 0;
	for (unsigned int i = 0; i < glyph_count; i++) {
		//hb_position_t x_offset  = glyph_pos[i].x_offset;
		//hb_position_t y_offset  = glyph_pos[i].y_offset;
		hb_position_t x_advance = glyph_pos[i].x_advance;
		hb_position_t y_advance = glyph_pos[i].y_advance;

		cursor_x += x_advance;
		cursor_y += y_advance;

		printf("i %u | gid: %u, x_advance %u, y_advance %u\n", i, glyph_info[i].codepoint, x_advance, y_advance);
	}
}

unsigned int HarfbuzzText::getGlyphCount() const{
	return glyph_count;
}

hb_codepoint_t HarfbuzzText::getGlyph(unsigned int i){
	return glyph_info[i].codepoint;
}

hb_position_t HarfbuzzText::getAdvanceX(unsigned int i){
	return glyph_pos[i].x_advance;
}

hb_position_t HarfbuzzText::getAdvanceY(unsigned int i){
	return glyph_pos[i].y_advance;
}

HarfbuzzText::~HarfbuzzText(){
	hb_buffer_destroy(buf);
}

void
draw_bitmap(FT_Bitmap* bitmap, Image& image, FT_Int bitmap_left, FT_Int bitmap_top,
	FT_Int line_offset)
{
	unsigned char *data = image.data.data();
	printf("FT Rows     : %d, width: %d\n", bitmap->rows, bitmap->width);
	printf("Image height: %d, width: %d\n", image.width,  image.height);
	printf("top height: %d\n", bitmap_top);

	for(unsigned int y = 0; y < bitmap->rows; y++){
		for(unsigned int x = 0; x < bitmap->width; x++){
			size_t idx = (line_offset + (y - bitmap_top)) *
				image.width * 4 + (x + bitmap_left) * 4;

			if(idx >= image.data.size()) continue;

			uint8_t px = bitmap->buffer[bitmap->width * y + x];

			data[idx + 0] |= px;
			data[idx + 1] |= px;
			data[idx + 2] |= px;
			data[idx + 3] = 255;
		}
	}
}

void drawText(HarfbuzzText& hb, FreetypeFace& face, int target_height, Image& image){
	FT_Vector pen = {0, 0};

	for(unsigned int n = 0;n < hb.getGlyphCount();n++){
		FT_Set_Transform( face.face, 0, &pen );

		FT_Error error = FT_Load_Glyph( face.face, hb.getGlyph(n), FT_LOAD_RENDER );
		if ( error ){
			printf("Load glyph failed for %d\n", hb.getGlyph(n));
			continue;
		}

		FT_GlyphSlot& slot = face.slot;

		draw_bitmap( &slot->bitmap, image, slot->bitmap_left, slot->bitmap_top, target_height);

		pen.x += slot->advance.x;
		pen.y += slot->advance.y;
	}
}

HarfbuzzBlob::HarfbuzzBlob(const char *fontfile){
	blob = hb_blob_create_from_file(fontfile);

	if(!blob){
		throw Exception("No blob");
	}
}

HarfbuzzBlob::~HarfbuzzBlob(){
	hb_blob_destroy(blob);
}

HarfbuzzFace::HarfbuzzFace(hb_blob_t *blob){
	face = hb_face_create(blob, 0);

	if(!face){
		throw Exception("No face");
	}
}

HarfbuzzFace::~HarfbuzzFace(){
	hb_face_destroy(face);
}

HarfbuzzFont::HarfbuzzFont(hb_face_t *face){
	font = hb_font_create(face);

	if(!font){
		throw Exception("No font");
	}
}

HarfbuzzFont::~HarfbuzzFont(){
	hb_font_destroy(font);
}


}
