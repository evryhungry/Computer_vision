#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

vector<Point2f> points; // 추출된 점을 저장할 벡터

void onMouse(int event, int x, int y, int flags, void* userdata) {
    if (event == EVENT_LBUTTONDOWN) {
        points.push_back(Point2f(x, y)); // 클릭한 좌표 저장
        cout << "Point: (" << x << ", " << y << ")" << endl;
    }
}

int main() {
    Mat img1 = imread("pano2.JPG");
    if (img1.empty()) {
        cout << "이미지를 불러올 수 없습니다!" << endl;
        return -1;
    }

    namedWindow("Select Points", WINDOW_AUTOSIZE);
    setMouseCallback("Select Points", onMouse, nullptr);

    cout << "이미지에서 좌표를 클릭하세요 (ESC를 누르면 종료)." << endl;
    while (true) {
        imshow("Select Points", img1);
        if (waitKey(1) == 27) break; // ESC 키로 종료
    }

    // 추출된 점 출력
    for (size_t i = 0; i < points.size(); i++) {
        cout << "Point " << i + 1 << ": (" << points[i].x << ", " << points[i].y << ")" << endl;
    }

    return 0;
}

// pano1 : (2740, 0)
// pano2 : (1880, 0) 1 , (3195, 0) 3
// pano3 : (2616, 0) (xx, 3024) 2 , (3820, 0) , (3842, 3024)
// pano4 : (3160, 0)