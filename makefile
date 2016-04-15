all: ray

ray: raytracer.o
		g++ -std=c++11 raytracer.o -o raytracer

raytracer.o: raytracer.cpp
	g++ -std=c++11 -c raytracer.cpp

clean:
		rm *.o raytracer *.bmp

