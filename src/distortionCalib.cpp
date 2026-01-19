#include "alglibrary/zkhyProjectHuaNan/distortionCalib.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	int realDistortionCalib(DistortionCalibInput& input, DistortionCalibOutput& output)
	{
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		HObject ho_SrcImage, ho_LensDistortCalibMap, ho_PerspectTransMap;
		if (input.srcImg.IsInitialized())
		{
			ho_SrcImage = input.srcImg;
		}
		else
		{
			return 1;
		}
		if (input.lensDistortCalibMap.IsInitialized())
		{
			ho_LensDistortCalibMap = input.lensDistortCalibMap;
		}
		else
		{
			return 2;
		}

		if (input.perspectTransMap.IsInitialized())
		{
			ho_PerspectTransMap = input.perspectTransMap;
		}
		else
		{
			return 3;
		}
		AlgParamDistortionCalib* algPara = (AlgParamDistortionCalib*)&input.algPara;
		HTuple hv_distortionCalibFlag = algPara->i_params.block.calibFlag;
		HTuple hv_CameraParameters, hv_CamParamOut, hv_FinalPose, hv_Width, hv_Height;
		try {

			if (hv_distortionCalibFlag == 0)
			{
				MapImage(ho_SrcImage, ho_LensDistortCalibMap, &output.calibImg);
			}
			if (hv_distortionCalibFlag == 1)
			{
				MapImage(ho_SrcImage, ho_PerspectTransMap, &output.calibImg);
			}
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			return 4;
		}
		//output.mCalibImg = zkhyHalconFunc::HObject2Mat(output.calibImg);
		zkhyPublicFuncHN::HObject2Mat(output.calibImg, output.mCalibImg);
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//------------------------------------------畸变校正 distortionCalib 20221123--------------------------//
	//名称：  distortionCalib
	//功能：  利用内参和外参进行校正
	//0-输入参数 1-输出参数
	//返回值：0-正常
	int distortionCalib(DistortionCalibInput& input, DistortionCalibOutput& output)
	{
		try
		{
			return realDistortionCalib(input, output);
		}
		catch (...)
		{
			return 5; //算法未知异常
		}
	}
}
