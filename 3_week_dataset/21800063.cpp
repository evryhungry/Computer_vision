#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
  Mat image, sharpening;
  image = imread("Moon.png",0);

  if (image.empty()) {
      cerr << "Error: Could not open or find the image" << endl;
      return -1;
  }


  int half_col = image.cols / 2; 
  Mat left_moon = image(Rect(0, 0, half_col, image.rows));
  Mat right_moon = image(Rect(half_col, 0, half_col, image.rows));

  GaussianBlur(right_moon, right_moon, Size(3, 3), 0, 0, BORDER_DEFAULT);

  hconcat(left_moon, right_moon, sharpening);

  imshow("Moon", image);
  imshow("Moon_filtered", sharpening);

  Mat saltnpepper, equalized_saltnpepper, result, mf2;
  saltnpepper = imread("saltnpepper.png", 0);

  if (saltnpepper.empty()) {
      cerr << "Error: Could not open or find the image" << endl;
      return -1;
  }



  int half_width = saltnpepper.cols / 2; 
  Mat left_saltnpepper = saltnpepper(Rect(0, 0, half_width, saltnpepper.rows));\
  Mat right_saltnpepper = saltnpepper(Rect(half_width, 0, half_width, saltnpepper.rows));
  
  medianBlur(left_saltnpepper, mf2, 9);
  equalizeHist(mf2, equalized_saltnpepper);

  hconcat(equalized_saltnpepper, right_saltnpepper, result);

  imshow("saltnpepper",saltnpepper);
  imshow("saltnpepper_filtered", result);
  waitKey(0);
}