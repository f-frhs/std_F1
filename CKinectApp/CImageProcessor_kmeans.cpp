#include "CImageProcessor.h"
#include <Windows.h>
#include <opencv2/opencv.hpp>
#include "CUtil.h"

/// <summary>コーナー群 corner を numberOfGroups 個のグループにクラスタリングする。kmean clustering を使う。 </summary>
/// <param name="points">コーナーの座標を1つ以上もつベクトル</param>
/// <param name="numberOfGroup">最終的にグループわけしたい数</param>
/// <param name="groupNums">結果として、i番目のコーナーがグループ groupNum に属していることを表すベクトル</param>
/// <returns>各クラスターの重心座標を複数集めたベクトル</returns>
std::vector<cv::Vec2i> CImageProcessor::KmeansPoints(
	std::vector<cv::Vec2i> points0,
	int numberOfGroups,
	std::vector<int> &groupNums)
{
	//コーナ格納用Mat
	cv::Mat points = cv::Mat::zeros(points0.size(), 2, CV_32F);

	//コーナーをMatへ格納
	auto i = 0;
	for (auto point : points0)
	{
		points.at<float>(i, 0) = point[0];
		points.at<float>(i, 1) = point[1];
		i++;
	}

	std::stringstream ss;
	ss << "points=\n" << points << std::endl;
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
	
	cv::Mat clusters = cv::Mat::zeros(points.rows, 1, CV_32SC1);

	cv::Mat centers;

	//クラスタリング
	cv::kmeans(points,
		numberOfGroups,
		clusters,
		cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1.0),
		1,
		cv::KMEANS_PP_CENTERS, 
		centers);

	ss.str("");
	ss.clear(std::stringstream::goodbit);
	ss << "clusters=\n" << clusters << std::endl;
	ss << "centers=\n" << centers << std::endl;
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	std::vector<cv::Vec2i> retCenters;
	for(auto ci=0;ci<centers.rows;ci++)
	{
		auto x = centers.at<float>(ci, 0);
		auto y = centers.at<float>(ci, 1);
		retCenters.push_back(cv::Vec2i{ (int)x,(int)y });
	}

	return  retCenters;

}

/// <summary>複数の整数 ints  を numberOfGroups 個のグループにクラスタリングする。kmean clustering を使う。 </summary>
/// <param name="ints">整数1つ以上からなるベクトル</param>
/// <param name="numberOfGroup">最終的にグループ分けしたい数</param>
/// <param name="groupNums">結果として、i番目のコーナーがグループ groupNum に属していることを表すベクトル</param>
/// <returns>各クラスターの重心座標を複数集めたベクトル</returns>
std::vector<int> CImageProcessor::KmeansInts(
	std::vector<int> ints0,
	int numberOfGroups,
	std::vector<int> &groupNums)
{
	//コーナ格納用Mat
	cv::Mat ints = cv::Mat::zeros(ints0.size(), 1, CV_32F);

	//コーナーをMatへ格納
	auto i = 0;
	for (auto point : ints0)
	{
		ints.at<float>(i, 0) = point;
		i++;
	}

	std::stringstream ss;
	ss << "points=\n" << ints << std::endl;
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	//以下の初期値を与える:
	//- 最大値だけクラスター1に所属
	//- 残りの要素はクラスター0に所属
	cv::Mat clusters = cv::Mat::zeros(ints.rows, 1, CV_32SC1);
	std::vector<int>::iterator maxIt = std::max_element(ints0.begin(), ints0.end());
	size_t maxIndex = std::distance(ints0.begin(), maxIt);
	clusters.at<int>(maxIndex, 0) = 1;

	cv::Mat centers;

	//クラスタリング
	cv::kmeans(ints,
		numberOfGroups,
		clusters,
		cvTermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 10, 1.0),
		1,
		cv::KMEANS_USE_INITIAL_LABELS,
		//cv::KMEANS_PP_CENTERS,
		centers);

	ss.str("");
	ss.clear(std::stringstream::goodbit);
	ss << "clusters=\n" << clusters << std::endl;
	ss << "centers=\n" << centers << std::endl;
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	for(auto i=0; i< clusters.rows; i++)
	{
		groupNums[i] = clusters.at<int>(i);
	}

	std::vector<int> retCenters;
	for (auto ci = 0; ci<centers.rows; ci++)
	{
		auto x = centers.at<float>(ci, 0);
		//auto y = centers.at<float>(ci, 1);
		retCenters.push_back((int)x);
	}

	return  retCenters;

}