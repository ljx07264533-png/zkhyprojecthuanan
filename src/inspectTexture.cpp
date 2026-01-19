#include "alglibrary/zkhyProjectHuaNan/inspectTexture.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/creatTexture.h"
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
	void defectRegionTimeProcess(HTuple hv_starttime, HTuple hv_overtimeTh, HTuple* hv_overtimeFlag)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_endtime, hv_duration;

		CountSeconds(&hv_endtime);
		hv_duration = hv_endtime - hv_starttime;
		//printf("Inner Time: %f \r\n", hv_duration.D());
		if (0 != (hv_duration > hv_overtimeTh))
		{
			(*hv_overtimeFlag) = 1;
		}

		return;
	}

	int inspectDefect(HObject ho_Image, HObject BmageModel, HObject DmageModel, HObject ho_InsRegion1, HObject Cregions2,
		HObject* ho_DefectRegion, HTuple hv_AngleStart, HTuple hv_AngleExtend, HTuple hv_FusionDist,
		HObject ho_FindtRegionROI, HTuple hv_startRow, HTuple hv_structure, HTuple hv_NumLevels,
		HTuple hv_ModelID1, HTuple hv_modelRow, HTuple hv_modelColumn,
		HTuple hv_MinScore, HTuple hv_NumMatches, HTuple hv_MinArea, HTuple hv_threshold1, HTuple hv_threshold2,
		HTuple hv_threshold3, HTuple hv_threshold4, HTuple hv_opening, HTuple hv_Scale, HTuple hv_overtimeTh,
		HTuple with, HTuple* hv_DefectContoursRow, HTuple* hv_DefectContoursColumn, HTuple* hv_Rows,
		HTuple* hv_Columns, HTuple* hv_Len1s, HTuple* hv_Len2s, HTuple* hv_Areas, HTuple* hv_row1s,
		HTuple* hv_col1s, HTuple* hv_row2s, HTuple* hv_col2s, HTuple* hv_row3s, HTuple* hv_col3s,
		HTuple* hv_row4s, HTuple* hv_col4s, HTuple* hv_overtimeFlag, HObject* DisBImageModel, HObject* DisDImageModel)

	{

		SetSystem("border_shape_models", "true");
		// Local iconic variables
		HObject  ho_EmptyObject1, ho_EmptyObject, ho_Contour, ho_Contour2;
		HObject  ho_FindRegionROI, ho_Rectangle1, ho_ImageReduced;
		HObject  ho_ImagePartTest, ho_ImageReduced3, ho_ImageReduced4, ho_BmageModel, ho_DmageModel;
		HObject  ho_ImagePart1, ho_ImagePart3, ho_Rectangle, ho_ImageReduced1, ho_ImagePart2;
		HObject  ho_ImageSub, ho_ImageAbs, ho_ImageModel, ho_ImageTest;
		HObject  D_Region2, D_Region3, ho_ROI_05, ho_ROI_06, ho_ConnectedRegions1;
		HObject  ho_SelectedRegion1, ho_RegionClosing, ho_RegionUnion;
		HObject  ho_ConnectedRegions2, ho_ObjectSelected;

		// Local control variables
		HTuple  hv_Width, hv_Height, hv_modelH, hv_HomMat2D, hv_GenParamValue;
		HTuple  hv_num, hv_Index, hv_Width2, hv_Height2, hv_H1;
		HTuple  hv_Row1, hv_Column1, hv_Angle1, Row, Clo, hv_Score1;
		HTuple  hv_col, hv_Mean, hv_Deviation, hv_finalRow, hv_finalCol, hv_MeanMin, hv_row;
		HTuple  hv_Length1, hv_Length2, hv_HomMat2DIdentity, hv_HomMat2DScale2Ori;
		HTuple  hv_i, hv_Row11, hv_Column11, hv_Phi, hv_Length11;
		HTuple  hv_Length12, hv_Cos, hv_Sin, hv_pt_x, hv_pt_y, hv__, hv_starttime;
		HObject ho_ModelContours1, SerchContours, TmpContoursRegion;


		(*hv_overtimeFlag) = 0;
		CountSeconds(&hv_starttime);

		GenEmptyObj(&(*ho_DefectRegion));
		GenEmptyObj(&ho_EmptyObject1);
		GenEmptyObj(&ho_EmptyObject);
		GetImageSize(ho_Image, &hv_Width, &hv_Height);
		GetImageSize(BmageModel, &hv_Width, &hv_modelH);
		//查找模型1ROI创建

		HTuple Number;

		CountObj(ho_FindtRegionROI, &Number);
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		HomMat2dScale(hv_HomMat2DIdentity, hv_Scale, hv_Scale, 0, 0, &hv_HomMat2DScale2Ori);//缩放检测

		VectorAngleToRigid(0, 0, 0, -hv_startRow, 0, 0, &hv_HomMat2D);


		hv_num = (hv_startRow.D() / hv_modelH.D());
		hv_num = hv_num.TupleCeil();
		{
			HTuple end_val13 = hv_Height - 1;
			HTuple step_val13 = hv_modelH;
			for (hv_Index = hv_startRow - (hv_num * hv_modelH); hv_Index.Continue(end_val13, step_val13); hv_Index += step_val13)
			{
				HTuple hv_Index3 = hv_Index + hv_modelH;
				GenRectangle1(&ho_Rectangle1, hv_Index, 0, hv_Index3 - 1, hv_Width - 1);
				ReduceDomain(ho_Image, ho_Rectangle1, &ho_ImageReduced);
				CropDomain(ho_ImageReduced, &ho_ImagePartTest);
				GetImageSize(ho_ImagePartTest, &hv_Width2, &hv_Height2);

				Row.Clear();
				Clo.Clear();
				for (int i = 1; i <= Number; i++)
				{
					SelectObj(ho_FindtRegionROI, &ho_FindRegionROI, i);
					DilationRectangle1(ho_FindRegionROI, &ho_FindRegionROI, with, with);
					//AffineTransRegion(ho_FindRegionROI, &ho_FindRegionROI, hv_HomMat2DScale2Ori, "nearest_neighbor");
					AffineTransRegion(ho_FindRegionROI, &ho_FindRegionROI, hv_HomMat2D, "nearest_neighbor");
					ReduceDomain(ho_ImagePartTest, ho_FindRegionROI, &ho_ImageReduced3);
					HTuple hv_ModelID, hv_HomMat2DM2;

					GetDictParam(hv_ModelID1, "keys", HTuple(), &hv_GenParamValue);
					GetDictTuple(hv_ModelID1, HTuple(hv_GenParamValue[i - 1]), &hv_ModelID);
					//get_shape_model_params (ModelID1, NumLevels, AngleStart, AngleExtent, AngleStep, ScaleMin, ScaleMax, ScaleStep, Metric, MinContrast)
					FindShapeModel(ho_ImageReduced3, hv_ModelID, hv_AngleStart.TupleRad(), hv_AngleExtend.TupleRad(),
						hv_MinScore, hv_NumMatches, 0.5, "least_squares", hv_NumLevels, 0, &hv_Row1, &hv_Column1,
						&hv_Angle1, &hv_Score1);
					HTuple hv_modelRow1 = hv_modelRow[i - 1];
					HTuple hv_modelColumn1 = hv_modelColumn[i - 1];
					GetShapeModelContours(&ho_ModelContours1, (hv_ModelID), 1);
					VectorAngleToRigid(0, 0, 0, (hv_Row1), (hv_Column1), hv_Angle1,
						&hv_HomMat2DM2);

					AffineTransContourXld(ho_ModelContours1, &ho_ModelContours1, hv_HomMat2DM2);

					Row.Append(hv_Row1);
					Clo.Append(hv_Column1);
				}
				//当在待测图中匹配到对应的mark时
				if (0 != ((Row.TupleLength()) > 0))
				{
					ho_BmageModel = BmageModel;
					ho_DmageModel = DmageModel;
					//将待测图 向模板图的匹配方向 进行变换
					VectorToRigid(hv_modelRow, hv_modelColumn, Row, Clo, &hv_HomMat2D);
					AffineTransImage(ho_BmageModel, &ho_BmageModel, hv_HomMat2D, "constant", "false");
					AffineTransImage(ho_DmageModel, &ho_DmageModel, hv_HomMat2D, "constant", "false");
					ho_ImagePart1 = ho_ImagePartTest;
				}
				else
				{
					ho_ImagePart1 = ho_ImagePartTest;
				}
				//当顶部的图像 或者底部的图像尺寸小于模板图时需要将模板图 进行裁剪 然后进行后面的灰度匹配
				HObject	ho_ImagePartMax, ho_ImagePartMin, ho_ImageSub1, ho_ImageSub2;
				SubImage(ho_ImagePart1, ho_DmageModel, &ho_ImageSub, 1, 1);
				//黑色缺陷检测
				GrayErosionRect(ho_DmageModel, &ho_ImagePartMax, hv_structure, hv_structure);
				SubImage(ho_ImagePartMax, ho_ImagePart1, &ho_ImageSub1, 1, 1);
				GrayDilationRect(ho_ImageSub1, &ho_ImageSub1, hv_structure, hv_structure);
				Threshold(ho_ImageSub1, &D_Region2, hv_threshold3, hv_threshold4);
				//白色缺陷检测
				GrayDilationRect(ho_BmageModel, &ho_ImagePartMin, hv_structure, hv_structure);
				SubImage(ho_ImagePart1, ho_ImagePartMin, &ho_ImageSub2, 1, 1);
				GrayDilationRect(ho_ImageSub2, &ho_ImageSub2, hv_structure, hv_structure);
				Threshold(ho_ImageSub2, &D_Region3, hv_threshold1, hv_threshold2);
				Union2(D_Region2, D_Region3, &D_Region2);
				//测试使用
				SubImage(ho_ImagePart1, ho_ImagePartMin, &ho_ImageSub2, 1, 128);//白色缺陷
				SubImage(ho_ImagePartMax, ho_ImagePart1, &ho_ImageSub1, 1, 128);//黑色缺陷
				//将缺陷变换到 原始待测小图(如果找到待测小图到模板小图的转换关系）
				//将缺陷变换到 原始大图
				if (0 != (hv_Index > 0))
				{
					VectorAngleToRigid(0, 0, 0, hv_Index + 1, 0, 0, &hv_HomMat2D);
					//AffineTransRegion(D_Region2, &D_Region2, hv_HomMat2D, "nearest_neighbor");
				}
				if (0 != (hv_Index < 0))
				{

					Intersection(ho_ROI_06, D_Region2, &D_Region2);
				}
				ConcatObj(ho_EmptyObject, D_Region2, &ho_EmptyObject);
			}
		}
		defectRegionTimeProcess(hv_starttime, hv_overtimeTh, &(*hv_overtimeFlag));
		if (0 != ((*hv_overtimeFlag) == 1))
		{
			return 6;
		}
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		HomMat2dScale(hv_HomMat2DIdentity, 1 / hv_Scale, 1 / hv_Scale, 0, 0, &hv_HomMat2DIdentity);//缩放检测
		AffineTransRegion(ho_EmptyObject, &ho_EmptyObject, hv_HomMat2DIdentity, "nearest_neighbor");
		HTuple Height1, Width1;
		GetImageSize(ho_BmageModel, &Width1, &Height1);
		Width1 = Width1 / hv_Scale;
		Height1 = Height1 / hv_Scale;
		AffineTransImageSize(ho_BmageModel, &(*DisBImageModel), hv_HomMat2DIdentity, "nearest_neighbor", Width1, Height1);
		AffineTransImageSize(ho_DmageModel, &(*DisDImageModel), hv_HomMat2DIdentity, "nearest_neighbor", Width1, Height1);
		Intersection(Cregions2, ho_EmptyObject, &ho_EmptyObject);
		OpeningCircle(ho_EmptyObject, &(*ho_DefectRegion), hv_opening);
		Connection((*ho_DefectRegion), &ho_ConnectedRegions1);
		//按照面积 以及宽度（或者其他属性特征）进行区域的筛选，不满足区间范围的 缺陷将被剔除
		SelectShape(ho_ConnectedRegions1, &ho_SelectedRegion1, "area", "and", hv_MinArea,
			99999999);

		//Step2 缺陷距离融合
		//进行圆形膨胀运算 按照融合距离的一半去膨胀，当两个缺陷距离小于融合距离时，膨胀后也会合并到一起
		ho_RegionClosing = ho_SelectedRegion1;
		if (0 != (hv_FusionDist >= 1))
		{
			DilationCircle(ho_SelectedRegion1, &ho_RegionClosing, hv_FusionDist / 2);
		}
		//将膨胀后的区域进行合并
		Union1(ho_RegionClosing, &ho_RegionUnion);
		//将膨胀后的区域 进行打散 原始需要融合的区域 会合并为独立的区域
		Connection(ho_RegionUnion, &ho_ConnectedRegions2);
		//将合并到一起的膨胀后的缺陷区域 与原始缺陷区域求交集 可以得到融合后的区域
		Intersection(ho_ConnectedRegions2, ho_ConnectedRegions1, &(*ho_DefectRegion));
		//计算缺陷区域的面积特征 （或者其他属性的特征 都是通过该算子实现）
		//calculate_features (DefectRegion, Image, 'area', Area)
		//获取缺陷区域的最小外接矩形的中心点和角度信息


		(*hv_Len1s) = HTuple();
		(*hv_Len2s) = HTuple();
		(*hv_Areas) = HTuple();
		(*hv_Rows) = HTuple();
		(*hv_Columns) = HTuple();
		AreaCenter((*ho_DefectRegion), &(*hv_Areas), &(*hv_Rows), &(*hv_Columns));
		SmallestRectangle2((*ho_DefectRegion), &hv__, &hv__, &hv__, &(*hv_Len1s), &(*hv_Len2s));
		(*hv_row1s) = HTuple();
		(*hv_col1s) = HTuple();
		(*hv_row2s) = HTuple();
		(*hv_col2s) = HTuple();
		(*hv_row3s) = HTuple();
		(*hv_col3s) = HTuple();
		(*hv_row4s) = HTuple();
		(*hv_col4s) = HTuple();
		{
			HTuple end_val133 = (*hv_Rows).TupleLength();
			HTuple step_val133 = 1;
			for (hv_i = 1; hv_i.Continue(end_val133, step_val133); hv_i += step_val133)
			{
				SelectObj((*ho_DefectRegion), &ho_ObjectSelected, hv_i);
				SmallestRectangle2(ho_ObjectSelected, &hv_Row11, &hv_Column11, &hv_Phi, &hv_Length11,
					&hv_Length12);
				TupleCos(hv_Phi, &hv_Cos);
				TupleSin(hv_Phi, &hv_Sin);
				//dev_set_color ('green')
				hv_pt_x.Clear();
				hv_pt_x[0] = 0;
				hv_pt_x[1] = 0;
				hv_pt_x[2] = 0;
				hv_pt_x[3] = 0;
				hv_pt_y.Clear();
				hv_pt_y[0] = 0;
				hv_pt_y[1] = 0;
				hv_pt_y[2] = 0;
				hv_pt_y[3] = 0;
				hv_pt_x[0] = hv_Column11 - ((hv_Length11 * hv_Cos) + (hv_Length12 * hv_Sin));
				hv_pt_y[0] = hv_Row11 + ((hv_Length11 * hv_Sin) - (hv_Length12 * hv_Cos));
				//gen_cross_contour_xld (Cross, pt_y[0], pt_x[0], 6, Phi)

				hv_pt_x[1] = hv_Column11 + ((hv_Length11 * hv_Cos) - (hv_Length12 * hv_Sin));
				hv_pt_y[1] = hv_Row11 - ((hv_Length11 * hv_Sin) + (hv_Length12 * hv_Cos));
				//gen_cross_contour_xld (Cross, pt_y[1], pt_x[1], 6, Phi)

				hv_pt_x[2] = hv_Column11 + ((hv_Length11 * hv_Cos) + (hv_Length12 * hv_Sin));
				hv_pt_y[2] = hv_Row11 - ((hv_Length11 * hv_Sin) - (hv_Length12 * hv_Cos));
				//gen_cross_contour_xld (Cross, pt_y[2], pt_x[2], 6, Phi)

				hv_pt_x[3] = hv_Column11 + (((-hv_Length11) * hv_Cos) + (hv_Length12 * hv_Sin));
				hv_pt_y[3] = hv_Row11 - (((-hv_Length11) * hv_Sin) - (hv_Length12 * hv_Cos));
				//gen_cross_contour_xld (Cross, pt_y[2], pt_x[2], 6, Phi)
				(*hv_row1s) = (*hv_row1s).TupleConcat(HTuple(hv_pt_y[0]));
				(*hv_col1s) = (*hv_col1s).TupleConcat(HTuple(hv_pt_x[0]));
				(*hv_row2s) = (*hv_row2s).TupleConcat(HTuple(hv_pt_y[1]));
				(*hv_col2s) = (*hv_col2s).TupleConcat(HTuple(hv_pt_x[1]));
				(*hv_row3s) = (*hv_row3s).TupleConcat(HTuple(hv_pt_y[2]));
				(*hv_col3s) = (*hv_col3s).TupleConcat(HTuple(hv_pt_x[2]));
				(*hv_row4s) = (*hv_row4s).TupleConcat(HTuple(hv_pt_y[3]));
				(*hv_col4s) = (*hv_col4s).TupleConcat(HTuple(hv_pt_x[3]));
				if (0 != ((hv_i % 50) == 0))
				{
					defectRegionTimeProcess(hv_starttime, hv_overtimeTh, &(*hv_overtimeFlag));
					if (0 != ((*hv_overtimeFlag) == 1))
					{
						return 6;
					}
				}
			}

		}
		return 0;
	}

	int realInspectTexture(const InspectTextureInput& input, InspectTextureOutput& output)
	{

		Mat srcImg = input.srcImg;
		int re;
		HObject	a;
		a = input.shapeInfo.BmodelImg;
		Mat modImage = HObject2Mat(a);
		if (srcImg.rows == 0 || srcImg.cols == 0)
			return 1;

		if (modImage.rows == 0 || modImage.cols == 0)
		{
			return 7;
		}

		//	if (Imgpart.rows == 0 || Imgpart.cols == 0)
			//	return 1;


		HObject BImageModel = input.shapeInfo.BmodelImg;//加载图像
		HObject DImageModel = input.shapeInfo.DmodelImg;
		HObject ho_Testproc = Mat2HObject(srcImg);
		HObject ho_FindtRegionROI;//查找轮廓ROI
		HObject DisBImageModel, DisDImageModel;
		HTuple Number;//区域个数

		AlgParamCreatTexture* alg = (AlgParamCreatTexture*)&input.alg;
		HTuple hv_Scale = alg->d_params.block.scal;
		HTuple hv_AngleStart = alg->d_params.block.angleStart;//匹配起始角度
		HTuple hv_AngleExtend = alg->d_params.block.angleExtend;//匹配终止角度
		HTuple hv_MinScore = alg->d_params.block.minScore;//匹配最小置信度
		HTuple NumMatches = alg->i_params.block.numMatches;//匹配个数
		HTuple hv_MinArea = alg->d_params.block.minArea;//缺陷最小面积
		HTuple hv_overtimeTh = alg->d_params.block.overTimeTh / 1000.0;
		HTuple r_offset, hv_HomMat2DIdentity1, hv_HomMat2DScale2Ori1, hv_HomMat2DIdentity2;
		HTuple with = alg->i_params.block.FindRoi;
		with = with * hv_Scale;
		if (alg->d_params.block.overTimeTh <= 0.0)
			hv_overtimeTh = 3.0;

		HTuple hv_startRow = 0;//偏移量	

		HTuple hv_modelColumn = input.shapeInfo.hv_modelColumn3;//行模型偏移量		
		HTuple hv_modelRow = input.shapeInfo.hv_modelRow3;//列模型偏移量

		HObject  ho_ModelContours3, ho_ModelContours4, SFindtRegionROI;
		HomMat2dIdentity(&hv_HomMat2DIdentity1);
		HomMat2dIdentity(&hv_HomMat2DIdentity2);
		HomMat2dScale(hv_HomMat2DIdentity1, hv_Scale, hv_Scale, 0, 0, &hv_HomMat2DScale2Ori1);//缩放检测
		HomMat2dScale(hv_HomMat2DIdentity2, 1 / hv_Scale, 1 / hv_Scale, 0, 0, &hv_HomMat2DIdentity2);//缩放检测


		try {

			GenEmptyObj(&ho_FindtRegionROI);
			GenEmptyObj(&SFindtRegionROI);
			if (input.posFindROI.size() != 0) {
				GenRegionFromContoursInput in2;
				RegionsOutput out2;
				in2.vecContourPts = input.posFindROI;
				int r = GenRegionFromContours(in2, out2);
				SFindtRegionROI = out2.hRegion;
				Connection(SFindtRegionROI, &SFindtRegionROI);

				AffineTransRegion(SFindtRegionROI, &ho_FindtRegionROI, hv_HomMat2DScale2Ori1, "nearest_neighbor");

				CountObj(ho_FindtRegionROI, &Number);
				int a = input.posFindROI.size();
				if (a > Number[0])
				{
					return 9;
				}
			}
			else {

				ho_FindtRegionROI = input.shapeInfo.findRegionROI;

				//DilationRectangle1(ho_FindtRegionROI,&ho_FindtRegionROI,with,with);
				Connection(ho_FindtRegionROI, &ho_FindtRegionROI);
				AffineTransRegion(ho_FindtRegionROI, &SFindtRegionROI, hv_HomMat2DIdentity2, "nearest_neighbor");

			}

			CountObj(ho_FindtRegionROI, &Number);
			if (Number < 2)
			{
				return  3;

			}
			//		AffineTransRegion(ho_FindtRegionROI, &ho_FindtRegionROI, hv_HomMat2DScale2Ori1, "nearest_neighbor");
		}

		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}
		output.SFindtRegionROI = SFindtRegionROI;
		HTuple hv_ModelID1 = input.shapeInfo.modelID;//模型1ID
		//GetShapeModelContours(&ho_ModelContours3, hv_ModelID1, 1);
		HObject Cregions1 = input.shapeInfo.cregions1;//检测区域

		HTuple hv_threshold1 = alg->d_params.block.threshold1;//亮缺陷高阈值
		HTuple hv_threshold3 = alg->d_params.block.threshold3;
		HTuple hv_opening = alg->d_params.block.opening;
		HTuple hv_structure = alg->i_params.block.StrucTure;
		HTuple hv_NumLevels = alg->i_params.block.NumLevels;
		if (hv_threshold3 < 0 || hv_threshold1 < 0)
		{
			return 8;
		}

		if (hv_MinArea < 0 || hv_MinScore < 0 || NumMatches < 0 || hv_Scale <= 0) {
			return 4;
		}


		HObject DefectRegion2;//缺陷区域
		HObject Cregions2;//缺陷区域

		try {
			GenEmptyObj(&Cregions2);
			if (input.cregions2.IsInitialized()) {
				Cregions2 = input.cregions2;
			}
			else {
				GetDomain(ho_Testproc, &Cregions2);
			}

		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			//throw except;
			return 5;
		}



		HTuple hv_HomMat2DIdentity, hv_HomMat2DScale2Ori;
		//	HObject ho_ImagePart = input.shapeInfo.modelImg;
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		HomMat2dScale(hv_HomMat2DIdentity, hv_Scale, hv_Scale, 0, 0, &hv_HomMat2DScale2Ori);//缩放检测
		//AffineTraneImage();
		AffineTransImage(ho_Testproc, &ho_Testproc, hv_HomMat2DScale2Ori, "constant", "false");
		CropDomain(ho_Testproc, &ho_Testproc);

		HTuple hv_FusionDist = alg->d_params.block.fusionDist;//缺陷融合距离

		HTuple hv_DefectContoursRow1, hv_DefectContoursColumn1;
		HTuple hv_Rows, hv_Columns, hv_Len1s, hv_Len2s, hv_Areas, hv_row1s, hv_col1s, hv_row2s, hv_col2s;
		HTuple hv_row3s, hv_col3s, hv_row4s, hv_col4s;
		HTuple hv_overtimeFlag;


		try {

			re = inspectDefect(ho_Testproc, BImageModel, DImageModel, Cregions1, Cregions2, &DefectRegion2, hv_AngleStart,
				hv_AngleExtend, hv_FusionDist, ho_FindtRegionROI, hv_startRow, hv_structure, hv_NumLevels,
				hv_ModelID1, hv_modelRow, hv_modelColumn,
				hv_MinScore, NumMatches, hv_MinArea, hv_threshold1, 255, hv_threshold3, 255,
				hv_opening, hv_Scale, hv_overtimeTh, with,
				&hv_DefectContoursRow1, &hv_DefectContoursColumn1, &hv_Rows, &hv_Columns, &hv_Len1s,
				&hv_Len2s, &hv_Areas, &hv_row1s, &hv_col1s, &hv_row2s,
				&hv_col2s, &hv_row3s, &hv_col3s, &hv_row4s, &hv_col4s,
				&hv_overtimeFlag, &DisBImageModel, &DisDImageModel);

			output.DisBImageModel = DisBImageModel;
			output.DisDImageModel = DisDImageModel;
			if (hv_overtimeFlag == 1)

				return 6;
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}

		output.defectRegion = DefectRegion2;
		for (int i = 0; i < hv_row1s.TupleLength(); i++)
		{
			FlawInfoStruct flawinfo;
			flawinfo.FlawBasicInfo.centerPt = Point2f((float)hv_Columns[i].D(), (float)hv_Rows[i].D());                     //缺陷坐标
			flawinfo.FlawBasicInfo.flawWidth = 2 * (hv_Len1s[i].D() + 0.5);
			flawinfo.FlawBasicInfo.flawHeight = 2 * (hv_Len2s[i].D() + 0.5);
			flawinfo.FlawBasicInfo.flawArea = hv_Areas[i].D();
			flawinfo.FlawBasicInfo.flawArea = hv_Areas[i].D();
			flawinfo.FlawBasicInfo.pts.push_back(Point2f((float)hv_col1s[i].D(), (float)hv_row1s[i].D()));
			flawinfo.FlawBasicInfo.pts.push_back(Point2f((float)hv_col2s[i].D(), (float)hv_row2s[i].D()));
			flawinfo.FlawBasicInfo.pts.push_back(Point2f((float)hv_col3s[i].D(), (float)hv_row3s[i].D()));
			flawinfo.FlawBasicInfo.pts.push_back(Point2f((float)hv_col4s[i].D(), (float)hv_row4s[i].D()));
			flawinfo.FlawBasicInfo.pts.push_back(Point2f((float)hv_col1s[i].D(), (float)hv_row1s[i].D()));
			output.flawinfos.push_back(flawinfo);
		}
		//提取缺陷的所有区域轮廓信息
		if (0 == 1) {
			HObject  ho_ObjectSelected, ho_ConnectedRegions, ho_ObjectSelectedj;
			// Local control variables
			HTuple  hv_Number, hv_i, hv_Number1, hv_j;

			CountObj(DefectRegion2, &hv_Number);
			if (0 != (hv_Number > 0)) {
				HTuple end_val8 = hv_Number;
				HTuple step_val8 = 1;
				for (hv_i = 1; hv_i.Continue(end_val8, step_val8); hv_i += step_val8) {
					SelectObj(DefectRegion2, &ho_ObjectSelected, hv_i);
					Connection(ho_ObjectSelected, &ho_ConnectedRegions);
					CountObj(ho_ConnectedRegions, &hv_Number1);
					if (0 != (hv_Number1 > 0)) {
						HTuple end_val13 = hv_Number1;
						HTuple step_val13 = 1;
						for (hv_j = 1; hv_j.Continue(end_val13, step_val13); hv_j += step_val13) {
							HTuple  hv_Columns, hv_Rows;
							SelectObj(ho_ConnectedRegions, &ho_ObjectSelectedj, hv_j);
							GetRegionPolygon(ho_ObjectSelectedj, 1, &hv_Rows, &hv_Columns);
							vector<Point2f> contour;
							for (int kk = 0; kk < hv_Rows.TupleLength().I(); kk++) {
								contour.push_back(Point2f((float)hv_Columns[kk].D(), (float)hv_Rows[kk].D()));
							}
							output.flawinfos[hv_i.I() - 1].FlawBasicInfo.flawContours.push_back(contour);
						}
					}
				}
			}
		}

		return re;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	//----------------------------------------------inspectTexture--------------------------//
	//功能：纹理检测异常缺陷函数入口
	int inspectTexture(const InspectTextureInput& input, InspectTextureOutput& output)
	{
		try
		{
			return realInspectTexture(input, output);
		}
		catch (...)
		{
			return 5; //算法内部异常
		}
	}

}
