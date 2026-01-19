//20240116 增加搭边测量算子 --edited by qiuyi
//20240411 侧面搭接测量（connectMeasure）算子：增加了粗定位与搭接区域偏移量、优化搭接、金属区域矩形框、增加金属区域宽高计算和结果赋值	--edited by Wan MaoJia
//20240417 侧面搭接测量（connectMeasure）算子：算子优化	--edited by Wan MaoJia
//20240426 侧面搭接测量（connectMeasure）算子：1. 增加金属区域金线区域筛选宽高。	   --edited by Wan MaoJia

#include "alglibrary/zkhyProjectHuaNan/sideConnectMeasure.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/utils.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;


namespace alglib::ops::zkhyProHN {

	int realConnectMeasure(const SideConnectMeasureInput& input, SideConnectMeasureOutput& output)
	{

		if (!(input.grayImg.IsInitialized()))
		{
			return 1;//输入图像为空
		}
		HObject ho_GrayImage = input.grayImg;



		// Local iconic variables
		HObject  ho_Region1, ho_RegionFillUp1;
		HObject  ho_RegionClosing, ho_ConnectedRegions, ho_SelectedRegions;
		HObject  ho_Contours, ho_Region2, ho_Rectangle2, ho_RegionIntersection;
		HObject  ho_Contour, ho_ImageAffineTrans, ho_lineDown, ho_baseDetectRegion;
		HObject  ho_lineUp, ho_MeasureContours, ho_Cross, ho_Rectangle;
		HObject  ho_RegionIntersection1, ho_RegionIntersection2;
		HObject  ho_locateRect, ho_wireRect, ho_wireRectUnion, ho_detectImage;
		HObject  ho_Region4, ho_RegionClosing1, ho_RegionOpening2;
		HObject  ho_ConnectedRegions2, ho_metalRegions, ho_visionWireCross;
		HObject  ho_visionMetalCross, ho_twoLineRegion, ho_wireRectDilation;
		HObject  ho_goldLineDetectRegion, ho_goldLineDetectImage;
		HObject  ho_Region5, ho_RegionFillUp, ho_goldLineRegion;
		HObject  ho_visionGoldLineCross;
		HObject  ho_RegionOpening3, ho_ConnectedRegions3, ho_RectRegions;

		HObject ho_wireRect1, ho_wireRectUnion1, ho_detectImage1;
		HObject ho_metalRegionDilation, ho_Region6, ho_RectRegionsdilation;
		HObject ho_metalRegionsdilation;


		// Local control variables
		HTuple  hv_WindowHandle, hv_measureObject, hv_strImagePath;
		HTuple  hv_ImageFiles, hv_fileName, hv_FileExists, hv_FileHandle;
		HTuple  hv_umPixel, hv_Seconds1, hv_Index, hv_Width, hv_Height;
		HTuple  hv_baseSegValue, hv_Row13, hv_Column13, hv_Row23;
		HTuple  hv_Column23, hv_RowBegin, hv_ColBegin, hv_RowEnd;
		HTuple  hv_ColEnd, hv_Nr, hv_Nc, hv_Dist, hv_angle, hv_Deg;
		HTuple  hv_HomMat2DIdentity, hv_HomMat2DRotate, hv_lineDownRow1;
		HTuple  hv_lineDownCol1, hv_lineDownRow2, hv_lineDownCol2;
		HTuple  hv_MetrologyHandle, hv_Index1, hv_lineUpParam, hv_MetrolRow;
		HTuple  hv_MetrolColumn, hv_Distance1, hv_Distance2, hv_medianDistance;
		HTuple  hv_maxDistance, hv_minDistance, hv_baseLineOffsetY;
		HTuple  hv_autoLocationRectH, hv_autoLocationRectW, hv_locateRectArea;
		HTuple  hv_locateRectRow, hv_locateRectCol, hv_widthFeatures;
		HTuple  hv_connectRegionW, hv_connectRegionH, hv_col, hv_row;
		HTuple  hv_metalH, hv_wireRectArea, hv_wireRectRow, hv_wireRectCol;
		HTuple  hv_metalRegionArea, hv_metalRegionRow, hv_metalRegionCol;
		HTuple  hv_metalAndwireOffsetX, hv_metalAndwireOffsetY;
		HTuple  hv_lineDownA, hv_lineDownB, hv_lineUpA, hv_lineUpB;
		HTuple  hv_lineUpPoint1C, hv_lineUpPoint1R, hv_lineUpPoint2C;
		HTuple  hv_lineUpPoint2R, hv_twoLineRegionPointR, hv_twoLineRegionPointC;
		HTuple  hv_goldLineArea, hv_goldLineRow, hv_goldLineCol;
		HTuple  hv_metalAndGoldLineOffsetX, hv_metalAndGoldLineOffsetY;
		HTuple	 hv_RectRegionArea, hv_RectRegionRow, hv_RectRegionCol;
		HTuple  hv_Indices1, hv_Indices2, hv_Indices3;

		//新增加修改参数
		HTuple  hv_LapareaValue1, hv_LapareaValue2, hv_lineDownRow3;
		HTuple  hv_LapareaValue3, hv_LapareaValue4, hv_autoLocationRectH1, hv_autoLocationRectW1;
		HTuple  hv_LapareaValue5, hv_LapareaValue6, hv_connectRegionW1, hv_connectRegionH1;
		HTuple  hv_lineUpPoint1L, hv_LapareaValue7, hv_LapareaValue8;
		HTuple  hv_Row4, hv_Column4, hv_Phi4, hv_Length11, hv_Length12;
		HTuple  hv_Row3, hv_Column3, hv_Phi3, hv_Length3, hv_Length4;
		HTuple  hv_metalRegionRow1, hv_metalRegionColumn1, hv_metalRegionRow2, hv_metalRegionColumn2;
		HTuple  hv_RectRegionRow1, hv_RectRegionColumn1, hv_RectRegionRow2, hv_RectRegionColumn2;
		HTuple  hv_metalAndwireOffsetX1, hv_metalAndwireOffsetX2, hv_autoLocationRectH2, hv_autoLocationRectW2;

		AlgParaSideConnectMeasure* algPara = (AlgParaSideConnectMeasure*)&input.algPara;


		hv_lineDownRow1 = algPara->i_params.block.lineDownRow1;				 //金属区域定位的线高度，需要在区域获取位置，值为待测区域纵坐标
		hv_LapareaValue1 = algPara->i_params.block.LapareaValue1;				//金属区域阈值1						
		hv_LapareaValue2 = algPara->i_params.block.LapareaValue2;				//金属区域阈值2
		hv_connectRegionH1 = algPara->i_params.block.connectRegionH1;			//金属区域矩形宽高
		hv_autoLocationRectH = algPara->i_params.block.autoLocationRectH;      //金属区域宽高
		hv_autoLocationRectW = algPara->i_params.block.autoLocationRectW;      //金属区域宽高


		hv_lineDownRow2 = algPara->i_params.block.lineDownRow2;					//金属区域与搭接矩形偏差值
		hv_connectRegionW = algPara->i_params.block.connectRegionW;			//搭接区域矩形的宽高
		hv_connectRegionH = algPara->i_params.block.connectRegionH;			//搭接区域矩形的宽高
		hv_LapareaValue3 = algPara->i_params.block.LapareaValue3;				//搭接区域阈值1
		hv_LapareaValue4 = algPara->i_params.block.LapareaValue4;				//搭接区域阈值2
		hv_autoLocationRectH1 = algPara->i_params.block.autoLocationRectH1;      //搭接区域宽高
		hv_autoLocationRectW1 = algPara->i_params.block.autoLocationRectW1;      //搭接区域宽高


		hv_lineUpPoint1R = algPara->i_params.block.lineUpPoint1R;				//金线区域上高度
		hv_lineUpPoint1L = algPara->i_params.block.lineUpPoint1L;				//金线区域下高度
		hv_LapareaValue7 = algPara->i_params.block.LapareaValue7;			  //金线区域阈值
		hv_LapareaValue8 = algPara->i_params.block.LapareaValue8;			  //金线区域阈值
		hv_autoLocationRectH2 = algPara->i_params.block.autoLocationRectH2;      //金线区域宽高
		hv_autoLocationRectW2 = algPara->i_params.block.autoLocationRectW2;      //金线区域宽高


		//step1:图像旋转成水平状态
		GetImageSize(ho_GrayImage, &hv_Width, &hv_Height);

		//根据拟合直线计算角度，再将产品转正
		TupleAtan((input.linePic.pt2.y - input.linePic.pt1.y) / (input.linePic.pt2.x - input.linePic.pt1.x), &hv_angle);
		TupleDeg(hv_angle, &hv_Deg);
		//rotate_image 耗时比affine_trans_image长
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		HomMat2dRotate(hv_HomMat2DIdentity, hv_angle, hv_Width / 2, hv_Height / 2, &hv_HomMat2DRotate);
		AffineTransImage(ho_GrayImage, &ho_ImageAffineTrans, hv_HomMat2DRotate, "constant", "false");


		//step2:自动定位，构建搭接检测区域，测量金属区域与金线区域的中心偏差(▲x和▲y)
		//生成一个长矩形，与线路相交，得到金属区域

		GenRectangle1(&ho_Rectangle, (input.linePic.pt2.y + input.linePic.pt1.y) / 2 + hv_lineDownRow1 - hv_connectRegionH1,
			0, (input.linePic.pt2.y + input.linePic.pt1.y) / 2 + hv_lineDownRow1 + hv_connectRegionH1, hv_Width);
		Threshold(ho_ImageAffineTrans, &ho_Region1, hv_LapareaValue1, hv_LapareaValue2);
		Intersection(ho_Rectangle, ho_Region1, &ho_RegionIntersection1);
		ClosingCircle(ho_RegionIntersection1, &ho_RegionIntersection2, 10);

		ClosingRectangle1(ho_RegionIntersection2, &ho_RegionIntersection2, 10, 1);
		OpeningRectangle1(ho_RegionIntersection2, &ho_RegionIntersection2, 30, 1);
		ClosingCircle(ho_RegionIntersection2, &ho_RegionIntersection2, 25);
		Connection(ho_RegionIntersection2, &ho_metalRegions);
		SelectShape(ho_metalRegions, &ho_metalRegions, (HTuple("width").Append("height")),
			"and", hv_autoLocationRectW.TupleConcat(hv_autoLocationRectH), (HTuple(99999).Append(99999)));
		SortRegion(ho_metalRegions, &ho_metalRegions, "first_point", "true", "column");
		AreaCenter(ho_metalRegions, &hv_metalRegionArea, &hv_metalRegionRow, &hv_metalRegionCol);
		SmallestRectangle1(ho_metalRegions, &hv_metalRegionRow1, &hv_metalRegionColumn1, &hv_metalRegionRow2, &hv_metalRegionColumn2);
		SmallestRectangle2(ho_metalRegions, &hv_Row4, &hv_Column4, &hv_Phi4, &hv_Length11, &hv_Length12);
		DilationRectangle1(ho_metalRegions, &ho_metalRegionsdilation, 30, 450);

		TupleFind(hv_metalRegionArea, 0, &hv_Indices1);
		if (hv_Indices1 != (-1))
		{
			hv_metalRegionArea[hv_Indices1] = 99999;
		}

		TupleFind(hv_metalRegionRow, 0, &hv_Indices2);
		if (hv_Indices2 != (-1))
		{
			hv_metalRegionRow[hv_Indices2] = 99999;
		}

		TupleFind(hv_metalRegionCol, 0, &hv_Indices3);
		if (hv_Indices3 != (-1))
		{
			hv_metalRegionCol[hv_Indices3] = 99999;
		}

		//获取搭接区域区域
		hv_col = hv_metalRegionCol;
		//hv_row = hv_lineDownRow1;

		TupleGenConst(hv_metalRegionCol.TupleLength(), (input.linePic.pt2.y + input.linePic.pt1.y) / 2 + hv_lineDownRow1, &hv_row);

		GenRectangle1(&ho_wireRect, hv_row - hv_connectRegionH - hv_lineDownRow2, hv_col - hv_Length11 - (hv_connectRegionW / 2.0),
			hv_row + hv_connectRegionH - hv_lineDownRow2, hv_col + hv_Length11 + (hv_connectRegionW / 2.0));
		AreaCenter(ho_wireRect, &hv_wireRectArea, &hv_wireRectRow, &hv_wireRectCol);
		Union1(ho_wireRect, &ho_wireRectUnion);
		ReduceDomain(ho_ImageAffineTrans, ho_wireRectUnion, &ho_detectImage);
		Threshold(ho_detectImage, &ho_Region4, hv_LapareaValue3, hv_LapareaValue4);
		OpeningRectangle1(ho_Region4, &ho_Region4, 50, 1);
		ClosingRectangle1(ho_Region4, &ho_RegionClosing1, 10, 1);
		OpeningRectangle1(ho_RegionClosing1, &ho_RegionOpening2, 30, 1);
		ClosingCircle(ho_RegionOpening2, &ho_RegionOpening2, 25);
		Connection(ho_RegionOpening2, &ho_ConnectedRegions2);
		SelectShape(ho_ConnectedRegions2, &ho_RectRegions, (HTuple("width").Append("height")),
			"and", hv_autoLocationRectW1.TupleConcat(hv_autoLocationRectH1), (HTuple(99999).Append(99999)));
		SortRegion(ho_RectRegions, &ho_RectRegions, "first_point", "true", "column");

		//新增加交集、与外接正矩形
		Intersection(ho_metalRegionsdilation, ho_RectRegions, &ho_RectRegions);
		SmallestRectangle1(ho_RectRegions, &hv_RectRegionRow1, &hv_RectRegionColumn1, &hv_RectRegionRow2, &hv_RectRegionColumn2);

		AreaCenter(ho_RectRegions, &hv_RectRegionArea, &hv_RectRegionRow, &hv_RectRegionCol);

		TupleFind(hv_RectRegionArea, 0, &hv_Indices1);
		if (hv_Indices1 != (-1))
		{
			hv_RectRegionArea[hv_Indices1] = 99999;
		}

		TupleFind(hv_RectRegionRow, 0, &hv_Indices2);
		if (hv_Indices2 != (-1))
		{
			hv_RectRegionRow[hv_Indices2] = 99999;
		}

		TupleFind(hv_RectRegionCol, 0, &hv_Indices3);
		if (hv_Indices3 != (-1))
		{
			hv_RectRegionCol[hv_Indices3] = 99999;
		}


		if (0 != ((hv_metalRegionRow.TupleLength()) != (hv_RectRegionRow.TupleLength())))
		{
			return 2; //'缺少金属搭接区域'
		}
		//计算金属区域与线路区域的中心偏差(▲x和▲y)
		//左右角点计算方法
		hv_metalAndwireOffsetX1 = (hv_metalRegionColumn1 - hv_RectRegionColumn1).TupleAbs();
		hv_metalAndwireOffsetX2 = (hv_metalRegionColumn2 - hv_RectRegionColumn2).TupleAbs();
		TupleMax2(hv_metalAndwireOffsetX1, hv_metalAndwireOffsetX2, &hv_metalAndwireOffsetX);

		//中心点计算方法
		//hv_metalAndwireOffsetX = (hv_metalRegionCol - hv_RectRegionCol).TupleAbs();
		hv_metalAndwireOffsetY = (hv_metalRegionRow - hv_RectRegionRow).TupleAbs();

		//显示搭接区域的中心点
		GenCrossContourXld(&ho_visionWireCross, hv_RectRegionRow, hv_RectRegionCol, 50, 0);
		GenCrossContourXld(&ho_visionMetalCross, hv_metalRegionRow, hv_metalRegionCol, 50, 0);

		//封装金属区域与线路区域的中心偏差的缺陷信息
		for (int i = 0; i < hv_metalAndwireOffsetX.TupleLength(); i++)
		{
			MeasureInfoStruct measureinfostruct;
			measureinfostruct.extendInfo.distanceX = hv_metalAndwireOffsetX[i];
			measureinfostruct.extendInfo.distanceY = hv_metalAndwireOffsetY[i];

			HObject ObjectSelected1;
			SelectObj(ho_metalRegions, &ObjectSelected1, i + 1);
			HObject ObjectSelected2, ho_RegionUnion;
			SelectObj(ho_RectRegions, &ObjectSelected2, i + 1);

			LineStruct line1, line2;
			vector<Point2f> contour1, contour2;
			vector<vector<Point2f>> contours1;

			line1.pt1 = Point2f(hv_metalRegionRow[i] - 5, hv_metalRegionCol[i] - 5);
			line1.pt2 = Point2f(hv_metalRegionRow[i] + 5, hv_metalRegionCol[i] + 5);
			line2.pt1 = Point2f(hv_RectRegionRow[i] - 5, hv_RectRegionCol[i] - 5);
			line2.pt2 = Point2f(hv_RectRegionRow[i] + 5, hv_RectRegionCol[i] + 5);

			convertLine2Contour(line1, contour1);
			convertLine2Contour(line2, contour2);
			Union2(ObjectSelected1, ObjectSelected2, &ho_RegionUnion);
			zkhyPublicFuncHN::genContourFromRegion(ho_RegionUnion, contours1);
			contours1.push_back(contour1);
			contours1.push_back(contour2);
			measureinfostruct.basicInfo.contours = contours1;

			//增加金属区域宽高输出
			measureinfostruct.basicInfo.width = hv_Length11[i];
			measureinfostruct.basicInfo.height = hv_Length12[i];

			//增加面积输出
			//搭接面积
			HTuple hv_ObjectSelected1Area, hv_ObjectSelected1Row, hv_ObjectSelected1Col;
			AreaCenter(ObjectSelected1, &hv_ObjectSelected1Area, &hv_ObjectSelected1Row, &hv_ObjectSelected1Col);
			measureinfostruct.extendInfo.distanceMean = hv_ObjectSelected1Area;
			//金属面积
			HTuple hv_ObjectSelected2Area, hv_ObjectSelected2Row, hv_ObjectSelected2Col;
			AreaCenter(ObjectSelected2, &hv_ObjectSelected2Area, &hv_ObjectSelected2Row, &hv_ObjectSelected1Col);
			measureinfostruct.extendInfo.distanceMin = hv_ObjectSelected2Area;
			output.measureinfostruct.push_back(measureinfostruct);
		}

		//step4:构建金线测量区域，测量金线区域与金属区域的偏差▲x
		GenRectangle1(&ho_twoLineRegion, (input.linePic.pt2.y + input.linePic.pt1.y) / 2 - hv_lineUpPoint1R,
			0, (input.linePic.pt2.y + input.linePic.pt1.y) / 2 + hv_lineUpPoint1L, hv_Width);


		//线路区域与两直线区域相交得到金线检测区域
		//::DilationRectangle1(ho_metalRegions, &ho_metalRegionDilation, 1, 150);
		Intersection(ho_twoLineRegion, ho_metalRegionsdilation, &ho_goldLineDetectRegion);
		ReduceDomain(ho_ImageAffineTrans, ho_goldLineDetectRegion, &ho_goldLineDetectImage);

		Threshold(ho_goldLineDetectImage, &ho_Region6, hv_LapareaValue7, hv_LapareaValue8);
		FillUp(ho_Region6, &ho_RegionFillUp);
		OpeningRectangle1(ho_RegionFillUp, &ho_goldLineRegion, 30, 30);
		Connection(ho_goldLineRegion, &ho_goldLineRegion);
		SelectShape(ho_goldLineRegion, &ho_goldLineRegion, (HTuple("width").Append("height")),
			"and", hv_autoLocationRectW2.TupleConcat(hv_autoLocationRectH2), (HTuple(99999).Append(99999)));
		SortRegion(ho_goldLineRegion, &ho_goldLineRegion, "first_point", "true", "column");

		Intersection(ho_metalRegionsdilation, ho_goldLineRegion, &ho_goldLineRegion);

		AreaCenter(ho_goldLineRegion, &hv_goldLineArea, &hv_goldLineRow, &hv_goldLineCol);

		TupleFind(hv_goldLineArea, 0, &hv_Indices1);
		if (hv_Indices1 != (-1))
		{
			hv_goldLineArea[hv_Indices1] = 99999;
		}

		TupleFind(hv_goldLineRow, 0, &hv_Indices2);
		if (hv_Indices2 != (-1))
		{
			hv_goldLineRow[hv_Indices2] = 99999;
		}

		TupleFind(hv_goldLineCol, 0, &hv_Indices3);
		if (hv_Indices3 != (-1))
		{
			hv_goldLineCol[hv_Indices3] = 99999;
		}


		if (0 != ((hv_metalRegionRow.TupleLength()) != (hv_goldLineRow.TupleLength())))
		{
			return 3;//'缺少金线区域'
		}
		//计算金属搭接区域与金线区域的中心偏差(▲x和▲y)
		hv_metalAndGoldLineOffsetX = (hv_metalRegionCol - hv_goldLineCol).TupleAbs();
		hv_metalAndGoldLineOffsetY = (hv_metalRegionRow - hv_goldLineRow).TupleAbs();
		GenCrossContourXld(&ho_visionGoldLineCross, hv_goldLineRow, hv_goldLineCol, 50, 0);


		//封装金属搭接区域与金线区域的缺陷信息
		for (int i = 0; i < hv_metalAndGoldLineOffsetX.TupleLength(); i++)
		{
			MeasureInfoStruct measureinfostruct;
			HObject ObjectSelected1;
			SelectObj(ho_metalRegions, &ObjectSelected1, i + 1);
			HObject ObjectSelected3, ho_RegionUnion;
			SelectObj(ho_goldLineRegion, &ObjectSelected3, i + 1);

			LineStruct line1, line2;
			vector<Point2f> contour1, contour2;
			vector<vector<Point2f>> contours1;

			line1.pt1 = Point2f(hv_metalRegionRow[i] - 5, hv_metalRegionCol[i] - 5);
			line1.pt2 = Point2f(hv_metalRegionRow[i] + 5, hv_metalRegionCol[i] + 5);
			line2.pt1 = Point2f(hv_goldLineRow[i] - 5, hv_goldLineCol[i] - 5);
			line2.pt2 = Point2f(hv_goldLineRow[i] + 5, hv_goldLineCol[i] + 5);

			convertLine2Contour(line1, contour1);
			convertLine2Contour(line2, contour2);
			Union2(ObjectSelected1, ObjectSelected3, &ho_RegionUnion);
			zkhyPublicFuncHN::genContourFromRegion(ho_RegionUnion, contours1);
			contours1.push_back(contour1);
			contours1.push_back(contour2);
			measureinfostruct.basicInfo.contours = contours1;

			//增加面积输出
			//金线面积
			HTuple hv_ObjectSelected3Area, hv_ObjectSelected3Row, hv_ObjectSelected3Col;
			AreaCenter(ObjectSelected3, &hv_ObjectSelected3Area, &hv_ObjectSelected3Row, &hv_ObjectSelected3Col);
			output.measureinfostruct[i].extendInfo.distanceMax = hv_ObjectSelected3Area;
			for (int j = 0; j < contours1.size(); j++) {
				output.measureinfostruct[i].basicInfo.contours.push_back(contours1[j]);
			}
			output.measureinfostruct[i].extendInfo.distanceZ = hv_metalAndGoldLineOffsetX[i];
			output.measureinfostruct[i].extendInfo.distanceR = hv_metalAndGoldLineOffsetY[i];
		}

		return 0;
	}


}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------- connectMeasure-----------------------------------------//
//函数说明：侧面搭接测量
//SideConnectMeasureInput &input;				输入：待测图像
//SideConnectMeasureOutput &output; 			输出：金线区域与搭接区域的偏差▲x，金属区域与线路区域的中心偏差(▲x和▲y)
	int sideConnectMeasure(const SideConnectMeasureInput& input, SideConnectMeasureOutput& output)
	{
		try
		{
			return realConnectMeasure(input, output);
		}
		catch (...)
		{
			return 2; //算法未知异常
		}
		return 0;
	}
}
