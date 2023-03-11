#include <iostream>
#include <string>
#include <cmath>

using namespace std;

enum Types
{
	cartesian,
	polar
};

class Point
{
protected:
	float x, y;

public:
	static Point *SimpleFactory(Types, const float &, const float &);

	friend ostream &operator<<(ostream &os, Point p)
	{
		os << "(" << p.x << "; " << p.y << ")";
		return os;
	}
};

class Cartesian : public Point
{
public:
	Cartesian(const float &x_, const float &y_)
	{
		x = x_;
		y = y_;
	}
};

class Polar : public Point
{
public:
	Polar(const float &r, const float &phi)
	{
		x = r * cos(phi);
		y = r * sin(phi);
	}
};

Point *Point::SimpleFactory(Types type, const float &a, const float &b)
{
	switch (type)
	{
	default:
		return new Cartesian(a, b);
		break;

	case cartesian:
		return new Cartesian(a, b);
		break;

	case polar:
		return new Polar(a, b);
		break;
	}
}

class Factory
{
public:
	virtual Point *FactoryMethod(const float &, const float &) = 0;
};

class CartesianFactory : public Factory
{
public:
	Point *FactoryMethod(const float &x, const float &y)
	{
		return new Cartesian(x, y);
	}
};

class PolarFactory : public Factory
{
public:
	Point *FactoryMethod(const float &r, const float &phi)
	{
		return new Polar(r, phi);
	}
};

int main()
{
	CartesianFactory *cartesianFactory = new CartesianFactory;
	Point *cartesianPoint = cartesianFactory->FactoryMethod(5, 12);
	cout << *cartesianPoint << endl;
	delete cartesianFactory;

	PolarFactory *polarFactory = new PolarFactory;
	Point *polarPoint = polarFactory->FactoryMethod(13, M_PI / 3);
	cout << *polarPoint << endl;
	delete polarFactory;

	Point *point;

	point = Point::SimpleFactory(cartesian, 5, 12);
	cout << *point << endl;

	point = Point::SimpleFactory(polar, 5, 12);
	cout << *point << endl;

	delete point;

	return 0;
}