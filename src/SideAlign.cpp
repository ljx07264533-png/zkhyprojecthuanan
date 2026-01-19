#include "alglibrary/zkhyProjectHuaNan/SideAlign.h"
#include "alglibrary/zkhyProjectHuaNan/utils.h"
#include "alglibrary/alglibLocation.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace cv;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::location;
using namespace alglib::ops::location::alg;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {

	float pointToline(Point2f p, Point2f p1, Point2f p2)
	{
		float a, b, c, dis;
		// 化简两点式为一般式
		// 两点式公式为(y - y1)/(x - x1) = (y2 - y1)/ (x2 - x1)
		// 化简为一般式为(y2 - y1)x + (x1 - x2)y + (x2y1 - x1y2) = 0
		// A = y2 - y1
		// B = x1 - x2
		// C = x2y1 - x1y2
		a = p2.y - p1.y;
		b = p1.x - p2.x;
		c = p2.x * p1.y - p1.x * p2.y;
		// 距离公式为d = |A*x0 + B*y0 + C|/√(A^2 + B^2)
		dis = float(1.0 * abs(a * p.x + b * p.y + c) / sqrt(a * a + b * b));
		return dis;
	}
	int ransacLines(std::vector<cv::Point2f>& input, std::vector<cv::Vec4d>& lines, float distance, unsigned int ngon, unsigned int itmax)
	{
		for (int i = 0; i <int(ngon); ++i) {
			unsigned int Mmax = 0;
			cv::Point2f imax;
			cv::Point2f jmax;
			cv::Vec4d line;
			size_t t1, t2;
			int inter = int(input.size());
			unsigned int it = itmax;
			while (--it)
			{
				t1 = GetRandom(inter);
				t2 = GetRandom(inter);
				int count = 0;
				while (t1 == t2)
				{
					t2 = GetRandom(inter);
					count++;
					if (count > 500)
					{
						return 1;
					}
				}
				unsigned int M = 0;

				Point2f i = input[t1];
				Point2f j = input[t2];
				for (int p = 0; p < input.size(); p++)
				{
					Point2f a = input[p];
					float dis = pointToline(a, i, j);

					if (dis < distance)
						++M;
				}
				if (M > Mmax) {
					Mmax = M;
					imax = i;
					jmax = j;
				}
			}
			line[0] = imax.x;
			line[1] = imax.y;
			line[2] = jmax.x;
			line[3] = jmax.y;


			lines.push_back(line);
			auto iter = input.begin();
			while (iter != input.end())
			{
				float dis = pointToline(*iter, imax, jmax);
				if (dis > distance)
					iter = input.erase(iter);  //erase the dis within , then point to
				//   the next element
				else ++iter;
			}
		}
		return 0;
	}

	int fittingLine(double& RansacDis, int& flag, vector<Point2f>& EdgePoint, vector<Point2f>& EdgePointOutput, LineStruct& LineRes)
	{
		if (EdgePoint.size() < 2)
			return 5;
		vector<Point2f>edgePtsFloat = EdgePoint;
		vector<Vec4d> lineV4;
		double distace = RansacDis;
		/*if (num20dis > 0)
		distace = algPara->d_params.block.distance;*/
		if (ransacLines(edgePtsFloat, lineV4, distace, 1, 500))   //对得到的边缘点进行ransacLines
			return 6;
		int numpts = int(edgePtsFloat.size());
		EdgePointOutput = edgePtsFloat;
		Vec4f lineV4f;
		Vec4f temp;                                              //拟合的轮廓线
		vector<Point2f> vertex;                                                        //ROI的四个顶点
		float k;
		float len;

#if drawline==1
		for (int c = 0; c < edgePtsFloat.size(); c++) {
			globalcolor.at<Vec3b>(Point(edgePtsFloat[c].x + 500, edgePtsFloat[c].y + 500)) = Vec3b(0, 0, 255);
		}
		imwrite("K:/zkhy_boardMeasure/Bin/测试/draw_PTSS.bmp", globalcolor);
#endif
		//float len = (float)sqrt(pow(edgePtsFloat[0].x - edgePtsFloat[numpts-1].x, 2) + pow(edgePtsFloat[0].y - edgePtsFloat[numpts - 1].y, 2));
		cv::fitLine(edgePtsFloat, lineV4f, flag, 0, 0.01, 0.01);
		RotatedRect rect = minAreaRect(edgePtsFloat);
		len = rect.size.height;
		if (rect.size.height < rect.size.width)
			len = rect.size.width;
		int flagK = 0;
		if (abs(lineV4f[0]) < 1e-6) {
			temp[0] = lineV4f[2];
			temp[1] = (float)(lineV4f[3] - len / 2.0);
			temp[2] = lineV4f[2];
			temp[3] = (float)(lineV4f[3] + len / 2.0);
			flagK = 1;
		}
		else if (abs(asin(lineV4f[1])) > CV_PI / 4) {        //所处理的线是竖直线 首点在上 末点在下
			k = (float)(lineV4f[1] / lineV4f[0]);
			temp[0] = (float)(lineV4f[2] - (len / 2.0) / k);
			temp[1] = (float)(lineV4f[3] - len / 2.0);
			temp[2] = (float)(lineV4f[2] + (len / 2.0) / k);
			temp[3] = (float)(lineV4f[3] + len / 2.0);
		}
		else {                                              //所处理的线是水平 首点在上 末点在下
			k = (float)(lineV4f[1] / lineV4f[0]);
			temp[0] = (float)(lineV4f[2] - len / 2.0);
			temp[1] = (float)(lineV4f[3] - len / 2.0 * k);
			temp[2] = (float)(lineV4f[2] + len / 2.0);
			temp[3] = (float)(lineV4f[3] + len / 2.0 * k);
		}
		LineRes.pt1 = Point2f(temp[0], temp[1]);
		LineRes.pt2 = Point2f(temp[2], temp[3]);

		return 0;
	}

	//----------------------------------------------realSideAlign--------------------------//
	//功能：封测侧面检测区域定位
	int realSideAlign(const SideAlignInput& input, SideAlignOutput& output)
	{
		HalconCpp::SetHcppInterfaceStringEncodingIsUtf8(false);
		AlgParamSideAlign* algPara = (AlgParamSideAlign*)&input.algPara;
		HTuple hv_AmpThreshSide = algPara->d_params.block.ampThreshSide;
		HTuple hv_SideSpecHeight = algPara->d_params.block.sideSpecHeight;
		HTuple hv_SideRangeHeight = algPara->d_params.block.sideRangeHeight;
		HTuple hv_SearchStepSide = algPara->i_params.block.searchStepSide;
		HTuple hv_EdgePolarity = algPara->i_params.block.nEdgePolarity;
		HTuple hv_EdgePosition = algPara->i_params.block.nEdgePosition;
		HTuple hv_DilationDist = algPara->i_params.block.nDilationDist;
		//高精度拟合直线参数设置
		AlgParamSideAlignDouble algParamSideAlignDouble = algPara->d_params.block;
		AlgParamSideAlignInt algParamSideAlignInt = algPara->i_params.block;
		//高精度拟合直线参数定义
		GatherLineHighPrecisionInput gatherLineHighPrecisionInput;
		GatherLineHighPrecisionOutput gatherLineHighPrecisionOutput;
		AlgParaGatherLineHighPrecision* algNew = (AlgParaGatherLineHighPrecision*)&gatherLineHighPrecisionInput.algPara;

		//int类型算法参数赋值
		algNew->i_params.block.isCalib = 0;
		algNew->i_params.block.isPreprocessing = 1;
		algNew->i_params.block.nCoarseStep = 5;
		algNew->i_params.block.nEdgePolarity = algParamSideAlignInt.nFitLineEdgePolarity;
		algNew->i_params.block.nEssenceStep = 2;
		algNew->i_params.block.nFitLinePara = 1;
		algNew->i_params.block.nGetEdgeMode = 0;
		algNew->i_params.block.nMinValue = 15;
		algNew->i_params.block.nOffset = 10;
		algNew->i_params.block.nPointCompoundSentence = 1;
		algNew->i_params.block.nThreshold = algParamSideAlignInt.fitLineAmpThresh;
		//double类型算法参数赋值
		algNew->d_params.block.dMultiple = 2.0;
		algNew->d_params.block.dRansacDis = 1.0;
		algNew->d_params.block.dValueDif = 10;

		HTuple hv_EdgePolarityStr;
		if (hv_EdgePolarity.I() == 0)
		{
			hv_EdgePolarityStr = "all";
		}
		else if (hv_EdgePolarity.I() == 1)
		{
			hv_EdgePolarityStr = "positive";
		}
		else if (hv_EdgePolarity.I() == 2)
		{
			hv_EdgePolarityStr = "negative";
		}

		HTuple hv_EdgePositionStr;
		if (hv_EdgePosition.I() == 0)
		{
			hv_EdgePositionStr = "all";
		}
		else if (hv_EdgePosition.I() == 1)
		{
			hv_EdgePositionStr = "first";
		}
		else if (hv_EdgePosition.I() == 2)
		{
			hv_EdgePositionStr = "last";
		}

		//判断输入图像是否为空
		Mat srcImg = input.mImg;
		if (srcImg.rows == 0 || srcImg.cols == 0)
			return 1;
		HObject ho_Image, ho_Median;
		HTuple hv_ImgWidth, hv_ImgHeight;
		//Mat图像转换为HObject
		ho_Median = Mat2HObject(srcImg);
		GetImageSize(ho_Median, &hv_ImgWidth, &hv_ImgHeight);
		//图像平滑处理-中值滤波处理，减少噪声干扰，提高找边准确性
		MedianImage(ho_Median, &ho_Image, "circle", 2, "mirrored");
		//SmoothImage(ho_Image, &meanImage, "deriche1", 1);

		//高精度拟合直线输入Mat图像
		gatherLineHighPrecisionInput.img = input.mImg;

		//判断输入的ROI个数是否等于3
		int rectangleROISize = input.vRectangleROI.size();
		if (rectangleROISize != 3)
			return 8;
		//判断参数设置合理性
		//侧面高度范围设置
		if (hv_SideSpecHeight.D() < 0 || hv_SideRangeHeight.D() < 0)
		{
			return 9;
		}
		//侧面查找边缘点最小梯度设置
		if (hv_AmpThreshSide.D() <= 0)
		{
			return 10;
		}
		//侧面边界查找边缘点步长设置
		if (hv_SearchStepSide.D() <= 0)
		{
			return 11;
		}

		//获取第一个输入矩形框
		RectangleROI RectangleRoi = input.vRectangleROI[0];
		//RectangleROI中轴线中心点坐标
		HTuple hv_RectangleRoiPt1Row = HTuple(RectangleRoi.pt1.y);
		HTuple hv_RectangleRoiPt1Column = HTuple(RectangleRoi.pt1.x);
		HTuple hv_RectangleRoiPt2Row = HTuple(RectangleRoi.pt2.y);
		HTuple hv_RectangleRoiPt2Column = HTuple(RectangleRoi.pt2.x);

		double RectangleRoiYCenter1 = abs(RectangleRoi.pt1.y - RectangleRoi.pt2.y) / 2 + min(RectangleRoi.pt1.y, RectangleRoi.pt2.y);
		double RectangleRoiXCenter1 = abs(RectangleRoi.pt1.x - RectangleRoi.pt2.x) / 2 + min(RectangleRoi.pt1.x, RectangleRoi.pt2.x);

		HTuple  hv_RowCenter, hv_ColumnCenter, hv_Phi, hv_Length1, hv_Length2, hv_rectAngle, hv_roiWidth, hv_roiHeight;
		HTuple  hv_searchStep, hv_searchStartX, hv_searchStartY;
		HTuple  hv_searchAngle, hv_realStepX, hv_realStepY, hv_lineRefStartRow;
		HTuple  hv_lineRefStartCol, hv_lineRefEndRow, hv_lineRefEndCol;

		hv_RowCenter = HTuple(RectangleRoiYCenter1);
		hv_ColumnCenter = HTuple(RectangleRoiXCenter1);
		//RectangleROI中轴线与水平方向夹角
		AngleLx(hv_RectangleRoiPt1Row, hv_RectangleRoiPt1Column, hv_RectangleRoiPt2Row, hv_RectangleRoiPt2Column, &hv_Phi);
		DistancePp(hv_RectangleRoiPt1Row, hv_RectangleRoiPt1Column, hv_RectangleRoiPt2Row, hv_RectangleRoiPt2Column, &hv_Length1);
		hv_Length1 = hv_Length1 / 2;
		hv_Length2 = HTuple(RectangleRoi.offset);

		//***************************************侧面上下长边获取*******************************************************//
		hv_rectAngle = -hv_Phi;
		hv_roiWidth = hv_Length1 * 2;
		hv_roiHeight = hv_Length2 * 2;
		hv_searchStep = hv_SearchStepSide;
		//********************************垂直矩形主轴方向搜索********************************************
		hv_searchStartX = hv_ColumnCenter - (hv_Length1 * (hv_rectAngle.TupleCos()));
		hv_searchStartY = hv_RowCenter - (hv_Length1 * (hv_rectAngle.TupleSin()));
		hv_searchAngle = hv_rectAngle;
		hv_searchAngle = (PI * 0.5) + hv_searchAngle;
		hv_realStepX = hv_searchStep * (hv_rectAngle.TupleCos());
		hv_realStepY = hv_searchStep * (hv_rectAngle.TupleSin());
		hv_lineRefStartRow = hv_searchStartY - ((0.5 * hv_roiHeight) * (hv_searchAngle.TupleSin()));
		hv_lineRefStartCol = hv_searchStartX - ((0.5 * hv_roiHeight) * (hv_searchAngle.TupleCos()));
		hv_lineRefEndRow = hv_searchStartY + ((0.5 * hv_roiHeight) * (hv_searchAngle.TupleSin()));
		hv_lineRefEndCol = hv_searchStartX + ((0.5 * hv_roiHeight) * (hv_searchAngle.TupleCos()));

		//存储最终所有的点
		HTuple  hv_RowsBottom, hv_ColumnsBottom, hv_RowsTop, hv_ColumnsTop;
		HTuple  hv_SideHeightMax, hv_SideHeightMin;
		hv_RowsBottom = HTuple();
		hv_ColumnsBottom = HTuple();
		hv_RowsTop = HTuple();
		hv_ColumnsTop = HTuple();
		HTuple hv_i, hv_lineStartRow, hv_lineStartCol, hv_lineEndRow, hv_lineEndCol, hv_hTmpCtrlRow, hv_hTmpCtrlCol, hv_hTmpCtrlDr;
		HTuple  hv_hTmpCtrlDc, hv_Row_Measure_01_0, hv_Amplitude_Measure_01_0, hv_Column_Measure_01_0, hv_Distance_Measure_01_0;
		HTuple hv_Row_Measure_01_1, hv_Column_Measure_01_1, hv_Amplitude_Measure_01_1, hv_InterDistance_01_0;
		HTuple hv_Row_Measure_02_0, hv_Amplitude_Measure_02_0, hv_Column_Measure_02_0, hv_Distance_Measure_02_0,
			hv_Row_Measure_02_1, hv_Column_Measure_02_1, hv_Amplitude_Measure_02_1, hv_InterDistance_02_0;
		HTuple hv_Less1, hv_Less2, hv_Less2Not, hv_LessAnd, hv_Indices;
		HTuple hv_hTmpCtrlPhi, hv_hTmpCtrlLen1, hv_hTmpCtrlLen2, hv_hMsrHandleMeasure;
		HObject ho_RegionLines, ho_RegionLines1;

		try
		{
			HTuple end_val49 = hv_roiWidth - 1;
			HTuple step_val49 = hv_SearchStepSide;
			for (hv_i = 0; hv_i.Continue(end_val49, step_val49); hv_i += step_val49)
			{
				hv_lineStartRow = hv_lineRefStartRow + ((hv_i / hv_searchStep) * hv_realStepY);
				hv_lineStartCol = hv_lineRefStartCol + ((hv_i / hv_searchStep) * hv_realStepX);
				hv_lineEndRow = hv_lineRefEndRow + ((hv_i / hv_searchStep) * hv_realStepY);
				hv_lineEndCol = hv_lineRefEndCol + ((hv_i / hv_searchStep) * hv_realStepX);

				hv_hTmpCtrlRow = 0.5 * (hv_lineStartRow + hv_lineEndRow);
				hv_hTmpCtrlCol = 0.5 * (hv_lineStartCol + hv_lineEndCol);
				hv_hTmpCtrlDr = hv_lineStartRow - hv_lineEndRow;
				hv_hTmpCtrlDc = hv_lineStartCol - hv_lineEndCol;
				if (RectangleRoi.direction == 0) //逆时针，反向，外部输入矩形朝上
				{
					hv_hTmpCtrlPhi = -hv_searchAngle + PI;
				}
				else
				{
					hv_hTmpCtrlPhi = -hv_searchAngle;//顺时针，正向，外部输入矩形朝下
				}

				hv_hTmpCtrlLen1 = 0.5 * (((hv_hTmpCtrlDr * hv_hTmpCtrlDr) + (hv_hTmpCtrlDc * hv_hTmpCtrlDc)).TupleSqrt());
				hv_hTmpCtrlLen2 = 1;
				//测量矩形找下边缘，边缘对第一个点
				/*赵帅帅代码
				GenMeasureRectangle2(hv_hTmpCtrlRow, hv_hTmpCtrlCol, hv_hTmpCtrlPhi, hv_hTmpCtrlLen1,
					hv_hTmpCtrlLen2, hv_ImgWidth, hv_ImgHeight, "nearest_neighbor", &hv_hMsrHandleMeasure);
				MeasurePos(ho_Image, hv_hMsrHandleMeasure, 1, hv_AmpThreshSide, hv_EdgePolarityStr, hv_EdgePositionStr,
					&hv_Row_Measure_01_0, &hv_Column_Measure_01_0, &hv_Amplitude_Measure_01_0,
					&hv_Distance_Measure_01_0);

				//排除大于某个值的点集
				hv_SideHeightMax = hv_SideSpecHeight + hv_SideSpecHeight * hv_SideRangeHeight;
				hv_SideHeightMin = hv_SideSpecHeight - hv_SideSpecHeight * hv_SideRangeHeight;
				TupleLessElem(hv_Distance_Measure_01_0, hv_SideHeightMax, &hv_Less1);
				TupleLessElem(hv_Distance_Measure_01_0, hv_SideHeightMin, &hv_Less2);
				if (0 != (HTuple(int((hv_Less1.TupleLength()) <= 0)).TupleOr(int((hv_Less2.TupleLength()) <= 0))))
				{
					continue;
				}
				TupleNot(hv_Less2, &hv_Less2Not);
				TupleAnd(hv_Less1, hv_Less2Not, &hv_LessAnd);
				TupleFind(hv_LessAnd, 1, &hv_Indices);

				if (0 != (int(hv_Indices >= 0)))
				{
					TupleConcat(hv_RowsBottom, HTuple(hv_Row_Measure_01_0[hv_Indices]), &hv_RowsBottom);
					TupleConcat(hv_ColumnsBottom, HTuple(hv_Column_Measure_01_0[hv_Indices]), &hv_ColumnsBottom);
					TupleConcat(hv_RowsTop, HTuple(hv_Row_Measure_01_0[hv_Indices + 1]), &hv_RowsTop);
					TupleConcat(hv_ColumnsTop, HTuple(hv_Column_Measure_01_0[hv_Indices + 1]), &hv_ColumnsTop);
				}*/

				//测量矩形找下边缘，边缘对第一个点。极性：黑到白，平滑参数为2
				GenMeasureRectangle2(hv_hTmpCtrlRow, hv_hTmpCtrlCol, hv_hTmpCtrlPhi, hv_hTmpCtrlLen1,
					hv_hTmpCtrlLen2, hv_ImgWidth, hv_ImgHeight, "nearest_neighbor", &hv_hMsrHandleMeasure);
				MeasurePairs(ho_Image, hv_hMsrHandleMeasure, 1, hv_AmpThreshSide, hv_EdgePolarityStr, hv_EdgePositionStr,
					&hv_Row_Measure_01_0, &hv_Column_Measure_01_0, &hv_Amplitude_Measure_01_0, &hv_Row_Measure_01_1, &hv_Column_Measure_01_1,
					&hv_Amplitude_Measure_01_1, &hv_InterDistance_01_0, &hv_Distance_Measure_01_0);
				//测量矩形找上边缘，边缘对最后一个点。极性：黑到白，平滑参数为2
				GenMeasureRectangle2(hv_hTmpCtrlRow, hv_hTmpCtrlCol, hv_hTmpCtrlPhi + PI, hv_hTmpCtrlLen1,
					hv_hTmpCtrlLen2, hv_ImgWidth, hv_ImgHeight, "nearest_neighbor", &hv_hMsrHandleMeasure);
				MeasurePairs(ho_Image, hv_hMsrHandleMeasure, 2, hv_AmpThreshSide, "positive", "last",
					&hv_Row_Measure_02_0, &hv_Column_Measure_02_0, &hv_Amplitude_Measure_02_0, &hv_Row_Measure_02_1, &hv_Column_Measure_02_1,
					&hv_Amplitude_Measure_02_1, &hv_InterDistance_02_0, &hv_Distance_Measure_02_0);

				//排除大于某个值的点集
				hv_SideHeightMax = hv_SideSpecHeight + hv_SideSpecHeight * hv_SideRangeHeight;
				hv_SideHeightMin = hv_SideSpecHeight - hv_SideSpecHeight * hv_SideRangeHeight;
				if ((hv_InterDistance_01_0 > hv_SideHeightMin) && (hv_InterDistance_01_0 < hv_SideHeightMax))
				{
					TupleConcat(hv_RowsBottom, hv_Row_Measure_01_0[0], &hv_RowsBottom);
					TupleConcat(hv_ColumnsBottom, hv_Column_Measure_01_0[0], &hv_ColumnsBottom);
					//TupleConcat(hv_RowsBottom, hv_Row_Measure_01_1[0], &hv_RowsBottom);
					//TupleConcat(hv_ColumnsBottom, hv_Column_Measure_01_1[0], &hv_ColumnsBottom);
				}
				//if ((hv_InterDistance_02_0 > 0.2*hv_SideSpecHeight * hv_SideRangeHeight) && (hv_InterDistance_02_0 < hv_SideHeightMin))
				if ((hv_InterDistance_02_0 > 1) && (hv_InterDistance_02_0 < hv_SideSpecHeight))
				{
					//TupleConcat(hv_RowsTop, HTuple(hv_Row_Measure_02_0[0]), &hv_RowsTop);
					//TupleConcat(hv_ColumnsTop, HTuple(hv_Column_Measure_02_0[0]), &hv_ColumnsTop);
					TupleConcat(hv_RowsTop, HTuple(hv_Row_Measure_02_0[0]), &hv_RowsTop);
					TupleConcat(hv_ColumnsTop, HTuple(hv_Column_Measure_02_0[0]), &hv_ColumnsTop);
				}
			}
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 6;
		}
		//判断侧面上下边界线拟合是否成功
		if (0 != (HTuple(int((hv_RowsTop.TupleLength()) <= 5)).TupleOr(int((hv_RowsBottom.TupleLength()) <= 5))))
		{
			return 6;
		}

		HObject ho_ContourBottom, ho_ContourTop;
		HTuple hv_R1, hv_C1, hv_RR1, hv_CC1, hv_Nr1, hv_Nc1, hv_Dist1;
		HTuple hv_R, hv_C, hv_RR, hv_CC, hv_Nr, hv_Nc, hv_Dist;
		GenCrossContourXld(&ho_ContourBottom, hv_RowsBottom, hv_ColumnsBottom, 10, 0);
		GenCrossContourXld(&ho_ContourTop, hv_RowsTop, hv_ColumnsTop, 10, 0);
		//FitLineContourXld(ho_Contour, "tukey", -1, 0, 5, 2, &hv_R, &hv_C,
		//	&hv_RR, &hv_CC, &hv_Nr, &hv_Nc, &hv_Dist);
		//FitLineContourXld(ho_Contour1, "tukey", -1, 0, 5, 2, &hv_R1, &hv_C1,
		//	&hv_RR1, &hv_CC1, &hv_Nr1, &hv_Nc1, &hv_Dist1)

		//dRansacDis距离 nFitLinePara--fitline模式 EdgePoint--输入点集 EdgePointOutput--输出点集  LineRes--输出直线
		double dRansacDis = 1.0;
		int nFitLinePara = 1;
		LineStruct LineRes;
		vector<Point2f> EdgePoint;
		vector<Point2f>EdgePointOutput;

		//ransac侧面下边缘
		Point2f pt;
		HTuple hv_index;
		HTuple end_val94 = (hv_RowsBottom.TupleLength()) - 1;
		HTuple step_val94 = 1;
		for (hv_index = 0; hv_index.Continue(end_val94, step_val94); hv_index += step_val94)
		{
			pt = Point2f(hv_ColumnsBottom[hv_index].D(), hv_RowsBottom[hv_index].D());
			EdgePoint.push_back(pt);
		}
		fittingLine(dRansacDis, nFitLinePara, EdgePoint, EdgePointOutput, LineRes);
		//输出轮廓点
		output.fitLineContourPts.push_back(EdgePointOutput);
		//ransic后下边缘点
		HTuple  hv_RowBottom, hv_ColumnBottom, hv_RowBottom1, hv_ColumnBottom1;
		hv_RowBottom = LineRes.pt1.y;
		hv_ColumnBottom = LineRes.pt1.x;
		hv_RowBottom1 = LineRes.pt2.y;
		hv_ColumnBottom1 = LineRes.pt2.x;
		GenRegionLine(&ho_RegionLines, hv_RowBottom, hv_ColumnBottom, hv_RowBottom1, hv_ColumnBottom1);

		//ransac侧面上边缘
		LineStruct LineRes1;
		vector<Point2f> EdgePoint1;
		vector<Point2f>EdgePointOutput1;
		Point2f pt1;
		HTuple end_val102 = (hv_RowsTop.TupleLength()) - 1;
		HTuple step_val102 = 1;
		for (hv_index = 0; hv_index.Continue(end_val102, step_val102); hv_index += step_val102)
		{
			pt1 = Point2f(hv_ColumnsTop[hv_index].D(), hv_RowsTop[hv_index].D());
			EdgePoint1.push_back(pt1);
		}
		fittingLine(dRansacDis, nFitLinePara, EdgePoint1, EdgePointOutput1, LineRes1);
		//输出轮廓点
		output.fitLineContourPts.push_back(EdgePointOutput1);
		HTuple  hv_RowTop, hv_ColumnTop, hv_RowTop1, hv_ColumnTop1;
		hv_RowTop = LineRes1.pt1.y;
		hv_ColumnTop = LineRes1.pt1.x;
		hv_RowTop1 = LineRes1.pt2.y;
		hv_ColumnTop1 = LineRes1.pt2.x;
		GenRegionLine(&ho_RegionLines1, hv_RowTop, hv_ColumnTop, hv_RowTop1, hv_ColumnTop1);
		//***************************************侧面上下长边获取*******************************************************//

		//***************************************侧面左侧边缘获取*******************************************************//
		RectangleROI RectangleRoi1 = input.vRectangleROI[1];

		//高精度拟合直线获取矩形框
		int gatherLineHighPrecisionFlag = 0;
		gatherLineHighPrecisionInput.rectangleROI = input.vRectangleROI[1];

		gatherLineHighPrecisionFlag = gatherLineHighPrecision(gatherLineHighPrecisionInput, gatherLineHighPrecisionOutput);
		if (gatherLineHighPrecisionFlag > 0)
		{
			return gatherLineHighPrecisionFlag;
		}

		//拟合直线点转换为htuple
		HTuple hv_LeftLinePt1Row, hv_LeftLinePt1Column, hv_LeftLinePt2Row, hv_LeftLinePt2Column;
		HTuple hv_LeftATopCrossRow, hv_LeftATopCrossColumn, IsOverlappingLAT;
		HTuple hv_LeftABottomCrossRow, hv_LeftABottomCrossColumn, IsOverlappingLAB;
		try
		{
			hv_LeftLinePt1Row = HTuple(gatherLineHighPrecisionOutput.linePic.pt1.y);
			hv_LeftLinePt1Column = HTuple(gatherLineHighPrecisionOutput.linePic.pt1.x);
			hv_LeftLinePt2Row = HTuple(gatherLineHighPrecisionOutput.linePic.pt2.y);
			hv_LeftLinePt2Column = HTuple(gatherLineHighPrecisionOutput.linePic.pt2.x);
			//左侧直线与上下长边交点	
			IntersectionLines(hv_LeftLinePt1Row, hv_LeftLinePt1Column, hv_LeftLinePt2Row, hv_LeftLinePt2Column, hv_RowBottom, hv_ColumnBottom, hv_RowBottom1,
				hv_ColumnBottom1, &hv_LeftATopCrossRow, &hv_LeftATopCrossColumn, &IsOverlappingLAT);
			IntersectionLines(hv_LeftLinePt1Row, hv_LeftLinePt1Column, hv_LeftLinePt2Row, hv_LeftLinePt2Column, hv_RowTop, hv_ColumnTop, hv_RowTop1,
				hv_ColumnTop1, &hv_LeftABottomCrossRow, &hv_LeftABottomCrossColumn, &IsOverlappingLAB);
			//对求出的两个交点向内腐蚀
			if (abs(RectangleRoi1.pt1.x - RectangleRoi1.pt2.x) < abs(RectangleRoi1.pt1.y - RectangleRoi1.pt2.y))
			{
				hv_LeftATopCrossColumn = hv_LeftATopCrossColumn + hv_DilationDist;
				hv_LeftABottomCrossColumn = hv_LeftABottomCrossColumn + hv_DilationDist;
			}
			else
			{
				hv_LeftATopCrossRow = hv_LeftATopCrossRow + hv_DilationDist;
				hv_LeftABottomCrossRow = hv_LeftABottomCrossRow + hv_DilationDist;
			}
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 13;
		}
		//***************************************侧面左侧边缘获取*******************************************************//

		//***************************************侧面右侧边缘获取*******************************************************//
		RectangleROI RectangleRoi2 = input.vRectangleROI[2];
		//高精度拟合直线获取矩形框
		gatherLineHighPrecisionFlag = 0;
		gatherLineHighPrecisionInput.rectangleROI = input.vRectangleROI[2];
		gatherLineHighPrecisionFlag = gatherLineHighPrecision(gatherLineHighPrecisionInput, gatherLineHighPrecisionOutput);
		if (gatherLineHighPrecisionFlag > 0)
		{
			return gatherLineHighPrecisionFlag;
		}
		//拟合直线点转换为htuple
		HTuple hv_RightLinePt1Row, hv_RightLinePt1Column, hv_RightLinePt2Row, hv_RightLinePt2Column;
		HTuple hv_RightATopCrossRow, hv_RightATopCrossColumn, IsOverlappingRAT;
		HTuple hv_RightABottomCrossRow, hv_RightABottomCrossColumn, IsOverlappingRAB;
		try
		{
			hv_RightLinePt1Row = HTuple(gatherLineHighPrecisionOutput.linePic.pt1.y);
			hv_RightLinePt1Column = HTuple(gatherLineHighPrecisionOutput.linePic.pt1.x);
			hv_RightLinePt2Row = HTuple(gatherLineHighPrecisionOutput.linePic.pt2.y);
			hv_RightLinePt2Column = HTuple(gatherLineHighPrecisionOutput.linePic.pt2.x);

			//右侧直线与上下长边交点		
			IntersectionLines(hv_RightLinePt1Row, hv_RightLinePt1Column, hv_RightLinePt2Row, hv_RightLinePt2Column, hv_RowBottom, hv_ColumnBottom, hv_RowBottom1,
				hv_ColumnBottom1, &hv_RightATopCrossRow, &hv_RightATopCrossColumn, &IsOverlappingRAT);
			IntersectionLines(hv_RightLinePt1Row, hv_RightLinePt1Column, hv_RightLinePt2Row, hv_RightLinePt2Column, hv_RowTop, hv_ColumnTop, hv_RowTop1,
				hv_ColumnTop1, &hv_RightABottomCrossRow, &hv_RightABottomCrossColumn, &IsOverlappingRAB);

			//对求出的两个交点向内腐蚀
			if (abs(RectangleRoi1.pt1.x - RectangleRoi1.pt2.x) < abs(RectangleRoi1.pt1.y - RectangleRoi1.pt2.y))
			{
				hv_RightATopCrossColumn = hv_RightATopCrossColumn - hv_DilationDist;
				hv_RightABottomCrossColumn = hv_RightABottomCrossColumn - hv_DilationDist;
			}
			else
			{
				hv_RightATopCrossRow = hv_RightATopCrossRow - hv_DilationDist;
				hv_RightABottomCrossRow = hv_RightABottomCrossRow - hv_DilationDist;
			}
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 14;
		}
		//***************************************侧面右侧边缘获取*******************************************************//

		//***************************************获取侧面区域*******************************************************//
		HObject AlignReion;
		HTuple CrossRows, CrossColumns;
		TupleConcat(CrossRows, hv_LeftATopCrossRow, &CrossRows);
		TupleConcat(CrossRows, hv_LeftABottomCrossRow, &CrossRows);
		TupleConcat(CrossRows, hv_RightABottomCrossRow, &CrossRows);
		TupleConcat(CrossRows, hv_RightATopCrossRow, &CrossRows);
		TupleConcat(CrossRows, hv_LeftATopCrossRow, &CrossRows);

		TupleConcat(CrossColumns, hv_LeftATopCrossColumn, &CrossColumns);
		TupleConcat(CrossColumns, hv_LeftABottomCrossColumn, &CrossColumns);
		TupleConcat(CrossColumns, hv_RightABottomCrossColumn, &CrossColumns);
		TupleConcat(CrossColumns, hv_RightATopCrossColumn, &CrossColumns);
		TupleConcat(CrossColumns, hv_LeftATopCrossColumn, &CrossColumns);
		HObject ho_SideAlignRegion;
		GenRegionPolygonFilled(&ho_SideAlignRegion, CrossRows, CrossColumns);
		//***************************************获取侧面区域*******************************************************//
		output.sideRegion = ho_SideAlignRegion;
		//对获取的轮廓生成点集输出
		vector<Point2f> ContourPt;
		for (int i = 0; i < CrossRows.TupleLength().I(); i++)
		{
			ContourPt.push_back(cv::Point2f(CrossColumns[i].D(), CrossRows[i].D()));
		}
		output.contourPts.push_back(ContourPt);
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------SideAlign--------------------------//
	//功能：封测侧面检测区域定位
	int SideAlign(const SideAlignInput& input, SideAlignOutput& output)
	{
		try
		{
			return realSideAlign(input, output);
		}
		catch (...)
		{
			return 15; //未知错误
		}
	}

}
