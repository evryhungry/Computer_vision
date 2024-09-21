#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(){
	Mat image = imread("lena.png", 0);
	Mat gamma_img; 
	Mat rotatedImage;
	MatIterator_<uchar> it, end; 
	float gamma = 10; 
	unsigned char pix[256];
	

	rotate(image, rotatedImage, ROTATE_90_COUNTERCLOCKWISE);

	for (int i = 0; i < 256; i++) {
		pix[i] = saturate_cast<uchar>(pow((float)(i / 255.0), gamma) * 255.0f);
		// 
	}

	gamma_img = rotatedImage.clone();
	for (int j = 0; j < rotatedImage.rows; j++) 
		for (int i = 0; i < rotatedImage.cols; i++)
			if (gamma_img.at<uchar>(j, i) < 127){
				gamma_img.at<uchar>(j, i) = 255 - gamma_img.at<uchar>(j, i);
			} else {
				gamma_img.at<uchar>(j, i) = pix[gamma_img.at<uchar>(j, i)];
			}
			
	imshow("Gray image", image); 
	imshow("Result", gamma_img); 


	waitKey(0);
}
