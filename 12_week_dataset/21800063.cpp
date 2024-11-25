#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;

struct MouseParams
{
  Mat img;
  vector<Point2f> in, out;
};

static void onMouse(int event, int x, int y, int, void* param)
{
  MouseParams* mp = (MouseParams*)param;

  if (event == EVENT_LBUTTONDOWN)
  {
    mp->in.push_back(Point2f(x, y));
  }

  if (event == EVENT_RBUTTONDOWN)
  {
    mp->in.clear();
  }
}

int main()
{
  VideoCapture background("Timesquare.mp4");
  VideoCapture overlay("contest.mp4");

  if (!background.isOpened() || !overlay.isOpened())
  {
    cerr << "비디오 파일을 여는 데 문제가 발생했습니다!" << endl;
    return -1;
  }

  int overlayWidth = overlay.get(CAP_PROP_FRAME_WIDTH);
  int overlayHeight = overlay.get(CAP_PROP_FRAME_HEIGHT);

  MouseParams mp;
  mp.out.push_back(Point2f(0, 0));
  mp.out.push_back(Point2f(overlayWidth, 0));
  mp.out.push_back(Point2f(overlayWidth, overlayHeight));
  mp.out.push_back(Point2f(0, overlayHeight));

  Mat frame;
  background >> frame;
  mp.img = frame.clone();

  imshow("background", mp.img);
  setMouseCallback("background", onMouse, (void*)&mp);

  while (true)
  {
    background >> frame; 
    if (frame.empty())
      break;

    Mat tempFrame = frame.clone();
    for (size_t i = 0; i < mp.in.size(); i++)
    {
      circle(tempFrame, mp.in[i], 5, Scalar(0, 0, 255), -1);
    }

    Mat overlayFrame;
    overlay >> overlayFrame;
    if (overlayFrame.empty())
    {
      overlay.set(CAP_PROP_POS_FRAMES, 0);
      overlay >> overlayFrame;
    }

    imshow("input", overlayFrame);

    if (mp.in.size() == 4)
    {
      Mat homo_mat = getPerspectiveTransform(mp.out, mp.in);
      Mat transformedOverlay;
      warpPerspective(overlayFrame, transformedOverlay, homo_mat, frame.size(), INTER_LINEAR, BORDER_CONSTANT);

      Mat mask;
      cvtColor(transformedOverlay, mask, COLOR_BGR2GRAY);
      threshold(mask, mask, 1, 255, THRESH_BINARY);

      for (int y = 0; y < frame.rows; y++) {
        for (int x = 0; x < frame.cols; x++) {
          if (mask.at<uchar>(y, x) > 0) {
            tempFrame.at<Vec3b>(y, x) = transformedOverlay.at<Vec3b>(y, x);
          }
        }
      }
    }

    imshow("background", tempFrame);

    if (waitKey(30) == 27)
    {
      break;
    }
  }

  background.release();
  overlay.release();
  destroyAllWindows();
  return 0;
}
