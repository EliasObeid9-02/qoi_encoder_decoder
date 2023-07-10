#include "qoi_image.hpp"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>

std::string getFilePath()
{
	std::string filePath{};
	while (true)
	{
		std::cout << "Specify image file path (must be an absolute path): ";
		std::getline(std::cin, filePath);

		size_t size{filePath.size()};
		if (size < 5 || std::count(filePath.begin(), filePath.end(), '.') > 1 ||
			filePath.substr(size - 4) != ".qoi")
		{
			std::cerr << "Invalid image file path!\n";
			continue;
		}
		return filePath;
	}
}

void getResponse(std::string_view message, std::string& response)
{
	while (true)
	{
		std::cout << message;
		std::getline(std::cin, response);
		size_t size{response.size()};
		if (size != 1)
		{
			std::cerr << "Invalid response! Type y for Yes and n for No.\n";
			continue;
		}
		response[0] = tolower(response[0]);
		break;
	}
}

void getResponse(std::string_view message, int32_t& response)
{
	while (true)
	{
		std::cout << message;
		std::string input{};
		std::getline(std::cin, input);
		if (!std::all_of(input.begin(), input.end(), [&](char c) { return isdigit(c); }) ||
			input.front() == '0')
		{
			std::cerr << "Invalid response! Please enter a positive integer.\n";

			continue;
		}
		response = std::stoi(input);
		break;
	}
}

int32_t main()
{
	std::string response{};
	std::string filePath{getFilePath()};
	QOI::Image	imageFile{filePath};

	getResponse("Do you want to flip the image? (y/n) ", response);
	if (response == "y")
	{
		imageFile.flipImage();
	}

	getResponse("Do you want to invert the image? (y/n) ", response);
	if (response == "y")
	{
		imageFile.invertImage();
	}

	getResponse("Do you want to rotate the image to the left? (y/n) ", response);
	if (response == "y")
	{
		int32_t times{};
		getResponse("How many times? ", times);
		imageFile.rotateImageLeft(times);
	}
	imageFile.writeImage("outputImage");
	std::cout << "Modified image has been written into \"outputImage.qoi\".\n";
	return 0;
}