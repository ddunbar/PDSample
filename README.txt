===============================================
 PDSample - Poisson-Disk sample set generation
===============================================

Daniel Dunbar, daniel@zuster.org

Overview
--------

PDSample generates Poisson-disk sampling sets in the domain [-1,1]^2
using a variety of methods. See "A Spatial Data Structure for Fast 
Poisson-Disk Sample Generation", in Proc' of SIGGRAPH 2006 for more
information.


Building
--------

The code should be portable to any platform with 32-bit float's and int's.

Windows: There is an included PDSample.sln for MSVS version 7.
Unix: Type 'make' and hope for the best. 


Usage
-----

PDSample [-m] [-t] [-r <relax count=0>] [-M <multiplier=1>] 
         [-N <minMaxThrows=1000>] <method> <radius> <output>

Options
-------

 * -t 

   Uses tiled (toroidal) domain for supporting samplers. The resulting point set
   will be suitable for tiling repeatedly in the x and y directions.
 
 * -m

   Maximize the resulting point set. For samplers which do not already produce a
   maximal point set then this will use the Boundary sampling method to ensure
   the resulting point set is maximal.
 
 * -r <relax count>

   Apply the specified number of relaxations to the resulting point set. This
   requires that qvoronoi be in the path.
 
 * -M <multiplier>

   For DartThrowing and BestCandidate methods this determines the factor to
   multiply the current number of points by to determine how many samples to
   take before exiting (DartThrowing) or accepting the best candidate
   (BestCandidate).
 
 * -N <minMaxThrows>

   This specifies a minimum number of samples that will be taken for the
   DartThrowing sampler. See below.
   
   
Available Samplers (for method argument)
----------------------------------------

 * DartThrowing

   Standard dart throwing. On each iteration the DartThrowing sampler will try
   min(N*multiplier,minMaxThrows) samples before termination. Note that for
   regular dart throwing where simply a maximum number of throws is used to
   determine the termination point, the multiplier should be set to 0.
 
 * BestCandidate

   Mitchell's Best Candidate algorithm. Uses the multiplier argument.
     
 * Boundary

   Dart throwing by maximizing boundaries.
 
 * Pure

   Dart throwing using scalloped sectors.

 * LinearPure

   Dart throwing using scalloped sectors but without sampling regions according
   to their probability of being hit.
     
 * Penrose

   Ostromoukhov et al.'s sampling method using their quasisampler_prototype.h
     
 * Uniform

   Random point generation. The number of samples to take is calculated as
   .75/radius^2 to approximately match the density of Poisson-disk sampling.


Output format
-------------

Point sets are output in a trivial binary format. The format is not intended for
distribution and does not encode the endianness of the generating platform.

The format matches the pseudo-C struct below::

  struct {
  	int N;       // number of points
  	float t;     // generation time
  	float r;     // radius used in generation
  	int isTiled; // flag for if the set is tileable
  	float points[N][2];
  };
  
The format can be converted to JSON with the ``dump-points`` script.


Acknowledgments
---------------

Thanks to Ares Lagae for comments on a preliminary release of the code,
Ostromoukhov et al. for making available their quasisampler implementation,
as well as Takuji Nishimura and Makoto Matsumoto for their Mersenne
Twister random number generator.
