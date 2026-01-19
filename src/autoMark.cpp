#include "alglibrary/zkhyProjectHuaNan/autoMark.h"
#include "alglibrary/alglibMisc.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {
	void SelectBestMarkChannel(HObject ho_hImages, HObject ho_hMarkRegion, HTuple* hv_tChannel)
	{

		// Local iconic variables
		HObject  ho_Image1, ho_Image2, ho_Image3;

		// Local control variables
		HTuple  hv_Number, hv_Mean1, hv_Deviation1, hv_Mean2;
		HTuple  hv_Deviation2, hv_Mean3, hv_Deviation3, hv_Deviation;
		HTuple  hv_Indices, hv_Selected;

		(*hv_tChannel) = 1;
		CountObj(ho_hImages, &hv_Number);
		if (0 != (int(hv_Number != 3)))
		{
			return;
		}
		////如果方差都过于小，则返回的通道值0
		SelectObj(ho_hImages, &ho_Image1, 1);
		SelectObj(ho_hImages, &ho_Image2, 2);
		SelectObj(ho_hImages, &ho_Image3, 3);
		Intensity(ho_hMarkRegion, ho_Image1, &hv_Mean1, &hv_Deviation1);
		Intensity(ho_hMarkRegion, ho_Image2, &hv_Mean2, &hv_Deviation2);
		Intensity(ho_hMarkRegion, ho_Image3, &hv_Mean3, &hv_Deviation3);
		hv_Deviation = HTuple();
		TupleConcat(hv_Deviation1, hv_Deviation2, &hv_Deviation);
		TupleConcat(hv_Deviation, hv_Deviation3, &hv_Deviation);
		//
		TupleSortIndex(hv_Deviation, &hv_Indices);

		////判断最大的发错是否符合要求
		TupleSelect(hv_Deviation, HTuple(hv_Indices[2]), &hv_Selected);
		if (0 != (int(hv_Selected < 5)))
		{
			(*hv_tChannel) = 0;
		}
		else
		{
			(*hv_tChannel) = HTuple(hv_Indices[2]) + 1;
		}
		//
		return;
	}

	void TestMark(HObject ho_Images, HObject ho_MarkRegion, HObject ho_MarkSearchRegion,
		HTuple hv_Accuracy, HTuple* hv_isOK, HTuple* hv_Channel)
	{

		// Local iconic variables
		HObject  ho_OneImage, ho_ImageMark, ho_ImageSearch;
		HObject  ho_Image1, ho_Image2, ho_Image3, ho_Cross, ho_Region1;
		HObject  ho_Region2, ho_One, ho_Edge, ho_ImagePart, ho_ImageZoomed;
		HObject  ho_ModelImages, ho_ModelRegions, ho_ConnectedRegions1;
		HObject  ho_SelectedRegions1, ho_ModelContours, ho_ContoursAffineTrans;
		HObject  ho_Cross1;

		// Local control variables
		HTuple  hv_Error;
		HTuple  hv_nCh, hv_Min, hv_Max, hv_Range, hv_NumThred;
		HTuple  hv_Rows, hv_Columns, hv_UsedThreshold, hv_Area1;
		HTuple  hv_Row, hv_Column, hv_Area2, hv_Range1, hv_Range2;
		HTuple  hv_n, hv_Ran1, hv_Ran2, hv_Area, hv_Row5, hv_Column5;
		HTuple  hv_Number1, hv_angleStart, hv_angleExtent, hv_anglestep;
		HTuple  hv__, hv_MarkRow, hv_MarkCol, hv_ModelID, hv_FindRow;
		HTuple  hv_FindCol, hv_Angle, hv_Score, hv_Length, hv_HomMat2DIdentity;
		HTuple  hv_HomMat2D, hv_Ratio;

		(*hv_isOK) = 0;
		(*hv_Channel) = 0;

		//先选出对比度大的通道，只对对比度大的通道判断
		CountObj(ho_Images, &hv_nCh);
		if (0 != (int(hv_nCh == 3)))
		{
			SelectBestMarkChannel(ho_Images, ho_MarkRegion, &(*hv_Channel));

			if (0 != (int((*hv_Channel) == 0)))
			{
				(*hv_isOK) = 0;
				return;
			}
			SelectObj(ho_Images, &ho_OneImage, (*hv_Channel));
			ReduceDomain(ho_OneImage, ho_MarkRegion, &ho_ImageMark);
			ReduceDomain(ho_OneImage, ho_MarkSearchRegion, &ho_ImageSearch);
		}
		else if (0 != (int(hv_nCh == 1)))
		{
			CountChannels(ho_Images, &hv_nCh);
			if (0 != (int(hv_nCh == 3)))
			{
				Decompose3(ho_Images, &ho_Image1, &ho_Image2, &ho_Image3);
				ConcatObj(ho_Image1, ho_Image2, &ho_Images);
				ConcatObj(ho_Images, ho_Image3, &ho_Images);
				SelectBestMarkChannel(ho_Images, ho_MarkRegion, &(*hv_Channel));
				//
				if (0 != (int((*hv_Channel) == 0)))
				{
					(*hv_isOK) = 0;
					return;
				}
				SelectObj(ho_Images, &ho_OneImage, (*hv_Channel));
				//
				ReduceDomain(ho_OneImage, ho_MarkRegion, &ho_ImageMark);
				ShapeTrans(ho_MarkSearchRegion, &ho_MarkSearchRegion, "convex");
				DilationCircle(ho_MarkSearchRegion, &ho_MarkSearchRegion, 200);
				ReduceDomain(ho_OneImage, ho_MarkSearchRegion, &ho_ImageSearch);

			}
			else if (0 != (int(hv_nCh == 1)))
			{
				(*hv_Channel) = 1;
				SelectObj(ho_Images, &ho_OneImage, (*hv_Channel));
				ReduceDomain(ho_Images, ho_MarkRegion, &ho_ImageMark);
				ShapeTrans(ho_MarkSearchRegion, &ho_MarkSearchRegion, "convex");
				DilationCircle(ho_MarkSearchRegion, &ho_MarkSearchRegion, 200);
				ReduceDomain(ho_Images, ho_MarkSearchRegion, &ho_ImageSearch);
			}
		}
		else
		{
			return;
		}



		//Accuracy=5, 角点个数多，无套印，前背景比例合适，使用于mark尺寸小（50左右），有不规则图案区域比较多的产品,目的是找到最佳mark
		//Accuracy=4,角点个数多，前背景比例合适，适用于mark尺寸较大的情况，此时如果还用无套印，可能找不到mark，但是不能适用于多层套印
		//Accuracy=3,角点个数多,适用于套印较好的产品
		//Accuracy=2，有角点即可，在自定位检测中使用。
		//Accuracy=1，有角点即可，在定位检测中使用，找到的点更多些。针对说明书中，文字线条很细时也能用作定位
		//Accuracy=0，只做唯一性判断。

		//计算对比度，对比度大的判断角点
		MinMaxGray(ho_MarkRegion, ho_OneImage, 5, &hv_Min, &hv_Max, &hv_Range);
		if (0 != (int(hv_Range < 15)))
		{
			(*hv_isOK) = 0;
			return;
		}

		//角点数量的判断
		if (0 != (int(hv_Accuracy > 0)))
		{
			if (0 != (int(hv_Range > 30)))
			{
				//V1:0.3,2,0.08,1000(1);  V2:0.4, 4, 0.08, 1000(8)；V3:0.4, 4, 0.02, 1000(6)
				hv_NumThred = 1;
				if (0 != (int(hv_Accuracy > 2)))
				{
					//对图像做一个比较大的滤波，去掉复杂且锐度小的定位，并滤除个别干扰的产生，适用于薄膜产品
					PointsHarris(ho_ImageMark, 0.3, 4, 0.02, 1000, &hv_Rows, &hv_Columns);
					hv_NumThred = 6;
				}
				else
				{
					//去掉弧形拐角的情况，且保证能找到比较多的边缘，适用于烟包，文字产品
					//points_harris (ImageMark, 0.6, 4, 0.08, 1000, Rows, Columns)
				//	WriteImage(ho_ImageMark, "bmp", 0, "D:/1");
					PointsHarrisBinomial(ho_ImageMark, 15, 50, 0.01, 1000, "off", &hv_Rows, &hv_Columns);
					hv_NumThred = 4;
				}
				GenCrossContourXld(&ho_Cross, hv_Rows, hv_Columns, 60, 0.785398);
				if (0 != (int((hv_Rows.TupleLength()) < hv_NumThred)))
				{
					(*hv_isOK) = 0;
					return;
				}
			}
		}

		//V1:0; V2:1 对前背景区域的分析，是否有套印，前背景比例是否合适
		if (0 != (int(hv_Accuracy > 3)))
		{
			//保证只要两种颜色
			BinaryThreshold(ho_ImageMark, &ho_Region1, "max_separability", "dark", &hv_UsedThreshold);
			Difference(ho_MarkRegion, ho_Region1, &ho_Region2);
			ErosionRectangle1(ho_Region1, &ho_Region1, 3, 3);
			ErosionRectangle1(ho_Region2, &ho_Region2, 3, 3);
			AreaCenter(ho_Region1, &hv_Area1, &hv_Row, &hv_Column);
			AreaCenter(ho_Region2, &hv_Area2, &hv_Row, &hv_Column);
			if (0 != (int(hv_Accuracy > 4)))
			{
				hv_Range1 = 0;
				hv_Range2 = 0;
				{
					HTuple end_val97 = hv_nCh;
					HTuple step_val97 = 1;
					for (hv_n = 1; hv_n.Continue(end_val97, step_val97); hv_n += step_val97)
					{
						SelectObj(ho_Images, &ho_One, hv_n);
						MinMaxGray(ho_Region1, ho_One, 5, &hv_Min, &hv_Max, &hv_Ran1);
						MinMaxGray(ho_Region2, ho_One, 5, &hv_Min, &hv_Max, &hv_Ran2);
						if (0 != (int(hv_Range1 < hv_Ran1)))
						{
							hv_Range1 = hv_Ran1;
						}
						if (0 != (int(hv_Range2 < hv_Ran2)))
						{
							hv_Range2 = hv_Ran2;
						}
					}
				}

				//50
				if (0 != (HTuple(HTuple(int(hv_Area1 > 100)).TupleAnd(int(hv_Range1 > 60))).TupleOr(HTuple(int(hv_Area2 > 100)).TupleAnd(int(hv_Range2 > 60)))))
				{
					(*hv_isOK) = 0;
					return;
				}
			}

			//去掉边缘太细的mark
			if (0 != (HTuple(int(hv_Area1 < 10)).TupleOr(int(hv_Area2 < 10))))
			{
				(*hv_isOK) = 0;
				return;
			}

			if (0 != (HTuple(int((hv_Area1 / hv_Area2) > 20)).TupleOr(int((hv_Area2 / hv_Area1) > 20))))
			{
				(*hv_isOK) = 0;
				return;
			}
		}

		//V1:1; V2:0 对边缘强度的判断，在自定位中用到，暂时先加上
		if (0 != (int(hv_Accuracy == 1)))
		{
			SobelAmp(ho_ImageMark, &ho_Edge, "sum_abs", 5);
			MinMaxGray(ho_MarkRegion, ho_Edge, 5, &hv_Min, &hv_Max, &hv_Range);
			if (0 != (int(hv_Max < 20)))
			{
				(*hv_isOK) = 0;
				return;
			}
		}

		//对3层金字塔顶的分析，去掉一些噪声干扰区域和交叉信息小的区域
		if (0 != 1)
		{
			//3层金字塔时，点的数量,去除一些没有拐点的,
			CropDomain(ho_ImageMark, &ho_ImagePart);
			ZoomImageFactor(ho_ImagePart, &ho_ImageZoomed, 0.25, 0.25, "constant");
			InspectShapeModel(ho_ImagePart, &ho_ModelImages, &ho_ModelRegions, 1, 15);
			AreaCenter(ho_ModelRegions, &hv_Area, &hv_Row5, &hv_Column5);
			//去除一些噪声对比度大的区域
			if (0 != (int(hv_Area < 3)))
			{
				(*hv_isOK) = 0;
				return;
			}
			Connection(ho_ModelRegions, &ho_ConnectedRegions1);
			SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions1, "rect2_len2", "and",
				0.6, 99999);
			CountObj(ho_SelectedRegions1, &hv_Number1);
			//去除一些没有交叉点的区域
			if (0 != (HTuple(int(hv_Area > 10)).TupleAnd(int(hv_Number1 == 0))))
			{
				(*hv_isOK) = 0;
				return;
			}
		}


		//出现重复的mark直接返回，在搜索范围内判断唯一性
		//角度：V1:0,0,0;  V2:-0.05,0.1,0.05
		hv_angleStart = -0.39;
		hv_angleExtent = 6.29;
		hv_anglestep = 0.05;
		AreaCenter(ho_MarkRegion, &hv__, &hv_MarkRow, &hv_MarkCol);
		HTuple Num, AngleStart1, AngleExtent2, AngleStep3, ScaleMin, ScaleMax5, ScaleStep6, Metric7, MinContrast8;
		//shape判断
		if (0 != 1)
		{
			// Error variable 'hv_Error' activated
			hv_Error = 2;
			// dev_set_check ("~give_error")
			//V1:0, 'use_polarity', 15, 5,
			//V2:'auto', 'use_polarity', 'auto', 'auto',
			//V4:'none', 'use_polarity', 20, 10,
			try
			{
				hv_Error = 2;
				CreateShapeModel(ho_ImageMark, 3, hv_angleStart, hv_angleExtent, hv_anglestep,
					"none", "use_polarity", 20, 10, &hv_ModelID);
				GetShapeModelParams(hv_ModelID, &Num, &AngleStart1, &AngleExtent2, &AngleStep3, &ScaleMin, &ScaleMax5, &ScaleStep6, &Metric7, &MinContrast8);

			}
			catch (HException e)
			{
				hv_Error = (int)e.ErrorCode();
				if (hv_Error < 0)
					throw e;
			}
			try
			{
				hv_Error = 2;
				GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
			}
			catch (HException e)
			{
				hv_Error = (int)e.ErrorCode();
				if (hv_Error < 0)
					throw e;
			}
			//create_shape_model (ImageMark, 'auto', angleStart, angleExtent, 'auto', 'auto', 'use_polarity', 'auto', 'auto', ModelID)
			if (0 != (int(hv_Error != 2)))
			{
				(*hv_isOK) = 0;
				return;
			}
			// dev_set_check ("give_error")
			//V0:0.4, 2, 0.5, 'least_squares', 0, 0.5
			//V1:0.5, 2, 0.5, 'least_squares', 0, 0.5
			//V4:0.5, 2, 0.2, 'least_squares', 0, 0.8

			try
			{
				hv_Error = 2;
				FindShapeModel(ho_ImageSearch, hv_ModelID, hv_angleStart, hv_angleExtent, 0.4,
					4, 0, "least_squares", 3, 0.4, &hv_FindRow, &hv_FindCol, &hv_Angle, &hv_Score);
				//find_shape_model (ImageSearch, ModelID, angleStart, angleExtent, 0.4, 2, 0, 'least_squares', 0, 0.4, Row, Column, Angle, Score)

				TupleLength(hv_FindRow, &hv_Length);
			}
			catch (HException e)
			{
				hv_Error = (int)e.ErrorCode();
				if (hv_Error < 0)
					throw e;
			}
			if (0 != (int(hv_Length > 1)))
			{
				(*hv_isOK) = 0;
				return;
			}


			if (0 != (int((hv_Score.TupleLength()) != 1)))
			{
				(*hv_isOK) = 0;
				return;
			}
			else
			{
				HomMat2dIdentity(&hv_HomMat2DIdentity);
				VectorAngleToRigid(0, 0, 0, hv_FindRow, hv_FindCol, hv_Angle, &hv_HomMat2D);
				AffineTransContourXld(ho_ModelContours, &ho_ContoursAffineTrans, hv_HomMat2D);
				GenCrossContourXld(&ho_Cross1, hv_FindRow, hv_FindCol, 6, hv_Angle);
				//V0:无Score[0]<0.8
				//V4:0.7 一些细笔画的字定位时，匹配率很很低
				//这个分数设置的高时，可以滤除一下线条很细的文字定位
				hv_Ratio = 0.5;
				if (0 != (int(hv_Accuracy == 1)))
				{
					hv_Ratio = 0.7;
				}
				if (0 != (HTuple(HTuple(int(HTuple(hv_Score[0]) < hv_Ratio)).TupleOr(int(((hv_FindRow - hv_MarkRow).TupleFabs()) > 1))).TupleOr(int(((hv_FindCol - hv_MarkCol).TupleFabs()) > 1))))
				{
					(*hv_isOK) = 0;
					return;
				}
				ClearShapeModel(hv_ModelID);
			}
		}

		//ncc判断
		//zmh 这里可以不判断
		if (0 != 0)
		{
			CreateNccModel(ho_ImageMark, 3, hv_angleStart, hv_angleExtent, hv_anglestep,
				"use_polarity", &hv_ModelID);
			//create_ncc_model (ImageMark, 'auto', angleStart, angleExtent, 'auto', 'use_polarity', ModelID)
			FindNccModel(ho_ImageSearch, hv_ModelID, hv_angleStart, hv_angleExtent, 0.4,
				2, 0.2, "false", 0, &hv_FindRow, &hv_FindCol, &hv_Angle, &hv_Score);
			//find_ncc_model (ImageSearch, ModelID, angleStart, angleExtent, 0.4, 2, 0, 'false', 0, Row1, Column1, Angle1, Score)
			ClearNccModel(hv_ModelID);
			//gen_cross_contour_xld (Cross2, FindRow, FindCol, 6, Angle)
			if (0 != (int((hv_Score.TupleLength()) != 1)))
			{
				(*hv_isOK) = 0;
				return;
			}
			else
			{
				//gen_cross_contour_xld (Cross1, FindRow, FindCol, 6, Angle)
				//V0:无Score[0]<0.8
				//V4:0.7 一些细笔画的字定位时，匹配率很很低
				//这个分数设置的高时，可以滤除一下线条很细的文字定位
				hv_Ratio = 0.8;
				if (0 != (int(hv_Accuracy == 1)))
				{
					hv_Ratio = 0.7;
				}
				if (0 != (HTuple(HTuple(int(HTuple(hv_Score[0]) < hv_Ratio)).TupleOr(int(((hv_FindRow - hv_MarkRow).TupleFabs()) > 1))).TupleOr(int(((hv_FindCol - hv_MarkCol).TupleFabs()) > 1))))
				{
					(*hv_isOK) = 0;
					return;
				}
			}
		}

		(*hv_isOK) = 1;

		if (HDevWindowStack::IsOpen())
			DispObj(ho_OneImage, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_MarkRegion, HDevWindowStack::GetActive());
		return;
	}

	void SelectOneMark(HObject ho_ImageMeans, HObject ho_Region, HObject ho_MaybeMarks,
		HObject* ho_MarkRegions, HTuple hv_UniqueScope, HTuple hv_Accuracy, HTuple* hv_MarkChannels)
	{

		// Local iconic variables
		HObject  ho_OneMark, ho_MarkSearchRegion, ho_MarkSearchRegionH;

		// Local control variables
		HTuple  hv_Area, hv_Rows, hv_Columns, hv_Row1;
		HTuple  hv_Column1, hv_Row2, hv_Column2, hv_CenterRow, hv_CenterCol;
		HTuple  hv_Num, hv_Dists, hv_i, hv_Distance, hv_Indices;
		HTuple  hv_Index, hv_MarkRow1, hv_MarkColumn1, hv_MarkRow2;
		HTuple  hv_MarkColumn2, hv_isOk, hv_nCh;


		GenEmptyObj(&(*ho_MarkRegions));
		(*hv_MarkChannels) = HTuple();

		//排序
		AreaCenter(ho_MaybeMarks, &hv_Area, &hv_Rows, &hv_Columns);
		SmallestRectangle1(ho_Region, &hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
		hv_CenterRow = (hv_Row1 + hv_Row2) / 2;
		hv_CenterCol = (hv_Column1 + hv_Column2) / 2;


		hv_Num = hv_Rows.TupleLength();
		hv_Dists = HTuple();
		{
			HTuple end_val13 = hv_Num - 1;
			HTuple step_val13 = 1;
			for (hv_i = 0; hv_i.Continue(end_val13, step_val13); hv_i += step_val13)
			{
				DistancePp(HTuple(hv_Rows[hv_i]), HTuple(hv_Columns[hv_i]), hv_CenterRow, hv_CenterCol,
					&hv_Distance);
				TupleConcat(hv_Dists, hv_Distance, &hv_Dists);
			}
		}
		TupleSortIndex(hv_Dists, &hv_Indices);


		{
			HTuple end_val20 = hv_Num - 1;
			HTuple step_val20 = 1;
			for (hv_i = 0; hv_i.Continue(end_val20, step_val20); hv_i += step_val20)
			{
				hv_Index = HTuple(hv_Indices[hv_i]);
				SelectObj(ho_MaybeMarks, &ho_OneMark, hv_Index + 1);
				if (0 != (int(hv_UniqueScope < 1)))
				{
					if (0 != (int(hv_UniqueScope == 0)))
					{
						CopyObj(ho_Region, &ho_MarkSearchRegion, 1, -1);
					}
					else if (0 != (int(hv_UniqueScope == -1)))
					{
						SmallestRectangle1(ho_OneMark, &hv_MarkRow1, &hv_MarkColumn1, &hv_MarkRow2,
							&hv_MarkColumn2);
						GenRectangle1(&ho_MarkSearchRegion, hv_Row1, hv_MarkColumn1, hv_Row2, hv_MarkColumn2);
					}
					else if (0 != (int(hv_UniqueScope == -2)))
					{
						SmallestRectangle1(ho_OneMark, &hv_MarkRow1, &hv_MarkColumn1, &hv_MarkRow2,
							&hv_MarkColumn2);
						GenRectangle1(&ho_MarkSearchRegion, hv_MarkRow1, hv_Column1, hv_MarkRow2,
							hv_Column2);
					}
					else if (0 != (int(hv_UniqueScope == -3)))
					{
						SmallestRectangle1(ho_OneMark, &hv_MarkRow1, &hv_MarkColumn1, &hv_MarkRow2,
							&hv_MarkColumn2);
						GenRectangle1(&ho_MarkSearchRegion, hv_Row1, hv_MarkColumn1, hv_Row2, hv_MarkColumn2);
						GenRectangle1(&ho_MarkSearchRegionH, hv_MarkRow1, hv_Column1, hv_MarkRow2,
							hv_Column2);
						Union2(ho_MarkSearchRegion, ho_MarkSearchRegionH, &ho_MarkSearchRegion);
					}
					else if (0 != (int(hv_UniqueScope == -4)))
					{
						ho_MarkSearchRegion = ho_OneMark;
					}

				}
				else
				{
					DilationRectangle1(ho_OneMark, &ho_MarkSearchRegion, hv_UniqueScope, hv_UniqueScope);
				}
				TestMark(ho_ImageMeans, ho_OneMark, ho_MarkSearchRegion, hv_Accuracy, &hv_isOk,
					&hv_nCh);
				if (0 != (int(hv_isOk == 1)))
				{
					ConcatObj((*ho_MarkRegions), ho_OneMark, &(*ho_MarkRegions));
					TupleConcat((*hv_MarkChannels), hv_nCh, &(*hv_MarkChannels));
					break;
				}
			}
		}


		return;
	} 	
	void GetMaybeMarks(HObject ho_hImage, HObject ho_hRegion, HObject* ho_MaybeMark,
		HObject* ho_hMidImage, HTuple MinArea, HTuple MaxArea)
	{

		// Local iconic variables
		HObject  ho_ImageROIs, ho_Image1, ho_Image2, ho_Image3;
		HObject  ho_hImageMeans, ho_ImageSubs, ho_R, ho_G, ho_ImageSub;
		HObject  ho_B, ho_RegionErosion, ho_RegionEdge, ho_RegionEdges;
		HObject  ho_RegionDilation, ho_RegionDilations, ho_RegionEdges1;
		HObject  ho_RegionEdge1, ho_RegionEdge2, ho_RegionEdges2;
		HObject  ho_RegionTrans;

		// Local control variables
		HTuple  hv_Channels, hv_Min, hv_Max, hv_Range;
		HTuple  hv_Thred, hv_Number;

		ReduceDomain(ho_hImage, ho_hRegion, &ho_ImageROIs);
		CountObj(ho_hImage, &hv_Channels);
		if (0 != (int(hv_Channels == 1)))
		{
			CountChannels(ho_hImage, &hv_Channels);
			if (0 != (int(hv_Channels == 3)))
			{
				Decompose3(ho_ImageROIs, &ho_Image1, &ho_Image2, &ho_Image3);
				ConcatObj(ho_Image1, ho_Image2, &ho_ImageROIs);
				ConcatObj(ho_ImageROIs, ho_Image3, &ho_ImageROIs);
			}
		}
		CopyObj(ho_ImageROIs, &(*ho_hMidImage), 1, -1);
		MedianRect(ho_ImageROIs, &ho_hImageMeans, 3, 3);

		SobelAmp(ho_hImageMeans, &ho_ImageSubs, "sum_abs", 3);

		//ImageSubs := hImageMeans
		if (0 != (int(hv_Channels == 3)))
		{
			SelectObj(ho_ImageSubs, &ho_R, 1);
			SelectObj(ho_ImageSubs, &ho_G, 2);
			MaxImage(ho_R, ho_G, &ho_ImageSub);
			SelectObj(ho_ImageSubs, &ho_B, 3);
			MaxImage(ho_ImageSub, ho_B, &ho_ImageSub);
		}
		else
		{
			CopyObj(ho_ImageSubs, &ho_ImageSub, 1, -1);
		}

		//20170630 by mf 自定找定位进行局部微调定位时，如果画的文字区域很小，不连贯时，缩小的太小会导致找不到mark
		//这两将原来21改为了5，不清楚原来为什么要缩小了。
		ErosionRectangle1(ho_hRegion, &ho_RegionErosion, 5, 5);
		WriteImage(ho_ImageSub, "bmp", 0, "D:/1");
		MinMaxGray(ho_RegionErosion, ho_ImageSub, 1, &hv_Min, &hv_Max, &hv_Range);
		if (0 != (int(hv_Max > 50)))
		{
			hv_Thred = 25;
		}
		else if (0 != (int(hv_Max > 5)))
		{
			hv_Thred = hv_Max - 3;
		}
		else
		{
			GenEmptyObj(&(*ho_MaybeMark));
			return;
		}

		//这个阈值有25和10两套，自动选择
		Threshold(ho_ImageSub, &ho_RegionEdge, hv_Thred, 255);
		Intersection(ho_RegionEdge, ho_RegionErosion, &ho_RegionEdge);
		FillUpShape(ho_RegionEdge, &ho_RegionEdge, "area", 1, 100);
		Connection(ho_RegionEdge, &ho_RegionEdges);


		if (0 != 1)
		{
			SelectShape(ho_RegionEdges, &ho_RegionEdges, "area", "and", 80, 99999999);
			Union1(ho_RegionEdges, &ho_RegionEdge);
			DilationRectangle1(ho_RegionEdge, &ho_RegionDilation, 5, 5);
			Connection(ho_RegionDilation, &ho_RegionDilations);
			Intersection(ho_RegionDilations, ho_RegionEdge, &ho_RegionEdges);
			//默认 50
			PartitionRectangle(ho_RegionEdges, &ho_RegionEdges, 1000, 1000);
			SelectShape(ho_RegionEdges, &ho_RegionEdges, "area", "and", MinArea, MaxArea);
			SelectShape(ho_RegionEdges, &ho_RegionEdges, "width", "and", 20, 999999999);
			SelectShape(ho_RegionEdges, &ho_RegionEdges, "height", "and", 20, 99999999);
			CountObj(ho_RegionEdges, &hv_Number);
		}
		else
		{
			SelectShape(ho_RegionEdges, &ho_RegionEdges1, "width", "and", (MinArea * 3) / 4,
				(MinArea * 3) / 2);
			SelectShape(ho_RegionEdges1, &ho_RegionEdges1, "height", "and", (MinArea * 3) / 4,
				(MinArea * 3) / 2);
			Union1(ho_RegionEdges1, &ho_RegionEdge1);
			Difference(ho_RegionEdge, ho_RegionEdge1, &ho_RegionEdge2);
			DilationRectangle1(ho_RegionEdge2, &ho_RegionDilation, 9, 9);
			Connection(ho_RegionDilation, &ho_RegionDilations);
			Intersection(ho_RegionDilations, ho_RegionEdge2, &ho_RegionEdges2);
			PartitionRectangle(ho_RegionEdges2, &ho_RegionEdges2, MinArea, MinArea);
			SelectShape(ho_RegionEdges2, &ho_RegionEdges2, "area", "and", (MinArea * MinArea) * 0.1,
				999999999);
			SelectShape(ho_RegionEdges2, &ho_RegionEdges2, "width", "and", 20, 99999);
			SelectShape(ho_RegionEdges2, &ho_RegionEdges2, "height", "and", 20, 99999);
			ConcatObj(ho_RegionEdges1, ho_RegionEdges2, &ho_RegionEdges);
			CountObj(ho_RegionEdges, &hv_Number);
		}
		ho_RegionTrans = ho_RegionEdges;
		//shape_trans (RegionEdges, RegionTrans, 'rectangle1')
		DilationRectangle1(ho_RegionTrans, &ho_RegionTrans, 11, 11);
		Intersection(ho_RegionTrans, ho_RegionErosion, &(*ho_MaybeMark));

		return;
	}


	void GetPolygonPoints(HTuple hv_Rows, HTuple hv_Columns, HTuple hv_ModifyIndiceInSrc,
		HTuple* hv_RectIndicesInSrc)
	{

		// Local iconic variables
		HObject  ho_Cross, ho_RegionPoint, ho_RegionTrans1;
		HObject  ho_Cross2, ho_RegionTrans, ho_Contours;

		// Local control variables
		HTuple  hv_ModifyRows, hv_ModifyColumns, hv_Row11;
		HTuple  hv_Column11, hv_Row21, hv_Column21, hv_rowAll, hv_colAll;
		HTuple  hv_RectIndicesInModify, hv_i, hv_RowMulti, hv_ColMulti;
		HTuple  hv_Distance, hv_Indices, hv_DistanceMin, hv_DistanceMax;
		HTuple  hv_Index;

		//*这段代码的主要目标是在一组点（由 ModifyRows 和 ModifyColumns 定义）中找到距离轮廓（由 Contours 定义）最近的点，并生成一个矩形区域。
		TupleSelect(hv_Rows, hv_ModifyIndiceInSrc, &hv_ModifyRows);
		TupleSelect(hv_Columns, hv_ModifyIndiceInSrc, &hv_ModifyColumns);
		GenCrossContourXld(&ho_Cross, hv_ModifyRows, hv_ModifyColumns, 60, 0.785398);
		GenRegionPoints(&ho_RegionPoint, hv_ModifyRows, hv_ModifyColumns);
		if (0 != 1)
		{
			ShapeTrans(ho_RegionPoint, &ho_RegionTrans1, "rectangle1");
			//计算矩形区域的最小外接矩形
			SmallestRectangle1(ho_RegionTrans1, &hv_Row11, &hv_Column11, &hv_Row21, &hv_Column21);
			//生成四个顶点的坐标
			hv_rowAll.Clear();
			hv_rowAll.Append(hv_Row11);
			hv_rowAll.Append(hv_Row11);
			hv_rowAll.Append(hv_Row21);
			hv_rowAll.Append(hv_Row21);
			hv_colAll.Clear();
			hv_colAll.Append(hv_Column11);
			hv_colAll.Append(hv_Column21);
			hv_colAll.Append(hv_Column21);
			hv_colAll.Append(hv_Column11);
			//初始化一个空数组
			hv_RectIndicesInModify = HTuple();
			//对于每个顶点，执行以下操作：
			{
				HTuple end_val15 = (hv_rowAll.TupleLength()) - 1;
				HTuple step_val15 = 1;
				for (hv_i = 0; hv_i.Continue(end_val15, step_val15); hv_i += step_val15)
				{
					//生成一个常数数组，值为当前顶点的坐标
					TupleGenConst(hv_ModifyRows.TupleLength(), HTuple(hv_rowAll[hv_i]), &hv_RowMulti);
					TupleGenConst(hv_ModifyColumns.TupleLength(), HTuple(hv_colAll[hv_i]), &hv_ColMulti);
					//计算当前顶点与修改后的点集的距离
					DistancePp(hv_RowMulti, hv_ColMulti, hv_ModifyRows, hv_ModifyColumns, &hv_Distance);
					//对距离进行排序，获取最小距离的索引
					TupleSortIndex(hv_Distance, &hv_Indices);
					if (0 != (int((hv_Indices.TupleLength()) > 2)))
					{
						hv_RectIndicesInModify = hv_RectIndicesInModify.TupleConcat(hv_Indices.TupleSelectRange(0, 1));
					}
					//将最小距离的索引添加到数组中
				}
			}
			//从修改后的点集的索引中选择最小距离的点，得到原始点集中的索引
			GenCrossContourXld(&ho_Cross2, HTuple(hv_ModifyRows[hv_RectIndicesInModify]),
				HTuple(hv_ModifyColumns[hv_RectIndicesInModify]), 600, 0);
			TupleSelect(hv_ModifyIndiceInSrc, hv_RectIndicesInModify, &(*hv_RectIndicesInSrc));
		}
		else
		{

			ShapeTrans(ho_RegionPoint, &ho_RegionTrans, "convex");
			GenContourRegionXld(ho_RegionTrans, &ho_Contours, "border");
			DistancePc(ho_Contours, hv_ModifyRows, hv_ModifyColumns, &hv_DistanceMin, &hv_DistanceMax);
			TupleSortIndex(hv_DistanceMin, &hv_Indices);
			{
				HTuple end_val37 = (hv_Indices.TupleLength()) - 1;
				HTuple step_val37 = 1;
				for (hv_i = 0; hv_i.Continue(end_val37, step_val37); hv_i += step_val37)
				{
					hv_Index = HTuple(hv_Indices[hv_i]);
					if (0 != (int(HTuple(hv_DistanceMin[hv_Index]) > 350)))
					{
						break;
					}
				}
			}
		}
		//tuple_max2 (i-1, 4, Max2)
		//tuple_min2 (Max2, |Indices|-1, Max2)
		//tuple_select_range (Indices, 0, Max2, RectIndicesInModify)


		//tuple_select (ModifyRows, RectIndicesInModify, SelectRows)
		//tuple_select (ModifyColumns, RectIndicesInModify, SelectCols)
		//gen_cross_contour_xld (Cross, SelectRows, SelectCols, 600, 0.785398)
		//gen_rectangle1 (Rectangle, Top, Left, Bottom, Right)

		//smallest_rectangle1 (RegionTrans, Row1, Column1, Row2, Column2)
		//gen_rectangle1 (Rectangle, Row1, Column1, Row2, Column2)

		//计算相对最原始（候选mark）的索引
		//tuple_select (ModifyIndiceInSrc, RectIndicesInModify, RectIndicesInSrc)
		return;
	}

	void DeleteTupleItem(HTuple hv_DataSrc, HTuple hv_DeleteData, HTuple* hv_DataDst)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_NotEqualIndices, hv_i, hv_DataItem;
		HTuple  hv_bEqual, hv_j, hv_Delete;



		hv_NotEqualIndices = HTuple();
		{
			HTuple end_val3 = (hv_DataSrc.TupleLength()) - 1;
			HTuple step_val3 = 1;
			for (hv_i = 0; hv_i.Continue(end_val3, step_val3); hv_i += step_val3)
			{
				hv_DataItem = HTuple(hv_DataSrc[hv_i]);
				hv_bEqual = 0;
				{
					HTuple end_val6 = (hv_DeleteData.TupleLength()) - 1;
					HTuple step_val6 = 1;
					for (hv_j = 0; hv_j.Continue(end_val6, step_val6); hv_j += step_val6)
					{
						hv_Delete = HTuple(hv_DeleteData[hv_j]);
						if (0 != (int(hv_DataItem == hv_Delete)))
						{
							hv_bEqual = 1;
							break;
						}
					}
				}
				if (0 != (int(hv_bEqual == 0)))
				{
					TupleConcat(hv_NotEqualIndices, hv_i, &hv_NotEqualIndices);
				}
			}
		}
		TupleSelect(hv_DataSrc, hv_NotEqualIndices, &(*hv_DataDst));

		//halcon12
		//tuple_gen_const (|DataSrc|, 0, Equals)
		//for j := 0 to |DeleteData|-1 by 1
			//Delete := DeleteData[j]
			//tuple_equal_elem (DataSrc, Delete, equal)
			//tuple_or (Equals, equal, Equals)
		//endfor
		//tuple_find (Equals, 0, Indices)
		//tuple_select (DataSrc, Indices, DataDst)



		return;

	}


	void UpdateModifyMark(HObject ho_ImageMeans, HObject ho_MaybeMarks, HTuple hv_ModifyIndexInSrc,
		HTuple hv_RectIndexsInSrc, HTuple hv_FindedsSrc, HTuple hv_ChannelsSrc, HTuple hv_UniqueScope,
		HTuple hv_Accuracy, HTuple* hv_ModifyOKIndexInSrc, HTuple* hv_RectOKIndexsInSrc,
		HTuple* hv_Findeds, HTuple* hv_Channels)
	{

		// Local iconic variables
		HObject  ho_OneMark, ho_MarkSearchRegion;

		// Local control variables
		HTuple  hv_DeleteIndex, hv_SelectNum, hv_n, hv_nIndex;
		HTuple  hv_isOk, hv_nCh;

		//gen_empty_obj (MarkRegions)
		//MarkChannels := []
		(*hv_Findeds) = hv_FindedsSrc;
		(*hv_Channels) = hv_ChannelsSrc;
		hv_DeleteIndex = HTuple();
		hv_SelectNum = hv_RectIndexsInSrc.TupleLength();
		{
			HTuple end_val6 = hv_SelectNum - 1;
			HTuple step_val6 = 1;
			for (hv_n = 0; hv_n.Continue(end_val6, step_val6); hv_n += step_val6)
			{
				hv_nIndex = HTuple(hv_RectIndexsInSrc[hv_n]);
				SelectObj(ho_MaybeMarks, &ho_OneMark, hv_nIndex + 1);
				if (0 != (int(HTuple((*hv_Findeds)[hv_nIndex]) == -1)))
				{
					DilationRectangle1(ho_OneMark, &ho_MarkSearchRegion, hv_UniqueScope, hv_UniqueScope);
					TestMark(ho_ImageMeans, ho_OneMark, ho_MarkSearchRegion, hv_Accuracy, &hv_isOk,
						&hv_nCh);
					if (0 != hv_isOk)
					{
						(*hv_Findeds)[hv_nIndex] = 1;
						(*hv_Channels)[hv_nIndex] = hv_nCh;
					}
					else
					{
						(*hv_Findeds)[hv_nIndex] = 0;
						TupleConcat(hv_DeleteIndex, hv_nIndex, &hv_DeleteIndex);
						continue;
					}
				}
				else
				{
					if (0 != (int(HTuple((*hv_Findeds)[hv_nIndex]) == 0)))
					{
						TupleConcat(hv_DeleteIndex, hv_nIndex, &hv_DeleteIndex);
						continue;
					}
					else
					{
						hv_nCh = HTuple((*hv_Channels)[hv_nIndex]);
					}
				}
				//concat_obj (MarkRegions, OneMark, MarkRegions)
				//tuple_concat (MarkChannels, nCh, MarkChannels)
			}
		}

		//计算修正mark的索引
		DeleteTupleItem(hv_ModifyIndexInSrc, hv_DeleteIndex, &(*hv_ModifyOKIndexInSrc));

		DeleteTupleItem(hv_RectIndexsInSrc, hv_DeleteIndex, &(*hv_RectOKIndexsInSrc));
		//RectOKIndexsInSrc := ModifyOKIndexInSrc

		return;
	}
	void DebugOutput(const std::string& message) {
		OutputDebugStringA(message.c_str());
	}
	void GetMaxAreaPoints(HTuple hv_Rows, HTuple hv_Columns, HTuple hv_RectOKIndiceInSrc,
		HTuple hv_nMarkNum, HTuple hv_area1, HTuple* hv_MaxAreaIndicesInSrc, HTuple* hv_MaxArea)
	{

		// Local iconic variables
		HObject  ho_Cross, ho_Region;

		// Local control variables
		HTuple  hv_Num, hv_bSelect, hv_RectRows, hv_RectColumns;
		HTuple  hv_MaxAreas, hv_Index1s, hv_Index2s, hv_Index3s;
		HTuple  hv_Index4s, hv_break_all, hv_i, hv_j, hv_Row1, hv_Col1;
		HTuple  hv_Row2, hv_Col2, hv_Area, hv_k, hv_Row3, hv_Col3;
		HTuple  hv_DistA, hv_DistB, hv_DistC, hv_S, hv_m, hv_Row4;
		HTuple  hv_Col4, hv_Row, hv_Column, hv_AreaIndices, hv_nIndex;
		HTuple  hv_MaxAreaIndicesInRectOK;

		hv_Num = hv_RectOKIndiceInSrc.TupleLength();

		(*hv_MaxArea) = 999999;
		hv_bSelect = 1;
		if (0 != (int(hv_nMarkNum == 2)))
		{
			if (0 != (int(hv_Num < 2)))
			{
				hv_bSelect = 0;
			}
		}
		else if (0 != (int(hv_nMarkNum == 3)))
		{
			if (0 != (int(hv_Num < 3)))
			{
				hv_bSelect = 0;
			}
		}
		else if (0 != (int(hv_nMarkNum == 4)))
		{
			if (0 != (int(hv_Num < 4)))
			{
				hv_bSelect = 0;
			}
		}
		else
		{
			hv_bSelect = 0;
		}

		if (0 != (int(hv_bSelect == 0)))
		{
			(*hv_MaxAreaIndicesInSrc) = hv_RectOKIndiceInSrc;
			return;
		}


		TupleSelect(hv_Rows, hv_RectOKIndiceInSrc, &hv_RectRows);
		TupleSelect(hv_Columns, hv_RectOKIndiceInSrc, &hv_RectColumns);
		GenCrossContourXld(&ho_Cross, hv_RectRows, hv_RectColumns, 6, 0.785398);

		hv_MaxAreas.Clear();
		hv_Index1s.Clear();
		hv_Index2s.Clear();
		hv_Index3s.Clear();
		hv_Index4s.Clear();
		hv_break_all = 0;
		{
			HTuple end_val36 = hv_Num - hv_nMarkNum;
			HTuple step_val36 = 1;
			for (hv_i = 0; hv_i.Continue(end_val36, step_val36); hv_i += step_val36)
			{
				if (0 != (int(hv_break_all == 1)))
				{
					break;
				}
				{
					HTuple end_val40 = (hv_Num - hv_nMarkNum) + 1;
					HTuple step_val40 = 1;
					for (hv_j = hv_i + 1; hv_j.Continue(end_val40, step_val40); hv_j += step_val40)
					{
						if (0 != (int(hv_break_all == 1)))
						{
							break;
						}
						if (0 != (int(hv_nMarkNum == 2)))
						{
							hv_Row1 = HTuple(hv_RectRows[hv_i]);
							hv_Col1 = HTuple(hv_RectColumns[hv_i]);
							hv_Row2 = HTuple(hv_RectRows[hv_j]);
							hv_Col2 = HTuple(hv_RectColumns[hv_j]);
							DistancePp(hv_Row1, hv_Col1, hv_Row2, hv_Col2, &hv_Area);
							TupleConcat(hv_MaxAreas, hv_Area, &hv_MaxAreas);
							TupleConcat(hv_Index1s, hv_i, &hv_Index1s);
							TupleConcat(hv_Index2s, hv_j, &hv_Index2s);
						}
						else
						{
							{
								HTuple end_val54 = (hv_Num - hv_nMarkNum) + 2;
								HTuple step_val54 = 1;
								for (hv_k = hv_j + 1; hv_k.Continue(end_val54, step_val54); hv_k += step_val54)
								{
									if (0 != (int(hv_break_all == 1)))
									{
										break;
									}
									hv_Row1 = HTuple(hv_RectRows[hv_i]);
									hv_Col1 = HTuple(hv_RectColumns[hv_i]);
									hv_Row2 = HTuple(hv_RectRows[hv_j]);
									hv_Col2 = HTuple(hv_RectColumns[hv_j]);
									hv_Row3 = HTuple(hv_RectRows[hv_k]);
									hv_Col3 = HTuple(hv_RectColumns[hv_k]);
									if (0 != (int(hv_nMarkNum == 3)))
									{
										DistancePp(hv_Row1, hv_Col1, hv_Row2, hv_Col2, &hv_DistA);
										DistancePp(hv_Row1, hv_Col1, hv_Row3, hv_Col3, &hv_DistB);
										DistancePp(hv_Row2, hv_Col2, hv_Row3, hv_Col3, &hv_DistC);
										hv_S = ((hv_DistA + hv_DistB) + hv_DistC) / 2.0;
										hv_Area = (((hv_S * (hv_S - hv_DistA)) * (hv_S - hv_DistB)) * (hv_S - hv_DistC)).TupleSqrt();
										TupleConcat(hv_MaxAreas, hv_Area, &hv_MaxAreas);
										TupleConcat(hv_Index1s, hv_i, &hv_Index1s);
										TupleConcat(hv_Index2s, hv_j, &hv_Index2s);
										TupleConcat(hv_Index3s, hv_k, &hv_Index3s);
									}
									else
									{
										{
											HTuple end_val75 = (hv_Num - hv_nMarkNum) + 3;
											HTuple step_val75 = 1;
											for (hv_m = hv_k + 1; hv_m.Continue(end_val75, step_val75); hv_m += step_val75)
											{
												if (0 != (int(hv_break_all == 1)))
												{
													break;
												}

												hv_Row4 = HTuple(hv_RectRows[hv_m]);
												hv_Col4 = HTuple(hv_RectColumns[hv_m]);
												GenRegionPolygonFilled(&ho_Region, ((hv_Row1.TupleConcat(hv_Row2)).TupleConcat(hv_Row3)).TupleConcat(hv_Row4),
													((hv_Col1.TupleConcat(hv_Col2)).TupleConcat(hv_Col3)).TupleConcat(hv_Col4));
												
												if (zkhyPublicFuncHN::isHRegionEmpty(ho_Region))
												{
													continue;
												}
												

												AreaCenter(ho_Region, &hv_Area, &hv_Row, &hv_Column);
												DebugOutput("------100000000---" + std::to_string(hv_Area.I()) + "\n");
												try {
													printf("1\r\n");
													DebugOutput("------11111111111-1------" + std::to_string(hv_Area.I()) + "\n");
													WriteTuple(hv_MaxAreas, "d://hv_MaxAreas.tup");
													TupleConcat(hv_MaxAreas, hv_Area, &hv_MaxAreas);
													DebugOutput("------22222222222----" + std::to_string(hv_MaxAreas.I()) + "\n");


													WriteTuple(hv_Index1s, "d://hv_Index1s.tup");
													TupleConcat(hv_Index1s, hv_i, &hv_Index1s);
													DebugOutput("------333333333333---" + std::to_string(hv_Index1s.I()) + "\n");

													WriteTuple(hv_Index2s, "d://hv_Index2s.tup");
													TupleConcat(hv_Index2s, hv_j, &hv_Index2s);
													DebugOutput("------44444444444---" + std::to_string(hv_Index2s.I()) + "\n");

													WriteTuple(hv_Index3s, "d://hv_Index3s.tup");
													TupleConcat(hv_Index3s, hv_k, &hv_Index3s);
													DebugOutput("------55555555555----" + std::to_string(hv_Index3s.I()) + "\n");

													TupleConcat(hv_Index4s, hv_m, &hv_Index4s);
													DebugOutput("------666666666660---" + std::to_string(hv_Index4s.I()) + "\n");

												}
												catch (HalconCpp::HException& except)
												{
													string errormsg = except.ErrorMessage().Text();
													HTuple hv_Exception;
													except.ToHTuple(&hv_Exception);
													return;
												}
												if (0 != (int(hv_Area > hv_area1)))
												{
													hv_break_all = 1;
													break;

												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
		TupleSortIndex(hv_MaxAreas, &hv_AreaIndices);
		hv_nIndex = ((const HTuple&)hv_AreaIndices)[(hv_AreaIndices.TupleLength()) - 1];
		if (0 != (int(hv_nMarkNum == 2)))
		{
			TupleConcat(HTuple(hv_Index1s[hv_nIndex]), HTuple(hv_Index2s[hv_nIndex]), &hv_MaxAreaIndicesInRectOK);
		}
		else if (0 != (int(hv_nMarkNum == 3)))
		{
			TupleConcat(HTuple(hv_Index1s[hv_nIndex]), HTuple(hv_Index2s[hv_nIndex]), &hv_MaxAreaIndicesInRectOK);
			TupleConcat(hv_MaxAreaIndicesInRectOK, HTuple(hv_Index3s[hv_nIndex]), &hv_MaxAreaIndicesInRectOK);
		}
		else
		{
			TupleConcat(HTuple(hv_Index1s[hv_nIndex]), HTuple(hv_Index2s[hv_nIndex]), &hv_MaxAreaIndicesInRectOK);
			TupleConcat(hv_MaxAreaIndicesInRectOK, HTuple(hv_Index3s[hv_nIndex]), &hv_MaxAreaIndicesInRectOK);
			TupleConcat(hv_MaxAreaIndicesInRectOK, HTuple(hv_Index4s[hv_nIndex]), &hv_MaxAreaIndicesInRectOK);
		}

		TupleSelect(hv_RectOKIndiceInSrc, hv_MaxAreaIndicesInRectOK, &(*hv_MaxAreaIndicesInSrc));
		(*hv_MaxArea) = HTuple(hv_MaxAreas[hv_nIndex]);
		return;
	}

	void SelectMaxScopeMark(HObject ho_ImageMeans, HObject ho_Region, HObject ho_MaybeMarks,
		HObject* ho_MarkRegions, HTuple hv_nMarkNum, HTuple hv_UniqueScope, HTuple hv_Accuracy,
		HTuple* hv_MarkChannels)
	{

		// Local iconic variables
		HObject  ho_Domain;

		// Local control variables
		HTuple  hv_Area, hv_Rows, hv_Columns, hv_Channels;
		HTuple  hv_Findeds, hv_ModifyRows, hv_ModifyCols, hv_ModifyIndicesInSrc;
		HTuple  hv_RectIndicesInSrc, hv_RectOKIndexsInSrc, hv_area1;
		HTuple  hv_Row, hv_col, hv_MaxAreaIndexsInSrc, hv_MaxArea;
		HTuple  hv_OKIndices;



		//full_domain (ImageMeans, ImageFull)
		GenEmptyObj(&(*ho_MarkRegions));
		(*hv_MarkChannels) = HTuple();

		if (0 != (HTuple(HTuple(int(hv_nMarkNum != 2)).TupleAnd(int(hv_nMarkNum != 3))).TupleAnd(int(hv_nMarkNum != 4))))
		{
			return;
		}

		//排序
		AreaCenter(ho_MaybeMarks, &hv_Area, &hv_Rows, &hv_Columns);
		//记录测试过mark的通道，没测试过的为-1
		TupleGenConst(hv_Rows.TupleLength(), -1, &hv_Channels);
		//记录测试过mark，合格为1，不合格为0， 没测试过为-1
		TupleGenConst(hv_Rows.TupleLength(), -1, &hv_Findeds);
		hv_ModifyRows = hv_Rows;
		hv_ModifyCols = hv_Columns;
		//tuple_gen_const (|Rows|, 1, ModifyIndicesInSrc)
		//tuple_cumul (ModifyIndicesInSrc, ModifyIndicesInSrc)
		//tuple_sub (ModifyIndicesInSrc, 1, ModifyIndicesInSrc)
		TupleGenSequence(0, (hv_Rows.TupleLength()) - 1, 1, &hv_ModifyIndicesInSrc);
		while (0 != 1)
		{
			//获取修改mark（去点已经判断不合格的mark）在外接矩上的点，输出为选择点相对候选mark的索引,大于外接矩形距离500的点被剔除。
			//GetRectPoints (Rows, Columns, ModifyIndicesInSrc, RectIndicesInSrc)
			GetPolygonPoints(hv_Rows, hv_Columns, hv_ModifyIndicesInSrc, &hv_RectIndicesInSrc);

			//测试在外接矩上的mark，不合格mark索引从修改mark索引和外接矩mark索引中去除,并更新mark的找过标志和通道标志
			UpdateModifyMark(ho_ImageMeans, ho_MaybeMarks, hv_ModifyIndicesInSrc, hv_RectIndicesInSrc,
				hv_Findeds, hv_Channels, hv_UniqueScope, hv_Accuracy, &hv_ModifyIndicesInSrc,
				&hv_RectOKIndexsInSrc, &hv_Findeds, &hv_Channels);

			//by zmh 20231225
			//if (|RectIndicesInSrc|#|RectOKIndexsInSrc|)
			  //continue
			//endif
			if (0 != (int((hv_RectOKIndexsInSrc.TupleLength()) < ((hv_RectIndicesInSrc.TupleLength()) - 2))))
			{
				continue;
			}
			GetDomain(ho_ImageMeans, &ho_Domain);
			AreaCenter(ho_Domain, &hv_area1, &hv_Row, &hv_col);
			try{
			//找nMarkNum个定位核构成面积最大情况，这个函数很耗时，需要优化
			GetMaxAreaPoints(hv_Rows, hv_Columns, hv_RectOKIndexsInSrc, hv_nMarkNum, hv_area1,
				&hv_MaxAreaIndexsInSrc, &hv_MaxArea);
			}
			catch (HalconCpp::HException& except)
			{
				string errormsg = except.ErrorMessage().Text();
				HTuple hv_Exception;
				except.ToHTuple(&hv_Exception);
				return;
			}
			SelectObj(ho_MaybeMarks, &(*ho_MarkRegions), hv_MaxAreaIndexsInSrc + 1);
			TupleSelect(hv_Channels, hv_MaxAreaIndexsInSrc, &(*hv_MarkChannels));
			if (0 != (HTuple(int(hv_MaxArea > 1000)).TupleAnd(HTuple(HTuple(int(hv_nMarkNum == 3)).TupleAnd(int((hv_MaxAreaIndexsInSrc.TupleLength()) == 3))).TupleOr(HTuple(int(hv_nMarkNum == 4)).TupleAnd(int((hv_MaxAreaIndexsInSrc.TupleLength()) == 4))))))
			{
				break;
			}

			//当不满足要求找到的个数要求，在testmark时没有不合格mark，那么就会陷入死循环，此时就跳出
			if (0 != (int((hv_RectIndicesInSrc.TupleLength()) == (hv_RectOKIndexsInSrc.TupleLength()))))
			{
				break;
			}

			//当不断的缩小mark的外接距，当面积太小时，选的是什么就是什么了，不再继续找了
			if (hv_MaxArea < 1000)
			{
				break;
			}


		}

		return;
	}
	void tuple_shuffle(HTuple hv_Tuple, HTuple* hv_Shuffled)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_ShuffleIndices;

		//This procedure sorts the input tuple randomly.
		//
		if (0 != (int((hv_Tuple.TupleLength()) > 0)))
		{
			//Create a tuple of random numbers,
			//sort this tuple, and return the indices
			//of this sorted tuple.
			hv_ShuffleIndices = HTuple::TupleRand(hv_Tuple.TupleLength()).TupleSortIndex();
			//Assign the elements of Tuple
			//to these random positions.
			(*hv_Shuffled) = HTuple(hv_Tuple[hv_ShuffleIndices]);
		}
		else
		{
			//If the input tuple is empty,
			//an empty tuple should be returned.
			(*hv_Shuffled) = HTuple();
		}
		return;
	}


	//自动查找mark
	void FindAutoMark(HObject ho_hImage, HObject ho_hRegion, HObject* ho_hMarkRegions, HObject* Maybemark,
		HTuple hv_UniqueScope, HTuple hv_Accuracy, HTuple MinArea, HTuple MaxArea, HTuple hv_MarkPosType,
		HTuple* hv_tMarkChannels, HTuple* hv_tMarkFinded)
	{

		// Local iconic variables
		HObject ho_hImageMeans, ho_hMarkRegions1, ho_hMaybeMarks;

		// Local control variables
		HTuple  hv_tMarkPos, hv_MarkNum;
		// Local iconic variables
		HObject  ho_RegionUnion, ho_Marks, ho_SubMarkRegion, ho_Rectangle;
		HObject  ho_EmptyObject, ho_Partitioned, ho_ObjectSelected;
		HObject  ho_ConnectedRegions, ho_ObjectSelected1;

		// Local control variables
		HTuple   hv_Number3, hv_Index2;
		HTuple  hv_i, hv_a, hv_Row, hv_Column, hv_Phi, hv_Length1;
		HTuple  hv_Length2, hv_AspectRatio, hv_Number1, hv_Index;
		HTuple  hv_Row3, hv_Column3, hv_Phi2, hv_Length11, hv_Length21;
		HTuple  hv_Number2, hv_Index1, hv_Row4, hv_Column4, hv_Phi3, hv_Phi4;
		HTuple  hv_Length12, hv_Length22, hv_AspectRatio22, hv_N;
		HTuple  hv_a1, hv_Sequence, hv_Reduced, hv_Shuffled, hv_lenth;
		HTuple  hv_Selected, hv_Concat;
		GenEmptyObj(&(*ho_hMarkRegions));
		(*hv_tMarkChannels) = HTuple();
		if (0 != 0)
		{
			//GetMaybeMarks_CL(ho_hImage, ho_hRegion, &ho_hMaybeMarks, &ho_hImageMeans, hv_MarkSize);
		}
		else
		{
			GetMaybeMarks(ho_hImage, ho_hRegion, &ho_hMaybeMarks, &ho_hImageMeans, MinArea, MaxArea);
			*Maybemark = ho_hMaybeMarks;
		}
		if (0 != (HTuple(HTuple(int(hv_MarkPosType == HTuple("4mark_corner"))).TupleOr(int(hv_MarkPosType == HTuple("2mark_corner02")))).TupleOr(int(hv_MarkPosType == HTuple("2mark_corner13")))))
		{
			TupleGenConst(4, 1, &hv_tMarkPos);
			if (0 != (int(hv_MarkPosType == HTuple("2mark_corner02"))))
			{
				hv_tMarkPos[1] = 0;
				hv_tMarkPos[3] = 0;
			}
			else if (0 != (int(hv_MarkPosType == HTuple("2mark_corner13"))))
			{
				hv_tMarkPos[0] = 0;
				hv_tMarkPos[2] = 0;
			}
			//SelectFourMark(ho_hImageMeans, ho_hRegion, ho_hMaybeMarks, &(*ho_hMarkRegions),
				//hv_tMarkPos, hv_UniqueScope, hv_Accuracy, &(*hv_tMarkChannels), &(*hv_tMarkFinded));
		}
		else if (0 != (HTuple(HTuple(int(hv_MarkPosType == HTuple("2mark_maxarea"))).TupleOr(int(hv_MarkPosType == HTuple("4mark_maxarea")))).TupleOr(int(hv_MarkPosType == HTuple("3mark_maxarea")))))
		{
			hv_MarkNum = 3;
			if (0 != (int(hv_MarkPosType == HTuple("2mark_maxarea"))))
			{
				hv_MarkNum = 2;
			}
			else if (0 != (int(hv_MarkPosType == HTuple("4mark_maxarea"))))
			{
				hv_MarkNum = 4;
			}
			SelectMaxScopeMark(ho_hImageMeans, ho_hRegion, ho_hMaybeMarks, &(*ho_hMarkRegions),
				hv_MarkNum, hv_UniqueScope, hv_Accuracy, &(*hv_tMarkChannels));
		}
		else if (0 != (int(hv_MarkPosType == HTuple("2mark_horiline"))))
		{
			//SelectTwoMark(ho_hImageMeans, ho_hRegion, ho_hMaybeMarks, &(*ho_hMarkRegions),
				//hv_UniqueScope, hv_Accuracy, &(*hv_tMarkChannels), &(*hv_tMarkFinded));
		}
		else if (0 != (HTuple(HTuple(HTuple(HTuple(int(hv_MarkPosType == HTuple("1mark_center"))).TupleOr(int(hv_MarkPosType == HTuple("1mark_center_allonly")))).TupleOr(int(hv_MarkPosType == HTuple("1mark_center_veronly")))).TupleOr(int(hv_MarkPosType == HTuple("1mark_center_horonly")))).TupleOr(int(hv_MarkPosType == HTuple("1mark_center_crossonly")))))
		{

			if (0 != (int(hv_MarkPosType == HTuple("1mark_center_allonly"))))
			{
				hv_UniqueScope = 0;
			}
			else if (0 != (int(hv_MarkPosType == HTuple("1mark_center_veronly"))))
			{
				hv_UniqueScope = -1;
			}
			else if (0 != (int(hv_MarkPosType == HTuple("1mark_center_horonly"))))
			{
				hv_UniqueScope = -2;
			}
			else if (0 != (int(hv_MarkPosType == HTuple("1mark_center_crossonly"))))
			{
				hv_UniqueScope = -3;
			}
			else if (0 != (int(hv_MarkPosType == HTuple("1mark_center"))))
			{
				hv_UniqueScope = -4;
			}
			SelectOneMark(ho_hImageMeans, ho_hRegion, ho_hMaybeMarks, &(*ho_hMarkRegions),
				hv_UniqueScope, hv_Accuracy, &(*hv_tMarkChannels));
		}
		else if (0 != (int(hv_MarkPosType == HTuple("allmark"))))
		{
			//SelectAllOKMark(ho_hImageMeans, ho_hRegion, ho_hMaybeMarks, &(*ho_hMarkRegions),
				//hv_UniqueScope, hv_Accuracy, &(*hv_tMarkChannels));
		}
		else if (0 != (int(hv_MarkPosType == HTuple("group_mark_corner02_veronly"))))
		{
			TupleGenConst(4, 1, &hv_tMarkPos);
			hv_tMarkPos[1] = 0;
			hv_tMarkPos[3] = 0;
			////SelectFourMark(ho_hImageMeans, ho_hRegion, ho_hMaybeMarks, &(*ho_hMarkRegions),
			//	hv_tMarkPos, hv_UniqueScope, hv_Accuracy, &(*hv_tMarkChannels), &(*hv_tMarkFinded));
			//SelectOneMark(ho_hImageMeans, ho_hRegion, ho_hMaybeMarks, &ho_hMarkRegions1,
			//	-1, hv_Accuracy, &(*hv_tMarkChannels));
			//ConcatObj((*ho_hMarkRegions), ho_hMarkRegions1, &(*ho_hMarkRegions));
		}
		GenEmptyObj(&ho_RegionUnion);
		GenEmptyObj(&ho_Marks);
		//计算的到的mark数量
		CountObj((*ho_hMarkRegions), &hv_Number3);

		{
			HTuple end_val56 = hv_Number3;
			HTuple step_val56 = 1;
			for (hv_Index2 = 1; hv_Index2.Continue(end_val56, step_val56); hv_Index2 += step_val56)
			{
				hv_i = 0;
				hv_a = 0;
				//计算mark的角度以及长宽比
				SelectObj((*ho_hMarkRegions), &ho_SubMarkRegion, hv_Index2);
				SmallestRectangle2(ho_SubMarkRegion, &hv_Row, &hv_Column, &hv_Phi, &hv_Length1,
					&hv_Length2);
				GenRectangle2(&ho_Rectangle, hv_Row, hv_Column, hv_Phi, hv_Length1, hv_Length2);
				//gen_rectangle2 (Rectangle1, Row, Column, 1.57, Length1, Length2)
				//smallest_rectangle1 (Rectangle, Row1, Column1, Row2, Column2)
				ho_Marks = ho_SubMarkRegion;
				hv_AspectRatio = hv_Length1 / hv_Length2;
				//如果mark长宽比大于3则说明可能不合理
				if (0 != (int(hv_AspectRatio > 3)))
				{
					GenEmptyObj(&ho_EmptyObject);
					ho_Marks = ho_EmptyObject;
					//通过mark的长宽比对mark进行拆分拆分按照占比较短的一侧进行拆分

					PartitionRectangle(ho_SubMarkRegion, &ho_Partitioned, hv_Length2 * hv_AspectRatio / 2,
						hv_Length2 * hv_AspectRatio / 2);
					//计算拆分后的轮廓区域数量
					CountObj(ho_Partitioned, &hv_Number1);
					//对拆分后的轮廓区域进行筛选
					{
						HTuple end_val76 = hv_Number1;
						HTuple step_val76 = 1;
						for (hv_Index = 1; hv_Index.Continue(end_val76, step_val76); hv_Index += step_val76)
						{
							//选择拆分后的轮廓
							SelectObj(ho_Partitioned, &ho_ObjectSelected, hv_Index);
							SmallestRectangle2(ho_ObjectSelected, &hv_Row3, &hv_Column3, &hv_Phi2, &hv_Length11,
								&hv_Length21);
							//如果为空则直接跳出，不知道为什么会为空
							if (0 != (int(hv_Row3 != 0)))
							{
								//打散选择的轮廓，因为有可能拆分后的轮廓没有连接是两个竖线
								Connection(ho_ObjectSelected, &ho_ConnectedRegions);
								CountObj(ho_ConnectedRegions, &hv_Number2);
								//再次对拆分后的轮廓进行筛选
								{
									HTuple end_val86 = hv_Number2;
									HTuple step_val86 = 1;
									for (hv_Index1 = 1; hv_Index1.Continue(end_val86, step_val86); hv_Index1 += step_val86)
									{
										SelectObj(ho_ConnectedRegions, &ho_ObjectSelected1, hv_Index1);
										SmallestRectangle2(ho_ObjectSelected1, &hv_Row4, &hv_Column4, &hv_Phi3,
											&hv_Length12, &hv_Length22);
										//拆分后的轮廓逐个进行长宽比以及角度的判断如果满足，拆分后的轮廓长宽比小于6或者和拆分前的角度相反则进行选定。
										hv_AspectRatio22 = hv_Length12 / hv_Length22;
										hv_N = hv_AspectRatio / 2;
										TupleAbs(hv_Phi, &hv_Phi);
										TupleAbs(hv_Phi3, &hv_Phi3);
										hv_Phi4 = hv_Phi - hv_Phi3;
										TupleAbs(hv_Phi4, &hv_Phi4);
										if (0 != (HTuple(int(hv_AspectRatio22 < hv_N)).TupleOr(int((hv_Phi4 > 0.3)))))
										{
											Union2(ho_ObjectSelected1, ho_Marks, &ho_Marks);
											hv_a[hv_i] = hv_Index;
											hv_i += 1;
										}
									}
								}
							}
						}
					}
					hv_a1 = hv_a - 1;
					HObject	test;
					HTuple L_a;
					if (hv_a1 != -1) {
						TupleGenSequence(1, hv_Number1, 1, &hv_Sequence);
						TupleRemove(hv_Sequence, hv_a, &hv_Reduced);
						tuple_shuffle(hv_Reduced, &hv_Shuffled);
						hv_lenth = hv_Number1 / 3;
						TupleLength(hv_a, &L_a);
						if (hv_lenth > 1&& hv_lenth> L_a)
						{
							TupleSelectRange(hv_Shuffled, 0, hv_lenth, &hv_Selected);
							TupleConcat(hv_Selected, hv_a, &hv_Concat);
							SelectObj(ho_Partitioned, &ho_Marks, hv_Concat);

						}
						else
						{
							SelectObj(ho_Partitioned, &ho_Marks, hv_a);
						}
						Union1(ho_Marks, &ho_Marks);
					}
					else
					{
						ho_Marks = ho_SubMarkRegion;
					}
				}

				else
				{
					ho_Marks = ho_SubMarkRegion;
				}

				ConcatObj(ho_Marks, ho_RegionUnion, &ho_RegionUnion);
			}
		}
		(*ho_hMarkRegions) = ho_RegionUnion;
		return;
	}

	int realAutoMark(const autoMarkInput& input, autoMarkOutput& output) {

		HObject ho_hImage = input.Himg;
		HObject ho_hRegion, Hregion1;//输入自动定位mark的范围
		HObject ho_hMarkRegions;//输出得到的mark区域
		HObject	ho_Maybemark;//候选mark区域
		AlgParamAutoMark* alg = (AlgParamAutoMark*)&input.algPara;
		HTuple MinArea = alg->i_params.block.MinArea;
		HTuple MaxArea = alg->i_params.block.MaxArea;
		HTuple hv_tMarkChannels, hv_tMarkFinded;
		try {

			HTuple IsOk;
			Hregion1 = input.Hregion;
			GenEmptyObj(&ho_hRegion);
			IsOk = Hregion1.IsInitialized();

			if ((input.posFindROI.size() != 0) || (IsOk == 1))
			{
				if (input.posFindROI.size() != 0) {
					GenRegionFromContoursInput in2;
					RegionsOutput out2;
					in2.vecContourPts = input.posFindROI;
					int r = GenRegionFromContours(in2, out2);
					ho_hRegion = out2.hRegion;
				}
				else {

					ho_hRegion = input.Hregion;

				}
			}
			else
			{
				GetDomain(ho_hImage, &ho_hRegion);
				ErosionRectangle1(ho_hRegion, &ho_hRegion, 10, 10);
			}
			FindAutoMark(ho_hImage, ho_hRegion, &ho_hMarkRegions, &ho_Maybemark, 20, 1, MinArea, MaxArea, "4mark_maxarea", &hv_tMarkChannels, &hv_tMarkFinded);
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}
		HTuple Area, Col, Row, LN;
		AreaCenter(ho_hMarkRegions, &Area, &Col, &Row);
		TupleLength(Area, &LN);
		if (LN == 0)
		{
			return 4;
		}
		output.SubMarkRegion = ho_hMarkRegions;
		output.Maybemark = ho_Maybemark;
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int autoMark(const autoMarkInput& input, autoMarkOutput& output) {
		try
		{
			return realAutoMark(input, output);
		}
		catch (...)
		{
			return 3; //算法内部异常
		}

	}
}
