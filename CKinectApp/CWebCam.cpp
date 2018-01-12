#include "CWebCam.h"

CWebCam::CWebCam(int i)
{
	cameraIndex = i;

	//ビデオデバイスを開く
	if (cap.isOpened()) cap.release();
	cap.open(cameraIndex);
	if(cap.isOpened())
	{
		std::cout << "cam(" << cameraIndex << ") is opened." << std::endl;
	}
	if (!cap.isOpened())
	{
		std::cerr << "camera(" << cameraIndex << ") not found!" << std::endl;
	}

	//画像のサイズを取得
	imageSize = cv::Size2d(
		cap.get(CV_CAP_PROP_FRAME_WIDTH),
		cap.get(CV_CAP_PROP_FRAME_HEIGHT));

	//FPSの設定。
	fps = cap.get(CV_CAP_PROP_FPS);
}

cv::Size2d CWebCam::getSize()
{
	return imageSize;
}

cv::Mat CWebCam::GetImage()
{
	//撮像ボタンを押したタイミングの画像を得るため。
	//cap.grab() , cap.retrieve() 一回だけでは、一回前に撮像した画像しか得られなかったため。

	cap >> bgrFrame;
	cap >> bgrFrame;
	return bgrFrame;
}

void::CWebCam::dispInfo()
{
	std::cout << "id: camera(" << cameraIndex << ")" << std::endl;
	std::cout << "size: " << imageSize << std::endl;
	std::cout << "fps: " << fps << std::endl;
}