# QOI Encoder Decoder
  A simple C++ based code to handle encoding/decoding QOI format images. It also allows the user to do some modifications
to the source image such as inverting the RGB values, flipping the image and rotating the image 90 degrees to the left
some number of times.

# How to use:
1. Run ```git clone https://github.com/EliasObeid9-02/qoi_encoder_decoder``` to clone this repository.
2. Run ```cmake -S ./src -B ./build``` in the repository directory.
3. Run ```./build/qoi_enocder_decoder``` in the repository directory.
4. Input the path of the .qoi image that you want to modify.
5. The resulting image will be saved in ```outputImage.qoi``` in the repository directory.
