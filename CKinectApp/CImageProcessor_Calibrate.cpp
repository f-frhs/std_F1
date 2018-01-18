#include "CImageProcessor.h"
#include <memory>
#include <filesystem>
#include <iostream>
#include "geo.h"
#include "CUtil.h"
#include <opencv2/opencv.hpp>

/// <summary>�L�����u���[�V�������s��</summary>
/// <param name="fName">�ۑ���t�@�C����(�L�����u���[�V�����pxml�t�@�C��)</param>
/// <returns></returns>
int CImageProcessor::Calibrate(std::string fName,int camID, std::string sensorName, std::string lenseName)
{
	CameraParamsFile = fName;	//�J�����p�����[�^�̃t�@�C��
	CamID = camID;				//�J����ID

	auto ret = Calibrate(sensorName, lenseName);

	return ret;
}

//�L�����u���[�V�������s���B���ʂ́A�t�@�C���� fileNameCameraParameters �Ɋi�[�����B
//int	CImageProcessor::Calibrate(std::string sensorName, std::string lenseName)
//{
//	//�E�B���h�E����t�@�C�����Ɋւ���p�����[�^
//	const std::string windowNameSrc = "Source";
//	const std::string windowNameUnd = "Undistorted Image";
//
//	const int numOfCheckerPatterns = 8;		//�`�F�b�J�[�p�^�[�����������邩
//	const cv::Size patternSize(8, 6);		//�`�F�b�J�[�p�^�[���̌�_�̐�
//	const float sizeOfSquare = 43.5;		//�`�F�b�J�[�p�^�[���̃}�X�ڂ̃T�C�Y [mm]
//
//	std::string fNameCamParams;
//
//	//�t�@�C�������ݒ肳��Ă��邩���`�F�b�N����
//	if (CameraParamsFile == std::string(""))
//		fNameCamParams = "cam_param.xml";
//	else
//		fNameCamParams = CameraParamsFile;
//
//	//�`�F�b�J�[�p�^�[���Ɋւ���ϐ��ƃp�����[�^
//	std::vector<cv::Mat> checkerImgs;      //�`�F�b�J�[�p�^�[���摜
//
//	//���W�Ɋւ���ϐ�
//	std::vector<std::vector<cv::Point3f> > worldPoints(numOfCheckerPatterns); //�`�F�b�J�[��_���W�ƑΉ����鐢�E���W�̒l���i�[����s��
//	std::vector<std::vector<cv::Point2f> > imagePoints(numOfCheckerPatterns); //�`�F�b�J�[��_���W���i�[����s��
//
//	//�J�����L�����u���[�V�����̃p�����[�^
//	cv::TermCriteria criteria(cv::TermCriteria::MAX_ITER | cv::TermCriteria::EPS, 20, 0.001);
//
//	//�J�����p�����[�^�s��
//	cv::Mat cameraMatrix;		                //�����p�����[�^�s��
//	cv::Mat distCoeffs;			                //�����Y�c�ݍs��
//	std::vector<cv::Mat> rotationVectors;	    //�B�e�摜���Ƃɓ������]�x�N�g��
//	std::vector<cv::Mat> translationVectors;	//�B�e�摜���Ƃɓ����镽�s�ړ��x�N�g��
//	cv::Mat stdDeviationsIntrinsics;     //intrinsic�̕W���΍�
//	cv::Mat stdDeviationsExtrinsics;     //intrinsic�̕W���΍�
//	std::vector<double> perViewErrors;
//
//	//�`�F�b�J�[�p�^�[���摜���t�@�C������ǂݍ���
//	for (auto i = 0; i < numOfCheckerPatterns; i++)
//	{
//		//�ʂ��ԍ�����t�@�C�����𐶐�
//		std::stringstream ssFName;
//		//ssFName << "pictures/" << std::setfill('0') << std::setw(2) << i << ".bmp";
//		ssFName << "pictures/" <<std::string("CamID")<<std::to_string(CamID)<<"_" << std::setfill('0') << std::setw(2) << i << ".bmp";
//		auto fileName = ssFName.str();
//
//		//�ǂݍ��񂾉摜���x�N�g���ɕۑ�
//		checkerImgs.push_back(cv::imread(fileName));
//		//std::cout << "�`�F�b�J�[�p�^�[���ǂݍ���: " << fileName << std::endl;
//
//		std::stringstream ss;
//		ss << std::string(u8"�`�F�b�J�[�p�^�[���ǂݍ��� :") << ssFName.str() << std::endl;
//		//std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;
//		//auto ws = cv.from_bytes(ss.str());
//		OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
//	}
//
//	//�`�F�b�N�p�^�[���̌�_���W�����߁CimagePoints�Ɋi�[����
//	for (auto i = 0; i < numOfCheckerPatterns; i++)
//	{
//		//std::cout << i + 1 << "�Ԗڂ̉摜����`�F�b�J�[�p�^�[���̌�_�����o���܂�";
//		std::stringstream ss;
//		ss << i + 1 << u8"�Ԗڂ̉摜����`�F�b�J�[�p�^�[���̌�_�����o���܂�: ";
//		OutputDebugString(CUtil::StringToWString(ss.str()).c_str());
//
//		if (cv::findChessboardCorners(checkerImgs[i], patternSize, imagePoints[i]))
//		{
//			//std::cout << " - ��_�����ׂČ��o���܂���" << std::endl;
//			OutputDebugString(L"��_�����ׂČ��o���܂���\n");
//
//			//���o�_��`�悷��
//			cv::drawChessboardCorners(checkerImgs[i], patternSize, imagePoints[i], true);
//
//			//�f�o�b�O�t���O��true�̏ꍇ�́A�摜��\�����A�ۑ�����
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
//			//�Q�l: �T�u�s�N�Z���P�ʂ܂ŃR�[�i�[���o���x���グ��ꍇ�͉��L�̃R�����g����������
//			//cv::Mat	gray;
//			//cv::cvtColor( checkerImgs[i], gray, cv::COLOR_BGR2GRAY );
//			//cv::cornerSubPix( gray, imagePoints[i], cv::Size( 11, 11 ), cv::Size( -1, -1 ), criteria );
//		}
//		else
//		{
//			//std::cout << " - ���o����Ȃ�������_������܂�" << std::endl;
//			OutputDebugString(L"���o����Ȃ�������_������܂�\n");
//			cv::waitKey(0);
//			return -1;
//		}
//	}
//
//	//���E���W�ɂ�����`�F�b�J�[�p�^�[���̌�_���W������
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
//	//Zhang�̎�@�ɂ��L�����u���[�V����
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
//	//�J�����L�����u���[�V�����̌덷��\������
//	//���肵���J�����s����g����3�������W��2�����֕ϊ����A
//	//�L�����u���[�V�����O�̃I���W�i���摜��̓_���W�Ɣ�r�B
//	//�����̓��a = reprojectionError
//	std::stringstream rpjErrorSs;
//	rpjErrorSs << "reprojection error=" << reprojectionError;
//	OutputDebugString(CUtil::StringToWString(rpjErrorSs.str()).c_str());
//	
//	
//	//��]�x�N�g���ƕ��s�ړ��x�N�g����4�s4��̊O���p�����[�^�s��ɏ���������
//	//�Ȃ��A�����ł�Zhang�̃L�����u���[�V������@�ɂ�����1���ڂ̊O���p�����[�^�s��̂ݏo�͂���
//
//	//�o�͐�ƂȂ�4�s4��̊O���p�����[�^�s��B�P�ʍs��ŏ�����
//	cv::Mat	extrinsic(4, 4, CV_64F);
//	cv::setIdentity(extrinsic);
//
//	//��]�x�N�g������]�s��ɕϊ����A�O���p�����[�^�s��Ɋi�[
//	cv::Rodrigues(rotationVectors[0], extrinsic(cv::Rect(0, 0, 3, 3)));
//
//	//���s�ړ��x�N�g�����O���p�����[�^�s��Ɋi�[
//	translationVectors[0].copyTo(extrinsic(cv::Rect(3, 0, 1, 3)));
//
//	//�J�����p�����[�^���t�@�C���ɏ����o��
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
//		//�c�ݕ␳�����摜��\��
//		cv::Mat	undistorted;
//		for (auto i = 0; i < numOfCheckerPatterns; i++)
//		{
//			//�����Y�c�݌W����p���ĉ摜�̘c�݂�␳
//			cv::undistort(checkerImgs[i], undistorted, cameraMatrix, distCoeffs);
//			// �c�ݕ␳�O�ƕ␳��̉摜��\��
//			cv::imshow(windowNameUnd, undistorted);
//			cv::imshow(windowNameSrc, checkerImgs[i]);
//			cv::waitKey(1);
//
//			//�t�@�C���ɕۑ�
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

/// <summary>���͂��ꂽ�摜����A�`�F�b�J�[�{�[�h�܂ł̓����ϊ��s������߂�</summary>
/// <param name="sourceImg"></param>
/// <returns></returns>
cv::Mat CImageProcessor::GetChessBoardPose(cv::Mat sourceImg)
{
	const cv::Size patternSize(10, 7);		//�`�F�b�J�[�p�^�[���̌�_�̐�
	const float sizeOfSquare = 24.0;		//�`�F�b�J�[�p�^�[���̃}�X�ڂ̃T�C�Y [mm]

	//���͉摜�̘c�݂���������
	//�c�݂�␳����
	cv::undistort(sourceImg, ImgUndistorted, intrinsicMatrix, distortionCoeffs);

	if (DebugMode == true)
	{
		cv::imshow("Undistorted image", ImgUndistorted);
		cv::imwrite(CUtil::getDateTimeString() + "_undis_chess_matrix.bmp", ImgUndistorted);
	}

	//���W�Ɋւ���ϐ�
	std::vector<cv::Point3f> worldPoints; //�`�F�b�J�[��_���W�ƑΉ����鐢�E���W�̒l���i�[����s��
	std::vector<cv::Point2f> imagePoints; //�`�F�b�J�[��_���W���i�[����s��

	//���E���W�ɂ�����`�F�b�J�[�p�^�[���̌�_���W������
	for (auto i = 0; i < patternSize.area(); i++)
	{
		auto cx = static_cast<float>(i % patternSize.width * sizeOfSquare);
		auto cy = static_cast<float>(i / patternSize.width * sizeOfSquare);
		auto cz = static_cast<float>(0.0);

		worldPoints.push_back(cv::Point3f(cx, cy, cz));
	}

	//�`�F�b�N�p�^�[���̌�_���W�����߁CimagePoints�Ɋi�[����
	std::stringstream ss;
	ss << u8"�摜����`�F�b�J�[�p�^�[���̌�_�����o���܂�";
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	if (cv::findChessboardCorners(ImgUndistorted, patternSize, imagePoints))
	{
		OutputDebugString(L"��_�����ׂČ��o���܂���");

		//���o�_��`�悷��
		cv::drawChessboardCorners(ImgUndistorted, patternSize, imagePoints, true);

		//�f�o�b�O�t���O��true�̏ꍇ�́A�摜��\�����A�ۑ�����
		if (DebugMode == true)
		{
			cv::imshow("check", ImgUndistorted);
			cv::waitKey(500);

			std::stringstream debugWriteFNameSS;
			debugWriteFNameSS << CUtil::getDateTimeString() << "_DrawChessCorners" << ".bmp";
			cv::imwrite(debugWriteFNameSS.str(), ImgUndistorted);
		}

		//�Q�l: �T�u�s�N�Z���P�ʂ܂ŃR�[�i�[���o���x���グ��ꍇ�͉��L�̃R�����g����������
		//cv::Mat	gray;
		//cv::cvtColor( checkerImgs[i], gray, cv::COLOR_BGR2GRAY );
		//cv::cornerSubPix( gray, imagePoints[i], cv::Size( 11, 11 ), cv::Size( -1, -1 ), criteria );
	}
	else
	{
		OutputDebugString(L"���o����Ȃ�������_������܂�");
		//cv::waitKey(0);
		return cv::Mat();
	}

	cv::Mat rvecEst;
	cv::Mat tvecEst;
	cv::solvePnPRansac(worldPoints, imagePoints, intrinsicMatrix, distortionCoeffs, rvecEst, tvecEst, false);

	cv::Mat rmat;
	cv::Rodrigues(rvecEst, rmat);

	//4X4�s��̓����ϊ��s��ɕϊ�����
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

/// <summary>�摜���̃`�F�X�{�[�h�܂ł̓����ϊ��s������߂�</summary>
/// <param name="sourceImg"></param>
/// <param name="imagePoints"></param>
/// <returns></returns>
cv::Mat CImageProcessor::GetChessBoardPose(cv::Mat sourceImg, std::vector<cv::Point2f>& imagePoints)
{
	//�`�F�X�{�[�h�֌W�̐ݒ�l���擾����
	int chessCrossWidth = ImageSetting.ChessCrossWidth;
	int chessCrossHeight = ImageSetting.CHessCrossHeight;
	double sizeOfSquare = ImageSetting.ChessSquareSize;
	int offsetX = ImageSetting.OffsetX;
	int offsetY = ImageSetting.OffsetY;

	const cv::Size patternSize(chessCrossWidth, chessCrossHeight);		//�`�F�b�J�[�p�^�[���̌�_�̐�

	//const cv::Size patternSize(9, 6);		//�`�F�b�J�[�p�^�[���̌�_�̐�
	//const float sizeOfSquare = 24.0;		//�`�F�b�J�[�p�^�[���̃}�X�ڂ̃T�C�Y [mm]

											//���͉摜�̘c�݂���������
											//�c�݂�␳����
	cv::undistort(sourceImg, ImgUndistorted, intrinsicMatrix, distortionCoeffs);
	//ImgUndistorted = sourceImg;

	if (DebugMode == true)
	{
		cv::imshow("Undistorted image", ImgUndistorted);
		cv::imwrite(CUtil::getDateTimeString() + "_undis_chess_matrix.bmp", ImgUndistorted);
	}

	//���W�Ɋւ���ϐ�
	std::vector<cv::Point3f> worldPoints; //�`�F�b�J�[��_���W�ƑΉ����鐢�E���W�̒l���i�[����s��
	//std::vector<cv::Point2f> imagePoints; //�`�F�b�J�[��_���W���i�[����s��

										  //���E���W�ɂ�����`�F�b�J�[�p�^�[���̌�_���W������
	for (auto i = 0; i < patternSize.area(); i++)
	{
		auto cx = static_cast<float>(i % patternSize.width * sizeOfSquare);
		auto cy = static_cast<float>(i / patternSize.width * sizeOfSquare);
		auto cz = static_cast<float>(0.0);

		worldPoints.push_back(cv::Point3f(cx, cy, cz));
	}

	//�`�F�b�N�p�^�[���̌�_���W�����߁CimagePoints�Ɋi�[����
	std::stringstream ss;
	ss << u8"�摜����`�F�b�J�[�p�^�[���̌�_�����o���܂�";
	OutputDebugString(CUtil::StringToWString(ss.str()).c_str());

	if (cv::findChessboardCorners(ImgUndistorted, patternSize, imagePoints))
	{
		OutputDebugString(L"��_�����ׂČ��o���܂���");

		//���o�_��`�悷��
		cv::drawChessboardCorners(ImgUndistorted, patternSize, imagePoints, true);

		//�f�o�b�O�t���O��true�̏ꍇ�́A�摜��\�����A�ۑ�����
		if (DebugMode == true)
		{
			cv::imshow("check", ImgUndistorted);
			cv::waitKey(500);

			std::stringstream debugWriteFNameSS;
			debugWriteFNameSS << CUtil::getDateTimeString() << "_DrawChessCorners" << ".bmp";
			cv::imwrite(debugWriteFNameSS.str(), ImgUndistorted);
		}

		//�Q�l: �T�u�s�N�Z���P�ʂ܂ŃR�[�i�[���o���x���グ��ꍇ�͉��L�̃R�����g����������
		//cv::Mat	gray;
		//cv::cvtColor( checkerImgs[i], gray, cv::COLOR_BGR2GRAY );
		//cv::cornerSubPix( gray, imagePoints[i], cv::Size( 11, 11 ), cv::Size( -1, -1 ), criteria );
	}
	else
	{
		OutputDebugString(L"���o����Ȃ�������_������܂�");
		//cv::waitKey(0);
		return cv::Mat();
	}

	cv::Mat rvecEst;
	cv::Mat tvecEst;
	cv::solvePnPRansac(worldPoints, imagePoints, intrinsicMatrix, distortionCoeffs, rvecEst, tvecEst, false);

	cv::Mat rmat;
	cv::Rodrigues(rvecEst, rmat);

	//4X4�s��̓����ϊ��s��ɕϊ�����
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


