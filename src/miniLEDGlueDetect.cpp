#include "alglibrary/zkhyProjectHuaNan/miniLEDGlueDetect.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace zkhyPublicFuncHN;

namespace alglib::ops::zkhyProHN {


	//--------------------------------------miniLEDGlueRegionsToDefectStruct--------------------------------------------------//
	//功能：计算输入区域的面积、角度、外接矩形以及轮廓等基本特征数据
	//输入：defectRegion 缺陷区域，输入单个或者多个缺陷区域
	//		   useContour 是否计算缺陷轮廓，true为计算轮廓，false为不计算轮廓，默认为计算
	//		   useRotateRect 计算区域的外接旋转矩形还是正矩形，true为旋转矩形，false为正矩形，默认为旋转矩形
	//		   homMat2D 坐标变换矩阵，用于对缺陷区域进行映射，默认为空
	//输出：outFlaw 缺陷结构体
	void miniLEDGlueRegionsToDefectStruct(HObject& defectRegions, vector<FlawInfoStruct>& outFlaw, const bool& useContour = true, const bool& useRotateRect = true,const HTuple& homMat2D = HTuple());

	// Local procedures 
	void findBottomGlue(HObject ho_S1V1, HObject ho_RegionTrans1, HObject* ho_bottomGlueRegion,
		HTuple hv_gluebottomDilationRadius, HTuple hv_bottomGlueThreshold)
	{

		// Local iconic variables
		HObject  ho_RegionDilation1, ho_findBottomGlueRegion;
		HObject  ho_S1V1Reduced, ho_Region2, ho_RegionFillUp1, ho_RegionClosing1, ho_RegionOpening1;
		HObject  ho_ConnectedRegions3, ho_SelectedRegions4;

		// Local control variables
		HTuple  hv_Area, hv_Row, hv_Column;
		HTuple  hv_Mean, hv_minArea;

		if (HDevWindowStack::IsOpen())
			DispObj(ho_S1V1, HDevWindowStack::GetActive());
		DilationCircle(ho_RegionTrans1, &ho_RegionDilation1, hv_gluebottomDilationRadius);
		Union1(ho_RegionDilation1, &ho_findBottomGlueRegion);
		ReduceDomain(ho_S1V1, ho_findBottomGlueRegion, &ho_S1V1Reduced);

		//2.1 定位底部胶水区域
		Threshold(ho_S1V1Reduced, &ho_Region2, 0, hv_bottomGlueThreshold);
		FillUp(ho_Region2, &ho_RegionFillUp1);
		ClosingCircle(ho_RegionFillUp1, &ho_RegionClosing1, 6);			// 后续看是否需要开放出来，因为开放参数太多了，暂时不想开放这个
		OpeningCircle(ho_RegionClosing1, &ho_RegionOpening1, 6);

		Connection(ho_RegionOpening1, &ho_ConnectedRegions3);
		//2.2 自适应筛选不符合的胶水区域
		AreaCenter(ho_ConnectedRegions3, &hv_Area, &hv_Row, &hv_Column);
		TupleMean(hv_Area, &hv_Mean);

		hv_minArea = hv_Mean - hv_Mean / 3;


		SelectShape(ho_ConnectedRegions3, &ho_SelectedRegions4, "area", "and", hv_minArea, 99999999);
		Union1(ho_SelectedRegions4, &(*ho_bottomGlueRegion));

		return;
	}

	void findIcGlueRegion(HObject ho_S1V3, HObject* ho_SelectedRegions1, HObject* ho_RegionTrans1,
		HTuple hv_glueLowGray, HTuple hv_glueHightGray, HTuple hv_icGlueOpeningSize)
	{

		// Local iconic variables
		HObject  ho_Region, ho_RegionFillUp, ho_RegionClosing, ho_RegionOpening;
		HObject  ho_ConnectedRegions, ho_SelectedRegions;
		// Local control variables
		HTuple  hv_Area, hv_Row, hv_Column, hv_Mean, hv_minArea, hv_maxArea;

		Threshold(ho_S1V3, &ho_Region, hv_glueLowGray, hv_glueHightGray);
		FillUp(ho_Region, &ho_RegionFillUp);
		OpeningCircle(ho_RegionFillUp, &ho_RegionOpening, hv_icGlueOpeningSize);
		// 因芯片两边的胶水有时会区分开，再加一个闭运算；

		ClosingCircle(ho_RegionOpening, &ho_RegionClosing, hv_icGlueOpeningSize);
		Connection(ho_RegionClosing, &ho_ConnectedRegions);
		AreaCenter(ho_ConnectedRegions, &hv_Area, &hv_Row, &hv_Column);
		TupleMean(hv_Area, &hv_Mean);
		hv_minArea = hv_Mean - (hv_Mean / 4);
		hv_maxArea = hv_Mean + (hv_Mean / 4);

		SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", hv_minArea, hv_maxArea);
		SelectShape(ho_SelectedRegions, &(*ho_SelectedRegions1), "outer_radius", "and", 15, 99999);
		ShapeTrans((*ho_SelectedRegions1), &(*ho_RegionTrans1), "outer_circle");

		return;
	}

	void findUpGlueRegion(HObject ho_bottomGlueRegion1, HObject ho_ICHeightImage, HObject* ho_RegionOpening,
		HTuple hv_glueUpErosionSize, HTuple hv_upGlueMinHight, HTuple hv_glueUpOpeningSize)
	{

		// Local iconic variables
		HObject  ho_RegionErosion, ho_S1V0Reduced, ho_upGlueRegionThreshold;
		HObject  ho_ConnectedRegions2, ho_SelectedRegions3;

		// Local control variables
		HTuple  hv_Area1, hv_Row2, hv_Column2, hv_Mean2;


		ErosionCircle(ho_bottomGlueRegion1, &ho_RegionErosion, hv_glueUpErosionSize);
		ReduceDomain(ho_ICHeightImage, ho_RegionErosion, &ho_S1V0Reduced);
		Threshold(ho_S1V0Reduced, &ho_upGlueRegionThreshold, hv_upGlueMinHight, 9999999);
		Connection(ho_upGlueRegionThreshold, &ho_ConnectedRegions2);
		//3.1 自适应筛选顶部胶水区域
		AreaCenter(ho_ConnectedRegions2, &hv_Area1, &hv_Row2, &hv_Column2);
		TupleMean(hv_Area1, &hv_Mean2);
		SelectShape(ho_ConnectedRegions2, &ho_SelectedRegions3, "area", "and", hv_Mean2,
			999999999);
		OpeningCircle(ho_SelectedRegions3, &(*ho_RegionOpening), hv_glueUpOpeningSize);

		return;
	}


	void findS1V1UpGlueRegion(HObject ho_bottomGlueRegion, HObject ho_S1V1, HObject* ho_ConnectedRegions,
		HTuple hv_glueUpErosionSize, HTuple hv_upGlueMinHight, HTuple hv_glueUpOpeningSize)
	{

		// Local iconic variables
		HObject  ho_RegionErosion, ho_ImageReducedS1V1;
		HObject  ho_Region, ho_RegionErosion1, ho_RegionFillUp;

		ErosionCircle(ho_bottomGlueRegion, &ho_RegionErosion, hv_glueUpErosionSize);
		ReduceDomain(ho_S1V1, ho_RegionErosion, &ho_ImageReducedS1V1);
		Threshold(ho_ImageReducedS1V1, &ho_Region, 0, hv_upGlueMinHight);


		OpeningCircle(ho_Region, &(*ho_ConnectedRegions), hv_glueUpOpeningSize);
		FillUp(*ho_ConnectedRegions, &ho_RegionFillUp);
		ErosionCircle(ho_RegionFillUp, &ho_RegionErosion1, hv_glueUpOpeningSize / 2);

		HObject ho_ConnectedRegionsOrg;
		HTuple hv_Area, hv_Row, hv_Column, hv_Median, hv_minArea, hv_maxArea;

		Connection((ho_RegionErosion1), &ho_ConnectedRegionsOrg);

		AreaCenter(ho_ConnectedRegionsOrg, &hv_Area, &hv_Row, &hv_Column);
		TupleMedian(hv_Area, &hv_Median);
		hv_minArea = hv_Median - (hv_Median / 3);
		hv_maxArea = hv_Median + (hv_Median / 3);
		SelectShape(ho_ConnectedRegionsOrg, &(*ho_ConnectedRegions), "area", "and", hv_minArea, hv_maxArea);



		return;
	}



	//----------------------------------------------miniLEDGlueDetect--------------------------//
	//功能：miniLED胶水检测
	int realminiLEDGlueDetect(const MiniLEDGlueDetectInput& input, MiniLEDGlueDetectOutput& output)
	{
		HObject ho_baseDeepImg, ho_baseGrayImg, ho_lensDeepImg, ho_lensGrayImg;

		// @gxl, 输入Mat格式图像替换为HObject格式
		//S1V0：基板高度图，S1V1：基板灰度图；S1V2：芯片高度图，S1V3：芯片灰度图，
		ho_baseDeepImg = input.baseDeepImg;
		ho_baseGrayImg = input.baseGrayImg;
		ho_lensDeepImg = input.lensDeepImg;
		ho_lensGrayImg = input.lensGrayImg;

		// 判断输入图像是否为空，目前只使用两幅深度图以及基板的灰度图
		if (isHObjectEmpty(ho_baseDeepImg) || isHObjectEmpty(ho_lensDeepImg) || isHObjectEmpty(ho_baseGrayImg))
		{
			return 1;
		}

#pragma region defVariable

		// Local iconic variables
		HObject  ho_orgIcGlueRegion;
		HObject  ho_RegionTrans1, ho_bottomGlueRegion, ho_upGlueRegion;
		HObject  ho_upGlueRegionErosion;
		HObject  ho_ObjectSelected11, ho_Circle, ho_RegionDilation2;
		HObject  ho_RegionDilation3, ho_bottomRingRegion, ho_ringRegion;
		HObject  ho_Rectangle, ho_RegionMoved, ho_ImageReduced, ho_ImagePart;
		HObject  ho_ImageMean, ho_ImageSurface, ho_ObjectSelected2;
		HObject  ho_RegionMoved1, ho_ConnectedRegions, ho_RegionDifference1;
		HObject  ho_RegionOpening, ho_RegionIntersection1, ho_icRegions;
		HObject  ho_ConnectedRegions2;

		// Local control variables
		HTuple  hv_glueLowGray, hv_glueHightGray, hv_icGlueOpeningSize;
		HTuple  hv_pixDist, hv_gluebottomDilationRadius, hv_bottomGlueThreshold;
		HTuple  hv_glueUpErosionSize, hv_upGlueMinHight, hv_glueUpOpeningSize;
		HTuple  hv_fitbackgroundHeightMode, hv_icOpeningSize, hv_IDidx;
		HTuple  hv_Index2, hv_HeightFileName, hv_RadiusFileName;
		HTuple  hv_offsetFileName, hv_Files, hv_Index1, hv_imgPath;
		HTuple  hv_BaseName, hv_Extension, hv_Directory, hv_newBaseName;
		HTuple  hv_Number2;
		HTuple  hv_glue_top, hv_k, hv_Rows11, hv_Columns11, hv_Grayval11;
		HTuple  hv_Median11, hv_Row3, hv_Column3, hv_Radius, hv_Mean4;
		HTuple  hv_Width, hv_Height, hv_background_height, hv_useFit;
		HTuple  hv_Index, hv_Row11, hv_Column11, hv_Row21, hv_Column21;
		HTuple  hv_tempRow, hv_tempCol, hv_Width1, hv_Height1, hv_Alpha;
		HTuple  hv_Beta, hv_Gamma, hv_Median12, hv_Rows2, hv_Columns2;
		HTuple  hv_Grayval2, hv_Mean, hv_ic_height, hv_ic_height_new;
		HTuple  hv_bottomGlueRadius_new, hv_Area;
		HTuple  hv_icCenterRow, hv_icCenterColumn, hv_Area1, hv_glueCenterRow;
		HTuple  hv_glueCenterColumn, hv_rowOffset, hv_colOffset;
		HTuple  hv_glueToLensOffset, hv_glueToLensOffset_new;
#pragma endregion


		AlgParaMiniLEDGlueDetect* algParam = (AlgParaMiniLEDGlueDetect*)&input.algPara;

		//输入参数

		//根据v3灰度图，定位芯片和胶水区域
		hv_glueLowGray = algParam->i_params.block.glueLowGray;	// 30;
		hv_glueHightGray = algParam->i_params.block.glueHightGray;	// 150
		hv_icGlueOpeningSize = algParam->i_params.block.icGlueOpeningSize;	// 10
		//找底部胶水区域的参数
		hv_gluebottomDilationRadius = algParam->i_params.block.gluebottomDilationRadius;	// 100
		hv_bottomGlueThreshold = algParam->i_params.block.bottomGlueThreshold;	// 70
		//找胶水顶点区域 相关参数
		hv_glueUpErosionSize = algParam->i_params.block.glueUpErosionSize;	// 50
		hv_upGlueMinHight = algParam->i_params.block.upGlueMinHight;	// 30000
		hv_glueUpOpeningSize = algParam->i_params.block.glueUpOpeningSize;	// 10
		//胶水高度
		HTuple dilationRing = algParam->i_params.block.dilationRing;	// 2
		hv_fitbackgroundHeightMode = algParam->i_params.block.fitbackgroundHeightMode;	// 2
		//偏心距
		hv_icOpeningSize = algParam->i_params.block.icOpeningSize;	// 8
		bool isCnt = algParam->i_params.block.isContours;	// 0.0068
		//Z轴的像素当量
		//hv_pixDist = algParam->d_params.block.mmPixleZ;	// 0.0068
		hv_pixDist = 0.0068;




		//输出参数
		//1.缺陷区域；2.输出的中间可视化区域：S1V3的芯片胶水区域、S1V1的底部胶水区域、S1V1的顶部胶水区域、S1V1的基板圆环、ic芯片区域



		//=========  第1找芯片并拟合外接圆，第2步芯片外扩找底部胶水区域，第3步去S1V2找顶部胶水区域  这三步 放到外面的获取区域及轮廓算子 实现   ===================

		//*1.根据基板灰度图S1V3，粗定位芯片区域   这一步后续放到检测软件中通过算子组合实现
		findIcGlueRegion(ho_lensGrayImg, &ho_orgIcGlueRegion, &ho_RegionTrans1, hv_glueLowGray, hv_glueHightGray, hv_icGlueOpeningSize);


		//2.粗定位芯片区域->外扩->定位底部胶水区域            这一步后续放到检测软件中通过算子组合实现
		findBottomGlue(ho_baseGrayImg, ho_RegionTrans1, &ho_bottomGlueRegion, hv_gluebottomDilationRadius,
			hv_bottomGlueThreshold);
		//2.3 todo  拟合底部胶水圆心，后续计算同心度

		//=========================================    第1、2步 输出 bottomGlueRegion 底部胶水区域 ========================


		//3.底部胶水内缩，进一步定位黑色的顶部胶水区域           这一步后续放到检测软件中通过算子组合实现
		//findUpGlueRegion(ho_bottomGlueRegion, ho_baseDeepImg, &ho_upGlueRegion, hv_glueUpErosionSize, hv_upGlueMinHight, hv_glueUpOpeningSize);
		//=========================================    第3步 输出 RegionErosion2 顶部胶水区域 ========================


		//3.1 换一个思路，在S1V0找顶部区域，
		findS1V1UpGlueRegion(ho_bottomGlueRegion, ho_baseGrayImg, &ho_upGlueRegion, hv_glueUpErosionSize, hv_upGlueMinHight, hv_glueUpOpeningSize);
		//=========================================    第3步 输出 RegionErosion2 顶部胶水区域 ========================


		//*有可能胶水拍到一半，芯片有但胶水的下半部分没有了，已胶水做个基准，提取新的 芯片胶水区域
		HObject ho_icGlueRegion, ho_tempRegion;
		Intersection(ho_orgIcGlueRegion, ho_bottomGlueRegion, &ho_tempRegion);
		SelectShape(ho_tempRegion, &ho_icGlueRegion, "area", "and", 2, 999999999);


		HObject ho_icGlueRegionU;
		Union1(ho_icGlueRegion, &ho_icGlueRegionU);
		output.icGlueRegion = ho_icGlueRegionU;				 // 提前输出



		//做一个判定，三次找出区域，数量需要一致

		HTuple  hv_upGlueRegionRegionNum, hv_icGlueRegionNum, hv_bottomGlueRegionNum;
		HObject ho_temp_bottomGlueRegion;
		CountObj(ho_icGlueRegion, &hv_icGlueRegionNum);
		Connection(ho_bottomGlueRegion, &ho_temp_bottomGlueRegion);
		CountObj(ho_temp_bottomGlueRegion, &hv_bottomGlueRegionNum);
		CountObj(ho_upGlueRegion, &hv_upGlueRegionRegionNum);
		if (hv_icGlueRegionNum.D() != hv_bottomGlueRegionNum.D())
			return 5;
		if (hv_icGlueRegionNum.D() != hv_upGlueRegionRegionNum.D())
			return 6;



		HObject ho_upGlueRegionU;
		Union1(ho_upGlueRegion, &ho_upGlueRegionU);
		output.upGlueRegion = ho_upGlueRegionU;	 // 提前输出



		//4.统计顶部胶水高度：顶部胶水区域——>内缩再统计；统计方法：均值，或最大值（排序，剔除前5或其它方法）
		HTuple hv_Row, hv_Column, hv_Radius1;
		InnerCircle(ho_upGlueRegion, &hv_Row, &hv_Column, &hv_Radius1);
		//region_features (temp_bottomGlueRegion, 'inner_radius', Value)
		GenCircle(&ho_upGlueRegionErosion, hv_Row, hv_Column, hv_Radius1);

		CountObj(ho_upGlueRegionErosion, &hv_Number2);
		hv_glue_top = HTuple();
		{
			HTuple end_val97 = hv_Number2;
			HTuple step_val97 = 1;
			for (hv_k = 1; hv_k.Continue(end_val97, step_val97); hv_k += step_val97)
			{
				SelectObj(ho_upGlueRegionErosion, &ho_ObjectSelected11, hv_k);
				GetRegionPoints(ho_ObjectSelected11, &hv_Rows11, &hv_Columns11);
				GetGrayval(ho_baseDeepImg, hv_Rows11, hv_Columns11, &hv_Grayval11);
				TupleMedian(hv_Grayval11, &hv_Median11);
				hv_glue_top = hv_glue_top.TupleConcat(hv_Median11);
			}
			TupleAdd(hv_glue_top, 0.0000001, &hv_glue_top);
		}


		//5. 拟合平面：方法一：直接拟合平面；方法二：确保和胶水同心，在基板找一个圆环，分析数据（均值滤波，或排序剔除前5）
		//找环形， 确保同心，对顶部胶水区域 upGlueRegionErosion  进行求外接圆，再膨胀
		SmallestCircle(ho_upGlueRegionErosion, &hv_Row3, &hv_Column3, &hv_Radius);
		TupleMean(hv_Radius, &hv_Mean4);
		GenCircle(&ho_Circle, hv_Row3, hv_Column3, hv_Radius);
		DilationCircle(ho_Circle, &ho_RegionDilation2, hv_Mean4 * dilationRing);			 //  圆环参数估计要放开
		DilationCircle(ho_Circle, &ho_RegionDilation3, hv_Mean4 * dilationRing * 2);
		Difference(ho_RegionDilation3, ho_RegionDilation2, &ho_bottomRingRegion);


		HObject ho_bottomRingRegionU;
		Union1(ho_bottomRingRegion, &ho_bottomRingRegionU);
		output.bottomRingRegion = ho_bottomRingRegionU;	 // 提前输出部分结果

		//5.1 拟合平面
		GetImageSize(ho_baseDeepImg, &hv_Width, &hv_Height);
		HObject Domain;
		GetDomain(ho_baseDeepImg, &Domain);
		hv_background_height = HTuple();
		hv_useFit = 1;
		if (0 != hv_useFit)
		{
			{
				HTuple end_val123 = hv_Number2;
				HTuple step_val123 = 1;
				for (hv_Index = 1; hv_Index.Continue(end_val123, step_val123); hv_Index += step_val123)
				{
					SelectObj(ho_bottomRingRegion, &ho_ringRegion, hv_Index);

					Intersection(Domain, ho_ringRegion, &ho_ringRegion);
					if (0 != (hv_fitbackgroundHeightMode == 0))
					{
						//创建外接矩形抠图
						SmallestRectangle1(ho_ringRegion, &hv_Row11, &hv_Column11, &hv_Row21, &hv_Column21);
						GenRectangle1(&ho_Rectangle, hv_Row11, hv_Column11, hv_Row21, hv_Column21);
						hv_tempRow = hv_Row21 - hv_Row11;
						hv_tempCol = hv_Column21 - hv_Column11;
						//移动圆环区域
						MoveRegion(ho_ringRegion, &ho_RegionMoved, -hv_Row11, -hv_Column11);
						//裁剪基板图片抠图，对高度图进行均值滤波
						ReduceDomain(ho_baseDeepImg, ho_Rectangle, &ho_ImageReduced);
						CropDomain(ho_ImageReduced, &ho_ImagePart);
						MeanImage(ho_ImagePart, &ho_ImageMean, 1, 1);
						GetImageSize(ho_ImageMean, &hv_Width1, &hv_Height1);
						//拟合基板平面
						FitSurfaceFirstOrder(ho_RegionMoved, ho_ImageMean, "regression", 5, 2,
							&hv_Alpha, &hv_Beta, &hv_Gamma);

						GenImageSurfaceFirstOrder(&ho_ImageSurface, "real", hv_Alpha, hv_Beta,
							hv_Gamma, hv_Height1 / 2, hv_Width1 / 2, hv_Width1, hv_Height1);
						//拿到顶部胶水区域的中心区域，在基板拟合的高度图里面进行统计基板的高度数据
						SelectObj(ho_upGlueRegionErosion, &ho_ObjectSelected2, hv_Index);
						MoveRegion(ho_ObjectSelected2, &ho_RegionMoved1, -hv_Row11, -hv_Column11);
						//
						GetRegionPoints(ho_RegionMoved1, &hv_Rows11, &hv_Columns11);
						GetGrayval(ho_ImageSurface, hv_Rows11, hv_Columns11, &hv_Grayval11);
						TupleMean(hv_Grayval11, &hv_Median12);

						hv_background_height = hv_background_height.TupleConcat(hv_Median12);
					}
					else if (0 != (hv_fitbackgroundHeightMode == 1))
					{

						GetRegionPoints(ho_ringRegion, &hv_Rows2, &hv_Columns2);
						GetGrayval(ho_baseDeepImg, hv_Rows2, hv_Columns2, &hv_Grayval2);
						TupleMedian(hv_Grayval2, &hv_Mean);
						hv_background_height = hv_background_height.TupleConcat(hv_Mean);

					}

				}
			}
		}

		//5.2 计算高度
		hv_ic_height = HTuple();
		TupleSub(hv_glue_top, hv_background_height, &hv_ic_height);
		hv_ic_height_new = (hv_ic_height / 10) / 1000;		// 采图软件对高度信息*10，1000是从um转换到mm


		//胶水半径，方法1：外接圆，方法2：拟合圆；目前采用方法2
		Connection(ho_bottomGlueRegion, &ho_ConnectedRegions);
		HTuple hv_fitBottomGlueRadius;
		{
			HObject  ho_Contours;
			HTuple hv_Row1, hv_Column1, hv_StartPhi, hv_EndPhi, hv_PointOrder;

			//RegionFeatures(ho_ConnectedRegions, "outer_radius", &hv_bottomGlueRadius);  // 外接方法，会偏大

			GenContourRegionXld(ho_ConnectedRegions, &ho_Contours, "border");		// 拟合方法
			FitCircleContourXld(ho_Contours, "algebraic", -1, 0, 0, 3, 2, &hv_Row1, &hv_Column1, &hv_fitBottomGlueRadius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);

		}
		hv_bottomGlueRadius_new = (hv_fitBottomGlueRadius * 2) * hv_pixDist;

		//6.求偏心距，胶水中心与芯片中心偏移量；思路：在S1V3获取芯片和胶水的区域 icGlueRegion ，再减去顶部胶水区域，剩下就是芯片区域
		//6.1 拿芯片中心
		Difference(ho_icGlueRegion, ho_upGlueRegion, &ho_RegionDifference1);
		OpeningCircle(ho_RegionDifference1, &ho_RegionOpening, hv_icOpeningSize);
		Intersection(ho_RegionOpening, ho_bottomGlueRegion, &ho_RegionIntersection1);
		SelectShape(ho_RegionIntersection1, &ho_icRegions, "area", "and", 2, 99999999);
		//AreaCenter(ho_icRegion, &hv_Area, &hv_icCenterRow, &hv_icCenterColumn);
		HTuple hv_Row12, hv_Column12, hv_Row22, hv_Column22;
		SmallestRectangle1(ho_icRegions, &hv_Row12, &hv_Column12, &hv_Row22, &hv_Column22);
		hv_icCenterRow = (hv_Row12 + hv_Row22) / 2;
		hv_icCenterColumn = (hv_Column12 + hv_Column22) / 2;


		HObject ho_icRegionU;
		Union1(ho_icRegions, &ho_icRegionU);
		output.icRegion = ho_icRegionU;	 // 提前输出

		//6.2 拿底部胶水中心
		Connection(ho_bottomGlueRegion, &ho_ConnectedRegions2);
		AreaCenter(ho_ConnectedRegions2, &hv_Area1, &hv_glueCenterRow, &hv_glueCenterColumn);

		//6.3 偏心距
		int icCenterRowNum = hv_icCenterRow.TupleLength().D();
		int glueCenterRowNum = hv_glueCenterRow.TupleLength().D();
		if (icCenterRowNum != glueCenterRowNum)	// 两组数据拿到的长度不一样，就是IC和基板的提取有问题，数量不一致
			return 7;

		hv_rowOffset = hv_icCenterRow - hv_glueCenterRow;
		hv_colOffset = hv_icCenterColumn - hv_glueCenterColumn;
		TupleSqrt((hv_colOffset * hv_colOffset) + (hv_rowOffset * hv_rowOffset), &hv_glueToLensOffset);
		hv_glueToLensOffset_new = hv_glueToLensOffset * hv_pixDist;


		// ========================    输出参数赋值     ==============================
		HObject ho_bottomGlueRegions;
		Connection(ho_bottomGlueRegion, &ho_bottomGlueRegions);

		output.bottomGlueRegion = ho_bottomGlueRegion;

		output.defectRegion = ho_bottomGlueRegions;// 用于获取缺陷特征用的， 必须用打散的
		int defectNum = hv_ic_height_new.TupleLength().D();


		output.flawinfos.resize(defectNum);
		miniLEDGlueRegionsToDefectStruct(ho_bottomGlueRegions, output.flawinfos, isCnt);

		for (int i = 0; i < defectNum; ++i)
		{
			output.flawinfos[i].FlawDiscreteFeature.d_params.block.maxDiameter = hv_bottomGlueRadius_new[i];
			//output.flawinfos[i].extendParameters.d_params.block.glueHeight = hv_glue_top_height[i] * mmPrePixleZ;
			//output.flawinfos[i].extendParameters.d_params.block.lensHeight = hv_ic_height[i] * mmPrePixleZ;
			//output.flawinfos[i].extendParameters.d_params.block.glueToLensHeight = hv_ic_height_new[i];	 // 平台C未实现
			//output.flawinfos[i].extendParameters.d_params.block.glueToLensOffset = hv_glueToLensOffset_new[i]; // 平台C未实现

			output.flawinfos[i].FlawDiscreteFeature.d_params.block.innerHeight = hv_ic_height_new[i];	 // 平台C未实现，临时采用该特征实现
			output.flawinfos[i].FlawDiscreteFeature.d_params.block.compactness = hv_glueToLensOffset_new[i]; // 平台C未实现，临时采用该特征实现


		}

		// 存储区域
		//WriteObject(output.bottomGlueRegion, "D:\\2-zkhy\\1-文档\\算法类\\算子相关\\dyn检测\\bottomGlueRegion");
		//WriteObject(output.icGlueRegion, "D:\\2-zkhy\\1-文档\\算法类\\算子相关\\dyn检测\\icGlueRegion");
		//WriteObject(output, "D:\\2-zkhy\\1-文档\\算法类\\算子相关\\dyn检测\\bottomGlueRegion");


		return 0;
	}







	void miniLEDGlueRegionsToDefectStruct(HObject& defectRegions, vector<FlawInfoStruct>& outFlaw, const bool& useContour, const bool& useRotateRect,const HTuple& homMat2D)
	{
		HTuple hFlawNum;
		// 缺陷个数
		CountObj(defectRegions, &hFlawNum);
		int flawNum = hFlawNum.I();

		outFlaw.resize(flawNum);
		vector<double> areaTmp(flawNum);
		vector<double> angleTmp(flawNum);
		vector<double> flawWidth(flawNum);
		vector<double> flawHeight(flawNum);
		vector<Point2f> centerPoint;
		vector<vector<Point2f>> rectContour;
		vector<vector<vector<Point2f>>> defectContours;
		zkhyPublicFuncHN::genFlawInfoFromRegions(defectRegions, areaTmp, angleTmp, flawWidth, flawHeight, centerPoint, rectContour, defectContours, useContour, useRotateRect, homMat2D);

		for (int i = 0; i < flawNum; i++)
		{
			outFlaw[i].FlawBasicInfo.flawArea = areaTmp[i];
			outFlaw[i].FlawBasicInfo.angle = angleTmp[i];
			outFlaw[i].FlawBasicInfo.flawWidth = flawWidth[i];
			outFlaw[i].FlawBasicInfo.flawHeight = flawHeight[i];
			outFlaw[i].FlawBasicInfo.centerPt = centerPoint[i];
			outFlaw[i].FlawBasicInfo.pts = rectContour[i];
			outFlaw[i].FlawBasicInfo.flawContours = defectContours[i];
			outFlaw[i].FlawBasicInfo.type = 0;
		}
	}



}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------miniLEDGlueDetect--------------------------//
	//功能：miniLED胶水检测
	int miniLEDGlueDetect(const MiniLEDGlueDetectInput& input, MiniLEDGlueDetectOutput& output)
	{
		try
		{
			return realminiLEDGlueDetect(input, output);
		}
		catch (HalconCpp::HException& except)
		{
			string errormsg = except.ErrorMessage().Text();
			return 4; //未知错误
		}
	}
}
