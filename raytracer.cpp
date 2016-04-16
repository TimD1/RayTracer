#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <limits>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Light.h"
#include "Sphere.h"
#include "Object.h"
#include "Plane.h"

using namespace std;

class Pixel
{
public:
	Pixel() : r(0), g(0), b(0) {}
	Pixel(int r0, int g0, int b0) : r(r0), g(g0), b(b0) {}

	int r;
	int g;
	int b;
};

void save_bmp(string filename, int w, int h, int dpi, Pixel *data)
{
	int num_pix = w*h;
	
	// assign necessary image headers, use some bit-shifting
	unsigned char bmp_file_header[14] = { 'B', 'M', 0,0,0,0, 0,0,0,0, 54,0,0,0 };
	int filesize = 54+4*num_pix;
	bmp_file_header[2] = (unsigned char) (filesize);
	bmp_file_header[3] = (unsigned char) (filesize>>8);
	bmp_file_header[4] = (unsigned char) (filesize>>16);
	bmp_file_header[5] = (unsigned char) (filesize>>24);
    
	unsigned char bmp_info_header[40] = { 40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0,24,0 };
	int s = 4*num_pix;
	int factor = 40;
	int ppm = dpi * factor;
    bmp_info_header[ 4] = (unsigned char) (w);
    bmp_info_header[ 5] = (unsigned char) (w>>8);
    bmp_info_header[ 6] = (unsigned char) (w>>16);
    bmp_info_header[ 7] = (unsigned char) (w>>24);
    bmp_info_header[ 8] = (unsigned char) (h);
    bmp_info_header[ 9] = (unsigned char) (h>>8);
    bmp_info_header[10] = (unsigned char) (h>>16);
    bmp_info_header[11] = (unsigned char) (h>>24);
    bmp_info_header[21] = (unsigned char) (s);
    bmp_info_header[22] = (unsigned char) (s>>8);
    bmp_info_header[23] = (unsigned char) (s>>16);
    bmp_info_header[24] = (unsigned char) (s>>24);
    bmp_info_header[25] = (unsigned char) (ppm);
    bmp_info_header[26] = (unsigned char) (ppm>>8);
    bmp_info_header[27] = (unsigned char) (ppm>>16);
    bmp_info_header[28] = (unsigned char) (ppm>>24);
    bmp_info_header[29] = (unsigned char) (ppm);
    bmp_info_header[30] = (unsigned char) (ppm>>8);
    bmp_info_header[31] = (unsigned char) (ppm>>16);
    bmp_info_header[32] = (unsigned char) (ppm>>24);

	// insert header info into the bmp file
	ofstream ofs_picture(filename);
	for(int i = 0; i < 14; i++) ofs_picture << bmp_file_header[i];
	for(int i = 0; i < 40; i++) ofs_picture << bmp_info_header[i];

	// insert pixel color info into the file as well
	for(int i = 0; i < num_pix; i++)
	{
		// original colors are 0-1, must be converted to hex
		unsigned char color[3] = { 	static_cast<unsigned char>(data[i].r*255), 
									static_cast<unsigned char>(data[i].g*255), 
									static_cast<unsigned char>(data[i].b*255)};
		for(int i = 0; i < 3; i++) ofs_picture << color[i];
	}
}



int main()
{
	cout << "Rendering..." << endl;

	//set scaling information for image
	int dpi = 72;
	int width = 640;
	int height = 480;
	int num_pixels = width*height;
	Pixel pixels[num_pixels];

	//set x, y, and z axes
	Vect X(1,0,0);
	Vect Y(0,1,0);
	Vect Z(0,0,1);
	Vect origin(0,0,0);

	//set direction of camera
	Vect cam_pos(3,1.5,-4);
	Vect point(0, 0, 0);
	Vect cam_dir(point.x()-cam_pos.x(), point.y()-cam_pos.y(), point.z()-cam_pos.z());
	cam_dir = cam_dir.normalize();
	
	//set orientation of camera, create object
	Vect cam_right = (Y^cam_dir).normalize();
	Vect cam_down = (cam_right^cam_dir);
	Camera camera(cam_pos, cam_dir, cam_right, cam_down);

	//create useful colors
	Color white_light(1, 1, 1, 0);
	Color green(0.5, 1, 0.5, 0.3);
	Color gray(0.5, 0.5, 0.5, 0);
	Color black(0, 0, 0, 0);
	Color red(1, 0, 0, 0);

	//create light source
	Vect light_position(-7, 10, -10);
	Light light_source(light_position, white_light);

	//create sphere
	Sphere sphere(origin, 1, green);

	//create plane
	Plane plane(Y, -1, red);
	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			// determine the color of each pixel
			int idx = y * width + x;
			
			pixels[idx].r = 0.5;
			pixels[idx].g = 1;
			pixels[idx].b = 0.5;
		}
	}

	save_bmp("pic.bmp", width, height, dpi, pixels);

	return 0;
}
