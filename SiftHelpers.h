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

struct SiftDescriptorCompare
{
	bool operator() (const SiftDescriptor& lhs, const SiftDescriptor& rhs) const
	{
		return lhs.row < rhs.row;
	}
};

typedef map<SiftDescriptor,SiftDescriptor,SiftDescriptorCompare> SiftDescriptorMap;

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
	
	string getName()
	{
		return name;
	}
	
	const vector<SiftDescriptor>& getDescriptors() const
	{
		return descriptors;
	}
	
	CImg<double> getImageData() const
	{
		return input_image;
	}
	
	static void DrawImage(const Image& queryImage,const Image& image,SiftDescriptorMap& mapping)
	{
		CImg<double> queryImageData = queryImage.getImageData();
		int queryHeight = queryImageData.height();
		int queryWidth = queryImageData.width();
		queryImageData.append(image.getImageData());
		
		SiftDescriptorMap::iterator mStart = mapping.begin();
		SiftDescriptorMap::iterator mEnd = mapping.end();
		
		
		const double yellow[] = {255.0,255.0,0.0};
		while(mStart != mEnd)
		{
			const int y0 = mStart->first.row;
			const int x0 = mStart->first.col;
			const int y1 = mStart->second.row;
			const int x1 = queryWidth + mStart->second.col;
			
			queryImageData.draw_line(x0,y0,x1,y1,yellow);
			++mStart;	
		}
		
		queryImageData.get_normalize(0,255).save("sift.png");
	}
	
	static int MatchSIFT(const Image& queryImage,const Image& image)
	{
		const vector<SiftDescriptor> queryDescriptors = queryImage.getDescriptors();
		const vector<SiftDescriptor> imageDescriptors = image.getDescriptors();
		
		//double distance = 0.0;
		
		SiftDescriptorMap mapping;
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
			
				SiftDescriptor q = queryDescriptors[i];
				SiftDescriptor im = imageDescriptors[descriptor];
				mapping[q] = im;
				
				//distance+=min/second_min;
			}
			
		}
		
		//cout<<"Count = "<<count<<endl;
		Image::DrawImage(queryImage,image,mapping);
		return count;
	}
	
	static void descriptorMatching1(Image& queryImage,vector<Image>& images)
	{
		multimap<int,string> ranking;
		for(int i = 0;i<images.size();++i)
		{
			int count = Image::MatchSIFT(queryImage,images[i]);
			ranking.insert(make_pair(count,images[i].getName()));
		}
			
		multimap<int,string>::reverse_iterator rankingStart = ranking.rbegin();
		multimap<int,string>::reverse_iterator rankingEnd = ranking.rend();
			
		int top10 = 0;
		while(rankingStart != rankingEnd && ++top10 <= 10)
		{
			cout<<"Image = "<<rankingStart->second<<" Count = "<<rankingStart->first<<endl;
			rankingStart++;
		}
	}
	
};


#endif