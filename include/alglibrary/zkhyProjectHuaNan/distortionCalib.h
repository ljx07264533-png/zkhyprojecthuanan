
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_DISTORTIONCALIB_H
#define ZKHYPROJECTHUANAN_DISTORTIONCALIB_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------畸变校正算子------------------------------------------//
	//Step1算子input结构体、output结构体
	struct DistortionCalibInput
	{
		HalconCpp::HObject srcImg;                       //原图
		HalconCpp::HObject lensDistortCalibMap;          //镜头畸变校正Map图
		HalconCpp::HObject perspectTransMap;             //透视变换Map图
		alglib::core::AlgrithmParams algPara;               //算法参数
	};
	struct DistortionCalibOutput
	{
		HalconCpp::HObject calibImg;                    //变换后图像
		cv::Mat mCalibImg;                       //用于显示
	};

	//Step2 算子参数 结构体定义 int和double分开写
	struct AlgParamDistortionCalibInt//int型算法参数，独立出来方便使用默认构造函数
	{
		int calibFlag = 0;         //校正方法
	};

	struct AlgParamDistortionCalibDouble//double型算法参数，独立出来方便使用默认构造函数
	{

	};


	struct AlgParamDistortionCalib
	{
		union
		{
			AlgParamDistortionCalibInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParamDistortionCalibDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};

	//Step3 算子参数描述
	struct AlgParaMsgDistortionCalib
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgDistortionCalib()
		{
			msgs.push_back("calibFlag	  $Int	    $Combox      $0     $校正方式      $校正方式 ￥镜头畸变校正 ￥透视变换校正 $10");
		}
	};

	//Step4 异常返回值描述
	struct AlgErrMsgDistortionCalib
	{
		std::vector<std::string> msgs;
		AlgErrMsgDistortionCalib()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:读取畸变校正Map图失败");
			msgs.push_back("3:读取透视变换Map图失败");
			msgs.push_back("4:校正失败");
			msgs.push_back("5:算法未知异常");

		}
	};

	//! 接口

	namespace alg {
		//------------------------------------------畸变校正 distortionCalib 20221123--------------------------//
		//名称：  distortionCalib
		//功能：  利用内参和外参进行校正
		//0-输入参数 1-输出参数
		//返回值：0-正常
		_ALGLIBRARY_API int distortionCalib(DistortionCalibInput& input, DistortionCalibOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_DISTORTIONCALIB_H
