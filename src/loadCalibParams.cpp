#include "alglibrary/zkhyProjectHuaNan/loadCalibParams.h"

#define LOG alglib::core::LoggerManager::getInstance()
#define  isDebugIPHN 0;

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	int realLoadCalibParams(LoadCalibParamsInput& input, LoadCalibParamsOutput& output)
	{
#if isDebugIPHN
		auto start1 = std::chrono::steady_clock::now();
		if (isDebugIPHN)
		{
			char debugInfo[512];
			sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[LoadCalibParams] Begain.\n");
			OutputDebugStringA(debugInfo);
		}
#endif

		//设置中文字符串转换
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		//读取相关Map图
		HTuple hv_LensDistortCalibPath = input.loadPath.saveLensDistortCalibMapPath.c_str();
		HTuple hv_PerspectTransPath = input.loadPath.savePerspectTransMapPath.c_str();

		HTuple	hv_FileExistsFlag;
		//检查畸变校正Map图是否存在
		FileExists(hv_LensDistortCalibPath, &hv_FileExistsFlag);
		//检查透视变换Mat图是否存在
		FileExists(hv_PerspectTransPath, &hv_FileExistsFlag);
		if (hv_FileExistsFlag.I() != 0) {
			try
			{
				ReadImage(&output.calibFilesInfo.lensDistortCalibMap, hv_LensDistortCalibPath);
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				return 1;
			}
		}
		else
			return 1;
		//检查透视变换Mat图是否存在
		//FileExists(hv_PerspectTransPath, &hv_FileExistsFlag);
		if (hv_FileExistsFlag.I() != 0)
		{
			try
			{
				ReadImage(&output.calibFilesInfo.perspectTransMap, hv_PerspectTransPath);
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				return 2;
			}
		}
		else
			return 2;

#if isDebugIPHN
		auto finish1 = std::chrono::steady_clock::now();
		using milliseconds = std::chrono::duration<double, std::milli>;
		milliseconds duration1 = finish1 - start1;
		char debugInfo[512];
		sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[LoadCalibParams] Successful End. Time used:%f\n", duration1.count() / loopNum);
		OutputDebugStringA(debugInfo);
#endif
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//------------------------------------------加载参数 loadCalibParams 20221123-------------------------//
	//名称：  loadCalibParams
	//功能：  加载标定内参和外参
	//0-输入参数 1-输出参数
	 int loadCalibParams(LoadCalibParamsInput& input, LoadCalibParamsOutput& output)
	{
		try
		{
			return realLoadCalibParams(input, output);
		}
		catch (...)
		{
			return 3; //算法未知异常
		}
	}
}
