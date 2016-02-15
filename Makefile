all: CImg.h a2.cpp SiftHelpers.h
	g++ -g a2.cpp -o a2 -lX11 -lpthread -I. -Isiftpp -O3 siftpp/sift.cpp 

clean:
	rm a2
