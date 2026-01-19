#include "alglibrary/zkhyProjectHuaNan/areaFlatFieldCorrect.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;
using namespace zkhyPublicFuncHN;

namespace alglib::ops::zkhyProHN {
	void flatFildlAreaCorrect(HObject ho_Image, HObject ho_WeightImg, HObject ho_regionCorrect,
		HObject* ho_ImageResult, HTuple hv_lowGray, HTuple hv_highGray)
	{

		// Local iconic variables
		HObject  ho_Region, ho_EmptyRegion, ho_EmptyObj;
		HObject  ho_RegionDifference, ho_ImageReduced, ho_ImageReduced1;
		HObject  ho_ObjectsConcat;

		// Local control variables
		HTuple  hv_Width, hv_Height;

		//对实际的IC进行平场校正在255区域 不进行处理
		Threshold(ho_Image, &ho_Region, hv_lowGray, hv_highGray);
		GetImageSize(ho_Image, &hv_Width, &hv_Height);
		GenEmptyRegion(&ho_EmptyRegion);
		GenEmptyObj(&ho_EmptyObj);
		if (0 != (HTuple(ho_regionCorrect == ho_EmptyRegion).TupleOr(ho_regionCorrect == ho_EmptyObj)))
		{
			GetDomain(ho_Image, &ho_regionCorrect);
		}
		//待平场校正区域
		Threshold(ho_Image, &ho_Region, hv_lowGray, hv_highGray);
		//不进行平场校正区域 等于255
		Difference(ho_regionCorrect, ho_Region, &ho_RegionDifference);
		FillUp(ho_RegionDifference, &ho_RegionDifference);
		MultImage(ho_WeightImg, ho_Image, &(*ho_ImageResult), 0.01, 0);
		//原始不做平场校正的区域
		ReduceDomain(ho_Image, ho_RegionDifference, &ho_ImageReduced);
		//进行平场校正的区域
		ReduceDomain((*ho_ImageResult), ho_Region, &ho_ImageReduced1);
		ConcatObj(ho_ImageReduced1, ho_ImageReduced, &ho_ObjectsConcat);
		//将校正的区域和不校正的区域进行融合成一个图
		TileImagesOffset(ho_ObjectsConcat, &(*ho_ImageResult), (HTuple(0).Append(0)), (HTuple(0).Append(0)),
			(HTuple(-1).Append(-1)), (HTuple(-1).Append(-1)), (HTuple(-1).Append(-1)),
			(HTuple(-1).Append(-1)), hv_Width, hv_Height);
		return;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//——————————————面阵相机平场校正——————————————//
	//函数说明：通过平场校正的权重因子进行待测图的平场校正
	//SideAlignInput &input;				            输入：待测图像、校正ROI/校正区域、算法参数
	//SideAlignOutput &output; 			            输出：平场校正的图
	int areaFlatFieldCorrect(const AreaFlatFieldCorrInput& input, AreaFlatFieldCorrOutput& output)
	{
		AlgParaAreaFlatFieldCorr* alg = (AlgParaAreaFlatFieldCorr*)&input.algPara;
		AlgParaAreaFlatFieldCorrInt algPara = alg->i_params.block;
		int lowGray = algPara.lowGray;
		int highGray = algPara.highGray;
		int flagShowmat = algPara.flagTransMat;
		if (lowGray > highGray || lowGray > 255) {
			return 1;
		}
		// 构造校正区域
		try {
			HObject region;
			vector<vector<Point2f>> inputRoi = input.inputRoi;
			genDefectRegion(input.srcImg, input.regionROI, inputRoi, &region);
			flatFildlAreaCorrect(input.srcImg, input.weightImg, region, &output.srcImg, lowGray, highGray);
			if (flagShowmat) {
				output.showImg = HObject2Mat(output.srcImg).clone();
			}
		}
		catch (...)
		{
			return 2;
		}
		return 0;
	}

}
