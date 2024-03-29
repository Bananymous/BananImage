#include "Image.h"

#include "BMP.h"

#include <unordered_map>

#pragma warning(push)
#pragma warning(disable: 26451)

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
		m_data(new color[int64_t(width) * int64_t(height)])
	{ }

	Image::Image(std::string_view path, ImageFormat format)
	{
		Load(path, format);
	}

	Image::~Image()
	{
		delete[] m_data;
	}

	void Image::Resize(int32_t width, int32_t height)
	{
		delete[] m_data;
		m_data		= new color[width * height];
		m_width		= width;
		m_height	= height;
	}

	void Image::At(int32_t x, int32_t y, void* out) const
	{
		void* pos = m_data + y * m_width + x;
		std::memcpy(out, pos, 3 * sizeof(double));
	}

	void Image::Set(int32_t x, int32_t y, void* val)
	{
		void* pos = m_data + y * m_width + x;
		std::memcpy(pos, val, 3 * sizeof(double));
	}

	bool Image::Load(std::string_view path, ImageFormat format)
	{
		switch (format)
		{
			// Try to determine format of the file
			case ImageFormat::None:
				format = GetFormat(path);
				if (format == ImageFormat::None)
					return false;
				return Image::Load(path, format);

			// Call 'Load' function for specified image format
			case ImageFormat::BMP:
				return Banan::LoadBMP(path, *this);
		}

		return true;
	}

	bool Image::Save(std::string_view path, ImageFormat format) const
	{
		switch (format)
		{
			// Try to determine format of the file
			case ImageFormat::None:
				format = GetFormat(path);
				if (format == ImageFormat::None)
					return false;
				return Save(path, format);

			// Call 'Save' function for specified image format
			case ImageFormat::BMP:
				return Banan::SaveBMP(path, *this);
		}
	}

}

#pragma warning(pop)