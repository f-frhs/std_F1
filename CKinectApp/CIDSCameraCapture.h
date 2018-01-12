#ifndef IDSCAMERACAPUTURE_HPP
#define IDSCAMERACAPUTURE_HPP

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#include <opencv2/core/mat.hpp>
#include "uEye.h"
#include <memory>
#include <functional>

//#include <cereal/archives/xml.hpp>
//#include <cereal/types/string.hpp>

//#define _CRT_SECURE_NO_WARNINGS

/*
* ///<summary></summary>
*/

//CIDSCameraCaputureの内部データ保存様
struct CameraCaputureData
{
	int deviceID;
	IO_FLASH_PARAMS flashParams;

	template<class Archive>
	void save(Archive& ar) const
	{
		ar(deviceID, flashParams);
	}

	template<class Archive>
	void load(Archive& ar)
	{
		ar(deviceID, flashParams);
	}
};

/// <summary>カメラステータス保存用構造体</summary>
typedef struct camerastatsu
{
	ULONG FifoOverCnt;
	ULONG SequenceCnt;
	ULONG SequenceSize;
	ULONG ExtTriggerEventCnt;
	ULONG TriggerMissed;
	ULONG LastCaptureErrir;
	ULONG ParameterExt;
	ULONG ParameterSet1;
	ULONG ParameterSet2;
	ULONG IsStandby;
	ULONG IsStandbySupported;
}CameraStatus, *PCameraStatus;

///<summary></summary>
class CIDSCameraCaputure
{
public:

	///<summary>コンストラクタ</summary>
	CIDSCameraCaputure();

	///<summary>デストラクタ</summary>
	~CIDSCameraCaputure();

	///<summary>カメラ番号を指定しするコンストラクタ</summary>
	CIDSCameraCaputure(int camNum, bool useDeviceID);	//カメラ番号を指定したコンストラクタ

														///<summary>カメラ番号を指定して初期化する</summary>
	bool SetCameraNumber(int camNum, bool useDeviceID);

	///<summary></summary>
	bool StartSingleCapture(void);	//1フレームキャプチャする

									///<summary>カメラの情報を取得する（不揮発メモリ内の情報）</summary>
	CAMINFO GetCameraInfo(void);

	///<summary>カメラのステータスを取得する</summary>
	bool GetCameraStatus(void);

	bool SetFlash(bool flashFlg);	//フラッシュの設定

									/// <summary>カメラをオープンする </summary>
	bool OpenCamera(void);

	/// <summary>設定をファイルに保存する</summary>
	bool SaveToFile(std::string fName);

	///<summary>設定をファイルから復元する</summary>
	bool LoadFromFile(std::string fName);

	void GetMaxImageSize(INT* sizeX, INT* sizeY);

	/// <summary>CV::Matを取得する </summary>
	cv::Mat GetCVMat();

	/// <summary>デバッグ設定フラグ </summary>
	bool ImageDebug = false;

	/// <summary>キャプチャ完了後に呼び出される関数オブジェクトの登録用</summary>
	std::function<void(int)> CapturedFunction = nullptr;

	/// <summary></summary>
	std::string GetCameraInfoStr();

	//TODO:イメージプロセッサ用の関数を追加するクラスメンバ関数登録を準備する

private:

	///<summary></summary>
	cv::Mat capturedImage;	//IDSカメラでキャプチャした画像

							///<summary>カメラ番号</summary>
	int cameraNumber{ -1 };

	/// <summary>カメラへのカメラハンドル</summary>
	HIDS hCam;

	/// <summary>カメラの初期化が正常に完了した場合にtrue </summary>
	bool isConnected = false;

	/// <summary>イメージメモリ保存用のポインタ </summary>
	char *memImg;
	INT memID;

	/// <summary>カメラ撮影完了用イベント </summary>
	HANDLE camEvent;

	bool SingleCaptureWork(void);

	//取得イメージのサイズ
	int capImageWidth;
	int capImageHeight;

	//１ピクセルのバイト数
	int pixelByte;


};



#endif
