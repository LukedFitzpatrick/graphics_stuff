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

void RayTracer()
{
	const double aspectRatio = 16/9;

	const int imageWidth = 400;
	const int imageHeight = imageWidth / aspectRatio;

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
			double xProportion = (double)x / (imageWidth-1);
			double yProportion = (double)y / (imageHeight-1);

			// TODO tomorrow implement rayColour();
			
			//WriteColour(Colour(1.0, 0, 0));
		}
	}
}

int main()
{
	// MakeDemoPPMFile();
	RayTracer();
}
