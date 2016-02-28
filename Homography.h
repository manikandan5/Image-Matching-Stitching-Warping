#ifndef __HOMOGRAPHY_H__
#define __HOMOGRAPHY_H__

/*
	Part-2 helpers
*/
#include "SiftHelpers.h"
#include <dirent.h>

/*
	Given Image and projection the image is projected and saved.
*/
void projectiveTransform(const Image& I,CImg<double>& projection,const string& name)
{
	CImg<double> imageData = I.getImageData();
	
	CImg<double> transformedImage(imageData.width(),imageData.height(),imageData.depth(),3,255);
	
	projection.invert();
	
	
	double ai = projection(0,0) ,bi = projection(1,0), ci = projection(2,0), di = projection(0,1), ei = projection(1,1), 
	        fi = projection(2,1), gi = projection(0,2), hi = projection(1,2), ii = projection(2,2);
	
	// Inverse warping
	for(int i = 0;i<transformedImage.width();++i)
	{
		for(int j = 0;j<transformedImage.height();++j)
		{
			double w = (ai*i + bi*j + ci)/(gi*i + hi*j + ii);
			double h = (di*i + ei*j + fi)/(gi*i + hi*j + ii);
			
			//Bilinear Interpolation
			// Credit to wikipedia  = https://en.wikipedia.org/wiki/Bilinear_interpolation	
			int w0 = (int)floor(w);
			int h0 = (int)floor(h);
			int w1 = (int)ceil(w);
			int h1 = (int)ceil(h);
			
			
			if(w0 <0 || h0<0 || w1<0 || h1<0 || w0>=imageData.width() || w1>=imageData.width() || h0>=imageData.height() || h1>=imageData.height())
				continue;
			
			for(int k = 0;k<3;++k)
			{
				double v1 = imageData(w0,h0,0,k);
				double v2 = imageData(w1,h0,0,k);
				double v3 = imageData(w0,h1,0,k);
				double v4 = imageData(w1,h1,0,k);
				
				double Tw = (((w1 - w)/(w1-w0)) * v1) + (((w-w0)/(w1-w0)) * v2);
				double Th = (((w1 - w)/(w1-w0)) * v3) + (((w-w0)/(w1-w0)) * v4);

				double value = (((h1-h)/(h1-h0)) * Tw) + (((h-h0)/(h1-h0)) * Th);
				
				transformedImage(i,j,k)	= value;		
			}
		}
	}
	
	transformedImage.get_normalize(0,255).save(name.c_str());	
}

/*
	Given corresponding 4 points matching in each image estimating the projection matrix
*/
CImg<double> linearSystemSolver(SiftDescriptorMap& mapping)
{
	CImg<double> A(8,8);
	CImg<double> B(1,8);
	
	int i = 0;
	
	srand(time(NULL));
	// To pick different 4 points everytime
	random_shuffle(mapping.begin(),mapping.end());
	
	SiftDescriptorMap::iterator start = mapping.begin();
	SiftDescriptorMap::iterator end = mapping.end();
	//Fill A and B matrix from the class slide
	while(start != end)
	{
		A(0,2*i) = start->first.col;
		A(1,2*i) = start->first.row;
		A(2,2*i) = 1;
		A(3,2*i) = A(4,2*i) = A(5,2*i) = 0;
		A(6,2*i) = -1.0 * start->first.col * start->second.col;
		A(7,2*i) = -1.0 * start->first.row * start->second.col;
		
		A(3,2*i+1) = start->first.col;
		A(4,2*i+1) = start->first.row;
		A(5,2*i+1) = 1;
		A(0,2*i+1) = A(1,2*i+1) = A(2,2*i+1) = 0;
		A(6,2*i+1) = -1.0 * start->first.col * start->second.row;
		A(7,2*i+1) = -1.0 * start->first.row * start->second.row;
		
		B(0,2*i) = start->second.col;
		B(0,2*i+1) = start->second.row;
		
		if(++i == 4)//Stop for 4 ponts.
			break;
		++start;
	}
	
	//AX = B, estimate X
	CImg<double> X = B.solve(A);
	
	/*CImg<double>::iterator ansStart = X.begin();
	CImg<double>::iterator ansEnd = X.end();
	while(ansStart != ansEnd)
	{
		cout<<*ansStart<<" ";
		++ansStart;
	}
	cout<<endl;*/
	
	return X;
}

/*
	To get number of inliners in the given projection from the matching descriptors 
*/
int getInliners(CImg<double>& projection,const SiftDescriptorMap& mapping)
{
	int count = 0;
	
	//point in Second Image = [projection]* point in first 	
	for(int i = 0;i<mapping.size();++i)
	{
		double zz = projection(0,6) * mapping[i].first.col + projection(0,7) * mapping[i].first.row + 1;
		double xx = (projection(0,0) * mapping[i].first.col + projection(0,1) * mapping[i].first.row + projection(0,2))/zz;
		double yy = (projection(0,3) * mapping[i].first.col + projection(0,4) * mapping[i].first.row + projection(0,5))/zz;
		
		//cout<<i+1<<" x-x'= "<<xx-mapping[i].second.col<<" y-y' = "<<yy-mapping[i].second.row<<endl; 		

		if(abs(xx-mapping[i].second.col) <5.0 && abs(yy-mapping[i].second.row)<5.0)
			count++;
	}
	return count;
}

/*
	Helper to find the projection between two images I1 - query image
	I2 - projected image
*/
void getProjection(const Image& I1,const Image& I2,const string& name)
{	
	SiftDescriptorMap mapping;
	int count = Image::MatchSIFT(I1,I2,mapping);
	
	if(count <4)
	{
		cout<<" Image "<<name<<" cannot be formed due to insufficient Sift matchings"<<endl;
		return ;
	}
	
	assert(count >= 4);
	// We need 4 points to get Projection
	
	//Figure out to find N
	int N = 10000;
		
	int inliners = -1;
	CImg<double> bestProjection;
	for(int i = 0;i<N;++i)
	{
		CImg<double> projection = linearSystemSolver(mapping);
		int numberOfInliners = getInliners(projection,mapping);
		if(numberOfInliners > inliners)
		{
			inliners = numberOfInliners;
			bestProjection = projection;
			//cout<<inliners<<endl;
		}
	}
	
	assert(inliners > 0);		
	//cout<<"Inliners = "<<inliners<<" , size = "<<mapping.size()<<endl;
	
	//Converting 8 X 1 matrix to 3 X 3 matrix
	CImg<double> actualProjection(3,3);
	actualProjection(0,0) = bestProjection(0,0);
	actualProjection(1,0) = bestProjection(0,1);
	actualProjection(2,0) = bestProjection(0,2);
	actualProjection(0,1) = bestProjection(0,3);
	actualProjection(1,1) = bestProjection(0,4);
	actualProjection(2,1) = bestProjection(0,5);
	actualProjection(0,2) = bestProjection(0,6);
	actualProjection(1,2) = bestProjection(0,7);
	actualProjection(2,2) = 1;
		
	projectiveTransform(I1,actualProjection,name);
}

/*
	Driver function to execute part-2 completely
*/
void warpingApplication(const Image& queryImage,const vector<Image>& images)
{
	for(int i = 0;i<images.size();++i)
	{
		string name = string(basename(images[i].getName().c_str()));
		string ext = name.substr(name.find("."));
		string newName = name.substr(0, name.find("."))+ "-warped"+ext;
		getProjection(images[i],queryImage,newName);
	}
}


#endif
