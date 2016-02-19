// B657 assignment 2 skeleton code
//
// Compile with: "make"
//
// See assignment handout for command line and project specifications.


//Link to the header file
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

		//string inputFile = argv[2];
		string part = argv[1];
		
		if(part == "part1")
		{
			if(argc < 4)
			{
				cout << "Insufficent number of arguments; correct usage:" << endl;
				cout << "    ./a2 part1  query1.png img_1.png....." << endl;
				return -1;
			}			
			Image queryImage(argv[2]);
			vector<Image> images;
			for(int i = 3;i < argc;++i)
			{
				Image I(argv[i]);
				images.push_back(I);
			}
			
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
		else if(part == "part2")
		{
			Image I("lincoln.png");
			projectiveTransform(I);
		}
		else
		throw std::string("unknown part!");

    // feel free to add more conditions for other parts (e.g. more specific)
    //  parts, for debugging, etc.
	}catch(const string &err) 
	{
		cerr << "Error: " << err << endl;
	}
}








