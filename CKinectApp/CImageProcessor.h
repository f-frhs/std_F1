#ifndef CIMAGE_PROCESSOR_H
#define CIMAGE_PROCESSOR_H


//表示用の色を宣言
#define BLUE   cv::Scalar(255,0,0)
#define GREEN  cv::Scalar(0,255,0)
#define RED    cv::Scalar(0,0,255)
#define YELLOW cv::Scalar(0,200,200)
#define BLACK  cv::Scalar(0,0,0)
#define LIGHTBULE cv::Scalar(25 ,	135, 	22)
#define PANSY  cv::Scalar(127, 0 ,73)
#define LILAC  cv::Scalar(224,132,213)
#define LEMON cv::Scalar(209, 	232, 	41)
#define WHITE cv::Scalar(255,255,255)


#pragma warning(disable: 4819)
#include <opencv2/opencv.hpp>
#pragma warning(default: 4819)

#include <afxwin.h>
#include <memory>
#include <direct.h>
#include "geo.h"
#include "Setting.h"
#include <string>
#include "CCorners.h"

class CImageProcessor
{
private:
	/// <summary>何枚目のスクリーンショットかを示す数(0から始まる) </summary>
	int screenshotCount;

	/// <summary>カメラ内部パラメータ行列</summary>
	cv::Mat intrinsicMatrix;

	/// <summary>真上から見た鳥観図へ変換するためのアフィン行列</summary>
	cv::Mat affineMat;

	/// <summary>レンズ収差補正用パラメータ行列</summary>
	cv::Mat distortionCoeffs;

	std::string stringf(const char * format, ...);

	void CImageProcessor::convert2BmpByteArray(cv::Mat src, unsigned char* dstArray);

	void CImageProcessor::saveToFile(std::string fName);

	void CImageProcessor::saveToFile();

	
public:
	/// <summary>設定を格納する構造体</summary>
	ImageProcessSetting ImageSetting;

	/// <summary>カメラから得た2Dカラー画像の配列 </summary>
	cv::Mat ImgRawColor;

	/// <summary>カメラの内部パラメータおよびレンズ歪みに起因する歪みを補正したカラー画像の配列 </summary>
	cv::Mat ImgUndistorted;

	/// <summary>出力画像サイズへと縮小されたカラー画像の配列 </summary>
	cv::Mat ImgResized;

	/// <summary>真上から見下ろした鳥観図</summary>
	cv::Mat ImgTopView;

	/// <summary>グレースケール化した画像の配列 </summary>
	cv::Mat ImgGray;

	/// <summary>二値化した画像の配列 </summary>
	cv::Mat ImgBlackAndWhite;

	/// <summary>二値化した画像に、検出した直線を重ねて描画した画像の配列 </summary>
	cv::Mat ImgWithLines;

	/// <summary>カメラから得た2Dカラー画像に、検出したコーナーを重ねて描画した画像の配列 </summary>
	cv::Mat ImgWithCorners;

	/// <summary> デバッグモード。true なら、途中経過の画像を各ウィンドウで表示する。 </summary>
	bool DebugMode;

	/// <summary>Captureコマンドが完了したか否かを示すフラグ。完了した場合は true となる。そうでない場合は false。</summary>
	bool EndCapFlg;

	/// <summary>内部パラメータ(カメラ、レンズパラメータ)を保存するファイル名 </summary>
	std::string CameraParamsFile;

	/// <summary>カメラID </summary>
	int CamID;

	/// <summary>真上から見た画像(ImgTopView)へ変換する必要の要否。ある場合はtrue。</summary>
	bool HasToGetTopView;

	/// <summary>コンストラクタ</summary>
	CImageProcessor::CImageProcessor();

	void CImageProcessor::SetSetting(ImageProcessSetting &s);

	/// <summary>ImgRawColorに代入された画像を元にして、 ImgBlackAndWhite を得る一連の処理を行う。</summary>
	/// <param name="calibResultPath">カメラキャリブレーション結果を保存したxmlファイルのパス</param>
	/// <param name="fNamePrefix">各処理の途中経過を示す画像ファイルを保存する際に、ファイル名はこの文字列から始まる。</param>
	void Preprocess(std::string calibResultPath, std::string fNamePrefix);

	cv::Mat CImageProcessor::DrawCorners(std::string calibResultPath, std::string fNamePrefix);

	std::vector<cv::Vec4i> CImageProcessor::DetectLines(const cv::Mat blackAndWhiteImage,
		const double rhoResolution = 1, const double thetaResolution = CV_PI / 180,
		const int threshold = 80, const double minLineLength = 30, const double maxLineGap = 10);

	std::vector<cv::Vec4i> CImageProcessor::ExtractLines(const std::vector<cv::Vec4i> lsList, const cv::Rect r);

	cv::Mat DrawLines(cv::Mat inputImage, std::vector<cv::Vec4i> lines);

	std::vector<cv::Vec2i> ExtractCornersInRect(const std::vector<cv::Vec2i> xyCandidate, cv::Rect r);

	cv::Mat DrawCircles(const cv::Mat image, const std::vector<cv::Vec2i> cornerXyList, int radius, int thickness);

	void CImageProcessor::DrawColorFrame();

	unsigned char* CImageProcessor::MakePacket(cv::Mat src, DWORD & packetLength);

	CCorners Corners;

	/// <summary>Mat形式の画像データをバイト配列に変換する </summary>
	/// <param name="src">画像データ</param>
	/// <returns></returns>
	byte* cvt2BmpByteArray(cv::Mat &src);

	/// <summary>計算済みのxmlファイルを読み込み、 affineMat へ代入する</summary>
	bool LoadAffineMatrix(std::string fName);

	/// <summary>鳥観図を得るためのアフィン行列を計算して、xmlファイルへ保存する </summary>
	/// <param name="image0"></param>
	/// <param name="board_w"></param>
	/// <param name="board_h"></param>
	/// <param name="fName"></param>
	/// <returns></returns>
	bool SaveAffineMatrix(cv::Mat image0, int board_w, int board_h, std::string fName);

	/// <summary> 真上から見下ろした図(bird's-eye view)を得るためのアフィン行列を得る。成功すればtrueを返す。そうでなければ falaseを返す。</summary>
	/// <param name="image0">斜めから見た(処理前の)画像</param>
	/// <param name="board_w">チェスボードの横[マス]</param>
	/// <param name="board_h">チェスボードの縦[マス]</param>
	/// <param name="AffineMat">(出力用)今後視点変換する際に使うアフィン行列</param>
	/// <returns>問題なく行列を取得できたか否か。</returns>
	bool GetAffineMatToGetTopView(cv::Mat image0, int board_w, int board_h, cv::Mat& AffineMat);

	void DrawLines(cv::Mat & dest, std::vector<CLineSegment> noDupulicatedLineSegments, cv::Scalar lilneColor, int circleSize);
	void Draw(cv::Point2i pos, cv::Mat& dst, int radius, int thickness, const cv::Scalar color, std::string label, const cv::Scalar labelColor, int labelSize, cv::Point offset=cv::Point(10,0));

	void Draw(CLineSegment ls, cv::Mat & dst, cv::Scalar lineColor = cv::Scalar(0, 255, 0, 0), int thickness=2, std::string label = "", int radius=2, cv::Scalar startingPointColor=cv::Scalar(255,0,0,0), cv::Scalar endingPointColor=cv::Scalar(0,0,255,0), int startingPoitCircleSize=2, int endingPointCircleSize=2, int circleThickness=1, const cv::Scalar labelColor=cv::Scalar(0,00,0), int labelSize=1.5, cv::Point labelOffset=cv::Point(10,0));

	/// <summary>カメラ(camID)の キャリブレーションを行う。センサー名、レンズ名も結果の xmlファイル(fNameで指定)に書き込む。</summary>
	int Calibrate(std::string fName, int camID, std::string sensorName = std::string("noNameSensor"), std::string lenseName = std::string("noNameLense"));

	/// <summary>カメラキャリブレーションを行う。センサー名、レンズ名も結果の xmlファイルに書き込む。</summary>
	int Calibrate(std::string sensorName=std::string("noNameSensor"), std::string lenseName=std::string("noNameLense"));
	cv::Mat GetChessBoardPose(cv::Mat sourceImg);
	cv::Mat GetChessBoardPose(cv::Mat sourceImg, std::vector<cv::Point2f>& imagePoints);
	bool SetIntrinsicMatrix(cv::Mat inputIntrinsicMatrix);
	bool SetDistortionCoefficients(cv::Mat inputDistortionCoefficients);
	std::vector<cv::Vec2i> KmeansPoints(std::vector<cv::Vec2i> points0, int numberOfGroups, std::vector<int>& groupNums);
	std::vector<int> KmeansInts(std::vector<int> ints0, int numberOfGroups, std::vector<int>& groupNums);
};

#endif // !CIMAGE_PROCESSOR_H
