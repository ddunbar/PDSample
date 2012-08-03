#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <math.h>
#include <string.h>

#include "PDSampling.h"

///

#ifdef _WIN32
double timeInSeconds()
{
	static double perfFreq;
	static int hasPerfTimer = -1;

	if (hasPerfTimer==-1) {
		LARGE_INTEGER perfFreqCountsPerSec;

		if (QueryPerformanceFrequency(&perfFreqCountsPerSec)) {
			hasPerfTimer = 1;
			perfFreq = (double) perfFreqCountsPerSec.QuadPart;
		} else {
			hasPerfTimer = 0;
		}
	}

	LARGE_INTEGER count;
	if (hasPerfTimer && QueryPerformanceCounter(&count)) {
		return (double) count.QuadPart/perfFreq;
	} else {
		return GetTickCount()/1000.0;
	}
}
#else
#include <time.h>
#include <sys/time.h>

double timeInSeconds()
{
	struct timeval tv;
	struct timezone tz;
	
	gettimeofday(&tv, &tz);
	return tv.tv_sec + tv.tv_usec/1000000.0;
}
#endif

///

void usage(char *app)
{
	printf(	"Usage: %s [-m] [-t] [-r <relax count=0>] [-M <multiplier=10>] [-N <minMaxThrows=1000>] <method> <radius> <output>\n", app);
	printf(	"	-m		maximize point set after sampling\n");
	printf(	"	-t		use tiled domain\n");
	printf(	"	-r		apply the specified number of relaxations after sampling (requires qvoronoi)\n");
	printf(	"	-M		set multiplier for DartThrowing and BestCandidate samplers\n");
	printf(	"	-N		set minimum number of maximum throws for DartThrowing sampler\n");
	printf(	"	available methods = {\n"
			"		DartThrowing, (uses multiplier and minMaxThrows)\n"
			"		BestCandidate, (uses multiplier)\n"
			"		Boundary, \n"
			"		Pure, \n"
			"		LinearPure, \n"
			"		Penrose, \n"
			"		Uniform" "}\n");

	exit(1);
}

int main(int argc, char **argv)
{
	int multiplier = 1, minMaxThrows = 1000;
	bool maximize = false;
	bool isTiled = false;
	int relax = 0;
	char *method;
	float radius;
	char *outputPath;
	PDSampler *sampler;
	FILE *output;
	double startTime;
	float elapTime;
	int i, N;

	for (i=1; i<argc; i++) {
		char *arg = argv[i];

		if (strcmp(arg,"-m")==0) {
			maximize = true;
		} else if (strcmp(arg,"-t")==0) {
			isTiled = true;
		} else if (strcmp(arg,"-r")==0) {
			if (i+1<argc) {
				i++;
				relax = atoi(argv[i]);
			} else {
				usage(argv[0]);
			}
		} else if (strcmp(arg,"-N")==0) {
			if (i+1<argc) {
				i++;
				minMaxThrows = atoi(argv[i]);
			} else {
				usage(argv[0]);
			}
		} else if (strcmp(arg,"-M")==0) {
			if (i+1<argc) {
				i++;
				multiplier = atoi(argv[i]);
			} else {
				usage(argv[0]);
			}
		} else {
			break;
		}
	}

	if (i+3!=argc) usage(argv[0]);
	
	method = argv[i++];
	radius = (float) strtod(argv[i++],NULL);
	outputPath = argv[i++];

	if (radius<0.0005 || radius>.2) {
		printf("Radius (%f) is outside allowable range.\n", radius);
		exit(1);
	}

	if (!strcmp(method, "DartThrowing")) {
		sampler = new DartThrowing(radius, isTiled, minMaxThrows, multiplier);
	} else if (!strcmp(method, "BestCandidate")) {
		sampler = new BestCandidate(radius, isTiled, multiplier);
	} else if (!strcmp(method, "Boundary")) {
		sampler = new BoundarySampler(radius, isTiled);
	} else if (!strcmp(method, "Pure")) {
		if (!isTiled) {
			printf("Pure sampler does not support untiled domain.\n");
			exit(1);
		}
		sampler = new PureSampler(radius);
	} else if (!strcmp(method, "LinearPure")) {
		if (!isTiled) {
			printf("LinearPure sampler does not support untiled domain.\n");
			exit(1);
		}
		sampler = new LinearPureSampler(radius);
	} else if (!strcmp(method, "Penrose")) {
		if (isTiled) {
			printf("Penrose sampler does not support tiled domain.\n");
			exit(1);
		}
		sampler = new PenroseSampler(radius);
	} else if (!strcmp(method, "Uniform")) {
		sampler = new UniformSampler(radius);
	} else {
		printf("Unrecognized sampling method (%s).\n", method);
		exit(1);
	}

	startTime = timeInSeconds();
	sampler->complete();
	if (maximize) sampler->maximize();
	for (i=0; i<relax; i++) sampler->relax();
	elapTime = (float) (timeInSeconds() - startTime);

	N = (int) sampler->points.size();
	output = fopen(outputPath,"wb");
	if (!output) {
		printf("Unable to open output file (%s).\n", outputPath);
		exit(1);
	} 

	fwrite(&N, 4, 1, output);
	fwrite(&elapTime, 4, 1, output);
	fwrite(&radius, 4, 1, output);
	fwrite(&isTiled, 4, 1, output);
	for (i=0; i<N; i++) {
		fwrite(&sampler->points[i], 8, 1, output);
	}
	fclose(output);
	printf("Wrote: %s (%d points, %fs, %6.1f pts/s)\n", outputPath, N, elapTime, N/elapTime);

	delete sampler;

	return 0;
}
