#ifndef QOI_ARGPARSE
#define QOI_ARGPARSE

#include "argparse.hpp"

#include <cstdint>
#include <exception>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string_view>

namespace QOI
{
Config::Config(const int32_t argc, const char* argv[])
{
	if (argc < 2)
	{
		throw std::invalid_argument("Input file not specified.");	 // handle(send help message).
	}

	for (int32_t idx{1}; idx < argc; ++idx)
	{
		std::string_view arg{argv[idx]};
		if (arg == "-o")
		{
			if (idx + 1 >= argc)
			{
				throw std::invalid_argument("Output file not specified after -o.");
			}
			++idx;
			this->outFilePath = argv[idx];
		}
		else if (arg == "-f")
		{
			this->flip = true;
		}
		else if (arg == "-i")
		{
			this->invert = true;
		}
		else if (arg == "-r")
		{
			if (idx + 1 >= argc)
			{
				throw std::invalid_argument("Rotation count not specified after -r");
			}
			++idx;
			this->rotations = std::atoi(argv[idx]);	   // Should throw exception
													   // if not integeral.
		}
		else
		{
			if (inFilePath.empty())	   // specify input file if not specified.
			{
				inFilePath = arg;
			}
			else
			{
				throw std::invalid_argument(
					"Can only process one file at a time.");	// only one file.}
			}
		}
	}

	if (this->inFilePath.empty())
	{
		throw std::invalid_argument("input file not specified.");	 // only one file.
	}

	if (!std::filesystem::exists(this->inFilePath))
	{
		throw std::invalid_argument("input file does not exist.");	  // only one file.
	}
}
}	 // namespace QOI

#endif