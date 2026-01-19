
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_INSPECTSEGEMENT_H
#define ZKHYPROJECTHUANAN_INSPECTSEGEMENT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------加载分割图片差异化模型 Start----------------//
	struct InspectSegementInput
	{
		HalconCpp::HObject   Src;//模板图像//检测图像;
		HalconCpp::HObject InspRegion;//检测区域
		SegmentShap segment;//加载信息
		std::vector<std::vector<cv::Point2f>>  InspROI; //查找模板2 匹配区域。
		alglib::core::AlgrithmParams algPara;	//算法参数

	};
	struct  InspectSegementOutput
	{
		HalconCpp::HObject ho_defectRegion;//缺陷区域
		std::vector<alglib::core::FlawInfo>	flawinfo;				//缺陷信息
	};

	struct IntParamsInspectSegement//int型算法参数，独立出来方便使用默认构造函数
	{
		int Bthre;
		int Dthre;
		int NW;
		int NH;
		int maskSize;
	};
	struct DoubleParamsInspectSegement//double型算法参数，独立出来方便使用默认构造函数
	{

	};
	struct AlgParamInspectSegement
	{
		union
		{
			IntParamsInspectSegement block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsInspectSegement block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};


	struct AlgParaMsgInspectSegement
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgInspectSegement()
		{
			msgs.push_back("Bthre	$Int		$I_Spinbox	$50  				$亮缺陷阈值		$检测亮缺陷的阈值，范围[1,255]	$10");
			msgs.push_back("Dthre	$Int		$I_Spinbox	$50 			    $暗缺陷阈值		$检测暗缺陷的阈值，范围[1,255]	$20");
			msgs.push_back("NW	$Int			$I_Spinbox	$1  				$大图裁剪列数		$大图裁剪列数，范围[1,10]	$30");
			msgs.push_back("NH	$Int			$I_Spinbox	$3  			    $大图裁剪行数		$大图裁剪行数，范围[1,10]	$40");
			msgs.push_back("maskSize	$Int    $I_Spinbox	$4  			    $灰度膨胀大小		$图像灰度膨胀大小，范围[1,30]	$50");
		}
	};

	struct AlgErrMsgInspectSegement
	{
		std::vector<std::string> msgs;
		AlgErrMsgInspectSegement()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:异常");
			msgs.push_back("3:缺陷个数超过一千检查mark或者重新训练");
			msgs.push_back("4:训练和检测行列不一致。");
			msgs.push_back("5:异常");
		}
	};



	//! 接口

	namespace alg {
		_ALGLIBRARY_API int inspectSegement(const InspectSegementInput& input, InspectSegementOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_INSPECTSEGEMENT_H
