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
#include <random>

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
	vector<SiftDescriptor> newDescriptors;
	
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
	
	vector<SiftDescriptor> getDescriptors() const
	{
		return descriptors;
	}
	
	CImg<double> getImageData() const
	{
		return input_image;
	}
	
	void setNewDescriptors(vector<SiftDescriptor> NewDescriptors)
	{
		newDescriptors = NewDescriptors;
	}
	
	vector<SiftDescriptor> getNewDescriptors() const
	{
		return newDescriptors;
	}
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	static void DrawImage(const Image& queryImage,const Image& image,SiftDescriptorMap& mapping,bool reduced = false)
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
		if(!reduced)
			queryImageData.get_normalize(0,255).save("sift.png");
		else
			queryImageData.get_normalize(0,255).save("newsift.png");
	}
	
	static int MatchSIFT(const Image& queryImage,const Image& image,bool reduced = false)
	{
		vector<SiftDescriptor> queryDescriptors = queryImage.getDescriptors();
		vector<SiftDescriptor> imageDescriptors = image.getDescriptors();
		
		if(reduced)
		{
			queryDescriptors = queryImage.getNewDescriptors();
			imageDescriptors = image.getNewDescriptors();
		}
			
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
		Image::DrawImage(queryImage,image,mapping,reduced);
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
		cout<<"Matching Type 1"<<endl;
		while(rankingStart != rankingEnd && ++top10 <= 10)
		{
			cout<<"Image = "<<rankingStart->second<<" Count = "<<rankingStart->first<<endl;
			rankingStart++;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	
	static vector<vector<float>> getUniformDistribution(int k = 50)
	{
		default_random_engine gen;
		normal_distribution<float> x(0.0,1.0);
		
		vector<vector<float>> distributionLists;
		for(int i = 0;i<k;++i)
		{
			vector<float> distribution;
			for(int j = 0;j<128;++j)
			{
				distribution.push_back(x(gen));
			}
			distributionLists.push_back(distribution);
		}
		
		
		return distributionLists;
	}
	
	
	static void reduceSift(Image& image,vector<vector<float>>& distributionLists)
	{
		const vector<SiftDescriptor> imageDescriptors = image.getDescriptors();
		float w = 20;
		
		vector<SiftDescriptor> newDescriptors;
		
		for(int i = 0;i<imageDescriptors.size() ;++i)
		{
			vector<float> descriptorValues;
			for(int j = 0;j<distributionLists.size();++j)
			{
				float sum = 0.0;
				for(int k = 0;k<128;++k)
				{
					sum += imageDescriptors[i].descriptor[k] * distributionLists[j][k];
				}
				sum/=w;
				
				descriptorValues.push_back(sum);
			}
			SiftDescriptor newDescriptor(imageDescriptors[i].row,imageDescriptors[i].col,imageDescriptors[i].sigma,imageDescriptors[i].angle,descriptorValues);	
			newDescriptors.push_back(newDescriptor);
		}
		image.setNewDescriptors(newDescriptors);	
	}
	
	static void descriptorMatching2(Image& queryImage,vector<Image>& images)
	{
		vector<vector<float>> distributionLists = getUniformDistribution();
		
		reduceSift(queryImage,distributionLists);
		
		/*cout<<queryImage.getDescriptors().size()<<endl;
		cout<<queryImage.getNewDescriptors().size()<<endl;
		cout<<queryImage.getNewDescriptors()[0].size()<<endl;*/
		
		multimap<int,string> ranking;
		for(int i = 0;i<images.size();++i)
		{
			reduceSift(images[i],distributionLists);
			int count = MatchSIFT(queryImage,images[i],true);
			ranking.insert(make_pair(count,images[i].getName()));
		}
			
		multimap<int,string>::reverse_iterator rankingStart = ranking.rbegin();
		multimap<int,string>::reverse_iterator rankingEnd = ranking.rend();
			
		int top10 = 0;
		cout<<"Matching Type 2"<<endl;
		while(rankingStart != rankingEnd && ++top10 <= 10)
		{
			cout<<"Image = "<<rankingStart->second<<" Count = "<<rankingStart->first<<endl;
			rankingStart++;
		}
	}

	
};


#endif