#include <iostream>
#include <afxwin.h>

#include <Kinect.h>
#include <atlbase.h>
#include <vector>

#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)

static int const FLIP_HORIZONTALLY = 1;

// 使い方
// ERROR_CHECK( ::GetDefaultkinectSensor(&kinect))

#define ERROR_CHECK(ret)  \
	if((ret) != S_OK) { \
		std::stringstream ss; \
		ss << "failed " #ret " " << std::hex << ret << std::endl; \
		throw std::runtime_error(ss.str().c_str()); \
}

class KinectApp
{
	// Kinect SDK
	CComPtr<IKinectSensor> kinect = nullptr;

	CComPtr<IColorFrameReader> colorFrameReader = nullptr;

	int colorWidth;
	int colorHeight;
	unsigned int colorBytesPerPixel;
	ColorImageFormat colorFormat = ColorImageFormat::ColorImageFormat_Bgra;

	//表示部分
	std::vector<BYTE> colorBuffer;

public:
	KinectApp()
	{
		// デフォルトのKinectを取得する
		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
		ERROR_CHECK(kinect->Open());

		//カラーリーダーを取得する
		CComPtr<IColorFrameSource> colorFrameSource;
		ERROR_CHECK(kinect->get_ColorFrameSource(&colorFrameSource));
		ERROR_CHECK(colorFrameSource->OpenReader(&colorFrameReader));

		//カラー画像のサイズを取得する
		CComPtr<IFrameDescription> colorFrameDescription;
		ERROR_CHECK(colorFrameSource->CreateFrameDescription(
			ColorImageFormat_Bgra, &colorFrameDescription));
		ERROR_CHECK(colorFrameDescription->get_Width(&colorWidth));
		ERROR_CHECK(colorFrameDescription->get_Height(&colorHeight));
		ERROR_CHECK(colorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));

		//バッファを作成する
		colorBuffer.resize(colorWidth*colorHeight*colorBytesPerPixel);
	}

	//2D画像を得る
	cv::Mat getImage()
	{
		updateColorFrame();

		// colorBuffer を Mat型の rowImg に変換
		cv::Mat rowImg(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);

		// Kinect の2Dカラー画像は左右反対なので、反転する
		cv::Mat resultImage;

		cv::flip(rowImg, resultImage, FLIP_HORIZONTALLY);
		return resultImage;
	}

	//カラーフレームを更新する
	void updateColorFrame()
	{
		//フレームを取得する
		CComPtr<IColorFrame> colorFrame;
		auto ret = colorFrameReader->AcquireLatestFrame(&colorFrame);
		if (FAILED(ret))
		{
			return;
		}

		//TODO: 直接 Mat 形式にできないか?
		//指定の形式でデータを取得する
		ERROR_CHECK(colorFrame->CopyConvertedFrameDataToArray(
			colorBuffer.size(), &colorBuffer[0], colorFormat));
	}
};