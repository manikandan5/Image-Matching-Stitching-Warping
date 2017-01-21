# Image-Matching-Stitching-Warping

This code was written for the course Computer Vision (CSCI-B 657) at Indiana University handled by Professor David Crandall. Skeleton code was provided by the Professor to get us started with the assignment.


**What does the program do?** <br/>
* The program finds the SIFT points in the given image. These are the points of importance in the image.
* Then it tries to compare how similar two images are based on these SIFT points.
* It also tries to rank how similar the images in a dataset are to a given query image.
* Also, the program tries to project the given image in the same projection as another given image.

**How does it find it?** <br/>

Detailed explanation about how the code works and the reason why we chose this implementation could be found [here](https://github.com/manikandan5/Image-Matching-Stitching-Warping/blob/master/A2-Report.pdf).

**How to run the program?** 

This command compiles the program:
* make 

To compare how similar two images are, run the following command:
* ./a2 part1 image1.png image2.png <br>
This outputs a file called sift.png which holds the output.

To check how similar a dataset of images are compared to the given query image, run the command:
* ./a2 part1 query.png image1.png image2.png ... <br>
This outputs the list of images in the descending order based on the similarity to the query image.

To warp the given image into the same projection as a certain image, run the following command:
* ./a2 part2 image1.png projection.png
This outputs the warped image.
