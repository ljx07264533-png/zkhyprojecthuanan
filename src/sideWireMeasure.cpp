//20240226 增加侧面线宽线距测量（sideWireMeasure）算子。--edited by Guo Xulong
//20240403 侧面线宽线距测量（sideWireMeasure）算子：1.修复图像旋转时角度计算错误的问题。--edited by Guo Xulong
//20240411 侧面线宽线距测量（sideWireMeasure）算子：1.修改输出结构体，增加轮廓用于后续显示和缺陷小图使用。	   --edited by Guo Xulong
//20240417 侧面线宽线距测量（sideWireMeasure）算子：1.修合并代码导致版本不正确的问题。--edited by Guo Xulong
//20240421 侧面线宽线距测量（sideWireMeasure）算子：1. bug修正，修复图像旋转时角度的问题。	   --edited by Guo Xulong

#include "alglibrary/zkhyProjectHuaNan/sideWireMeasure.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/zkhy_publicClassHN.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

const double Inf = 1e-8;

namespace alglib::ops::zkhyProHN {
	//侧面线路测量算法类
	class wireMeasure : public zkhyPublicClass::AlgorithmBase
	{
	public:
		wireMeasure();
		~wireMeasure() {};
		wireMeasure(const sideWireMeasureInput& input);

		int checkParams();		// 检查输入参数是否越界

		int preProcess();		// 预处理
		int measureDistance();	// 测量距离
		int execute();			// 执行算法
		int postProcess();		// 后处理

		void getResult(sideWireMeasureOutput& output);		// 获取测量结果

	protected:
		HObject hDetImg, hDetRegion, hBaseLineContour;
		HTuple hImgWidth, hImgHeight, hHomMat2D, hMeasureRows, hMeasureCols, hMeasureAmplitudes, hMeasureDistances, hImgWidthTrans, hImgHeightTrans;

		int lineOffsetDirection, lineHeight, lineWidth, productDirection, lineSpacing, lineColor;
		double smoothFactor, edgeStrength, lineWidthRange, lineSpacingRange;

		LineStruct baseLine;
		sideWireMeasureOutput measureResult;
	};

	// 默认构造函数
	wireMeasure::wireMeasure()
	{
		lineOffsetDirection = 0;
		lineHeight = 200;
		lineWidth = 50;
		productDirection = 0;
		lineSpacing = 100;
		lineColor = 0;
		smoothFactor = 1;
		edgeStrength = 30;
		lineWidthRange = 0.1;
		lineSpacingRange = 0.1;

		GenEmptyObj(&hDetImg);
		GenEmptyObj(&hDetRegion);
		GenEmptyObj(&hBaseLineContour);
		hImgWidth.Clear();
		hImgHeight.Clear();
		hHomMat2D.Clear();
		hMeasureRows.Clear();
		hMeasureCols.Clear();
		hMeasureAmplitudes.Clear();
		hMeasureDistances.Clear();
		hImgHeightTrans.Clear();
		hImgWidthTrans.Clear();

		baseLine.pt1 = cv::Point2f(0, 0);
		baseLine.pt2 = cv::Point2f(0, 0);
	}

	// 构造函数
	wireMeasure::wireMeasure(const sideWireMeasureInput& input)
	{
		AlgParaSideWireMeasure* algParams = (AlgParaSideWireMeasure*)&input.algPara;

		lineOffsetDirection = algParams->i_params.block.iLineOffsetDirection;
		lineHeight = algParams->i_params.block.iLineHeight;
		lineWidth = algParams->i_params.block.iLineWidth;
		lineSpacing = algParams->i_params.block.iLineSpacing;
		lineColor = algParams->i_params.block.iLineColor;
		productDirection = algParams->i_params.block.iProductDirection;
		smoothFactor = algParams->d_params.block.dSmoothFactor;
		edgeStrength = algParams->d_params.block.dEdgeStrength;
		lineWidthRange = algParams->d_params.block.dLineWidthRange;
		lineSpacingRange = algParams->d_params.block.dLineSpacingRange;

		hDetImg = input.inputImage;
		GetImageSize(hDetImg, &hImgWidth, &hImgHeight);

		hDetRegion = input.hDetRegion;

		if (zkhyPublicFuncHN::isHRegionEmpty(hDetRegion))
		{
			vector<cv::Point2f> detRegionTmp = input.detRegion;
			if (detRegionTmp.size() != 0)
			{
				zkhyPublicFuncHN::genRegionFromContour(detRegionTmp, &hDetRegion);
			}
		}

		baseLine = input.baseLine;

		GenEmptyObj(&hBaseLineContour);
		hHomMat2D.Clear();
		hMeasureRows.Clear();
		hMeasureCols.Clear();
		hMeasureAmplitudes.Clear();
		hMeasureDistances.Clear();
		hImgHeightTrans.Clear();
		hImgWidthTrans.Clear();
	}

	// 检查输入参数是否越界
	int wireMeasure::checkParams()
	{
		if (lineOffsetDirection < 0 || lineOffsetDirection > 1)
		{
			return 3;
		}
		if (lineHeight < 0)
		{
			return 4;
		}
		if (lineWidth < 0)
		{
			return 5;
		}
		if (lineSpacing < 0)
		{
			return 6;
		}
		if (lineColor < 0 || lineColor > 3)
		{
			return 7;
		}
		if (smoothFactor < 0)
		{
			return 8;
		}
		if (edgeStrength < 0)
		{
			return 9;
		}
		if (productDirection < 0 || productDirection > 1)
		{
			return 10;
		}
		if (lineWidthRange < 0)
		{
			return 11;
		}
		if (lineSpacingRange < 0)
		{
			return 12;
		}

		return 0;
	}

	// 预处理
	int wireMeasure::preProcess()
	{
		// 根据查找到直线的旋转角度将图像旋转成水平状态
		HObject hDetImgRot, hLineCountour, hDetRegionRot;

		LineStruct lineTmp;

		if (baseLine.pt1.y > baseLine.pt2.y)
		{
			lineTmp.pt1 = baseLine.pt2;
			lineTmp.pt2 = baseLine.pt1;
		}
		else
		{
			lineTmp.pt1 = baseLine.pt1;
			lineTmp.pt2 = baseLine.pt2;
		}

		double angle2zero;
		if (productDirection == 0)
			angle2zero = 0;
		else
			angle2zero = PI;

		//double lineAngle = (atan2(lineTmp.pt2.y - lineTmp.pt1.y, lineTmp.pt2.x - lineTmp.pt1.x) - angle2zero);
		double lineAngle = (atan((lineTmp.pt2.y - lineTmp.pt1.y) / (lineTmp.pt2.x - lineTmp.pt1.x)) - angle2zero);

		// 注意旋转角度
		VectorAngleToRigid(hImgWidth / 2, hImgHeight / 2, 0, hImgHeight / 2, hImgWidth / 2, lineAngle, &hHomMat2D);

		AffineTransImage(hDetImg, &hDetImgRot, hHomMat2D, "bilinear", "false");
		hDetImg = hDetImgRot;

		HTuple hLineRow, hLineCol;
		hLineRow.Clear();
		hLineCol.Clear();
		hLineRow.Append(lineTmp.pt1.y);
		hLineRow.Append(lineTmp.pt2.y);
		hLineCol.Append(lineTmp.pt1.x);
		hLineCol.Append(lineTmp.pt2.x);

		GenContourPolygonXld(&hLineCountour, hLineRow, hLineCol);
		AffineTransContourXld(hLineCountour, &hBaseLineContour, hHomMat2D);

		if (!zkhyPublicFuncHN::isHRegionEmpty(hDetRegion))
		{
			AffineTransRegion(hDetRegion, &hDetRegionRot, hHomMat2D, "nearest_neighbor");
			hDetRegion = hDetRegionRot;
		}
		GetImageSize(hDetImg, &hImgWidthTrans, &hImgHeightTrans);

		return 0;
	}

	// 测量距离
	int wireMeasure::measureDistance()
	{
		// 计算扫描矩形
		// 计算测量矩形起点和终点
		// 列坐标为检测区域的左右两端或者图像宽度
		HTuple hRectStartRow, hRectStartCol, hRectEndRow, hRectEndCol;
		if (zkhyPublicFuncHN::isHRegionEmpty(hDetRegion))
		{
			hRectStartCol = 0;
			hRectEndCol = hImgWidthTrans;
		}
		else
		{
			HTuple hRow1, hCol1, hRow2, hCol2;
			SmallestRectangle1(hDetRegion, &hRow1, &hCol1, &hRow2, &hCol2);
			hRectStartCol = hCol1;
			hRectEndCol = hCol2;
		}
		// 行坐标根据hLineCountour的行坐标，iLineOffsetDirection以及iLineHeight计算

		HTuple hLineCenterRow, hLineCenterCol, hLineAngle, hLineLength1, hLineLength2;
		SmallestRectangle2Xld(hBaseLineContour, &hLineCenterRow, &hLineCenterCol, &hLineAngle, &hLineLength1, &hLineLength2);

		double offsetDistance = lineHeight * 0.5;

		if (lineOffsetDirection == 0)
		{
			offsetDistance = -offsetDistance;
		}

		hRectStartRow = hLineCenterRow + offsetDistance;
		hRectEndRow = hRectStartRow;

		zkhyPublicClass::measure1D measureTool;

		measureTool.setMeasureRect(hRectStartRow, hRectStartCol, hRectEndRow, hRectEndCol, lineHeight);

		measureTool.setMeasureParams(smoothFactor, edgeStrength, edgePolarity::allPolarity, edgeSelect::allPoints);

		measureTool.genMeasureHandle(hDetImg);

		measureTool.execute(hDetImg);

		measureTool.getResult(hMeasureRows, hMeasureCols, hMeasureAmplitudes, hMeasureDistances);

		// 调试，NDEBUG在gcc和clang中生效，_DEBUG在vs中生效
#if not defined(NDEBUG) || defined(_DEBUG)
		HObject hMeasureRectDisp, hMeasureOutDisp;
		measureTool.getRes2Disp(hMeasureRectDisp, hMeasureOutDisp);
		Sleep(5);
#endif

		return 0;
	}

	// 执行算法
	int wireMeasure::execute()
	{
		// 检查输入参数是否越界
		int ret = checkParams();
		if (ret != 0)
		{
			return ret;
		}

		// 预处理
		try
		{
			preProcess();
		}
		catch (HException& except)
		{
			string errorMsg = except.ErrorMessage().Text();
			LOG.log(errorMsg, alglib::core::err);
			return 13; //预处理异常
		}

		// 测量距离
		try
		{
			measureDistance();
		}
		catch (HException& except)
		{
			string errorMsg = except.ErrorMessage().Text();
			LOG.log(errorMsg, alglib::core::err);
			return 14; //测量距离异常
		}

		// 后处理
		try
		{
			postProcess();
		}
		catch (HException& except)
		{
			string errorMsg = except.ErrorMessage().Text();
			LOG.log(errorMsg, alglib::core::err);
			return 15; //后处理异常
		}

		return 0;
	}

	// 后处理
	int wireMeasure::postProcess()
	{

		HTuple hMeasureTargetNum;
		TupleLength(hMeasureRows, &hMeasureTargetNum);
		int resNum = hMeasureTargetNum.I();

		// 清理变量
		measureResult.measureinfos.clear();
		measureResult.wireEdges.clear();

		if (resNum > 0)
		{
			// 处理测量结果
			double lineWidthMin, lineWidthMax, lineSpacingMin, lineSpacingMax;
			lineWidthMin = lineWidth - lineWidth * lineWidthRange;
			lineWidthMax = lineWidth + lineWidth * lineWidthRange;
			lineSpacingMin = lineSpacing - lineSpacing * lineSpacingRange;
			lineSpacingMax = lineSpacing * 2;

			// 找到的第一条线路不完整，舍弃
			int indexRes = 0;
			while ((lineColor == 0 && hMeasureAmplitudes[indexRes].D() > 0) || (lineColor == 1 && hMeasureAmplitudes[indexRes].D() < 0))
				if (++indexRes >= resNum)
					break;

			// 申请空间
			vector<wireMeasureInfo> measureInfos(resNum);
			vector<vector<cv::Point2f>> wireEdges(resNum);

			HTuple hHomMat2DInvert;
			HomMat2dInvert(hHomMat2D, &hHomMat2DInvert);

			// 计算轮廓
			for (int i = 0; i < resNum; ++i)
			{
				HTuple hRow1, hCol1, hRow2, hCol2;
				AffineTransPixel(hHomMat2DInvert, hMeasureRows[i].D() - lineHeight * 0.32, hMeasureCols[i], &hRow1, &hCol1);
				AffineTransPixel(hHomMat2DInvert, hMeasureRows[i].D() + lineHeight * 0.32, hMeasureCols[i], &hRow2, &hCol2);
				vector<cv::Point2f> edge(4);
				edge[0].x = hCol1.D();
				edge[0].y = hRow1.D();
				edge[1].x = hCol2.D();
				edge[1].y = hRow2.D();
				edge[2] = edge[1];
				edge[3] = edge[0];

				wireEdges[i] = edge;
			}

			int flawInfoIndex = 0;
			for (; indexRes < resNum; ++flawInfoIndex)
			{
				if (indexRes + 1 < resNum)
				{
					measureInfos[flawInfoIndex].wireWidth = hMeasureCols[indexRes + 1].D() - hMeasureCols[indexRes].D();
					// 计算轮廓
					measureInfos[flawInfoIndex].basicInfo.contours.push_back(wireEdges[indexRes]);
					measureInfos[flawInfoIndex].basicInfo.contours.push_back(wireEdges[indexRes + 1]);
				}
				else
					break;

				if (indexRes + 2 < resNum)
				{
					measureInfos[flawInfoIndex].wireSpacing = hMeasureCols[indexRes + 2].D() - hMeasureCols[indexRes].D();
					// 当线间距超过最大允许范围时，将值设置为正常值
					//if (measureInfos[flawInfoIndex].wireSpacing > lineSpacingMax)
					//	measureInfos[flawInfoIndex].wireSpacing = lineSpacing;
					measureInfos[flawInfoIndex].basicInfo.contours.push_back(wireEdges[indexRes + 2]);
				}

				// 查找下一条线路左侧边缘
				for (++indexRes; indexRes < resNum; ++indexRes)
				{
					if ((lineColor == 0 && hMeasureAmplitudes[indexRes].D() < 0) || (lineColor == 1 && hMeasureAmplitudes[indexRes].D() > 0))
						break;
				}
			}

			// 删除多申请的空间
			measureInfos.resize(flawInfoIndex);
			measureInfos.shrink_to_fit();


			measureResult.measureinfos = measureInfos;
			measureResult.wireEdges = wireEdges;
		}

		return 0;
	}

	// 获取测量结果
	void wireMeasure::getResult(sideWireMeasureOutput& output)
	{
		output = measureResult;
	}


	//侧面线路测量算法实现
	int realWireMeasure(const sideWireMeasureInput& input, sideWireMeasureOutput& output)
	{
		if (zkhyPublicFuncHN::isHObjectEmpty(input.inputImage))
			return 16;

		// 解析参数
		AlgParaSideWireMeasure* algParams = (AlgParaSideWireMeasure*)&input.algPara;
		double timeOutThreshold = algParams->d_params.block.dTimeOutThreshold;

		// 开始计时
		DWORD startTime = GetTickCount64();

		wireMeasure cWireMeasure(input);
		int ret = cWireMeasure.execute();
		if (ret != 0)
			return ret;

		cWireMeasure.getResult(output);

		// 计算算法耗时
		DWORD costTime = GetTickCount64() - startTime;
		if (costTime > timeOutThreshold)
			return 2;                   // 算法超时

		return 0;
	};

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------- sideWireMeasure 调用接口-----------------------------//
	int sideWireMeasure(const sideWireMeasureInput& input, sideWireMeasureOutput& output)
	{
		try
		{
			return realWireMeasure(input, output);
		}
		catch (...)
		{
			return 1; //算法未知异常
		}
		return 0;
	}
}
