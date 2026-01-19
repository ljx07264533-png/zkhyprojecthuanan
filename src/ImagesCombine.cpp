#include "alglibrary/zkhyProjectHuaNan/ImagesCombine.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {
	//----------------------------------------------realImagesCombine--------------------------------//
//功能：对两幅输入图像进行相加、相减、与、或等操作
// 输入：两幅需要进行操作的图像
// 输出：图像组合后的结果
	int realImagesCombine(const ImagesCombineInput& input, ImagesCombineOutput& output)
	{
		HObject imgCurrent = input.imageCurrent;
		HObject imgAux = input.imageAux;

		// 解析算法参数
		AlgParamImagesCombine* pParams = (AlgParamImagesCombine*)&input.algPara;
		int iCombineOperate = pParams->i_params.block.iCombineOperate;
		double dMultFactor = pParams->d_params.block.dMultFactor;
		double dAddValue = pParams->d_params.block.dAddValue;
		double dTimeOutThreshold = pParams->d_params.block.dTimeOutThreshold;

		// 判断输入合法性
		// 需要先判断对象是否初始化，再判断是否为空对象，否则判断对象为空时会产生异常
		if (imgCurrent.IsInitialized() == false)
			return 1;           // 输入图像为空，图像未初始化
		if (imgAux.IsInitialized() == false)
			return 2;           // 输入图像为空，图像未初始化
		HObject hEmpty;
		HTuple hIsEuqal;
		GenEmptyObj(&hEmpty);
		TestEqualObj(imgCurrent, hEmpty, &hIsEuqal);
		if (hIsEuqal.I() == 1)
		{
			return 1;           // 输入当前图像为空
		}
		TestEqualObj(imgAux, hEmpty, &hIsEuqal);
		if (hIsEuqal.I() == 1)
		{
			return 2;           // 输入辅助图像为空
		}
		if (dTimeOutThreshold > 999999 || dTimeOutThreshold <= 0)
			return 3;           // 算法超时参数输入有误
		if (dMultFactor < -255.0 || dMultFactor>255.0)
			return 4;           // 乘性系数超出范围
		if (dAddValue < -512.0 || dAddValue>512.0)
			return 5;           // 加性系数超出范围

		// 开始计时
		DWORD startTime = GetTickCount64();
		HTuple hChannelImgCurrent, hChannelImgAux, hImgCurrentWidth, hImgCurrentHeight, hImgAuxWidth, hImgAuxHeight;
		//判断图像通道数是否相同
		CountChannels(imgCurrent, &hChannelImgCurrent);
		CountChannels(imgAux, &hChannelImgAux);
		if (hChannelImgCurrent.I() != hChannelImgAux.I())
			return 6;           // 输入图像通道不一致
		GetImageSize(imgCurrent, &hImgCurrentWidth, &hImgCurrentHeight);
		GetImageSize(imgAux, &hImgAuxWidth, &hImgAuxHeight);
		if (hImgCurrentWidth.I() != hImgAuxWidth.I() || hImgCurrentHeight.I() != hImgAuxHeight.I())
			return 7;          // 输入图像尺寸不一致

		HObject hResultImage;
		if (iCombineOperate == 0)    //图像相加
		{
			AddImage(imgCurrent, imgAux, &hResultImage, dMultFactor, dAddValue);
		}
		else if (iCombineOperate == 1)  //图像相减
		{
			SubImage(imgCurrent, imgAux, &hResultImage, dMultFactor, dAddValue);
		}
		else if (iCombineOperate == 2)   //图像相乘
		{
			MultImage(imgCurrent, imgAux, &hResultImage, dMultFactor, dAddValue);
		}
		else if (iCombineOperate == 3)   //图像相与
		{
			BitAnd(imgCurrent, imgAux, &hResultImage);
		}
		else if (iCombineOperate == 4)   //图像相或
		{
			BitOr(imgCurrent, imgAux, &hResultImage);
		}

		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > dTimeOutThreshold)
			return 8;                   // 算法超时

		output.hImageCombined = hResultImage;
		output.imageCombined = HObject2Mat(hResultImage);
		// 保存图片
		/*string fileRoot = "F:\\项目资料\\3-半导体封测\\BGA锡球测试图\\";
		string filename = "";
		for (int i = 0; i < 9999; i++) {
			filename = fileRoot + "\\ballDet" + to_string(i) + ".bmp";
			struct  stat buffer;
			if (stat(filename.c_str(), &buffer) != 0)
				break;
		}
		cv::imwrite(filename, output.imageCombined);*/
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------ImagesCombine--------------------------//
	//功能：ImagesCombine函数入口
	int ImagesCombine(const ImagesCombineInput& input, ImagesCombineOutput& output)
	{
		try
		{
			return realImagesCombine(input, output);
		}
		catch (...)
		{
			return 9; //未知错误
		}
	}
}
