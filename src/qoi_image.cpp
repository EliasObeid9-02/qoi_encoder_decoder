#include "qoi_image.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <vector>

namespace QOI
{
RGBA::RGBA(Channel red, Channel green, Channel blue, Channel alpha)
	: m_red{red}, m_green{green}, m_blue{blue}, m_alpha{alpha}
{
}

void RGBA::invertPixel()
{
	m_red	= 255 - m_red;
	m_green = 255 - m_green;
	m_blue	= 255 - m_blue;
}

uint32_t RGBA::indexHashing() const
{
	return (m_red * 3 + m_green * 5 + m_blue * 7 + m_alpha * 11) % s_storedPixelsCount;
}

Header::Header()
{
	m_qoif.fill(0);
	m_height	 = 0;
	m_width		 = 0;
	m_channels	 = 0;
	m_colorSpace = 0;
}

Image::Image(const std::string& imageFilePath)
{
	try
	{
		decodeImage(readImage(imageFilePath));
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

void Image::readHeader(const std::vector<uint8_t>& bytes)
{
	// the header is made up of 14 bytes:
	// the first four bytes contain the letters "qoif"
	for (size_t position = 0; position < 4; ++position)
	{
		m_imageHeader.m_qoif[position] = bytes[position];
	}

	// the second four bytes contain the height of the image
	m_imageHeader.m_height = read4Bytes(bytes, 4);

	// the third four bytes contain the width of the image
	m_imageHeader.m_width = read4Bytes(bytes, 8);

	// the last two bytes contain the number of channels and colorspace respectivly
	m_imageHeader.m_channels   = bytes[12];
	m_imageHeader.m_colorSpace = bytes[13];
}

// Combines 4 bytes into a single 32 bit unsigned integer
uint32_t Image::read4Bytes(const std::vector<uint8_t>& bytes, size_t position)
{
	// to combine the 4 bytes we take:
	//     the first  byte and shifting it to the left by 24
	//   , the second byte and shifting it to the left by 16
	//   , the third  byte and shifting it to the left by  8
	// and the fourth byte as it is
	// and taking the bitwise OR of all of these values
	uint32_t data{};
	for (int32_t i = 0; i < 4; ++i)
	{
		int32_t byte{bytes[position + i]};
		data |= (byte << ((3 - i) * 8));
	}
	return data;
}

std::vector<uint8_t> Image::encodeImage()
{
	size_t pixelCount{m_imageHeader.m_height * m_imageHeader.m_width};
	size_t size{pixelCount * (m_imageHeader.m_channels + 1)};
	size_t qoiSize{size + s_headerSize + s_endMarkerSize};

	std::vector<uint8_t> bytes(qoiSize);

	// encoding the header
	bytes[0] = m_imageHeader.m_qoif[0];
	bytes[1] = m_imageHeader.m_qoif[1];
	bytes[2] = m_imageHeader.m_qoif[2];
	bytes[3] = m_imageHeader.m_qoif[3];

	bytes[4] = static_cast<uint8_t>(m_imageHeader.m_height >> 24 & 0b11111111u);
	bytes[5] = static_cast<uint8_t>(m_imageHeader.m_height >> 16 & 0b11111111u);
	bytes[6] = static_cast<uint8_t>(m_imageHeader.m_height >> 8 & 0b11111111u);
	bytes[7] = static_cast<uint8_t>(m_imageHeader.m_height >> 0 & 0b11111111u);

	bytes[8]  = static_cast<uint8_t>(m_imageHeader.m_width >> 24 & 0b11111111u);
	bytes[9]  = static_cast<uint8_t>(m_imageHeader.m_width >> 16 & 0b11111111u);
	bytes[10] = static_cast<uint8_t>(m_imageHeader.m_width >> 8 & 0b11111111u);
	bytes[11] = static_cast<uint8_t>(m_imageHeader.m_width >> 0 & 0b11111111u);

	bytes[12] = m_imageHeader.m_channels;
	bytes[13] = m_imageHeader.m_colorSpace;

	std::array<RGBA, RGBA::s_storedPixelsCount> previousPixels{};
	RGBA										lastPixel{0, 0, 0, 255};
	RGBA										currentPixel{0, 0, 0, 255};

	uint8_t run{0};
	size_t	index{s_headerSize};
	for (size_t pixelIndex{0}; pixelIndex < pixelCount; ++pixelIndex)
	{
		currentPixel = m_pixels[pixelIndex];
		if (currentPixel == lastPixel)
		{
			++run;
			if (run == 62 || pixelIndex == pixelCount - 1)
			{
				bytes[index] = (Tag::run | (run - 1));
				++index;
				run = 0;
			}
		}
		else
		{
			if (run > 0)
			{
				bytes[index] = (Tag::run | (run - 1));
				++index;
				run = 0;
			}

			if (previousPixels[currentPixel.indexHashing()] == currentPixel)
			{
				bytes[index] = (Tag::index | (currentPixel.indexHashing()));
				++index;
			}
			else if (currentPixel.m_alpha != lastPixel.m_alpha)
			{
				bytes[index] = Tag::rgba;
				++index;

				bytes[index] = currentPixel.m_red;
				++index;

				bytes[index] = currentPixel.m_green;
				++index;

				bytes[index] = currentPixel.m_blue;
				++index;

				bytes[index] = currentPixel.m_alpha;
				++index;
			}
			else
			{
				int8_t diffRed{static_cast<int8_t>(currentPixel.m_red - lastPixel.m_red)};
				int8_t diffGreen{static_cast<int8_t>(currentPixel.m_green - lastPixel.m_green)};
				int8_t diffBlue{static_cast<int8_t>(currentPixel.m_blue - lastPixel.m_blue)};

				if (-2 <= diffRed && diffRed <= 1 && -2 <= diffGreen && diffGreen <= 1 &&
					-2 <= diffBlue && diffBlue <= 1)
				{
					bytes[index] = (Tag::diff | ((diffRed + 2) << 4) | ((diffGreen + 2) << 2) |
									((diffBlue + 2) << 0));
					++index;
				}
				else if (-32 <= diffGreen && diffGreen <= 31 && -8 <= diffRed - diffGreen &&
						 diffRed - diffGreen <= 7 && -8 <= diffBlue - diffGreen &&
						 diffBlue - diffGreen <= 7)
				{
					bytes[index] = (Tag::luma | ((diffGreen + 32) << 0));
					++index;

					bytes[index] =
						(((diffRed - diffGreen + 8) << 4) | ((diffBlue - diffGreen + 8) << 0));
					++index;
				}
				else
				{
					bytes[index] = Tag::rgb;
					++index;

					bytes[index] = currentPixel.m_red;
					++index;

					bytes[index] = currentPixel.m_green;
					++index;

					bytes[index] = currentPixel.m_blue;
					++index;
				}
			}
		}
		previousPixels[currentPixel.indexHashing()] = currentPixel;
		lastPixel									= currentPixel;
	}

	for (size_t i = 0; i < s_endMarkerSize; ++i)
	{
		bytes[index] = s_endBytes[i];
		++index;
	}
	bytes.resize(index);
	bytes.shrink_to_fit();
	return bytes;
}

void Image::decodeImage(const std::vector<uint8_t>& bytes)
{
	readHeader(bytes);
	size_t pixelCount{m_imageHeader.m_height * m_imageHeader.m_width};
	m_pixels.resize(pixelCount);

	std::array<RGBA, RGBA::s_storedPixelsCount> previousPixels{};
	RGBA										currentPixel{0, 0, 0, 255};
	for (size_t streamIndex{s_headerSize}, pixelIndex{0}; pixelIndex < pixelCount; ++pixelIndex)
	{
		if (streamIndex < (bytes.size() - s_endMarkerSize))
		{
			if (bytes[streamIndex] == Tag::rgb)
			{
				uint8_t red{bytes[streamIndex + 1]};
				uint8_t green{bytes[streamIndex + 2]};
				uint8_t blue{bytes[streamIndex + 3]};
				currentPixel = RGBA(red, green, blue, currentPixel.m_alpha);
				streamIndex += 3;
			}
			else if (bytes[streamIndex] == Tag::rgba)
			{
				uint8_t red{bytes[streamIndex + 1]};
				uint8_t green{bytes[streamIndex + 2]};
				uint8_t blue{bytes[streamIndex + 3]};
				uint8_t alpha{bytes[streamIndex + 4]};
				currentPixel = RGBA(red, green, blue, alpha);
				streamIndex += 4;
			}
			else
			{
				if ((Tag::mask & bytes[streamIndex]) == Tag::index)
				{
					uint8_t index{static_cast<uint8_t>(0b00111111 & bytes[streamIndex])};
					currentPixel = previousPixels[index];
				}
				else if ((Tag::mask & bytes[streamIndex]) == Tag::diff)
				{
					uint8_t diffRed{
						static_cast<uint8_t>(((0b00110000 & bytes[streamIndex]) >> 4) - 2)};
					currentPixel.m_red += diffRed;

					uint8_t diffGreen{
						static_cast<uint8_t>(((0b00001100 & bytes[streamIndex]) >> 2) - 2)};
					currentPixel.m_green += diffGreen;

					uint8_t diffBlue{
						static_cast<uint8_t>(((0b00000011 & bytes[streamIndex]) >> 0) - 2)};
					currentPixel.m_blue += diffBlue;
				}
				else if ((Tag::mask & bytes[streamIndex]) == Tag::luma)
				{
					uint8_t diffGreen{
						static_cast<uint8_t>(((0b00111111 & bytes[streamIndex]) >> 0) - 32)};
					currentPixel.m_green += diffGreen;
					++streamIndex;

					uint8_t diffRed{static_cast<uint8_t>(((0b11110000 & bytes[streamIndex]) >> 4) -
														 8 + diffGreen)};
					currentPixel.m_red += diffRed;

					uint8_t diffBlue{static_cast<uint8_t>(((0b00001111 & bytes[streamIndex]) >> 0) -
														  8 + diffGreen)};
					currentPixel.m_blue += diffBlue;
				}
				else if ((Tag::mask & bytes[streamIndex]) == Tag::run)
				{
					uint8_t length{static_cast<uint8_t>((0b00111111 & bytes[streamIndex]) >> 0)};
					for (size_t i = 0; i < length; ++i)
					{
						m_pixels[pixelIndex] = currentPixel;
						++pixelIndex;
					}
				}
				else
				{
					throw std::invalid_argument("invalid image file encoding!\n");
				}
			}
			previousPixels[currentPixel.indexHashing()] = currentPixel;
			++streamIndex;
		}
		m_pixels[pixelIndex] = currentPixel;
	}
}

std::vector<uint8_t> Image::readImage(const std::string& imageFilePath)
{
	if (imageFilePath.substr(imageFilePath.size() - 4) != ".qoi")
	{
		throw std::invalid_argument("image file given is not qoi!");
	}

	std::vector<uint8_t> bytes;
	std::fstream		 imageFile{imageFilePath,
						   std::fstream::in | std::fstream::binary | std::fstream::ate};
	if (!imageFile.is_open())
	{
		throw std::ios_base::failure("image file opening failed!");
	}

	std::streamsize size{imageFile.tellg()};
	imageFile.seekg(imageFile.beg);

	bytes.resize(size);
	if (!imageFile.read(reinterpret_cast<char*>(bytes.data()), size))
	{
		throw std::ios_base::failure("image file reading failed!");
	}
	imageFile.close();
	return bytes;
}

void Image::writeImage(const std::string& imagePath)
{
	std::vector<uint8_t> bytes{encodeImage()};
	if (imagePath.find('.') != imagePath.npos)
	{
		throw std::invalid_argument("name provided is a file!");
	}

	std::fstream imageFile(imagePath + ".qoi", std::fstream::out | std::fstream::binary);
	if (!imageFile.is_open())
	{
		throw std::ios_base::failure("image file opening failed!");
	}

	if (!imageFile.write(reinterpret_cast<const char*>(bytes.data()), bytes.size()))
	{
		throw std::ios_base::failure("image file writing failed!");
	}
	imageFile.close();
}

void Image::flipImage()
{
	const size_t	  size{m_pixels.size()};
	const uint32_t	  height{m_imageHeader.m_height};
	std::vector<RGBA> newPixels(size);
	for (size_t index{0}; index < size; ++index)
	{
		const size_t x{index % height};
		const size_t y{index / height};

		const size_t newX{height - 1 - x};
		const size_t newY{y};

		const size_t newIndex{newX + newY * height};
		newPixels[newIndex] = m_pixels[index];
	}
	m_pixels.swap(newPixels);
}

void Image::invertImage()
{
	for (size_t i = 0; i < m_pixels.size(); ++i)
	{
		m_pixels[i].invertPixel();
	}
}

void Image::rotateImageLeft(int32_t times)
{
	for (int32_t rotation{0}; rotation < times % 4; ++rotation)
	{
		const size_t	  size{m_pixels.size()};
		uint32_t&		  height{m_imageHeader.m_height};
		uint32_t&		  width{m_imageHeader.m_width};
		std::vector<RGBA> newPixels(size);
		for (size_t index{0}; index < size; ++index)
		{
			const size_t x{index % height};
			const size_t y{index / height};

			const size_t newX{y};
			const size_t newY{height - 1 - x};

			const size_t newPixelIndex{newX + newY * width};
			newPixels[newPixelIndex] = m_pixels[index];
		}
		std::swap(height, width);
		m_pixels.swap(newPixels);
	}
}

}	 // namespace QOI