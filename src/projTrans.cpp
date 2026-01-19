#include "alglibrary/zkhyProjectHuaNan/projTrans.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()
#define  isDebugIPHN 0;

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {
	int realProjTrans(ProjTransInput& input, ProjTransOutput& output)
	{
#if isDebugIPHN
		if (isDebugIPHN)
		{
			char debugInfo[512];
			sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[ProjTrans] Begain.\n");
			OutputDebugStringA(debugInfo);
		}
#endif
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		HObject ho_srcImage;
		if (input.srcImg.IsInitialized()) {
			ho_srcImage = input.srcImg;
		}
		else
		{
			return 1;
		}
		HTuple hv_HomMat2D;
		try
		{
			hv_HomMat2D = input.homMat2DInfo.projTransMat;
			//投影变换矩阵移除像素矩阵数据
			TupleRemove(hv_HomMat2D, 9, &hv_HomMat2D);
#if isDebugIPHN
			char debugInfo[512];
			sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[ProjTrans] hv_HomMat2D : %f, %f, %f, %f, %f, %f, %f, %f, %f\n", hv_HomMat2D[0].D(), hv_HomMat2D[1].D(), hv_HomMat2D[2].D(), hv_HomMat2D[3].D(), hv_HomMat2D[4].D(), hv_HomMat2D[5].D(), hv_HomMat2D[6].D(), hv_HomMat2D[7].D(), hv_HomMat2D[8].D());
			OutputDebugStringA(debugInfo);
#endif
			ProjectiveTransImage(ho_srcImage, &output.dstImg, hv_HomMat2D, "bilinear",
				"false", "false");
		}
		catch (HalconCpp::HException& except)
		{
			//string errormsg = except.ErrorMessage().Text();
			char debugInfo[256];
			sprintf_s(debugInfo, "[Alg Debug][Alg Error] Alg_Name:[ProjTrans] errMsg: %s\n", except.ErrorMessage().Text());
			OutputDebugStringA(debugInfo);
			return 2;
		}
		output.mdstImg = HObject2Mat(output.dstImg);
		//imwrite("E:/工程代码/workCodeEdit/zkhyProject/变换后.bmp", output.mdstImg);
		//WriteImage(output.dstImg, "bmp", 0, "E:/工程代码/workCodeEdit/zkhyProject/变换后.bmp");

#if isDebugIPHN
		if (isDebugIPHN)
		{
			char debugInfo[512];
			sprintf_s(debugInfo, "[Alg Debug] Alg_Name:[ProjTrans] Successful End.\n");
			OutputDebugStringA(debugInfo);
		}
#endif

		return 0;
	}


}

namespace alglib::ops::zkhyProHN::alg {
	//-------------------------------------投影变换算子projectiveTransImage 20221115----------------------------//
	//名称：  projectiveTrans
	//功能：  加载投影变换矩阵，对输入图像进行投影变换，输出投影变换后图像
	//0-输入参数 1-输出参数
	//返回值：0-正常 1-图像为空 2-存储投影变换矩阵异常 3-创建投影变换矩阵失败 4-加载投影变换矩阵失败 5-投影变换转换失败
	int projTrans(ProjTransInput& input, ProjTransOutput& output)
	{
		try
		{
			return realProjTrans(input, output);
		}
		catch (...)
		{
			return 3; //算法未知异常
		}
	}

}
