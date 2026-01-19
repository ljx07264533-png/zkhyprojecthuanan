
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_MINILEDGLUEDETECT_H
#define ZKHYPROJECTHUANAN_MINILEDGLUEDETECT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//————————————————————————miniLEDGlueDetect miniLED胶水检测算子 Begin————————————————//
	//Step1 输入结构体
	struct MiniLEDGlueDetectInput
	{
		HalconCpp::HObject baseDeepImg;                        //基板深度图像
		HalconCpp::HObject baseGrayImg;                        //基板灰度图像
		HalconCpp::HObject lensDeepImg;                        //芯片深度图像
		HalconCpp::HObject lensGrayImg;                        //芯片灰度图像
		alglib::core::AlgrithmParams algPara;					//算法参数
	};
	//Step2 输出结构体
	struct MiniLEDGlueDetectOutput
	{
		HalconCpp::HObject defectRegion;                   //缺陷胶水区域
		std::vector<alglib::core::FlawInfoStruct>	flawinfos;		//缺陷所有信息

		HalconCpp::HObject icGlueRegion;                   //芯片胶水区域
		HalconCpp::HObject bottomGlueRegion;                   //底部胶水区域
		HalconCpp::HObject upGlueRegion;                   //顶部胶水区域
		HalconCpp::HObject bottomRingRegion;                   //基板圆环
		HalconCpp::HObject icRegion;                   //芯片区域
	};
	//Step3 算法参数（int double）
	struct AlgParaMiniLEDGlueDetectInt          //int型算法参数，独立出来方便使用默认构造函数
	{
		int glueLowGray;				//芯片胶水最小灰度值
		int glueHightGray;				 //芯片胶水最大灰度值
		int icGlueOpeningSize;			 //芯片胶水区域开运算大小

		int gluebottomDilationRadius;         //芯片胶水区域外扩找底部胶水
		int bottomGlueThreshold;			 //基板胶水区域低阈值

		int glueUpErosionSize;				 //底部胶水区域找顶点胶水区域内缩大小
		int upGlueMinHight;					//胶水顶部最小高度
		int glueUpOpeningSize;				//顶部胶水区域开运算核大小

		int dilationRing;						// 外扩圆环半径
		int fitbackgroundHeightMode;		//求基板高度方法
		int icOpeningSize;					//芯片区域开运算大小

		int isContours;						//是否输出详细轮廓

	};
	struct AlgParaMiniLEDGlueDetectDouble          //double型算法参数，独立出来方便使用默认构造函数
	{
		double mmPixleZ;  //Z轴的像素当量
	};
	struct AlgParaMiniLEDGlueDetect
	{
		union
		{
			AlgParaMiniLEDGlueDetectInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaMiniLEDGlueDetectDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgMiniLEDGlueDetect
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgMiniLEDGlueDetect()
		{
			msgs.push_back("glueLowGray				$Int		$I_Spinbox	 $30  	 $芯片胶水最小灰度值			 $S1V3芯片和胶水区域的最小灰度值	$10");
			msgs.push_back("glueHightGray			$Int		$I_Spinbox	 $150  	 $芯片胶水最大灰度值			 $S1V3芯片和胶水区域的最大灰度值	$20");
			msgs.push_back("icGlueOpeningSize		 $Int		$I_Spinbox	 $10  	 $芯片胶水区域开运算大小	     $S1V3芯片和胶水区域开运算大小	$30");

			msgs.push_back("gluebottomDilationRadius $Int		$I_Spinbox	 $100	 $芯片胶水区域外扩参数	     $S1V3芯片和胶水区域外接圆后，在外扩区域找底部胶水	$40");
			msgs.push_back("bottomGlueThreshold      $Int		$I_Spinbox	 $70  	 $基板胶水区域高阈值		 $S1V1的基板胶水区域高阈值，低阈值默认为0	$50");

			msgs.push_back("glueUpErosionSize		$Int		$I_Spinbox	 $50  	 $底部胶水区域找顶点胶水区域内缩大小	 $S1V1的底部胶水区域找顶点胶水区域内缩大小	$60");
			msgs.push_back("upGlueMinHight			$Int		$I_Spinbox	 $30000  $胶水顶部最小阈值			$S1V0胶水顶部最小高度阈值，会在S1V0高度图中找出顶部胶水区域 或 在S1V1中根据阈值方法找出顶部胶水	$70");
			msgs.push_back("glueUpOpeningSize		$Int		$I_Spinbox	 $10  	 $顶部胶水区域开运算核大小	 $S1V0顶部胶水区域进行开运算核大小	$80");

			msgs.push_back("dilationRing			$Int		$I_Spinbox	 $8		$外扩圆环半径			$利用该参数，在底部胶水区域进行外扩圆，得到圆环	$82");
			msgs.push_back("fitbackgroundHeightMode $Int		$Combox	 $1			$求基板高度方法			$求S1V0基板高度方法：￥拟合平面￥圆环均值	$90");
			msgs.push_back("icOpeningSize $Int		$I_Spinbox	$100  				$芯片区域开运算大小		$S1V1的芯片区域开运算大小	$100");

			msgs.push_back("isContours $Int			$CheckBox	$1  				$是否输出详细轮廓		$是否输出详细轮廓	$100");

			msgs.push_back("mmPixleZ   $Double      $D_Spinbox	$0.0067			$Z轴像素当量	 $Z轴的像素当量，单位为mm每像素	$110");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgMiniLEDGlueDetect
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgMiniLEDGlueDetect()
		{
			// 胶水特征
			msgs.push_back("glueHeight		    $Double	   $miniLed胶水高度			    $miniLed胶水高度				$147   $OF          $Hidden      $(p)");
			msgs.push_back("lensHeight		    $Double	   $miniLedLens高度			    $miniLedLens高度				$148   $OF          $Hidden      $(p)");
			msgs.push_back("glueToLensHeight	$Double	   $miniLed胶水到lens高度		$miniLed胶水到lens高度			$149   $OF          $Hidden      $(p)");
			msgs.push_back("glueToLensOffset	$Double	   $胶水与lens中心偏移量		$胶水与lens中心偏移量			$150   $OF          $Hidden      $(p)");
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgMiniLEDGlueDetect
	{
		std::vector<std::string> msgs;
		AlgErrMsgMiniLEDGlueDetect()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:图像为空");
			msgs.push_back("2:没有找到胶水区域");
			msgs.push_back("3:没有找到芯片区域");
			msgs.push_back("4:未知异常");
			msgs.push_back("5:芯片胶水数量 和 底部胶水数量 不一致，请检查参数");
			msgs.push_back("6:芯片胶水数量 和 顶部胶水数量 不一致，请检查参数，可能原因是顶部胶水的开运算核大小 过大");
			msgs.push_back("7:芯片中心坐标数量 和 底部中心坐标数量 不一致，请注意排查参数");
		}
	};
	//————————————————————————miniLEDGlueDetect miniLED胶水检测算子 End————————————————//

	//! 接口

	namespace alg {
		
		//-----------------------------miniLEDGlueDetect------------------------------------------//
		//函数说明：miniLED胶水检测
		//MiniLEDGlueDetectInput &input;				        输入：待测图像、手动输入ROI、算法参数
		//MiniLEDGlueDetectOutput &output; 			            输出：胶水区域、缺陷特征
		_ALGLIBRARY_API int miniLEDGlueDetect(const MiniLEDGlueDetectInput& input, MiniLEDGlueDetectOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_MINILEDGLUEDETECT_H
