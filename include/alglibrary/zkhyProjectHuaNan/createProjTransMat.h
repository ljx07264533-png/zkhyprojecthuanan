
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_CREATEPROJTRANSMAT_H
#define ZKHYPROJECTHUANAN_CREATEPROJTRANSMAT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------------------------创建投影变换矩阵------------------------//
	struct CreateProjTransMatInput
	{
		cv::Mat msrcImg;                           //标定板图像，用于求取变换前后4个点
		std::vector<alglib::core::RectangleROI>  vRectangleROI;    //矩形ROI
		SaveProjTransMatPath savePath;         //投影变换矩阵存储路径
		alglib::core::AlgrithmParams algPara;                //算法参数
	};
	struct CreateProjTransMatOutput
	{
		HalconCpp::HTuple projTransMat;                           //输出投影变换矩阵，平台启动时可加载投影变换矩阵
		std::vector<alglib::core::PointStruct> vCornerPoints;             //投影变换前4个角点
		std::vector<alglib::core::PointStruct> vCornerPointsTrans;        //投影变换后4个角点
	};

	//Step2 算子参数 结构体定义 int和double分开写
	struct AlgParamCreateProjTransMatInt//int型算法参数，独立出来方便使用默认构造函数
	{
		int nEdgePolarity = 0;	//图像边缘属性，0-ALL，1-黑到白，2-白到黑  //高精度拟合直线参数
		int nThreshold = 50;   //二值化阈值
	};

	struct AlgParamCreateProjTransMatDouble//double型算法参数，独立出来方便使用默认构造函数
	{
		double pixelDist = 0;          //像素精度  单个像素代表的实际距离 单位：mm/pixel
		double widthWorld = 0;          //标定板宽度(世界距离)
		double heightWorld = 0;         //标定板高度(世界距离)
	};


	struct AlgParamCreateProjTransMat
	{
		union
		{
			AlgParamCreateProjTransMatInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParamCreateProjTransMatDouble block;
			int data[PARAMS_MAXNUM];
		}d_params;
	};

	//Step3 算子参数描述
	struct AlgParaMsgCreateProjTransMat
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgCreateProjTransMat()
		{
			msgs.push_back("nEdgePolarity	$ Int      $Combox	    $0		$图像边缘属性                 $待检测的点沿着扫描线的方向灰度变化 ￥ALL ￥黑到白 ￥白到黑 $10");
			msgs.push_back("nThreshold   	$ Int      $I_Spinbox	$50		$二值化阈值                   $拟合直线的二值化阈值 $20");
			msgs.push_back("pixelDist       $Double    $D_Spinbox   $0.01   $像素精度(单位：mm/pixel)     $每个像素代表的实际距离，单位：mm/pixel $30");
			msgs.push_back("widthWorld      $Double    $D_Spinbox   $60     $标定板实际宽度(单位：mm)     $标定板实际的宽度 $40");
			msgs.push_back("heightWorld     $Double    $D_Spinbox   $60     $标定板实际高度(单位：mm)     $标定板实际的高度 $50");
		}
	};

	//Step4 异常返回值描述 
	struct AlgErrMsgCreateProjTransMat
	{
		std::vector<std::string> msgs;
		AlgErrMsgCreateProjTransMat()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入Roi异常不在当前图像内");
			msgs.push_back("2:ROI过小不符合规范");
			msgs.push_back("3:边缘点失败");
			msgs.push_back("4:拟合直线失败");
			msgs.push_back("5:拟合直线与ROI求交点失败");
			msgs.push_back("6:拟合直线与图片边缘求交点失败");
			msgs.push_back("7:拟合直线输入参数异常");
			msgs.push_back("8:输入ROI数量不等于4");
			msgs.push_back("9:输入图像为空");
			msgs.push_back("10:构造交点失败");
			msgs.push_back("11:投影变换后角点求取失败");
			msgs.push_back("12:投影变换转换失败");
			msgs.push_back("13:存储投影变换矩阵异常");
			msgs.push_back("14:算法未知异常");

		}
	};


	//! 接口

	namespace alg {
		//------------------------------------------创建投影变换矩阵  createProjectiveTransHomMat2D 20221115----------------------------------//
		//名称：  createProjectiveTransHomMat2D
		//功能：  创建并保存投影变换矩阵
		//0-输入参数 1-输出参数
		//返回值：0-正常
		_ALGLIBRARY_API int createProjTransMat(CreateProjTransMatInput& input, CreateProjTransMatOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_CREATEPROJTRANSMAT_H
