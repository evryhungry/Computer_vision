// #include <opencv2/opencv.hpp>
// #include <opencv2/features2d.hpp>
// #include <iostream>
// #include <vector>

// using namespace cv;
// using namespace std;

// int main() {
//     vector<Mat> images = {
//         imread("pano1.JPG"),
//         imread("pano2.JPG"),
//         imread("pano3.JPG"),
//         imread("pano4.JPG")
//     };

//     if (images[0].empty() || images[1].empty() || images[2].empty() || images[3].empty()) {
//         cout << "이미지를 불러올 수 없습니다!" << endl;
//         return -1;
//     }

//     // 대비 조정 (명도 정규화)
//     for (size_t i = 0; i < images.size(); i++) {
//         normalize(images[i], images[i], 0, 255, NORM_MINMAX);  // 명도 정규화
//     }

//     // 첫 번째 이미지를 초기 파노라마로 설정
//     Mat panorama(Size(images[0].cols * images.size(), images[0].rows * 1.6), CV_8UC3, Scalar(0, 0, 0));
//     images[0].copyTo(panorama(Rect(0, 0, images[0].cols, images[0].rows)));

//     Ptr<ORB> orb = ORB::create();

//     for (size_t i = 1; i < images.size(); i++) {
//         vector<KeyPoint> keypoints1, keypoints2;
//         Mat descriptors1, descriptors2;

//         // 특징점 및 디스크립터 계산
//         orb->detectAndCompute(panorama, noArray(), keypoints1, descriptors1);
//         orb->detectAndCompute(images[i], noArray(), keypoints2, descriptors2);

//         // 매칭
//         BFMatcher matcher(NORM_HAMMING);
//         vector<DMatch> matches;
//         matcher.match(descriptors1, descriptors2, matches);

//         // 매칭 점 필터링
//         sort(matches.begin(), matches.end(), [](const DMatch& a, const DMatch& b) {
//             return a.distance < b.distance;
//         });
//         matches.resize(min(100, (int)matches.size()));  // 매칭 점 수를 100으로 증가

//         vector<Point2f> points1, points2;
//         for (const auto& match : matches) {
//             points1.push_back(keypoints1[match.queryIdx].pt);
//             points2.push_back(keypoints2[match.trainIdx].pt);
//         }

//         // 호모그래피 계산
//         Mat H = findHomography(points2, points1, RANSAC, 5.0);  // 허용 오차를 5픽셀로 설정
//         if (H.empty()) {
//             cout << "호모그래피 계산 실패!" << endl;
//             continue;
//         }

//         // 이미지 변환
//         Mat warpedImg;
//         warpPerspective(images[i], warpedImg, H, 
//                         Size(panorama.cols + images[i].cols, max(panorama.rows, images[i].rows)));

//         // 검은 영역 마스크 생성
//         Mat mask;
//         inRange(warpedImg, Scalar(0, 0, 0), Scalar(0, 0, 0), mask);
//         bitwise_not(mask, mask);

//         // 기존 파노라마와 변환된 이미지를 병합
//         Mat temp(panorama.rows, panorama.cols, CV_8UC3, Scalar(0, 0, 0));
//         panorama.copyTo(temp(Rect(0, 0, panorama.cols, panorama.rows))); // 기존 파노라마 복사
//         warpedImg.copyTo(temp(Rect(0, 0, warpedImg.cols, warpedImg.rows)), mask); // 마스크로 병합

//         panorama = temp.clone(); // 병합 결과 갱신
//     }

//     imshow("Panorama", panorama);
//     waitKey(0);

//     return 0;
// }

#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

int main() {
    vector<Mat> images = {
        imread("pano1.JPG"),
        imread("pano2.JPG"),
        imread("pano3.JPG"),
        imread("pano4.JPG")
    };

    if (images[0].empty() || images[1].empty() || images[2].empty() || images[3].empty()) {
        cout << "이미지를 불러올 수 없습니다!" << endl;
        return -1;
    }

    // 초기 파노라마 캔버스 크기 설정 (4096 * 4, 3000)
    Mat panorama(Size(images[0].cols * images.size(), images[0].rows * images.size()/2), CV_8UC3, Scalar(0, 0, 0));
    images[0].copyTo(panorama(Rect(0, 0, images[0].cols, images[0].rows))); // 첫 번째 이미지 복사

    Ptr<ORB> orb = ORB::create();

    for (size_t i = 1; i < images.size(); i++) {
        vector<KeyPoint> keypoints1, keypoints2;
        Mat descriptors1, descriptors2;

        // 특징점 및 디스크립터 계산
        orb->detectAndCompute(panorama, noArray(), keypoints1, descriptors1);
        orb->detectAndCompute(images[i], noArray(), keypoints2, descriptors2);

        // 매칭
        BFMatcher matcher(NORM_HAMMING);
        vector<DMatch> matches;
        matcher.match(descriptors1, descriptors2, matches);

        // 매칭 점 필터링
        sort(matches.begin(), matches.end(), [](const DMatch& a, const DMatch& b) {
            return a.distance < b.distance;
        });
        matches.resize(min(75, (int)matches.size()));  // 매칭 점 수 제한

        vector<Point2f> points1, points2;
        for (const auto& match : matches) {
            points1.push_back(keypoints1[match.queryIdx].pt);
            points2.push_back(keypoints2[match.trainIdx].pt);
        }

        // 호모그래피 계산
        Mat H = findHomography(points2, points1, RANSAC, 3.0);
        if (H.empty()) {
            cout << "호모그래피 계산 실패!" << endl;
            continue;
        }

        // 이미지 변환
        Mat warpedImg;
        warpPerspective(images[i], warpedImg, H, 
                        Size(panorama.cols + images[i].cols, panorama.rows));

        // 검은 영역 마스크 생성
        Mat mask;
        inRange(warpedImg, Scalar(0, 0, 0), Scalar(0, 0, 0), mask);
        bitwise_not(mask, mask); //확인 해보아야 할것

        // 병합
        for (int y = 0; y < panorama.rows; y++) {
            for (int x = 0; x < panorama.cols; x++) {
                if (mask.at<uchar>(y, x) > 0) {
                    panorama.at<Vec3b>(y, x) = warpedImg.at<Vec3b>(y, x);
                }
            }
        }
    }

    imshow("Panorama", panorama);
    waitKey(0);

    return 0;
}
