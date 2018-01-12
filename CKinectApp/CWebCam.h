#ifndef CWEBCAM_H
#define CWEBCAM_H


#include <iostream>

#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)

class CWebCam
{
private:
	//<summary>何番目のビデオデバイスか(0始まり)</summary>
	int cameraIndex;

	//<summary>2D画像を得るためのビデオデバイス</summary>
	cv::VideoCapture cap;

	//<summary>BGR形式の画像の配列</summary>
	cv::Mat bgrFrame;

	//<summary>画像の幅[pixel]、高さ[pixel]</summary>
	cv::Size2d imageSize;

	//<summary>ビデオデバイスの1秒あたりのフレーム数</summary>
	double fps;

public:

	//<summary>コンストラクタ</summary>
	CWebCam(int i = 0);

	//<sumamry>デストラクタ</summary>
	~CWebCam() {};

	//<summary>カメラの撮影サイズ(幅、高さ)を得る</summary>
	cv::Size2d getSize();

	//<summary>2D画像を得る</summary>
	cv::Mat GetImage();

	//<summary>ビデオデバイスの情報を表示する</summary>
	void dispInfo();
};

#endif // !CWEBCAM_H
