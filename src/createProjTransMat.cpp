#include "alglibrary/zkhyProjectHuaNan/createProjTransMat.h"
#include "alglibrary/alglibLocation.h"
#include "alglibrary/alglibMisc.h"
#include "alglibrary/alglibCalibration.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::location;
using namespace alglib::ops::location::alg;
using namespace alglib::ops::calibration;
using namespace alglib::ops::calibration::alg;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {
	int realCreateProjTransMat(CreateProjTransMatInput& input, CreateProjTransMatOutput& output)
	{
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		AlgParamCreateProjTransMat* algPara = (AlgParamCreateProjTransMat*)&input.algPara;

		HTuple hv_pixelDist = algPara->d_params.block.pixelDist;
		HTuple hv_widthWorld = algPara->d_params.block.widthWorld;
		HTuple hv_heightWorld = algPara->d_params.block.heightWorld;
		//判断参数设置合理性
		if (hv_pixelDist.D() <= 0)
		{
			hv_pixelDist = 0.01;//初始值
		}
		if (hv_widthWorld.D() <= 0)
		{
			hv_widthWorld = 60;//初始值
		}
		if (hv_heightWorld.D() <= 0)
		{
			hv_heightWorld = 60;//初始值
		}

		//判断输入的ROI个数是否等于4
		int rectangleROISize = input.vRectangleROI.size();
		if (rectangleROISize != 4)
			return 8;

		AlgParamCreateProjTransMatDouble algParamCreateProjTransMatDouble = algPara->d_params.block;
		AlgParamCreateProjTransMatInt algParamCreateProjTransMatInt = algPara->i_params.block;
		//高精度拟合直线参数定义
		GatherLineHighPrecisionInput gatherLineHighPrecisionInput;
		GatherLineHighPrecisionOutput gatherLineHighPrecisionOutput;
		AlgParaGatherLineHighPrecision* algNew = (AlgParaGatherLineHighPrecision*)&gatherLineHighPrecisionInput.algPara;
		if (algParamCreateProjTransMatInt.nThreshold <= 0 || algParamCreateProjTransMatInt.nThreshold > 255)
		{
			algParamCreateProjTransMatInt.nThreshold = 50;
		}
		//int类型算法参数赋值
		algNew->i_params.block.isCalib = 0;
		algNew->i_params.block.isPreprocessing = 1;
		algNew->i_params.block.nCoarseStep = 5;
		algNew->i_params.block.nEdgePolarity = algParamCreateProjTransMatInt.nEdgePolarity;
		algNew->i_params.block.nEssenceStep = 1;
		algNew->i_params.block.nFitLinePara = 1;
		algNew->i_params.block.nGetEdgeMode = 0;
		algNew->i_params.block.nMinValue = 12;
		algNew->i_params.block.nOffset = 10;
		algNew->i_params.block.nPointCompoundSentence = 1;
		algNew->i_params.block.nThreshold = algParamCreateProjTransMatInt.nThreshold;//高精度拟合直线二值化阈值，≥nThreshold置为白色，＜nThreshold置为黑色
		//double类型算法参数赋值
		algNew->d_params.block.dMultiple = 2.0;
		algNew->d_params.block.dRansacDis = 2.0;
		algNew->d_params.block.dValueDif = 10;

		int imgWidth, imgHeight;
		HObject ho_SrcImg;
		if (input.msrcImg.empty())
		{
			return 9;
		}
		else
		{
			ho_SrcImg = Mat2HObject(input.msrcImg);
			gatherLineHighPrecisionInput.img = input.msrcImg;
			imgWidth = input.msrcImg.cols;
			imgHeight = input.msrcImg.rows;
		}

		//对于输入的4个ROI进行排序
		//按照首先行坐标排序
		vector<int> R, RTemp, RIndex1;
		R.clear();
		RTemp.clear();
		R.push_back(input.vRectangleROI[0].pt1.y);
		R.push_back(input.vRectangleROI[1].pt1.y);
		R.push_back(input.vRectangleROI[2].pt1.y);
		R.push_back(input.vRectangleROI[3].pt1.y);
		RTemp = R;
		//升序排列
		sort(R.begin(), R.end());
		vector<int>::iterator iter;
		for (int i = 0; i < R.size(); i++)
		{
			iter = find(RTemp.begin(), RTemp.end(), R[i]);
			RIndex1.push_back(iter - RTemp.begin());
		}
		vector<RectangleROI>  vRectangleROIFinal;
		vRectangleROIFinal.clear();
		vRectangleROIFinal.push_back(input.vRectangleROI[RIndex1[0]]);
		vRectangleROIFinal.push_back(input.vRectangleROI[RIndex1[1]]);
		vRectangleROIFinal.push_back(input.vRectangleROI[RIndex1[2]]);
		vRectangleROIFinal.push_back(input.vRectangleROI[RIndex1[3]]);
		swap(vRectangleROIFinal[2], vRectangleROIFinal[3]);

		//按列再排序
		if (vRectangleROIFinal[1].pt1.x > vRectangleROIFinal[3].pt1.x)
		{
			swap(vRectangleROIFinal[1], vRectangleROIFinal[3]);
		}

		//按照顺序拟合直线-上-左-下-右 
		vector<LineStruct> vLineStruct;
		vLineStruct.clear();
		int gatherLineHighPrecisionFlag = 0;
		for (int j = 0; j < 4; j++)
		{
			gatherLineHighPrecisionInput.rectangleROI = vRectangleROIFinal[j];
			gatherLineHighPrecision(gatherLineHighPrecisionInput, gatherLineHighPrecisionOutput);
			if (gatherLineHighPrecisionFlag > 0)
			{
				return gatherLineHighPrecisionFlag;
				break;
			}
			vLineStruct.push_back(gatherLineHighPrecisionOutput.linePic);
		}

		//按照顺序求角点
		ConstructCrossPointInput constructCrossPointInput;
		int ConstructCrossPointInputFlag = 0;
		Point2f crossPoint2f;
		vector<Point2f> vCrossPoint2f;
		//左上角点
		vCrossPoint2f.clear();
		constructCrossPointInput.line1 = vLineStruct[0];
		constructCrossPointInput.line2 = vLineStruct[1];
		ConstructCrossPointInputFlag = constructCrossPoint(constructCrossPointInput, crossPoint2f);
		if (ConstructCrossPointInputFlag != 0)
		{
			return 10;
		}
		vCrossPoint2f.push_back(crossPoint2f);
		//左下角点
		constructCrossPointInput.line1 = vLineStruct[1];
		constructCrossPointInput.line2 = vLineStruct[2];
		ConstructCrossPointInputFlag = constructCrossPoint(constructCrossPointInput, crossPoint2f);
		if (ConstructCrossPointInputFlag != 0)
		{
			return 10;
		}
		vCrossPoint2f.push_back(crossPoint2f);
		//右下角点
		constructCrossPointInput.line1 = vLineStruct[2];
		constructCrossPointInput.line2 = vLineStruct[3];
		ConstructCrossPointInputFlag = constructCrossPoint(constructCrossPointInput, crossPoint2f);
		if (ConstructCrossPointInputFlag != 0)
		{
			return 10;
		}
		vCrossPoint2f.push_back(crossPoint2f);
		//右上角点
		constructCrossPointInput.line1 = vLineStruct[0];
		constructCrossPointInput.line2 = vLineStruct[3];
		ConstructCrossPointInputFlag = constructCrossPoint(constructCrossPointInput, crossPoint2f);
		if (ConstructCrossPointInputFlag != 0)
		{
			return 10;
		}
		vCrossPoint2f.push_back(crossPoint2f);
		output.vCornerPoints.clear();
		output.vCornerPoints = vCrossPoint2f;

		//#define DEBUG
		//#ifdef DEBUG
		//	hv_heightWorld = 30;
		//	hv_widthWorld = 30;
		//	hv_pixelDist = 0.1;
		//	HObject testImg, testImgTrans;
		//	ReadImage(&testImg, "E:/工程代码/workCodeEdit/zkhyProject/scratch_calib_01.png");
		//#endif 

			//变换前4个角点坐标
		HTuple hv_RCorners, hv_CCorners;
		hv_RCorners.Clear();
		hv_RCorners[0] = vCrossPoint2f[0].y;
		hv_RCorners[1] = vCrossPoint2f[1].y;
		hv_RCorners[2] = vCrossPoint2f[2].y;
		hv_RCorners[3] = vCrossPoint2f[3].y;

		hv_CCorners.Clear();
		hv_CCorners[0] = vCrossPoint2f[0].x;
		hv_CCorners[1] = vCrossPoint2f[1].x;
		hv_CCorners[2] = vCrossPoint2f[2].x;
		hv_CCorners[3] = vCrossPoint2f[3].x;

		//求标定板上边缘和左边缘像素长度
		HTuple hv_DistanceTop, hv_DistanceLeft;
		DistancePp(hv_RCorners[0], hv_CCorners[0], hv_RCorners[3], hv_CCorners[3], &hv_DistanceTop);
		DistancePp(hv_RCorners[0], hv_CCorners[0], hv_RCorners[1], hv_CCorners[1], &hv_DistanceLeft);
		//标定板上边缘和左边缘像素精度
		HTuple hv_PixelDistTop, hv_PixelDistLeft;
		hv_PixelDistTop = hv_widthWorld / hv_DistanceTop;
		hv_PixelDistLeft = hv_heightWorld / hv_DistanceLeft;
		//投影变换前标定板平均像素精度
		HTuple hv_PixelDistMean = (hv_PixelDistTop + hv_PixelDistLeft) / 2;

		//对变换前4个角点求外接矩形(rectangle1)
		HObject ho_Region, ho_RegionTrans;
		HTuple hv_Row1, hv_Column1, hv_Row2, hv_Column2, hv_CenterArea, hv_CenterR, hv_CenterC;
		try
		{
			GenRegionPolygonFilled(&ho_Region, hv_RCorners, hv_CCorners);
			ShapeTrans(ho_Region, &ho_RegionTrans, "rectangle1");
			SmallestRectangle1(ho_RegionTrans, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
			AreaCenter(ho_RegionTrans, &hv_CenterArea, &hv_CenterR, &hv_CenterC);//by QY
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 11;
		}

		//按照像素精度求标定板像素宽和高
		HTuple hv_widthPixel, hv_heightPixel;
		hv_widthPixel = hv_widthWorld / hv_pixelDist;
		hv_heightPixel = hv_heightWorld / hv_pixelDist;

		//变换后4个角点坐标
		HTuple hv_RCornersTrans, hv_CCornersTrans;
		TupleGenConst(4, 0, &hv_RCornersTrans);
		TupleGenConst(4, 0, &hv_CCornersTrans);
		//hv_RCornersTrans[0] = hv_Row1[0];
		//hv_CCornersTrans[0] = hv_Column1[0];
		hv_RCornersTrans[0] = hv_CenterR - hv_heightPixel / 2;//by QY
		hv_CCornersTrans[0] = hv_CenterC - hv_widthPixel / 2;//by QY
		hv_RCornersTrans[1] = hv_RCornersTrans[0] + hv_heightPixel;
		if (hv_RCornersTrans[1].D() >= double(imgHeight))
		{
			hv_RCornersTrans[1] = HTuple(imgHeight);
		}
		hv_CCornersTrans[1] = hv_CCornersTrans[0];
		hv_RCornersTrans[2] = hv_RCornersTrans[0] + hv_heightPixel;
		if (hv_RCornersTrans[2].D() >= double(imgHeight))
		{
			hv_RCornersTrans[2] = HTuple(imgHeight);
		}
		hv_CCornersTrans[2] = hv_CCornersTrans[0] + hv_widthPixel;
		if (hv_CCornersTrans[2].D() >= double(imgWidth))
		{
			hv_CCornersTrans[2] = HTuple(imgWidth);
		}
		hv_RCornersTrans[3] = hv_RCornersTrans[0];
		hv_CCornersTrans[3] = hv_CCornersTrans[0] + hv_widthPixel;
		if (hv_CCornersTrans[3].D() >= double(imgWidth))
		{
			hv_CCornersTrans[3] = HTuple(imgWidth);
		}

		//输出变换后4个角点
		output.vCornerPointsTrans = vCrossPoint2f;
		output.vCornerPointsTrans[0].x = hv_CCornersTrans[0];
		output.vCornerPointsTrans[1].x = hv_CCornersTrans[1];
		output.vCornerPointsTrans[2].x = hv_CCornersTrans[2];
		output.vCornerPointsTrans[3].x = hv_CCornersTrans[3];
		output.vCornerPointsTrans[0].y = hv_RCornersTrans[0];
		output.vCornerPointsTrans[1].y = hv_RCornersTrans[1];
		output.vCornerPointsTrans[2].y = hv_RCornersTrans[2];
		output.vCornerPointsTrans[3].y = hv_RCornersTrans[3];

		HTuple hv_zValue;
		TupleGenConst(4, 1, &hv_zValue);
		HTuple hv_HomMat2D;
		try
		{
			HomVectorToProjHomMat2d(hv_RCorners, hv_CCorners, hv_zValue, hv_RCornersTrans, hv_CCornersTrans, hv_zValue,
				"normalized_dlt", &hv_HomMat2D);
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 12;
		}

		//#define DEBUG
		//#ifdef DEBUG
		//	//测试用
		//	ProjectiveTransImage(testImg, &testImgTrans, hv_HomMat2D, "bilinear",
		//		"false", "false");
		//	WriteImage(testImgTrans, "bmp", 0, "E:/工程代码/workCodeEdit/zkhyProject/transimage.bmp");
		//#endif 

		HTuple hv_saveModelID_path = input.savePath.saveProjTransMatPath.c_str();
		//投影变换矩阵+像素精度
		TupleConcat(hv_HomMat2D, hv_PixelDistMean, &hv_HomMat2D);
		if (input.savePath.saveProjTransMatPath != "")
		{
			HTuple hv_SerializedItemHandle, hv_FileHandle;
			try {
				//保存Tup数据
				WriteTuple(hv_HomMat2D, hv_saveModelID_path);
				/*SerializeHomMat2d(hv_HomMat2D, &hv_SerializedItemHandle);
				OpenFile(hv_saveModelID_path, "output_binary", &hv_FileHandle);
				FwriteSerializedItem(hv_FileHandle, hv_SerializedItemHandle);
				CloseFile(hv_FileHandle);*/
			}
			catch (HalconCpp::HException& except) {
				string errormsg = except.ErrorMessage().Text();
				return 13;
			}
		}
		output.projTransMat = hv_HomMat2D;
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//返回值：0-正常 1-图像为空 2-存储投影变换矩阵异常 3-创建投影变换矩阵失败 4-加载投影变换矩阵失败 5-投影变换转换失败
	int createProjTransMat(CreateProjTransMatInput& input, CreateProjTransMatOutput& output)
	{
		try
		{
			return realCreateProjTransMat(input, output);
		}
		catch (...)
		{
			return 14; //算法未知异常
		}
	}
}
