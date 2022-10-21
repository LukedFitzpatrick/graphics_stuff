#include <stdio.h>
#include <optional>
#include "../types.h"
#include <limits>
#include <vector>

const int MAX_COLOUR = 255;

double RandomDouble() {
    // random double between 0 (inclusive) and 1 (not inclusive)
    return rand() / (RAND_MAX + 1.0);
}

double RandomDouble(double min, double max) {
    // random double between min (inclusive) and max (not inclusive)
    return min + (max-min)*RandomDouble();
}

void WritePPMHeader(int width, int height)
{
	// see https://en.wikipedia.org/wiki/Netpbm
	printf("P3\n"); // RGB colour image in ASCII
	printf("%d %d\n", width, height);  // image size
	printf("%d\n", MAX_COLOUR); // max value for each colour
}

// colour represents the sum of 'samplesPerPixel' samples, we divide
// in this function. I wouldn't personally implement it like this,
// maybe it's best practice.
void WriteColour(Colour c, int samplesPerPixel)
{
	int rOut = (MAX_COLOUR * c.mX) / samplesPerPixel;
	int gOut = (MAX_COLOUR * c.mY) / samplesPerPixel;
	int bOut = (MAX_COLOUR * c.mZ) / samplesPerPixel;
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

			WriteColour(Colour(r, g, b), 1);
		}
	}
}

struct HitRecord
{
	Point3 mPointOfIntersection;
	Vec3 mOutwardNormal;
	double mRayT;

	// did we hit inside the object (frontFace=false) or outside
	// (frontFace=true)
	bool mIsFrontFace;
};

struct Camera
{
	Camera()
	{
		double aspectRatio = 16.0 / 9.0;
		double viewportHeight = 2.0;
		double viewportWidth = viewportHeight * aspectRatio;

		// how far from the eye to the projection plane
		double focalLength = 1.0;

		// this is where the eye is
		mOrigin = Point3(0, 0, 0);

		mViewportXVec = Vec3(viewportWidth, 0, 0);
		mViewportYVec = Vec3(0, viewportHeight, 0);

		// bottom left of the projection plane. You can either calculate
		// this by calculating each x, y, z component individually, or by
		// doing vector arithmetic.
		mBottomLeftCorner = Point3(-viewportWidth*0.5, -viewportHeight*0.5, mOrigin.mZ - focalLength);
		//Point3 bottomLeftCornerCheck = origin - (viewportXVec * 0.5) - (viewportYVec * 0.5) - Vec3(0, 0, focalLength);
	}

	// ray from the eye to the projection plane at a particular point
	Ray GetEyeToPixelRay(double xProportion, double yProportion) const {
		Ray eyeToPixelRay(mOrigin, (mBottomLeftCorner + (mViewportXVec*xProportion) + (mViewportYVec*yProportion)) - mOrigin);
		return eyeToPixelRay;
	}
	
	Point3 mOrigin;
	Vec3 mViewportXVec;
	Vec3 mViewportYVec;
	Point3 mBottomLeftCorner;
};

// TODO exercise: come back and write this with no virtual function calls
class IHittable
{
public:
	virtual ~IHittable() = default;
	virtual std::optional<HitRecord> Hit(const Ray& ray, double tMin, double tMax) = 0;
};

struct World
{
	std::vector<IHittable*> mHittables;

	std::optional<HitRecord> Hit(const Ray& ray, double tMin, double tMax) const
	{
		std::optional<HitRecord> closestHitRecord;
		
		// find the closest thing that our ray hits
		for(auto* hittable : mHittables)
		{
			auto hitRecord = hittable->Hit(ray, tMin, tMax);
			if(hitRecord.has_value())
			{
				if((!closestHitRecord.has_value()) || hitRecord->mRayT < closestHitRecord->mRayT)
				{
					closestHitRecord = hitRecord;
				}
			}
		}

		return closestHitRecord;
	}
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

		Vec3 normal = UnitVector(hr.mPointOfIntersection - mCenter);

		// determine if this was a front or back face
		if(DotProduct(normal, ray.mDirection) > 0)
		{
			// dot product > 0 implies ray and normal are pointing in
			// the same direction, so we must have hit the sphere from
			// the inside.
			hr.mOutwardNormal = -normal;
			hr.mIsFrontFace = false;
		}
		else
		{
			hr.mOutwardNormal = normal;
			hr.mIsFrontFace = true;
		}

		return hr;
	}
};

Colour PixelColour(Ray r, const World& world)
{
	// Sphere sph(Point3(0, 0, -1), 0.5);
	// auto hitRecord = sph.Hit(r, -999, 999);

	auto hitRecord = world.Hit(r, 0, INFINITY);
	
	if(hitRecord.has_value())
	{
		// work out the normal to the point of intersection
		// Point3 pointOfIntersection = r.AtParam(*hitParam);
		// Vec3 normal = UnitVector(pointOfIntersection - sphereCenter);

		// all of the components of normal are between -1.0 and
		// 1.0. If we add 1.0 it is between 0 and 2.0. Then multiply
		// by 0.5 to get it between 0 and 1.0 (the range of a colour).
		return 0.5 * Colour(hitRecord->mOutwardNormal.mX + 1.0,
							hitRecord->mOutwardNormal.mY + 1.0,
							hitRecord->mOutwardNormal.mZ + 1.0);
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
	World world;
	Sphere sph1(Point3(0, 0, -1), 0.5);
	Sphere sph2(Point3(0, -100.5, -1), 100);
	world.mHittables.push_back(&sph1);
	world.mHittables.push_back(&sph2);

	Camera camera;
	
	const double aspectRatio = 16.0 / 9.0;
	const int imageWidth = 400;
	const int imageHeight = (int)(imageWidth / aspectRatio);

	const int samplesPerPixel = 100;
	
	WritePPMHeader(imageWidth, imageHeight);

	// no idea why they did the loops like this (starting at max value
	// for y, but starting at min value for x). Just following
	// convention so that the images appear around the right way.
	for(int y = imageHeight-1; y >= 0; y--)
	{
		for(int x = 0; x < imageWidth; x++)
		{
			Colour pixelColour(0, 0, 0);

			// take average of multiple samples to achieve anti aliasing
			for(int sample = 0; sample < samplesPerPixel; sample++)
			{
				double xProportion = (((double)x + RandomDouble(0, 1)) / (imageWidth-1));
				double yProportion = (((double)y + RandomDouble(0, 1)) / (imageHeight-1));
			
				// starts at the eye, goes to the current pixel we're trying to draw
				Ray eyeToPixelRay = camera.GetEyeToPixelRay(xProportion, yProportion);
				pixelColour += PixelColour(eyeToPixelRay, world);
			}

			WriteColour(pixelColour, samplesPerPixel);
		}
	}
}

int main()
{
	// MakeDemoPPMFile();
	RayTracer();
}
