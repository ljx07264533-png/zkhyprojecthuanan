
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_WAFERIMAGEPROCESS_H
#define ZKHYPROJECTHUANAN_WAFERIMAGEPROCESS_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------晶圆处理图像算子 BEGIN-----------------//

	struct WaferImageProcessInfo
	{
		WaferImageProcessInfo() {
			Rotate.bEnable = false;
			Mirror.bEnable = false;
			ROIDomain.bEnable = false;
			ZoomSize.bEnable = false;
			CutSize.bEnable = false;
			ImageSave.bEnable = false;
		}

		int nChannel;
		int nRow;
		int nCol;
		std::string sImageName;
		struct Rotate
		{
			bool bEnable = false;
			double sAngle = 0;
		}Rotate;
		struct Mirror
		{
			bool bEnable = false;
			std::string sMirrorType = "Row";
		}Mirror;
		struct ROIDomain
		{
			bool bEnable = false;
			double dRow1 = 0;
			double dColumn1 = 0;
			double dRow2 = 500;
			double dColumn2 = 500;
		}ROIDomain;
		struct ZoomSize
		{
			bool bEnable = false;
			double dWidth = 0;
			double dHeight = 0;
		}ZoomSize;
		struct CutSize
		{
			bool bEnable = false;
			double dRow1 = 0;
			double dColumn1 = 0;
			double dRow2 = 500;
			double dColumn2 = 500;
		}CutSize;
		struct ImageSave
		{
			bool bEnable = false;
			std::string sPath;
		}ImageSave;

	};


	struct WaferImageProcessInput
	{
		cv::Mat srcImage;
		cv::Mat imageInfo;
		alglib::core::AlgrithmParams algPara;
		WaferImageProcessInfo waferImageProcessInfo;
	};
	struct WaferImageProcessOutput
	{
		cv::Mat mOutImage;
		HalconCpp::HObject hOutImage;
	};

	struct AlgParamWaferImageProcess
	{
		union
		{
			struct
			{
				int saveImageType;
				int saveImageSuffix;
				int compressScale;
			}block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			struct
			{
			}block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct   AlgParaMsgWaferImageProcess
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgWaferImageProcess()
		{
			msgs.push_back("saveType          $Int      $CheckBox	 $0  	 $存图方式	     $存图方式（opencv/halcon/gpu） 	$5");
			msgs.push_back("saveImageSuffix   $Int      $CheckBox	 $0  	 $存图后缀	     $存图后缀（jpg） 	$10");
			msgs.push_back("compressScale	   $Int      $CheckBox	 $0  	 $压缩比例	     $压缩比例（0-100） 	$15");
		}
	};

	struct AlgErrMsgWaferImageProcess
	{
		std::vector<std::string> msgs;
		AlgErrMsgWaferImageProcess()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:接口未知异常");
			msgs.push_back("2:输入图像异常");
			msgs.push_back("3:旋转图像未知异常");
			msgs.push_back("4:图像翻转格式异常");
			msgs.push_back("5:图像翻转未知异常");
			msgs.push_back("6:图像reduceDomain异常");
			msgs.push_back("7:图像Zoom未知异常");
			msgs.push_back("8:图像Cut未知异常");
			msgs.push_back("9:图像保存未知异常");
			msgs.push_back("10:假图跳过");
			msgs.push_back("11:未知异常");
		}
	};

	//---------------------晶圆处理图像算子 END-----------------//
	//! 接口

	namespace alg {
		
		//——————————————晶圆图像处理——————————————//
		//函数说明：通过解析运控的参数，对图像进行旋转、翻转、裁剪、存图等处理
		//WaferImageProcessInput &input;				            输入：待处理图、算法参数、运控的信息
		//WaferImageProcessOutput &output; 			            输出：处理后的图
		_ALGLIBRARY_API int waferImageProcess(WaferImageProcessInput& input, WaferImageProcessOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_WAFERIMAGEPROCESS_H
