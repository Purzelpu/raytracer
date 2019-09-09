#include <vector>
#include <iostream>
#include <cmath>
#include <fstream>

struct Vec
{
	public: 
	double x;
	double y;
	double z;

	Vec(double a, double b, double c): x(a), y(b), z(c) { }
	Vec normalize()
	{
		double l = x*x + y*y + z*z;
		l = sqrt(l);
		l = 1/l;
		Vec v(x*l, y*l, z*l);
		return Vec(x*l, y*l, z*l);
	}
};

Vec operator+ (const Vec& a,const  Vec& b)
{
	return Vec(a.x + b.x, a.y+b.y, a.z+b.z);
}

Vec operator- (const Vec& a,const  Vec& b)
{
	return Vec(a.x - b.x, a.y-b.y, a.z-b.z);
}

Vec operator*(const double& a, const Vec& v)
{
	return Vec(a*v.x, a*v.y, a*v.z);
}

double operator*(const Vec& a,const Vec& b)
{
	return a.x*b.x + a.y*b.y + a.z*b.z;
}

struct Colour
{
	unsigned char r;
	unsigned char g;
	unsigned char b;
	Colour(int R, int G, int B): r(R), g(G), b(B) {} 
};


struct Ray
{
	Vec origin;
	Vec direction;
	Ray(Vec o, Vec v): origin(o), direction(v.normalize()) {}
};

class Object
{
	public:
	Colour c;
	Object(Colour col): c(col) {}
	//Intersects the Object with a ray
	//returns t such that origin+t*direction
	//is the nearest intersection
	//returns -1 if no intersection exists
	virtual double intersect(const Ray r) const=0;
};

class Sphere : public Object
{
	public:
		Vec center;
		double radius;
		Sphere(Vec c, double r) : Object(Colour(255,255,255)), center(c), radius(r) {
		}
		double intersect(const Ray r) const
		{
			//t for point with minimal distance
			Vec v = center-r.origin;
			double tmin = v*r.direction;
			if (tmin < 0) return -1;
			//squared distance from the closest point to the
			//center
			double d2 = v*v - tmin*tmin;
			if(d2 > radius*radius) return -1;
			double dt = sqrt(radius*radius - d2);
			return tmin - dt;
		}
};

void output(const std::vector<Colour>& image, unsigned int width, unsigned int height)
{
    std::ofstream ofs("./untitled.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << width << " " << height << "\n255\n";
    for (unsigned i = 0; i < width * height; ++i) {
        ofs << image[i].r <<
               image[i].g <<
               image[i].b;
    }
    ofs.close();
}

int main(int argc, char* argv[])
{
	std::vector<Object*> objects;
	objects.push_back(new Sphere(Vec(0,0,5),3));
	Sphere* s = new Sphere(Vec(1,1,3),1);
	s->c = Colour(255,0,0);
	objects.push_back(s);
	unsigned int width = 400;
	unsigned int height = 400;
	std::vector<Colour> image(width*height, Colour(0,0,0));
	double dx = double(2)/width;
	double dy = double(2)/height;
	//Projektion auf das Rechteck
	//(-1,-1,1) bis (1,1,1).
	Vec v(0,0,1);
	//Position des Betrachter: (0,0,0)
	Vec origin(0,0,0);
	//
	//Iteration Ebene unten links bis
	//oben rechts
	auto pixel = image.begin();
	for(int x=0;x< width; x++)
	{
		v.x = -1 + x*dx;
		for(int y =0;y<height; y++, ++pixel)
		{
			v.y = -1 + y*dy;
			double tmin = 1000;
			Ray r(origin, v);

			for (const auto &obj : objects)
			{
				double t = obj->intersect(r);
				if(t > -1 && t < tmin)
				{
					tmin = t;
					*pixel = obj->c;
				}
			}
		}
	}

	output(image, width, height);
	return 0;
}
