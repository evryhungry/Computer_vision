#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>

using namespace cv;
using namespace cv::xfeatures2d;
using namespace std;

int main() {
    Mat pano1 = imread("pano1.JPG");
    Mat pano2 = imread("pano2.JPG");

    if (pano1.empty() || pano2.empty()) {
        cout << "이미지를 불러올 수 없습니다!" << endl;
        return -1;
    }

    // SIFT 특징점 검출
    Ptr<SIFT> detector = SIFT::create();
    vector<KeyPoint> keypoints1, keypoints2;
    Mat descriptors1, descriptors2;
    detector->detectAndCompute(pano1, noArray(), keypoints1, descriptors1);
    detector->detectAndCompute(pano2, noArray(), keypoints2, descriptors2);

    // 매칭
    BFMatcher matcher(NORM_L2);
    vector<DMatch> matches;
    matcher.match(descriptors1, descriptors2, matches);

    // 상위 매칭 10개 선택
    sort(matches.begin(), matches.end(), [](const DMatch& a, const DMatch& b) {
        return a.distance < b.distance;
    });
    matches.resize(10);

    // 매칭 결과 출력
    cout << "=== 매칭된 특징점 (Top 10) ===" << endl;
    for (size_t i = 0; i < matches.size(); i++) {
        Point2f srcPt = keypoints1[matches[i].trainIdx].pt;
        Point2f dstPt = keypoints2[matches[i].queryIdx].pt;
        cout << "Match " << i + 1 << ": "
             << "pano1 (" << srcPt.x << ", " << srcPt.y << ") -> "
             << "pano2 (" << dstPt.x << ", " << dstPt.y << ")" << endl;
    }

    // 좌표 추출
    vector<Point2f> srcPoints, dstPoints;
    for (auto& match : matches) {
        srcPoints.push_back(keypoints1[match.trainIdx].pt);
        dstPoints.push_back(keypoints2[match.queryIdx].pt);
    }

    // 호모그래피 계산
    Mat H = findHomography(dstPoints, srcPoints, RANSAC);
    if (H.empty()) {
        cout << "호모그래피 계산 실패!" << endl;
        return -1;
    }

    // pano2를 pano1에 맞게 변환
    Mat warpedImg;
    warpPerspective(pano2, warpedImg, H, Size(pano1.cols + pano2.cols, pano1.rows));

    // 병합
    Mat panorama = warpedImg.clone();
    Mat roi(panorama, Rect(0, 0, pano1.cols, pano1.rows));
    pano1.copyTo(roi);

    // 결과 출력
    imwrite("panorama_result.jpg", panorama);
    imshow("Panorama", panorama);
    waitKey(0);

    return 0;
}
