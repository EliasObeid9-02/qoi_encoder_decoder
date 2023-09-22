#ifndef QOI_ARGPARSE_HPP
#define QOI_ARGPARSE_HPP

#include <string_view>
#include <cstdint>
#include <exception>
#include <stdexcept>

namespace QOI 
{

class Config 
{
public:
	std::string_view inFilePath {}; // Should be a QOI file
	std::string_view outFilePath {"outputImage"}; // Output name [-o OUTPUT]
	bool flip {}; // Flip Image [-f]
	bool invert {}; // Invert Image [-i]
	int32_t rotations {}; // Rotations to the left [-r COUNT]
	Config(const int32_t argc, const char* argv[]);
};

} // namespace QOI
#endif
