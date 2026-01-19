
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_QFNINFOSETUP_H
#define ZKHYPROJECTHUANAN_QFNINFOSETUP_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------QFN Param Setup------------------------------------------//
	//BGA检测算子算法参数结构体
	struct QFNSetupInput
	{
		alglib::core::AlgrithmParams alg;               //算法参数
		std::vector<cv::Point2f> inputPoints;  //软件输入中心点坐标
	};
	struct QFNSetupOutput
	{
		cv::Mat showImg;     //排布后的显示图
		QFNParam QFNParam;//BGA参数设置
		std::vector<cv::Point2f> inputPoints;  //中心点坐标，物理坐标
	};
	struct IntParamsQFNSetup
	{
		int gridNumX = 14;        //行数
		int gridNumY = 14;        //列数
	};
	struct DoubleParamsQFNSetup
	{
		double bodySizeX = 8;    //封测尺寸x 长度
		double bodySizeY = 8;    //封测尺寸y 宽度
		double bodySizeZ = 1;    //封测尺寸z 厚度
		double gridOffsetX = 0;  //网格中心偏移x距离
		double gridOffsetY = 0;  //网格中心偏移y距离
		double gridPitchX = 0.18;   //横向间距
		double gridPitchY = 0.18;   //纵向间距
		double padWidth = 0.2;     //pad宽度
		double padLenght = 0.45;    //pad长度
		double padHeight = 0.02;    //pad厚度
		double distToTipX = 0;   //到顶部x距离
		double distToTipY = 0;   //到顶部y距离
		double centerPadLenght = 5.18;//中间pad的长度
		double centerPadWidth = 5.18;//中间pad的宽度
		double mmOfPixelX = 0.014;    //x方向像素当量 mm/pixel
		double mmOfPixelY = 0.014;    //y方向像素当量 mm/pixel
		double leftUpCornerX = 0.64;   //左上角点x距离
		double leftUpCornerY = 0.57;   //左上角点y距离
		double centerPadToLeft = 4;//中间pad中心到左边距离
		double centerPadToUp = 4;//中间pad中心到上边距离
	};
	struct AlgParamQFNSetup
	{
		union
		{
			IntParamsQFNSetup block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsQFNSetup block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgQFNSetup
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//        “参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgQFNSetup()
		{
			msgs.push_back("bodySizeX        $Double    $D_SpinBox	  $8.0  	 $bodySizeX 	      $默认值8.0，单位：mm，必须大于0。  $10");
			msgs.push_back("bodySizeY        $Double    $D_SpinBox	  $8.0  	 $bodySizeY 	      $默认值8.0，单位：mm，必须大于0。  $20");
			msgs.push_back("bodySizeZ        $Double    $D_SpinBox	  $1.0  	 $bodySizeZ 	      $默认值1.0，单位：mm，必须大于0。  $30");
			msgs.push_back("gridNumX   	     $Int       $I_Spinbox	  $14  	     $gridNumX  	      $默认值3，必须大于0。  $40");
			msgs.push_back("gridNumY   	     $Int       $I_Spinbox	  $14  	     $gridNumY  	      $默认值3，必须大于0。  $50");
			msgs.push_back("gridOffsetX      $Double    $D_SpinBox	  $0.0  	 $gridOffsetX 	      $默认值0，单位：mm，必须大于等于0。  $60");
			msgs.push_back("gridOffsetY      $Double    $D_SpinBox	  $0.0  	 $gridOffsetY 	      $默认值0，单位：mm，必须大于等于0。  $70");
			msgs.push_back("gridPitchX       $Double    $D_SpinBox	  $0.18  	 $gridPitchX 	      $默认值0.18，单位：mm，必须大于0。  $80");
			msgs.push_back("gridPitchY       $Double    $D_SpinBox	  $0.18  	 $gridPitchY 	      $默认值0.18，单位：mm，必须大于0。  $90");
			msgs.push_back("padWidth         $Double    $D_SpinBox	  $0.2  	 $padWidth 	          $默认值0.2，单位：mm，必须大于0。  $100");
			msgs.push_back("padLenght        $Double    $D_SpinBox	  $0.45  	 $padLenght 	      $默认值0.45，单位：mm，必须大于0。  $110");
			msgs.push_back("padHeight        $Double    $D_SpinBox	  $0.02  	 $padHeight 	      $默认值0.02，单位：mm，必须大于0。  $120");
			msgs.push_back("distToTipX       $Double    $D_SpinBox	  $0.0  	 $distToTipX 	      $默认值0，单位：mm，必须大于等于0。  $130");
			msgs.push_back("distToTipY       $Double    $D_SpinBox	  $0.0  	 $distToTipY 	      $默认值0，单位：mm，必须大于等于0。  $140");
			msgs.push_back("centerPadLenght  $Double    $D_SpinBox	  $5.18  	 $centerPadLenght 	  $默认值5.18，单位：mm，必须大于0。  $150");
			msgs.push_back("centerPadWidth   $Double    $D_SpinBox	  $5.18  	 $centerPadWidth 	  $默认值5.18，单位：mm，必须大于0。  $160");
			msgs.push_back("mmOfPixelX       $Double    $D_SpinBox	  $0.014  	 $mmOfPixelX 	      $默认值0.014，单位：mm/pixel，必须大于0。  $170");
			msgs.push_back("mmOfPixelY       $Double    $D_SpinBox	  $0.014  	 $mmOfPixelY 	      $默认值0.014，单位：mm/pixel，必须大于0。  $180");
			msgs.push_back("leftUpCornerX    $Double    $D_SpinBox	  $0.64  	 $leftUpCornerX 	  $默认值0.64，单位：mm，必须大于0。  $190");
			msgs.push_back("leftUpCornerY    $Double    $D_SpinBox	  $0.57  	 $leftUpCornerY 	  $默认值0.57，单位：mm，必须大于0。  $200");
			msgs.push_back("centerPadToLeft  $Double    $D_SpinBox	  $5.0  	 $centerPadToLeft 	  $默认值5.0，单位：mm，必须大于0。  $210");
			msgs.push_back("centerPadToUp    $Double    $D_SpinBox	  $5.0  	 $centerPadToUp 	  $默认值5.0，单位：mm，必须大于0。  $220");
		}
	};
	struct AlgErrMsgQFNSetup
	{
		std::vector<std::string> msgs;
		AlgErrMsgQFNSetup()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入参数错误");
			msgs.push_back("2:PKG输入中心点个数为0");
			msgs.push_back("3:PKG Layout计算异常");
		}
	};
	//-----------------------------PKG Information Setup------------------------------------------//

	//! 接口

	namespace alg {
		_ALGLIBRARY_API int QFNInfoSetup(const QFNSetupInput& input, QFNSetupOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_QFNINFOSETUP_H
