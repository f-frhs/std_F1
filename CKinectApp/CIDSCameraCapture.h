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

//CIDSCameraCaputure�̓����f�[�^�ۑ��l
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

/// <summary>�J�����X�e�[�^�X�ۑ��p�\����</summary>
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

	///<summary>�R���X�g���N�^</summary>
	CIDSCameraCaputure();

	///<summary>�f�X�g���N�^</summary>
	~CIDSCameraCaputure();

	///<summary>�J�����ԍ����w�肵����R���X�g���N�^</summary>
	CIDSCameraCaputure(int camNum, bool useDeviceID);	//�J�����ԍ����w�肵���R���X�g���N�^

														///<summary>�J�����ԍ����w�肵�ď���������</summary>
	bool SetCameraNumber(int camNum, bool useDeviceID);

	///<summary></summary>
	bool StartSingleCapture(void);	//1�t���[���L���v�`������

									///<summary>�J�����̏����擾����i�s�������������̏��j</summary>
	CAMINFO GetCameraInfo(void);

	///<summary>�J�����̃X�e�[�^�X���擾����</summary>
	bool GetCameraStatus(void);

	bool SetFlash(bool flashFlg);	//�t���b�V���̐ݒ�

									/// <summary>�J�������I�[�v������ </summary>
	bool OpenCamera(void);

	/// <summary>�ݒ���t�@�C���ɕۑ�����</summary>
	bool SaveToFile(std::string fName);

	///<summary>�ݒ���t�@�C�����畜������</summary>
	bool LoadFromFile(std::string fName);

	void GetMaxImageSize(INT* sizeX, INT* sizeY);

	/// <summary>CV::Mat���擾���� </summary>
	cv::Mat GetCVMat();

	/// <summary>�f�o�b�O�ݒ�t���O </summary>
	bool ImageDebug = false;

	/// <summary>�L���v�`��������ɌĂяo�����֐��I�u�W�F�N�g�̓o�^�p</summary>
	std::function<void(int)> CapturedFunction = nullptr;

	/// <summary></summary>
	std::string GetCameraInfoStr();

	//TODO:�C���[�W�v���Z�b�T�p�̊֐���ǉ�����N���X�����o�֐��o�^����������

private:

	///<summary></summary>
	cv::Mat capturedImage;	//IDS�J�����ŃL���v�`�������摜

							///<summary>�J�����ԍ�</summary>
	int cameraNumber{ -1 };

	/// <summary>�J�����ւ̃J�����n���h��</summary>
	HIDS hCam;

	/// <summary>�J�����̏�����������Ɋ��������ꍇ��true </summary>
	bool isConnected = false;

	/// <summary>�C���[�W�������ۑ��p�̃|�C���^ </summary>
	char *memImg;
	INT memID;

	/// <summary>�J�����B�e�����p�C�x���g </summary>
	HANDLE camEvent;

	bool SingleCaptureWork(void);

	//�擾�C���[�W�̃T�C�Y
	int capImageWidth;
	int capImageHeight;

	//�P�s�N�Z���̃o�C�g��
	int pixelByte;


};



#endif
