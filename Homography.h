#ifndef __HOMOGRAPHY_H__
#define __HOMOGRAPHY_H__

#include "SiftHelpers.h"

double getAverage(CImg<double>& imageData,int w0,int h0)
{
	return (imageData(w0,h0,0,0) + imageData(w0,h0,0,1) + imageData(w0,h0,0,2))/3;
}

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


#endif
