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
	CImg<double> input_image;
	
	public:
	
	Image(string Name)
	{
		name = Name;
		CImg<double> imageData(name.c_str());
		input_image = imageData;
		CImg<double> gray = imageData.get_RGBtoHSI().get_channel(2);
		descriptors = Sift::compute_sift(gray);
	}
	
	const vector<SiftDescriptor>& getDescriptors() const
	{
		return descriptors;
	}
	
	CImg<double> getImageData() const
	{
		return input_image;
	}
	
	struct SiftDescriptorCompare
	{
		bool operator() (const SiftDescriptor& lhs, const SiftDescriptor& rhs) const
		{
			return lhs.row < rhs.row;
		}
	};


	static void MatchSIFT(const Image& queryImage,const Image& image)
	{
		const vector<SiftDescriptor> queryDescriptors = queryImage.getDescriptors();
		const vector<SiftDescriptor> imageDescriptors = image.getDescriptors();
		
		double distance = 0.0;
		
		map<SiftDescriptor,SiftDescriptor,SiftDescriptorCompare> mapping;
		int count = 0;
			
		for(int i = 0;i<queryDescriptors.size();++i)
		{
			double min = DBL_MAX;
			double second_min = DBL_MAX;
			int descriptor = -1;
			for(int j =0;j<imageDescriptors.size();++j)
			{
				double sum = 0;
				for(int k = 0;k<128;++k)
				{
					sum+= abs(queryDescriptors[i].descriptor[k] - imageDescriptors[j].descriptor[k]);
				}
				
				sum = sqrt(sum);
				if(sum < min)
				{
					second_min = min;
					min = sum;
					descriptor = j;
				}
				else if(sum < second_min)
				{
					second_min = sum;
				}
			}
			
			if(min/second_min < 0.8)
			{
				count++;
				cout<<min<<",";
			
				SiftDescriptor q = queryDescriptors[i];
				SiftDescriptor im = imageDescriptors[descriptor];
				mapping[q] = im;
			}
			distance+=min;
		}
		cout<<"Count = "<<count<<endl;
		
		CImg<double> queryImageData = queryImage.getImageData();
		queryImageData.append(image.getImageData());
		queryImageData.get_normalize(0,255).save("sift.png");
		cout<<distance<<endl;
	}
	
};


#endif