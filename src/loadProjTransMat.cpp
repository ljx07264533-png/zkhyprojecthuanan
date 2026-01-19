#include "alglibrary/zkhyProjectHuaNan/loadProjTransMat.h"

#define LOG alglib::core::LoggerManager::getInstance()
#define  isDebugIPHN 0;

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {

	int realLoadProjTransMat(const LoadProjTransMatInput& input, LoadProjTransMatOutput& output)
	{
#if isDebugIPHN
		if (isDebugIPHN)
		{
			char debugInfo[512];
			sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[LoadProjTransMat] Begain.\n[Alg Debug] Alg_Name:[LoadProjTransMat] loadPath.saveProjTransMatPath: %s\n", input.loadPath.saveProjTransMatPath.data());
			OutputDebugStringA(debugInfo);
		}
#endif
		//设置中文字符串转换
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		//读取投影变换矩阵变量
		HTuple hv_projective_trans_homat2d;
		HTuple hv_modelId_path = input.loadPath.saveProjTransMatPath.c_str();
		HTuple	hv_file_exists_flag;
		HTuple hv_FileHandle, hv_SerializedItemHandle;
		//检查投影变换矩阵是否存在
		FileExists(hv_modelId_path, &hv_file_exists_flag);
		if (hv_file_exists_flag.I() != 0) {
			try {
				//读取投影变换矩阵 .tup数据
				ReadTuple(hv_modelId_path, &hv_projective_trans_homat2d);
				TupleRemove(hv_projective_trans_homat2d, 9, &hv_projective_trans_homat2d);
				output.transHomMat2DInfo.projTransMat = hv_projective_trans_homat2d;
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				char debugInfo[512];
				sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[LoadProjTransMat] Error: %s\n", errormsg.c_str());
				OutputDebugStringA(debugInfo);
				return 1;
			}
		}
#if isDebugIPHN
		if (isDebugIPHN)
		{
			char debugInfo[512];
			sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[LoadProjTransMat] End.\n");
			OutputDebugStringA(debugInfo);
		}
#endif
		return 0;
	}


}

namespace alglib::ops::zkhyProHN::alg {
	//-------------------------------------加载投影变换矩阵算子loadProjectiveTransHomMat2D 20221115----------------------------//
	//名称：  loadProjectiveTransHomMat2D
	//功能：  加载投影变换矩阵
	//0-输入参数 1-输出参数
	//返回值：0-正常 1-图像为空 2-存储投影变换矩阵异常 3-创建投影变换矩阵失败 4-加载投影变换矩阵失败 5-投影变换转换失败
	int loadProjTransMat(const LoadProjTransMatInput& input, LoadProjTransMatOutput& output)
	{
		try
		{
			return realLoadProjTransMat(input, output);
		}
		catch (...)
		{
			return 2; //未知异常
		}
	}
}
