#include <stdio.h>
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

// does the ray 'ray' intersect with a sphere center 'center', radius 'radius'
// this is some magic maths for now - don't fully understand how this works
bool HitSphere(Point3 center, double radius, Ray ray)
{
	Vec3 oc = ray.mOrigin - center;
	double a = DotProduct(ray.mDirection, ray.mDirection);
	double b = 2.0 * DotProduct(oc, ray.mDirection);
	double c = DotProduct(oc, oc) - (radius*radius);
	double discriminant = (b*b) - (4*a*c);
	
	return (discriminant > 0);
}

Colour PixelColour(Ray r)
{
	if(HitSphere(Point3(0, 0, -1), 0.5, r))
	{
		return Colour(1.0, 0, 0);
	}
	
	Vec3 unitDirection = UnitVector(r.mDirection);
	double t = 0.5*(unitDirection.mY + 1.0);

	// lerp between white and blue as the normalised y coordinate changes
	return ((1.0-t) * Colour(1.0, 1.0, 1.0)) + (t * Colour(0.5, 0.7, 1.0));
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
	
	for(int y = 0; y < imageHeight; y++)
	{
		for(int x = 0; x < imageWidth; x++)
		{
			double xProportion = 1 - ((double)x / (imageWidth-1));
			double yProportion = 1 - ((double)y / (imageHeight-1));
			
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
