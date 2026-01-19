
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_CREATTEXTURE_H
#define ZKHYPROJECTHUANAN_CREATTEXTURE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体
	struct CreatTextureInput
	{
		cv::Mat SrcImg;               //暗模板图
		HalconCpp::HObject region;          //  选择模板区域
		alglib::core::AlgrithmParams alg;
		std::vector <std::vector<cv::Point2f>>	posROIContour;  //创建匹配模板轮廓1
		std::vector < std::vector<cv::Point2f>>	posContour;    //检测区域多点集轮廓
		HalconCpp::HObject        hoRegion;     //模板检测区域
		alglib::core::RectROI		   roiRect;	    //纹理模板图轮廓
		SaveShapeModelPath1 modelImgsPath1;//模板图路径
		ShapeModelInfo1 shapeInfo;
	};

	//! 输出结构体
	struct CreatTextureOutput
	{
		ShapeModelInfo1 shapeInfo;
		HalconCpp::HObject ContoursAffineTrans1;   //显示创建轮廓1
		HalconCpp::HObject ContoursAffineTrans2;   //显示创建轮廓2
	};

	//! 错误信息结构体

	struct AlgParamCreatTextureDouble
	{
		double  angleStart = 0.5;			//形状匹配模型：样本可能的旋转起始角度
		double  angleExtend = 20.0;			//形状匹配模型：样本可能的旋转角度范围                      //是否输出Mat图    0 不输出Mat图 1 输出Mat图
		double  minScore = 0.4;				//形状匹配模型：匹配置信度。
		double scal = 0.5;					//缩放比例
		double contrstH = 255;				   //创建和查找时的高对比度阈值
		double contrstL = 100;                 //创建和查找时的低对比度阈值
		double contourNumber = 1;            //模板创建轮廓个数
		double  minArea = 0.5;				//最小面积
		double fusionDist = 20;				//最小融合距离
		double  threshold1 = 100;			//亮阈值
		double  threshold3 = 250;			//暗阈值
		double opening = 5;					//缺陷开运算大小
		double overTimeTh = 3000;			//超时设定

	};
	struct AlgParamCreatTextureInt
	{
		int	numMatches = 1;			//形状匹配模型：模型匹配个数
		int FindRoi = 500; 		    //基于创建ROI膨胀操作大小
		int StrucTure = 2;         //基于图像相减时的开运算大小。
		int NumLevels = 4;        //基于图像相减时的开运算大小。
	};
	struct AlgParamCreatTexture
	{

		union
		{
			AlgParamCreatTextureInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParamCreatTextureDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};

	//! 接口

	namespace alg {
		
		//-----------------------------creat_texture------------------------------------------//
		//函数说明：创建纹理模型算子
		//功能： 绘制创建模型ROI，检测ROI，模板图ROI。      
		//返回值： 0-正常
		_ALGLIBRARY_API int creatTexture(const  CreatTextureInput& input, CreatTextureOutput& output);
	}

	//-----------------------------------------------creatModle----------------------------//
	//功能：创建模板模型，为后续检测提供放射变换信息，模板图等信息。
	void creatModle(HalconCpp::HObject ho_Image, HalconCpp::HObject* ho_ImagePart, HalconCpp::HObject* ho_FindRegionROI, HalconCpp::HObject* ho_Rectangle,
		HalconCpp::HTuple contrstL, HalconCpp::HTuple  contrstH, HalconCpp::HTuple contourNumber, HalconCpp::HTuple hv_AngleStart, HalconCpp::HTuple hv_AngleExtend, HalconCpp::HTuple hv_offset, HalconCpp::HTuple hv_NumLevels,
		HalconCpp::HTuple hv_Row1, HalconCpp::HTuple hv_Column1, HalconCpp::HTuple hv_Row2, HalconCpp::HTuple hv_Column2, HalconCpp::HObject CreatRegions, HalconCpp::HTuple hv_MinScore,
		HalconCpp::HTuple hv_NumMatches, HalconCpp::HTuple hv_Scale, HalconCpp::HTuple hv_Dilate, HalconCpp::HTuple* hv_ModelID, HalconCpp::HTuple* row, HalconCpp::HTuple* col,
		HalconCpp::HTuple* hv_startRow, HalconCpp::HObject* ho_ContoursAffineTrans1, HalconCpp::HObject* ho_ContoursAffineTrans2);
}

#endif	// ZKHYPROJECTHUANAN_CREATTEXTURE_H
