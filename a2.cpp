// B657 assignment 2 skeleton code
//
// Compile with: "make"
//
// See assignment handout for command line and project specifications.


//Link to the header file
#include "CImg.h"
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>

//Use the cimg namespace to access the functions easily
using namespace cimg_library;
using namespace std;


int main(int argc, char **argv)
{
  try {

    if(argc < 2)
      {
	cout << "Insufficent number of arguments; correct usage:" << endl;
	cout << "    a2-p1 part_id ..." << endl;
	return -1;
      }

    string part = argv[1];
    string inputFile = argv[2];

    if(part == "part1")
      {
	// This is just a bit of sample code to get you started, to
	// show how to use the SIFT library.

	CImg<double> input_image(inputFile.c_str());

	// convert image to grayscale
	CImg<double> gray = input_image.get_RGBtoHSI().get_channel(2);
		vector<SiftDescriptor> descriptors = Sift::compute_sift(gray);

	for(int i=0; i<descriptors.size(); i++)
	  {
	    cout << "Descriptor #" << i << ": x=" << descriptors[i].col << " y=" << descriptors[i].row << " descriptor=(";
	    for(int l=0; l<128; l++)
	      cout << descriptors[i].descriptor[l] << "," ;
	    cout << ")" << endl;

	    for(int j=0; j<5; j++)
	      for(int k=0; k<5; k++)
		if(j==2 || k==2)
		  for(int p=0; p<3; p++)
		    input_image(descriptors[i].col+k, descriptors[i].row+j, 0, p)=0;

	  }

	input_image.get_normalize(0,255).save("sift.png");
      }
    else if(part == "part2")
      {
	// do something here!
      }
    else
      throw std::string("unknown part!");

    // feel free to add more conditions for other parts (e.g. more specific)
    //  parts, for debugging, etc.
  }
  catch(const string &err) {
    cerr << "Error: " << err << endl;
  }
}








