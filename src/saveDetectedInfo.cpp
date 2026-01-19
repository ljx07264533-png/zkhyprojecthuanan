#include <fstream>
#include <iostream>
#include <mutex>
#include <numeric>
#include <string>
#include <vector>

#include "alglibrary/zkhyProjectHuaNan/saveDetectedInfo.h"
#include "alglibrary/zkhyProjectHuaNan/cJSON.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

std::mutex mtxLock;		// 互斥锁

using namespace std;
using namespace cv;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {

	// 需要新增 新产品线的 productInfo 和 environmentInfo 时需要在 "zkhy_publicClassHN.h" 中新增新的派生类

	class detectedInfo : public zkhyPublicClass::AlgorithmBase
	{
	public:

		detectedInfo()
		{
			this->infoStruct = nullptr;
			this->savePath = "";
			this->infoAnalyzed = "";
			this->inputInfo = "";
			this->fileSavedName = "";
			this->infoSaved = "";

			memset(this->cProductInfo, 0, sizeof(this->cProductInfo));
			memset(this->cEnvironmentInfo, 0, sizeof(this->cEnvironmentInfo));
		}

		~detectedInfo() {}

		// 构造函数	解析输入
		detectedInfo(const saveDetectedInfoInput& input, saveDetectedInfoOutput& output)
		{
			this->infoStruct = input.infoStruct;
			this->savePath = input.fileSavePath;
			this->inputInfo = input.inputInfo;
			this->fileSavedName = "";
			this->infoSaved = "";

			sprintf_s(this->cProductInfo, "%s", input.productInfo);
			sprintf_s(this->cEnvironmentInfo, "%s", input.environmentInfo);

		}

		int analyzeInfo(const int& infoType);		// 解析数据

		int getAnalyzedInfo(string& infoAnalyzed)		// 获取分析后的数据
		{
			infoAnalyzed = this->infoAnalyzed;
			return 0;
		}

		// 根据不同的产品线 自行对方法进行修改或者重载

		int saveInfo(const string& infoStr, const int& infoType, const int& saveModel);		// 存储数据

	protected:
		void* infoStruct;
		string savePath;
		string infoAnalyzed;
		string inputInfo;
		string fileSavedName;
		string infoSaved;

		char cProductInfo[2048], cEnvironmentInfo[2048];
	};

	// 根据不同的产品线 自行对下列方法进行修改或者重载

	//	构造存储的文件名称和路径
	int setSavedFileInfo(const zkhyPublicClass::productInfoReel& productInfo, const string& pathRoot, string& fileName, string& filePath);

	// 将数据写入文件
	int writeInfo2File(const int& infoType, const string& fileName, const string& infoStr, const zkhyPublicClass::productInfoReel& productInfo);

	int realsaveDetectedInfo(const saveDetectedInfoInput& input, saveDetectedInfoOutput& output);

	//-----------------------------saveDetectedInfo 函数入口-----------------------------//

	int realsaveDetectedInfo(const saveDetectedInfoInput& input, saveDetectedInfoOutput& output)
	{
		AlgParamSaveDetectedInfo* alg = (AlgParamSaveDetectedInfo*)&input.algPara;
		int infoType = alg->i_params.block.iInfoType;
		int saveModel = alg->i_params.block.iSaveModel;

		double timeOutThreshold = alg->d_params.block.dTimeOutThreshold;

		// 开始计时
		DWORD startTime = GetTickCount64();

		// 解析数据
		detectedInfo info(input, output);

		info.analyzeInfo(infoType);		// 解析数据

		string infoAnalyzed;
		info.getAnalyzedInfo(infoAnalyzed);		// 获取分析后的数据

		// 构建需要输出的数据
		string infoFinalSaved = "";

		switch (saveModel)
		{
		case 0:		//默认
		{
			break;
		}
		case 1:		//卷带机
		{
			infoFinalSaved = infoAnalyzed;
			break;
		}
		default:
		{
			infoFinalSaved = infoAnalyzed;
			break;
		}
		}

		// 存储数据
		int exeRes = info.saveInfo(infoFinalSaved, infoType, saveModel);

		if (exeRes != 0)
			return exeRes;

		// 计算算法耗时
		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 1;                   // 算法超时

		return 0;
	}

	// 解析数据
	int detectedInfo::analyzeInfo(const int& infoType)
	{
		if (infoStruct != nullptr)
		{
			switch (infoType)
			{
			case 0: //二维码
			{
				DataCode2dEnhancedOutput* QDRes = (DataCode2dEnhancedOutput*)infoStruct;
				infoAnalyzed = QDRes->resultstr;
				break;
			}
			case 1: //OCR
			{
				FindSingleCharModeOutput* OCRRes = (FindSingleCharModeOutput*)infoStruct;
				infoAnalyzed = OCRRes->charOut;
				break;
			}
			default:
				break;
			}
		}
		else
		{
			if (inputInfo.empty())
				return 5;		// 输入数据为空
			else
				infoAnalyzed = inputInfo;
		}
		return 0;
	}

	int detectedInfo::saveInfo(const string& infoStr, const int& infoType, const int& saveModel)
	{
		switch (saveModel)
		{
		case 0:		//默认
		{
			break;
		}
		case 1:		//卷带机 文件存储规则：指定路径/批次号/批次号_卷号.txt
		{
			// 解析产品信息
			zkhyPublicClass::productInfoReel productInfo;
			productInfo.getProductInfo(cProductInfo);

			string fileName, filePath;
			int ret = setSavedFileInfo(productInfo, this->savePath, fileName, filePath);

			if (ret != 0)
				return ret;

			// 文件全路径
			string fileFullPath = filePath + "/" + fileName;

			// 信息写入文件
			ret = writeInfo2File(infoType, fileFullPath, infoStr, productInfo);

			if (ret != 0)
				return ret;

			break;
		}
		default:
			break;
		}
		return 0;
	}

	int setSavedFileInfo(const zkhyPublicClass::productInfoReel& productInfo, const string& pathRoot, string& fileName, string& filePath)
	{
		// 根据输入信息和规则构造文件名称和路径，适用于卷带包装机产品
		// 文件名称
		fileName = productInfo.lotID + "_" + productInfo.RollID + ".txt";

		// 文件路径
		filePath = pathRoot + "/" + productInfo.lotID;
		if (_access(filePath.c_str(), 0) == -1)
			if (zkhyPublicFuncHN::createDirectory(filePath) != 0)
				return 2;           // 存储路径异常

		return 0;
	}

	int writeInfo2File(const int& infoType, const string& fileName, const string& infoStr, const zkhyPublicClass::productInfoReel& productInfo)
	{
		// 将信息写入文件，适用于卷带包装机产品
		// 判断文件是否存在
		int lineNum = -1;
		int lineID = 0;

		// 设置互斥锁
		std::lock_guard<std::mutex> lock(mtxLock);

		ifstream fileTmp(fileName);
		if (fileTmp.good())
		{
			// 计算行数
			string line;
			while (getline(fileTmp, line))
			{
				if (line.compare("\n") == 0)
					break;
				++lineNum;
			}
			lineID = lineNum + 1;
			fileTmp.close();
		}
		else
		{
			// 文件不存在时，创建文件， 并写入表头
			// 表头格式如下，第一列占用15位，第二列占用30位
			// NO          二维码/盖印字符
			// 第二列采用什么根据 infoType决定
			ofstream fileOut(fileName);
			if (fileOut.good())
			{
				switch (infoType)
				{
				case 0:		//二维码
				{
					// 表头 第一列占用15位，第二列占用30位
					fileOut << std::left << std::setw(10) << "NO" << "\t" << std::setw(30) << "二维码" << endl;
					break;
				}
				case 1:		//OCR
				{
					fileOut << std::left << std::setw(10) << "NO" << "\t" << std::setw(30) << "盖印字符" << endl;
					break;
				}
				default:
					break;
				}
			}
			lineID = 1;
			fileOut.close();
		}

		// 写入数据
		ofstream fileOut(fileName, ios::app);
		if (fileOut.good())
		{
			fileOut << std::left << std::setw(15) << lineID << "\t" << std::setw(30) << infoStr << endl;
		}
		fileOut.close();

		return 0;
	}
}

namespace alglib::ops::zkhyProHN::alg {
	int saveDetectedInfo(const saveDetectedInfoInput& input, saveDetectedInfoOutput& output)
	{
		try {
			int ret = realsaveDetectedInfo(input, output);
			return ret;
		}
		catch (...) {
			return 4;
		}
	}
}
