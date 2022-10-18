#include <stdio.h>
#include <optional>
#include "../types.h"

const int MAX_COLOUR = 255;

void WritePPMHeader(int width, int height)
{
	// see https://en.wikipedia.org/wiki/Netpbm
	printf("P3\n"); // RGB colour image in ASCII
	printf("%d %d\n", width, height);  // image size
	printf("%d\n", MAX_COLOUR); // max value for each colour
}

void WriteColour(Colour c)
{
	int rOut = MAX_COLOUR * c.mX;
	int gOut = MAX_COLOUR * c.mY;
	int bOut = MAX_COLOUR * c.mZ;
	printf("%d %d %d\n", rOut, gOut, bOut);
}

void MakeDemoPPMFile()
{
	int width = 256;
	int height = 256;

	WritePPMHeader(width, height);
	
	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			// rgb represented from 0 to 1
			double r = double(x) / (width-1);
			double g = double(height-y) / (height-1);
			double b = 0.25;

			WriteColour(Colour(r, g, b));
		}
	}
}

struct HitRecord
{
	Point3 mPointOfIntersection;
	Vec3 mNormal;
	double mRayT;
};

// TODO exercise: come back and write this with no virtual function calls
class IHittable
{
public:
	virtual ~IHittable() = default;
	virtual std::optional<HitRecord> Hit(const Ray& ray, double tMin, double tMax) = 0;
};

class Sphere : public IHittable
{
public:
	Sphere(Point3 center, double radius)
		: mCenter(center)
		, mRadius(radius)
	{
	}
	
	Point3 mCenter;
	double mRadius;
	
	std::optional<HitRecord> Hit(const Ray& ray, double tMin, double tMax) override
	{
		// magic maths for the intersection of a Ray and a Sphere
		Vec3 oc = ray.mOrigin - mCenter;

		
		double a = ray.mDirection.LengthSquared();
		double halfB = DotProduct(oc, ray.mDirection);
		double c = oc.LengthSquared() - (mRadius*mRadius);

		double discriminant = (halfB*halfB) - (a*c);

		if(discriminant < 0)
		{
			// negative sqrt indicates no intersection
			return std::nullopt;
		}

		double sqrtd = sqrt(discriminant);
		double root = (-halfB - sqrtd) / a;
		if(root < tMin || root > tMax)
		{
			// try the other intersection point
			root = (-halfB + sqrtd) / a;
			if(root < tMin || root > tMax)
			{
				return std::nullopt;
			}
		}

		HitRecord hr;
		hr.mRayT = root;		
		hr.mPointOfIntersection = ray.AtParam(hr.mRayT);
		hr.mNormal = UnitVector(hr.mPointOfIntersection - mCenter);
		return hr;
	}
};

Colour PixelColour(Ray r)
{
	Sphere sph(Point3(0, 0, -1), 0.5);
	auto hitRecord = sph.Hit(r, -999, 999);
	
	if(hitRecord.has_value())
	{
		// work out the normal to the point of intersection
		// Point3 pointOfIntersection = r.AtParam(*hitParam);
		// Vec3 normal = UnitVector(pointOfIntersection - sphereCenter);

		// all of the components of normal are between -1.0 and
		// 1.0. If we add 1.0 it is between 0 and 2.0. Then multiply
		// by 0.5 to get it between 0 and 1.0 (the range of a colour).
		return 0.5 * Colour(hitRecord->mNormal.mX + 1.0, hitRecord->mNormal.mY + 1.0, hitRecord->mNormal.mZ + 1.0);
	}
	else
	{
		Vec3 unitDirection = UnitVector(r.mDirection);
		double t = 0.5*(unitDirection.mY + 1.0);

		// lerp between white and blue as the normalised y coordinate changes
		return ((1.0-t) * Colour(1.0, 1.0, 1.0)) + (t * Colour(0.5, 0.7, 1.0));
	}
}

void RayTracer()
{
	const double aspectRatio = 16.0 / 9.0;

	const int imageWidth = 400;
	const int imageHeight = (int)(imageWidth / aspectRatio);

	// not sure on why we picked 2.0 for this - this doesn't appear to be measured in pixels.
	const double viewportHeight = 2.0;
	const double viewportWidth = viewportHeight * aspectRatio;

	// how far is it from the eye to the projection plane
	const double focalLength = 1.0;

	// this is where the eye is
	Point3 origin(0, 0, 0);

	Vec3 viewportXVec(viewportWidth, 0, 0);
	Vec3 viewportYVec(0, viewportHeight, 0);

	// bottom left of the projection plane. You can either calculate
	// this by calculating each x, y, z component individually, or by
	// doing vector arithmetic.
	Point3 bottomLeftCorner(-viewportWidth*0.5, -viewportHeight*0.5, origin.mZ - focalLength);
	//Point3 bottomLeftCornerCheck = origin - (viewportXVec * 0.5) - (viewportYVec * 0.5) - Vec3(0, 0, focalLength);
	
	WritePPMHeader(imageWidth, imageHeight);

	// no idea why they did the loops like this (starting at max value
	// for y, but starting at min value for x). Just following
	// convention so that the images appear around the right way.
	for(int y = imageHeight-1; y >= 0; y--)
	{
		for(int x = 0; x < imageWidth; x++)
		{
			double xProportion = ((double)x / (imageWidth-1));
			double yProportion = ((double)y / (imageHeight-1));
			
			// starts at the eye, goes to the current pixel we're trying to draw
			Ray eyeToPixelRay(origin, (bottomLeftCorner + (viewportXVec*xProportion) + (viewportYVec*yProportion)) - origin);
			
			Colour pixelColour = PixelColour(eyeToPixelRay);
			
			WriteColour(pixelColour);
		}
	}
}

int main()
{
	// MakeDemoPPMFile();
	RayTracer();
}
