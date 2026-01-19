
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_BGAINFOSETUP_H
#define ZKHYPROJECTHUANAN_BGAINFOSETUP_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"
#include "alglibrary/zkhyProjectHuaNan/alglibmisc.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------BGA Param Setup------------------------------------------//
	//BGA检测算子算法参数结构体
	struct BGASetupInput
	{
		alglib::core::AlgrithmParams alg;               //算法参数
		std::vector<cv::Point2f> inputPoints;  //软件输入中心点坐标
	};
	struct BGASetupOutput
	{
		cv::Mat showImg;     //排布后的显示图
		BGAParam BGAParam;//BGA参数设置
		std::vector<cv::Point2f> inputPoints;  //中心点坐标，物理坐标
	};
	struct IntParamsBGASetup
	{
		int gridNumX = 12;        //行数
		int gridNumY = 22;        //列数
	};
	struct DoubleParamsBGASetup
	{
		double bodySizeX = 15;    //封测尺寸x 长度
		double bodySizeY = 10;    //封测尺寸y 宽度
		double bodySizeZ = 1;    //封测尺寸z 厚度
		double gridOffsetX = 0;  //网格中心偏移x距离
		double gridOffsetY = 0;  //网格中心偏移y距离
		double gridPitchX = 0.64;   //横向间距
		double gridPitchY = 0.8;   //纵向间距
		double ballWidth = 0.33;    //球宽
		double ballHeight = 0.22;   //球高
		double mmOfPixelX = 0.014;   //x方向像素当量 mm/pixel
		double mmOfPixelY = 0.014;   //y方向像素当量 mm/pixel
		double leftUpCornerX = 0.64;   //左上角点x距离
		double leftUpCornerY = 0.57;   //左上角点y距离
	};
	struct AlgParamBGASetup
	{
		union
		{
			IntParamsBGASetup block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsBGASetup block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgBGASetup
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//        “参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgBGASetup()
		{
			msgs.push_back("bodySizeX        $Double    $D_SpinBox	  $15.0  	 $bodySizeX 	      $默认值15，单位：mm，必须大于0。  $10");
			msgs.push_back("bodySizeY        $Double    $D_SpinBox	  $10.0  	 $bodySizeY 	      $默认值10，单位：mm，必须大于0。  $20");
			msgs.push_back("bodySizeZ        $Double    $D_SpinBox	  $1.0  	 $bodySizeZ 	      $默认值1，单位：mm，必须大于0。  $30");
			msgs.push_back("gridNumX   	     $Int       $I_Spinbox	  $12  	     $gridNumX  	      $默认值12，必须大于0。  $40");
			msgs.push_back("gridNumY   	     $Int       $I_Spinbox	  $22  	     $gridNumY  	      $默认值22，必须大于0。  $50");
			msgs.push_back("gridOffsetX      $Double    $D_SpinBox	  $0.0  	 $gridOffsetX 	      $默认值0，单位：mm，必须大于等于0。  $60");
			msgs.push_back("gridOffsetY      $Double    $D_SpinBox	  $0.0  	 $gridOffsetY 	      $默认值0，单位：mm，必须大于等于0。  $70");
			msgs.push_back("gridPitchX       $Double    $D_SpinBox	  $0.49  	 $gridPitchX 	      $默认值0.49，单位：mm，必须大于0。  $80");
			msgs.push_back("gridPitchY       $Double    $D_SpinBox	  $0.63  	 $gridPitchY 	      $默认值0.63，单位：mm，必须大于0。  $90");
			msgs.push_back("ballWidth        $Double    $D_SpinBox	  $0.2  	 $ballWidth 	      $默认值0.2，单位：mm，必须大于0。  $100");
			msgs.push_back("ballHeight       $Double    $D_SpinBox	  $0.22  	 $ballHeight 	      $默认值0.22，单位：mm，必须大于0。  $110");
			msgs.push_back("mmOfPixelX       $Double    $D_SpinBox	  $0.014  	 $mmOfPixelX 	      $默认值0.014，单位：mm/pixel，必须大于0。  $120");
			msgs.push_back("mmOfPixelY       $Double    $D_SpinBox	  $0.014  	 $mmOfPixelY 	      $默认值0.014，单位：mm/pixel，必须大于0。  $130");
		}
	};
	struct AlgErrMsgBGASetup
	{
		std::vector<std::string> msgs;
		AlgErrMsgBGASetup()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入参数错误");
			msgs.push_back("2:输入中心点个数为0");
			msgs.push_back("3:PKG计算异常");
		}
	};


	//! 接口

	namespace alg {
		_ALGLIBRARY_API int BGAInfoSetup(const BGASetupInput &input, BGASetupOutput &output);
	}

	void pointKdTreeBuild3D(std::vector<cv::Point2f>& points, kdtree& kdTreerootMask);
	void searchNearestNew3D(kdtree& kdTreerootMask, cv::Point2f target, cv::Point2f& nearestPoint, double& distance, int numNearest, int funcId);
}

#endif	// ZKHYPROJECTHUANAN_BGAINFOSETUP_H
