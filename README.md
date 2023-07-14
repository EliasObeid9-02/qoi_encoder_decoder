# QOI Encoder Decoder
  A simple C++ based code to handle encoding/decoding QOI format images. It also allows the user to do some modifications
to the source image such as inverting the RGB values, flipping the image and rotating the image 90 degrees to the left
some number of times.

## Installation:
1. Run ```git clone https://github.com/EliasObeid9-02/qoi_encoder_decoder``` to clone this repository.
2. Run ```cmake -S ./src -B ./build``` in the repository directory.
3. 
4. Go into the build direcotory and run ```make install```.

## Un-installation:
1. Go to your home directory.
2. Go into the ```bin``` directory.
3. Run ```rm qoi_encoder_decoder``` in the ```bin``` directory to uninstall the program.

#### How to use:
1. Open a terminal.
2. Run ```qoi_encoder_decoder``` to launch the program.
3. Input the path of the .qoi image that you want to modify.
4. The resulting image will be saved in ```outputImage.qoi``` in the repository directory.
