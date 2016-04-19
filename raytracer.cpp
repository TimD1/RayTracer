#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Light.h"
#include "Sphere.h"
#include "Object.h"
#include "Plane.h"
#include "Source.h"

using namespace std;



class Pixel
{
public:
	Pixel() : r(0), g(0), b(0) {}
	Pixel(double r0, double g0, double b0) : r(r0), g(g0), b(b0) {}

	double r;
	double g;
	double b;
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
		unsigned char color[3] = { 	static_cast<unsigned char>(data[i].b*255), 
									static_cast<unsigned char>(data[i].g*255), 
									static_cast<unsigned char>(data[i].r*255)};
		for(int i = 0; i < 3; i++) ofs_picture << color[i];
	}
}



int closest_obj_idx(const vector<double> & intersections)
{	
	// do most common base cases first to avoid extra operations
	if(intersections.size() == 0) { return -1; }
	else if(intersections.size() == 1) { return (intersections[0] > 0) ? 0 : -1; }
	else //we have multiple objects
	{
		int idx_min = -1;
		double closest_valid = 10e10;
		
		for(int i = 0; i < intersections.size(); i++)
		{
			if(intersections[i] > 0 && intersections[i] < closest_valid)
			{
				closest_valid = intersections[i];
				idx_min = i;
			}
		}
		if(closest_valid == 10e10) return -1; //no valid intersections found
		else return idx_min;
	}
}



Color color_at(	const Vect & int_pos, const Vect & int_ray_dir, 
				const vector<Object*> & scene_objects, const int idx_closest, 
				const vector<Source*> & light_sources, const double accuracy, 
				const double ambient_light)
{
	//object surface begins with this color
	Color obj_color = scene_objects[idx_closest]->color();
	Vect obj_normal = scene_objects[idx_closest]->normal(int_pos);
	Color final_color = obj_color * ambient_light;

	//apply changes to color for each light source
	for(int light_idx = 0; light_idx < light_sources.size(); light_idx++)
	{
		Vect light_dir = (int_pos - light_sources[light_idx]->light_pos()).normalize();

		float cos_angle = obj_normal * light_dir;
		if(cos_angle > 0) //light can directly hit object
		{
			//test for shadows
			bool shadowed = false;
			float dist_to_light = 
					(int_pos - light_sources[light_idx]->light_pos()).magnitude();
			
			//draw ray directly to light source
			Ray shadow_ray(int_pos, 
					(light_sources[light_idx]->light_pos() - int_pos).normalize());
			
			//test to see what objects the shadow ray intersects
			vector<double> shadow_ints;
			for(int obj_idx = 0; obj_idx < scene_objects.size(); obj_idx++)
			{
				shadow_ints.push_back(scene_objects[obj_idx]
						->find_intersection(shadow_ray));
			}
			
			//if the ray intersects something before the light, the point is in shadow
			for(int i = 0; i < shadow_ints.size(); i++)
			{
				if(shadow_ints[i] > accuracy)
				{
					if(shadow_ints[i] <= dist_to_light) shadowed = true;
					break;
				}
			}

			//if the object was not in shadow, adjust color accordingly
			if(!shadowed)
			{
				final_color = final_color + 
					(obj_color * (light_sources[light_idx]->color() * cos_angle));
				
				//if the object is shiny
				if(obj_color.s() > 0 && obj_color.s() <= 1)
				{
					//perform operations to simulate "shininess"
					double dot1 = obj_normal * (int_ray_dir.invert());
					Vect scalar1 = obj_normal * dot1;
					Vect add1 = scalar1 + int_ray_dir;
					Vect scalar2 = add1 * 2;
					Vect add2 = int_ray_dir.invert() + scalar2;
					Vect refl_dir = add2.normalize();

					double specular = refl_dir * light_dir;
					if(specular > 0)
					{
						specular = pow(specular, 10);
						final_color = final_color + 
							(light_sources[light_idx]->color() * 
							 (specular * obj_color.s()));
					}
				}	
			}
			else cout << "Shadow" << endl;
		}
	}
	return final_color.clip();
}


int main()
{
	cout << "Rendering..." << endl;

	//set scaling information for image
	int dpi = 72;
	int width = 640;
	int height = 480;
	double aspectratio = static_cast<double>(width)/static_cast<double>(height);
	double ambient_light = 0.2;
	double accuracy = 0.000001;
	int num_pixels = width*height;
	Pixel pixels[num_pixels];

	//set x, y, and z axes
	Vect X(1,0,0);
	Vect Y(0,1,0);
	Vect Z(0,0,1);
	Vect origin(0,0,0);

	//set direction of camera
	Vect cam_pos(0,-8, 2);
	Vect lookat(0, 0, 0);
	Vect cam_dir(lookat.x()-cam_pos.x(), lookat.y()-cam_pos.y(), lookat.z()-cam_pos.z());
	cam_dir = cam_dir.normalize();
	
	//set orientation of camera, create object
	Vect cam_right = (cam_dir^Z).normalize(); //switched order
	Vect cam_down = (cam_dir^cam_right);
	Camera camera(cam_pos, cam_dir, cam_right, cam_down);

	//create useful colors
	Color white_light(1, 1, 1, 0);
	Color green(0, 1, 0, 0.3);
	Color gray(0.5, 0.5, 0.5, 0);
	Color black(0, 0, 0, 0);
	Color red(1, 0, 0, 0);

	//create light source
	Vect light_position(5, 5, 10);
	Light light_source(light_position, white_light);
	vector<Source*> light_sources;
	light_sources.push_back(dynamic_cast<Source*>(&light_source));

	//create objects in scene
	Sphere sphere(origin, 1, green);
	Plane plane(Z, -1, red);

	vector<Object*> scene_objects;
	scene_objects.push_back(dynamic_cast<Object*>(&sphere));
	scene_objects.push_back(dynamic_cast<Object*>(&plane));


	double xamt, yamt;

	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			//start with no anti-aliasing

			//set distances so a ray will go through each pixel
			if(width > height) //wide rectangular image 
			{
				xamt = ((x+0.5) / width) * aspectratio - 
					((width - height) / static_cast<double>(height)) / 2;
				yamt = (height-y + 0.5) / height;
			}
			else if (height > width) //tall rectangular image
			{
				xamt = (x+0.5) / width;
				yamt = (((height - y) + 0.5) / height) / aspectratio - 
					((height - width) / static_cast<double>(width)) / 2;
			}
			else //square image
			{
				xamt = (x + 0.5) / width;
				yamt = (height - y +0.5) / height;
			}

			//create a ray through each point
			Vect cam_ray_start = camera.position();
			Vect cam_ray_dir = (cam_dir + (cam_right*(xamt-0.5) 
								+ (cam_down*(yamt - 0.5)))).normalize();
			Ray cam_ray(cam_ray_start, cam_ray_dir);
			

			// find the closest intersection pt for each object
			vector<double> intersections;
			for(int i = 0; i < scene_objects.size(); i++)
			{
				intersections.push_back(scene_objects[i]->find_intersection(cam_ray));
				/* if(i == 0) cout << "Sphere: "; */
				/* if(i == 1) cout << "Plane: "; */
				/* cout << scene_objects[i]->find_intersection(cam_ray); */
				/* cout << " (" << x << ", " << y << ")" << endl; */
			}
			
			//determine which of those is the overall closest point
			int idx_closest = closest_obj_idx(intersections);
			/* cout << idx_closest << endl << endl; */

			// determine the color of each pixel
			int idx = y * width + x;
			
			//the ray never actually hit an object
			if(idx_closest < 0)
			{
				pixels[idx].r = 0;
				pixels[idx].g = 0;
				pixels[idx].b = 0;
			}
			else
			{
				
				/* Color this_color = scene_objects[idx_closest]->color(); */
				/* pixels[idx].r = this_color.r(); */
				/* pixels[idx].g = this_color.g(); */
				/* pixels[idx].b = this_color.b(); */

				
				if(intersections[idx_closest] > accuracy)
				{
					//determine position/direction at intersection
					Vect int_pos = cam_ray_start + 
						(cam_ray_dir * intersections[idx_closest]);
					Vect int_ray_dir = cam_ray_dir; //not dealing with reflections yet
					Color int_color;
					int_color = color_at(int_pos, int_ray_dir, scene_objects,
						idx_closest, light_sources, accuracy, ambient_light);
					pixels[idx].r = int_color.r();
					pixels[idx].g = int_color.g();
					pixels[idx].b = int_color.b();
				}
			}
		}
	}

	save_bmp("pic.bmp", width, height, dpi, pixels);

	return 0;
}
