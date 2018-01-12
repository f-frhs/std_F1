#include <iostream>
#include <afxwin.h>

#include <Kinect.h>
#include <atlbase.h>
#include <vector>

#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)

static int const FLIP_HORIZONTALLY = 1;

// �g����
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

	//�\������
	std::vector<BYTE> colorBuffer;

public:
	KinectApp()
	{
		// �f�t�H���g��Kinect���擾����
		ERROR_CHECK(::GetDefaultKinectSensor(&kinect));
		ERROR_CHECK(kinect->Open());

		//�J���[���[�_�[���擾����
		CComPtr<IColorFrameSource> colorFrameSource;
		ERROR_CHECK(kinect->get_ColorFrameSource(&colorFrameSource));
		ERROR_CHECK(colorFrameSource->OpenReader(&colorFrameReader));

		//�J���[�摜�̃T�C�Y���擾����
		CComPtr<IFrameDescription> colorFrameDescription;
		ERROR_CHECK(colorFrameSource->CreateFrameDescription(
			ColorImageFormat_Bgra, &colorFrameDescription));
		ERROR_CHECK(colorFrameDescription->get_Width(&colorWidth));
		ERROR_CHECK(colorFrameDescription->get_Height(&colorHeight));
		ERROR_CHECK(colorFrameDescription->get_BytesPerPixel(&colorBytesPerPixel));

		//�o�b�t�@���쐬����
		colorBuffer.resize(colorWidth*colorHeight*colorBytesPerPixel);
	}

	//2D�摜�𓾂�
	cv::Mat getImage()
	{
		updateColorFrame();

		// colorBuffer �� Mat�^�� rowImg �ɕϊ�
		cv::Mat rowImg(colorHeight, colorWidth, CV_8UC4, &colorBuffer[0]);

		// Kinect ��2D�J���[�摜�͍��E���΂Ȃ̂ŁA���]����
		cv::Mat resultImage;

		cv::flip(rowImg, resultImage, FLIP_HORIZONTALLY);
		return resultImage;
	}

	//�J���[�t���[�����X�V����
	void updateColorFrame()
	{
		//�t���[�����擾����
		CComPtr<IColorFrame> colorFrame;
		auto ret = colorFrameReader->AcquireLatestFrame(&colorFrame);
		if (FAILED(ret))
		{
			return;
		}

		//TODO: ���� Mat �`���ɂł��Ȃ���?
		//�w��̌`���Ńf�[�^���擾����
		ERROR_CHECK(colorFrame->CopyConvertedFrameDataToArray(
			colorBuffer.size(), &colorBuffer[0], colorFormat));
	}
};