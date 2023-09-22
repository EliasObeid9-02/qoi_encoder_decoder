#include "qoi_image.hpp"
#include "argparse.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <exception>
#include <format>

constexpr std::string_view help_message =
"Usage: {} [-o OUTFILE][-r COUNT][-i][-f] <INFILE>";


int32_t main(int32_t argc, const char* argv[])
{
	try {
		QOI::Config args{argc, argv};
	} catch (std::exception& exception) {
		std::cerr << std::format(help_message, argv[0]) << '\n';
		std::cerr << "Error: " <<  exception.what() << '\n';
		exit(EXIT_FAILURE);
	}
	QOI::Config args{argc, argv};
	QOI::Image	imageFile{std::string{args.inFilePath}};
	

	if (args.flip)
	{
		imageFile.flipImage();
	}

	if (args.invert)
	{
		imageFile.invertImage();
	}

	if (args.rotations > 0)
	{
		imageFile.rotateImageLeft(args.rotations);
	}

	imageFile.writeImage(std::string{args.outFilePath});
	std::cout << "Modified image has been written into \"" 
		<< args.outFilePath<< "\".\n";
	return 0; 
}
