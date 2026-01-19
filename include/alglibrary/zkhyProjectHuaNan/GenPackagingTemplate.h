
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_GENPACKAGINGTEMPLATE_H
#define ZKHYPROJECTHUANAN_GENPACKAGINGTEMPLATE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//Step1 算子input结构体、output结构体
	//-----------------------------GenPackagingTemplate算子名称------------------------------------------//
	struct GenPackagingTemplateInput
	{
		HalconCpp::HObject srcImg;                         //输入图像
		std::string filePath = "";                   //读取dxf文件路径
		alglib::core::AlgrithmParams algPara;					   //算法参数
	};
	struct GenPackagingTemplateOutput
	{
		cv::Mat templateImg;                              //模板图
		HalconCpp::HObject templateRegion;                       //模板区域
		std::vector<std::vector<cv::Point2f>> templateContour;      //模板轮廓点集
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct IntParamsGenPackagingTemplate//int型算法参数，独立出来方便使用默认构造函数
	{
		int iPackagingType = 0;                   //封装类型
	};
	struct DoubleParamsGenPackagingTemplate//double型算法参数，独立出来方便使用默认构造函数
	{
		double dTolerance = 1.0;                       //逼近距离
		double dMmPrePixel = 1.0;                      //像素当量
		double dTimeOutThreshold = 3000;               //算法超时设定
	};
	struct AlgParamGenPackagingTemplate
	{
		union
		{
			IntParamsGenPackagingTemplate block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsGenPackagingTemplate block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgGenPackagingTemplate
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgGenPackagingTemplate()
		{
			msgs.push_back("iPackagingType      $Int  $Combox   $0   $封装类型                      $默认为BGA封装，支持BGA、QFN封装 ￥BGA　￥QFN $10");
			msgs.push_back("dTolerance                 $Double     $D_Spinbox	$1.0 	     $逼近距离阈值(单位：像素)			    $默认值1.0,范围:[0,∞]。目标物体轮廓通过多边形逼近的距离阈值 $20");
			msgs.push_back("dMmPrePixel      $Double  $D_Spinbox   $1.0   $像素当量(毫米/像素)                      $默认值1.0，范围(0,∞]。设备的像素当量，单位为毫米/像素 $30");
			msgs.push_back("dTimeOutThreshold      $Double  $D_Spinbox   $3000.0   $算法超时设定(单位：ms)                      $默认值3000.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常 $40");
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgGenPackagingTemplate
	{
		std::vector<std::string> msgs;
		AlgErrMsgGenPackagingTemplate()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:文件格式错误");
			msgs.push_back("3:像素当量输入错误");
			msgs.push_back("4:算法超时参数输入有误");
			msgs.push_back("5:读取dxf文件失败");
			msgs.push_back("6:算法超时");
			msgs.push_back("7:未知错误");
		}
	};


	//! 接口

	namespace alg {
		
		//-----------------------------GenPackagingTemplate------------------------------------------//
		//函数说明：根据DXF文件生成标准的封装器件底部pin脚或锡球模板排列
		//GenPackagingTemplateInputt &input;				输入：模板图像、DXF文件路径、像素当量、封装类型
		//GenPackagingTemplateOutput &output;				输出：模板图信息
		_ALGLIBRARY_API int GenPackagingTemplate(const GenPackagingTemplateInput& input, GenPackagingTemplateOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_GENPACKAGINGTEMPLATE_H
