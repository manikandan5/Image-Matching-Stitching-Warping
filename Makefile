all: CImg.h a2.cpp SiftHelpers.h Homography.h
	g++ -g a2.cpp -o a2 -lX11 -lpthread -I. -Isiftpp -O3 siftpp/sift.cpp -std=c++11

clean:
	rm a2
