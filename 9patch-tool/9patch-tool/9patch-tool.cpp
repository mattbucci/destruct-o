// 9patch-tool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "NinePatchBinary.h"


int main(int argc, char* argv[])
{
	if (argc < 1) {
		cout << "You must specify the name of an image to 9patch\n";
		return -1;
	}
	std::vector<unsigned char> image; //the raw pixels
	unsigned width, height;

	string fname = argv[1];

	//decode
	unsigned error = lodepng::decode(image, width, height, fname);

	//if there's an error, display it
	if(error) { 
		std::cout << "lode pngdecoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	return -2;
	}


	//Find the limits along the top
	int firstBlackX = -1;
	int firstBlackY = -1;
	int lastBlackX = -1;
	int lastBlackY = -1;

	for (int i = 0; i < width; i++) {
		if ((image[(i+width*0)*4+1] < 127) && (image[(i+width*0)*4+3] > 127)) {
			//assume black
			if (firstBlackX < 0)
				firstBlackX = i;
			lastBlackX = i;
		}
	}
	for (int i = 0; i < height; i++) {
		if ((image[(0+width*i)*4+1] < 127) && (image[(0+width*i)*4+3] > 127)) {
			//assume black
			if (firstBlackY < 0)
				firstBlackY = i;
			lastBlackY = i;
		}
	}

	fname = fname.substr(0,fname.size()-4);

	//Build the ninepatch
	NinePatchBinary np(firstBlackX,lastBlackX-firstBlackX+1,firstBlackY,lastBlackY-firstBlackY+1);
	np.Save(fname + ".9ptch");

	//Now save cropped photo
	vector<unsigned char> croppedImage;
	for (int y= 1; y < height-1; y++) {
		for (int x = 1; x < width-1; x++) {
			for (int i = 0; i < 4; i++)
				croppedImage.push_back(image[(x+width*y)*4+i]);
		}
	}

	error = lodepng::encode(fname + ".cropped.png",croppedImage,width-2,height-2);
	if(error) { 
		std::cout << "lode pngencoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	return -5;
	}
	cout << "Success!\n";
}

