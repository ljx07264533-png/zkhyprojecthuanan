#include "alglibrary/zkhyProjectHuaNan/GenPackagingTemplate.h"
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

namespace alglib::ops::zkhyProHN {
	//-----------------------------realGenPackagingTemplate------------------------------------------//
	//函数说明：根据DXF文件生成标准的BGA锡球模板排列
	int realGenPackagingTemplate(const GenPackagingTemplateInput& input, GenPackagingTemplateOutput& output)
	{
		/*-----------------------------解析参数包----------------------------*/
		HObject hSrcImg = input.srcImg;
		string filePath = input.filePath;
		AlgParamGenPackagingTemplate* pParams = (AlgParamGenPackagingTemplate*)&input.algPara;
		int iPackagingType = pParams->i_params.block.iPackagingType;
		double dTolerance = pParams->d_params.block.dTolerance;
		double dMmPrePixel = pParams->d_params.block.dMmPrePixel;
		DWORD timeOutThreshold = (DWORD)abs(pParams->d_params.block.dTimeOutThreshold);

		// 参数合法性判断
		// 需要先判断对象是否初始化，再判断是否为空对象，否则判断对象为空时会产生异常
		if (hSrcImg.IsInitialized() == false)
			return 1;           // 输入图像为空，图像未初始化
		HObject hEmpty;
		HTuple hIsEuqal;
		GenEmptyObj(&hEmpty);
		TestEqualObj(hSrcImg, hEmpty, &hIsEuqal);
		if (hIsEuqal.I() == 1)
		{
			return 1;           // 输入图像为空
		}
		string fileExt = filePath.substr(filePath.find_last_of('.') + 1);
		if (fileExt != "dxf")
			return 2;           // 文件格式错误
		if (dMmPrePixel <= 0)
			return 3;           // 像素当量输入错误
		if (timeOutThreshold > 999999 || timeOutThreshold <= 0)
			return 4;           // 算法超时参数输入有误

		// 开始计时
		DWORD startTime = GetTickCount64();

		// Step 1，读取dxf文件，读取后每个像素代表1mm
		HObject hContourDxf;
		HTuple hDxfStatus;
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		ReadContourXldDxf(&hContourDxf, filePath.c_str(), HTuple(), HTuple(), &hDxfStatus);

		TestEqualObj(hContourDxf, hEmpty, &hIsEuqal);
		if (hIsEuqal.I() == 1)
		{
			return 5;           // 读取dxf文件失败
		}

		// Step 2，计算偏移量，读取的坐标可能为负数
		HObject hContourDxfUnion;
		HTuple hOriArea, hOriRow, hOriCol, hOriPointOrder, hImgWidth, hImgHigh;
		double offsetRow, offsetCol;
		UnionAdjacentContoursXld(hContourDxf, &hContourDxfUnion, 5, 1, "attr_keep");        // 合并临近的XLD，使细小的线段连起来
		AreaCenterXld(hContourDxfUnion, &hOriArea, &hOriRow, &hOriCol, &hOriPointOrder);
		offsetRow = -hOriRow.TupleMin();    // 计算坐标偏移量
		offsetCol = -hOriCol.TupleMin();
		GetImageSize(hSrcImg, &hImgWidth, &hImgHigh);
		double edgeDist = 200.0;           // 底部Pin脚或锡球区域外扩大小
		double zoomFactor = 1.0 / dMmPrePixel;

		// Step 3, 使用计算得到的偏移距离对轮廓点位置进行修正
		HObject hTargetContour;
		HTuple hHomMat2dIdentity, hHomMat2dTranslate;
		HomMat2dIdentity(&hHomMat2dIdentity);
		HomMat2dTranslate(hHomMat2dIdentity, offsetRow + edgeDist, offsetCol + edgeDist, &hHomMat2dTranslate);
		AffineTransContourXld(hContourDxfUnion, &hTargetContour, hHomMat2dTranslate);

		// Step 4, 生成区域，并根据缩放系数进行缩放，计算缩放后的轮廓

		HObject hTargetShape, hTargetRegion, hTargetRegionZoom, hTargetRegionMirror, hTemplateRegion, hTemplateContour, hContourTmp;
		HTuple hRow, hCol, hRadius, hStartPi, hEndPi, hPointOrder;
		ShapeTransXld(hTargetContour, &hTargetShape, "outer_circle");
		GenRegionContourXld(hTargetShape, &hTargetRegion, "filled");
		ZoomRegion(hTargetRegion, &hTargetRegionZoom, zoomFactor, zoomFactor);
		MirrorRegion(hTargetRegionZoom, &hTargetRegionMirror, "row", hImgHigh);

		if (iPackagingType == 0)   // BGA封装
		{
			GenContourRegionXld(hTargetRegionMirror, &hContourTmp, "border");
			FitCircleContourXld(hContourTmp, "algebraic", -1, 0, 0, 3, 2, &hRow, &hCol, &hRadius, &hStartPi, &hEndPi, &hPointOrder);

			GenCircleContourXld(&hTemplateContour, hRow, hCol, hRadius, hStartPi, hEndPi, "positive", 1);
			GenCircle(&hTemplateRegion, hRow, hCol, hRadius);
		}
		else {                    // QFN封装
			hTemplateRegion = hTargetRegionMirror;
		}
		// Step 5，生成模板图，输出模板图和模板轮廓
		HObject hTemplateImg, hTemplateRegions, hRegionSelect, hRegionTmp;
		HTuple hRegionNum;
		vector<vector<Point2f>> templateContour;
		Connection(hTemplateRegion, &hTemplateRegions);
		CountObj(hTemplateRegions, &hRegionNum);
		for (int i = 1; i <= hRegionNum; i++)
		{
			HTuple hRowTmp, hColTmp;
			vector<Point2f> regionTmp;
			SelectObj(hTemplateRegions, &hRegionSelect, i);
			if (dTolerance == 0)
				GetRegionContour(hRegionSelect, &hRowTmp, &hColTmp);
			else
				GetRegionPolygon(hRegionSelect, dTolerance, &hRowTmp, &hColTmp);
			zkhyPublicFuncHN::hTuple2Points(hColTmp, hRowTmp, regionTmp);
			templateContour.push_back(regionTmp);
		}
		RegionToBin(hTemplateRegion, &hTemplateImg, 255, 0, hImgWidth, hImgHigh);
		cv::Mat templateImg = HObject2Mat(hTemplateImg);
		output.templateImg = templateImg;
		output.templateRegion = hTemplateRegions;
		output.templateContour = templateContour;

		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 6;                   // 算法超时

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//-----------------------------GenPackagingTemplate------------------------------------------//
	//功能：封测底部2D模板轮廓及图像生成函数入口
	int GenPackagingTemplate(const GenPackagingTemplateInput& input, GenPackagingTemplateOutput& output)
	{
		try
		{
			return realGenPackagingTemplate(input, output);
		}
		catch (...)
		{
			return 7; //未知错误
		}
	}
}
