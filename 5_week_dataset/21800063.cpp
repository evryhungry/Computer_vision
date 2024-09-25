#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
  Mat frame, originalFrame;
  VideoCapture cap("video.mp4");

  if (!cap.isOpened()) {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

  int fps, delay;
    
  fps = cap.get(CAP_PROP_FPS);
  delay = 1000 / fps;
  

  Mat frame, gray, blur, canny, blur_left, blur_right, canny_left, canny_right;

  while(1){
    cap >> frame; // 프레임을 읽습니다.

    if (frame.empty())
      break;

    cvtColor(frame, gray, CV_BGR2GRAY);
    GaussianBlur(gray, blur, Size(5, 5), 5, 5, BORDER_DEFAULT);
    Canny(blur, canny, 10, 60);

    Mat canny_left = canny(Rect(200, 400, 400, 200));
    Mat canny_right = canny(Rect(600, 400, 400, 200));

    namedWindow("Left canny");
    moveWindow("Left canny", 200, 0);
    imshow("Left canny", canny_left);

    namedWindow("Right canny", WINDOW_NORMAL);
    moveWindow("Right canny", 600, 0);
    imshow("Right canny", canny_right);
  

    waitKey(20);
  }
}