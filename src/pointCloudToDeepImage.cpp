#include "alglibrary/zkhyProjectHuaNan/pointCloudToDeepImage.h"
#include "alglibrary/zkhyProjectHuaNan/halconcodePublic.h"
#include "alglibrary/zkhyProjectHuaNan/utils.h"
#include "alglibrary/alglibMisc.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace cv;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {
	void get_object_models_center(HTuple hv_ObjectModel3DID, HTuple* hv_Center)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_Diameters, hv_Index, hv_Diameter, hv_C;
		HTuple  hv_Exception, hv_MD, hv_Weight, hv_SumW, hv_ObjectModel3DIDSelected;
		HTuple  hv_InvSum;

		//Compute the mean of all model centers (weighted by the diameter of the object models)
		hv_Diameters = HTuple(hv_ObjectModel3DID.TupleLength(), 0.0);
		{
			HTuple end_val2 = (hv_ObjectModel3DID.TupleLength()) - 1;
			HTuple step_val2 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val2, step_val2); hv_Index += step_val2)
			{
				try
				{
					GetObjectModel3dParams(HTuple(hv_ObjectModel3DID[hv_Index]), "diameter_axis_aligned_bounding_box",
						&hv_Diameter);
					GetObjectModel3dParams(HTuple(hv_ObjectModel3DID[hv_Index]), "center", &hv_C);
					hv_Diameters[hv_Index] = hv_Diameter;
				}
				// catch (Exception) 
				catch (HException& HDevExpDefaultException)
				{
					HDevExpDefaultException.ToHTuple(&hv_Exception);
					//Object model is empty, has no center etc. -> ignore it by leaving its diameter at zero
				}
			}
		}

		if (0 != ((hv_Diameters.TupleSum()) > 0))
		{
			//Normalize Diameter to use it as weights for a weighted mean of the individual centers
			hv_MD = (hv_Diameters.TupleSelectMask(hv_Diameters.TupleGreaterElem(0))).TupleMean();
			if (0 != (hv_MD > 1e-10))
			{
				hv_Weight = hv_Diameters / hv_MD;
			}
			else
			{
				hv_Weight = hv_Diameters;
			}
			hv_SumW = hv_Weight.TupleSum();
			if (0 != (hv_SumW < 1e-10))
			{
				hv_Weight = HTuple(hv_Weight.TupleLength(), 1.0);
				hv_SumW = hv_Weight.TupleSum();
			}
			(*hv_Center).Clear();
			(*hv_Center)[0] = 0;
			(*hv_Center)[1] = 0;
			(*hv_Center)[2] = 0;
			{
				HTuple end_val26 = (hv_ObjectModel3DID.TupleLength()) - 1;
				HTuple step_val26 = 1;
				for (hv_Index = 0; hv_Index.Continue(end_val26, step_val26); hv_Index += step_val26)
				{
					if (0 != (HTuple(hv_Diameters[hv_Index]) > 0))
					{
						hv_ObjectModel3DIDSelected = HTuple(hv_ObjectModel3DID[hv_Index]);
						GetObjectModel3dParams(hv_ObjectModel3DIDSelected, "center", &hv_C);
						(*hv_Center)[0] = HTuple((*hv_Center)[0]) + (HTuple(hv_C[0]) * HTuple(hv_Weight[hv_Index]));
						(*hv_Center)[1] = HTuple((*hv_Center)[1]) + (HTuple(hv_C[1]) * HTuple(hv_Weight[hv_Index]));
						(*hv_Center)[2] = HTuple((*hv_Center)[2]) + (HTuple(hv_C[2]) * HTuple(hv_Weight[hv_Index]));
					}
				}
			}
			hv_InvSum = 1.0 / hv_SumW;
			(*hv_Center)[0] = HTuple((*hv_Center)[0]) * hv_InvSum;
			(*hv_Center)[1] = HTuple((*hv_Center)[1]) * hv_InvSum;
			(*hv_Center)[2] = HTuple((*hv_Center)[2]) * hv_InvSum;
		}
		else
		{
			(*hv_Center) = HTuple();
		}
		return;
	}

	//-----------------------------pointCloudToDeepImage------------------------------------------//
	//函数说明：
	//PointCloudToDeepImageInput &input;				输入：点云数据、算法参数
	//PointCloudToDeepImageOutput &output:				输出：深度图、显示图
	int realPointCloudToDeepImage1(PointCloudToDeepImageInput& input, PointCloudToDeepImageOutput& output)
	{
		AlgParamPointCloudToDeepImage* algPara = (AlgParamPointCloudToDeepImage*)&input.algPara;
		DWORD timeThreshold = (DWORD)abs(algPara->d_params.block.overtimeTh);  //超时阈值设置
		if (timeThreshold > 90000)
			timeThreshold = 60000;
		int flag = 0;
		// Local iconic variables
		HObject  ho_BigImageConstReal, ho_BigImageConstByte;
		HObject  ho_Region, ho_RegionClosing, ho_RegionOpening, ho_RegionFillUp;
		HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_SortedRegions;
		HObject  ho_MarkRectangle, ho_RegionIntersection, ho_Image;
		HObject  ho_Rectangle, ho_ImageConst;

		// Local control variables
		HTuple  hv_WindowHandle, hv_ObjectModel3D, hv_Status;
		HTuple  hv_Instructions, hv_Message, hv_PoseOut, hv_point_coord_x1;
		HTuple  hv_point_coord_y1, hv_point_coord_z1, hv_Distance1;
		HTuple  hv_segBackground_z_min, hv_segBackground_z_max, hv_ObjectModel3DZ;
		HTuple  hv_segProduct_threshold, hv_ObjectModel3DConnected, hv_ParamValue;
		HTuple  hv_segProductPointsNum_min, hv_segProductPointsNum_max, hv_ObjectModelProduct;
		HTuple  hv_ParamValue1, hv_twoPointDist_x, hv_twoPointDist_y;
		HTuple  hv_packagingSize_W, hv_packagingSize_H, hv_packagingRows_num, hv_packagingCols_num, hv_tray_x_distance;
		HTuple  hv_tray_y_distance, hv_begin_x_offset, hv_begin_y_offset, hv_firstProduce_center_x, hv_firstProduce_center_y;
		HTuple  hv_produce_center_x, hv_produce_center_y, hv_row;
		HTuple  hv_col, hv_smooth3DModel_factor, hv_onePacking_width_height;
		HTuple  hv_bigImage_width, hv_bigImage_height, hv_produce_num;
		HTuple  hv_Index, hv_segFitPlane_z_min, hv_segFitPlane_z_max;
		HTuple  hv_point_coord_z2, hv_ObjectModelFit, hv_ObjectModel3DOut;
		HTuple  hv_Pose, hv_plane, hv_HomMat3D, hv_HomMat3DInvert;
		HTuple  hv_ObjectModel3DAffineTrans, hv_A, hv_B, hv_C, hv_D;
		HTuple  hv_Distance, hv_X_Coord, hv_Y_Coord, hv_Z_Coord;
		HTuple  hv_point_coord_x, hv_point_coord_y, hv_point_coord_z;
		HTuple  hv_MaxX, hv_MinX, hv_x_max_min, hv_MaxY, hv_MinY;
		HTuple  hv_y_max_min, hv_MaxZ, hv_MinZ, hv_x_offset, hv_IntX;
		HTuple  hv_y_offset, hv_IntY, hv_point_zCoord_byte, hv_Greatereq;
		HTuple  hv_Indices0, hv_Indices1, hv_IntZ, hv_Center, hv_min_distance;
		HTuple  hv_min_Distance_index, hv_i, hv_row_index, hv_col_index;
		HTuple  hv_Row1, hv_Col1, hv_Width1, hv_Height1, hv_ballHeight_standard;

		//******计算单位统一按照毫米mm*****************
		//输入参数，已知参数如下：
		//（基恩士读取出来的数据单位为m）

		hv_segBackground_z_min = algPara->d_params.block.segBackground_z_min * 0.001;//分割背景最小高度，单位mm转成m
		hv_segBackground_z_max = 0.0;																				 //hv_segBackground_z_max = algPara->d_params.block.segBackground_z_max * 0.001;//分割背景最大高度，单位mm转成m	
		hv_segProduct_threshold = algPara->d_params.block.segProduct_threshold * 0.001;//分割产品距离阈值，单位mm转成m	
		hv_segProductPointsNum_min = algPara->i_params.block.segProductPointsNum_min;//分割产品点云最小个数
		hv_segProductPointsNum_max = 1e30;
		//hv_segProductPointsNum_max = algPara->i_params.block.segProductPointsNum_max;//分割产品点云最大个数
		//线激光硬件固定参数：x方向两点间距：twoPointDist_x；y方向两点间距：twoPointDist_y，单位mm	
		hv_twoPointDist_x = algPara->d_params.block.twoPointDist_x * 1000;//线激光x方向两点间距，单位mm转成um
		hv_twoPointDist_y = algPara->d_params.block.twoPointDist_y * 1000;//线激光y方向两点间距，单位mm转成um
		//芯片尺寸：W*H，单位mm，正方形
		hv_packagingSize_W = algPara->d_params.block.packagingSize_W * 1000;//封装尺寸宽，单位mm转成um
		hv_packagingSize_H = algPara->d_params.block.packagingSize_H * 1000;//封装尺寸高，单位mm转成um
		//输入产品行列m*n，2*8	
		hv_packagingRows_num = algPara->i_params.block.packagingRows_num;//封装行数
		hv_packagingCols_num = algPara->i_params.block.packagingCols_num;//封装列数
		//tray盘间距（即吸嘴间距），单位mm	
		hv_tray_x_distance = algPara->d_params.block.tray_x_distance;//tray盘x方向间距（即吸嘴x间距），单位mm
		hv_tray_y_distance = algPara->d_params.block.tray_y_distance;//tray盘x方向间距（即吸嘴y间距），单位mm
		//3D相机开始扫描位置（0，0）点到料盘或吸嘴第一个位置[0,0]的距离，单位mm。扫描方向是从y=0开始	
		hv_begin_x_offset = algPara->d_params.block.begin_x_offset;
		hv_begin_y_offset = algPara->d_params.block.begin_y_offset;
		//平滑点云系数，用于将MLS曲面与每个点匹配的最近邻k的数量，建议值:40、60(默认值)、80、100、400	
		hv_smooth3DModel_factor = algPara->i_params.block.smooth3DModel_factor;	//平滑点云系数（最近邻k的数量）
		hv_segFitPlane_z_min = algPara->d_params.block.segFitPlane_z_min * 0.001;//单位mm转成m
		hv_segFitPlane_z_max = algPara->d_params.block.segFitPlane_z_max * 0.001;//单位mm转成m	
		hv_ballHeight_standard = algPara->d_params.block.ballHeight_standard * 1000;//单位mm转成um
		double imageOutputMat = algPara->i_params.block.imageOutputMat;//是否保存Mat图

		if (0 == input.pointCloud.size())
		{
			return 1;//输入点云数据为空
		}
		//std::cout << "原始点云个数：" << (hv_point_coord_z1.TupleLength())[0].I() << std::endl;
		//对输入点云数据进行转换成HTuple
		DWORD startTime = GetTickCount();
		int tupleLength = input.pointCloud.size();
		TupleGenConst(tupleLength, 0, &hv_point_coord_x1);
		TupleGenConst(tupleLength, 0, &hv_point_coord_y1);
		TupleGenConst(tupleLength, 0, &hv_point_coord_z1);
		for (int i = 0; i < tupleLength; i++)
		{
			hv_point_coord_x1[i] = input.pointCloud[i].x;
			hv_point_coord_y1[i] = input.pointCloud[i].y;
			hv_point_coord_z1[i] = input.pointCloud[i].z;
		}
		/*DWORD time = GetTickCount() - startTime;
		std::cout << "点云转换赋值耗时：" << time << std::endl;*/

		//#define debug
#ifdef debug
		timeThreshold = 999999;
		ReadObjectModel3d("C:/Users/yi.qiu/Desktop/8080.ply", "mm", "convert_to_triangles",
			"true", &hv_ObjectModel3D, &hv_Status);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_x", &hv_point_coord_x1);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_y", &hv_point_coord_y1);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_z", &hv_point_coord_z1);

		hv_segBackground_z_min = -0.7 * 0.001;
		hv_segBackground_z_max = -0 * 0.001;
		hv_segProduct_threshold = 1.0 * 0.001;
		hv_segProductPointsNum_min = 5000;
		hv_segProductPointsNum_max = 1e30;
		hv_twoPointDist_x = 0.0125 * 1000;
		hv_twoPointDist_y = 0.02 * 1000;
		hv_packagingSize_W = 4.0 * 1000;
		hv_packagingSize_H = 4.0 * 1000;
		hv_packagingRows_num = 2;
		hv_packagingCols_num = 8;
		hv_tray_x_distance = 10;
		hv_tray_y_distance = 10;
		hv_begin_x_offset = 5;
		hv_begin_y_offset = 5;
		hv_smooth3DModel_factor = 400;
		hv_segFitPlane_z_min = -0.3 * 0.001;
		hv_segFitPlane_z_max = -0.2 * 0.001;
		hv_ballHeight_standard = 0.1 * 1000;
		imageOutputMat = 1;
#endif // debug

		//step0:判断软件输入参数的正确性
		//软件输入参数判定
		if (hv_segProduct_threshold <= 0 || hv_segProductPointsNum_min <= 0 || hv_twoPointDist_x <= 0 || hv_twoPointDist_y <= 0 || hv_packagingSize_W <= 0 || hv_packagingSize_H <= 0 ||
			hv_packagingRows_num <= 0 || hv_packagingCols_num <= 0 || hv_tray_x_distance <= 0 || hv_tray_y_distance <= 0 || hv_begin_x_offset <= 0 || hv_begin_y_offset <= 0 ||
			hv_smooth3DModel_factor <= 0 || hv_ballHeight_standard <= 0)
		{
			return 6;//参数错误，必须大于0
		}
		if (hv_segBackground_z_min > hv_segBackground_z_max || hv_segFitPlane_z_min > hv_segFitPlane_z_max)
		{
			return 7;//参数错误，最大值必须大于最小值
		}

		//根据输入点云坐标生成点云3D模型
		GenObjectModel3dFromPoints(hv_point_coord_x1, hv_point_coord_y1, hv_point_coord_z1,
			&hv_ObjectModel3D);

		//step1：剔除产品背景点云干扰****************************************************
		//沿X,Y,Z筛选点云数据，在x,y,z方向滤除固定范围的噪声点云
		//将点云模型中的数据拆分成x,y,z坐标的三个集合	
		SelectPointsObjectModel3d(hv_ObjectModel3D, "point_coord_z", hv_segBackground_z_min,
			hv_segBackground_z_max, &hv_ObjectModel3DZ);
		try
		{
			HTuple hv_zPoints;
			GetObjectModel3dParams(hv_ObjectModel3DZ, "point_coord_z", &hv_zPoints);
		}
		catch (const std::exception&)
		{
			return 2;//剔除背景参数错误
		}

		//step2：分割获取每个产品的点云块****************************************************
		//分割连通域
		//'distance_3d'：两点之间的最大距离 //参数值 （目前的数据单位为m，0.001m=1mm，小于或等于参数值的为一个区域）。	
		ConnectionObjectModel3d(hv_ObjectModel3DZ, "distance_3d", hv_segProduct_threshold, &hv_ObjectModel3DConnected);
		//获取点云块个数
		GetObjectModel3dParams(hv_ObjectModel3DConnected, "num_points", &hv_ParamValue);
		//'num_points'：得到各区域点的数量。
		//按点云块的点数量，筛选点云，类似于二维的特征面积筛选，筛选各区域在范围内的点集
		SelectObjectModel3d(hv_ObjectModel3DConnected, "num_points", "and", hv_segProductPointsNum_min,
			hv_segProductPointsNum_max, &hv_ObjectModelProduct);
		GetObjectModel3dParams(hv_ObjectModelProduct, "num_points", &hv_ParamValue1);
		//std::cout << "分割封测产品个数：" << (hv_ParamValue1.TupleLength())[0].I() << std::endl;
		if (0 != (HTuple((hv_ParamValue1.TupleLength()) == 0).TupleOr((hv_ParamValue1.TupleLength()) >= (hv_packagingRows_num * hv_packagingCols_num))))
		{
			return 3;//分割产品个数错误
		}

		//step3：循环每个点云块 转深度图，把所有产品的深度图合并到一张大图中****************************************************
		//理论上第一个产品中心点坐标，3D相机开始扫描位置（0，0）点到料盘或吸嘴第一个位置[0,0]的距离，单位mm。扫描方向是从y=0开始	
		hv_firstProduce_center_x = hv_begin_x_offset + (hv_tray_x_distance / 2);
		hv_firstProduce_center_y = hv_begin_y_offset + (hv_tray_y_distance / 2);
		//计算出每个产品的中心在点云中的位置（理论标准）
		hv_produce_center_x = HTuple();
		hv_produce_center_y = HTuple();
		{
			HTuple end_val72 = hv_packagingRows_num - 1;
			HTuple step_val72 = 1;
			for (hv_row = 0; hv_row.Continue(end_val72, step_val72); hv_row += step_val72)
			{
				{
					HTuple end_val73 = hv_packagingCols_num - 1;
					HTuple step_val73 = 1;
					for (hv_col = 0; hv_col.Continue(end_val73, step_val73); hv_col += step_val73)
					{
						TupleConcat(hv_produce_center_x, hv_firstProduce_center_x + (hv_tray_x_distance * hv_row),
							&hv_produce_center_x);
						TupleConcat(hv_produce_center_y, hv_firstProduce_center_y + (hv_tray_y_distance * hv_col),
							&hv_produce_center_y);
					}
				}
			}
		}

		//确定需要大图的尺寸
		hv_onePacking_width_height = (((hv_packagingSize_W / hv_twoPointDist_x).TupleInt()).TupleMax2((hv_packagingSize_H / hv_twoPointDist_y).TupleInt())) + 20;//计算一个封装产品一行有多少个点云数据
		hv_bigImage_width = hv_packagingCols_num * hv_onePacking_width_height;//一个点表示一个像素
		hv_bigImage_height = hv_packagingRows_num * hv_onePacking_width_height;

		//创建一张类型为'real'的图像，用于计算球高、共面度
		GenImageConst(&ho_BigImageConstReal, "real", hv_bigImage_width, hv_bigImage_height);
		//创建一张类型为'byte'的图像，用于软件显示使用
		GenImageConst(&ho_BigImageConstByte, "byte", hv_bigImage_width, hv_bigImage_height);

		//根据产品个数，循环给大图赋值
		hv_produce_num = hv_ParamValue1.TupleLength();
		{
			HTuple end_val95 = hv_produce_num - 1;
			HTuple step_val95 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val95, step_val95); hv_Index += step_val95)
			{
				//step3.1：z向分割得到产品基座点云，用于拟合平面
				SelectPointsObjectModel3d(HTuple(hv_ObjectModelProduct[hv_Index]), "point_coord_z",
					hv_segFitPlane_z_min, hv_segFitPlane_z_max, &hv_ObjectModelFit);
				try
				{
					HTuple hv_zPoints;
					GetObjectModel3dParams(hv_ObjectModelFit, "point_coord_z", &hv_zPoints);
				}
				catch (const std::exception&)
				{
					return 4;//分割产品底座参数错误
				}
				//拟合平面
				//fit_primitives_object_model_3d( : : ObjectModel3D, ParamName, ParamValue : ObjectModel3DOut)
				//ObjectModel3D:输入模型
				//ParamName：拟合的参数 ：fitting_algorithm, max_radius, min_radius, output_point_coord, output_xyz_mapping, primitive_type
				//ParamValue:对应'primitive_type'------'cylinder'（圆柱体）, 'sphere'（球体）, 'plane'（平面）。对应'primitive_type'------'least_squares', 'least_squares_huber', 'least_squares_tukey'几种最小二乘法,这里选择plane和least_squares
				//ObjectModel3DOut：输出的平面
				FitPrimitivesObjectModel3d(hv_ObjectModelFit, (HTuple("primitive_type").Append("fitting_algorithm")),
					(HTuple("plane").Append("least_squares_tukey")), &hv_ObjectModel3DOut);

				//获取法向量,plane的前三个数值就是单位法向量
				GetObjectModel3dParams(hv_ObjectModel3DOut, "primitive_parameter", &hv_plane);
				//获取平面位姿
				GetObjectModel3dParams(hv_ObjectModel3DOut, "primitive_pose", &hv_Pose);
				//step3.2：将点云坐标系转为拟合平面的坐标系，调平3D模型;
				//无论是拟合平面、非规则物体调平、规则三维物体调平核心思想都是先求出当前三维空间物体的姿态，之后对姿态(Pose)翻转，使其与坐标轴重合。
				////世界坐标在摄像机坐标下的齐次变换矩阵//
				PoseToHomMat3d(hv_Pose, &hv_HomMat3D);
				////求世界坐标在摄像机坐标坐标系下矩阵的逆矩阵//
				HomMat3dInvert(hv_HomMat3D, &hv_HomMat3DInvert);
				//进行两个坐标系之间的3D坐标的仿射变换。
				AffineTransObjectModel3d(HTuple(hv_ObjectModelProduct[hv_Index]), hv_HomMat3DInvert,
					&hv_ObjectModel3DAffineTrans);

				//平滑点云，剔除噪点
				SmoothObjectModel3d(hv_ObjectModel3DAffineTrans, "mls", "mls_kNN", hv_smooth3DModel_factor,
					&hv_ObjectModel3DAffineTrans);

				flag = timeOutFusing(startTime, timeThreshold);
				if (flag == 1)
					return 8;

				//*******************************************************************************************************************************
				//step3.3：点云转深度图，实际上深度图是使用X，Y，Z三个数组的数据创建来的
				//X，Y是作为图像的行列坐标，Z是实数（表示的是深度/高度），而不是灰度，因为灰度值是0——255之间的整数
				//将点云模型中的数据拆分成x,y,z坐标的三个集合
				GetObjectModel3dParams(hv_ObjectModel3DAffineTrans, "point_coord_x", &hv_X_Coord);
				GetObjectModel3dParams(hv_ObjectModel3DAffineTrans, "point_coord_y", &hv_Y_Coord);
				GetObjectModel3dParams(hv_ObjectModel3DAffineTrans, "point_coord_z", &hv_Z_Coord);
				//std::cout << "单个封测产品点云个数：" << (hv_Z_Coord.TupleLength())[0].I() << std::endl;
				//单位m转成um，把点与点之间的间距进行压缩
				hv_point_coord_x = (hv_X_Coord * 1000000) / hv_twoPointDist_x;
				hv_point_coord_y = (hv_Y_Coord * 1000000) / hv_twoPointDist_y;
				hv_point_coord_z = hv_Z_Coord * 1000000;

				//获取x/y/z的范围，用于生成深度图的大小
				TupleMax(hv_point_coord_x, &hv_MaxX);
				TupleMin(hv_point_coord_x, &hv_MinX);
				hv_x_max_min = (hv_MaxX - hv_MinX) + 2;
				TupleInt(hv_x_max_min, &hv_x_max_min);
				TupleMax(hv_point_coord_y, &hv_MaxY);
				TupleMin(hv_point_coord_y, &hv_MinY);
				hv_y_max_min = (hv_MaxY - hv_MinY) + 2;
				TupleInt(hv_y_max_min, &hv_y_max_min);
				TupleMax(hv_point_coord_z, &hv_MaxZ);
				TupleMin(hv_point_coord_z, &hv_MinZ);

				//计算偏移，转到图像坐标系
				TupleInt(hv_MinX, &hv_x_offset);
				TupleInt(hv_point_coord_x, &hv_IntX);
				hv_IntX = hv_IntX - hv_x_offset;
				TupleInt(hv_MinY, &hv_y_offset);
				TupleInt(hv_point_coord_y, &hv_IntY);
				hv_IntY = hv_IntY - hv_y_offset;

				//找出小于100的高度值，并置为0			
				hv_point_zCoord_byte = hv_point_coord_z;
				TupleGreaterEqualElem(hv_point_zCoord_byte, hv_ballHeight_standard, &hv_Greatereq);
				TupleFind(hv_Greatereq, 0, &hv_Indices0);
				TupleFind(hv_Greatereq, 1, &hv_Indices1);
				hv_point_zCoord_byte[hv_Indices0] = 0;
				hv_point_zCoord_byte[hv_Indices1] = 255;
				//Z向坐标强制转成int，单位um
				TupleInt(hv_point_zCoord_byte, &hv_IntZ);


				//获取该产品原始点云中心，确定产品在几行几列
				get_object_models_center(HTuple(hv_ObjectModelProduct[hv_Index]), &hv_Center);
				//计算与该产品中心距离最近的点，获取到序号索引
				hv_min_distance = 999999;
				hv_min_Distance_index = 0;
				{
					HTuple end_val223 = (hv_packagingRows_num * hv_packagingCols_num) - 1;
					HTuple step_val223 = 1;
					for (hv_i = 0; hv_i.Continue(end_val223, step_val223); hv_i += step_val223)
					{
						DistancePp(HTuple(hv_produce_center_x[hv_i]), HTuple(hv_produce_center_y[hv_i]),
							HTuple(hv_Center[0]), HTuple(hv_Center[1]), &hv_Distance);
						if (0 != (hv_min_distance > hv_Distance))
						{
							hv_min_distance = hv_Distance;
							hv_min_Distance_index = hv_i;
						}
					}
				}
				//根据数组的索引号，图像对应的行、列位置
				hv_row_index = hv_min_Distance_index / hv_packagingCols_num;
				hv_col_index = hv_min_Distance_index % hv_packagingCols_num;
				//把高度值赋给图像的灰度值
				SetGrayval(ho_BigImageConstReal, hv_IntY + (hv_row_index * hv_onePacking_width_height),
					hv_IntX + (hv_col_index * hv_onePacking_width_height), hv_point_coord_z);
				SetGrayval(ho_BigImageConstByte, hv_IntY + (hv_row_index * hv_onePacking_width_height),
					hv_IntX + (hv_col_index * hv_onePacking_width_height), hv_IntZ);

				//test:把高度值赋给图像的灰度值
				//{
				//	HTuple end_val239 = hv_packagingRows_num - 1;
				//	HTuple step_val239 = 1;
				//	for (hv_Row1 = 0; hv_Row1.Continue(end_val239, step_val239); hv_Row1 += step_val239)
				//	{
				//		{
				//			HTuple end_val240 = hv_packagingCols_num - 1;
				//			HTuple step_val240 = 1;
				//			for (hv_Col1 = 0; hv_Col1.Continue(end_val240, step_val240); hv_Col1 += step_val240)
				//			{
				//				//使用X,Y,Z给图像赋值A
				//				SetGrayval(ho_BigImageConstReal, hv_IntY + (hv_Row1*hv_onePacking_width_height),
				//					hv_IntX + (hv_Col1*hv_onePacking_width_height), hv_point_coord_z);
				//				SetGrayval(ho_BigImageConstByte, hv_IntY + (hv_Row1*hv_onePacking_width_height),
				//					hv_IntX + (hv_Col1*hv_onePacking_width_height), hv_IntZ);
				//			}
				//		}
				//	}
				//}
				output.ho_deepImage = ho_BigImageConstReal;//ho深度图
				//output.ho_showImage = ho_BigImageConstByte;//ho显示图	

				output.mat_deepImage = HObject2Mat(ho_BigImageConstReal).clone();
				//选择输出格式，imageOutputMat=0 不输出Mat显示图，imageOutputMat=1 输出Mat显示图
				if (imageOutputMat == 1)
				{

					output.mat_showImage = HObject2Mat(ho_BigImageConstByte).clone();
				}
				//WriteImage(ho_BigImageConstReal, "tiff", 0, "C:/Users/yi.qiu/Desktop/realImage.tif");
				//WriteImage(ho_BigImageConstByte, "bmp", 0, "C:/Users/yi.qiu/Desktop/byteImage.bmp");
			}
		}
		return 0;
	}

	int realPointCloudToDeepImage(PointCloudToDeepImageInput& input, PointCloudToDeepImageOutput& output)
	{
		PrintMsg("1、开始调用 点云转深度图 函数");
		printf("11、开始调用点云转深度图\n");

		AlgParamPointCloudToDeepImage* algPara = (AlgParamPointCloudToDeepImage*)&input.algPara;
		DWORD timeThreshold = (DWORD)abs(algPara->d_params.block.overtimeTh);  //超时阈值设置
		if (timeThreshold > 90000)
			timeThreshold = 60000;
		int flag = 0;
		// Local iconic variables
		HObject  ho_BigImageConstReal, ho_BigImageConstByte;
		HObject  ho_Region, ho_RegionClosing, ho_RegionOpening, ho_RegionFillUp;
		HObject  ho_ConnectedRegions, ho_SelectedRegions, ho_SortedRegions;
		HObject  ho_MarkRectangle, ho_RegionIntersection, ho_Image;
		HObject  ho_Rectangle, ho_ImageConst;

		// Local control variables
		HTuple  hv_WindowHandle, hv_ObjectModel3D, hv_Status;
		HTuple  hv_Instructions, hv_Message, hv_PoseOut, hv_point_coord_x1;
		HTuple  hv_point_coord_y1, hv_point_coord_z1, hv_Distance1;
		HTuple  hv_segBackground_z_min, hv_segBackground_z_max, hv_ObjectModel3DZ;
		HTuple  hv_segProduct_threshold, hv_ObjectModel3DConnected, hv_ParamValue;
		HTuple  hv_segProductPointsNum_min, hv_segProductPointsNum_max, hv_ObjectModelProduct;
		HTuple  hv_ROIOffsetX, hv_ROIOffsetY, hv_segFitPlane_x_min, hv_segFitPlane_y_min, hv_segFitPlane_x_max, hv_segFitPlane_y_max, hv_ObjectModelFitX;
		HTuple  hv_segOnePacking_x_min, hv_segOnePacking_y_min, hv_segOnePacking_x_max, hv_segOnePacking_y_max, hv_ObjectOneModel, hv_twoPointDist_x, hv_twoPointDist_y;
		HTuple  hv_packagingSize_W, hv_packagingSize_H, hv_packagingRows_num, hv_packagingCols_num, hv_tray_x_distance;
		HTuple  hv_tray_y_distance, hv_begin_x_offset, hv_begin_y_offset, hv_firstProduce_center_x, hv_firstProduce_center_y;
		HTuple  hv_produce_center_x, hv_produce_center_y, hv_row;
		HTuple  hv_col, hv_smooth3DModel_factor, hv_onePacking_width_height;
		HTuple  hv_bigImage_width, hv_bigImage_height, hv_produce_num;
		HTuple  hv_Index, hv_segFitPlane_z_min, hv_segFitPlane_z_max;
		HTuple  hv_point_coord_z2, hv_ObjectModelFit, hv_ObjectModel3DOut;
		HTuple  hv_Pose, hv_plane, hv_HomMat3D, hv_HomMat3DInvert;
		HTuple  hv_ObjectModel3DAffineTrans, hv_A, hv_B, hv_C, hv_D;
		HTuple  hv_Distance, hv_X_Coord, hv_Y_Coord, hv_Z_Coord;
		HTuple  hv_point_coord_x, hv_point_coord_y, hv_point_coord_z;
		HTuple  hv_MaxX, hv_MinX, hv_x_max_min, hv_MaxY, hv_MinY;
		HTuple  hv_y_max_min, hv_MaxZ, hv_MinZ, hv_x_offset, hv_IntX;
		HTuple  hv_y_offset, hv_IntY, hv_point_zCoord_byte, hv_Greatereq;
		HTuple  hv_Indices0, hv_Indices1, hv_IntZ, hv_Center, hv_min_distance;
		HTuple  hv_min_Distance_index, hv_i, hv_row_index, hv_col_index;
		HTuple  hv_Row1, hv_Col1, hv_Width1, hv_Height1, hv_ballHeight_standard;

		//******计算单位统一按照毫米mm*****************
		//输入参数，已知参数如下：
		//（基恩士读取出来的数据单位为m）

		hv_segBackground_z_min = algPara->d_params.block.segBackground_z_min * 0.001;//分割背景最小高度，单位mm转成m
		hv_segBackground_z_max = 0.9;																				 //hv_segBackground_z_max = algPara->d_params.block.segBackground_z_max * 0.001;//分割背景最大高度，单位mm转成m	
		hv_segProduct_threshold = algPara->d_params.block.segProduct_threshold * 0.001;//分割产品距离阈值，单位mm转成m	
		hv_segProductPointsNum_min = algPara->i_params.block.segProductPointsNum_min;//分割产品点云最小个数
		hv_segProductPointsNum_max = 1e30;
		//hv_segProductPointsNum_max = algPara->i_params.block.segProductPointsNum_max;//分割产品点云最大个数
		//线激光硬件固定参数：x方向两点间距：twoPointDist_x；y方向两点间距：twoPointDist_y，单位mm	
		hv_twoPointDist_x = algPara->d_params.block.twoPointDist_x * 1000;//线激光x方向两点间距，单位mm转成um
		hv_twoPointDist_y = algPara->d_params.block.twoPointDist_y * 1000;//线激光y方向两点间距，单位mm转成um
		//芯片尺寸：W*H，单位mm，正方形
		hv_packagingSize_W = algPara->d_params.block.packagingSize_W * 1000;//封装尺寸宽，单位mm转成um
		hv_packagingSize_H = algPara->d_params.block.packagingSize_H * 1000;//封装尺寸高，单位mm转成um
		//输入产品行列m*n，2*8	
		hv_packagingRows_num = algPara->i_params.block.packagingRows_num;//封装行数
		hv_packagingCols_num = algPara->i_params.block.packagingCols_num;//封装列数
		//tray盘间距（即吸嘴间距），单位mm	
		hv_tray_x_distance = algPara->d_params.block.tray_x_distance;//tray盘x方向间距（即吸嘴x间距），单位mm
		hv_tray_y_distance = algPara->d_params.block.tray_y_distance;//tray盘x方向间距（即吸嘴y间距），单位mm
		//3D相机开始扫描位置（0，0）点到料盘或吸嘴第一个位置[0,0]的距离，单位mm。扫描方向是从y=0开始	
		hv_begin_x_offset = algPara->d_params.block.begin_x_offset;
		hv_begin_y_offset = algPara->d_params.block.begin_y_offset;
		//平滑点云系数，用于将MLS曲面与每个点匹配的最近邻k的数量，建议值:40、60(默认值)、80、100、400	
		hv_smooth3DModel_factor = algPara->i_params.block.smooth3DModel_factor;	//平滑点云系数（最近邻k的数量）
		hv_segFitPlane_z_min = algPara->d_params.block.segFitPlane_z_min * 0.001;//单位mm转成m
		hv_segFitPlane_z_max = algPara->d_params.block.segFitPlane_z_max * 0.001;//单位mm转成m	
		hv_ballHeight_standard = algPara->d_params.block.ballHeight_standard * 1000;//单位mm转成um
		double imageOutputMat = algPara->i_params.block.imageOutputMat;//是否保存Mat图

		if (0 == input.pointCloud.size())
		{
			return 1;//输入点云数据为空
		}
		//std::cout << "原始点云个数：" << (hv_point_coord_z1.TupleLength())[0].I() << std::endl;


		//对输入点云数据进行转换成HTuple
	//	DWORD startTime = GetTickCount();
	//	int tupleLength = input.pointCloud.size();
	//	TupleGenConst(tupleLength, 0, &hv_point_coord_x1);
	//	TupleGenConst(tupleLength, 0, &hv_point_coord_y1);
	//	TupleGenConst(tupleLength, 0, &hv_point_coord_z1);
	////#pragma omp parallel for  
	//	for (int i = 0; i < tupleLength; i++)
	//	{
	//		hv_point_coord_x1[i] = input.pointCloud[i].x;
	//		hv_point_coord_y1[i] = input.pointCloud[i].y;
	//		hv_point_coord_z1[i] = input.pointCloud[i].z;
	//	}
	//	DWORD time = (GetTickCount() - startTime)*1.0/1000.0;
	//	std::cout << "点云转换赋值耗时：" << time << std::endl;

		hv_point_coord_x1 = input.pointCloudX;
		hv_point_coord_y1 = input.pointCloudY;
		hv_point_coord_z1 = input.pointCloudZ;

		PrintMsg("2、点云数据进行转换成HTuple");
		printf("22、点云数据进行转换成HTuple\n");

		//#define debug
#ifdef debug
		timeThreshold = 999999;
		ReadObjectModel3d("C:/Users/yi.qiu/Desktop/8080.ply", "mm", "convert_to_triangles",
			"true", &hv_ObjectModel3D, &hv_Status);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_x", &hv_point_coord_x1);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_y", &hv_point_coord_y1);
		GetObjectModel3dParams(hv_ObjectModel3D, "point_coord_z", &hv_point_coord_z1);

		hv_segBackground_z_min = -0.7 * 0.001;
		hv_segBackground_z_max = -0 * 0.001;
		hv_segProduct_threshold = 1.0 * 0.001;
		hv_segProductPointsNum_min = 5000;
		hv_segProductPointsNum_max = 1e30;
		hv_twoPointDist_x = 0.0125 * 1000;
		hv_twoPointDist_y = 0.02 * 1000;
		hv_packagingSize_W = 4.0 * 1000;
		hv_packagingSize_H = 4.0 * 1000;
		hv_packagingRows_num = 2;
		hv_packagingCols_num = 8;
		hv_tray_x_distance = 10;
		hv_tray_y_distance = 10;
		hv_begin_x_offset = 5;
		hv_begin_y_offset = 5;
		hv_smooth3DModel_factor = 400;
		hv_segFitPlane_z_min = -0.3 * 0.001;
		hv_segFitPlane_z_max = -0.2 * 0.001;
		hv_ballHeight_standard = 0.1 * 1000;
		imageOutputMat = 1;
#endif // debug

		//step0:判断软件输入参数的正确性
		//软件输入参数判定
		if (hv_segProduct_threshold <= 0 || hv_segProductPointsNum_min <= 0 || hv_twoPointDist_x <= 0 || hv_twoPointDist_y <= 0 || hv_packagingSize_W <= 0 || hv_packagingSize_H <= 0 ||
			hv_packagingRows_num <= 0 || hv_packagingCols_num <= 0 || hv_tray_x_distance <= 0 || hv_tray_y_distance <= 0 || hv_begin_x_offset <= 0 || hv_begin_y_offset <= 0 ||
			hv_smooth3DModel_factor <= 0 || hv_ballHeight_standard <= 0)
		{
			return 6;//参数错误，必须大于0
		}
		if (hv_segBackground_z_min > hv_segBackground_z_max || hv_segFitPlane_z_min > hv_segFitPlane_z_max)
		{
			return 7;//参数错误，最大值必须大于最小值
		}

		//根据输入点云坐标生成点云3D模型
		GenObjectModel3dFromPoints(hv_point_coord_x1, hv_point_coord_y1, hv_point_coord_z1,
			&hv_ObjectModel3D);

		//ReadObjectModel3d("C:/Users/yi.qiu/Desktop/1229-4.ply", "mm", "convert_to_triangles",
		//	"true", &hv_ObjectModel3D, &hv_Status);


		//step1：剔除产品背景点云干扰****************************************************
		//沿X,Y,Z筛选点云数据，在x,y,z方向滤除固定范围的噪声点云
		//将点云模型中的数据拆分成x,y,z坐标的三个集合	
		SelectPointsObjectModel3d(hv_ObjectModel3D, "point_coord_z", hv_segBackground_z_min,
			hv_segBackground_z_max, &hv_ObjectModel3DZ);
		try
		{
			HTuple hv_zPoints;
			GetObjectModel3dParams(hv_ObjectModel3DZ, "point_coord_z", &hv_zPoints);
		}
		catch (const std::exception&)
		{
			return 2;//剔除背景参数错误
		}

		PrintMsg("3、剔除产品背景点云干扰");
		printf("33、剔除产品背景点云干扰\n");

		//step3：循环每个点云块 转深度图，把所有产品的深度图合并到一张大图中****************************************************
		//理论上第一个产品中心点坐标，3D相机开始扫描位置（0，0）点到料盘或吸嘴第一个位置[0,0]的距离，单位mm。扫描方向是从y=0开始	
		hv_firstProduce_center_x = hv_begin_x_offset + (hv_packagingSize_W / 2.0 / 1000);
		hv_firstProduce_center_y = hv_begin_y_offset + (hv_packagingSize_H / 2.0 / 1000);
		//计算出每个产品的中心在点云中的位置（理论标准）
		hv_produce_center_x = HTuple();
		{
			HTuple end_val88 = hv_packagingRows_num - 1;
			HTuple step_val88 = 1;
			for (hv_row = 0; hv_row.Continue(end_val88, step_val88); hv_row += step_val88)
			{
				{
					HTuple end_val89 = hv_packagingCols_num - 1;
					HTuple step_val89 = 1;
					for (hv_col = 0; hv_col.Continue(end_val89, step_val89); hv_col += step_val89)
					{
						TupleConcat(hv_produce_center_x, hv_firstProduce_center_x + (hv_tray_x_distance * hv_col),
							&hv_produce_center_x);
						TupleConcat(hv_produce_center_y, hv_firstProduce_center_y + (hv_tray_y_distance * hv_row),
							&hv_produce_center_y);
					}
				}
			}
		}

		//确定需要大图的尺寸
		hv_onePacking_width_height = (((hv_packagingSize_W / hv_twoPointDist_x).TupleInt()).TupleMax2((hv_packagingSize_H / hv_twoPointDist_y).TupleInt())) + 20;//计算一个封装产品一行有多少个点云数据
		hv_bigImage_width = hv_packagingCols_num * hv_onePacking_width_height;//一个点表示一个像素
		hv_bigImage_height = hv_packagingRows_num * hv_onePacking_width_height;

		//创建一张类型为'real'的图像，用于计算球高、共面度
		GenImageConst(&ho_BigImageConstReal, "real", hv_bigImage_width, hv_bigImage_height);
		//创建一张类型为'byte'的图像，用于软件显示使用
		GenImageConst(&ho_BigImageConstByte, "byte", hv_bigImage_width, hv_bigImage_height);

		//产品中心往左右、上下偏移，生成ROI裁剪点云用于拟合平面，单位mm
		hv_ROIOffsetX = 5;
		hv_ROIOffsetY = 0.5;
		//根据产品个数，循环给大图赋值
		hv_produce_num = hv_packagingRows_num * hv_packagingCols_num;
		{
			HTuple end_val95 = hv_produce_num - 1;
			HTuple step_val95 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val95, step_val95); hv_Index += step_val95)
			{
				//step3.1：x/y向分割得到产品基座点云，用于拟合平面
				hv_segFitPlane_x_min = (HTuple(hv_produce_center_x[hv_Index]) - hv_ROIOffsetX) * 0.001;
				hv_segFitPlane_x_max = (HTuple(hv_produce_center_x[hv_Index]) + hv_ROIOffsetX) * 0.001;
				hv_segFitPlane_y_min = (HTuple(hv_produce_center_y[hv_Index]) - hv_ROIOffsetY) * 0.001;
				hv_segFitPlane_y_max = (HTuple(hv_produce_center_y[hv_Index]) + hv_ROIOffsetY) * 0.001;
				SelectPointsObjectModel3d(hv_ObjectModel3DZ, "point_coord_x", hv_segFitPlane_x_min,
					hv_segFitPlane_x_max, &hv_ObjectModelFitX);
				SelectPointsObjectModel3d(hv_ObjectModelFitX, "point_coord_y", hv_segFitPlane_y_min,
					hv_segFitPlane_y_max, &hv_ObjectModelFit);
				try
				{
					HTuple hv_zPoints;
					GetObjectModel3dParams(hv_ObjectModelFit, "point_coord_z", &hv_zPoints);
				}
				catch (const std::exception&)
				{
					return 4;//分割产品底座参数错误
				}
				//拟合平面
				//fit_primitives_object_model_3d( : : ObjectModel3D, ParamName, ParamValue : ObjectModel3DOut)
				//ObjectModel3D:输入模型
				//ParamName：拟合的参数 ：fitting_algorithm, max_radius, min_radius, output_point_coord, output_xyz_mapping, primitive_type
				//ParamValue:对应'primitive_type'------'cylinder'（圆柱体）, 'sphere'（球体）, 'plane'（平面）。对应'primitive_type'------'least_squares', 'least_squares_huber', 'least_squares_tukey'几种最小二乘法,这里选择plane和least_squares
				//ObjectModel3DOut：输出的平面
				FitPrimitivesObjectModel3d(hv_ObjectModelFit, (HTuple("primitive_type").Append("fitting_algorithm")),
					(HTuple("plane").Append("least_squares_tukey")), &hv_ObjectModel3DOut);

				PrintMsg("4、拟合平面");
				printf("44、拟合平面\n");

				//step3.2分割一个产品的点云
				hv_segOnePacking_x_min = ((HTuple(hv_produce_center_x[hv_Index]) - (hv_packagingSize_W / 2000.0)) - 1.5) * 0.001;
				hv_segOnePacking_x_max = ((HTuple(hv_produce_center_x[hv_Index]) + (hv_packagingSize_W / 2000.0)) + 1.5) * 0.001;
				hv_segOnePacking_y_min = ((HTuple(hv_produce_center_y[hv_Index]) - (hv_packagingSize_H / 2000.0)) - 1.5) * 0.001;
				hv_segOnePacking_y_max = ((HTuple(hv_produce_center_y[hv_Index]) + (hv_packagingSize_H / 2000.0)) + 1.5) * 0.001;
				SelectPointsObjectModel3d(hv_ObjectModel3DZ, (HTuple("point_coord_x").Append("point_coord_y")),
					hv_segOnePacking_x_min.TupleConcat(hv_segOnePacking_y_min), hv_segOnePacking_x_max.TupleConcat(hv_segOnePacking_y_max),
					&hv_ObjectOneModel);
				try
				{
					HTuple hv_zPoints;
					GetObjectModel3dParams(hv_ObjectOneModel, "point_coord_z", &hv_zPoints);
				}
				catch (const std::exception&)
				{
					return 4;//分割产品底座参数错误
				}

				//获取法向量,plane的前三个数值就是单位法向量
				GetObjectModel3dParams(hv_ObjectModel3DOut, "primitive_parameter", &hv_plane);
				//获取平面位姿
				GetObjectModel3dParams(hv_ObjectModel3DOut, "primitive_pose", &hv_Pose);
				//step3.3：将点云坐标系转为拟合平面的坐标系，调平3D模型;
				//无论是拟合平面、非规则物体调平、规则三维物体调平核心思想都是先求出当前三维空间物体的姿态，之后对姿态(Pose)翻转，使其与坐标轴重合。
				////世界坐标在摄像机坐标下的齐次变换矩阵//
				PoseToHomMat3d(hv_Pose, &hv_HomMat3D);
				////求世界坐标在摄像机坐标坐标系下矩阵的逆矩阵//
				HomMat3dInvert(hv_HomMat3D, &hv_HomMat3DInvert);
				//进行两个坐标系之间的3D坐标的仿射变换。
				AffineTransObjectModel3d(hv_ObjectOneModel, hv_HomMat3DInvert,
					&hv_ObjectModel3DAffineTrans);

				//标准球高200，200+30=230,分割剔除部分噪点。仿射变换后，原始点云的Z轴朝向不确定（朝上或朝下）
				SelectPointsObjectModel3d(hv_ObjectModel3DAffineTrans, "point_coord_z", -0.3 * 0.001,
					0.3 * 0.001, &hv_ObjectModel3DAffineTrans);

				PrintMsg("5、分割出产品点云");
				printf("55、分割出产品点云\n");

				//timeThreshold = 999999999;
				//flag = timeOutFusing(startTime, timeThreshold);
				//if (flag == 1)
				//	return 8;

				//*******************************************************************************************************************************
				//step3.3：点云转深度图，实际上深度图是使用X，Y，Z三个数组的数据创建来的
				//X，Y是作为图像的行列坐标，Z是实数（表示的是深度/高度），而不是灰度，因为灰度值是0——255之间的整数
				//将点云模型中的数据拆分成x,y,z坐标的三个集合，该单位为m，后续需要转换成um
				GetObjectModel3dParams(hv_ObjectModel3DAffineTrans, "point_coord_x", &hv_X_Coord);
				GetObjectModel3dParams(hv_ObjectModel3DAffineTrans, "point_coord_y", &hv_Y_Coord);
				GetObjectModel3dParams(hv_ObjectModel3DAffineTrans, "point_coord_z", &hv_Z_Coord);
				//std::cout << "单个封测产品点云个数：" << (hv_Z_Coord.TupleLength())[0].I() << std::endl;
				//单位m转成um，把点与点之间的间距进行压缩
				hv_point_coord_x = (hv_X_Coord * 1000000) / hv_twoPointDist_x;
				hv_point_coord_y = (hv_Y_Coord * 1000000) / hv_twoPointDist_y;
				//给所有z坐标取绝对值（拟合平面的姿态不确定：朝上或朝下）
				hv_point_coord_z = (hv_Z_Coord * 1000000).TupleAbs();

				//获取x/y/z的范围，用于生成深度图的大小
				TupleMax(hv_point_coord_x, &hv_MaxX);
				TupleMin(hv_point_coord_x, &hv_MinX);
				hv_x_max_min = (hv_MaxX - hv_MinX) + 2;
				TupleInt(hv_x_max_min, &hv_x_max_min);
				TupleMax(hv_point_coord_y, &hv_MaxY);
				TupleMin(hv_point_coord_y, &hv_MinY);
				hv_y_max_min = (hv_MaxY - hv_MinY) + 2;
				TupleInt(hv_y_max_min, &hv_y_max_min);
				TupleMax(hv_point_coord_z, &hv_MaxZ);
				TupleMin(hv_point_coord_z, &hv_MinZ);

				//计算偏移，转到图像坐标系
				TupleInt(hv_MinX, &hv_x_offset);
				TupleInt(hv_point_coord_x, &hv_IntX);
				hv_IntX = hv_IntX - hv_x_offset;
				TupleInt(hv_MinY, &hv_y_offset);
				TupleInt(hv_point_coord_y, &hv_IntY);
				hv_IntY = hv_IntY - hv_y_offset;

				//找出小于100的高度值，并置为0			
				hv_point_zCoord_byte = hv_point_coord_z;
				TupleGreaterEqualElem(hv_point_zCoord_byte, hv_ballHeight_standard, &hv_Greatereq);
				TupleFind(hv_Greatereq, 0, &hv_Indices0);
				TupleFind(hv_Greatereq, 1, &hv_Indices1);
				hv_point_zCoord_byte[hv_Indices0] = 0;
				hv_point_zCoord_byte[hv_Indices1] = 255;
				//Z向坐标强制转成int，单位um
				TupleInt(hv_point_zCoord_byte, &hv_IntZ);


				//获取该产品原始点云中心，确定产品在几行几列
				get_object_models_center(hv_ObjectOneModel, &hv_Center);
				//计算与该产品中心距离最近的点，获取到序号索引
				hv_min_distance = 999999;
				hv_min_Distance_index = 0;
				{
					HTuple end_val259 = (hv_packagingRows_num * hv_packagingCols_num) - 1;
					HTuple step_val259 = 1;
					for (hv_i = 0; hv_i.Continue(end_val259, step_val259); hv_i += step_val259)
					{
						DistancePp(HTuple(hv_produce_center_x[hv_i]), HTuple(hv_produce_center_y[hv_i]),
							HTuple(hv_Center[0]) * 1000, HTuple(hv_Center[1]) * 1000, &hv_Distance);
						if (0 != (hv_min_distance > hv_Distance))
						{
							hv_min_distance = hv_Distance;
							hv_min_Distance_index = hv_i;
						}
					}
				}
				//根据数组的索引号，图像对应的行、列位置
				hv_row_index = hv_min_Distance_index / hv_packagingCols_num;
				hv_col_index = hv_min_Distance_index % hv_packagingCols_num;
				//把高度值赋给图像的灰度值
				SetGrayval(ho_BigImageConstReal, hv_IntY + (hv_row_index * hv_onePacking_width_height),
					hv_IntX + (hv_col_index * hv_onePacking_width_height), hv_point_coord_z);
				SetGrayval(ho_BigImageConstByte, hv_IntY + (hv_row_index * hv_onePacking_width_height),
					hv_IntX + (hv_col_index * hv_onePacking_width_height), hv_IntZ);

				PrintMsg("6、高度值赋给图像的灰度值");
				printf("66、高度值赋给图像的灰度值\n");
			}
		}
		PrintMsg("7、输出深度图");
		printf("77、输出深度图\n");
		output.ho_deepImage = ho_BigImageConstReal;//ho深度图
		//output.ho_showImage = ho_BigImageConstByte;//ho显示图	
		output.mat_deepImage = HObject2Mat(ho_BigImageConstReal).clone();
		//选择输出格式，imageOutputMat=0 不输出Mat显示图，imageOutputMat=1 输出Mat显示图
		if (imageOutputMat == 1)
		{

			output.mat_showImage = HObject2Mat(ho_BigImageConstByte).clone();
		}
		//WriteImage(ho_BigImageConstReal, "tiff", 0, "C:/Users/yi.qiu/Desktop/realImage.tif");
		//WriteImage(ho_BigImageConstByte, "bmp", 0, "C:/Users/yi.qiu/Desktop/byteImage.bmp");
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int pointCloudToDeepImage(PointCloudToDeepImageInput& input, PointCloudToDeepImageOutput& output)
	{
		try
		{
			PrintMsg("0、函数入口");
			printf("00、函数入口\n");
			return realPointCloudToDeepImage(input, output);
		}
		catch (...)
		{
			return 5; //未知错误
		}
	}
}
