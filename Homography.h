#ifndef __HOMOGRAPHY_H__
#define __HOMOGRAPHY_H__

#include "SiftHelpers.h"


void projectiveTransform(Image& I)
{
	CImg<double> imageData = I.getImageData();
	
	CImg<double> transformedImage(imageData.width(),imageData.height(),imageData.depth(),3,255);
	
	double a = 0.907 ,b = 0.258, c= -182, d = -0.153, e = 1.44, f = 58, g= -0.000306, h = 0.000731, i0 = 1;
	
	// Doing inverse warping
	
	//calcualted inverse from calculator
	double ai = 1.1246685805361205 ,bi = -0.3146766039759572, ci = 222.9409246881795, di = 0.10883905064150695, ei = 0.6850586647407801, 
	        fi = -19.92469533821099, gi = 0.0002645872396251113, hi = -0.0005970689247421533, ii = 1.0827848752468152;
	
	for(int i = 0;i<transformedImage.width();++i)
	{
		for(int j = 0;j<transformedImage.height();++j)
		{
			double w = (ai*i + bi*j + ci)/(gi*i + hi*j + ii);
			double h = (di*i + ei*j + fi)/(gi*i + hi*j + ii);
			
			//Bilinear Interpolation
			
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
	
	transformedImage.get_normalize(0,255).save("transform.png");	
	
}

CImg<double> linearSystemSolver(SiftDescriptorMap& mapping)
{
	CImg<double> A(8,8);
	CImg<double> B(1,8);
	
	int i = 0;
	random_shuffle(mapping.begin(),mapping.end());
	
	SiftDescriptorMap::iterator start = mapping.begin();
	SiftDescriptorMap::iterator end = mapping.end();
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
		
		if(++i == 4)
			break;
		++start;
	}
	
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

void getProjection(Image I1,Image I2)
{	
	SiftDescriptorMap mapping;
	int count = Image::MatchSIFT(I1,I2,mapping);
	
	assert(count >= 4);
	// We need 4 points to get Projection
	
	//Figure out to find N
	int N = 10;
	for(int i = 0;i<N;++i)
	{
		CImg<double> projection = linearSystemSolver(mapping);
	}
}


#endif
