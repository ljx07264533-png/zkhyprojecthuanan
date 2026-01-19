
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_INSPECTTEXTURE_H
#define ZKHYPROJECTHUANAN_INSPECTTEXTURE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

		//----------------------------------------------基于模板图查找异常的缺陷检测-------------------------------------//
	struct InspectTextureInput
	{
		cv::Mat srcImg;                  //待测图,需要指定基准图，将该图用于创建模型。
		std::vector<std::vector<cv::Point2f>>  posFindROI; //查找模板2 匹配区域。
		ShapeModelInfo1 shapeInfo;      //在编写xml阶段，直接赋值为“创建形状匹配模型算子”的输出，在软件启动之前，赋值为“加载形状匹配模型算子”的输出
		alglib::core::RectROI		   roiRect2;	   //模板图矩形轮廓
		alglib::core::AlgrithmParams alg;           //算法参数
		HalconCpp::HObject          cregions2;//整图检测区域
	};
	struct InspectTextureOutput
	{
		HalconCpp::HObject defectRegion;          //缺陷H区域
		std::vector<alglib::core::FlawInfoStruct>	flawinfos;		//缺陷所有信息
		HalconCpp::HObject SFindtRegionROI;//显示查找mark区域
		HalconCpp::HObject  SContour;     //显示匹配轮廓
		HalconCpp::HObject DisDImageModel;//显示亮模板图
		HalconCpp::HObject DisBImageModel;//显示暗模板图
	};


	//! 接口

	namespace alg {
		
		//---------------------------------------------缺陷检测算子inspect_texture ----------------------------------//
		//名称：inspect_texture
		//功能： 加载模板图，绘制匹配ROI区域。模板图与待测图相减获取异常区域。
		//返回值：  0-正常
		_ALGLIBRARY_API int inspectTexture(const InspectTextureInput& input, InspectTextureOutput& output);
	}
	//-----------------------------------------------inspectDefect----------------------------//
	//功能：对图像进行周期性模板比对检测。获取缺陷区域以及缺陷信息。
	int inspectDefect(HalconCpp::HObject ho_Image, HalconCpp::HObject BmageModel, HalconCpp::HObject DmageModel, HalconCpp::HObject ho_InsRegion1, HalconCpp::HObject Cregions2,
		HalconCpp::HObject* ho_DefectRegion, HalconCpp::HTuple hv_AngleStart, HalconCpp::HTuple hv_AngleExtend, HalconCpp::HTuple hv_FusionDist,
		HalconCpp::HObject ho_FindtRegionROI, HalconCpp::HTuple hv_startRow, HalconCpp::HTuple hv_structure, HalconCpp::HTuple hv_NumLevels,
		HalconCpp::HTuple hv_ModelID1, HalconCpp::HTuple hv_modelRow, HalconCpp::HTuple hv_modelColumn,
		HalconCpp::HTuple hv_MinScore, HalconCpp::HTuple hv_NumMatches, HalconCpp::HTuple hv_MinArea, HalconCpp::HTuple hv_threshold1, HalconCpp::HTuple hv_threshold2,
		HalconCpp::HTuple hv_threshold3, HalconCpp::HTuple hv_threshold4, HalconCpp::HTuple hv_opening, HalconCpp::HTuple hv_Scale, HalconCpp::HTuple hv_overtimeTh,
		HalconCpp::HTuple with, HalconCpp::HTuple* hv_DefectContoursRow, HalconCpp::HTuple* hv_DefectContoursColumn, HalconCpp::HTuple* hv_Rows,
		HalconCpp::HTuple* hv_Columns, HalconCpp::HTuple* hv_Len1s, HalconCpp::HTuple* hv_Len2s, HalconCpp::HTuple* hv_Areas, HalconCpp::HTuple* hv_row1s,
		HalconCpp::HTuple* hv_col1s, HalconCpp::HTuple* hv_row2s, HalconCpp::HTuple* hv_col2s, HalconCpp::HTuple* hv_row3s, HalconCpp::HTuple* hv_col3s,
		HalconCpp::HTuple* hv_row4s, HalconCpp::HTuple* hv_col4s, HalconCpp::HTuple* hv_overtimeFlag, HalconCpp::HObject* DisBImageModel, HalconCpp::HObject* DisDImageModel);
}

#endif	// ZKHYPROJECTHUANAN_INSPECTTEXTURE_H
