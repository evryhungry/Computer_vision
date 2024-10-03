#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(){
  Mat adaptive_1, adaptive;
  Mat finger_print = imread("finger_print.png");
  cvtColor(finger_print, finger_print, COLOR_BGR2GRAY);

  threshold(finger_print, finger_print, 150, 255, THRESH_BINARY);

  imshow("finger_print", finger_print);


  adaptive = imread("adaptive.png");
  adaptive_1 = imread("adaptive_1.jpg");

  cvtColor(adaptive, adaptive, COLOR_BGR2GRAY);
  cvtColor(adaptive_1, adaptive_1, COLOR_BGR2GRAY);

  adaptiveThreshold(adaptive, adaptive, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 7, 10);
  adaptiveThreshold(adaptive_1, adaptive_1, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 85, 15);

  imshow("adaptive", adaptive);
  imshow("adaptive_1", adaptive_1);

  waitKey(0);
  return 0;
}
