#include "CImageProcessor.h"
#include <memory>
#include <filesystem>
#include <iostream>
#include "geo.h"
#include "CUtil.h"
#include <opencv2/opencv.hpp>

/// <summary>キャリブレーションを行う</summary>
/// <param name="fName">保存先ファイル名(キャリブレーション用xmlファイル)</param>
/// <returns></returns>
int CImageProcessor::Calibrate(std::string fName,int camID, std::string sensorName, std::string lenseName)
{
	CameraParamsFile = fName;	//カメラパラメータのファイル
	CamID = camID;				//カメラID

	auto ret = Calibrate(sensorName, lenseName);

	return ret;
}

//キャリブレーションを行う。結果は、ファイル名 fileNameCameraParameters に格納される。
//int	CImageProcessor::Calibrate(std::string sensorName, std::string lenseName)
//{
//	//ウィンドウ名やファイル名に関するパラメータ
//	const std::string windowNameSrc = "Source";
//	const std::string windowNameUnd = "Undistorted Image";
//
//	const int numOfCheckerPatterns = 8;		//チェッカーパターンが何枚あるか
//	const cv::Size patternSize(8, 6);		//チェッカーパターンの交点の数
//	const float sizeOfSquare = 43.5;		//チェッカーパターンのマス目のサイズ [mm]
//
//	std::string fNameCamParams;
//
//	//ファイル名が設定されているかをチェックする
//	if (CameraParamsFile == std::string(""))
//		fNameCamParams = "cam_param.xml";
//	else
//		fNameCamParams = CameraParamsFile;
//
//	//チェッカーパターンに関する変数とパラメータ
//	std::vector<cv::Mat> checkerImgs;      //チェッカーパターン画像
//
//	//座標に関する変数
//	std::vector<std::vector<cv::Point3f> > worldPoints(numOfCheckerPatterns); //チェッカー交点座標と対応する世界座標の値を格納する行列
//	std::vector<std::vector<cv::Point2f> > imagePoints(numOfCheckerPatterns); //チェッカー交点座標を格納する行列
//
//	//カメラキャリブレーションのパラメータ
//	cv::TermCriteria criteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.001);
//
//	//カメラパラメータ行列
//	cv::Mat cameraMatrix;		                //内部パラメータ行列
//	cv::Mat distCoeffs;			                //レンズ歪み行列
//	std::vector<cv::Mat> rotationVectors;	    //撮影画像ごとに得られる回転ベクトル
//	std::vector<cv::Mat> translationVectors;	//撮影画像ごとに得られる平行移動ベクトル
//	cv::Mat stdDeviationsIntrinsics;     //intrinsicの標準偏差
//	cv::Mat stdDeviationsExtrinsics;     //intrinsicの標準偏差
//	std::vector<double> perViewErrors;
//
//	//チェッカーパターン画像をファイルから読み込む
//	for (auto i = 0; i < numOfCheckerPatterns; i++)
//	{
//		//通し番号からファイル名を生成
//		std::stringstream ssFName;
//		//ssFName << "pictures/" << std::setfill('0') << std::setw(2) << i << ".bmp";
//		ssFName << "pictures/" <<std::string("CamID")<<std::to_string(CamID)<<"_" << std::setfill('0') << std::setw(2) << i << ".bmp";
//		auto fileName = ssFName.str();
//
//		//読み込んだ画像をベクトルに保存
//		checkerImgs.push_back(cv::imread(fileName));
//		//std::cout << "チェッカーパターン読み込み: " << fileName << std::endl;
//
//		std::stringstream ss;
//		ss << std::string(u8"チェッカーパターン読み込み :") << ssFName.str() << std::endl;
//		//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
//		//auto ws = cv.from_bytes(ss.str());
//		OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
//	}
//
//	//チェックパターンの交点座標を求め，imagePointsに格納する
//	for (auto i = 0; i < numOfCheckerPatterns; i++)
//	{
//		//std::cout << i + 1 << "番目の画像からチェッカーパターンの交点を検出します";
//		std::stringstream ss;
//		ss << i + 1 << u8"番目の画像からチェッカーパターンの交点を検出します: ";
//		OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
//
//		if (cv::findChessboardCorners(checkerImgs[i], patternSize, imagePoints[i]))
//		{
//			//std::cout << " - 交点をすべて検出しました" << std::endl;
//			OutputDebugString(L"交点をすべて検出しました\n");
//
//			//検出点を描画する
//			cv::drawChessboardCorners(checkerImgs[i], patternSize, imagePoints[i], true);
//
//			//デバッグフラグがtrueの場合は、画像を表示し、保存する
//			if (DebugMode == true)
//			{
//				cv::imshow(windowNameSrc, checkerImgs[i]);
//				cv::waitKey(500);
//
//				std::stringstream debugWriteFNameSS;
//				debugWriteFNameSS << "DrawChessCorners_" << i << ".bmp";
//				cv::imwrite(debugWriteFNameSS.str(), checkerImgs[i]);
//			}
//
//			//参考: サブピクセル単位までコーナー検出精度を上げる場合は下記のコメントを解除する
//			//cv::Mat	gray;
//			//cv::cvtColor( checkerImgs[i], gray, cv::COLOR_BGR2GRAY );
//			//cv::cornerSubPix( gray, imagePoints[i], cv::Size( 11, 11 ), cv::Size( -1, -1 ), criteria );
//		}
//		else
//		{
//			//std::cout << " - 検出されなかった交点があります" << std::endl;
//			OutputDebugString(L"検出されなかった交点があります\n");
//			cv::waitKey(0);
//			return -1;
//		}
//	}
//
//	//世界座標におけるチェッカーパターンの交点座標を決定
//	for (auto i = 0; i < numOfCheckerPatterns; i++)
//	{
//		for (auto j = 0; j < patternSize.area(); j++)
//		{
//			auto cx = static_cast<float>(j % patternSize.width * sizeOfSquare);
//			auto cy = static_cast<float>(j / patternSize.width * sizeOfSquare);
//			auto cz = static_cast<float>(0.0);
//
//			worldPoints[i].push_back(cv::Point3f(cx, cy, cz));
//			//worldPoints[i].push_back(
//			//	cv::Point3f(static_cast<float>(j % patternSize.width * sizeOfSquare),
//			//	static_cast<float>(j / patternSize.width * sizeOfSquare),
//			//	0.0));
//		}
//	}
//
//	//Zhangの手法によるキャリブレーション
//	auto reprojectionError
//		= cv::calibrateCamera(
//			worldPoints,
//			imagePoints,
//			checkerImgs[0].size(),
//			cameraMatrix,
//			distCoeffs,
//			rotationVectors,
//			translationVectors,
//			stdDeviationsIntrinsics,
//			stdDeviationsExtrinsics,
//			perViewErrors,
//			0,
//			cv::TermCriteria(cv::TermCriteria::COUNT + cv::TermCriteria::EPS, INT_MAX, DBL_EPSILON));
//
//	//カメラキャリブレーションの誤差を表示する
//	//推定したカメラ行列を使って3次元座標を2次元へ変換し、
//	//キャリブレーション前のオリジナル画像上の点座標と比較。
//	//距離の二乗和 = reprojectionError
//	std::stringstream rpjErrorSs;
//	rpjErrorSs << "reprojection error=" << reprojectionError;
//	OutputDebugString(CUtil::StringToWString(rpjErrorSs.str()).c_str());
//	
//	
//	//回転ベクトルと平行移動ベクトルを4行4列の外部パラメータ行列に書き換える
//	//なお、ここではZhangのキャリブレーション手法における1枚目の外部パラメータ行列のみ出力する
//
//	//出力先となる4行4列の外部パラメータ行列。単位行列で初期化
//	cv::Mat	extrinsic(4, 4, CV_64F);
//	cv::setIdentity(extrinsic);
//
//	//回転ベクトルを回転行列に変換し、外部パラメータ行列に格納
//	cv::Rodrigues(rotationVectors[0], extrinsic(cv::Rect(0, 0, 3, 3)));
//
//	//平行移動ベクトルを外部パラメータ行列に格納
//	translationVectors[0].copyTo(extrinsic(cv::Rect(3, 0, 1, 3)));
//
//	//カメラパラメータをファイルに書き出す
//	cv::FileStorage fswrite(fNameCamParams, cv::FileStorage::WRITE);
//	if (fswrite.isOpened())
//	{
//		cv::write(fswrite, "SensorSN", sensorName);
//		cv::write(fswrite, "LenseSN", lenseName);
//		cv::write(fswrite, "ReprojectionError", reprojectionError);
//		cv::write(fswrite, "numOfCheckerPatterns", numOfCheckerPatterns);
//		cv::write(fswrite, "extrinsic", extrinsic);
//		cv::write(fswrite, "intrinsic", cameraMatrix);
//		cv::write(fswrite, "distortion", distCoeffs);
//		cv::write(fswrite, "stdDevationsExtrinsics", stdDeviationsExtrinsics);
//		cv::write(fswrite, "stdDevationsIntrinsics", stdDeviationsIntrinsics);
//		cv::write(fswrite, "stdDevationsIntrinsics", stdDeviationsIntrinsics);
//		cv::write(fswrite, "perViewErrors", perViewErrors);
//
//		fswrite.release();
//	}
//
//	if (DebugMode == true)
//	{
//		//歪み補正した画像を表示
//		cv::Mat	undistorted;
//		for (auto i = 0; i < numOfCheckerPatterns; i++)
//		{
//			//レンズ歪み係数を用いて画像の歪みを補正
//			cv::undistort(checkerImgs[i], undistorted, cameraMatrix, distCoeffs);
//			// 歪み補正前と補正後の画像を表示
//			cv::imshow(windowNameUnd, undistorted);
//			cv::imshow(windowNameSrc, checkerImgs[i]);
//			cv::waitKey(1);
//
//			//ファイルに保存
//			std::stringstream undisFNameSS;
//			undisFNameSS << "UndisImage_" << i << ".bmp";
//			cv::imwrite(undisFNameSS.str(), undistorted);
//
//			std::stringstream undisSrcFNameSS;
//			undisSrcFNameSS << "UndisSrcImage_" << i << ".bmp";
//			cv::imwrite(undisSrcFNameSS.str(), checkerImgs[i]);
//		}
//	}
//
//	return 0;
//}

/// <summary>入力された画像から、チェッカーボードまでの同時変換行列を求める</summary>
/// <param name="sourceImg"></param>
/// <returns></returns>
cv::Mat CImageProcessor::GetChessBoardPose(cv::Mat sourceImg)
{
	const cv::Size patternSize(10, 7);		//チェッカーパターンの交点の数
	const float sizeOfSquare = 24.0;		//チェッカーパターンのマス目のサイズ [mm]

	//入力画像の歪みを除去する
	//歪みを補正する
	cv::undistort(sourceImg, ImgUndistorted, intrinsicMatrix, distortionCoeffs);

	if (DebugMode == true)
	{
		cv::imshow("Undistorted image", ImgUndistorted);
		cv::imwrite(CUtil::getDateTimeString() + "_undis_chess_matrix.bmp", ImgUndistorted);
	}

	//座標に関する変数
	std::vector<cv::Point3f> worldPoints; //チェッカー交点座標と対応する世界座標の値を格納する行列
	std::vector<cv::Point2f> imagePoints; //チェッカー交点座標を格納する行列

	//世界座標におけるチェッカーパターンの交点座標を決定
	for (auto i = 0; i < patternSize.area(); i++)
	{
		auto cx = static_cast<float>(i % patternSize.width * sizeOfSquare);
		auto cy = static_cast<float>(i / patternSize.width * sizeOfSquare);
		auto cz = static_cast<float>(0.0);

		worldPoints.push_back(cv::Point3f(cx, cy, cz));
	}

	//チェックパターンの交点座標を求め，imagePointsに格納する
	std::stringstream ss;
	ss << u8"画像からチェッカーパターンの交点を検出します";
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	if (cv::findChessboardCorners(ImgUndistorted, patternSize, imagePoints))
	{
		OutputDebugString(L"交点をすべて検出しました");

		//検出点を描画する
		cv::drawChessboardCorners(ImgUndistorted, patternSize, imagePoints, true);

		//デバッグフラグがtrueの場合は、画像を表示し、保存する
		if (DebugMode == true)
		{
			cv::imshow("check", ImgUndistorted);
			cv::waitKey(500);

			std::stringstream debugWriteFNameSS;
			debugWriteFNameSS << CUtil::getDateTimeString() << "_DrawChessCorners" << ".bmp";
			cv::imwrite(debugWriteFNameSS.str(), ImgUndistorted);
		}

		//参考: サブピクセル単位までコーナー検出精度を上げる場合は下記のコメントを解除する
		//cv::Mat	gray;
		//cv::cvtColor( checkerImgs[i], gray, cv::COLOR_BGR2GRAY );
		//cv::cornerSubPix( gray, imagePoints[i], cv::Size( 11, 11 ), cv::Size( -1, -1 ), criteria );
	}
	else
	{
		OutputDebugString(L"検出されなかった交点があります");
		//cv::waitKey(0);
		return cv::Mat();
	}

	cv::Mat rvecEst;
	cv::Mat tvecEst;
	cv::solvePnPRansac(worldPoints, imagePoints, intrinsicMatrix, distortionCoeffs, rvecEst, tvecEst, false);

	cv::Mat rmat;
	cv::Rodrigues(rvecEst, rmat);

	//4X4行列の同時変換行列に変換する
	cv::Mat retMatrix = cv::Mat::eye(4, 4, CV_64F);
	for(auto i=0;i<rmat.rows;i++)
	{
		for(auto j=0;j<rmat.cols;j++)
		{
			retMatrix.at<double>(i, j) = rmat.at<double>(i, j);
		}
	}

	retMatrix.at<double>(0, 3) = tvecEst.at<double>(0);
	retMatrix.at<double>(1, 3) = tvecEst.at<double>(1);
	retMatrix.at<double>(2, 3) = tvecEst.at<double>(2);

	ss.str("");
	ss.clear(std::stringstream::goodbit);
	ss << retMatrix << std::endl;
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	return retMatrix;
}

/// <summary>画像内のチェスボードまでの同時変換行列を求める</summary>
/// <param name="sourceImg"></param>
/// <param name="imagePoints"></param>
/// <returns></returns>
cv::Mat CImageProcessor::GetChessBoardPose(cv::Mat sourceImg, std::vector<cv::Point2f>& imagePoints)
{
	//チェスボード関係の設定値を取得する
	int chessCrossWidth = ImageSetting.ChessCrossWidth;
	int chessCrossHeight = ImageSetting.CHessCrossHeight;
	double sizeOfSquare = ImageSetting.ChessSquareSize;
	int offsetX = ImageSetting.OffsetX;
	int offsetY = ImageSetting.OffsetY;

	const cv::Size patternSize(chessCrossWidth, chessCrossHeight);		//チェッカーパターンの交点の数

	//const cv::Size patternSize(9, 6);		//チェッカーパターンの交点の数
	//const float sizeOfSquare = 24.0;		//チェッカーパターンのマス目のサイズ [mm]

											//入力画像の歪みを除去する
											//歪みを補正する
	cv::undistort(sourceImg, ImgUndistorted, intrinsicMatrix, distortionCoeffs);
	//ImgUndistorted = sourceImg;

	if (DebugMode == true)
	{
		cv::imshow("Undistorted image", ImgUndistorted);
		cv::imwrite(CUtil::getDateTimeString() + "_undis_chess_matrix.bmp", ImgUndistorted);
	}

	//座標に関する変数
	std::vector<cv::Point3f> worldPoints; //チェッカー交点座標と対応する世界座標の値を格納する行列
	//std::vector<cv::Point2f> imagePoints; //チェッカー交点座標を格納する行列

										  //世界座標におけるチェッカーパターンの交点座標を決定
	for (auto i = 0; i < patternSize.area(); i++)
	{
		auto cx = static_cast<float>(i % patternSize.width * sizeOfSquare);
		auto cy = static_cast<float>(i / patternSize.width * sizeOfSquare);
		auto cz = static_cast<float>(0.0);

		worldPoints.push_back(cv::Point3f(cx, cy, cz));
	}

	//チェックパターンの交点座標を求め，imagePointsに格納する
	std::stringstream ss;
	ss << u8"画像からチェッカーパターンの交点を検出します";
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	if (cv::findChessboardCorners(ImgUndistorted, patternSize, imagePoints))
	{
		OutputDebugString(L"交点をすべて検出しました");

		//検出点を描画する
		cv::drawChessboardCorners(ImgUndistorted, patternSize, imagePoints, true);

		//デバッグフラグがtrueの場合は、画像を表示し、保存する
		if (DebugMode == true)
		{
			cv::imshow("check", ImgUndistorted);
			cv::waitKey(500);

			std::stringstream debugWriteFNameSS;
			debugWriteFNameSS << CUtil::getDateTimeString() << "_DrawChessCorners" << ".bmp";
			cv::imwrite(debugWriteFNameSS.str(), ImgUndistorted);
		}

		//参考: サブピクセル単位までコーナー検出精度を上げる場合は下記のコメントを解除する
		//cv::Mat	gray;
		//cv::cvtColor( checkerImgs[i], gray, cv::COLOR_BGR2GRAY );
		//cv::cornerSubPix( gray, imagePoints[i], cv::Size( 11, 11 ), cv::Size( -1, -1 ), criteria );
	}
	else
	{
		OutputDebugString(L"検出されなかった交点があります");
		//cv::waitKey(0);
		return cv::Mat();
	}

	cv::Mat rvecEst;
	cv::Mat tvecEst;
	cv::solvePnPRansac(worldPoints, imagePoints, intrinsicMatrix, distortionCoeffs, rvecEst, tvecEst, false);

	cv::Mat rmat;
	cv::Rodrigues(rvecEst, rmat);

	//4X4行列の同時変換行列に変換する
	cv::Mat retMatrix = cv::Mat::eye(4, 4, CV_64F);
	for (auto i = 0; i<rmat.rows; i++)
	{
		for (auto j = 0; j<rmat.cols; j++)
		{
			retMatrix.at<double>(i, j) = rmat.at<double>(i, j);
		}
	}

	retMatrix.at<double>(0, 3) = tvecEst.at<double>(0);
	retMatrix.at<double>(1, 3) = tvecEst.at<double>(1);
	retMatrix.at<double>(2, 3) = tvecEst.at<double>(2);

	ss.str("");
	ss.clear(std::stringstream::goodbit);
	ss << retMatrix;
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	return retMatrix;
}


