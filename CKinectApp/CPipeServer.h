#ifndef CPIPE_SERVER_H
#define CPIPE_SERVER_H

#include <iostream>
#include <string>
#include <sstream>
//#include <windows.h>
#include "CUtil.h"
#include "CCommandPacket.h"
#include "CImageProcessor.h"
#include "Setting.h"
#include "CWebCam.h"
#include "CIDSCameraCapture.h"

class CPipeServer
{
private:
	/// <summary>コンピュータ名</summary>
	std::string computerName;

	/// <summary>パイプ名</summary>
	std::string pipeName;

	/// <summary>パイプのフルネーム。 \\[computerName]\pipe\[pipeName] になっているはず。</summary>
	std::string pipeFullName;

	/// <summary>接続したカメラから画像を取得するか否か。取得する予定の場合は true。 そうでない(=保存した画像ファイルを読み込む)場合は false。</summary>
	bool useCamMode;

	/// <summary>パイプに書き込まれた命令が構造体("StructAll")か否か</summary>
	bool isStructAll();

	/// <summary>パイプに書き込まれた命令が画像("Image")か否か</summary>
	bool isImage();

	/// <summary>パイプに書き込まれた命令が構造体の差分か否か</summary>
	bool isStructDelta();

	/// <summary>パイプに書き込まれた命令が空か否か</summary>
	bool isEmpty();

	/// <summary>パイプに書き込まれた命令の1文字目を返す。何も書き込まれていない場合は空白 std::string("") を返す。</summary>
	std::string get1stChar();

public:
	/// <summary> Exit 時のステータスを示す列挙体</summary>
	enum class ExitStatus
	{
		OK = 0,
		NO_PIPE_CREATED,
		NO_CLIENT_CONNECTED,
	};

	HANDLE PipeHandle;
	int WaitingCommandInMilliSecond = 100;

	/// <summary>コンストラクタ。コンピュータ名、パイプ名、撮像用カメラのID(0スタート)を指定する。</summary>
	CPipeServer(std::string computerName, std::string pipeName, int camId, std::string camName);

	/// <summary>デストラクタ</summary>
	~CPipeServer() {};

	/// <summary>画像を取得するカメラへのポインタ</summary>
	//CWebCam *pCam;

	/// <summary>名前付きパイプを通じてクライアントと接続する</summary>
	bool ConnectToNewClient();

	/// <summary>ConnectCameraコマンドを受信し、指定したカメラと接続し、その結果を名前付きパイプへ書きこむ。</summary>
	bool ConnectCameraRequestAndResponse();

	/// <summary>GetCamInfo コマンドを受信し、その結果のカメラ型番を名前付きパイプへ書き込む。</summary>
	bool GetCamInfoRequestAndResponse();

	/// <summary>画像解析用の設定(構造体)を受信するまで待ち、設定を ip.ImageSetting へ代入する</summary>
	bool WaitAndReceiveAndSetSettingOfImageAnalysis();

	/// <summary>名前付きパイプを通じて Capture コマンドを受信し、 camera で撮像し、画像処理の結果文字列を名前付きパイプへ書き込む。</summary>
	bool CaptureRequestAndResponse();
	
	/// <summary>以下の作業の成否を返す: カメラから1フレーム取得して、 ip.ImgRawColor(cv::Mat形式)へ代入する。 </summary>
	bool Get1FrameFromCamera();

	/// <summary>Ip.ImgRawColorへ画像を取得・代入し、その成否を返す(成功すればtrue)。cameraUseMode=true なら カメラで撮像した画像を代入する。 cameraUseMode=false なら、 FujitaAssy\data\img.bmpの画像(Mat形式)を代入する。</summary>
	bool GetImage(bool cameraUseMode);

	/// <summary>画像解析にて検出したコーナー情報のリクエストを受信し、結果を名前付きパイプに書き込む。</summary>
	bool GetMeasureDataRequestAndResponse();

	/// <summary>画像解析結果の画像のリクエストを受信し、画像を名前付きパイプに書き込む。</summary>
	bool GetImageRequestAndResponse();

	/// <summary>画像解析用のオブジェクト</summary>
	CImageProcessor Ip;

	/// <summary>カメラID番号</summary>
	int CamId;

	/// <summary>カメラ名</summary>
	std::string CamName;

	/// <summary>画像撮像用のカメラ</summary>
	CIDSCameraCaputure Cap;

	bool IsConnected();
	CCommandPacket GetCommand();
	bool SendMessage(std::string msg);
	bool ReceiveMessage(std::string& msg);
	bool SendBytes(const char* bytes, int length);
	bool RecvBytes(unsigned char* bytes, int& byteLength);
	bool IsEmpty();
	CCommandPacket WaitAndGetCommand();

	/// <summary>文字列(u8Literal)を出力する。出力先は、標準出力とOutputDebugString。 </summary>
	/// <param name="u8Literal">UTF-8文字列リテラル(例: u8"デバッグ用文字列")</param>
	void OutputLog(std::string sjisString, std::string sjisRportType=std::string("Report"));
	
	/// <summary>セッター。 EndCapFlg に true を代入する。</summary>
	bool SetEndCapFlgTrue();

	/// <summary>セッター。 EndCapFlg に false を代入する。</summary>
	bool SetEndCapFlgFalse();
};


#endif // !CPIPE_SERVER_H
