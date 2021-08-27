#include "Image.h"

namespace BR {

Image::Image(uint16_t width_, uint16_t height_) : width(width_), height(height_){
	data.resize(width * height * 4);
}

} // namespace BR
