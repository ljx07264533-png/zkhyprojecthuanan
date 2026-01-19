
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_GETBGABALL3DFEATURE_H
#define ZKHYPROJECTHUANAN_GETBGABALL3DFEATURE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------获取BGA锡球3D特征------------------------------------------//
	struct GetBGABall3DFeatureInput
	{
		HalconCpp::HObject ballImg;                           //ball深度图
		HalconCpp::HObject baseImg;                           //base深度图
		HalconCpp::HObject ballGray;                          //ball灰度图
		HalconCpp::HObject baseGray;                          //base灰度图
		BGAParam bgaParam;                         //BGA参数设置
		alglib::core::CSYSTransPara layout2DetectTransPara;      //Layout到待测图的坐标变换关系
		std::string loadAlgINIFilePath;                 //加载算法ini文件路径
		alglib::core::AlgrithmParams algPara;					   //算法参数
	};
	struct GetBGABall3DFeatureOutput
	{
		std::vector<std::vector<cv::Point2f>> bga3DAlignROI;       //bga3D Align生成的RoI
		std::vector<std::vector<cv::Point2f>>showContours;         //BGA锡球轮廓信息
		std::vector<alglib::core::FlawInfoStruct>  flaws;               //BGA锡球特征信息
		std::vector<alglib::core::FlawInfoStruct>  flawsDataStatistics; //缺陷信息统计
	};
	struct IntParamsGetBGABall3DFeature//int型算法参数
	{
		int package3DType = 0;            //封测3D检测方案类型（0-单目线激光，1-双目线激光，2-结构光）
		int detectModel = 0;              //检测模式（0-产品IC检测，1-黄金样本检测）
		//翘曲度计算的ROI
		int patchAreaOffsetLeft = 10;     //ROI左区域偏移，像素单位
		int patchAreaOffsetRight = 10;    //ROI右区域偏移，像素单位
		int patchAreaOffsetTop = 10;      //ROI上区域偏移，像素单位
		int patchAreaOffsetBottom = 10;   //ROI下区域偏移，像素单位
		int patchCount = 4;               //ROI个数
		int patchSize = 10;               //ROI尺寸，像素单位
	};
	struct DoubleParamsGetBGABall3DFeature//double型算法参数
	{
		//双目线激光3D方案参数
		double segICRangeMaxValue = 229.937;//分割IC区间最大值（相机软件读取数据mm）
		double segICRangeMinValue = 227.750;//分割IC区间最小值（相机软件读取数据mm）
		double zoomFactor = 1;//缩放系数
	};
	struct AlgParamGetBGABall3DFeature
	{
		union
		{
			IntParamsGetBGABall3DFeature block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsGetBGABall3DFeature block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgGetBGABall3DFeature
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgGetBGABall3DFeature()
		{
			msgs.push_back("segICRangeMaxValue	        $Double   $D_SpinBox	$229.937     $searchROIOffsetLeft   	  $默认值229.937，单位：um。分割IC区间最大值（相机软件读取数据mm） $10");
			msgs.push_back("segICRangeMinValue  	    $Double   $D_SpinBox	$227.750     $searchROIOffsetRight  	  $默认值227.750，单位：um。分割IC区间最小值（相机软件读取数据mm） $20");
			msgs.push_back("zoomFactor         	        $Double   $D_SpinBox	$1.0         $zoomFactor  	              $默认值1.0，单位：无。BGA锡球缩放系数(0，1] $30");
			msgs.push_back("package3DType	            $Int      $I_SpinBox	$0           $package3DType  	          $默认值0，封测3D检测方案类型 $60");
			msgs.push_back("detectModel	                $Int      $I_SpinBox	$0           $detectModel   	          $默认值0，检测模式（0-产品IC检测，1-黄金样本检测） $61");
			msgs.push_back("patchAreaOffsetLeft	        $Int      $I_SpinBox	$10        $patchAreaOffsetLeft   	  $默认值10，单位：像素。ROI左区域偏移。$70");
			msgs.push_back("patchAreaOffsetRight	    $Int      $I_SpinBox	$10        $patchAreaOffsetRight  	  $默认值10，单位：像素。ROI右区域偏移 $80");
			msgs.push_back("patchAreaOffsetTop	        $Int      $I_SpinBox	$10        $patchAreaOffsetTop  	  $默认值10，单位：像素。ROI上区域偏移 $90");
			msgs.push_back("patchAreaOffsetBottom	    $Int      $I_SpinBox	$10        $patchAreaOffsetBottom  	  $默认值10，单位：像素。ROI下区域偏移 $100");
			msgs.push_back("patchCount	                $Int      $I_SpinBox	$4         $patchCount  	          $默认值4。ROI个数 $110");
			msgs.push_back("patchSize	                $Int      $I_SpinBox	$10        $patchSize  	              $默认值10，单位：像素。ROI尺寸 $120");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgGetBGABall3DFeature
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;//扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgGetBGABall3DFeature() {

			exmsgs.push_back("ballHeight         $Double     $球高      $球高         $50    $CF ");
			exmsgs.push_back("ballCoplanarity    $Double	 $球共面度	$球共面度     $51    $CF ");
			exmsgs.push_back("warpage            $Double	 $翘曲度	$翘曲度       $52    $CF ");
		}
	};
	//Step5 异常返回值描述 锡球特征检测错误信息结构体
	struct AlgErrMsgGetBGABall3DFeature
	{
		std::vector<std::string> msgs;
		AlgErrMsgGetBGABall3DFeature()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:输入参数错误");
			msgs.push_back("3:3D特征计算异常");
			msgs.push_back("4:锡球区域数据为空");
			msgs.push_back("5:缺少算法ini文件");
		}
	};
	//-----------------------------获取BGA锡球3D特征------------------------------------------//


	//! 接口

	namespace alg {
		
		//-----------------------------getBGABall3DFeature------------------------------------------//
		//函数说明：获取BGA锡球3D特征
		//GetBGABall3DFeatureInput &input;				输入：BGA锡球深度图、算法参数
		//GetBGABall3DFeatureOutput &output:		    输出：球高、球共面度特征
		_ALGLIBRARY_API int getBGABall3DFeature(GetBGABall3DFeatureInput& input, GetBGABall3DFeatureOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_GETBGABALL3DFEATURE_H
