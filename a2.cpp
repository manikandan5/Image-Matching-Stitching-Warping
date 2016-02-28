// B657 assignment 2 skeleton code
//
// Compile with: "make"
//
// See assignment handout for command line and project specifications.


/* How to run the application - part1
 1) Comapre Sift between two Images : 
		./a2 part1 query.png img_1.png 
		
		Output in "sift.png"
		
		./a2 part1fast query.png img_1.png 
		This will create "newsift.png" for quantized projection function of the Sift Descriptor. 
		
 2) Normal Sift Match between Query Image and and its warped Image
        ./a2 part1 query.png img_1.png img_2.png ... img_n.png
			
		This will rank 	img_1.png img_2.png ... img_n.png to 10 rankings with respect to query.png
		The output is followed by "Normal Sift Matching top 10 results"
		
		./a2 part1fast query.png img_1.png 
		This will rank for quantized projection function of the Sift Descriptor.	
		The output is followed by "quantized projection function Sift matching results top 10"
 
 3) to do the ranking picking each one from each group and ranking them in order. Just do
		./a2 part1 
		./a2 part1fast
		
*/			  

/* How to run part2

1) ./a2 part2 
	Gives the tarnsformation of Lincoln.png with the given projection in the assigment. Saved in lincoln_transform_png
	
2) ./a2 part2 img_1.png img_2.png ... img_n.png

		Gives the Warped images named as img 2-warped.png, img 3-warped.png,
		img_1.png corresponds to img_1-warped.png and so on
*/
#include "SiftHelpers.h"
#include "Homography.h"

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
		
		if(part == "part1")
		{
			if(argc < 4)
			{
				//Ranking of each image from a group chosen randomly.
				Image::randomRanking();
				return -1;
			}			
			Image queryImage(argv[2]);
			vector<Image> images;
			for(int i = 3;i < argc;++i)
			{
				Image I(argv[i]);
				images.push_back(I);
			}
			
			//1 - ranking of all the images with respect to query images
			Image::descriptorMatching1(queryImage,images);
						
				
		}
		else if(part == "part1fast")
		{
			if(argc < 4)
			{
				//Ranking of each image from a group chosen randomly.
				Image::randomRanking(true);
				return -1;
			}
			
			Image queryImage(argv[2]);
			vector<Image> images;
			for(int i = 3;i < argc;++i)
			{
				Image I(argv[i]);
				images.push_back(I);
			}
			
			//3 - ranking of all the images with respect to query images - quantized projection function
			Image::descriptorMatching2(queryImage,images);
		}
		else if(part == "part2")
		{
			if(argc < 4)
			{
				//Lincoln Transformation
				Image I("lincoln.png");
				CImg<double> projection(3,3);
				projection(0,0) = 0.907;
				projection(1,0) = 0.258;
				projection(2,0) = -182;
				projection(0,1) = -0.153;
				projection(1,1) = 1.44;
				projection(2,1) = 58;
				projection(0,2) = -0.000306;
				projection(1,2) = 0.000731;
				projection(2,2) = 1;
			
				string lincolnName = "lincoln_transform.jpg";
				projectiveTransform(I,projection,lincolnName);
				
				return -1;
			}
			
			// Warping of query image in terms of all other images
			Image queryImage(argv[2]);
			vector<Image> images;
			for(int i = 3;i < argc;++i)
			{
				Image I(argv[i]);
				images.push_back(I);
			}
			
			warpingApplication(queryImage,images);
		}
		else
		throw std::string("unknown part!");

	}catch(const string &err) 
	{
		cerr << "Error: " << err << endl;
	}
}








