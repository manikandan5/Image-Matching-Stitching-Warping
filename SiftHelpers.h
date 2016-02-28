#ifndef __SIFTHELPERS_H__
#define __SIFTHELPERS_H__

/*
	Helper Class for part-1
	Contains Class Image - which constitutes all the attrubutes and static function tas helpers to each question
*/

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
#include <dirent.h>
#include <unistd.h>
#include <utility>

using namespace cimg_library;
using namespace std;

//Set of matching descriptors
typedef vector<pair<SiftDescriptor,SiftDescriptor>> SiftDescriptorMap;

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
	
	string getName() const
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
	// Static Helpers
	
	
	//Given two Images and descriptors between them , the image is joined and descriptors are drawn with yellow line
	// Reduced to distinguish between quantized projection function and the normal one.	
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
			
			for(int j=0; j<5; j++)
				for(int k=0; k<5; k++)
					if(j==2 || k==2)
						if(x0+k < queryImageData.width() && x1+k < queryImageData.width() && y0+j < queryImageData.height() && y1+j < queryImageData.height())
						{	
							queryImageData(x0+k, y0+j, 0, 0)=queryImageData(x1+k, y1+j, 0, 1)=255;
							queryImageData(x0+k, y0+j, 0, 1)= queryImageData(x0+k, y0+j, 0, 2) =queryImageData(x1+k, y1+j, 0, 0)=queryImageData(x1+k, y1+j, 0, 2)= 0;
						}
						
			queryImageData.draw_line(x0,y0,x1,y1,yellow);
			++mStart;
	
		}
		if(!reduced)
			queryImageData.get_normalize(0,255).save("sift.png");
		else
			queryImageData.get_normalize(0,255).save("newsift.png");
	}
	
	/*
		code compute matching descriptors of queryimage in given image
		mapping is out parameter.
		Reduced to distinguish between quantized projection function and the normal one.	
	*/
	static int MatchSIFT(const Image& queryImage,const Image& image,SiftDescriptorMap& mapping,bool reduced = false)
	{
		vector<SiftDescriptor> queryDescriptors = queryImage.getDescriptors();
		vector<SiftDescriptor> imageDescriptors = image.getDescriptors();
		
		if(reduced)
		{
			queryDescriptors = queryImage.getNewDescriptors();
			imageDescriptors = image.getNewDescriptors();
		}
		
		int count = 0;
			
		for(int i = 0;i<queryDescriptors.size();++i)
		{
			double min = DBL_MAX;
			double second_min = DBL_MAX;
			int descriptor = -1;
			for(int j =0;j<imageDescriptors.size();++j)
			{
				double sum = 0;
				//Euclidean Distance 
				for(int k = 0;k<imageDescriptors[j].descriptor.size();++k)
				{
					sum+= pow(queryDescriptors[i].descriptor[k] - imageDescriptors[j].descriptor[k],2);
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
			
			//ratio of the Euclidean distances between the closest match and the second-closest match, and comparing to a threshold.
			if(min/second_min < 0.7)
			{
				count++;
			
				SiftDescriptor q = queryDescriptors[i];
				SiftDescriptor im = imageDescriptors[descriptor];
				mapping.push_back(make_pair(q,im));
			}
			
		}
		
		//cout<<"Count = "<<count<<endl;
		Image::DrawImage(queryImage,image,mapping,reduced);
		
		return count;
	}
	
	
	/*
		Driver Function to execute the part1. 
	*/
	static void descriptorMatching1(Image& queryImage,vector<Image>& images)
	{
		multimap<int,string> ranking;
		for(int i = 0;i<images.size();++i)
		{
			SiftDescriptorMap mapping;
			int start = clock();
			int count = Image::MatchSIFT(queryImage,images[i],mapping);
			int stop = clock();
			//cout<<"Time consumption = "<<stop - start<<endl;
			ranking.insert(make_pair(count,images[i].getName()));
		}
			
		multimap<int,string>::reverse_iterator rankingStart = ranking.rbegin();
		multimap<int,string>::reverse_iterator rankingEnd = ranking.rend();
			
		int top10 = 0;
		cout<<"Normal Sift Matching top 10 results"<<endl;
		while(rankingStart != rankingEnd && ++top10 <= 10)
		{
			cout<<"Image = "<<rankingStart->second<<" Count = "<<rankingStart->first<<endl;
			rankingStart++;
		}
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//part2
	
	// Function to create uniform distribution k = is default
	static vector<vector<float>> getUniformDistribution(int k = 50)
	{
		default_random_engine gen;
		normal_distribution<float> x(0.0,1.0);
		
		vector<vector<float>> distributionLists;
		for(int i = 0;i<k;++i)
		{
			srand(time(NULL));
			vector<float> distribution;
			for(int j = 0;j<128;++j)
			{
				distribution.push_back(x(gen));
			}
			distributionLists.push_back(distribution);
		}
		
		
		return distributionLists;
	}
	
	/*
		applied quantized projection function for each descriptor to reduce the dimension ot the descriptor
		from 128 to k;
	*/
	static void reduceSift(Image& image,vector<vector<float>>& distributionLists)
	{
		const vector<SiftDescriptor> imageDescriptors = image.getDescriptors();
		//W is set
		float w = 400;
		
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
	
	/*
		Driver function for quantized projection function Sift matching basically reduction in computation time.
	*/
	static void descriptorMatching2(Image& queryImage,vector<Image>& images)
	{
		vector<vector<float>> distributionLists = getUniformDistribution();
		
		//reduction in dimension
		reduceSift(queryImage,distributionLists);
		
		multimap<int,string> ranking;
		for(int i = 0;i<images.size();++i)
		{
			reduceSift(images[i],distributionLists);
			SiftDescriptorMap mapping;
			int start = clock();
			int count = MatchSIFT(queryImage,images[i],mapping,true);
			int stop = clock();
			//cout<<"Time consumption = "<<stop - start<<endl;
			ranking.insert(make_pair(count,images[i].getName()));
		}
			
		multimap<int,string>::reverse_iterator rankingStart = ranking.rbegin();
		multimap<int,string>::reverse_iterator rankingEnd = ranking.rend();
			
		int top10 = 0;
		cout<<"quantized projection function Sift matching results top 10"<<endl;
		while(rankingStart != rankingEnd && ++top10 <= 10)
		{
			cout<<"Image = "<<rankingStart->second<<" Count = "<<rankingStart->first<<endl;
			rankingStart++;
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	/*
		Diriver function to take one image from each group and run the Sift match comparing to all 100 images given 
	*/
	static void randomRanking(bool reduced = false)
	{
		char buf1[1024];
		
		getcwd(buf1, sizeof(buf1));
		
		string directory = string(buf1) + "/part1_images/";
		DIR *dir;
		struct dirent *ent;
		vector<string> imageNames;
		int count = 0;
		if((dir = opendir(directory.c_str())) != nullptr)
		{
			while ((ent = readdir (dir)) != NULL) 
			{
				string fileName = ent->d_name;
				if(	fileName.find(".jpg") != string::npos)
				{
					string name = "part1_images/" + string(ent->d_name);
					imageNames.push_back(name);
				}
			}
			
			closedir(dir);
		}
		else
		{
			cout<<" Error in current directory"<<endl;
			return;
		}
		
		//Sort images based on names
		sort(imageNames.begin(),imageNames.end());
		vector<Image> images;
		for(int i = 0;i < imageNames.size();++i)
		{
			Image I(imageNames[i]);
			images.push_back(I);	
		}
		
		for(int i = 0;i<10;++i)
		{
			srand(time(NULL));
			//Pick random images one from each bin of size 10.
			int randomNumber = (10*i) + rand() % 10;
			
			Image queryImage = images[randomNumber];
			cout<<endl;
			cout<<"Query Image = "<<queryImage.getName()<<endl;
			if(!reduced)
				descriptorMatching1(queryImage,images);
			else
				descriptorMatching2(queryImage,images);
		}
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



#endif