
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_SEGMENTVARIATION_H
#define ZKHYPROJECTHUANAN_SEGMENTVARIATION_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"

using namespace cv;
using namespace HalconCpp;
using namespace std;
namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//---------------------训练分割图片差异化模型 Start----------------//
	struct SegementVariationInput
	{
		HalconCpp::HObject   Himg;//模板图像
		HalconCpp::HObject Hregion;//粗定位mark的区域
		std::string  SegmentImgPath = "";  //创建差异化模型的图片文件夹。
		cv::String saveSegmentImgPath;    //保存模板图路径
		alglib::core::AlgrithmParams algPara;					//算法参数
		int nCol;				//miniled列号
		int nColCount;			//miniled总列号


	};
	struct  SegementVariationOutput
	{
		HalconCpp::HObject Maybemark;
	};

	struct IntParamsSegementVariation//int型算法参数，独立出来方便使用默认构造函数
	{
		int MinArea;//备选mark面积最小值
		int MaxArea;//备选mark面积最大值	
		int NW;//拆分列数
		int NH;//拆分列数

	};
	struct DoubleParamsSegementVariation//double型算法参数，独立出来方便使用默认构造函数
	{
		double  hv_angleStart;
		double  hv_angleExtent;
	};
	struct AlgParamSegementVariation
	{
		union
		{
			IntParamsSegementVariation block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsSegementVariation block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};


	struct AlgParaMsgSegementVariation
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgSegementVariation()
		{
			msgs.push_back("MinArea	$Int			$I_Spinbox	$8000  				$mark最小面积		$查找mark的最小面积，范围[1,999999999]	$10");
			msgs.push_back("MaxArea	$Int			$I_Spinbox	$20000  			$mark最大面积		$查找mark的最小面积，范围[1,999999999]	$20");
			msgs.push_back("NW	$Int			$I_Spinbox	$1  				$大图裁剪列数		$大图裁剪列数，范围[1,10]	$30");
			msgs.push_back("NH	$Int			$I_Spinbox	$3  			    $大图裁剪行数		$大图裁剪行数，范围[1,10]	$40");
		}
	};

	struct AlgErrMsgSegementVariation
	{
		std::vector<std::string> msgs;
		AlgErrMsgSegementVariation()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:");
			msgs.push_back("3:异常");
			msgs.push_back("4:异常");
			msgs.push_back("5:异常");
		}
	};
	//---------------------训练分割图片差异化模型 End----------------//

	//! 接口

	namespace alg {
		_ALGLIBRARY_API int segementVariation(const SegementVariationInput& input, SegementVariationOutput& output);
	}
	void NNinter_COPY_2(HalconCpp::HTuple hv_RCdata, HalconCpp::HTuple* hv_resultData, HalconCpp::HTuple* hv_resultIndex);
	void Position(HalconCpp::HObject ho_Src, HalconCpp::HObject ho_markRegion, HalconCpp::HObject ho_Partitioned, HalconCpp::HTuple hv_DictHandle,
		HalconCpp::HTuple hv_BMrow2, HalconCpp::HTuple hv_BMclo2, HalconCpp::HTuple hv_BDictHandle, HalconCpp::HTuple hv_BMregion,
		HalconCpp::HTuple hv_col1, HalconCpp::HTuple hv_row11, HalconCpp::HTuple hv_col1Indices, HalconCpp::HTuple hv_NW, HalconCpp::HTuple hv_NH, HalconCpp::HTuple disx,
		HalconCpp::HTupleVector/*{eTupleVector,Dim=1}*/* hvec_HomDZM, HalconCpp::HTupleVector/*{eTupleVector,Dim=1}*/* hvec_HomMZD,
		HalconCpp::HTuple* hv_HomMat2D2, HalconCpp::HTuple* hv_BTomMat2D);
	void  inter_getRegionRect(HObject ho_defectRegions, HTuple* hv_Rows, HTuple* hv_Columns, HTuple* hv_Len1s, HTuple* hv_Len2s, HTuple* hv_Areas, HTuple* hv_row1s,
		HTuple* hv_col1s, HTuple* hv_row2s, HTuple* hv_col2s, HTuple* hv_row3s, HTuple* hv_col3s,
		HTuple* hv_row4s, HTuple* hv_col4s);
}

#endif	// ZKHYPROJECTHUANAN_SEGMENTVARIATION_H
