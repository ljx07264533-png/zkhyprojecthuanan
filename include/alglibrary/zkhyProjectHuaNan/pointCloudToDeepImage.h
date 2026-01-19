
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_POINTCLOUDTODEEPIMAGE_H
#define ZKHYPROJECTHUANAN_POINTCLOUDTODEEPIMAGE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------点云转深度图------------------------------------------//
	struct PointCloudToDeepImageInput
	{
		HalconCpp::HTuple pointCloudX;
		HalconCpp::HTuple pointCloudY;
		HalconCpp::HTuple pointCloudZ;
		std::vector<cv::Point3f> pointCloud;            //原始点云数据
		alglib::core::AlgrithmParams algPara;					   //算法参数
	};
	struct PointCloudToDeepImageOutput
	{
		HalconCpp::HObject ho_deepImage;                      //halcon深度图（32位）
		cv::Mat mat_deepImage;                         //Mat深度图（32位）
		cv::Mat mat_showImage;                         //Mat显示图（8位）
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct IntParamsPointCloudToDeepImage//int型算法参数，独立出来方便使用默认构造函数
	{
		int packagingRows_num = 2;              //封装行数
		int packagingCols_num = 8;              //封装列数
		int segProductPointsNum_min = 5000;     //分割产品点云最小个数
		int smooth3DModel_factor = 400;         //平滑点云系数（最近邻k的数量）
		int imageOutputMat = 0;                //是否输出mat显示图  0-不输出Mat图, 1-输出Mat图
	};
	struct DoubleParamsPointCloudToDeepImage//int型算法参数，独立出来方便使用默认构造函数
	{
		double segBackground_z_min = -700.0;     //剔除背景最小高度，单位mm
		double segProduct_threshold = 1.0;       //分割产品距离阈值，单位mm
		double twoPointDist_x = 0.0125;          //线激光x方向两点间距，单位mm
		double twoPointDist_y = 0.02;            //线激光y方向两点间距，单位mm
		double packagingSize_W = 4.0;            //封装尺寸宽，单位mm
		double packagingSize_H = 4.0;            //封装尺寸高，单位mm
		double tray_x_distance = 10.0;           //tray盘x方向间距（即吸嘴x间距），单位mm
		double tray_y_distance = 10.0;           //tray盘x方向间距（即吸嘴y间距），单位mm
		double begin_x_offset = 5.0;             //3D相机开始扫描位置（0，0）点到料盘或吸嘴第一个位置的x向距离，单位mm。扫描方向是从y=0开始
		double begin_y_offset = 5.0;             //3D相机开始扫描位置（0，0）点到料盘或吸嘴第一个位置的y向距离，单位mm。扫描方向是从y=0开始
		double segFitPlane_z_min = -0.3;       //分割底座最小高度，用于拟合平面，单位mm
		double segFitPlane_z_max = -0.2;       //分割底座最大高度，用于拟合平面，单位mm
		double ballHeight_standard = 0.1;      //球高标准尺寸，单位mm
		double overtimeTh = 3000;              //算法超时，单位ms
	};
	struct AlgParamPointCloudToDeepImage
	{
		union
		{
			IntParamsPointCloudToDeepImage block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsPointCloudToDeepImage block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgPointCloudToDeepImage
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgPointCloudToDeepImage()
		{
			msgs.push_back("packagingRows_num	        $Int    $I_Spinbox	$2  	  $来料产品行数  	                  $默认值2，单位：无，必须大于0。$10");
			msgs.push_back("packagingCols_num	        $Int    $I_Spinbox	$8  	  $来料产品列数  	                  $默认值8，单位：无，必须大于0。$20");
			msgs.push_back("segProductPointsNum_min	    $Int    $I_Spinbox	$5000  	  $产品点云最小个数  	              $默认值5000，单位：无，必须大于0。$30");
			//msgs.push_back("segProductPointsNum_max	$Int    $I_Spinbox	$99999999 $产品点云最大个数  	              $默认值99999999，单位：无，必须大于0。");
			msgs.push_back("segBackground_z_min	        $Double $D_SpinBox	$-700.0   $剔除背景最小高度(单位:mm)  	      $默认值-700，单位：mm。产品与背景分开的高度值 $40");
			//msgs.push_back("segBackground_z_max	    $Double $D_SpinBox	$0.0  	  $剔除背景最大高度  	              $默认值0.0，单位：mm。产品与背景分开的高度值");
			msgs.push_back("segProduct_threshold	    $Double $D_SpinBox	$1.0      $分割产品距离阈值(单位:mm)  	      $默认值1.0，单位：mm，必须大于0。两个产品分离开的最小距离阈值，小于或等于参数值的为一个区域 $50");
			msgs.push_back("twoPointDist_x	            $Double $D_SpinBox	$0.0125   $x方向两点间距(单位:mm)  	          $默认值0.0125，单位：mm，必须大于0。线激光x方向两点间距 $60");
			msgs.push_back("twoPointDist_y	            $Double $D_SpinBox	$0.02     $y方向两点间距(单位:mm)  	          $默认值0.02，单位：mm，必须大于0。线激光y方向两点间距 $70");
			msgs.push_back("packagingSize_W	            $Double $D_SpinBox	$4.0      $封装尺寸宽(单位:mm)  	          $默认值4.0，单位：mm，必须大于0。一个产品的外观尺寸宽度 $80");
			msgs.push_back("packagingSize_H	            $Double $D_SpinBox	$4.0      $封装尺寸高(单位:mm)  	          $默认值4.0，单位：mm，必须大于0。一个产品的外观尺寸高度 $90");
			msgs.push_back("tray_x_distance	            $Double $D_SpinBox	$10.0     $tray盘x方向间距(单位:mm)  	      $默认值10.0，单位：mm，必须大于0。tray盘x方向间距（即吸嘴x间距）$100");
			msgs.push_back("tray_y_distance	            $Double $D_SpinBox	$10.0     $tray盘y方向间距(单位:mm)  	      $默认值10.0，单位：mm，必须大于0。tray盘y方向间距（即吸嘴y间距）$110");
			msgs.push_back("begin_x_offset	            $Double $D_SpinBox	$5.0      $扫描预留x方向间距(单位:mm)  	      $默认值5.0，单位：mm，必须大于0。预留3D相机开始扫描初始点(0,0)到第一个产品的x方向间距 $120");
			msgs.push_back("begin_y_offset	            $Double $D_SpinBox	$5.0      $扫描预留y方向间距(单位:mm)   	  $默认值5.0，单位：mm，必须大于0。预留3D相机开始扫描初始点(0,0)到第一个产品的y方向间距 $130");
			msgs.push_back("segFitPlane_z_min	        $Double $D_SpinBox	$-0.3     $分割底座最小高度(单位:mm) 	      $默认值-0.3，单位：mm。分割底座与锡球点云的最小高度 $140");
			msgs.push_back("segFitPlane_z_max	        $Double $D_SpinBox	$-0.2     $分割底座最大高度(单位:mm)  	      $默认值-0.2，单位：mm。分割底座与锡球点云的最大高度 $150");
			msgs.push_back("ballHeight_standard	        $Double $D_SpinBox	$0.1      $球高标准尺寸(单位:mm)  	          $默认值0.1，单位：mm，必须大于0。BGA锡球标准高度参考 $160");
			msgs.push_back("smooth3DModel_factor	    $Int    $I_Spinbox	$400      $平滑点云系数  	                  $默认值400，单位：无，必须大于0。平滑点云系数（最近邻k的数量）$170");
			msgs.push_back("imageOutputMat        	    $Int    $CheckBox	$0  	  $是否输出Mat显示图   	              $默认值0。默认不输出Mat显示图。0-不输出Mat显示图，1-输出Mat显示图 $180");
			msgs.push_back("overtimeTh		            $Double $D_SpinBox $3000 	  $算法超时设定(单位：毫秒)	          $默认值3000，单位：ms，必须大于0。当算法超过次设定时间时，返回异常 $190");
		}
	};

	//Step5 异常返回值描述 
	struct AlgErrMsgPointCloudToDeepImage
	{
		std::vector<std::string> msgs;
		AlgErrMsgPointCloudToDeepImage()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入点云数据为空");
			msgs.push_back("2:剔除背景点云异常");
			msgs.push_back("3:分割产品点云数量异常");
			msgs.push_back("4:分割产品底座点云异常");
			msgs.push_back("5:算法未知异常");
			msgs.push_back("6:输入参数错误，必须大于零");
			msgs.push_back("7:输入参数错误，分割底座最小高度<最大高度");
			msgs.push_back("8:算法超时");
		}
	};


	//! 接口

	namespace alg {
		
		//-----------------------------pointCloudToDeepImage------------------------------------------//
		//函数说明：点云转深度图
		//PointCloudToDeepImageInput &input;				输入：点云原始数据、算法参数
		//PointCloudToDeepImageOutput &output:				输出：深度图和显示图
		_ALGLIBRARY_API int pointCloudToDeepImage(PointCloudToDeepImageInput& input, PointCloudToDeepImageOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_POINTCLOUDTODEEPIMAGE_H
