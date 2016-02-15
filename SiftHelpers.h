#ifndef __SIFTHELPERS_H__
#define __SIFTHELPERS_H__

#include "CImg.h"
#include <ctime>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <Sift.h>
#include <float.h>
#include <map>

using namespace cimg_library;
using namespace std;

class Image
{	
	string name;
	vector<SiftDescriptor> descriptors;
	
	public:
	
	Image(string Name)
	{
		name = Name;
		CImg<double> imageData(name.c_str());
		CImg<double> gray = imageData.get_RGBtoHSI().get_channel(2);
		descriptors = Sift::compute_sift(gray);
	}
	
	const vector<SiftDescriptor>& getDescriptors() const
	{
		return descriptors;
	}

	static void MatchSIFT(const Image& queryImage,const Image& image)
	{
		const vector<SiftDescriptor> queryDescriptors = queryImage.getDescriptors();
		const vector<SiftDescriptor> imageDescriptors = image.getDescriptors();
		
		double distance = 0.0;
		
		for(int i = 0;i<queryDescriptors.size();++i)
		{
			double min = DBL_MAX;
			int descriptor = -1;
			for(int j =0;j<imageDescriptors.size();++j)
			{
				double sum = 0;
				for(int k = 0;k<128;++k)
				{
					sum+= abs(queryDescriptors[i].descriptor[k] - imageDescriptors[j].descriptor[k]);
				}
				if(sum < min)
				{
					min = sum;
					descriptor = j;
				}
			}
			
			distance+=min;
		}
		
		cout<<distance<<endl;
	}
	
};


#endif