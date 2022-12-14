#include <stdio.h>
#include <cmath>

struct Vec3
{
	double mX = 0;
	double mY = 0;
	double mZ = 0;

	Vec3()
	{
	}

	Vec3(double x, double y, double z)
		: mX(x)
		, mY(y)
		, mZ(z)
	{
	}

	Vec3 operator-()
	{
		return Vec3(-mX, -mY, -mZ);
	}
	
	Vec3& operator+=(const Vec3& rhs)
	{
		mX += rhs.mX;
		mY += rhs.mY;
		mZ += rhs.mZ;
		return *this;
	}

	Vec3& operator*=(double scalar)
	{
		mX *= scalar;
		mY *= scalar;
		mZ *= scalar;
		return *this;
	}

	Vec3& operator/=(double scalar)
	{
		*this *= (1/scalar);
		return *this;
	}

	double LengthSquared() const
	{
		return (mX*mX) + (mY*mY) + (mZ*mZ);
	}

	double Length() const
	{
		return sqrt(LengthSquared());
	}

	void PrintSelf()
	{
		printf("[%.2f %.2f %.2f]\n", mX, mY, mZ);
	}
};

inline Vec3 operator*(double t, Vec3 v) {
	v *= t;
	return v;
}

inline Vec3 operator*(Vec3 v, double t) {
	return t * v;
}

inline Vec3 operator+(Vec3 v1, Vec3 v2) {
	v1 += v2;
	return v1;
}

inline Vec3 operator-(Vec3 v1, Vec3 v2) {
	v1 += (-v2);
	return v1;
}

inline Vec3 UnitVector(Vec3 v)
{
	v *= (1 / v.Length());
	return v;
}

inline double DotProduct(Vec3 v1, Vec3 v2)
{
	return (v1.mX * v2.mX) + (v1.mY * v2.mY) + (v1.mZ * v2.mZ);
}

typedef Vec3 Colour;
typedef Vec3 Point3;

struct Ray
{
	Ray()
	{
	}
	
	Ray(Point3 origin, Vec3 direction)
		: mOrigin(origin)
		, mDirection(direction)
	{
	}

	// t is a sliding parameter along the ray - positive numbers
	// result in something in the direction of the ray, negative is
	// behind it.
	Point3 AtParam(double t) const
	{
		return mOrigin + (t*mDirection);
	}

	Point3 mOrigin;
	Vec3 mDirection;
};
