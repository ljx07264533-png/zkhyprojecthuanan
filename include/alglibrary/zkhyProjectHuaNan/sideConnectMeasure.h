
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: 侧面搭接测量（connectMeasure）算子，详细内容:侧面搭接测量（connectMeasure）算子
***************************************************/
#ifndef ZKHYPROJECTHUANAN_CONNECTMEASURE_H
#define ZKHYPROJECTHUANAN_CONNECTMEASURE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体


	//————————————————————————sideConnectMeasure 侧面搭接测量算子 Begin————————————————//
	//Step1 输入结构体
	struct SideConnectMeasureInput
	{
		HalconCpp::HObject grayImg;                    //输入灰度图像
		alglib::core::AlgrithmParams algPara;			    //算法参数
		alglib::core::LineStruct linePic;					//拟合直线
	};
	//Step2 输出结构体

	struct SideConnectMeasureOutput
	{
		std::vector <alglib::core::MeasureInfoStruct> measureinfostruct;            //金属区域与线路区域的中心偏差的缺陷信息与金属搭接区域与金线区域的缺陷信息
		HalconCpp::HObject Region1;									   //显示区域1
		HalconCpp::HObject Region2;								       //显示区域2
		alglib::core::LineStruct line1;								   //显示直线1
		alglib::core::LineStruct line2;								   //显示直线2
	};
	//Step3 算法参数（int double）
	struct AlgParaSideConnectMeasureInt       //int型算法参数，独立出来方便使用默认构造函数
	{
		int lineDownRow1 = 70;          //金属区域定位的线高度，需要在区域获取位置，值为距离直线纵坐标的差值
		int LapareaValue1 = 50;			 //金属区域阈值1						
		int LapareaValue2 = 255;		 //金属区域阈值2
		int connectRegionH1 = 30;        //金属区域矩形的宽高
		int autoLocationRectH = 20;      //金属区域矩形宽高
		int autoLocationRectW = 25;      //金属区域矩形宽高

		int lineDownRow2 = -70;             //金属区域与搭接矩形偏差值
		int LapareaValue3 = 50;			  //搭接区域阈值1
		int LapareaValue4 = 255;		  //搭接区域阈值2
		int connectRegionW = 20;         //搭接区域矩形的宽高
		int connectRegionH = 30;        //搭接区域矩形的宽高
		int autoLocationRectH1 = 30;      //搭接区域宽高
		int autoLocationRectW1 = 30;      //搭接区域宽高

		int lineUpPoint1R = 5;		  //金线区域上高度
		int lineUpPoint1L = 60;		  //金线区域下高度
		int LapareaValue7 = 0;			  //金线区域阈值
		int LapareaValue8 = 50;			  //金线区域阈值
		int autoLocationRectH2 = 30;      //金线区域宽高
		int autoLocationRectW2 = 30;      //金线区域宽高

	};
	struct AlgParaSideConnectMeasureDouble    //double型算法参数，独立出来方便使用默认构造函数
	{
		//double umPixel = 1.2;             //像素当量
	};
	struct AlgParaSideConnectMeasure
	{
		union
		{
			AlgParaSideConnectMeasureInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaSideConnectMeasureDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgSideConnectMeasure
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgSideConnectMeasure()
		{
			msgs.push_back("lineDownRow1			$Int		$I_Spinbox	 $70  	     $金属区域到直线差值				 $金属区域定位的线高度，需要在区域获取位置，值为距离直线纵坐标的差值	        $10");
			msgs.push_back("LapareaValue1			$Int		$I_Spinbox	 $50  		 $金属区域阈值1						 $金属区域阈值1	    $20");
			msgs.push_back("LapareaValue2			$Int		$I_Spinbox	 $255  		 $金属区域阈值2				         $金属区域阈值2	    $30");
			msgs.push_back("connectRegionH1		    $Int		$I_Spinbox	 $30 		 $金属矩形高						 $金属矩形宽	$40");
			msgs.push_back("autoLocationRectH		$Int		$I_Spinbox	 $20  		 $金属区域高						 $金属区高	$50");
			msgs.push_back("autoLocationRectW	    $Int		$I_Spinbox	 $25 		 $金属区域宽						 $金属区域宽	$60");

			msgs.push_back("lineDownRow2    		$Int		$I_Spinbox	 $-70  		 $金属区域与搭接矩形偏差值		 $金属区域与搭接矩形偏差值	$70");
			msgs.push_back("LapareaValue3			$Int		$I_Spinbox	 $50  		 $搭接区域阈值1					 $搭接区域阈值1	$80");
			msgs.push_back("LapareaValue4			$Int		$I_Spinbox	 $255 		 $搭接区域阈值2					 $搭接区域阈值2	$90");
			msgs.push_back("connectRegionW  		$Int		$I_Spinbox	 $20  		 $搭接矩形高					 $搭接矩形高	$100");
			msgs.push_back("connectRegionH		    $Int		$I_Spinbox	 $30 		 $搭接矩形宽					 $搭接矩形宽	$110");
			msgs.push_back("autoLocationRectH1		$Int		$I_Spinbox	 $30  		 $搭接区域高					 $搭接区域高	$120");
			msgs.push_back("autoLocationRectW1	    $Int		$I_Spinbox	 $30 		 $搭接区域宽					 $搭接区域宽	$130");

			msgs.push_back("lineUpPoint1R  			$Int		$I_Spinbox	 $5 		 $金线区域上高度				 $金线区域上高度	$140");
			msgs.push_back("lineUpPoint1L		    $Int		$I_Spinbox	 $60 		 $金线区域下高度				 $金线区域下高度	$150");
			msgs.push_back("LapareaValue7			$Int		$I_Spinbox	 $0  		 $金属区域阈值1					 $金属区域阈值1	$160");
			msgs.push_back("LapareaValue8			$Int		$I_Spinbox	 $50		 $金属区域阈值2					 $金属区域阈值2	$170");
			msgs.push_back("autoLocationRectH2		$Int		$I_Spinbox	 $30  		 $金属区域高					 $金属区域高	$180");
			msgs.push_back("autoLocationRectW2	    $Int		$I_Spinbox	 $30 		 $金属区域宽					 $金属区域宽	$190");

		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgSideConnectMeasure
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgSideConnectMeasure()
		{

		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgSideConnectMeasure
	{
		std::vector<std::string> msgs;
		AlgErrMsgSideConnectMeasure()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:图像为空");
			msgs.push_back("2:缺少金属搭接区域");
			msgs.push_back("3:缺少金线区域");

		}
	};
	//————————————————————————sideConnectMeasure 侧面搭接测量算子 End————————————————//

	//! 接口

	namespace alg {
		
		//----------------------------- sideConnectMeasure-----------------------------------------//
		//函数说明：侧面搭接测量
		//SideConnectMeasureInput &input;				输入：待测图像
		//SideConnectMeasureOutput &output; 			输出：金线区域与搭接区域的偏差▲x，金属区域与线路区域的中心偏差(▲x和▲y)
		_ALGLIBRARY_API int sideConnectMeasure(const SideConnectMeasureInput& input, SideConnectMeasureOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_CONNECTMEASURE_H
