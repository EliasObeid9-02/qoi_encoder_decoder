#ifndef QOI_IMAGE_HPP
#define QOI_IMAGE_HPP

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
constexpr static uint32_t				s_headerSize{14};
constexpr static uint32_t				s_endMarkerSize{8};
constexpr static std::array<uint8_t, 8> s_endBytes{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};

namespace Tag
{
	constexpr uint8_t rgb	= 0b11111110u;
	constexpr uint8_t rgba	= 0b11111111u;
	constexpr uint8_t index = 0b00000000u;
	constexpr uint8_t diff	= 0b01000000u;
	constexpr uint8_t luma	= 0b10000000u;
	constexpr uint8_t run	= 0b11000000u;
	constexpr uint8_t mask	= 0b11000000u;
};	  // namespace Tag

struct Header
{
	Header();

	std::array<uint8_t, 4> m_qoif{};		  // the first 4 bytes of each .qoi file
	uint32_t			   m_height{};		  // the height of the image
	uint32_t			   m_width{};		  // the width of the image
	uint8_t				   m_channels{};	  // the number of channels used (3 for RGB, 4 for RGBA)
	uint8_t				   m_colorSpace{};	  // the colorspace of the image (0 for sRGB with
											  // linear alpha, 1 for all channels linear)
};

class RGBA
{
  public:
	using Channel = uint8_t;

	constexpr static uint32_t s_storedPixelsCount{64};

	Channel m_red{};
	Channel m_green{};
	Channel m_blue{};
	Channel m_alpha{};

	RGBA(Channel red = 0, Channel green = 0, Channel blue = 0, Channel alpha = 0);

	void	 invertPixel();			  // inverts the RGB values in the pixels
	uint32_t indexHashing() const;	  // hashes the RGBA values

	friend bool operator==(const RGBA& a, const RGBA& b)
	{
		return (a.m_red == b.m_red && a.m_green == b.m_green && a.m_blue == b.m_blue &&
				a.m_alpha == b.m_alpha);
	}
};

class Image
{
  public:
	Image(const std::string& imageFilePath);

	// reads the image file
	std::vector<uint8_t> readImage(const std::string& imageFilePath);

	// takes the path of the image file that will
	// be written into (without the extension)
	void writeImage(const std::string& imagePath);

	// flips the image in the current object
	void flipImage();

	// inverts the RGB values in every pixel
	void invertImage();

	// rotates the image to the left (times % 4) times
	void rotateImageLeft(int32_t times);

  private:
	Header			  m_imageHeader{};
	std::vector<RGBA> m_pixels{};

	// encodes the image information using
	// qoi-specification.pdf to create the image file
	std::vector<uint8_t> encodeImage();

	// decodes the image file information
	// using qoi-specification.pdf
	void decodeImage(const std::vector<uint8_t>& bytes);

	// reads the first 14 bytes associated
	// with the qoi image files header
	void readHeader(const std::vector<uint8_t>& bytes);

	// reads 4 bytes and combines them into a single 32-bit integer
	uint32_t read4Bytes(const std::vector<uint8_t>& bytes, size_t position);
};

}	 // namespace QOI

#endif