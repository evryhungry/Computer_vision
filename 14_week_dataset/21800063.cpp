#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>

using namespace cv;
using namespace std;

Mat dct_quantize(const Mat& img, const Mat& quant_matrix) {
    Mat compressed_img = Mat::zeros(img.size(), CV_64F);

    for (int i = 0; i < img.rows; i += 8) {
        for (int j = 0; j < img.cols; j += 8) {
            Mat block = img(Rect(j, i, 8, 8));
            Mat dct_block, quant_block, dequant_block, idct_block;

            block.convertTo(block, CV_64F);
            dct(block - 128, dct_block);

            quant_block = dct_block / quant_matrix;
            quant_block.forEach<double>([](double& val, const int* position) -> void {
                val = std::round(val);  // 요소별로 round 적용
            });

            dequant_block = quant_block.mul(quant_matrix);

            idct(dequant_block, idct_block);
            idct_block += 128;

            idct_block.copyTo(compressed_img(Rect(j, i, 8, 8)));
        }
    }

    Mat result;
    compressed_img.convertTo(result, CV_8U);
    return result;
}

double calculate_psnr(const Mat& original, const Mat& compressed) {
    Mat diff;
    absdiff(original, compressed, diff);
    diff.convertTo(diff, CV_64F);
    diff = diff.mul(diff);

    double mse = sum(diff)[0] / (original.total());
    if (mse == 0) return INFINITY;

    double psnr = 10.0 * log10((255 * 255) / mse);
    return psnr;
}

int main(int argc, char* argv[]) {
    Mat image = imread("lena.png", IMREAD_COLOR);
    if (image.empty()) {
        cerr << "이미지를 로드할 수 없습니다!" << endl;
        return -1;
    }

    Mat image_Ycbcr, Ycbcr_channels[3];
    cvtColor(image, image_Ycbcr, COLOR_BGR2YCrCb);
    split(image_Ycbcr, Ycbcr_channels);
    Mat y_channel = Ycbcr_channels[0];

    Mat quantization_mat1 = (Mat_<double>(8, 8) <<
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 113, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99);
    Mat quantization_mat2 = (Mat_<double>(8, 8) <<
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1);
    Mat quantization_mat3 = (Mat_<double>(8, 8) <<
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100,
        100, 100, 100, 100, 100, 100, 100, 100);

    vector<Mat> quant_matrices = {quantization_mat1, quantization_mat2, quantization_mat3};
    vector<string> window_names = {"Original Y", "QM1", "QM2", "QM3"};
    vector<double> psnrs;

    // 원본 Y 채널 표시
    imshow("Original Y", y_channel);

    for (size_t i = 0; i < quant_matrices.size(); ++i) {
        Mat compressed_y = dct_quantize(y_channel, quant_matrices[i]);
        double psnr = calculate_psnr(y_channel, compressed_y);
        psnrs.push_back(psnr);

        imshow(window_names[i + 1], compressed_y);
        cout << "PSNR (" << window_names[i + 1] << "): " << fixed << setprecision(6) << psnr << " dB" << endl;
    }

    waitKey(0);
    return 0;
}
