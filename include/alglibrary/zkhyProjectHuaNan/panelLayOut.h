
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_PANELLAYOUT_H
#define ZKHYPROJECTHUANAN_PANELLAYOUT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	
	struct LayOutInput
	{
		cv::Mat               img;             //输入图像
		GlassLayout  layOut;           //panel的布局
		std::vector<cv::Point2f>   recpoint;         //前序算子产品五个坐标点
		alglib::core::CSYSTransPara     templateTransPara;  //坐标系
		alglib::core::AlgorithmParams   algPara;            //算法参数
	};
	struct LayOutOutput
	{
		cv::Mat     img;//界面显示
		HalconCpp::HObject himg;  // 产品H图像
		std::vector<PanelRegion>     pRegion; //panel区域
		std::vector<std::vector<cv::Point2f>> contours;//panel布局轮廓显示

	};
	struct AlgParaMuraRegionDouble
	{

	};
	struct AlgParaMuraRegionInt
	{
		int Trans;//是否经过坐标系变换。
	};

	struct AlgParaMuraRegion
	{
		union
		{
			AlgParaMuraRegionInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaMuraRegionDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct   AlgParaMsgMuraRegion
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgMuraRegion()
		{
			msgs.push_back("Trans          $Int      $CheckBox	 $0  	 $是否链接坐标	     $坐标系区域链接 	$10");
		}
	};

	struct AlgErrMsgMuraRegion
	{
		std::vector<std::string> msgs;
		AlgErrMsgMuraRegion()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:图像为空");
			msgs.push_back("2:软件传入为空");
			msgs.push_back("3:未知异常");
		}
	};

	//————————————————————————MuraGetPanel mura End———————————\
	//! 接口

	namespace alg {
		_ALGLIBRARY_API int panelLayOut(const LayOutInput& input, LayOutOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_PANELLAYOUT_H
