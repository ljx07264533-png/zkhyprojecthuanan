
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_CAMCALIB_H
#define ZKHYPROJECTHUANAN_CAMCALIB_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//--------------------------------------------相机标定--------------------------------------------//
	//Step1算子input结构体、output结构体
	struct CamCalibInput
	{
		HalconCpp::HObject srcImg;                       //位姿参考图
		std::string imgsPath;                      //标定板图像文件夹路径
		SaveCalibFilesPath savePath;          //标定文件路径,用于保存两种校正方式Map图
		alglib::core::AlgrithmParams algPara;               //算法参数
	};
	struct CamCalibOutput
	{
		double calibError;                   //标定误差 用于显示到结果部分
		HalconCpp::HObject  lensDistortCalibMap;          //镜头畸变校正Map图
		HalconCpp::HObject  perspectTransMap;             //透视变换Map图
	};

	//Step2 算子参数 结构体定义 int和double分开写
	struct AlgParamCamCalibInt//int型算法参数，独立出来方便使用默认构造函数
	{
		int xMarkDotNum = 0;                 //每行黑色标志圆点的数量
		int yMarkDotNum = 0;                 //每列黑色标志圆点的数量
	};

	struct AlgParamCamCalibDouble//double型算法参数，独立出来方便使用默认构造函数
	{
		double calibPlateThick = 0.0;       //标定板厚度
		double pixelDist = 0.0;             //像素精度  单个像素代表的实际距离
		double focus = 0.0;                 //镜头焦距
		double sensorSize = 0.0;            //像元尺寸
		double distMarkDot = 0.0;           //两个就近黑色圆点中心之间的距离
		double diameterMarkDot = 0.0;       //黑色圆点直径
	};

	struct AlgParamCamCalib
	{
		union
		{
			AlgParamCamCalibInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParamCamCalibDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};

	//Step3 算子参数描述
	struct AlgParaMsgCamCalib
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgCamCalib()
		{
			msgs.push_back("xMarkDotNum	     $Int       $I_Spinbox	 $7 	 $标定板每行黑色标志圆点的数量   $默认值7，个数必须大于0 $10");
			msgs.push_back("yMarkDotNum	     $Int       $I_Spinbox	 $7  	 $标定板每列黑色标志圆点的数量   $默认值7，个数必须大于0 $20");
			msgs.push_back("diameterMarkDot  $Double    $D_Spinbox   $1      $黑色原点直径(单位：mm)         $黑色圆点直径,单位：mm $30");
			msgs.push_back("distMarkDot      $Double    $D_Spinbox   $2      $圆心之间距离（单位：mm）       $两个就近黑色圆点中心之间的距离,单位：mm $40");
			msgs.push_back("focus            $Double    $D_Spinbox   $6      $镜头焦距(单位：mm)             $镜头的实际焦距，默认值为6，单位mm $50");
			msgs.push_back("sensorSize       $Double    $D_Spinbox   $4.8    $相机像元尺寸(单位：um)         $相机像元尺寸，默认值4.8，单位：um $60");
			msgs.push_back("calibPlateThick  $Double    $D_Spinbox   $1      $标定板厚度(单位：mm)           $标定板厚度，默认值为1，单位mm $70");
			msgs.push_back("pixelDist        $Double    $D_Spinbox   $0.13   $像素精度(单位：mm/pixel)       $每个像素代表的实际距离，单位：mm/pixel $80");
		}
	};

	//Step4 异常返回值描述
	struct AlgErrMsgCamCalib
	{
		std::vector<std::string> msgs;
		AlgErrMsgCamCalib()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:批量读入标定板图像失败");
			msgs.push_back("2:标定初始化失败");
			msgs.push_back("3:标定失败");
			msgs.push_back("4:保存畸变校正Map图失败");
			msgs.push_back("5:输入图像为空");
			msgs.push_back("6:保存透视变换Map图失败");
			msgs.push_back("7:算法未知异常");
		}
	};


	//! 接口

	namespace alg {
		//------------------------------------------相机标定 camCalib 20221123----------------------------------//
		//名称：  camCalib
		//功能：  相机标定
		//0-输入参数 1-输出参数
		//返回值：0-正常
		_ALGLIBRARY_API int camCalib(CamCalibInput& input, CamCalibOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_CAMCALIB_H
