#include <stdio.h>

struct vec2
{
	double mX = 0;
	double mY = 0;

	vec2()
	{
	}

	vec2(double x, double y)
		: mX(x)
		, mY(y)
	{
	}

	vec2& operator+=(const vec2& rhs)
	{
		mX += rhs.mX;
		mY += rhs.mY;
		return *this;
	}

	vec2& operator*=(double scalar)
	{
		mX *= scalar;
		mY *= scalar;
		return *this;
	}

	void printSelf()
	{
		printf("[%.2f %.2f]\n", mX, mY);
	}
};
