#include "CImageProcessor.h"

CImageProcessor::CImageProcessor()
{
	screenshotCount = 0;
	DebugMode = true;
};

void CImageProcessor::SetSetting(ImageProcessSetting &s)
{
	ImageSetting = s;
};

void CImageProcessor::Preprocess(std::string calibResultPath, std::string fNamePrefix)
{
	//カメラによる歪みを補正する
	cv::FileStorage fs;

	//以前にキャリブレーションした結果を読み込み、関連する行列へ格納する
	fs.open(calibResultPath, cv::FileStorage::READ);
	cv::Mat intrinsic_matrix, distortion_coeffs, stdDevIntrinsics;
	fs["intrinsic"] >> intrinsic_matrix;
	fs["distortion"] >> distortion_coeffs;
	//std::cout << "\nintrinsic matrix:" << intrinsic_matrix;
	//std::cout << "\ndistortion coefficients: " << distortion_coeffs << std::endl;

	//歪みを補正する。補正した場合のみ、 imshow()や *undistorted.jpg を保存する。
	if (ImageSetting.HasToUndistort)
	{
		cv::undistort(ImgRawColor, ImgUndistorted, intrinsic_matrix, distortion_coeffs);
		if (DebugMode) cv::imshow("Undistorted image", ImgUndistorted);
		cv::imwrite(fNamePrefix + "undistorted.jpg", ImgUndistorted);
	}
	else
	{
		ImgUndistorted = ImgRawColor.clone();
	}

	//リサイズ
	cv::resize(ImgUndistorted, ImgResized, cv::Size(ImageSetting.OutputWidth, ImageSetting.OutputHeight));
	cv::imwrite(fNamePrefix + "resized.jpg", ImgResized);

	// 鳥観図へ変換
	if (HasToGetTopView)
	{
		assert(affineMat.size() != cv::Size(0, 0));
		cv::warpAffine(ImgUndistorted, ImgTopView, affineMat, ImgUndistorted.size());
		if (DebugMode) cv::imshow("TopView", ImgTopView);
		cv::imwrite(fNamePrefix + "topView.jpg", ImgTopView);
	}
	else
	{
		ImgTopView = ImgUndistorted;
	}

	//HSV色空間での値を確認する
	cv::Mat heu, sat, bri, imgHsv[3];
	cv::split(ImgTopView, imgHsv);
	if (DebugMode)
	{
		cv::imshow("Hue", imgHsv[0]);
		cv::imshow("Saturation", imgHsv[1]);
		cv::imshow("Brightness", imgHsv[2]);
	}

	//ジグザグのノイズ除去
	cv::Mat imgBlured;
	cv::GaussianBlur(imgHsv[1], imgBlured, cv::Size(ImageSetting.KSizeX, ImageSetting.KSizeY), ImageSetting.SigmaX, ImageSetting.SigmaY);
	if (DebugMode) cv::imshow("With Gaussian filter", imgBlured);

	//二値化する
	ImgGray = imgBlured.clone();
	//cv::cvtColor(imgBlured, ImgGray, CV_BGRA2GRAY);
	if (DebugMode) cv::imshow("Gray Image", ImgGray);
	cv::Canny(ImgGray, ImgBlackAndWhite, ImageSetting.Threshold1, ImageSetting.Threshold2, 3);
	if (DebugMode) cv::imshow("Binary Image", ImgBlackAndWhite);
	cv::imwrite(fNamePrefix + "gray.jpg", ImgGray);
	cv::imwrite(fNamePrefix + "blackAndWhite.jpg", ImgBlackAndWhite);
}

/// <summary>コーナーを検出して、image に描画した Mat型の配列を返す。 </summary>
/// <param name="image">コーナーを検出・重ねて描画したい画像データ</param>
/// <param name="calibResultPath">カメラキャリブレーション結果を保存したxmlファイルのパス</param>
/// <returns>検出したコーナーを image に重ねて描画した配列</returns>
cv::Mat CImageProcessor::DrawCorners(std::string calibResultPath, std::string fNamePrefix)
{
	Preprocess(calibResultPath, fNamePrefix);

	//線分を検出する -線分を返す
	auto lineCandidates = DetectLines(ImgBlackAndWhite,
		ImageSetting.RhoResolution, ImageSetting.ThetaResolution,
		ImageSetting.Threshold, ImageSetting.MinLineLength, ImageSetting.MaxLineGap);


	//抽出する領域を設定
	auto p1 = cv::Point2i(ImageSetting.X1, ImageSetting.Y1);
	auto p2 = cv::Point2i(ImageSetting.RecWidth, ImageSetting.RecHeight);
	auto rect = cv::Rect(p1, p1 + p2);
	auto extractedLineCandidates = ExtractLines(lineCandidates, rect);
	auto ls = CLineSegment::ConverFrom(extractedLineCandidates);

	cv::cvtColor(ImgBlackAndWhite, ImgWithLines, CV_GRAY2BGR);

	ImgWithCorners = ImgTopView.clone();
	cv::rectangle(ImgWithLines, rect, RED);
	cv::rectangle(ImgWithCorners, rect, RED);

	//x軸にほぼ平行な線を抽出
	auto horizontalLines = CLineSegment::Extract(ls, 0, 10);
	auto hls = CLineSegment::UniqLines(horizontalLines);
	DrawLines(ImgWithLines, hls, LIGHTBULE, 5);
	DrawLines(ImgWithCorners, hls, LIGHTBULE, 5);

	//y軸にほぼ平行な線を抽出
	auto verticalLines = CLineSegment::Extract(ls, 90, 10);
	auto vls = CLineSegment::UniqLines(verticalLines);
	DrawLines(ImgWithLines, vls, YELLOW, 5);
	DrawLines(ImgWithCorners, vls, YELLOW, 5);


	if (DebugMode)
	{
		cv::imshow("Detected Lines", ImgWithLines);
		cv::imwrite(fNamePrefix + "withLines.jpg", ImgWithLines);
	}

	bool foundUpperGroup = false, foundLowerGroup = false;
	bool foundLeftVls = false, foundRightVls = false;

	////交点を検出する
	if (0 < hls.size() && 2 <= vls.size())
	{
		//線の候補が複数あるので、選定する

		//縦線を選ぶ
		//線分上のy座標最大値を基準に抽出する
		std::vector<int> bottomYList;
		for (auto i = 0; i < vls.size(); i++)
		{
			bottomYList.push_back(std::max<int>(vls[i].P1.y, vls[i].P2.y));
		}

		//上下で分ける
		//クラスタリング
		std::vector<int> whichGroup(bottomYList.size()), clusteredCenter;
		clusteredCenter = KmeansInts(bottomYList, 2, whichGroup);
		std::vector<CLineSegment> upperVls, lowerVls;
		if (clusteredCenter.at(0) < clusteredCenter.at(1))
		{
			for (auto i = 0; i < whichGroup.size(); i++)
			{
				if (whichGroup[i] == 0) upperVls.push_back(vls[i]);
				if (whichGroup[i] == 1) lowerVls.push_back(vls[i]);
			}
		}
		else
		{
			for (auto i = 0; i < whichGroup.size(); i++)
			{
				if (whichGroup[i] == 1) upperVls.push_back(vls[i]);
				if (whichGroup[i] == 0) lowerVls.push_back(vls[i]);
			}
		}

		foundUpperGroup = 0 < upperVls.size();
		foundLowerGroup = 0 < lowerVls.size();

		DrawLines(ImgWithCorners, upperVls, YELLOW, 5);
		DrawLines(ImgWithCorners, lowerVls, GREEN, 5);

		//lowerVls に2本以上の候補がある場合は、板の縦エッジを抽出できる可能性がある
		cv::Vec2i
			refPoint1(ImageSetting.Point1X, ImageSetting.Point1Y),
			refPoint2(ImageSetting.Point2X, ImageSetting.Point2Y);
		Draw(refPoint1, ImgWithCorners, 10, 4, RED, "Ref1", RED, 1.5);
		Draw(refPoint2, ImgWithCorners, 10, 4, BLUE, "Ref2", BLUE, 1.5);

		CLineSegment leftVls, rightVls;
		if (1 <= lowerVls.size())
		{
			if (lowerVls.size() == 1) OutputDebugString(L"vls has only 1 element.");

			leftVls = CLineSegment::GetNearLine(lowerVls, refPoint1);
			rightVls = CLineSegment::GetNearLine(lowerVls, refPoint2);

			foundLeftVls = true;
			foundRightVls = true;

			Draw(leftVls, ImgWithCorners, RED);
			Draw(rightVls, ImgWithCorners, BLUE);
		}

		//横線を選ぶ
		bool
			foundLeftHls = false,
			foundRightHls = false,
			foundLeftHlsGroup = false,
			foundRightHlsGroup = false,
			foundOtherHlsGroup = false;
		CLineSegment leftHls, rightHls;
		std::vector<CLineSegment> leftHlsGroup, rightHlsGroup, otherHlsGroup;

		//板-板間の隙間の縦線が存在する x座標の範囲を求めて、そこを境に2グループへ分ける。
		// 左板にある横線: leftVlsGroup
		// 隙間にある横線: 無視
		// 右板にある横線: rightVlsGroup

		auto lengthCoeff = 0.6;

		//範囲を求める
		int xMinLowerVls, xMaxLowerVls;
		std::vector<int> xList;
		for (auto ls : lowerVls)
		{
			xList.push_back(ls.P1.x);
			xList.push_back(ls.P2.x);
		}
		xMinLowerVls = *std::min_element(xList.begin(), xList.end());
		xMaxLowerVls = *std::max_element(xList.begin(), xList.end());

		//グループ分け
		for (auto ls : hls)
		{
			if (ls.P1.x < xMinLowerVls || ls.P2.x < xMinLowerVls) leftHlsGroup.push_back(ls);
			if (xMaxLowerVls < ls.P1.x || xMaxLowerVls < ls.P2.x) rightHlsGroup.push_back(ls);
		}

		foundLeftHlsGroup = 0 < leftHlsGroup.size();
		foundRightHlsGroup = 0 < rightHlsGroup.size();
		foundOtherHlsGroup = 0 < otherHlsGroup.size();

		//leftHlsGroup に候補が存在した場合の処理
		//抽出のソースを設定
		std::vector<CLineSegment> leftHlsSource;
		if (foundLeftHlsGroup) leftHlsSource = leftHlsGroup;
		else if (foundOtherHlsGroup) leftHlsSource = otherHlsGroup;
		else leftHlsSource = hls;

		{
			//複数あったら、一番長いものを選ぶ
			auto longest = CLineSegment::GetLongestFrom(leftHlsSource);
			auto nearest = CLineSegment::GetLowest(leftHlsSource, refPoint1);
			if (lengthCoeff * longest.LineLength <= nearest.LineLength) leftHls = nearest;
			else leftHls = longest;
			Draw(leftHls, ImgWithCorners, RED);
		}


		//righttHlsGroup に候補が存在した場合の処理
		//抽出のソースを設定
		std::vector<CLineSegment> rightHlsSource;
		if (0 < foundRightHlsGroup) rightHlsSource = rightHlsGroup;
		else if (0 < foundOtherHlsGroup) rightHlsSource = otherHlsGroup;
		else rightHlsSource = hls;

		{
			//複数あったら、一番長いものを選ぶ
			auto longest = CLineSegment::GetLongestFrom(rightHlsSource);
			auto nearest = CLineSegment::GetLowest(rightHlsSource, refPoint1);
			if (lengthCoeff * longest.LineLength <= nearest.LineLength) rightHls = nearest;
			else rightHls = longest;
			Draw(rightHls, ImgWithCorners, BLUE);
		}

		if (foundLeftHlsGroup && foundRightHlsGroup  &&
			foundLeftVls && foundRightVls)
		{

			//コーナーを計算する
			auto leftCorner = geo::DetectCorners(std::vector<CLineSegment>{leftHls, leftVls}, ImageSetting.Torelance, ImageSetting.LowerThetaLimit);
			auto rightCorner = geo::DetectCorners(std::vector<CLineSegment>{rightHls, rightVls}, ImageSetting.Torelance, ImageSetting.LowerThetaLimit);

			//交点(コーナーのはず)を図示する
			if (0 < leftCorner.size() && 0 < rightCorner.size()  // コーナー座標の存在確認
				&& leftCorner[0][0] <= rightCorner[0][0])        // x座標の比較
			{
				std::stringstream pos1, pos2;
				pos1 << leftCorner[0];
				pos2 << rightCorner[0];
				Draw(leftCorner[0], ImgWithCorners, 10, 4, RED, pos1.str(), RED, 1.5, cv::Point(-180, 40));
				Draw(rightCorner[0], ImgWithCorners, 10, 4, BLUE, pos2.str(), BLUE, 1.5, cv::Point(10, 40));

				auto theta = rightHls.Theta - leftHls.Theta;
				std::stringstream cornerSs;
				cornerSs << "delta Theta = " << theta << "[deg]";
				cv::putText(ImgWithCorners, cornerSs.str(), cv::Point(20, 40), CV_FONT_HERSHEY_TRIPLEX, 1, RED, 2);

				auto displacement = rightCorner[0] - leftCorner[0];
				std::stringstream dispSs;
				dispSs << "Displacement=" << displacement << "[pixel]";
				cv::putText(ImgWithCorners, dispSs.str(), cv::Point(20, 80), CV_FONT_HERSHEY_TRIPLEX, 1, RED, 2);

			}
		}

	}

	if (DebugMode) cv::imshow("Detected corners", ImgWithCorners);
	cv::imwrite(fNamePrefix + "detectedCorners.jpg", ImgWithCorners);

	return ImgWithCorners;
}

/// <summary>直線を検出する。 </summary>
/// <param name="rhoResolution">ハフ変換でのρの解像度[pixel]</param>
/// <param name="thetaResolution">ハフ変換でのθの解像度[rad]</param>
/// <param name="threshold">累算して得られた結果を、直線とみなす閾値</param>
/// <param name="minLineLength">線分の最短長さ。それ未満の長さの線分は無視される。</param>
/// <param name="maxLineGap">同一直線上にある2点のギャップを結びつける最大の長さ。</param>
/// <returns>4要素ベクトル(x1,y1,x2,y2)のベクトル。線分の両端座標が(x1,y1), (x2,y2)であることを示す。</returns>
std::vector<cv::Vec4i> CImageProcessor::DetectLines(const cv::Mat blackAndWhiteImage,
	const double rhoResolution, const double thetaResolution,
	const int threshold, const double minLineLength, const double maxLineGap)
{
	//cout << rhoResolution << endl;

	std::vector<cv::Vec4i> lines;


	HoughLinesP(blackAndWhiteImage, lines, rhoResolution, thetaResolution, threshold, minLineLength, maxLineGap);

	return lines;
}


std::vector<cv::Vec4i> CImageProcessor::ExtractLines(const std::vector<cv::Vec4i> lsList, const cv::Rect r)
{
	std::vector<cv::Vec4i> resultList;
	for (auto v : lsList)
	{
		auto p1 = cv::Point(v[0], v[1]);
		auto p2 = cv::Point(v[2], v[3]);

		//TODO: 領域を通過するだけで端点が領域に含まれない線分も、リストへ追加するべき
		if (r.contains(p1) || r.contains(p2))
		{
			resultList.push_back(v);
		}
	}

	return resultList;
}

cv::Mat CImageProcessor::DrawLines(cv::Mat inputImage, std::vector<cv::Vec4i> lines)
{
	cv::Mat imageWithLines;
	cv::cvtColor(inputImage, imageWithLines, CV_GRAY2BGR);

	for (size_t i = 0; i < lines.size(); i++)
	{
		line(imageWithLines,
			cv::Point(lines[i][0], lines[i][1]),
			cv::Point(lines[i][2], lines[i][3]),
			cv::Scalar(0, 0, 255), 1, 8);
	}

	return imageWithLines;
}

/// <summary>2点(左上 topLeft, 右下 bottomRight)で指定された矩形領域内に含まれる点を抽出して返す。y軸は下向きが正であることに注意。 </summary>
/// <param name="topLeft"></param>
/// <param name="bottomRight"></param>
/// <param name="xyCandidate"></param>
/// <returns></returns>
std::vector<cv::Vec2i> CImageProcessor::ExtractCornersInRect(const std::vector<cv::Vec2i> xyCandidate, cv::Rect r)
{
	std::vector<cv::Vec2i> pointList;
	pointList.empty();

	for (cv::Point xy : xyCandidate)
	{
		if (r.contains(xy))
		{
			pointList.push_back(xy);
		}
	}
	return pointList;
}

cv::Mat CImageProcessor::DrawCircles(const cv::Mat image, const std::vector<cv::Vec2i> cornerXyList, int radius, int thickness)
{
	for (auto xy : cornerXyList)
	{
		cv::circle(image, xy, radius, cv::Scalar(0, 0, 255), thickness);
		putText(
			image,
			stringf("(%4d, %4d)", xy(0), xy(1)),
			cv::Point(xy),
			cv::FONT_HERSHEY_PLAIN,
			0.8,
			CV_RGB(0, 255, 0),
			1);
	}																						
	return image;
}

//カラーフレームを加工する
void CImageProcessor::DrawColorFrame()
{
	//カラーデータから角を検出して重ねて描画する(表示はしない)
	ImgWithCorners = DrawCorners("cam_param_center.xml", "");
}

std::string CImageProcessor::stringf(const char* format, ...)
{
	va_list args;
	va_start(args, format);

	int size = _vscprintf(format, args) + 1;
	std::unique_ptr<char[]> buf(new char[size]);

#ifndef _MSC_VER
	vsnprintf(buf.get(), size, format, args);
#else
	vsnprintf_s(buf.get(), size, _TRUNCATE, format, args);
#endif

	va_end(args);

	return std::string(buf.get());
}

//画像受け渡し用のパケットを作成する
//途中の処理で失敗したら、その時点で nullptr を返す。
//最後まで処理が完了したら、作成したパケットのアドレスを返す。
//後に、返り値(例えばアドレス ptr)で free(ptr)する必要がある
unsigned char* CImageProcessor::MakePacket(cv::Mat src, DWORD & packetLength)
{
	//パケットの例:
	//
	// 種類:   "Image,"
	// サイズ: "16,"
	// 幅:     "2,"
	// 高さ:   "2,"
	// 中身:   "0000FFFFFF0000FFFF0000FFFF0000FF"

	//種類
	std::string typeStr("Image");
	const char* typeChar = typeStr.c_str();
	char typeLength = strlen(typeChar);

	//サイズ
	int bmpSizeInt = src.rows * src.cols * 4; // *sizeof(RGBQUAD);
	std::string bmpSizeStr = std::to_string(bmpSizeInt);
	const char* bmpSizeChar = bmpSizeStr.c_str();
	auto bmpSizeStrLength = strlen(bmpSizeChar);

	//幅
	std::string bmpWidthStr = std::to_string(src.cols);
	const char* bmpWidthChar = bmpWidthStr.c_str();
	auto bmpWidthLength = strlen(bmpWidthChar);

	//高さ
	std::string bmpHeightStr = std::to_string(src.rows);
	const char* bmpHeightChar = bmpHeightStr.c_str();
	auto bmpHeighthLength = strlen(bmpHeightChar);

	//中身
	//最終的に送るバイト配列のアドレス
	unsigned char * packet;

	//パケット全体の長さを算出して、その長さのメモリを確保
	packetLength = DWORD(
		typeLength + 1 +
		bmpSizeStrLength + 1 +
		bmpWidthLength + 1 +
		bmpHeighthLength + 1 +
		bmpSizeInt);

	try
	{
		packet = (unsigned char *)malloc(packetLength);
	}
	catch (std::bad_alloc)
	{
		cout << "Error: メモリ割り当て(malloc)に失敗 in MakePacket()" << endl;
		return nullptr;
	}

	//各要素間のセパレータは ','
	char const sep = ',';

	//種類を書き込む
	unsigned char offset = 0;
	memcpy(packet + offset, typeChar, typeLength);
	offset += typeLength;
	packet[offset] = sep;
	offset += 1;

	//サイズを書き込む
	memcpy(packet + offset, bmpSizeChar, bmpSizeStrLength);
	offset += bmpSizeStrLength;
	packet[offset] = sep;
	offset += 1;

	//幅を書き込む
	memcpy(packet + offset, bmpWidthChar, bmpWidthLength);
	offset += bmpWidthLength;
	packet[offset] = sep;
	offset += 1;

	//高さを書き込む
	memcpy(packet + offset, bmpHeightChar, bmpHeighthLength);
	offset += bmpHeighthLength;
	packet[offset] = sep;
	offset += 1;

	//中身(画像のバイト配列)を書き込む
	convert2BmpByteArray(src, packet + offset);

	return packet;
}

//Mat形式 src をバイト配列 byteArray に格納する
//以下の入力・出力を想定
//入力: BGR 形式
//出力: BGRA形式(ByteArrayToImage()メソッドへ渡すため)
void CImageProcessor::convert2BmpByteArray(cv::Mat src, unsigned char* dstArray)
{
	CBitmap m_bmp;    //画像(bitmapファイル)

	//色の順番を入れ替える
	//Mat形式:    BGR
	//Bitmap形式: RGB
	for (auto y = 0; y < src.rows; y++) {
		for (auto x = 0; x < src.cols; x++) {
			// Blue
			dstArray[y * src.cols * 4 + x * 4 + 0] = src.data[y * src.step + x * 3 + 0];
			// Green
			dstArray[y * src.cols * 4 + x * 4 + 1] = src.data[y * src.step + x * 3 + 1];
			// Red
			dstArray[y * src.cols * 4 + x * 4 + 2] = src.data[y * src.step + x * 3 + 2];
			//Reserved
			dstArray[y * src.cols * 4 + x * 4 + 3] = 255;
		}
	}

	m_bmp.CreateBitmap(src.cols, src.rows, 1, 32, dstArray);
}

//取得したカラー画像をファイル fName に保存する
void CImageProcessor::saveToFile(std::string fName)
{
	cv::imwrite(fName, ImgRawColor);
}

//画像をファイルに連番(pictures/00.jpスタート)で保存する。始まり。
void CImageProcessor::saveToFile()
{
	//pictrues フォルダが存在しなければ、pictruesフォルダを作成する
	if (::GetFileAttributes(TEXT("pictures")) != FILE_ATTRIBUTE_DIRECTORY)
	{
		_mkdir("pictures");
	}

	char fName[32];
	sprintf_s(fName, "pictures/%02d.jpg", screenshotCount);
	cv::imwrite(fName, ImgRawColor);
	screenshotCount++;
}

//Mat形式の画像データをバイト配列に変換する
byte * CImageProcessor::cvt2BmpByteArray(cv::Mat & src)
{
	return nullptr;
}

bool CImageProcessor::LoadAffineMatrix(std::string fName)
{
	cv::FileStorage fs;
	if(fs.open(fName, cv::FileStorage::READ))
	{
		fs["AffineMatrix"] >> affineMat;
		fs.release();

		if (DebugMode) std::cout << "affineMat=" << affineMat << std::endl;
		return true;
	}
	return false;
}

bool CImageProcessor::SaveAffineMatrix(cv::Mat image0, int board_w, int board_h, std::string fName)
{
	assert(!fName.empty());

	//鳥観図を得るための行列を計算する
	cv::Mat A;
	GetAffineMatToGetTopView(image0, board_w, board_h, A);


	//カメラパラメータをファイルに書き出す
	cv::FileStorage fs(fName, cv::FileStorage::WRITE);
	if (fs.isOpened())
	{
		cv::write(fs, "AffineMatrix", A);
		
		fs.release();
		return true;
	}
	return false;
}

bool CImageProcessor::GetAffineMatToGetTopView(cv::Mat image0, int board_w, int board_h, cv::Mat& AffineMat)
{
	//鳥観図を得るためのアフィン行列を求める。
	//本当は投影行列を求めたいができなかった。とりあえずアフィン変換で。

	assert(image0.size() != cv::Size(0, 0));
	assert(0 < board_w);
	assert(0 < board_h);

	assert(intrinsicMatrix.size() != cv::Size(0, 0));
	assert(distortionCoeffs.size() != cv::Size(0, 0));

	int board_n = board_w*board_h;
	cv::Size board_sz(board_w, board_h);

	assert(intrinsicMatrix.size() != cv::Size(0, 0));
	assert(distortionCoeffs.size() != cv::Size(0, 0));
	
	cv::Mat grayImage, image;

	// 歪みを補正する
	cv::undistort(image0, image, intrinsicMatrix, distortionCoeffs);
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

	//チェッカーボードの面を得る
	std::vector<cv::Point2f> corners;
	bool found = cv::findChessboardCorners(  // True if found
		image,                               // Input image
		board_sz,                            // Pattern size
		corners,                             // Results
		cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_FILTER_QUADS
	);
	if(!found)
	{
		std::cout << "Coundn't acquire checkerboard on image, only found " << corners.size() << " of " << board_n << " corners\n";
		return false;
	}

	//得たコーナー座標をサブピクセルの精度で得る -- これは不要?
	cv::cornerSubPix(
		grayImage,         // Input Image
		corners,           // Initialguesses, also output
		cv::Size(11, 11),  // Searchi window size
		cv::Size(-1, -1),  // Zero zone (in this case, don't use)
		cv::TermCriteria(
			cv::TermCriteria::EPS | cv::TermCriteria::COUNT,
			30, 0.1)
	);

	//目的点(object point=objPts) と対応する画像上の点(imgPts)の座標を得る
	//座標の順番は、
	//0:左上
	//1:右上
	//2:左下
	//3:右下(アフィン変換では不使用)
	
	auto w = image0.size().width;
	auto h = image0.size().height;
	cv::Point2i objLeftTop(w / 2, 300), delta(640 , 400 );

	cv::Point2f objPts[4], imgPts[4];
	objPts[0].x = objLeftTop.x;            objPts[0].y = objLeftTop.y;
	objPts[1].x = objLeftTop.x + delta.x;  objPts[1].y = objLeftTop.y;
	objPts[2].x = objLeftTop.x;            objPts[2].y = objLeftTop.y + delta.y;
	//objPts[3].x = objLeftTop.x + delta.x;  objPts[1].y = objLeftTop.y + delta.y;

	imgPts[0] = corners[0];
	imgPts[1] = corners[board_w - 1];
	imgPts[2] = corners[(board_h - 1)*board_w + 0];
	//imgPts[3] = corners[(board_h - 1)*board_w + board_w - 1];


	//点を順番に描画する: B,G, R, Yellow 
	cv::circle(image, imgPts[0], 9, cv::Scalar(255, 0,   0), 3);
	cv::circle(image, imgPts[1], 9, cv::Scalar(0, 255,   0), 3);
	cv::circle(image, imgPts[2], 9, cv::Scalar(0,   0, 255), 3);
	//cv::circle(image, imgPts[3], 9, cv::Scalar(0, 255, 255), 3);

	cv::putText(image, std::string("[0]"), imgPts[0], CV_FONT_HERSHEY_TRIPLEX, 1.5, BLUE, 3);
	cv::putText(image, std::string("[1]"), imgPts[1], CV_FONT_HERSHEY_TRIPLEX, 1.5, GREEN, 3);
	cv::putText(image, std::string("[2]"), imgPts[2], CV_FONT_HERSHEY_TRIPLEX, 1.5, RED, 3);
	//cv::putText(image, std::string("[3]- not in Use"), imgPts[3], CV_FONT_HERSHEY_TRIPLEX, 1.5, YELLOW, 3);

	if (DebugMode)
	{
		cv::drawChessboardCorners(image, board_sz, corners, found);
		cv::imshow("Checkers", image);
	}

	//アフィン行列を得る
	AffineMat = cv::getAffineTransform(imgPts, objPts);

	if (DebugMode)
	{
		cv::Mat topView;
		cv::warpAffine(image, topView, AffineMat, image.size());
		cv::imshow("TopView", topView);
	}

	//問題なく行列を得られたので、 true を返す。
	return true;
}

bool CImageProcessor::SetIntrinsicMatrix(cv::Mat inputIntrinsicMatrix)
{
	this->intrinsicMatrix = inputIntrinsicMatrix;
	return true;
}

bool CImageProcessor::SetDistortionCoefficients(cv::Mat inputDistortionCoefficients)
{
	this->distortionCoeffs = inputDistortionCoefficients;
	return true;
}
