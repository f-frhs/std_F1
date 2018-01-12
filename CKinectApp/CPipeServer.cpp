#include "CPipeServer.h"

#define MAX_BUFFER_SIZE 256

CPipeServer::CPipeServer(std::string computerName, std::string pipeName, int camId, std::string camName)
{
	this->computerName = computerName;
	this->pipeName = pipeName;
	std::stringstream ss;
	ss << "\\\\" << computerName << "\\pipe\\" << pipeName;
	pipeFullName = ss.str();

	PipeHandle = CreateNamedPipe(
		CUtil::StringToWString(pipeFullName).c_str(),
		PIPE_ACCESS_DUPLEX | FILE_FLAG_WRITE_THROUGH,
		PIPE_TYPE_MESSAGE | PIPE_WAIT,
		1,
		(1280 * 1280 * 4),
		(1280 * 1280 * 4),
		INFINITY,
		nullptr);

	if (PipeHandle == INVALID_HANDLE_VALUE)
	{
		OutputLog("パイプ作成に失敗", "Error");
		exit(static_cast<int>(ExitStatus::NO_PIPE_CREATED));
		return;
	}
	else
	{
		OutputLog("パイプ作成に成功");
	}

	this->CamId = camId;
	this->CamName = camName;
}

bool CPipeServer::ConnectToNewClient()
{
	OutputLog("クライアントの接続待ち...");

	if (IsConnected())
	{
		OutputLog("クライアントと接続しました。");

		//Clientに接続報告
		SendMessage(pipeName + ",connect");

		return true;
	}
	else
	{
		OutputLog("クライアントとの接続に失敗しました。", "Error");
		exit(static_cast<int>(ExitStatus::NO_CLIENT_CONNECTED));
	}
}

bool CPipeServer::ConnectCameraRequestAndResponse()
{
	std::stringstream ss;
	ss << "FailedConnectCamera," << std::setfill('0') << std::setw(3) << CamId;
	auto failedMsg = ss.str();

	if (IsConnected())
	{
		OutputLog("カメラの初期化信号を待ち受け中...");

		auto cmd = WaitAndGetCommand();
		if (cmd.IsParsedSuccessfully && cmd.Command == std::string("ConnectCamera"))
		{
			//カメラを使うか、保存済みの画像ファイルを読み込むか?
			useCamMode = cmd.NumString != "999";

			//保存済みの画像を読み込む場合
			if(!useCamMode)
			{
				OutputLog("カメラ不使用(画像ファイル読込)モードになります。");
				
				CamId = cmd.Num;
				OutputLog("カメラ初期化完了信号送信: FailedConnectCamera," + cmd.NumString);

				std::stringstream connectedSs;
				connectedSs << "FailedConnectCamera," << std::setfill('0') << std::setw(3) << CamId;
				SendMessage(connectedSs.str());			//OKを送信

				return true;
			}

			//カメラを使用して画像を取り込む場合
			if (useCamMode)
			{
				OutputLog("カメラ初期化信号を受信: " + cmd.OriginalMessage);

				//カメラ接続コマンドを実行
				Cap.ImageDebug = true;				//デバッグモード有効
				Cap.CapturedFunction = nullptr;		//撮影完了後に呼ばれる関数
				auto setCamSuccessfully = Cap.SetCameraNumber(CamId, true);	//デバイスIDを指定してカメラを開く
				if(!setCamSuccessfully)
				{
					OutputLog("カメラIDの設定で失敗しました。カメラを開けません。", "Error");
					SendMessage(failedMsg);
					return false;
				}

				auto openFlg = Cap.OpenCamera();	//カメラを開く

				if (openFlg)
				{

					OutputLog("カメラ初期化完了信号送信: " + CamName + "Connect");

					std::stringstream connectedSs;
					connectedSs << CamName << "Connect," << std::setfill('0') << std::setw(3) << CamId;
					SendMessage(connectedSs.str());			//OKを送信

					return true;
				}
				else
				{
					OutputLog("No camera found.", "Error");
					SendMessage(failedMsg);
					return false;
				}
			}
		}
		else
		{
			OutputLog("不正なコマンドを受信: " + cmd.OriginalMessage, "Error");

			//Clientに結果送信
			std::stringstream errorResponseSs;
			errorResponseSs << "InvalidCommand," << std::setfill('0') << std::setw(3) << CamId;
			SendMessage(errorResponseSs.str());

			return false;
		}
	}
}


bool CPipeServer::GetCamInfoRequestAndResponse()
{
	if (IsConnected())
	{
		OutputLog("カメラ情報取得の信号を待ち受け中...");

		auto cmd = WaitAndGetCommand();
		if (cmd.IsParsedSuccessfully && cmd.Command == std::string("GetCamInfo"))
		{
			OutputLog("カメラ情報取得の信号を受信: " + cmd.OriginalMessage);

			auto camName = useCamMode ? Cap.GetCameraInfoStr() : "NoCamera";
			SendMessage(camName);

			OutputLog("カメラ情報送信完了: " + camName);

			return true;
		}
		else
		{
			OutputLog("カメラ情報の取得に失敗" + cmd.OriginalMessage, "Error");

			//Clientに結果送信
			std::stringstream errorResponseSs;
			errorResponseSs << "FailedGetCameraInfo," << std::setfill('0') << std::setw(3) << CamId;
			SendMessage(errorResponseSs.str());

			return false;
		}
	}
}

bool CPipeServer::WaitAndReceiveAndSetSettingOfImageAnalysis()
{
	auto receivedWholeSetting = false;
	if (IsConnected() && !receivedWholeSetting)
	{
		OutputLog("設定管理用構造体の受信を開始します...");

		do
		{
			//cout << "クライアントから、設定管理用構造体が渡されるのを待ちます..." << endl;
			Sleep(WaitingCommandInMilliSecond);
		} while (!isStructAll());

		unsigned char receivedBytes[MAX_BUFFER_SIZE];
		int bytesLength;
		auto receivedSuccessfully = RecvBytes(receivedBytes, bytesLength);

		if (receivedSuccessfully)
		{
			//構造体へ格納する
			ImageProcessSetting setting;
			setting.deserialize(receivedBytes);
			Ip.SetSetting(setting);
			receivedWholeSetting = true;

			OutputLog("設定管理用構造体の受信・格納を完了しました。");

			//Clientに構造体受取報告
			SendMessage(std::string("StructerReceived"));

			return true;
		}
		else
		{
			OutputLog("設定管理用構造体の受信に失敗しました。", "Error");

			//Clientに構造体受取報告
			SendMessage(std::string("FailedReceivedStruc"));

			//Clientに結果送信
			std::stringstream errorResponseSs;
			errorResponseSs << "FailedReceivedStruc," << std::setfill('0') << std::setw(3) << CamId;
			SendMessage(errorResponseSs.str());

			return false;
		}
	}
}

bool CPipeServer::isStructAll()
{
	if (isEmpty()) return false;

	return get1stChar() == "S"; // "StructAll,..."だから
}

bool CPipeServer::isImage()
{
	if (isEmpty()) return false;

	return get1stChar() == "I"; // "Image,..."だから
}

bool CPipeServer::isStructDelta()
{
	if (isEmpty()) return false;

	auto s = get1stChar();

	return(islower(s[0]));  	// 小文字で始まるため。 StructAll や Image では始まらない
}

bool CPipeServer::isEmpty()
{
	return get1stChar() == "";
}

// クライアントがパイプに書き込んだ内容を1byteだけ取得する。
// パイプに書き込んだ内容を変更しない。
// パイプに何も書かれていなければ、空白 "" を返す。
std::string CPipeServer::get1stChar()
{
	DWORD bytesRead = 0, bytesAvail = 0;
	byte buf[2];
	bool isPeeked = PeekNamedPipe(PipeHandle, &buf, 1, &bytesRead, &bytesAvail, NULL);

	//ダメな場合を先に列挙
	if (!isPeeked) return "";       //PeekNamedPipe が失敗したら
	if (bytesRead == 0) return "";  //1文字もPeekできなかったら

	buf[1] = '\0';
	std::string s = (reinterpret_cast<char const *>(buf));

	return s;
}

//撮像信号を受信->画像の取得->測定結果を画像に上書きしC#に返す
bool CPipeServer::CaptureRequestAndResponse()
{
	OutputLog("撮像信号の受信を開始します...");

	//Pipeからデータ読み込み
	auto captureReq = WaitAndGetCommand();

	//コマンドのパースに失敗したら、falseを返す
	if (!captureReq.IsParsedSuccessfully)
	{
		OutputLog("撮像信号のパースに失敗しました。", "Error");
		return false;
	}

	//コマンド内容を確認
	if (captureReq.Command != std::string("Capture") &&
		captureReq.Command != std::string("OfflineImage"))
	{
		OutputLog("コマンドが異なります(期待: Capture or OfflineImage): " + captureReq.OriginalMessage, "Error");
		return false;
	}


	OutputLog("画像取得信号を受信: " + captureReq.OriginalMessage);

	//cv::Matとして取得する
	bool imageAcquiredSuccessfully;
	if (captureReq.Command == std::string("Capture"))
	{
		//カメラから2D画像を得る
		imageAcquiredSuccessfully = Get1FrameFromCamera();
	}
	else if (captureReq.Command == std::string("OfflineImage"))
	{
		//画像ファイルから読み込む
		Ip.ImgRawColor = cv::imread("../data/img.bmp");
		imageAcquiredSuccessfully = Ip.ImgRawColor.size() != cv::Size(0,0);
	}
	else
	{
		OutputLog("この条件にはならないはず in CPipeServer::CaptureRequestAndResponse(): " + captureReq.OriginalMessage, "Error");
		return false;
	}

	//Clientに画像取得結果を報告
	if (!imageAcquiredSuccessfully)
	{
		//失敗の場合
		SendMessage(std::string("FailedCapture"));
		return false;
	}
	else
	{
		//成功の場合
		SendMessage(std::string("SucceedCapture"));

		//その後の処理(コーナー検出)を行う
		Ip.DrawCorners(Ip.CameraParamsFile, CUtil::getDateTimeString());
		
		return true;
	}
}

bool CPipeServer::Get1FrameFromCamera()
{
	//カメラから2D画像を得る

	//撮影開始コマンドを実行する
	Ip.EndCapFlg = false;
	Cap.CapturedFunction = std::bind(&CPipeServer::SetEndCapFlgTrue, this);  //撮影が完了したら EndCapFlg を true にする関数を登録
	Cap.StartSingleCapture();	//撮影を開始する

	if (Ip.EndCapFlg)
	{
		//画像をMat形式で格納する
		Ip.ImgRawColor = Cap.GetCVMat();
		OutputLog("カメラからの撮像が完了しました");
		return true;
	}
	return false;
}

bool CPipeServer::GetImage(bool cameraUseMode)
{
	if(cameraUseMode)
	{
		
	}
	return false;
}

//画像からコーナー情報を取得する
bool CPipeServer::GetMeasureDataRequestAndResponse()
{
	OutputLog("コーナー情報リクエストの受信を開始します...");

	auto getMeasureDataReq = WaitAndGetCommand();
	if (getMeasureDataReq.IsParsedSuccessfully && getMeasureDataReq.Command == std::string("GetMeasureData"))
	{
		OutputLog("コーナー情報リクエストを受信: " + getMeasureDataReq.OriginalMessage);

		auto cornersPacket = Ip.Corners.ToPacket();
		if (cornersPacket.size() != NULL)
		{
			//Clientに結果送信
			SendMessage(cornersPacket);
			OutputLog("コーナー情報を送信しました。");

			return true;
		}
		else
		{
			OutputLog("コーナー情報の測定に失敗しました。", "Error");

			//Clientに結果送信
			std::stringstream errorResponseSs;
			errorResponseSs << "FailedMeasureing," << std::setfill('0') << std::setw(3) << CamId;
			SendMessage(errorResponseSs.str());

			return false;
		}
	}

	OutputLog("コーナー情報の受信に失敗しました。", "Error");

	//Clientに結果送信
	std::stringstream errorResponseSs;
	errorResponseSs << "FailedMeasureingReq," << std::setfill('0') << std::setw(3) << CamId;
	SendMessage(errorResponseSs.str());

	return false;
}

bool CPipeServer::GetImageRequestAndResponse()
{
	OutputLog("結果画像リクエストの受信を開始します...");

	auto getImageReq = WaitAndGetCommand();
	if (getImageReq.IsParsedSuccessfully && getImageReq.Command == std::string("GetImage"))
	{
		OutputLog("結果画像リクエストを受信しました: " + getImageReq.OriginalMessage);

		//角に印をつけた画像を、名前付きパイプで送る
		DWORD packetLength;
		auto buf = Ip.MakePacket(Ip.ImgWithCorners, packetLength);
		if (buf != nullptr)
		{
			DWORD writeBytes;
			WriteFile(PipeHandle, buf, packetLength, &writeBytes, NULL);
			free(buf);

			OutputLog("結果画像を送信しました。");

			return true;
		}
		else
		{
			OutputLog("結果画像生成に失敗しました。", "Error");

			//Clientに結果送信
			std::stringstream errorResponseSs;
			errorResponseSs << "FailedReceivedStruc," << std::setfill('0') << std::setw(3) << CamId;
			SendMessage(errorResponseSs.str());

			return false;
		}
		free(buf);

		OutputLog("結果画像の送信に失敗しました。(buf == nullptr)", "Error");

		//Clientに結果送信
		std::stringstream errorResponseSs;
		errorResponseSs << "FailedSendImage," << std::setfill('0') << std::setw(3) << CamId;
		SendMessage(errorResponseSs.str());

		return false;
	}
	OutputLog("結果画像リクエストの受信に失敗しました。", "Error");

	//Clientに結果送信
	std::stringstream errorResponseSs;
	errorResponseSs << "FailedReceivedImageRequest" << std::setfill('0') << std::setw(3) << CamId;
	SendMessage(errorResponseSs.str());
	return false;
}

/// <summary>パイプに書かれた文字列を読み込み、区切り文字で区切って、1番目,2番目の要素を str1st, camId に代入する。正常に作業が完了したときはtrue を返す。そうでなければ falseを返す </summary>
/// <param name="pipeHandle">読みに行くパイプのハンドル</param>
/// <param name="str1st">出力用。1番目の要素。</param>
/// <param name="camId">出力用。2番目の要素。</param>
/// <returns>操作が正常に完了したか否か。</returns>
CCommandPacket CPipeServer::GetCommand()
{
	char receiveFromClient[256];
	DWORD dwRead;

	if (!ReadFile(PipeHandle, receiveFromClient, sizeof(receiveFromClient), &dwRead, NULL))
	{
		//読み込み失敗時
		OutputLog("パイプ読み取り時にエラーが発生しました。", "Error");

		//読み取れなかったバイト列を極力壊さないように保持。
		CCommandPacket ans(std::string(receiveFromClient).substr(0,255));
		ans.IsParsedSuccessfully = false;

		return ans;
	}

	// "ConnectCamera,001" カンマから3文字目までを取得する
	std::string allString(receiveFromClient);
	auto sepPos = allString.find(",");
	auto endPos = sepPos + 3 + 1;
	auto allString2 = allString.substr(0, endPos);

	//CCommandPacktインスタンスを作成
	auto commandPacket = CCommandPacket(allString2);
	auto parsedSuccessfully = commandPacket.TryParse();
	commandPacket.IsParsedSuccessfully = parsedSuccessfully;

	return commandPacket;
}

bool CPipeServer::SendMessage(std::string msg)
{
	if (msg.empty())
	{
		OutputLog("Empty message in CPipeServer::SendMessage()", "Error");
		return false;
	}

	const char* msgChars = msg.c_str();
	auto sendBytesSuccessfully = SendBytes(msgChars, msg.length());

	return sendBytesSuccessfully;
}

bool CPipeServer::SendBytes(const char * bytes, int length)
{
	DWORD dwWritten = 0;
	auto sendSuccessfully = WriteFile(PipeHandle, bytes, length, &dwWritten, NULL);

	//一部分しか書き込まれない場合は、一応トレース出力する
	if (dwWritten < length)
	{
		std::stringstream ss;
		ss << "WriteFile writes partially (dwWritten/length)=("
			<< dwWritten << "/" << length << ")";

		OutputLog(ss.str());
	}

	return sendSuccessfully;
}

bool CPipeServer::RecvBytes(unsigned char* bytes, int& byteLength)
{
	//パイプに書かれたサイズを取得して、そのサイズ分だけを ReadFileする。
	DWORD bytesRead = 0, bytesAvail = 0;
	byte buffer[MAX_BUFFER_SIZE];
	auto peekedSuccessfuly = PeekNamedPipe(PipeHandle, NULL, 0, NULL, &bytesAvail, NULL);
	auto readSuccessfully = ReadFile(PipeHandle, &buffer, bytesAvail, &bytesRead, NULL);

	//取得した値を出力用の変数に代入
	memcpy(bytes, &buffer, bytesRead);
	byteLength = (int)bytesRead;

	return peekedSuccessfuly && readSuccessfully && (*bytes==*buffer) && (byteLength == bytesRead);
}

bool CPipeServer::IsEmpty()
{
	DWORD bytesAvail;
	auto peekResult = PeekNamedPipe(PipeHandle, NULL, 0, NULL, &bytesAvail, NULL);

	if (!peekResult)
	{
		OutputDebugString(L"Falied to Peek.");
	}

	return peekResult && (bytesAvail == 0);
}

//pipeHandle が接続しているか
bool CPipeServer::IsConnected()
{
	return (ConnectNamedPipe(PipeHandle, nullptr)
		? true
		: GetLastError() == ERROR_PIPE_CONNECTED);
}

CCommandPacket CPipeServer::WaitAndGetCommand()
{
	//パイプが空ならちょっと待つ
	do
	{
		Sleep(WaitingCommandInMilliSecond);
	} while (this->IsEmpty());

	return this->GetCommand();
}

void CPipeServer::OutputLog(std::string sjisString, std::string sjisReportType)
{
	std::stringstream ss1, ss2;

	//標準出力用
	ss1 << pipeName << ": " << sjisString;
	std::cout << ss1.str() << std::endl;

	//OutputDebugString用
	ss2 << sjisReportType << ":(C++):" << pipeName << ": " << sjisString << std::endl;
	OutputDebugStringA(ss2.str().c_str());  //sjis 前提でOutputDebugString**A**()を読んでいる
}

bool CPipeServer::SetEndCapFlgTrue()
{
	Ip.EndCapFlg = true;
	return true;
}

bool CPipeServer::SetEndCapFlgFalse()
{
	Ip.EndCapFlg = false;
	return true;
}
