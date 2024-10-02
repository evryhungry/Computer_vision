#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main() {
  Mat frame;
  VideoCapture cap("Road.mp4");

  if (!cap.isOpened()) {
        cout << "Error opening video stream or file" << endl;
        return -1;
    }

  int fps, delay, total_frames, current_frame;
    
  fps = cap.get(CAP_PROP_FPS);
  total_frames = fps * 15;
  current_frame = 0;
  delay = 1000 / fps;
  

  Mat gray, blur, canny, left_roi, right_roi;

  while(current_frame < total_frames){
    cap >> frame; // 프레임을 읽습니다.

    if (frame.empty())
      break;

    cvtColor(frame, gray, COLOR_BGR2GRAY);
    GaussianBlur(gray, blur, Size(5, 5), 5, 5, BORDER_DEFAULT);
    GaussianBlur(blur, blur, Size(5, 5), 5, 5, BORDER_DEFAULT);
    Canny(blur, canny, 10, 60);

    Mat left_roi = canny(Rect(200, 400, 400, 200));
    Mat right_roi = canny(Rect(600, 400, 400, 200));

    vector<Vec2f> left_lines;
    vector<Vec2f> right_lines;

    HoughLines(left_roi, left_lines, 1, CV_PI / 180, 50, 0, CV_PI);
    HoughLines(right_roi, right_lines, 1, CV_PI / 180, 50, 0, CV_PI);

    vector<float> left_rho, left_theta;
    for (size_t i = 0; i < left_lines.size(); i++) {
        float rho = left_lines[i][0];
        float theta = left_lines[i][1];
        if (theta >= CV_PI / 6 && theta <= CV_PI / 3) {
            left_rho.push_back(rho);
            left_theta.push_back(theta);
        }
    }

    vector<float> right_rho, right_theta;
    for (size_t i = 0; i < right_lines.size(); i++) {
        float rho = right_lines[i][0];
        float theta = right_lines[i][1];
        if (theta >= 2 * CV_PI / 3 && theta <= 5 * CV_PI / 6) {
            right_rho.push_back(rho);
            right_theta.push_back(theta);
        }
    }

    if (!left_rho.empty() && !left_theta.empty()) {
      float sum_rho = 0.0;
      float sum_theta = 0.0;

      for (size_t i = 0; i < left_rho.size(); i++) {
          sum_rho += left_rho[i];
          sum_theta += left_theta[i];
      }

      float avg_rho = sum_rho / left_rho.size();
      float avg_theta = sum_theta / left_theta.size();
      
      float a = cos(avg_theta);
      float b = sin(avg_theta);
      float x0 = a * avg_rho;
      float y0 = b * avg_rho;

      Point p1 = Point(cvRound(x0 + 1000 * (-b)) + 200, cvRound(y0 + 1000 * a) + 400);
      Point p2 = Point(cvRound(x0 - 1000 * (-b)) + 200, cvRound(y0 - 1000 * a) + 400);
    
      line(frame, p1, p2, Scalar(0, 0, 255), 3, 8);
    }
    
    if (!right_rho.empty() && !right_theta.empty()) {
      float sum_rho = 0.0;
      float sum_theta = 0.0;

      for (size_t i = 0; i < right_rho.size(); i++) {
          sum_rho += right_rho[i];
          sum_theta += right_theta[i];
      }

      float avg_rho = sum_rho / right_rho.size();
      float avg_theta = sum_theta / right_theta.size();
      
      float a = cos(avg_theta);
      float b = sin(avg_theta);
      float x0 = a * avg_rho;
      float y0 = b * avg_rho;

      Point p1 = Point(cvRound(x0 + 1000 * (-b)) + 600, cvRound(y0 + 1000 * a) + 400);
      Point p2 = Point(cvRound(x0 - 1000 * (-b)) + 600, cvRound(y0 - 1000 * a) + 400);
      
      line(frame, p1, p2, Scalar(0, 0, 255), 3, 8);
    }


    namedWindow("Left canny");
    moveWindow("Left canny", 200, 0);
    imshow("Left canny", left_roi);

    namedWindow("Right canny");
    moveWindow("Right canny", 600, 0);
    imshow("Right canny", right_roi);

    namedWindow("Frame");
    imshow("Frame", frame);

    waitKey(delay);
    current_frame++;
  }
}
