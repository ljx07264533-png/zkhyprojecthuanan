#include "alglibrary/zkhyProjectHuaNan/EdgeAlign.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/Packaging2dBodyMeasure.h"
#include "alglibrary/alglibLocation.h"
#include "alglibrary/alglibCalibration.h"
#include "alglibrary/alglibMeasure.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::zkhyProHN::alg;
using namespace alglib::ops::location;
using namespace alglib::ops::location::alg;
using namespace alglib::ops::calibration;
using namespace alglib::ops::calibration::alg;
using namespace alglib::ops::measure;
using namespace alglib::ops::measure::alg;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {
	int fitRectangle(vector<RectangleROI> rectangelROIs, vector<GatherLineHighPrecisionInput> lineInputs, vector<Point2f>& intersections, vector<double>& crossAngle)
	{
		// 判断输入是否合法
		int rectangleROISize = rectangelROIs.size();
		int lineInputSize = lineInputs.size();
		if (rectangleROISize != 4)
			return 8;         // 输入矩形ROI个数应为4个
		if (lineInputSize != 1 && lineInputSize != 4)
			return 9;         // 输入的拟合直线参数应为1个或4个

		//---------------对于输入的4个ROI进行排序---------------//
		// 计算每个扫描矩形ROI的中心点
		struct tempParams {           // 将ROI和拟合直线参数进行关联
			Point2f center;
			RectangleROI roi;
			GatherLineHighPrecisionInput lineParam;
		};
		tempParams params[4];
		for (int i = 0; i < 4; i++)
		{
			params[i].roi = rectangelROIs[i];
			params[i].center.x = (params[i].roi.pt1.x + params[i].roi.pt2.x) * 0.5;
			params[i].center.y = (params[i].roi.pt1.y + params[i].roi.pt2.y) * 0.5;
			if (lineInputSize == 1)
				params[i].lineParam = lineInputs[0];
			else
				params[i].lineParam = lineInputs[i];
		}
		// 按照行坐标排序
		auto sortCenterY = [](const tempParams a, const tempParams b) {return a.center.y < b.center.y; };    // Lambda表达式，用于sort
		sort(params, params + 4, sortCenterY);
		// 按照列坐标排序，考虑与坐标轴平行的情况，只需要对左右ROI排序即可
		if (params[1].center.x > params[2].center.x)
		{
			swap(params[1], params[2]);
		}
		swap(params[2], params[3]);  // 交换位置，使排序后的ROI顺序由 上-左-右-下 变为 上-左-下-右

		// 测试用
		//for (int i = 0; i < 4; i++)
		//{
		//	cout << "pt1: " << params[i].roi.pt1.x << ", " << params[i].roi.pt1.y << " pt2: " << params[i].roi.pt2.x << ", " << params[i].roi.pt2.y << " center:" << params[i].center.x << ", " << params[i].center.y << endl;
		//}

		//-------------按照上-左-下-右的顺序在每个ROI内拟合直线-------------//
		vector<LineStruct> lineStructs;
		lineStructs.clear();
		int gatherLineHighPrecisionFlag = 0;

		/*Mat imgTmp0;
		cv::cvtColor(lineInputs[0].img, imgTmp0, COLOR_GRAY2BGR);

		for (int i = 0; i < 4; i++)
		{
			if (i == 1 || i == 3)
				cv::rectangle(imgTmp0, Point(params[i].roi.pt1.x - params[i].roi.offset, params[i].roi.pt1.y), Point(params[i].roi.pt2.x + params[i].roi.offset, params[i].roi.pt2.y), Scalar(0, 0, 255));
			else
				cv::rectangle(imgTmp0, Point(params[i].roi.pt1.x, params[i].roi.pt1.y - params[i].roi.offset), Point(params[i].roi.pt2.x, params[i].roi.pt2.y + params[i].roi.offset), Scalar(0, 255, 0));
		}
		*/
		for (int i = 0; i < 4; i++)
		{
			GatherLineHighPrecisionOutput gatherLineHighPrecisionOutput;
			params[i].lineParam.rectangleROI = params[i].roi;
			gatherLineHighPrecisionFlag = gatherLineHighPrecision(params[i].lineParam, gatherLineHighPrecisionOutput);
			if (gatherLineHighPrecisionFlag > 0)
			{
				if (gatherLineHighPrecisionFlag >= 7)
					return (gatherLineHighPrecisionFlag + 10);         //高精度拟合直线参数异常
				else
					return gatherLineHighPrecisionFlag;
			}
			lineStructs.push_back(gatherLineHighPrecisionOutput.linePic);
		}

		// 计算两直线构成的交点坐标和直线所构成的四个内角的角度
		ConstructCrossPointInput constructCrossPointInput;
		int constructCrossPointInputFlag = 0;
		Point2f crossPointTmp;
		intersections.clear();
		// 依次为左上角，左下角，右下角和右上角
		// 方法1和方法2在小数点后第五位开始有差异
		int methodFlag = 2;
		for (int i = 0; i < 4; i++)
		{
			int j = (i + 1) % 4;
			constructCrossPointInput.line1 = lineStructs[i];
			constructCrossPointInput.line2 = lineStructs[j];
			constructCrossPointInputFlag = constructCrossPoint(constructCrossPointInput, crossPointTmp);
			if (constructCrossPointInputFlag != 0)
			{
				return 10;        //构造交点失败
			}
			intersections.push_back(crossPointTmp);
			if (methodFlag == 1)
			{
				// 方法1
				LineAngleInput angleTmpInput1, angleTmpInput2;
				LineAngleRlt angleTmpOutput1, angleTmpOutput2;
				angleTmpInput1.line = constructCrossPointInput.line1;
				angleTmpInput2.line = constructCrossPointInput.line2;
				// 获得直线和X轴正轴的夹角
				// 以X轴正轴进行旋转到直线，顺时针旋转为正 逆时针为负
				constructLineAngle(angleTmpInput1, angleTmpOutput1);
				constructLineAngle(angleTmpInput2, angleTmpOutput2);
				double angleTmp1, angleTmp2, angleRlt;
				angleTmp1 = angleTmpOutput1.angle;
				angleTmp2 = angleTmpOutput2.angle;
				// 角度为负值时+2Π转换为正值
				if (angleTmp1 > 0)
					angleTmp1 = angleTmp1 - myPi;
				if (angleTmp2 > 0)
					angleTmp2 = angleTmp2 - myPi;
				switch (i)
				{
				case 0:
					angleRlt = myPi + (angleTmp2 - angleTmp1);
					break;
				case 1:
					angleRlt = angleTmp2 - angleTmp1;
					break;
				case 2:
					angleRlt = myPi + (angleTmp2 - angleTmp1);
					break;
				case 3:
					angleRlt = angleTmp2 - angleTmp1;
					break;
				default:
					break;
				}
				angleRlt = angleRlt * 180 / myPi;
				crossAngle.push_back(angleRlt);
			}
			else
			{
				// 方法2
				MeasureIntersectionAngleInput measureIntersectionAngleInputTmp;
				MeasureIntersectionAngleRlt measureIntersectionAngleRltTmp;
				measureIntersectionAngleInputTmp.Line1 = constructCrossPointInput.line1;
				measureIntersectionAngleInputTmp.Line2 = constructCrossPointInput.line2;

				measureIntersectionAngleInputTmp.flag = 0;

				measureIntersectionAngle(measureIntersectionAngleInputTmp, measureIntersectionAngleRltTmp);
				crossAngle.push_back(measureIntersectionAngleRltTmp.measureResult.rlt);
			}
		}

		// 显示拟合的直线
		/*Mat imgTmp1;
		cv::cvtColor(lineInputs[0].img, imgTmp1, COLOR_GRAY2BGR);
		for (int i = 0; i < 4; i++)
		{
			cv::line(imgTmp1, Point(lineStructs[i].pt1.x, lineStructs[i].pt1.y), Point(lineStructs[i].pt2.x, lineStructs[i].pt2.y), Scalar(0, 0, 255), 3);
		}
		//*/
		//// 显示四个角点和对应的角度
		/*Mat imgTmp2;
		cv::cvtColor(lineInputs[0].img, imgTmp2, COLOR_GRAY2BGR);
		for (int i = 0; i < 4; i++)
		{
			cv::circle(imgTmp2, Point(intersections[i].x, intersections[i].y), 3, Scalar(0, 255, 0));

			string info1 = string("angle1: ") + to_string(crossAngle[i]);
			//string info2 = string("angle2: ") + to_string(crossAngle[i * 2 + 1]);
			Point2f tmpPoint;
			tmpPoint.x = intersections[0].x - 60;
			tmpPoint.y = 50 + i * 2 * 30;
			cv::putText(imgTmp2, info1, tmpPoint, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 1);
			//tmpPoint.x = intersections[0].x - 60;
			//tmpPoint.y = 50 + (i * 2 + 1) * 30;
			//cv::putText(imgTmp2, info2, tmpPoint, FONT_HERSHEY_COMPLEX, 1, Scalar(0, 255, 0), 1);
		}

		Sleep(10);*/
		return 0;
	}

	//----------------------------------------------realEdgeAlign--------------------------------//
	//功能：采用拟合产品四条边缘的方式构建坐标系，计算待测图到Layout以及待测图到模板图的坐标变换关系
	int realEdgeAlign(const EdgeAlignInput& input, EdgeAlignOutput& output)
	{
		//char debugInfo[512];
		//sprintf_s(debugInfo, "[Alg Debug] Alg_name:[EdgeAlign] 开始执行\n");
		//OutputDebugStringA(debugInfo);
		Mat matImg = input.matImg;
		// 判断输入合法性
		if (matImg.empty())
			return 11;           // 输入Mat图像为空

		// 解析参数
		AlgParamEdgeAlign* algEdgeAlign = (AlgParamEdgeAlign*)&input.algPara;
		int isDepthImage = algEdgeAlign->i_params.block.isDepthImage;
		int isComputeFlaw = algEdgeAlign->i_params.block.isComputeFlaw;
		double timeOutThreshold = algEdgeAlign->d_params.block.dTimeOutThreshold;
		if (timeOutThreshold > 999999 || timeOutThreshold <= 0)
			return 13;           // 算法超时参数输入有误

		Mat imgUsed;
		double isDebug = false;

		//输入为深度图时对图像的灰度值进行拉伸
		if (isDepthImage == 1)
		{
			/*string filename = "EdgeAlign.txt";
			ofstream ofs;
			ofs.open(filename, ios::out | ios::app);
			if (ofs.is_open())
				ofs << "[EdgeAlign] image type: " << matImg.type() << ", image Size: "<<matImg.size() << endl;
			ofs.close();*/
			//memset(debugInfo, 0, sizeof(debugInfo));
			//sprintf_s(debugInfo, "[Alg Debug] Alg_name:[EdgeAlign] image type: %d\n", matImg.type());
			//OutputDebugStringA(debugInfo);
			//cout<< "[EdgeAlign] image type: " << matImg.type() << ", image Size: " << matImg.size() << endl;
			/*if (matImg.type() == 0)  //CV_8U
			{
				int imgHeight, imgWidth;
				imgHeight = matImg.rows;
				imgWidth = matImg.cols;
				double maxValue, minValue, maxMinDifferValue;
				cv::minMaxLoc(matImg, &minValue, &maxValue, NULL, NULL);
				maxMinDifferValue = maxValue - minValue;
				imgUsed = cv::Mat_<UINT8>(imgHeight, imgWidth);
				UINT8* ptimgOri = (UINT8*)matImg.data;
				UINT8* ptimgOut = (UINT8*)imgUsed.data;
				for (int i = 0; i < imgHeight; ++i) {
					ptimgOri = (UINT8*)(matImg.data + i * matImg.step);
					ptimgOut = (UINT8*)(imgUsed.data + i * imgUsed.step);
					for (int j = 0; j < imgWidth; ++j)
					{
						*ptimgOut = (*ptimgOri - minValue) / maxMinDifferValue * 255.0;
						++ptimgOri;
						++ptimgOut;
					}
				}
			}*/
			//else if (matImg.type() == 2)  //CV_16U
			/*if (matImg.type() == 2)  //CV_16U
			{
				int imgHeight, imgWidth;
				imgHeight = matImg.rows;
				imgWidth = matImg.cols;
				double maxValue, minValue, maxMinDifferValue;
				cv::minMaxLoc(matImg, &minValue, &maxValue, NULL, NULL);
				maxMinDifferValue = maxValue - minValue;
				imgUsed = cv::Mat_<UINT8>(imgHeight, imgWidth);
				UINT16* ptimgOri = (UINT16*)matImg.data;
				UINT8* ptimgOut = (UINT8*)imgUsed.data;
				for (int i = 0; i < imgHeight; ++i) {
					ptimgOri = (UINT16*)(matImg.data + i * matImg.step);
					ptimgOut = (UINT8*)(imgUsed.data + i * imgUsed.step);
					for (int j = 0; j < imgWidth; ++j)
					{
						*ptimgOut = (*ptimgOri - minValue) / maxMinDifferValue * 255.0;
						++ptimgOri;
						++ptimgOut;
					}
				}
			}
			else if (matImg.type() == 5)  //CV_32F
			{
				int imgHeight, imgWidth;
				imgHeight = matImg.rows;
				imgWidth = matImg.cols;
				double maxValue, minValue, maxMinDifferValue;
				cv::minMaxLoc(matImg, &minValue, &maxValue, NULL, NULL);
				maxMinDifferValue = maxValue - minValue;
				imgUsed = cv::Mat_<UINT8>(imgHeight, imgWidth);
				float* ptimgOri = (float*)matImg.data;
				UINT8* ptimgOut = (UINT8*)imgUsed.data;
				for (int i = 0; i < imgHeight; ++i) {
					ptimgOri = (float*)(matImg.data + i * matImg.step);
					ptimgOut = (UINT8*)(imgUsed.data + i * imgUsed.step);
					for (int j = 0; j < imgWidth; ++j)
					{
						*ptimgOut = (*ptimgOri - minValue) / maxMinDifferValue * 255.0;
						++ptimgOri;
						++ptimgOut;
					}
				}
			}*/
			int imageType = matImg.type();
			switch (imageType)
			{
			case CV_16U:
				int imgHeight, imgWidth;
				imgHeight = matImg.rows;
				imgWidth = matImg.cols;
				double maxValue, minValue, maxMinDifferValue;
				//cv::minMaxLoc(matImg, &minValue, &maxValue, NULL, NULL);
				//maxMinDifferValue = maxValue - minValue;
				maxMinDifferValue = 0xFFFF;
				imgUsed = cv::Mat_<UINT8>(imgHeight, imgWidth);
				matImg.convertTo(imgUsed, CV_8UC1, 255.0 / maxMinDifferValue);
				break;
			case CV_32F:
				int imgHeight2, imgWidth2;
				imgHeight2 = matImg.rows;
				imgWidth2 = matImg.cols;
				double maxValue2, minValue2, maxMinDifferValue2;
				//cv::minMaxLoc(matImg, &minValue, &maxValue, NULL, NULL);
				//maxMinDifferValue = maxValue - minValue;
				maxMinDifferValue2 = 0XFFFFFFFF;
				imgUsed = cv::Mat_<UINT8>(imgHeight2, imgWidth2);
				matImg.convertTo(imgUsed, CV_8UC1, 255.0 / maxMinDifferValue2);
				break;
			default:
				//memset(debugInfo, 0, sizeof(debugInfo));
				//sprintf_s(debugInfo, "[Alg Debug] Alg_name:[EdgeAlign] 不支持的深度图类型\n");
				//OutputDebugStringA(debugInfo);
				return 15;       // 不支持的深度图类型
			}
		}
		// 3通道图像转为单通道
		else if (matImg.channels() == 3)
			cvtColor(matImg, imgUsed, cv::COLOR_RGB2GRAY);
		else
			imgUsed = matImg;

		vector<GatherLineHighPrecisionInput> lineInputs;
		// 拟合直线高精度参数赋值
		GatherLineHighPrecisionInput lineParams;
		lineParams.img = imgUsed;
		lineParams.calParaMat.pixEquival = -99999;
		AlgParaGatherLineHighPrecision* algGatherLine = (AlgParaGatherLineHighPrecision*)&lineParams.algPara;
		// 输入参数赋值
		algGatherLine->i_params.block.isPreprocessing = algEdgeAlign->i_params.block.isPreprocessing;
		algGatherLine->i_params.block.nCoarseStep = algEdgeAlign->i_params.block.iCoarseStep;
		algGatherLine->i_params.block.nEdgePolarity = algEdgeAlign->i_params.block.iEdgePolarity;
		algGatherLine->i_params.block.nEssenceStep = algEdgeAlign->i_params.block.iEssenceStep;
		algGatherLine->i_params.block.nThreshold = algEdgeAlign->i_params.block.iThreshold;
		algGatherLine->i_params.block.nMinValue = algEdgeAlign->i_params.block.iMinValue;
		algGatherLine->i_params.block.nOffset = algEdgeAlign->i_params.block.iOffset;
		algGatherLine->i_params.block.nPointCompoundSentence = algEdgeAlign->i_params.block.iPointCompoundSentence;
		algGatherLine->i_params.block.nFitLinePara = algEdgeAlign->i_params.block.iFitLinePara;
		algGatherLine->d_params.block.dMultiple = algEdgeAlign->d_params.block.dMultiple;
		algGatherLine->d_params.block.dRansacDis = algEdgeAlign->d_params.block.dRansacDis;
		algGatherLine->d_params.block.dValueDif = algEdgeAlign->d_params.block.dValueDif;
		// 固定参数赋值
		algGatherLine->i_params.block.isCalib = 0;
		algGatherLine->i_params.block.nGetEdgeMode = 0;
		algGatherLine->i_params.block.nEssenceOffset = 1;

		lineInputs.push_back(lineParams);

		CSYSTransPara  templateTransPara = input.templateTransPara; //模板图中特征坐标系参数
		CSYSTransPara  layoutTransPara;     //Layout特征坐标系,默认layout位于(0, 0)点，旋转角度为0
		layoutTransPara.angle = 0;
		//layoutTransPara.translationValue.x = layoutCenter.x;
		//layoutTransPara.translationValue.y = layoutCenter.y;
		layoutTransPara.translationValue.x = 0;
		layoutTransPara.translationValue.y = 0;

		// 开始计时
		DWORD startTime = GetTickCount64();

		vector<Point2f> intersections;
		vector<double> crossAngle;
		int exeFlag = 0;
		exeFlag = fitRectangle(input.rectangelROIs, lineInputs, intersections, crossAngle);
		if (exeFlag != 0)
		{
			//memset(debugInfo, 0, sizeof(debugInfo));
			//sprintf_s(debugInfo, "[Alg Debug] Alg_name:[EdgeAlign] 拟合边缘错误\n");
			//OutputDebugStringA(debugInfo);
			return exeFlag;          // 拟合边缘错误
		}
		if (intersections.size() != 4)
		{
			//memset(debugInfo, 0, sizeof(debugInfo));
			//sprintf_s(debugInfo, "[Alg Debug] Alg_name:[EdgeAlign] 交点构造失败\n");
			//OutputDebugStringA(debugInfo);
			return 10;
		}
		// 根据四个顶点计算中心点坐标
		Point2f centerPoint;
		//Point2f centerPoint2, centerPoint3;
		cv::Moments  vertexMoments = cv::moments(intersections);
		centerPoint.x = float(vertexMoments.m10 / vertexMoments.m00);
		centerPoint.y = float(vertexMoments.m01 / vertexMoments.m00);
		//LineStruct lineTmp1, lineTmp2;
		//lineTmp1.pt1 = intersections[0];
		//lineTmp1.pt2 = intersections[2];
		//lineTmp2.pt1 = intersections[1];
		//lineTmp2.pt2 = intersections[3];
		//ConstructCrossPointInput constructCrossPointInput;
		//constructCrossPointInput.line1 = lineTmp1;
		//constructCrossPointInput.line2 = lineTmp2;
		//int constructCrossPointInputFlag = constructCrossPoint(constructCrossPointInput, centerPoint2);
		//if (constructCrossPointInputFlag != 0)
		//{
		//	return 10;        //构造交点失败
		//}
		//centerPoint3.x = (intersections[0].x + intersections[1].x + intersections[2].x + intersections[3].x) / 4.0;
		//centerPoint3.y = (intersections[0].y + intersections[1].y + intersections[2].y + intersections[3].y) / 4.0;

		//auto computeDistance = [](Point2f p1, Point2f p2) {return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y)); };

		// 使用产品中心点作为参考点
		//double lineLength = computeDistance(intersections[0], intersections[3]);
		CSYSTransPara  baseTransInfo;
		//baseTransInfo.translationValue = centerPoint;
		baseTransInfo.translationValue = intersections[0]; // 根据顶部直线计算坐标变换信息
		//baseTransInfo.angle = asin((intersections[0].y - intersections[3].y) / lineLength);
		baseTransInfo.angle = atan((intersections[3].y - intersections[0].y) / (intersections[3].x - intersections[0].x));

		// 坐标变换信息赋值
		// !!!!!!!!!!!! 使用坐标系时需注意 当采用halcon的homMat2d时，需将旋转角度取反
		// computeTemplate2SampleCSYSpara 计算旋转角度时，使用sample的角度减去template的角度，此时假设template的角度为0，sample的角度为旋转角度a，a > 0，sample需要顺时针旋转a能与template角度一致
		// 而halcon中顺时针旋转时，旋转角度为负值，所以需要将旋转角度取反
		//模板图到待测图
		ComputeTemplate2SampleCSYSparaInput template2DetectTransPara;
		template2DetectTransPara.sampleImgCSYSpara = baseTransInfo;
		template2DetectTransPara.templateImgCSYSpara = templateTransPara;
		computeTemplate2SampleCSYSpara(template2DetectTransPara, output.template2DetectTransPara);
		//Layout到待测图
		ComputeTemplate2SampleCSYSparaInput layout2DetectTransPara;
		layout2DetectTransPara.sampleImgCSYSpara = baseTransInfo;
		layout2DetectTransPara.templateImgCSYSpara = layoutTransPara;
		computeTemplate2SampleCSYSpara(layout2DetectTransPara, output.layout2DetectTransPara);

		// debug 测试旋转效果
		if (isDebug)
		{
			HTuple homMat2d1, homMat2d2;
			VectorAngleToRigid(baseTransInfo.translationValue.y, baseTransInfo.translationValue.x, baseTransInfo.angle, layoutTransPara.translationValue.y + 50, layoutTransPara.translationValue.x + 50, layoutTransPara.angle, &homMat2d1);
			HObject hImg, hImgTrans1, hImgTrans2;
			hImg = Mat2HObject(imgUsed);
			AffineTransImage(hImg, &hImgTrans1, homMat2d1, "nearest_neighbor", "true");

			VectorAngleToRigid(50, 50, 0, output.layout2DetectTransPara.translationValue.y, output.layout2DetectTransPara.translationValue.x, output.layout2DetectTransPara.angle, &homMat2d2);
			HomMat2dInvert(homMat2d2, &homMat2d2);
			AffineTransImage(hImg, &hImgTrans2, homMat2d2, "nearest_neighbor", "true");

			Sleep(10);
		}

		//建模板图时需要记录的模板图位置信息
		output.templateTransPara = baseTransInfo;
		//生成IC区域
		HObject hICRegion;
		HTuple hICRow, hICCol;
		zkhyPublicFuncHN::points2HTuple(intersections, hICCol, hICRow);
		GenRegionPolygonFilled(&hICRegion, hICRow, hICCol);
		output.ICRegion = hICRegion;
		// 顶点信息赋值
		vector<Point2f>  vertexs(5);
		vector<Point2f>  contourTmp(5);
		vector<vector<Point2f>> ICContour(1);
		for (int i = 0; i < 4; i++)
		{
			vertexs[i] = intersections[i];
			contourTmp[i] = intersections[i];
		}
		vertexs[4] = centerPoint;
		contourTmp[4] = intersections[0];
		output.vertexs = vertexs;
		ICContour[0] = contourTmp;
		output.ICContour = ICContour;

		int stateFlag = 0;
		// 计算封测产品的垂直度，平行度，封装宽度和高度信息
		if (isComputeFlaw == 1)
		{
			Packaging2dBodyMeasureInput bodyMeasureInput;
			Packaging2dBodyMeasureOutput bodyMeasureOutput;
			/*HObject hEmpty;
			HTuple hIsEuqal;
			GenEmptyRegion(&hEmpty);
			TestEqualRegion(input.detICRegion, hEmpty, &hIsEuqal);
			if (hIsEuqal.I() == 1)
			{
				return 14;           // 输入区域为空
			}
			bodyMeasureInput.detICRegion = input.detICRegion;*/
			bodyMeasureInput.vertexs = vertexs;
			bodyMeasureInput.crossAngles = crossAngle;
			bodyMeasureInput.detTransPara = baseTransInfo;
			AlgParamPackaging2dBodyMeasure* algBodyMeasure = (AlgParamPackaging2dBodyMeasure*)&bodyMeasureInput.algPara;
			algBodyMeasure->i_params.block.iPackagingType = 0;
			algBodyMeasure->d_params.block.dTimeOutThreshold = algEdgeAlign->d_params.block.dTimeOutThreshold;

			stateFlag = Packaging2dBodyMeasure(bodyMeasureInput, bodyMeasureOutput);
			if (stateFlag == 1)
				stateFlag = 13;
			else if (stateFlag == 2)
				stateFlag = 14;
			else if (stateFlag == 3)
				stateFlag = 16;

			if (stateFlag != 0)
				return stateFlag;
			output.flawsData = bodyMeasureOutput.flawsData;
			//output.flawsData.assign(bodyMeasureOutput.flawsData.begin(), bodyMeasureOutput.flawsData.end());
			// 只有一个缺陷
			output.flawStatic.resize(3);
			output.flawStatic[0] = bodyMeasureOutput.flawsData[0];
			output.flawStatic[1] = bodyMeasureOutput.flawsData[0];
			output.flawStatic[2] = bodyMeasureOutput.flawsData[0];
			//output.flawStatic = bodyMeasureOutput.flawsData;

			// 输出检测数据
			// 将宽高信息写入文件
			/*string fileRoot = "D:\\code\\数据解析\\packagingtest\\bodyData0427\\";
			string filename = "EdgeAlign-4-2.txt";
			ofstream ofs;
			ofs.open((fileRoot + filename), ios::out | ios::app);
			ofs.setf(ios::fixed, ios::floatfield);
			ofs.precision(20);
			if (ofs.is_open())
				ofs << "[Alg Debug] Alg_name:[EdgeAlign] TransPara: Width: " << output.flawsData[0].extendParameters.d_params.block.bodySizeX << ", Height: " << output.flawsData[0].extendParameters.d_params.block.bodySizeY << endl;
			ofs.close();*/

			//if (stateFlag != 1)     //返回错误值
			//	return 100 + stateFlag;
		}

		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 14;                   // 算法超时

		// 将位置信息写入文件
		/*string fileRoot = "D:\\code\\数据解析\\packagingtest\\";
		string filename = "EdgeAlign-0423.txt";
		ofstream ofs;
		ofs.open((fileRoot + filename), ios::out | ios::app);
		ofs.setf(ios::fixed, ios::floatfield);
		ofs.precision(20);
		if (ofs.is_open())
			ofs <<"[Alg Debug] Alg_name:[EdgeAlign] TransPara:" << output.layout2DetectTransPara.translationValue.x << ", " << output.layout2DetectTransPara.translationValue.y << ", " << output.layout2DetectTransPara.angle << endl;
		ofs.close();*/
		//memset(debugInfo, 0, sizeof(debugInfo));
		//sprintf_s(debugInfo, "[Alg Debug] Alg_name:[EdgeAlign] 执行成功\n[Alg Debug] Alg_name:[EdgeAlign] angle:%f x:%f y:%f\n", output.layout2DetectTransPara.angle, output.layout2DetectTransPara.translationValue.x, output.layout2DetectTransPara.translationValue.y);
		//OutputDebugStringA(debugInfo);

		// 删除vector<point2f>
		vector<Point2f>().swap(intersections);
		vector<Point2f>().swap(vertexs);
		vector<Point2f>().swap(contourTmp);
		vector<vector<Point2f>>().swap(ICContour);

		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------EdgeAlign--------------------------//
	//功能：封测产品边缘对齐函数入口
	int EdgeAlign(const EdgeAlignInput& input, EdgeAlignOutput& output)
	{
		try
		{
			int iRet = realEdgeAlign(input, output);
			return iRet;
		}
		catch (...)
		{
			return 16; //未知错误
		}
	}
}
