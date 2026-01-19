#include "alglibrary/zkhyProjectHuaNan/emptyCheckerBlob.h"
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
	//功能：计算输入区域的面积、角度、外接矩形以及轮廓等基本特征数据
	//输入：defectRegion 缺陷区域，可以为单个区域，也可以为多个区域
	//		useContour 是否计算缺陷轮廓，true为计算轮廓，false为不计算轮廓，默认为计算
	//		useRotateRect 计算区域的外接旋转矩形还是正矩形，true为旋转矩形，false为正矩形，默认为旋转矩形
	//		homMat2D 坐标变换矩阵，用于对缺陷区域进行映射，默认为空
	//输出：outFlaw 缺陷结构体
	void defectRegion2Flaw(HObject& defectRegion, FlawInfoStruct& outFlaw, const bool& useContour = true, const bool& useRotateRect = true, const HTuple& homMat2D = HTuple());

	int realEmptyCheckerBlob(const EmptyCheckerBlobInput& input, EmptyCheckerBlobOutput& output)
	{

		//Step1 参数赋值
		if (input.srcImage.IsInitialized() == 0)
			return 1;
		AlgParaEmptyCheckerBlob* algBlob = (AlgParaEmptyCheckerBlob*)&input.algPara;
		HObject region;
		GenEmptyRegion(&region);
		if (input.detectROI.size() > 0) {
			HTuple hv_Rows = HTuple();
			HTuple hv_Cols = HTuple();
			for (int i = 0; i < input.detectROI.size(); i++) {
				hv_Rows[i] = input.detectROI[i].y;
				hv_Cols[i] = input.detectROI[i].x;
			}
			GenRegionPolygonFilled(&region, hv_Rows, hv_Cols);
		}
		else {
			if (input.ho_region.IsInitialized())
				region = input.ho_region;
		}
		HObject srcImg;
		ReduceDomain(input.srcImage, region, &srcImg);
		int lowGray = algBlob->i_params.block.lowThreshold;
		int highGray = algBlob->i_params.block.highThreshold;
		int flagOutputShowImg = algBlob->i_params.block.flagOutputShowImg;
		int minArea = algBlob->i_params.block.minblobArea;
		//Step2 图像二值化获取blob
		HObject ho_blobs, ho_ConnectedRegions, ho_SelectedRegions, ho_mask, ho_ObjectSelectedj, ho_blobRes;
		HTuple hv_Number, width, height, hv_area, hv_centerRow, hv_centerCol, hv_totalarea;
		Threshold(srcImg, &ho_blobs, lowGray, highGray);
		GetImageSize(input.srcImage, &width, &height);
		GenEmptyObj(&ho_mask);
		GenEmptyObj(&ho_SelectedRegions);
		if (flagOutputShowImg == 1) {
			RegionToBin(ho_blobs, &ho_mask, 255, 0, width, height);
			output.detectImg = HObject2Mat(ho_mask);
		}
		Connection(ho_blobs, &ho_ConnectedRegions);
		//筛选出满足面积阈值的blob
		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", minArea, 9999999999);
		CountObj(ho_SelectedRegions, &hv_Number);
		int numberBolb = hv_Number.I();
		//区域合并
		Union1(ho_SelectedRegions, &ho_blobRes);
		FlawInfoStruct flawinfo;
		flawinfo.extendParameters.d_params.block.emptyCheckerBlob = numberBolb;

		defectRegion2Flaw(ho_blobRes, flawinfo);
		output.flaws.push_back(flawinfo);
		return 0;
	}


	void defectRegion2Flaw(HObject& defectRegion, FlawInfoStruct& outFlaw, const bool& useContour, const bool& useRotateRect, const HTuple& homMat2D)
	{
		double areaTmp, angleTmp, flawWidth, flawHeight;
		Point2f centerPoint;
		vector<Point2f> rectCountour;
		vector<vector<Point2f>> defectContours;
		genFlawInfoFromRegion(defectRegion, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectCountour, defectContours, false, true);
		outFlaw.FlawBasicInfo.flawArea = areaTmp;
		outFlaw.FlawBasicInfo.angle = angleTmp;
		outFlaw.FlawBasicInfo.flawWidth = flawWidth;
		outFlaw.FlawBasicInfo.flawHeight = flawHeight;
		outFlaw.FlawBasicInfo.centerPt = centerPoint;
		outFlaw.FlawBasicInfo.pts = rectCountour;
		outFlaw.FlawBasicInfo.flawContours = defectContours;
		outFlaw.FlawBasicInfo.type = 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//-----------------------------emptyCheckerBlob------------------------------------------//
	//函数说明：判断产品有无
	//EmptyCheckerBlobInput &input;				输入：
	//EmptyCheckerBlobInput &output; 			输出：物料中的blob个数
	int emptyCheckerBlob(const EmptyCheckerBlobInput& input, EmptyCheckerBlobOutput& output)
	{
		try
		{
			return realEmptyCheckerBlob(input, output);
		}
		catch (...)
		{
			return 2; //算法未知异常
		}
	}
}
