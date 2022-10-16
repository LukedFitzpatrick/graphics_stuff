#include <stdio.h>
#include "../types.h"

void MakeDemoPPMFile()
{
	int width = 256;
	int height = 256;
	
	// see https://en.wikipedia.org/wiki/Netpbm
	printf("P3\n"); // RGB colour image in ASCII
	printf("%d %d\n", width, height);  // image size
	printf("255\n"); // max value for each colour

	for(int y = 0; y < height; y++)
	{
		for(int x = 0; x < width; x++)
		{
			// rgb represented from 0 to 1
			double r = double(x) / (width-1);
			double g = double(height-y) / (height-1);
			double b = 0.25;

			// convert to output format
			int rOut = 255 * r;
			int gOut = 255 * g;
			int bOut = 255 * b;
				
			printf("%d %d %d\n", rOut, gOut, bOut);
		}
	}
	
}

int main()
{
	//MakeDemoPPMFile();

	vec2 v1(4, 5);
	vec2 v2(-4, -5);

	v1.printSelf();
	v2.printSelf();
	v1 += v2;
	v1.printSelf();

	v2 *= 3;
	v2.printSelf();
}
