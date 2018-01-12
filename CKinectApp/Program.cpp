#include "Program.h"

#define MAX_BUFFER_SIZE 255

Program::Program()
{
}

Program::~Program()
{
}


int main()
{
	// 名前付きパイプを生成し、クライアント(C#)からの接続を待機する。
	// また、 Cam も初期化する
	auto pipe1 = CPipeServer(".", "TestPipe1", 1, "Cam1");
	auto pipe2 = CPipeServer(".", "TestPipe2", 2, "Cam2");
	
	pipe1.ConnectToNewClient();
	pipe2.ConnectToNewClient();

	// 初期化したカメラが適切か否かを確認する。
	pipe1.ConnectCameraRequestAndResponse();
	pipe2.ConnectCameraRequestAndResponse();


	// C#からカメラ情報要請を受信、指定されたカメラの情報を取得する
	pipe1.GetCamInfoRequestAndResponse();
	pipe2.GetCamInfoRequestAndResponse();

	pipe1.Ip.CameraParamsFile = "camParam-S4103159790-L372440.xml";
	pipe2.Ip.CameraParamsFile = "camParam-S4103159793-L372434.xml";

	//鳥観図を得るためのアフィン変換でつかうアフィン行列を読み込む
	//TODO: pipe2用のアフィン変換を用意する
	pipe1.Ip.LoadAffineMatrix("AffineTopView.xml");
	pipe2.Ip.LoadAffineMatrix("AffineTopView.xml");
	pipe1.Ip.HasToGetTopView = true;
	pipe2.Ip.HasToGetTopView = true;

	//TODO:  以下のxmの値を XAMLからこちらへ送信できるようにする
	//pipe1.Ip.ImageSetting.Point1X = 200;
	//pipe1.Ip.ImageSetting.Point1Y = 800;
	//pipe1.Ip.ImageSetting.Point2X = 833;
	//pipe1.Ip.ImageSetting.Point2Y = 800;

	cout << "---" << endl;

	//本プログラムを動かすための設定
	pipe1.Ip.DebugMode = false;
	pipe2.Ip.DebugMode = false;
	auto waitingTimeInMilliSecond = std::min<int>(pipe1.WaitingCommandInMilliSecond, pipe2.WaitingCommandInMilliSecond);


	while (pipe1.IsConnected() && pipe2.IsConnected())
	{
		if(!pipe1.IsEmpty())
		{
			// C#側で生成した設定(構造体)を受信、撮影用構造体を設定する
			pipe1.WaitAndReceiveAndSetSettingOfImageAnalysis();

			// C＃側から撮影信号と撮影位置番号を受信、カメラから2D画像を得る
			// 画像を得たことをC#に送信する
			pipe1.CaptureRequestAndResponse();

			// C＃側から測定結果要請と撮影位置番号を受信、検出した角情報をC#に送信する
			pipe1.GetMeasureDataRequestAndResponse();

			// C＃側から結果画像要請と撮影位置番号を受信、角に印をつけた画像をC#に送信する
			pipe1.GetImageRequestAndResponse();

			cout << "----" << endl;
		}

		if (!pipe2.IsEmpty())
		{
			// C#側で生成した設定(構造体)を受信、撮影用構造体を設定する
			pipe2.WaitAndReceiveAndSetSettingOfImageAnalysis();

			// C＃側から撮影信号と撮影位置番号を受信、カメラから2D画像を得る
			// 画像を得たことをC#に送信する
			pipe2.CaptureRequestAndResponse();

			// C＃側から測定結果要請と撮影位置番号を受信、検出した角情報をC#に送信する
			pipe2.GetMeasureDataRequestAndResponse();

			// C＃側から結果画像要請と撮影位置番号を受信、角に印をつけた画像をC#に送信する
			pipe2.GetImageRequestAndResponse();

			cout << "----" << endl;
		}

		
		Sleep(waitingTimeInMilliSecond);
	}
	return 0;
}

//設定用構造体の差分更新コマンドをパースして、
//変数名の文字列・代入したい値の文字列に代入する
void Program::parseDeltaSettingPacket(unsigned char * chars, std::string &valName, std::string &valValueStr)
{
	std::string valLengthStr;
	int valLength;

	auto sepCountUpperLimit = 2;
	auto sepCount = 0;
	char elementLength = 0;
	auto offset = 0;

	while (sepCount < sepCountUpperLimit)
	{
		auto c = chars[offset + elementLength];
		if (c == ',')
		{
			char *tempChars = new char[elementLength + 1];
			memcpy(tempChars, &chars[offset], elementLength);
			tempChars[elementLength] = '\0';

			switch (sepCount)
			{
			case 0:
				valName = std::string(tempChars);
				break;
			case 1:
				valLengthStr = std::string(tempChars);
				valLength = std::atoi(tempChars);
				break;
			default:
				break;
			}

			sepCount++;
			offset += elementLength + 1;
			elementLength = 0;
		}
		else
		{
			elementLength++;
		}
	}

	//ここで値を読み取る
	char *tempChars = new char[valLength + 1];
	memcpy(tempChars, &chars[offset], valLength);
	tempChars[valLength] = '\0';
	valValueStr = std::string(tempChars);
}
