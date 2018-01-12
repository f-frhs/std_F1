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
	// ���O�t���p�C�v�𐶐����A�N���C�A���g(C#)����̐ڑ���ҋ@����B
	// �܂��A Cam ������������
	auto pipe1 = CPipeServer(".", "TestPipe1", 1, "Cam1");
	auto pipe2 = CPipeServer(".", "TestPipe2", 2, "Cam2");
	
	pipe1.ConnectToNewClient();
	pipe2.ConnectToNewClient();

	// �����������J�������K�؂��ۂ����m�F����B
	pipe1.ConnectCameraRequestAndResponse();
	pipe2.ConnectCameraRequestAndResponse();


	// C#����J�������v������M�A�w�肳�ꂽ�J�����̏����擾����
	pipe1.GetCamInfoRequestAndResponse();
	pipe2.GetCamInfoRequestAndResponse();

	pipe1.Ip.CameraParamsFile = "camParam-S4103159790-L372440.xml";
	pipe2.Ip.CameraParamsFile = "camParam-S4103159793-L372434.xml";

	//���ϐ}�𓾂邽�߂̃A�t�B���ϊ��ł����A�t�B���s���ǂݍ���
	//TODO: pipe2�p�̃A�t�B���ϊ���p�ӂ���
	pipe1.Ip.LoadAffineMatrix("AffineTopView.xml");
	pipe2.Ip.LoadAffineMatrix("AffineTopView.xml");
	pipe1.Ip.HasToGetTopView = true;
	pipe2.Ip.HasToGetTopView = true;

	//TODO:  �ȉ���xm�̒l�� XAML���炱����֑��M�ł���悤�ɂ���
	//pipe1.Ip.ImageSetting.Point1X = 200;
	//pipe1.Ip.ImageSetting.Point1Y = 800;
	//pipe1.Ip.ImageSetting.Point2X = 833;
	//pipe1.Ip.ImageSetting.Point2Y = 800;

	cout << "---" << endl;

	//�{�v���O�����𓮂������߂̐ݒ�
	pipe1.Ip.DebugMode = false;
	pipe2.Ip.DebugMode = false;
	auto waitingTimeInMilliSecond = std::min<int>(pipe1.WaitingCommandInMilliSecond, pipe2.WaitingCommandInMilliSecond);


	while (pipe1.IsConnected() && pipe2.IsConnected())
	{
		if(!pipe1.IsEmpty())
		{
			// C#���Ő��������ݒ�(�\����)����M�A�B�e�p�\���̂�ݒ肷��
			pipe1.WaitAndReceiveAndSetSettingOfImageAnalysis();

			// C��������B�e�M���ƎB�e�ʒu�ԍ�����M�A�J��������2D�摜�𓾂�
			// �摜�𓾂����Ƃ�C#�ɑ��M����
			pipe1.CaptureRequestAndResponse();

			// C�������瑪�茋�ʗv���ƎB�e�ʒu�ԍ�����M�A���o�����p����C#�ɑ��M����
			pipe1.GetMeasureDataRequestAndResponse();

			// C�������猋�ʉ摜�v���ƎB�e�ʒu�ԍ�����M�A�p�Ɉ�������摜��C#�ɑ��M����
			pipe1.GetImageRequestAndResponse();

			cout << "----" << endl;
		}

		if (!pipe2.IsEmpty())
		{
			// C#���Ő��������ݒ�(�\����)����M�A�B�e�p�\���̂�ݒ肷��
			pipe2.WaitAndReceiveAndSetSettingOfImageAnalysis();

			// C��������B�e�M���ƎB�e�ʒu�ԍ�����M�A�J��������2D�摜�𓾂�
			// �摜�𓾂����Ƃ�C#�ɑ��M����
			pipe2.CaptureRequestAndResponse();

			// C�������瑪�茋�ʗv���ƎB�e�ʒu�ԍ�����M�A���o�����p����C#�ɑ��M����
			pipe2.GetMeasureDataRequestAndResponse();

			// C�������猋�ʉ摜�v���ƎB�e�ʒu�ԍ�����M�A�p�Ɉ�������摜��C#�ɑ��M����
			pipe2.GetImageRequestAndResponse();

			cout << "----" << endl;
		}

		
		Sleep(waitingTimeInMilliSecond);
	}
	return 0;
}

//�ݒ�p�\���̂̍����X�V�R�}���h���p�[�X���āA
//�ϐ����̕�����E����������l�̕�����ɑ������
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

	//�����Œl��ǂݎ��
	char *tempChars = new char[valLength + 1];
	memcpy(tempChars, &chars[offset], valLength);
	tempChars[valLength] = '\0';
	valValueStr = std::string(tempChars);
}
