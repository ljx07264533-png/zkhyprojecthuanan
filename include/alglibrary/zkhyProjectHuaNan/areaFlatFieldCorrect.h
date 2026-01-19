
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_AREAFLATFIELDCORRECT_H
#define ZKHYPROJECTHUANAN_AREAFLATFIELDCORRECT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体


	//————————————————————————areaFlatFieldCorr 面阵平场校正 Begin————————————————//
	//Step1 输入结构体
	struct AreaFlatFieldCorrInput
	{
		HalconCpp::HObject srcImg;                         //待校正的图像
		HalconCpp::HObject weightImg;                      //校正权重图像
		std::vector<std::vector<cv::Point2f>>  inputRoi;           // 校正区域
		HalconCpp::HObject  regionROI;                     // 校正区域(两者只存在一个）
		alglib::core::AlgrithmParams algPara;					//算法参数
	};
	//Step2 输出结构体
	struct AreaFlatFieldCorrOutput
	{
		HalconCpp::HObject srcImg;                         //校正后图像
		cv::Mat     showImg;                       //校正后示意图
	};
	//Step3 算法参数（int double）
	struct AlgParaAreaFlatFieldCorrInt          //int型算法参数，独立出来方便使用默认构造函数
	{
		int flagTransMat;     // 是否转换为Mat图
		int lowGray;         //校正低灰度阈值
		int highGray;        //校正高灰度阈值

	};
	struct AlgParaAreaFlatFieldCorr
	{
		union
		{
			AlgParaAreaFlatFieldCorrInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			struct
			{
			}block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgAreaFlatFieldCorr
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgAreaFlatFieldCorr()
		{
			msgs.push_back("flagTransMat $Int      $CheckBox	 $0  	 $是否显示Mat图		$在正常运行模式不需要显示校正后的mat图，只在调试过程中可以观察校正后的效果图			$1");
			msgs.push_back("lowGray 	$Int       $I_Spinbox	 $0  	 $平场校正低灰度阈值		$平场校正低灰度阈值				$10");
			msgs.push_back("highGray 	$Int       $I_Spinbox	 $254  	 $平场校正高灰度阈值		$平场校正高灰度阈值				$20");
		}
	};
	//Step4 异常返回值描述
	struct AlgErrMsgAreaFlatFieldCorr
	{
		std::vector<std::string> msgs;
		AlgErrMsgAreaFlatFieldCorr()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:低灰度阈值必须要小于高灰度阈值");
			msgs.push_back("2:算子异常");
		}
	};

	//! 接口

	namespace alg {
		
		//——————————————面阵相机平场校正——————————————//
		//函数说明：通过平场校正的权重因子进行待测图的平场校正
		//SideAlignInput &input;				            输入：待测图像、校正ROI/校正区域、算法参数
		//SideAlignOutput &output; 			            输出：平场校正的图
		_ALGLIBRARY_API int areaFlatFieldCorrect(const AreaFlatFieldCorrInput& input, AreaFlatFieldCorrOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_AREAFLATFIELDCORRECT_H
