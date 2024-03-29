#include "BMP.h"

#include <algorithm>
#include <fstream>

/*
* 
* Used this wikipedia article for implementation
* https://en.wikipedia.org/wiki/BMP_file_format
*
*/

#pragma warning(push)
#pragma warning(disable: 6385 26451)

namespace Banan
{

#pragma pack(push, 2)
	struct BMP_HEADER
	{
		uint16_t	indentifier;
		uint32_t	size;
		uint16_t	reserved1;
		uint16_t	reserved2;
		uint32_t	pixel_array;
	};
	struct DIB_HEADER_12
	{
		int32_t		size;
		uint16_t	width;
		uint16_t	height;
		int16_t		color_planes;
		int16_t		bits_per_pixel;
	};
	struct DIB_HEADER_40
	{
		int32_t size;
		int32_t width;
		int32_t height;
		int16_t color_planes;
		int16_t bits_per_pixel;
		int32_t compression;
		int32_t raw_bitmap_size;
		int32_t horizontal_res;
		int32_t vertical_res;
		int32_t color_count;
		int32_t important_colors;
	};
#pragma pack(pop)

	bool LoadBMP(std::string_view path, Image& image)
	{
		std::ifstream file(path.data(), std::ios::binary | std::ios::ate);

		if (file.fail())
			return false;

		uint32_t file_size = uint32_t(file.tellg());
		file.seekg(std::ios::beg);

		// Read BMP header
		BMP_HEADER bmp;
		file.read((char*)&bmp, sizeof(BMP_HEADER));

		// Confirm that the header field is correct
		if (bmp.indentifier != 0x4D42)
			return false;

		// Confirm that file is atleast as large as needed
		if (bmp.size > file_size)
			return false;

		// Read DIB header
		int32_t header_size;
		file.read((char*)&header_size, 4);
		file.seekg(-4, std::ios::cur);

		int16_t bits_per_pixel;

		switch (header_size)
		{
			// BITMAPCOREHEADER/OS21BITMAPHEADER
			case 12:
			{
				// Read DIB header
				DIB_HEADER_12 dib;
				file.read((char*)&dib, sizeof(DIB_HEADER_12));

				// Assing needed variables
				image.Resize(dib.width, dib.height);
				bits_per_pixel = dib.bits_per_pixel;

				break;
			}

			// BITMAPINFOHEADER
			case 40:
			{
				// Read DIB header
				DIB_HEADER_40 dib;
				file.read((char*)&dib, sizeof(DIB_HEADER_40));

				// Check for compression (not supported)
				if (dib.compression != 0)
					return false;

				// Assign needed variables
				image.Resize(dib.width, dib.height);
				bits_per_pixel = dib.bits_per_pixel;

				break;
			}

			// Unknown DIP Header
			default:
				return false;
		}

		// Handle only 32 and 24 bits per pixel
		if (bits_per_pixel != 32 && bits_per_pixel != 24)
			return false;


		// Confirm that we are at the beginning of pixel array
		file.seekg(bmp.pixel_array, std::ios::beg);

		// Calculate start, end and step for top-down or down-top images
		int32_t start = (image.Height() > 0) ? 0 : -image.Height() - 1;
		int32_t end = (image.Height() > 0) ? image.Height() : -1;
		int32_t step = (image.Height() > 0) ? 1 : -1;

		// Calculate length of row in bytes
		int32_t row_size = int32_t(std::ceil((bits_per_pixel * image.Width()) / 32)) * 4;

		// Setup pixelarray
		double scale = 1.0 / 255.0;
		uint32_t image_index = 0;

		// Loop for pixelarray's y values
		for (int32_t y = start; y != end; y += step)
		{
			// Read full row of pixels
			uint8_t* row = new uint8_t[row_size];
			file.read((char*)row, row_size);

			uint32_t byte_index = 0;
			for (int32_t x = 0; x < image.Width(); x++)
			{
				// Read RGB values from file
				uint8_t b = row[byte_index++];
				uint8_t g = row[byte_index++];
				uint8_t r = row[byte_index++];
				if (bits_per_pixel == 32)
					byte_index += 1;


				// Initialize data in pixelarray
				double color[3] {
					double(r) * scale,
					double(g) * scale,
					double(b) * scale
				};
				image.Set(x, y, color);

				image_index++;
			}

			// Delete the row
			delete[] row;
		}

		file.close();
		return true;
	}

	bool SaveBMP(std::string_view path, const Image& image)
	{
		// Open a filestream
		std::ofstream file(path.data(), std::ios::binary);

		// Confirm that filestream opened successfully
		if (file.fail())
			return false;

		// Create Bitmap file header
		BMP_HEADER bmp;
		bmp.indentifier = 0x4D42;
		bmp.size = image.Width() * image.Height() * 3 + sizeof(BMP_HEADER) + sizeof(DIB_HEADER_12);
		bmp.reserved1 = 0;
		bmp.reserved2 = 0;
		bmp.pixel_array = sizeof(BMP_HEADER) + sizeof(DIB_HEADER_12);
		file.write((char*)&bmp, sizeof(BMP_HEADER));

		// Create DIB header
		DIB_HEADER_12 dib;
		dib.size = sizeof(DIB_HEADER_12);
		dib.width = image.Width();
		dib.height = image.Height();
		dib.color_planes = 1;
		dib.bits_per_pixel = 24;
		file.write((char*)&dib, sizeof(DIB_HEADER_12));

		// Calculate padding size
		uint32_t padding = (4 - (image.Width() * 3) % 4) % 4;

		// Create pixel array
		for (int32_t j = 0; j < image.Height(); j++)
		{
			for (int32_t i = 0; i < image.Width(); i++)
			{
				// Write and transform colors from [0, 1] to [0, 255] (1 byte each)
				double color[3];
				image.At(i, j, color);

				uint8_t buffer[3];
				buffer[0] = uint8_t(256.0 * std::clamp(color[2], 0.0, 0.999));
				buffer[1] = uint8_t(256.0 * std::clamp(color[1], 0.0, 0.999));
				buffer[2] = uint8_t(256.0 * std::clamp(color[0], 0.0, 0.999));

				file.write((char*)buffer, 3);
			}

			// Write padding after each row of pixels
			file.write("\0\0\0\0", padding);
		}

		// Close the filestream
		file.close();

		return true;
	}
	
}

#pragma warning(pop)