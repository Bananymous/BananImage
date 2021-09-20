#pragma once

#include <string_view>

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
		~Image();

		bool Load(std::string_view path, ImageFormat format = ImageFormat::None);
		bool Save(std::string_view path, ImageFormat format = ImageFormat::None) const;

		void At(int32_t x, int32_t y, void* out) const;
		void Set(int32_t x, int32_t y, void* val);

		int32_t Width() const { return m_width; }
		int32_t Height() const { return m_height; }

	private:
		bool LoadBMP(std::string_view path);
		bool SaveBMP(std::string_view path) const;

	private:
		int32_t		m_width, m_height;
		double*		m_data;
	};

	static void OpenImage(std::string path)
	{
		path = "start " + path;
		system(path.c_str());
	}

}