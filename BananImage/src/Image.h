#pragma once

#include <string_view>
#include "vec.h"


namespace Banan
{

	enum class ImageFormat
	{
		None,
		BMP
	};

	class Image
	{
	public:
		Image();
		Image(int32_t width, int32_t height);
		Image(std::string_view path, ImageFormat format = ImageFormat::None);
		//~Image();

		bool Load(std::string_view path, ImageFormat format = ImageFormat::None);
		bool Save(std::string_view path, ImageFormat format = ImageFormat::None);

		vec3d&			At(int32_t x, int32_t y);
		const vec3d&	At(int32_t x, int32_t y) const;

		int32_t Width() const { return m_width; }
		int32_t Height() const { return m_height; }

	private:
		bool LoadBMP(std::string_view path);
		bool SaveBMP(std::string_view path) const;

	private:
		int32_t		m_width, m_height;
		vec3d*		m_data;
	};

	static void OpenImage(std::string path)
	{
		path = "start " + path;
		system(path.c_str());
	}

}