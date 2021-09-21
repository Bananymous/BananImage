#pragma once

#include "Image.h"

namespace Banan
{

	bool LoadBMP(std::string_view path, Image& out);
	bool SaveBMP(std::string_view path, const Image& image);

}