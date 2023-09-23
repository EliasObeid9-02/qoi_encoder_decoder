# QOI Encoder Decoder
  A simple C++ based code to handle encoding/decoding QOI format images. It also allows the user to do some modifications
to the source image such as inverting the RGB values, flipping the image and rotating the image 90 degrees to the left
some number of times.

# Usage
##### Installation
1. Run git clone ```https://github.com/EliasObeid9-02/qoi_encoder_decoder``` to clone this repository.
2. Run ```cmake -S src/ -B build/``` in the repository directory.
3. Go into the build direcotory and run ```make install```.

##### Un-installation
	1. Go to your home directory.
	2. Go into the ```bin``` directory.
	3. Run ```rm qoi_encoder_decoder``` in the ```bin``` directory to uninstall the program.

##### How to Use
	qoi_encoder_decoder dice.qoi
    
Optional Arguments:
1. `-o` to specify the name of the output file (don't include file extension).
2. `-i` to invert the colors of the image.
3. `-f` to flip the image.
4. `-r COUNT` to specify how many time you want to rotate the image to the left.