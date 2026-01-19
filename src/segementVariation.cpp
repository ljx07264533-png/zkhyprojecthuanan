#include "alglibrary/zkhyProjectHuaNan/segementVariation.h"
#include "alglibrary/zkhyProjectHuaNan/autoMark.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
	int CreatModelxld(HObject ho_Image, HObject ho_markRegion, HObject* ho_image, HTuple hv_File,
		HTuple* hv_row, HTuple* hv_clo, HTuple* hv_DictHandle, HTuple* hv_Disx)
	{

		// Local iconic variables
		HObject  ho_temp, ho_ObjectSelected1, ho_ImageReduced5;
		HObject  ho_RegionDilation, ho_ImageReduced6, ho_ModelContours1;
		HObject  ho_ContoursAffineTrans1, ho_Region5;

		// Local control variables
		HTuple  hv_Number1, hv_Index1, hv_ModelID4, hv_Row5;
		HTuple  hv_Column5, hv_Angle5, hv_Score5, hv_HomMat2D5;
		HTuple  hv_Row2, hv_Col, hv_Model;
		try {		
		CountObj(ho_markRegion, &hv_Number1);
		GenEmptyRegion(&ho_temp);
		CreateDict(&(*hv_DictHandle));
		{
			HTuple end_val3 = hv_Number1;
			HTuple step_val3 = 1;
			for (hv_Index1 = 1; hv_Index1.Continue(end_val3, step_val3); hv_Index1 += step_val3)
			{
				SelectObj(ho_markRegion, &ho_ObjectSelected1, hv_Index1);
				ReduceDomain(ho_Image, ho_ObjectSelected1, &ho_ImageReduced5);
				CreateShapeModel(ho_ImageReduced5, "auto", -0.39, 0.79, "auto", "auto", "use_polarity",
					"auto", "auto", &hv_ModelID4);
				//set_shape_model_origin (ModelID4, Index1, Index1)

				//DilationCircle(ho_ObjectSelected1, &ho_RegionDilation, 500);
				ShapeTrans(ho_ObjectSelected1, &ho_RegionDilation, "convex");
				DilationCircle(ho_RegionDilation, &ho_RegionDilation, 200);

				ReduceDomain(ho_Image, ho_RegionDilation, &ho_ImageReduced6);
				FindShapeModel(ho_ImageReduced6, hv_ModelID4, -0.39, 0.79, 0.5, 1, 0, "least_squares",
					4, 0.9, &hv_Row5, &hv_Column5, &hv_Angle5, &hv_Score5);
				GetShapeModelContours(&ho_ModelContours1, hv_ModelID4, 1);

				VectorAngleToRigid(0, 0, 0, hv_Row5, hv_Column5, hv_Angle5, &hv_HomMat2D5);
				AffineTransContourXld(ho_ModelContours1, &ho_ContoursAffineTrans1, hv_HomMat2D5);
				(*hv_row)[hv_Index1 - 1] = hv_Row5;
				(*hv_clo)[hv_Index1 - 1] = hv_Column5;
				hv_Model[hv_Index1 - 1] = hv_ModelID4;
				SetDictTuple((*hv_DictHandle), hv_Index1, hv_ModelID4);
			}
		}
		HTuple MaxCol, MinCol;
		TupleMax(*hv_clo, &MaxCol);
		TupleMin(*hv_clo, &MinCol);
		*hv_Disx = MaxCol - MinCol;
		WriteDict((*hv_DictHandle), hv_File + "FDHandle", HTuple(), HTuple());
		WriteObject(ho_markRegion, hv_File + "FRegion");
		WriteTuple((*hv_row), hv_File + "Frow");
		WriteTuple((*hv_clo), hv_File + "Fclo");
		WriteTuple((*hv_Disx), hv_File + "FDisx");
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}
		return 0;
	}

	void NNinter_COPY_2(HTuple hv_RCdata, HTuple* hv_resultData, HTuple* hv_resultIndex)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_Indices, hv_i, hv_Index, hv_Value;
		HTuple  hv_Nequal1, hv_NotEqual, hv_Newtuple, hv_Diff, hv_Indices1;
		HTuple  hv_Min;

		TupleFind(hv_RCdata, -1, &hv_Indices);
		if (0 != (int(hv_Indices == -1)))
		{
			(*hv_resultData) = hv_RCdata;
			return;
		}
		(*hv_resultData) = hv_RCdata;
		(*hv_resultIndex) = hv_Indices;
		{
			HTuple end_val7 = (hv_Indices.TupleLength()) - 1;
			HTuple step_val7 = 1;
			for (hv_i = 0; hv_i.Continue(end_val7, step_val7); hv_i += step_val7)
			{
				TupleSelect(hv_Indices, hv_i, &hv_Index);
				if (0 != (int(hv_Index == 0)))
				{
					TupleSelect(hv_RCdata, 1, &hv_Value);
				}
				else
				{
					TupleSelect(hv_RCdata, hv_Index - 1, &hv_Value);
					(*hv_resultIndex)[hv_i + (hv_Indices.TupleLength())] = hv_Index - 1;
				}
				if (0 != (int(hv_Value == -1)))
				{
					TupleNotEqualElem(hv_RCdata, -1, &hv_Nequal1);
					TupleFind(hv_Nequal1, 1, &hv_NotEqual);
					TupleGenConst(hv_NotEqual.TupleLength(), hv_Index, &hv_Newtuple);

					TupleSub(hv_NotEqual, hv_Newtuple, &hv_Diff);
					TupleAbs(hv_Diff, &hv_Diff);
					TupleSortIndex(hv_Diff, &hv_Indices1);
					TupleSelect(hv_NotEqual, HTuple(hv_Indices1[0]), &hv_Min);
					TupleSelect(hv_RCdata, hv_Min, &hv_Value);
					(*hv_resultIndex)[hv_i + (hv_Indices.TupleLength())] = hv_Min;
				}
				(*hv_resultData)[hv_Index] = hv_Value;
			}
		}

		return;
	}


	int PreciseModelXld(HObject ho_Image, HObject* ho_Partitioned, HObjectVector/*{eObjectVector,Dim=1}*/* hvec_Onemark,
		HTuple hv_NWidth, HTuple hv_NHeight, HTuple hv_File, HTuple hv_MinArea, HTuple hv_MaxArea,
		HTuple* hv_col1, HTuple* hv_row1, HTuple* hv_CRIndices, HTuple* hv_BDictHandle,
		HTuple* hv_BMregion)
	{

		// Local iconic variables
		HObject  ho_Onemark2, ho_Domain, ho_ISFalse, ho_SegmentRegion;
		HObject  ho_ImageReduced, ho_Domain1, ho_SubMarkRegion, ho_CreatReduced1, ho_Maybemark;
		HObject  ho_SreachRegion, ho_SreachReduce, ho_ModelContours;
		HObject  ho_ContoursAffineTrans;

		// Local control variables
		HTuple  hv_Width, hv_Height, hv_width, hv_heigth;
		HTuple  hv_Number, hv_Row, hv_I, hv_Index1, hv_Index, hv_tMarkChannels;
		HTuple  hv_tMarkFinded, hv_IsEqual, hv_row, hv_col, hv_ModelID;
		HTuple  hv_Column, hv_Angle, hv_Score, hv_HomMat2D, hv_tx;
		HTuple  hv_ty;
		try {


			//存储mark
			GenEmptyRegion(&ho_Onemark2);
			//创建存储model的dict
			CreateDict(&(*hv_BDictHandle));
			CreateDict(&(*hv_BMregion));
			//获取图像区域按照输入的格子进行划分
			GetDomain(ho_Image, &ho_Domain);
			GetImageSize(ho_Image, &hv_Width, &hv_Height);
			hv_width = (hv_Width * 1.0) / hv_NWidth;
			hv_heigth = (hv_Height * 1.0) / hv_NHeight;
			PartitionRectangle(ho_Domain, &(*ho_Partitioned), hv_width, hv_heigth);
			CountObj((*ho_Partitioned), &hv_Number);
			//定义一个空区域用来判断区域是否为空
			GenEmptyObj(&ho_ISFalse);
			hv_Row = HTuple();
			hv_I = 0;
			//行和列进行划分I为总变量

			{
				HTuple end_val18 = hv_NHeight;
				HTuple step_val18 = 1;
				for (hv_Index1 = 1; hv_Index1.Continue(end_val18, step_val18); hv_Index1 += step_val18)
				{
					{
						HTuple end_val19 = hv_NWidth;
						HTuple step_val19 = 1;
						for (hv_Index = 1; hv_Index.Continue(end_val19, step_val19); hv_Index += step_val19)
						{
							//根据划分区域去进行选择并在划分的区域内查找中心mark一个里边找一个

							SelectObj((*ho_Partitioned), &ho_SegmentRegion, hv_I + 1);
							DilationRectangle1(ho_SegmentRegion, &ho_SegmentRegion, 400, 400);
							ReduceDomain(ho_Image, ho_SegmentRegion, &ho_ImageReduced);
							CropDomain(ho_ImageReduced, &ho_ImageReduced);
							GetDomain(ho_ImageReduced, &ho_Domain1);
							FindAutoMark(ho_ImageReduced, ho_Domain1, &ho_SubMarkRegion, &ho_Maybemark, 20, 1, hv_MinArea, hv_MaxArea, "1mark_center",
								&hv_tMarkChannels, &hv_tMarkFinded);

							TestEqualObj(ho_SubMarkRegion, ho_ISFalse, &hv_IsEqual);
							hv_Row = HTuple();
							//判断是否找到了可以创建的mark如果没找到则把这个row和clo以及ModelID置为-1，
							if (0 != hv_IsEqual)
							{
								hv_row[hv_I] = -1;
								hv_col[hv_I] = -1;
								hv_ModelID = -1;
								SetDictTuple((*hv_BDictHandle), hv_I, hv_ModelID);
								(*hvec_Onemark)[hv_I] = HObjectVector(ho_SegmentRegion);
								SetDictObject(ho_Domain1, (*hv_BMregion), hv_I);
							}
							else
							{

								//找到了可以创建的mark则对其进行创建
								ReduceDomain(ho_ImageReduced, ho_SubMarkRegion, &ho_CreatReduced1);
								//膨胀500个像素进行查找mark查找

								//DilationRectangle1(ho_SubMarkRegion, &ho_SreachRegion, 300, 300);
								ShapeTrans(ho_SubMarkRegion, &ho_SreachRegion, "convex");
								DilationCircle(ho_SreachRegion, &ho_SreachRegion, 400);
								ReduceDomain(ho_ImageReduced, ho_SreachRegion, &ho_SreachReduce);
								//mark创建
								CreateShapeModel(ho_CreatReduced1, "auto", -0.39, 0.79, "auto", "auto", "use_polarity",
									"auto", "auto", &hv_ModelID);
								//创建的轮廓显示以及查找轮廓
								GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
								FindShapeModel(ho_SreachReduce, hv_ModelID, HTuple(1).TupleRad(), HTuple(5).TupleRad(),
									0.5, 1, 0, "least_squares", 4, 0, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
								hv_row[hv_I] = hv_Row;
								hv_col[hv_I] = hv_Column;
								//row[I] := Row+heigth*( Index1-1)
								//col[I] := Column+width*(Index-1)
								//这里不做补偿相当于为后边在其他图查找时直接进行了补偿

								//保存创建的model
								SetDictTuple((*hv_BDictHandle), hv_I, hv_ModelID);
							}
							if (0 != (int((hv_Row.TupleLength()) > 0)))
							{
								//对FindAutoMark找到的区域进行平移传出去为后续查找轮廓使用
								hv_tx = ((hv_heigth * 1.0) * (hv_Index1 - 1)) - 200;
								hv_ty = ((hv_width * 1.0) * (hv_Index - 1)) - 200;
								if (0 != (HTuple(int((hv_Index1 - 1) == 0)).TupleOr(int((hv_Index - 1) == 0))))
								{
									if (0 != (int((hv_Index1 - 1) == 0)))
									{
										hv_tx = 0;
									}
									if (0 != (int((hv_Index - 1) == 0)))
									{
										hv_ty = 0;
									}
								}
								//hom_mat2d_translate (BTomMat2D, tx, ty, HomMat2DTranslate)
								VectorAngleToRigid(0, 0, 0, hv_Row + hv_tx, hv_Column + hv_ty, hv_Angle, &hv_HomMat2D);
								AffineTransContourXld(ho_ModelContours, &ho_ContoursAffineTrans, hv_HomMat2D);

								MoveRegion(ho_SubMarkRegion, &ho_SubMarkRegion, hv_tx, hv_ty);

								(*hvec_Onemark)[hv_I] = HObjectVector(ho_SubMarkRegion);
								SetDictObject(ho_SubMarkRegion, (*hv_BMregion), hv_I);
							}
							hv_I += 1;
						}
					}
				}
			}
			//检查是否有没有mark的区域如果有则把最近的哪一个传递给他
			HTuple hv_CRIndices1;
			TupleFind(hv_col, -1, &hv_CRIndices1);
			NNinter_COPY_2(hv_col, &hv_col, &(*hv_CRIndices));
			//NNinter_COPY_2(hv_row, &hv_row, &(*hv_CRIndices));
			hv_col[hv_I] = hv_NWidth;
			hv_row[hv_I] = hv_NHeight;
			(*hv_col1) = hv_col;
			(*hv_row1) = hv_row;
			//查找小的轮廓区域
			WriteDict((*hv_BMregion), hv_File + "SMregion", HTuple(), HTuple());
			WriteDict((*hv_BDictHandle), hv_File + "SHModel", HTuple(), HTuple());
			WriteTuple((*hv_col1), hv_File + "Scol");
			WriteTuple((*hv_row1), hv_File + "Srow");
			WriteTuple((hv_CRIndices1), hv_File + "CIndice");
			WriteRegion((*ho_Partitioned), hv_File + "PRegion");
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}
		return 0;
	}

	void Position(HObject ho_Src, HObject ho_markRegion, HObject ho_Partitioned, HTuple hv_DictHandle,
		HTuple hv_BMrow2, HTuple hv_BMclo2, HTuple hv_BDictHandle, HTuple hv_BMregion,
		HTuple hv_col1, HTuple hv_row11, HTuple hv_col1Indices, HTuple hv_NW, HTuple hv_NH, HTuple disx,
		HTupleVector/*{eTupleVector,Dim=1}*/* hvec_HomDZM, HTupleVector/*{eTupleVector,Dim=1}*/* hvec_HomMZD,
		HTuple* hv_HomMat2D2, HTuple* hv_BTomMat2D)
	{

		// Local iconic variables
		HObject  ho_EmptyObject, ho_markRegionI, ho_RegionDilation1;
		HObject  ho_ImageReduced4, ho_ConnectedRegions, ho_segment;
		HObject  ho_ImageReduced5, ho_CreatRegion, ho_ImageReduced;
		HObject  ho_ModelContours1, ho_ContoursAffineTrans1, ho_ImageAffineTrans;

		// Local control variables
		HTuple  hv_Width, hv_Height, hv_width, hv_heigth;
		HTuple  hv_GenParamValue, hv_BGenParamValue, hv_BMGenParamValue;
		HTuple  hv_Number1, hv_Index, hv_hv_ModelID, hv_Row4, hv_Column4;
		HTuple  hv_Angle4, hv_Score4, hv_Trow, hv_Tclo2, hv_HomMat2D24, hv_BTomMat2D4;
		HTuple  hv_Number2, hv_Index1, hv_bol2, hv_BModelID, hv_Mod;
		HTuple  hv_Quot, hv_tx, hv_ty, hv_HomMat2DTranslate, hv_HomMat2DTranslate2;
		HTuple  hv_OSRow, hv_OSColumn, hv_Angle, hv_Score, hv_HomMat2D7;
		HTuple  hv_HomDZM1, hv_HomMZD1;
		HTuple  hv_offsetDZM1;
		HTuple  hv_HomMat2DTranslate1, hv_offsetMZD1;
		HTupleVector  hvec_offsetDZM(1), hvec_offsetMZD(1);
		GenEmptyObj(&ho_EmptyObject);
		GetImageSize(ho_Src, &hv_Width, &hv_Height);
		hv_width = (hv_Width * 1.0) / hv_NW;
		hv_heigth = (hv_Height * 1.0) / hv_NH;
		SetSystem("border_shape_models", "true");
		GetDictParam(hv_DictHandle, "keys", HTuple(), &hv_GenParamValue);
		//小图模板轮廓
		GetDictParam(hv_BDictHandle, "keys", HTuple(), &hv_BGenParamValue);
		//小图模板轮廓查找区域
		GetDictParam(hv_BMregion, "keys", HTuple(), &hv_BMGenParamValue);
		CountObj(ho_markRegion, &hv_Number1);
		//得到模板图的仿射变换信息
		{
			HTuple end_val12 = hv_Number1;
			HTuple step_val12 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val12, step_val12); hv_Index += step_val12)
			{
				HTuple Rlenth;
				GetDictTuple(hv_DictHandle, HTuple(hv_GenParamValue[hv_Index - 1]), &hv_hv_ModelID);
				SelectObj(ho_markRegion, &ho_markRegionI, hv_Index);
				ShapeTrans(ho_markRegionI, &ho_RegionDilation1, "convex");
				//DilationCircle(ho_RegionDilation1, &ho_RegionDilation1, 500);
				DilationRectangle1(ho_RegionDilation1, &ho_RegionDilation1, 400, 400);
				ReduceDomain(ho_Src, ho_RegionDilation1, &ho_ImageReduced4);
				GetShapeModelContours(&ho_ModelContours1, hv_hv_ModelID, 1);

				FindShapeModel(ho_ImageReduced4, hv_hv_ModelID, HTuple(-10).TupleRad(), HTuple(20).TupleRad(),
					0.5, 1, 0, "least_squares", 3, 1, &hv_Row4, &hv_Column4, &hv_Angle4, &hv_Score4);
				TupleLength(hv_Row4, &Rlenth);
				if (Rlenth == 0 || hv_Score4 < 0.93)
				{
					DilationRectangle1(ho_RegionDilation1, &ho_RegionDilation1, 400, 400);
					ReduceDomain(ho_Src, ho_RegionDilation1, &ho_ImageReduced4);
					FindShapeModel(ho_ImageReduced4, hv_hv_ModelID, HTuple(-10).TupleRad(), HTuple(20).TupleRad(),
						0.5, 1, 0, "least_squares", 3, 1, &hv_Row4, &hv_Column4, &hv_Angle4, &hv_Score4);
				}
				VectorAngleToRigid(0, 0, 0, hv_Row4, hv_Column4, hv_Angle4, &hv_HomMat2D7);
				AffineTransContourXld(ho_ModelContours1, &ho_ContoursAffineTrans1, hv_HomMat2D7);

				hv_Trow[hv_Index - 1] = hv_Row4;
				hv_Tclo2[hv_Index - 1] = hv_Column4;
			}
		}
		HTuple MaxCol, MinCol;
		TupleMax(hv_Tclo2, &MaxCol);
		TupleMin(hv_Tclo2, &MinCol);
		HTuple Ddisx = MaxCol - MinCol;

		HTuple DScal = Ddisx / disx;

		if (Ddisx < 2500)
		{
			DScal = 1;
		}
		//整图的放射变化信息	
		HTuple	x, y, P, T, x1, y1, hv_HomMat2D23;
		HTuple x2, y2, P2, T2, x3, y3, hv_Tclo3, hv_Tclo4;
		hv_Tclo3 = hv_Tclo2 / DScal;
		hv_Tclo4 = hv_BMclo2 * DScal;
		VectorToRigid(hv_Trow, hv_Tclo3, hv_BMrow2, hv_BMclo2, &(*hv_HomMat2D2));
		//AffineTransImage(ho_Src, &ho_ImageAffineTrans, *hv_HomMat2D2, "constant", "false");
		VectorToRigid(hv_BMrow2, hv_Tclo4, hv_Trow, hv_Tclo2, &*hv_BTomMat2D);
		HomMat2dScaleLocal(*hv_HomMat2D2, 1 / DScal, 1, &(*hv_HomMat2D2));
		HomMat2dScaleLocal(*hv_BTomMat2D, DScal, 1, &(*hv_BTomMat2D));
		AffineTransImage(ho_Src, &ho_ImageAffineTrans, *hv_HomMat2D2, "constant", "false");

		//VectorToHomMat2d(hv_BMrow2, hv_BMclo2, hv_Trow, hv_Tclo2, &hv_BTomMat2D);
		//计算区域个数
		CountObj(ho_Partitioned, &hv_Number2);
		GetDictParam(hv_BDictHandle, "keys", HTuple(), &hv_BGenParamValue);
		TupleLength(hv_BGenParamValue, &hv_Number2);
		GenEmptyObj(&ho_ConnectedRegions);
		//connection (EmptyRegion, EmptyRegion)
		{
			HTuple end_val32 = hv_Number2;
			HTuple step_val32 = 1;
			for (hv_Index1 = 1; hv_Index1.Continue(end_val32, step_val32); hv_Index1 += step_val32)
			{
				TupleFind(hv_col1Indices, hv_Index1 - 1, &hv_bol2);
				//小图模板轮廓
				GetDictTuple(hv_BDictHandle, HTuple(hv_BGenParamValue[hv_Index1 - 1]), &hv_BModelID);
				SelectObj(ho_Partitioned, &ho_segment, hv_Index1);
				DilationRectangle1(ho_segment, &ho_segment, 400, 400);
				ReduceDomain(ho_Src, ho_segment, &ho_ImageReduced5);
				//小图轮廓查找区域
				GetDictObject(&ho_CreatRegion, hv_BMregion, HTuple(hv_BMGenParamValue[hv_Index1 - 1]));
				DilationRectangle1(ho_CreatRegion, &ho_CreatRegion, 50, 50);
				ShapeTrans(ho_CreatRegion, &ho_CreatRegion, "convex");
				//行
				TupleMod(hv_Index1 - 1, hv_NW, &hv_Mod);
				//列
				TupleDiv(hv_Index1 - 1, hv_NW, &hv_Quot);
				hv_tx = ((hv_heigth * hv_Quot) * 1.0) - 200;
				hv_ty = ((hv_width * hv_Mod) * 1.0) - 200;
				if (0 != (int(hv_Mod == 0)))
				{
					hv_ty = 0;
				}
				if (0 != (int(hv_Quot == 0)))
				{
					hv_tx = 0;
				}
				if (0 != (int(hv_BModelID == -1)))
				{
					HomMat2dIdentity(&hv_HomMat2D24);
					HomMat2dIdentity(&hv_BTomMat2D4);
					HomMat2dTranslate((hv_HomMat2D24), -hv_tx, -hv_ty, &hv_HomMat2DTranslate);
					(*hvec_HomDZM)[hv_Index1 - 1] = HTupleVector(hv_HomMat2DTranslate);
					HomMat2dTranslate(hv_BTomMat2D4, hv_tx, hv_ty, &hv_HomMat2DTranslate2);
					(*hvec_HomMZD)[hv_Index1 - 1] = HTupleVector(hv_HomMat2DTranslate2);
					hvec_offsetDZM[hv_Index1 - 1] = HTupleVector(hv_HomMat2D24);
					hvec_offsetMZD[hv_Index1 - 1] = HTupleVector(hv_BTomMat2D4);
				}
				else
				{
					HTuple Rxlenth;
					//affine_trans_region (Region, RegionAffineTrans1, BTomMat2D, 'nearest_neighbor')
					ShapeTrans(ho_CreatRegion, &ho_CreatRegion, "convex");
					DilationCircle(ho_CreatRegion, &ho_CreatRegion, 200);
					ReduceDomain(ho_ImageAffineTrans, ho_CreatRegion, &ho_ImageReduced);
					GetShapeModelContours(&ho_ModelContours1, hv_BModelID, 1);
					FindShapeModel(ho_ImageReduced, hv_BModelID, HTuple(1).TupleRad(), HTuple(5).TupleRad(), 0.6, 1, 0, "least_squares",
						4, 0, &hv_OSRow, &hv_OSColumn, &hv_Angle, &hv_Score);
					VectorAngleToRigid(0, 0, 0, hv_OSRow, hv_OSColumn, hv_Angle, &hv_HomMat2D7);
					AffineTransContourXld(ho_ModelContours1, &ho_ContoursAffineTrans1, hv_HomMat2D7);
					//待测图转模板图的仿射变换信息
					VectorAngleToRigid(hv_OSRow, hv_OSColumn, 0, HTuple(hv_row11[hv_Index1 - 1]),
						HTuple(hv_col1[hv_Index1 - 1]), 0, &hv_HomDZM1);
					HomMat2dTranslate(hv_HomDZM1, hv_tx, hv_ty, &hv_offsetDZM1);
					(*hvec_HomDZM)[hv_Index1 - 1] = HTupleVector(hv_HomDZM1);
					hvec_offsetDZM[hv_Index1 - 1] = HTupleVector(hv_offsetDZM1);
					//HomMat2dTranslate(hv_HomDZM1, -hv_tx, -hv_ty, &hv_offsetDZM1);
					//模板图转待测图仿射变换信息
					VectorAngleToRigid(HTuple(hv_row11[hv_Index1 - 1]), HTuple(hv_col1[hv_Index1 - 1]),
						0, hv_OSRow, hv_OSColumn, 0, &hv_HomMZD1);
					(*hvec_HomMZD)[hv_Index1 - 1] = HTupleVector(hv_HomMZD1);
					HomMat2dTranslate(hv_HomMZD1, -hv_tx, -hv_ty, &hv_offsetMZD1);
					hvec_offsetMZD[hv_Index1 - 1] = HTupleVector(hv_offsetMZD1);
				}
			}
		}
		HTuple end_val116 = ((hv_col1Indices.TupleLength()) / 2) - 1;
		HTuple step_val116 = 1, hv_Index2, hv_Selected2, hv_Selected, hv_c, hv_b, hv_HomMat2DCompose;
		for (hv_Index2 = 0; hv_Index2.Continue(end_val116, step_val116); hv_Index2 += step_val116)
		{
			TupleSelect(hv_col1Indices, hv_Index2 + ((hv_col1Indices.TupleLength()) / 2), &hv_Selected);
			TupleSelect(hv_col1Indices, hv_Index2, &hv_Selected2);
			//待测图转模板图临近mark
			hv_c = (*hvec_HomDZM)[hv_Selected2].T();
			hv_b = hvec_offsetDZM[hv_Selected].T();
			HomMat2dCompose(hv_c, hv_b, &hv_HomMat2DCompose);
			(*hvec_HomDZM)[hv_Selected2] = HTupleVector(hv_HomMat2DCompose);
			//模板图转待测图临近mark
			hv_c = (*hvec_HomMZD)[hv_Selected2].T();
			hv_b = hvec_offsetMZD[hv_Selected].T();
			HomMat2dCompose(hv_c, hv_b, &hv_HomMat2DCompose);
			(*hvec_HomMZD)[hv_Selected2] = HTupleVector(hv_HomMat2DCompose);
		}
	}

	void  inter_getRegionRect(HObject ho_defectRegions, HTuple* hv_Rows, HTuple* hv_Columns, HTuple* hv_Len1s, HTuple* hv_Len2s, HTuple* hv_Areas, HTuple* hv_row1s,
		HTuple* hv_col1s, HTuple* hv_row2s, HTuple* hv_col2s, HTuple* hv_row3s, HTuple* hv_col3s,
		HTuple* hv_row4s, HTuple* hv_col4s) {

		HTuple hv_Phi1, hv_Number, hv_Row1, hv_Column1, hv_Cos, hv_Sin;
		HObject ho_Rectangle;
		CountObj((ho_defectRegions), &hv_Number);
		(*hv_Areas) = HTuple();
		(*hv_Rows) = HTuple();
		(*hv_Columns) = HTuple();
		(*hv_col1s) = HTuple();
		(*hv_row1s) = HTuple();
		(*hv_col2s) = HTuple();
		(*hv_row2s) = HTuple();
		(*hv_col3s) = HTuple();
		(*hv_row3s) = HTuple();
		(*hv_col4s) = HTuple();
		(*hv_row4s) = HTuple();

		if (0 != (int(hv_Number == 0)))
		{
			return;
		}
		AreaCenter(ho_defectRegions, &(*hv_Areas), &(*hv_Rows), &(*hv_Columns));
		SmallestRectangle2(ho_defectRegions, &hv_Row1, &hv_Column1, &hv_Phi1, &(*hv_Len1s),
			&(*hv_Len2s));
		GenRectangle2(&ho_Rectangle, hv_Row1, hv_Column1, hv_Phi1, (*hv_Len1s), (*hv_Len2s));
		TupleCos(hv_Phi1, &hv_Cos);
		TupleSin(hv_Phi1, &hv_Sin);
		(*hv_col1s) = hv_Column1 - (((*hv_Len1s) * hv_Cos) + ((*hv_Len2s) * hv_Sin));
		(*hv_row1s) = hv_Row1 + (((*hv_Len1s) * hv_Sin) - ((*hv_Len2s) * hv_Cos));
		(*hv_col2s) = hv_Column1 + (((*hv_Len1s) * hv_Cos) - ((*hv_Len2s) * hv_Sin));
		(*hv_row2s) = hv_Row1 - (((*hv_Len1s) * hv_Sin) + ((*hv_Len2s) * hv_Cos));
		(*hv_col3s) = hv_Column1 + (((*hv_Len1s) * hv_Cos) + ((*hv_Len2s) * hv_Sin));
		(*hv_row3s) = hv_Row1 - (((*hv_Len1s) * hv_Sin) - ((*hv_Len2s) * hv_Cos));
		(*hv_col4s) = hv_Column1 + (((-(*hv_Len1s)) * hv_Cos) + ((*hv_Len2s) * hv_Sin));
		(*hv_row4s) = hv_Row1 - (((-(*hv_Len1s)) * hv_Sin) - ((*hv_Len2s) * hv_Cos));
	}
	int TRAIN(HObject ho_Image2, HObject ho_SubMarkRegion, HObject ho_Partitioned,
		HObjectVector/*{eObjectVector,Dim=1}*/ hvec_Onemark, HObjectVector/*{eObjectVector,Dim=1}*/* hvec_vImage,
		HTuple hv_DictHandle, HTuple hv_ImageFiles, HTuple hv_Number1, HTuple hv_BMrow2,
		HTuple hv_BMclo2, HTuple hv_BDictHandle, HTuple hv_col1Indices, HTuple hv_row11,
		HTuple hv_col1, HTuple hv_NW, HTuple hv_NH, HTuple hv_MimgFiles, HTuple hv_BMregion, HTuple Ncol, HTuple hv_Disx)
	{

		// Local iconic variables
		HObject  ho_Image, ho_ImageAffineTrans, ho_ObjectsConcat;
		HObject  ho_segment, ho_ImageReduced1, ho_ImageAffineTrans3, ho_ImageAffineTrans4;
		HObject  ho_DupImage, ho_ModelContours3, ho_ImageMax, ho_ImageMin, Paintregion;

		// Local control variables
		HTuple  hv_Width, hv_Height, hv_width, hv_heigth;
		HTuple  hv_Maximage, hv_Minimage, hv_WindowHandle, hv_GenParamValue;
		HTuple  hv_imgIndex, hv_HomMat2D2, hv_Number2, hv_BGenParamValue, hv_BTomMat2D;
		HTuple  hv_Index1, hv_bol, hv_BModelID, hv_HomMat2D5, hv_test;
		HTuple  hv_a, hv_Index2, hv_Ceil, hv_Floor, fArea, fC, fR;
		HTupleVector  hvec_HomDZM(1), hvec_HomMZD(1);
		HTuple hv_FileName, hv_ImageFiles_tow, hv_imgID;
		GetImageSize(ho_Image2, &hv_Width, &hv_Height);
		hv_width = hv_Width / hv_NW;
		hv_heigth = hv_Height / hv_NH;
		SetSystem("border_shape_models", "true");
		CreateDict(&hv_Maximage);
		CreateDict(&hv_Minimage);
		//粗定位mark进行粗定位
		GetDictParam(hv_DictHandle, "keys", HTuple(), &hv_GenParamValue);
		{

			ListFiles(hv_ImageFiles, (HTuple("files").Append("follow_links")), &hv_FileName);
			TupleRegexpSelect(hv_FileName, (HTuple("\\.(tif|tiff|gif|bmp|jpg|jpeg|jp2|png|pcx|pgm|ppm|pbm|xwd|ima)$").Append("ignore_case")),
				&hv_FileName);
			HTuple end_val11 = (hv_FileName.TupleLength()) - 1;
			if (end_val11 < 3)
			{
				return   3;

			}
			HTuple step_val11 = 1;
			for (hv_imgIndex = 0; hv_imgIndex.Continue(end_val11, step_val11); hv_imgIndex += step_val11)
			{
				//ReadImage(&ho_Image, HTuple(hv_ImageFiles[hv_imgIndex]));

				ReadImage(&ho_Image, HTuple(hv_FileName[hv_imgIndex]));
				if (Ncol == 1)
				{
					MirrorImage(ho_Image, &ho_Image, "row");
				}
				Position(ho_Image, ho_SubMarkRegion, ho_Partitioned, hv_DictHandle, hv_BMrow2,
					hv_BMclo2, hv_BDictHandle, hv_BMregion, hv_col1, hv_row11, hv_col1Indices,
					hv_NW, hv_NH, hv_Disx, &hvec_HomDZM, &hvec_HomMZD, &hv_HomMat2D2, &hv_BTomMat2D);
				//将待测图转向模板图位置转换之后对图像进行拼接确保不会漏检。
				//AffineTransImage(ho_Image, &ho_ImageAffineTrans, hv_HomMat2D2, "constant", "false");
				AffineTransImageSize(ho_Image, &ho_ImageAffineTrans, hv_HomMat2D2, "constant",
					hv_Width, hv_Height);
				CountObj(ho_Partitioned, &hv_Number2);
				GetDictParam(hv_BDictHandle, "keys", HTuple(), &hv_BGenParamValue);
				//小图进行平移计算
				{
					HTuple end_val22 = hv_Number2;
					HTuple step_val22 = 1;
					for (hv_Index1 = 1; hv_Index1.Continue(end_val22, step_val22); hv_Index1 += step_val22)
					{
						TupleFind(hv_col1Indices, hv_Index1 - 1, &hv_bol);
						GetDictTuple(hv_BDictHandle, HTuple(hv_BGenParamValue[hv_Index1 - 1]), &hv_BModelID);
						SelectObj(ho_Partitioned, &ho_segment, hv_Index1);
						DilationRectangle1(ho_segment, &ho_segment, 400, 400);
						//fill_up (CreatRegion, CreatRegion)

						//如果模板轮廓为=-1则说明这个分块没有找到mark点
						if (0 != (int(hv_BModelID == -1)))
						{
							hv_HomMat2D5 = hvec_HomDZM[hv_Index1 - 1].T();
							AffineTransImageSize(ho_ImageAffineTrans, &ho_ImageAffineTrans3, hv_HomMat2D5, "constant",
								hv_width, hv_heigth);

							/*ConcatObj(ho_Image2, ho_ImageAffineTrans, &ho_ObjectsConcat);
							TileImagesOffset(ho_ObjectsConcat, &ho_ImageAffineTrans4, (HTuple(0).Append(0)),
							(HTuple(0).Append(0)), (HTuple(0).Append(0)), (HTuple(0).Append(0)), (HTuple(-1).Append(-1)),
							(HTuple(-1).Append(-1)), hv_Width, hv_Height);
							ReduceDomain(ho_ImageAffineTrans4, ho_segment, &ho_ImageReduced1);
							CropDomain(ho_ImageReduced1, &ho_ImageAffineTrans3);*/
							if (0 != (int(hv_imgIndex == 0)))
							{
								CopyImage(ho_ImageAffineTrans3, &ho_DupImage);
								(*hvec_vImage)[hv_Index1] = HObjectVector(ho_DupImage);
							}
							else
							{
								{
									HObject ExpTmpOutVar_0;
									AppendChannel((*hvec_vImage)[hv_Index1].O(), ho_ImageAffineTrans3, &ExpTmpOutVar_0
									);
									(*hvec_vImage)[hv_Index1].O() = ExpTmpOutVar_0;
								}
							}

						}
						else
						{
							GetShapeModelContours(&ho_ModelContours3, hv_BModelID, 1);
							hv_HomMat2D5 = hvec_HomDZM[hv_Index1 - 1].T();
							AffineTransImageSize(ho_ImageAffineTrans, &ho_ImageAffineTrans3, hv_HomMat2D5, "constant",
								hv_width, hv_heigth);
							if (0 != (int(hv_imgIndex == 0)))
							{
								CopyImage(ho_ImageAffineTrans3, &ho_DupImage);
								(*hvec_vImage)[hv_Index1] = HObjectVector(ho_DupImage);
							}
							else
							{
								{
									HObject ExpTmpOutVar_0;
									AppendChannel((*hvec_vImage)[hv_Index1].O(), ho_ImageAffineTrans3, &ExpTmpOutVar_0
									);
									(*hvec_vImage)[hv_Index1].O() = ExpTmpOutVar_0;
								}
							}
						}
					}
				}
			}
		}
		hv_test = HTuple((*hvec_vImage).Length());
		hv_a = hv_test;
		{
			HTuple end_val54 = hv_test - 1;
			HTuple step_val54 = 1;
			for (hv_Index2 = 1; hv_Index2.Continue(end_val54, step_val54); hv_Index2 += step_val54)
			{
				TupleCeil((((hv_FileName.TupleLength()) + 1) / 3) * 2, &hv_Ceil);
				TupleFloor(((hv_FileName.TupleLength()) + 1) / 3, &hv_Floor);
				RankN((*hvec_vImage)[hv_Index2].O(), &ho_ImageMax, hv_Ceil);
				RankN((*hvec_vImage)[hv_Index2].O(), &ho_ImageMin, hv_Floor);
				SetDictObject(ho_ImageMax, hv_Maximage, hv_Index2);
				SetDictObject(ho_ImageMin, hv_Minimage, hv_Index2);
			}
		}
		WriteDict(hv_Maximage, hv_MimgFiles + "Maximg", HTuple(), HTuple());
		WriteDict(hv_Minimage, hv_MimgFiles + "Minimg", HTuple(), HTuple());
		return 0;
	}

	int realSegementVariation(const SegementVariationInput& input, SegementVariationOutput& output) {
		HObject Onemark, TempMark;
		try {
			HTuple  hv_MimgFiles, hv_NW, hv_NH, hv_ImageFiles, hv_BDictHandle;
			HTuple  hv_Number1, hv_BMrow2, MaxArea, MinArea;
			HTuple  hv_BMclo2, hv_DictHandle, hv_col1, hv_row11, hv_col1Indices, hv_BMregion1, hv_Dis;
			HObject  ho_Image2;
			HObject  ho_SubMarkRegion, ho_image, ho_Partitioned;
			HObjectVector  hvec_Onemark(1), hvec_vImage2(1);
			AlgParamSegementVariation* alg = (AlgParamSegementVariation*)&input.algPara;
			int Ncol = input.nCol;
			ho_Image2 = input.Himg;
			ho_SubMarkRegion = input.Hregion;
			hv_MimgFiles = input.saveSegmentImgPath.c_str();//数据存储文件夹
			hv_NW = alg->i_params.block.NW;//拆分列数
			hv_NH = alg->i_params.block.NH;//拆分行数
			hv_ImageFiles = input.SegmentImgPath.c_str();//训练图片文件路径
			MaxArea = alg->i_params.block.MaxArea;
			MinArea = alg->i_params.block.MinArea;
			CountObj(ho_SubMarkRegion, &hv_Number1);

		int c1=	CreatModelxld(ho_Image2, ho_SubMarkRegion, &ho_image, hv_MimgFiles, &hv_BMrow2,
				&hv_BMclo2, &hv_DictHandle, &hv_Dis);
		if (c1 == 5)
		{
			return 5;
		}
			//小图模板创建。
		int p1=	PreciseModelXld(ho_Image2, &ho_Partitioned, &hvec_Onemark, hv_NW, hv_NH, hv_MimgFiles,
				MinArea, MaxArea, &hv_col1, &hv_row11, &hv_col1Indices, &hv_BDictHandle,
				&hv_BMregion1);
		if (p1 == 5)
		{
			return 5;
		}
			int N = hvec_Onemark.Length();

			GenEmptyObj(&Onemark);
			for (int i = 0; i < N; i++)
			{
				TempMark = hvec_Onemark[i].O();
				Union2(TempMark, Onemark, &Onemark);
			}
			int a = TRAIN(ho_Image2, ho_SubMarkRegion, ho_Partitioned, hvec_Onemark, &hvec_vImage2,
				hv_DictHandle, hv_ImageFiles, hv_Number1, hv_BMrow2, hv_BMclo2, hv_BDictHandle,
				hv_col1Indices, hv_row11, hv_col1, hv_NW, hv_NH, hv_MimgFiles, hv_BMregion1, Ncol, hv_Dis);
			output.Maybemark = Onemark;
			return a;
		}
		catch (HalconCpp::HException& except) {
			string errormsg = except.ErrorMessage().Text();
			HTuple hv_Exception;
			except.ToHTuple(&hv_Exception);
			return 5;
		}
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int segementVariation(const SegementVariationInput& input, SegementVariationOutput& output) {
		try
		{
			return realSegementVariation(input, output);
		}
		catch (...)
		{
			return 3; //算法内部异常
		}

	}
}
