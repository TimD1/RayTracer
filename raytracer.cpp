#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <ctime>

#include "Vect.h"
#include "Ray.h"
#include "Camera.h"
#include "Color.h"
#include "Light.h"
#include "Sphere.h"
#include "Object.h"
#include "Plane.h"
#include "Source.h"
#include "Triangle.h"

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
	
	// s = [2-3): checkered floor pattern
	if(obj_color.s() >= 2 && obj_color.s() < 3)
	{
		int square;
		if(obj_color.s() == 2) // create tiles of size 1
		{
			square = floor(int_pos.x()+0.0000001) + floor(int_pos.y()+0.0000001) + floor(int_pos.z()+0.0000001);
		}
		else // create tiles of size obj_color.s()-2 (s = 2.25 -> tile size 0.25)
		{
			square = floor((1 / float(obj_color.s() - 2)) * (int_pos.x()+0.0000001)) + 
					 floor((1 / float(obj_color.s() - 2)) * (int_pos.y()+0.0000001)) + 
					 floor((1 / float(obj_color.s() - 2)) * (int_pos.z()+0.0000001));
		}

		if(square % 2)
		{
			obj_color.set_red(0);
			obj_color.set_green(0);
			obj_color.set_blue(0);
		}
		else
		{
			obj_color.set_red(0.7);
			obj_color.set_green(0.7);
			obj_color.set_blue(0.7);
		}
	}

	Color final_color = obj_color * ambient_light;

	// s = (0, 1]: add reflectiveness/shininess to object
	if (obj_color.s() > 0 && obj_color.s() <= 1)
	{
		// find projection of intersecting ray onto normal vector
		// aka, the distance the ray is from the plane/surface before/after rebounding
		Vect dist_ortho_to_plane = obj_normal * (obj_normal * int_ray_dir.invert());
		
		// if ray hadn't reflected, it would be dist_ortho_to_plane on the other side of the plane
		// just follow the original ray's path, and correct dist_ortho_to_plane since it bounced back
		Vect refl_dir = (int_ray_dir + dist_ortho_to_plane * 2).normalize();

		Ray refl_ray(int_pos, refl_dir);

		//find first intersection
		vector<double> refl_intersections;

		for(int refl_idx = 0; refl_idx < scene_objects.size(); refl_idx++)
		{
			refl_intersections.push_back(scene_objects[refl_idx]
					->find_intersection(refl_ray));
		}
		int idx_closest_refl = closest_obj_idx(refl_intersections); 
		if(idx_closest_refl >= 0)
		{
			//reflection ray intersects an object
			if(refl_intersections[idx_closest_refl] > accuracy)
			{
				// find position/direction at intersection point
				Vect refl_int_pos = int_pos + (refl_dir * 
						refl_intersections[idx_closest_refl]);
				Vect refl_int_ray_dir = refl_dir;

				Color refl_int_color = color_at(refl_int_pos, refl_int_ray_dir, 
						scene_objects, idx_closest_refl, light_sources, accuracy, 
						ambient_light);
				final_color = final_color + (refl_int_color * obj_color.s());
			}
		}
	}

	//apply changes to color for each light source
	for(int light_idx = 0; light_idx < light_sources.size(); light_idx++)
	{
		Vect light_dir = (int_pos - light_sources[light_idx]->light_pos()).normalize();

		double cos_angle = (obj_normal.invert() * light_dir);
		if(cos_angle > 0) //light can directly hit object
		{
			//test for shadows
			bool shadowed = false;
			
			double dist_to_light = 
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
				if(shadow_ints[i] > accuracy && shadow_ints[i] <= dist_to_light) 
				{
					shadowed = true;
					/* cout << "Shadowed(" << i << "): "; */
					/* cout << int_pos << ' ' << int_ray_dir << endl; */
					/* cout << shadow_ints[i] << " < " << dist_to_light << endl; */
					break;
				}
			}

			//if the object was not in shadow, adjust color accordingly
			if(!shadowed)
			{
				final_color = final_color + 
					(obj_color * (light_sources[light_idx]->color() * cos_angle));
				
				//if the object is reflective
				if(obj_color.s() > 0 && obj_color.s() <= 1)
				{
					//exponentially increase intensity if reflection is of light source
					Vect dist_ortho_to_plane = obj_normal * 
						(obj_normal * int_ray_dir.invert());
					Vect refl_dir = (int_ray_dir + dist_ortho_to_plane * 2).normalize();
					double specular = refl_dir * light_dir;
					
					if(specular > 0)
					{
						specular = pow(specular, 10);
						final_color = final_color + 
							(light_sources[light_idx]->color() * 
							 (specular * obj_color.s()));
					}
				}	
			/* cout << "In Light" << endl; */
			}
		}
		/* else cout << "Facing Away" << endl; */
	}
	return final_color.clip();
}


int main()
{
	cout << "Rendering..." << endl;
	clock_t start, stop;
	start = clock();

	//set scaling information for image
	int dpi = 72;
	int width = 640;
	int height = 480;
	const int aa_depth = 4;
	int rays_per_pixel = aa_depth * aa_depth;
	double aa_threshold = 0.1;
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
	Vect cam_dir( (lookat - cam_pos).normalize() );
	
	//set orientation of camera, create object
	Vect cam_right = (cam_dir^Z).normalize();
	Vect cam_down = (cam_dir^cam_right);
	Camera camera(cam_pos, cam_dir, cam_right, cam_down);

	//create useful colors
	Color white_light(1, 1, 1, 0);
	Color green(0, 1, 0, 0.3);
	Color mirror(0.8, 0.8, 0.8, 1);
	Color gray(0.5, 0.5, 0.5, 0);
	Color black(0, 0, 0, 0);
	Color red(1, 0, 0, 0);
	Color checkered(1, 1, 1, 2);

	//create light source
	Vect light_position(10, 0, 5);
	Light light_source(light_position, white_light);
	vector<Source*> light_sources;
	light_sources.push_back(dynamic_cast<Source*>(&light_source));

	//create objects in scene
	Sphere sphere(origin, 1, mirror);
	Sphere sphere2(Vect(2, 4, 1), 2, green);
	Plane plane(Z, -1, checkered);

	vector<Object*> scene_objects;
	scene_objects.push_back(dynamic_cast<Object*>(&sphere));
	scene_objects.push_back(dynamic_cast<Object*>(&sphere2));
	scene_objects.push_back(dynamic_cast<Object*>(&plane));


	double xamt, yamt;

	for(int x = 0; x < width; x++)
	{
		for(int y = 0; y < height; y++)
		{
			int idx = y * width + x;
			
			//start with blank pixel
			Color pixel_color(0,0,0,0);

			for(int aa_x = 0; aa_x < aa_depth; aa_x++)
			{
				for(int aa_y = 0; aa_y < aa_depth; aa_y++)
				{
					int aa_idx = (aa_y * aa_depth) + aa_x;
					
					srand(time(0));

					if(aa_depth == 1) // no anti-aliasing
					{
						// send a ray through each pixel
						if(width > height)
						{
							//avoid stretching
							xamt = ((x+0.5) / height) - 
								//shift camera view left to accomodate larger width
								(width - height) / double(height*2);
							yamt = (height-y + 0.5) / height;
						}
						else if (height > width)
						{
							xamt = (x+0.5) / width;
							//avoid stretching
							yamt = (((height - y) + 0.5) / width) - 
								//shift camera view up to accomodate larger height
								(height - width) / double(width*2);
						}
						else //square image
						{
							xamt = (x + 0.5) / double(width);
							yamt = (height - y + 0.5) / double(height);
						}
					}
					else //use anti-aliasing
					{
						if(width > height) //wide rectangular image 
						{
							xamt = ((x+double(aa_x)/double(aa_depth-1)) / height) -
								(width - height) / double(height*2);
							yamt = (height-y + double(aa_y)/double(aa_depth-1)) / height;
						}
						else if (height > width) //tall rectangular image
						{
							xamt = (x+double(aa_x)/double(aa_depth-1)) / width;
							yamt = ((height - y) + double(aa_y)/double(aa_depth-1)) / 
									width - (height - width) / double(width*2);
						}
						else //square image
						{
							xamt = (x + double(aa_x)/double(aa_depth-1)) / width;
							yamt = (height-y + double(aa_y)/double(aa_depth-1)) / height;
						}
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
						intersections.push_back(scene_objects[i]->
								find_intersection(cam_ray));
						/* if(i == 0) cout << "Sphere: "; */
						/* if(i == 1) cout << "Plane: "; */
						/* cout << scene_objects[i]->find_intersection(cam_ray); */
						/* cout << " (" << x << ", " << y << ")" << endl; */
					}
					
					//determine which of those is the overall closest point
					int idx_closest = closest_obj_idx(intersections);
					/* cout << idx_closest << endl << endl; */

					
					//the ray never actually hit an object
					if(idx_closest < 0)
					{
						// add nothing to our total color for this pixel
					}
					else
					{
						if(intersections[idx_closest] > accuracy)
						{
							//determine position/direction at intersection
							Vect int_pos = cam_ray_start + 
								(cam_ray_dir * intersections[idx_closest]);
							Vect int_ray_dir = cam_ray_dir; //not dealing with reflections yet
							Color int_color;
							int_color = color_at(int_pos, int_ray_dir, scene_objects,
								idx_closest, light_sources, accuracy, ambient_light);
							pixel_color += (int_color / rays_per_pixel);
						}
					}
				} //end aa_y
			} //end aa_x
			
			pixels[idx].r = pixel_color.r();
			pixels[idx].g = pixel_color.g();
			pixels[idx].b = pixel_color.b();
		} //end y
	} //end x

	save_bmp("pic.bmp", width, height, dpi, pixels);
	
	stop = clock();
	float time = float(stop - start) / CLOCKS_PER_SEC;
	cout << "Image rendered in " << time << " seconds." << endl;

	return 0;
}
