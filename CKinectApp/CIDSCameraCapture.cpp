
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#include "opencv2\opencv.hpp"
#include "uEye.h"
#include "CIDSCameraCapture.h"
#include <stdexcept>
//#include <cereal/archives/xml.hpp>
#include <bitset>
#include <debugapi.h>
#include <future>
#include <ctime>
#include "../CKinectApp/CUtil.h"

#define STR(var) #var

//cerealにより、データをシリアライズ化
//http://uscilab.github.io/cereal/index.html

namespace
{

}

///<summary></summary>
CIDSCameraCaputure::CIDSCameraCaputure()
{

}

///<summary></summary>
CIDSCameraCaputure::CIDSCameraCaputure(int camNum, bool useDeviceID)
{

}

/// <summary>フラッシュの設定</summary>
/// <returns></returns>
bool CIDSCameraCaputure::SetFlash(bool flashFlg)
{
	IO_FLASH_PARAMS flashParams;

	std::stringstream ssFlash;	//取得情報をデバッグ出力用

	INT ansRet = IS_SUCCESS;

	//フラッシュ設定関係を取得し、Trace出力する

	//フラッシュに使用できるGPIOを取得する
	UINT nGpioFlash = 0;
	ansRet = is_IO(hCam, IS_IO_CMD_FLASH_GET_SUPPORTED_GPIOS, (void*)&nGpioFlash, sizeof nGpioFlash);
	ssFlash << STR(IS_IO_CMD_FLASH_GET_SUPPORTED_GPIOS) << " " << nGpioFlash << std::endl;
	//OutputDebugString(ssFlash.str().c_str());
	OutputDebugString(CUtil::StringToWString(ssFlash.str()).c_str());
	ssFlash.str("");
	ssFlash.clear(std::stringstream::goodbit);

	//PWMに使用可能な全てのGPIOを取得する
	UINT nGpioPwm = 0;
	ansRet = is_IO(hCam, IS_IO_CMD_PWM_GET_SUPPORTED_GPIOS, (void*)&nGpioPwm, sizeof nGpioPwm);
	ssFlash << STR(IS_IO_CMD_PWM_GET_SUPPORTED_GPIOS) << " " << nGpioPwm << std::endl;
	//OutputDebugString(ssFlash.str().c_str());
	OutputDebugString(CUtil::StringToWString(ssFlash.str()).c_str());
	ssFlash.str("");
	ssFlash.clear(std::stringstream::goodbit);

	//グローバル露光ウィンドウのパラメータを取得する
	INT nDelay = 0;
	UINT nDuration = 0;
	ansRet = is_IO(hCam, IS_IO_CMD_FLASH_GET_GLOBAL_PARAMS, (void*)&flashParams, sizeof flashParams);
	if (ansRet == IS_SUCCESS)
	{
		nDelay = flashParams.s32Delay;
		nDuration = flashParams.u32Duration;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_GLOBAL_PARAMS) << ":Delay" << " " << nDelay << std::endl;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_GLOBAL_PARAMS) << ":Duration" << " " << nDuration << std::endl;
		//OutputDebugString(ssFlash.str().c_str());
		OutputDebugString(CUtil::StringToWString(ssFlash.str()).c_str());
		ssFlash.str("");
		ssFlash.clear(std::stringstream::goodbit);
	}

	//フラッシュ遅延とフラッシュ継続時間の最小可能値を取得する
	INT nFlashDelayMin = 0;
	UINT nFlashDurationMin = 0;
	ansRet = is_IO(hCam, IS_IO_CMD_FLASH_GET_PARAMS_MIN, (void*)&flashParams, sizeof flashParams);
	if (ansRet == IS_SUCCESS)
	{
		nFlashDelayMin = flashParams.s32Delay;
		nFlashDurationMin = flashParams.u32Duration;

		ssFlash << STR(IS_IO_CMD_FLASH_GET_PARAMS_MIN) << ":Delay" << " " << nFlashDelayMin << std::endl;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_PARAMS_MIN) << ":Duration" << " " << nFlashDurationMin << std::endl;
		//OutputDebugString(ssFlash.str().c_str());
		OutputDebugString(CUtil::StringToWString(ssFlash.str()).c_str());
		ssFlash.str("");
		ssFlash.clear(std::stringstream::goodbit);
	}

	//フラッシュ遅延とフラッシュ継続時間の最大可能値を取得する
	INT nFlashDelayMax = 0;
	UINT nFlashDurationMax = 0;
	ansRet = is_IO(hCam, IS_IO_CMD_FLASH_GET_PARAMS_MAX, (void*)&flashParams, sizeof flashParams);
	if (ansRet == IS_SUCCESS)
	{
		nFlashDelayMax = flashParams.s32Delay;
		nFlashDurationMax = flashParams.u32Duration;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_PARAMS_MAX) << ":Delay" << " " << nFlashDelayMax << std::endl;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_PARAMS_MAX) << ":Duration" << " " << nFlashDurationMax << std::endl;
		//OutputDebugString(ssFlash.str().c_str());
		OutputDebugString(CUtil::StringToWString(ssFlash.str()).c_str());
		ssFlash.str("");
		ssFlash.clear(std::stringstream::goodbit);
	}

	//フラッシュ遅延とフラッシュ継続時間のインクリメントを取得する
	INT nFlashDelayInc = 0;
	UINT nFlashDurationInc = 0;
	ansRet = is_IO(hCam, IS_IO_CMD_FLASH_GET_PARAMS_INC, (void*)&flashParams, sizeof flashParams);
	if (ansRet == IS_SUCCESS)
	{
		nFlashDelayInc = flashParams.s32Delay;
		nFlashDurationInc = flashParams.u32Duration;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_PARAMS_INC) << ":Delay" << " " << nFlashDelayInc << std::endl;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_PARAMS_INC) << ":Duration" << " " << nFlashDurationInc << std::endl;
		//OutputDebugString(ssFlash.str().c_str());
		OutputDebugString(CUtil::StringToWString(ssFlash.str()).c_str());
		ssFlash.str("");
		ssFlash.clear(std::stringstream::goodbit);
	}

	//フラッシュ遅延とフラッシュ継続時間の現在値を取得する
	INT nFlashDelayCurrent = 0;
	UINT nFlashDurationCurrent = 0;
	ansRet = is_IO(hCam, IS_IO_CMD_FLASH_GET_PARAMS, (void*)&flashParams, sizeof flashParams);
	if (ansRet == IS_SUCCESS)
	{
		nFlashDelayCurrent = flashParams.s32Delay;
		nFlashDurationCurrent = flashParams.u32Duration;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_PARAMS) << ":Delay" << " " << nFlashDelayCurrent << std::endl;
		ssFlash << STR(IS_IO_CMD_FLASH_GET_PARAMS) << ":Duration" << " " << nFlashDurationCurrent << std::endl;
		//OutputDebugString(ssFlash.str().c_str());
		OutputDebugString(CUtil::StringToWString(ssFlash.str()).c_str());
		ssFlash.str("");
		ssFlash.clear(std::stringstream::goodbit);
	}

	//現在のフラッシュモードを取得する
	UINT nFlashModeCurrent = IO_FLASH_MODE_OFF;
	ansRet = is_IO(hCam, IS_IO_CMD_FLASH_GET_MODE, (void*)nFlashModeCurrent, sizeof nFlashModeCurrent);
	if (ansRet == IS_SUCCESS)
	{
		ssFlash << STR(IS_IO_CMD_FLASH_GET_MODE) << " " << nFlashModeCurrent << std::endl;
		//OutputDebugString(ssFlash.str().c_str());
		OutputDebugString(CUtil::StringToWString(ssFlash.str()).c_str());
		ssFlash.str("");
		ssFlash.clear(std::stringstream::goodbit);
	}

	//std::stringstream ss;
	//{
	//	CameraCaputureData cd;
	//	cereal::XMLOutputArchive oa(ss);
	//	oa(cd);

	//}

	return true;
}

/// <summary>カメラの情報を取得する</summary>
CAMINFO CIDSCameraCaputure::GetCameraInfo()
{
	//出力をboolにして、CAMINFOを&で返すことを検討する

	CAMINFO info;

	auto ansRet = is_GetCameraInfo(hCam, &info);
	if (ansRet == IS_SUCCESS)
	{
		//カメラ情報をTrace出力する
		std::stringstream ss;
		ss << "  SerNo:" << info.SerNo << std::endl;
		ss << "     ID:" << info.ID << std::endl;
		ss << "Version:" << info.Version << std::endl;
		ss << "   Date:" << info.Date << std::endl;
		ss << " Select:" << info.Select << std::endl;
		ss << "   Type:" << info.Type << std::endl;
		//OutputDebugString(ss.str().c_str());
		//OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
	}
	else
	{
		std::stringstream ss;
		ss << "GetCameraInfoが正常に終了しませんでした. code=" << ansRet << std::endl;
		//OutputDebugString(ss.str().c_str());
		OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
	}

	return info;
}

/// <summary>カメラ情報の文字列を取得する</summary>
/// <returns>カメラ情報の文字列</returns>
std::string CIDSCameraCaputure::GetCameraInfoStr()
{
	auto camInfo = GetCameraInfo();

	std::stringstream ss;
	ss << "  SerNo:" << camInfo.SerNo << ',';
	ss << "     ID:" << camInfo.ID << ',';
	ss << "Version:" << camInfo.Version << ',';
	ss << "   Date:" << camInfo.Date << ',';
	ss << " Select:" << camInfo.Select << ',';
	ss << "   Type:" << camInfo.Type << std::endl;

	//OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	return ss.str();
}


bool CIDSCameraCaputure::GetCameraStatus()
{
	//カメラのステータスを取得する
	CameraStatus camStatus;

	INT nInfo = 0;
	UINT ulValue = IS_GET_STATUS;

	//取得するステータス情報
	std::vector<INT> paramList = {
		IS_FIFO_OVR_CNT,
		IS_SEQUENCE_CNT,
		IS_SEQUENCE_SIZE,
		IS_EXT_TRIGGER_EVENT_CNT,
		IS_TRIGGER_MISSED,
		IS_LAST_CAPTURE_ERROR,
		IS_PARAMETER_EXT,
		IS_PARAMETER_SET_1,
		IS_PARAMETER_SET_2,
		IS_STANDBY,
		IS_STANDBY_SUPPORTED,
	};

	for (auto cnt : paramList)
	{
		auto ansRet = is_CameraStatus(hCam, cnt, ulValue);
		switch (cnt)
		{
		case IS_FIFO_OVR_CNT:
			camStatus.FifoOverCnt = ansRet;
			break;
		case IS_SEQUENCE_CNT:
			camStatus.SequenceCnt = ansRet;
			break;
		case IS_SEQUENCE_SIZE:
			camStatus.SequenceSize = ansRet;
			break;
		case IS_EXT_TRIGGER_EVENT_CNT:
			camStatus.ExtTriggerEventCnt = ansRet;
			break;
		}
	}

	return true;
}



///<summary>カメラから画像を取得する</summary>
bool CIDSCameraCaputure::StartSingleCapture()
{
	bool retAns = false;

	//カメラから画像をキャプチャするときに、スレッド化を検討すること
	auto retFunc = is_SetExternalTrigger(hCam, IS_SET_TRIGGER_SOFTWARE);

	//キャプチャ用のスレッドを起動する
	std::future<bool> f1 = std::async(std::launch::async, &CIDSCameraCaputure::SingleCaptureWork, this);

	return retAns;
}

/// <summary>非同期で撮影を行なうスレッド関数 </summary>
/// <returns></returns>
bool CIDSCameraCaputure::SingleCaptureWork(void)
{
	//イベントを使用して、画像取得完了の通知を受け取る
	std::stringstream ss;
	ss << "Start SingleCapureWork" << std::endl;
	//OutputDebugString(ss.str().c_str());
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	bool ans = false;

	//イベントの作成
	camEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	is_InitEvent(hCam, camEvent, IS_SET_EVENT_FRAME);

	is_EnableEvent(hCam, IS_SET_EVENT_FRAME);
	auto retFunc = is_FreezeVideo(hCam, IS_DONT_WAIT);	//キャプチャ開始

	auto retEvent = WaitForSingleObject(camEvent, 1000);
	if (retEvent == WAIT_TIMEOUT)
	{
		//イベントのタイムアウト処理
		return false;
	}
	else
	{
		//イベント通知を受け取った場合
		//取得画像をMat形式にして格納する
		for (auto y = 0; y < capturedImage.rows; y++)
		{
			auto ptr = capturedImage.ptr<cv::Vec3b>(y);

			for (auto x = 0; x < capturedImage.cols; x++)
			{
				cv::Vec3b bgr = ptr[x];
				auto b = memImg + (y * capImageWidth * pixelByte) + (x * pixelByte) + 0;
				auto g = memImg + (y * capImageWidth * pixelByte) + (x * pixelByte) + 1;
				auto r = memImg + (y * capImageWidth * pixelByte) + (x * pixelByte) + 2;

				ptr[x] = cv::Vec3b(*b, *g, *r);
			}
		}

		//登録された関数を実行する
		if (CapturedFunction != nullptr) CapturedFunction(0);

		return true;
	}

	if (retFunc == IS_SUCCESS)
	{
		return true;
	}
	else
	{
		return false;
	}

}

/// <summary>CV::Matを取得する </summary>
/// <returns></returns>
cv::Mat CIDSCameraCaputure::GetCVMat()
{
	//ImageDbugがtrueの場合は画像を保存する
	if (ImageDebug)
	{
		//デバッグ用にイメージを保存
		auto nowTime = std::time(nullptr);
		auto localNow = std::localtime(&nowTime);

		//2017_0101_0101_01_Cam1.bmp形式のファイル名を作成
		std::stringstream ssTime;
		ssTime << localNow->tm_year + 1900 << "_"
			<< std::setfill('0') << std::setw(2) << localNow->tm_mon + 1
			<< std::setfill('0') << std::setw(2) << localNow->tm_mday << "_"
			<< std::setfill('0') << std::setw(2) << localNow->tm_hour
			<< std::setfill('0') << std::setw(2) << localNow->tm_min << "_"
			<< std::setfill('0') << std::setw(2) << localNow->tm_sec
			<< "_Cam" << hCam << ".bmp";

		auto fname = ssTime.str();
		OutputDebugString(CUtil::StringToWString(std::string("Debug Output ImageFile=") + fname).c_str());

		cv::imwrite(fname, capturedImage);	//イメージ保存

	}
	return capturedImage;
}



/// <summary>カメラをオープンする</summary>
/// <returns></returns>
bool CIDSCameraCaputure::OpenCamera(void)
{
	SENSORINFO sensorInfo;

	auto retCamOpen = is_GetSensorInfo(hCam, &sensorInfo);
	if (retCamOpen != IS_SUCCESS) return false;

	INT sizeX{ 0 }, sizeY{ 0 };
	GetMaxImageSize(&sizeX, &sizeY);

	std::stringstream ss;
	ss << "Image Size X:" << sizeX << ",Y:" << sizeY << std::endl;
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
	ss.str("");
	ss.clear(std::stringstream::goodbit);

	INT bitPerPixel{ 0 }, colorMode{ 0 };
	is_GetColorDepth(hCam, &bitPerPixel, &colorMode);
	is_SetColorMode(hCam, colorMode);
	ss << "bitPerPixel:" << bitPerPixel << std::endl;
	ss << "colorMode:" << colorMode << std::endl;
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
	ss.str("");
	ss.clear(std::stringstream::goodbit);

	double dEnable = 1;

	//int ret = is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_GAIN, &dEnable, 0);

	//double dblEnable = 1;
	//double dblDummy = 0;
	//ret = is_SetAutoParameter(hCam, IS_SET_ENABLE_AUTO_SENSOR_SHUTTER, &dblEnable, &dblDummy);

	std::string fname("iniFile_" + std::to_string(cameraNumber) + ".ini");

	auto nRet = is_ParameterSet(hCam, IS_PARAMETERSET_CMD_LOAD_FILE, (void*)CUtil::StringToWString(fname).c_str(), NULL);

	//メモリ初期化
	is_AllocImageMem(hCam, sizeX, sizeY, bitPerPixel, &memImg, &memID);
	is_SetImageMem(hCam, memImg, memID);

	IS_SIZE_2D imageSize;
	imageSize.s32Width = sizeX;
	imageSize.s32Height = sizeY;

	is_AOI(hCam, IS_AOI_IMAGE_SET_AOI, (void*)&imageSize, sizeof imageSize);

	is_SetDisplayMode(hCam, IS_SET_DM_DIB);

	//oopencv用の領域を初期化する
	//sizeY:行数
	//sizeX:列数
	capturedImage = cv::Mat::zeros(sizeY, sizeX, CV_8UC3);

	//クラス内部変数に、取得イメージのサイズを登録
	capImageWidth = sizeX;
	capImageHeight = sizeY;
	pixelByte = (bitPerPixel + 7) / 8;

	return true;

}

void CIDSCameraCaputure::GetMaxImageSize(INT* sizeX, INT* sizeY)
{
	// Check if the camera supports an arbitrary AOI
	// Only the ueye xs does not support an arbitrary AOI
	INT nAOISupported = 0;
	BOOL bAOISupported = TRUE;
	if (is_ImageFormat(hCam,
		IMGFRMT_CMD_GET_ARBITRARY_AOI_SUPPORTED,
		(void*)&nAOISupported,
		sizeof(nAOISupported)) == IS_SUCCESS)
	{
		bAOISupported = (nAOISupported != 0);
	}

	if (bAOISupported)
	{
		// All other sensors
		// Get maximum image size
		SENSORINFO sInfo;
		is_GetSensorInfo(hCam, &sInfo);
		*sizeX = sInfo.nMaxWidth;
		*sizeY = sInfo.nMaxHeight;
	}
	else
	{
		// Only ueye xs
		// Get image size of the current format
		IS_SIZE_2D imageSize;
		is_AOI(hCam, IS_AOI_IMAGE_GET_SIZE, (void*)&imageSize, sizeof(imageSize));

		*sizeX = imageSize.s32Width;
		*sizeY = imageSize.s32Height;
	}
}





/// <summary>カメラ番号を指定してカメラを初期化する</summary>
/// <param name="camNum">カメラ番号又はデバイス番号</param>
/// <param name="useDeviceID">デバイス番号で初期化する場合は TRUE</param>
bool CIDSCameraCaputure::SetCameraNumber(int camNum, bool useDeviceID)
{
	//カメラ番号から、カメラの情報を取得する
	cameraNumber = camNum;

	if (useDeviceID) hCam = camNum | IS_USE_DEVICE_ID;
	else hCam = camNum;

	auto ansRet = is_InitCamera(&hCam, NULL);

	if (ansRet != IS_SUCCESS)
	{
		std::stringstream ss;
		ss << "SetCameraNumber内部でis_InitCameraの戻り値が" << ansRet << "です." << std::endl;
		OutputDebugStringA(ss.str().c_str());
		//OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

		if (ansRet == IS_STARTER_FW_UPLOAD_NEEDED)
		{
			// Time for the firmware upload = 25 seconds by default
			INT nUploadTime = 25000;
			is_GetDuration(hCam, IS_STARTER_FW_UPLOAD, &nUploadTime);

			std::stringstream ss1;
			ss1 << "This camera requires a new firmware. The upload will take about" << nUploadTime / 1000 << "seconds. Please wait ..." << std::endl;
			OutputDebugStringA(ss1.str().c_str());
			//OutputDebugString(CUtil::StringToWString(ss1.str()).c_str());

			// Try again to open the camera. This time we allow the automatic upload of the firmware by
			// specifying "IS_ALLOW_STARTER_FIRMWARE_UPLOAD"
			hCam = (HIDS)(((INT)hCam) | IS_ALLOW_STARTER_FW_UPLOAD);
			ansRet = is_InitCamera(&hCam, NULL);
		}
	}

	//初期化が正常に完了した
	if (ansRet == IS_SUCCESS)
	{
		//cv::Mat関係を初期化する
		isConnected = true;

		return true;;
	}
	else
	{
		isConnected = false;
		std::cout << "カメラの初期化に失敗しました。" << std::endl;
 		OutputDebugString(L"カメラの初期化に失敗しました。");

		//カメラ不使用でもアプリを動作させ続けるために、ここでは例外を吐かない
		//throw std::runtime_error("カメラの初期化に失敗しました.");
		return false;
		//取得した情報を元に、capturedImage変数を初期化する
	}
}


///<summary></summary>
CIDSCameraCaputure::~CIDSCameraCaputure()
{

}









