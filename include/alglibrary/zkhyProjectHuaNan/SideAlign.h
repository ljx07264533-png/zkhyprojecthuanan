
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_SIDEALIGN_H
#define ZKHYPROJECTHUANAN_SIDEALIGN_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------------------------------------侧面检测区域定位--------------------------------------------------------//
	struct SideAlignInput
	{
		cv::Mat                   mImg;                        //侧面图像
		std::vector<alglib::core::RectangleROI>  vRectangleROI;               //矩形ROI，输入矩形ROI数量为3,第一个侧面中间矩形框 第二个侧面左侧 第三个侧面右侧
		alglib::core::AlgrithmParams        algPara;                     //算法参数
	};
	struct SideAlignOutput
	{
		HalconCpp::HObject			           sideRegion;	          //侧面区域轮廓
		std::vector<std::vector<cv::Point2f>>    contourPts;	          //侧面区域轮廓点集
		std::vector<std::vector<cv::Point2f>>    fitLineContourPts;     //拟合侧面4个边界直线的点集 边界点集顺序：上下-左-右

	};

	//Step2 算子参数 结构体定义 int和double分开写
	struct AlgParamSideAlignInt                           //int型算法参数，独立出来方便使用默认构造函数
	{
		int searchStepSide = 0;                          //侧面上下边界搜索步长
		int nEdgePolarity = 0;			                 //图像边缘属性，0-ALL，1-黑到白，2-白到黑
		int nEdgePosition = 0;			                 //图像边缘位置，0-所有位置，1-第一个点，2-最后一个点
		int nDilationDist = 0;                           //侧面左右两侧腐蚀距离
		int nFitLineEdgePolarity = 1;                    //侧面左右拟合直线时边缘属性
		int fitLineAmpThresh = 50;                       //侧面左右边界拟合直线阈值分割阈值
	};

	struct AlgParamSideAlignDouble                       //double型算法参数，独立出来方便使用默认构造函数
	{
		double sideSpecHeight = 0.0;                     //侧面指定高度
		double sideRangeHeight = 0.0;                    //侧面允许的高度变化范围
		double ampThreshSide = 15;                       //侧面边界拟合边缘点最小梯度
	};

	struct AlgParamSideAlign
	{
		union
		{
			AlgParamSideAlignInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParamSideAlignDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};

	//Step3 算子参数描述
	struct AlgParaMsgSideAlign
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgSideAlign()
		{
			msgs.push_back("searchStepSide   	 $Int       $I_Spinbox	 $20 	  $搜索边缘点步长                          $侧面拟合上下边界直线时从左至右搜索边缘点步长，默认值20，搜索步长大于0              $10");
			msgs.push_back("nEdgePolarity	     $Int       $Combox	     $1	      $图像边缘属性                            $寻找的边缘点沿扫描线的方向灰度变化，默认值1， 0-ALL 1-黑到白 2-白到黑              $20");
			msgs.push_back("nEdgePosition	     $Int       $Combox	     $0  	  $图像边缘位置                            $侧面选取寻找到的边缘点的位置，默认值0， 0-所有位置 1-第一个点 2-最后一个点         $30");
			msgs.push_back("sideSpecHeight       $Double    $D_Spinbox   $20      $侧面指定高度                            $侧面寻找上下边界边缘对的高度，默认值20, 单位:pixel                                 $40");
			msgs.push_back("sideRangeHeight      $Double    $D_Spinbox   $0.3     $侧面允许高度变化范围                    $侧面寻找上下边界边缘对距离允许的范围，默认值0.3                                    $50");
			msgs.push_back("ampThreshSide        $Double    $D_Spinbox   $15      $边缘拟合点最小梯度值                    $侧面边缘拟合点允许的最小梯度值，默认值15                                           $60");
			msgs.push_back("nDilationDist        $Int       $I_Spinbox   $5       $侧面两侧向内腐蚀距离                    $利用产品两侧拟合的直线向内腐蚀的距离，默认值5                                      $70");
			msgs.push_back("nFitLineEdgePolarity $Int       $Combox	     $1	      $图像边缘属性(拟合直线)                  $拟合直线时寻找的边缘点沿扫描线的方向灰度变化，默认值1， 0-ALL 1-黑到白 2-白到黑    $80");
			msgs.push_back("fitLineAmpThresh     $Double    $D_Spinbox   $50      $二值化阈值(拟合直线)                    $拟合直线使用阈值分割的阈值，默认值50，范围：[0,255]前景背景的交界灰度值            $90");
		}
	};

	//Step4 异常返回值描述
	struct AlgErrMsgSideAlign
	{
		std::vector<std::string> msgs;
		AlgErrMsgSideAlign()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:ROI过小不符合规范");
			msgs.push_back("3:边缘点失败");
			msgs.push_back("4:拟合直线失败");
			msgs.push_back("5:拟合直线与ROI求交点失败");
			msgs.push_back("6:拟合直线与图片边缘求交点失败");
			msgs.push_back("7:输入参数异常,边缘灰度阈值超出范围限定(0,128)");
			msgs.push_back("8:输入ROI数量不等于3");
			msgs.push_back("9:侧面高度范围设置异常");
			msgs.push_back("10:侧面边界查找边缘点梯度设置异常");
			msgs.push_back("11:侧面边界查找边缘点步长设置异常");
			msgs.push_back("12:侧面上下边界拟合失败");
			msgs.push_back("13:侧面左侧与上下边界求取交点失败");
			msgs.push_back("14:侧面右侧与上下边界求取交点失败");
			msgs.push_back("15:算法未知异常");
		}
	};

	//! 接口

	namespace alg {
		
		//-----------------------------SideAlign------------------------------------------//
		//函数说明：封测侧面检测区域定位，根据输入的矩形框拟合侧面4条边边界，输出侧面检测区域
		//SideAlignInput &input;				        输入：待测图像、手动输入ROI、算法参数
		//SideAlignOutput &output; 			            输出：侧面检测区域、边界拟合的直线、边界拟合的点集
		_ALGLIBRARY_API int SideAlign(const SideAlignInput& input, SideAlignOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_SIDEALIGN_H
