#include "Image.h"

#include <unordered_map>

namespace Banan
{
	
	// File extensions
	static std::unordered_map<std::string_view, ImageFormat> formats({
		{ "bmp", ImageFormat::BMP }
	});

	// Parse file extension from path
	static ImageFormat GetFormat(std::string_view path)
	{
		std::string_view ext = path.substr(path.find_last_of('.'));
		auto it = formats.find(ext);
		if (it == formats.end())
			return ImageFormat::None;
		return it->second;
	}

	Image::Image() :
		m_width(0),
		m_height(0),
		m_data(nullptr)
	{ }

	Image::Image(int32_t width, int32_t height) :
		m_width(width),
		m_height(height),
		m_data(new double[3 * int64_t(width) * int64_t(height)])
	{ }

	Image::Image(std::string_view path, ImageFormat format)
	{
		Load(path, format);
	}

	Image::~Image()
	{
		delete[] m_data;
	}

	void Image::At(int32_t x, int32_t y, void* out) const
	{
		double* pos = m_data + 3 * (int64_t(y) * int64_t(m_width) + int64_t(x));
		((double*)out)[0] = pos[0];
		((double*)out)[1] = pos[1];
		((double*)out)[2] = pos[2];
	}

	void Image::Set(int32_t x, int32_t y, void* val)
	{
		double* pos = m_data + 3 * (int64_t(y) * int64_t(m_width) + int64_t(x));
		pos[0] = ((double*)val)[0];
		pos[1] = ((double*)val)[1];
		pos[2] = ((double*)val)[2];
	}

	bool Image::Load(std::string_view path, ImageFormat format)
	{
		switch (format)
		{
			// Try to determine format of the file
			case Banan::ImageFormat::None:
				format = GetFormat(path);
				if (format == ImageFormat::None)
					return false;
				return Image::Load(path, format);

			// Call 'Load' function for specified image format
			case Banan::ImageFormat::BMP:
				return LoadBMP(path);
		}

		return true;
	}

	bool Image::Save(std::string_view path, ImageFormat format) const
	{
		switch (format)
		{
			// Try to determine format of the file
			case Banan::ImageFormat::None:
				format = GetFormat(path);
				if (format == ImageFormat::None)
					return false;
				return Image::Save(path, format);

			// Call 'Save' function for specified image format
			case Banan::ImageFormat::BMP:
				return SaveBMP(path);
		}
	}

}