/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_TYPES_H
#define ZKHYPROJECTHUANAN_TYPES_H

#include "alglibrary/alglibCore.h"

#ifndef FlawInfoTab_Size_szVarName
#define FlawInfoTab_Size_szVarName 0x80
#endif 



namespace alglib::ops::zkhyProHN {
	struct SaveCalibFilesPath
	{
		std::string saveLensDistortCalibMapPath = "";   //镜头畸变校正Map图，后缀tiff
		std::string savePerspectTransMapPath = "";   //透视变换Map图，后缀tiff
	};

	struct CalibFilesInfo
	{
		HalconCpp::HObject  lensDistortCalibMap;          //镜头畸变校正Map图
		HalconCpp::HObject  perspectTransMap;             //透视变换Map图
	};

	struct SaveProjTransMatPath
	{
		std::string saveProjTransMatPath = "";    //投影变换矩阵存储路径 后缀为mat 若为空，则不存储
	};

	struct ProjTransMatInfo {

		HalconCpp::HTuple projTransMat;              //投影变换矩阵
	};
#if 0
	struct MeasureInfoBasic {
		double width;				//宽度
		double height;				//高度
		double angle;				//角度
		double area;				//面积

		UINT type;					//类型

		cv::Point2f center;				//中心点
		std::vector<cv::Point2f> boundingBox;	//外接矩形轮廓
		std::vector<std::vector<cv::Point2f>> contours;	//轮廓点集

		MeasureInfoBasic() : width(0), height(0), angle(0), area(0), type(0), center(cv::Point2f(0, 0)) { boundingBox.clear(); contours.clear(); }
	};
	struct MeasureInfoExtend {
		double distanceMean;		//平均距离
		double distanceMin;			//最近距离
		double distanceMax;			//最远距离
		double distanceX;			//X方向距离
		double distanceY;			//Y方向距离
		double distanceZ;			//Z方向距离
		double distanceR;			//直接距离
		double distanceMaxMin;		//最大最小距离差值

		MeasureInfoExtend() : distanceMean(0), distanceMin(0), distanceMax(0), distanceX(0), distanceY(0), distanceZ(0), distanceR(0), distanceMaxMin(0) {}
	};

	struct MeasureInfoStruct {

		MeasureInfoBasic basicInfo;		//基础信息
		MeasureInfoExtend extendInfo;	//扩展信息	

	};

	struct wireMeasureInfo
	{
		MeasureInfoBasic basicInfo;
		double wireWidth;		// 线宽
		double wireSpacing;		// 线间距

		// 初始化
		wireMeasureInfo() :wireWidth(0), wireSpacing(0) {}
	};
#endif

	struct wireMeasureInfo
	{
		alglib::core::MeasureInfoBasic basicInfo;
		double wireWidth;		// 线宽
		double wireSpacing;		// 线间距

		// 初始化
		wireMeasureInfo() :wireWidth(0), wireSpacing(0) {}
	};

	enum edgePolarity :int	// 边缘点极性
	{
		allPolarity = 0,		// 所有边缘点
		positive = 1,	// 黑到白
		negative = 2,	// 白到黑
		uniform = 3		// 所有边缘点，但拟合时会分别拟合不同极性的边缘点
	};

	enum edgeSelect :int		// 边缘点选择
	{
		allPoints = 0,		// 所有边缘点
		first = 1,		// 第一个边缘点
		last = 2		// 最后一个边缘点
	};

	typedef struct WaferInfoAlg
	{
		int nChannel;
		int nRow;
		int nCol;
		char szHdictPath[MAX_PATH];

	}WaferInfoAlg;
	typedef struct Logic_Flaw_ResultAlg
	{
		char szCheck_Flaw_Group[FlawInfoTab_Size_szVarName];
		char szDisplayName[FlawInfoTab_Size_szVarName];
		char szTag_Flaw[FlawInfoTab_Size_szVarName];
		int nResult_Index;
	}Logic_Flaw_ResultAlg;
	typedef struct Param_Wafer_DefectSummaryAlg
	{
		WaferInfoAlg waferInfo;
		std::vector <std::vector <alglib::core::FlawInfo>*> flaws;
		std::vector <std::vector <Logic_Flaw_ResultAlg>*> flawsFilter;
		std::vector <HalconCpp::HObject*> SdefectRegion;
	}Param_Wafer_DefectSummaryAlg;

	//定义常量
	constexpr double myPi = 3.141592653589793238462643383279502884L;
	const double Inf = 1e-8;

	//layout模块入口
	struct PKGInfo
	{
		cv::Point2f pos;    //中心坐标点（图像坐标）
		int existFlag;  //PKG是否存在标志位
	};

	struct BGAParam
	{
		double bodySizeX = 15;    //封测尺寸x 长度
		double bodySizeY = 10;    //封测尺寸y 宽度
		double bodySizeZ = 1;    //封测尺寸z 厚度，单位mm
		int gridNumX = 12;        //行数
		int gridNumY = 22;        //列数
		double gridOffsetX = 0;  //网格中心偏移x距离
		double gridOffsetY = 0;  //网格中心偏移y距离
		double gridPitchX = 0.49;   //网格x方向间距
		double gridPitchY = 0.63;   //网格y方向间距
		double ballWidth = 0.2;    //球宽
		double ballHeight = 0.22;   //球高，单位mm
		double leftUpCornerX = 0.64;   //左上角点x距离
		double leftUpCornerY = 0.57;   //左上角点y距离
		double mmOfPixelX = 0.014;    //x方向像素当量 mm/pixel
		double mmOfPixelY = 0.014;    //y方向像素当量 mm/pixel
		HalconCpp::HTuple ballPosX;      //球中心图像坐标
		HalconCpp::HTuple ballPosY;      //球中心图像坐标
		std::vector<std::vector<PKGInfo>> posInfo;//中心点坐标+标志位，图像坐标
	};

	struct QFNParam
	{
		double bodySizeX = 8;    //封测尺寸x 长度
		double bodySizeY = 8;    //封测尺寸y 宽度
		double bodySizeZ = 1;    //封测尺寸z 厚度，单位mm
		int gridNumX = 14;        //行数
		int gridNumY = 14;        //列数
		double gridOffsetX = 0;  //网格中心偏移x距离
		double gridOffsetY = 0;  //网格中心偏移y距离
		double gridPitchX = 0.18;   //网格x方向间距
		double gridPitchY = 0.18;   //网格y方向间距
		double padWidth = 0.2;     //pad宽度
		double padLenght = 0.45;    //pad长度
		double padHeight = 0.02;    //pad厚度，单位mm
		double distToTipX = 0;   //到顶部x距离
		double distToTipY = 0;   //到顶部y距离
		double centerPadWidth = 5.18; //中间pad的宽度
		double centerPadLenght = 5.18;//中间pad的长度		
		double mmOfPixelX = 0.014;    //x方向像素当量 mm/pixel
		double mmOfPixelY = 0.014;    //y方向像素当量 mm/pixel
		double leftUpCornerX = 0.64;   //左上角点x距离
		double leftUpCornerY = 0.57;   //左上角点y距离
		double centerPadToLeft = 4;//中间pad中心到左边距离
		double centerPadToUp = 4;//中间pad中心到上边距离
		cv::Point2f centerPadPoint; //中间pad的坐标
		std::vector<std::vector<PKGInfo>> posInfo;//中心点坐标+标志位（图像坐标）
	};

	//记录QFN产品待测图中Pad的信息
	class QFNDetInfo
	{
	public:
		cv::Point2f pointLT;
		cv::Point2f pointRD;
		cv::Point2f circleCenter;
		cv::Point2f templateCenter;     //Layout中Pad的中心点
		HalconCpp::HObject padRegion;
		int TDPadIndx;
		int isExist;
		double area;
		double radius;
		double pitch;
		double offset;
		double contrast;
		double JLength;
		double offSetLT;
		double TD;
		double span;
		bool isMissing;     //Pad是否缺失
		bool useCircle;     //Pad顶部是否使用圆
		std::string padType;  //Pad类型   normal, corner, center
		std::string padLocated;  //Pad位于IC的哪个边  top down left right

		QFNDetInfo() {
			cv::Point2f pointTmp;
			pointTmp.x = 0;
			pointTmp.y = 0;
			pointLT = pointTmp;
			pointRD = pointTmp;
			circleCenter = pointTmp;
			templateCenter = pointTmp;     //Layout中Pad的中心点
			HalconCpp::GenEmptyObj(&padRegion);
			TDPadIndx = -9999;
			isExist = 0;
			area = -9999;
			radius = -9999;
			pitch = -9999;
			offset = -9999;
			contrast = -9999;
			JLength = -9999;
			offSetLT = -9999;
			TD = -9999;
			span = -9999;
			isMissing = false;     //Pad是否缺失
			useCircle = false;     //Pad顶部是否使用圆
			padType = "normal";  //Pad类型   normal, corner, center
			padLocated = "top";  //Pad位于IC的哪个边  top down left right
		}
	};

	struct SaveShapeModelPath1
	{
		std::string saveModelIDpath = "";                  //模型ID存储路径 后缀为shm 若为空，则不存储
		std::string saveModelID2path = "";                  //模型2ID存储路径 后缀为shm 若为空，则不存储
		std::string saveBModelImgPath = "";              //亮模板图存储路径 后缀为bmp 若为空，则不存储
		std::string saveDModelImgPath = "";             //暗模板图存储路径 后缀为bmp 若为空，则不存储
		std::string save_modelRow = "";	//存储模版图的row。
		std::string save_modelCol = "";//存储模版图的clo。
		std::string FindRegionROI = "";//查找模板roi
		std::string rectangle = "";//模板图矩形框图
		std::string  cregions = "";//检测区域
	};
	struct TransRlt1 {
		double row;
		double column;
		double angle;
		double startRow;
	};
	struct ShapeModelInfo1 {
		HalconCpp::HTuple    modelID;                         //模型1ID
		HalconCpp::HTuple    hv_modelRow3;				 //仿射变换row
		HalconCpp::HObject   BmodelImg;                     //亮模板图
		HalconCpp::HObject   DmodelImg;				    //暗模板图
		TransRlt1 modelTrans;                   //模板匹配的信息,其中的模板轮廓信息可以在 创建形状匹配模型算子时 在界面上进行显示
		HalconCpp::HObject   findRegionROI;               //查找模板ROI
		HalconCpp::HObject   rectangle;				  //模板图矩形ROI
		HalconCpp::HTuple    hv_modelColumn3;            //仿射变换Clo
		HalconCpp::HObject   cregions1;				//检测ROI
	};

	/********************************
	 * 功能 液晶面板布局
	 ********************************/
	struct PanelLayouts
	{
		int num = 0;		// id序号
		std::string id;			// panelId A1、B1、C1
		std::string panelModel;		// panel类型
		std::vector<cv::Point2f>  pPts;	// Panel区域的四个点位坐标 左上-左下-右下-右上
		float contract = 0;		// 内缩像素值
	};

	/********************************
	 * 功能 基板布局
	 ********************************/
	struct  GlassLayout {
		std::string galssName;                                   //基板名称
		std::vector<cv::Point2f>        gPts;                        //Glass区域的四个点位坐标
		std::vector<PanelLayouts>   layoutList;                  //panel列表
	};
	struct  PanelRegion {
		std::string ID;//区域对应ID；
		std::vector<cv::Point2f>                rPts;                    //Panel区域的四个点位坐标
		HalconCpp::HObject region;//检测区域
		//cv::Point2f topleft;//左上角点
	};
	struct SegmentShap
	{
		HalconCpp::HObject ho_markRegion;//粗定位mark
		HalconCpp::HObject ho_Partitioned;//分区区域
		HalconCpp::HTuple hv_DictHandle;//粗定位版轮廓
		HalconCpp::HTuple hv_BMrow2;//粗定位row
		HalconCpp::HTuple hv_BMclo2;//粗定位Col
		HalconCpp::HTuple hv_BDictHandle;//分割小图模板罗阔
		HalconCpp::HTuple hv_BMregion;//分割小图模板轮廓区域
		HalconCpp::HTuple hv_col1;//分割小图col
		HalconCpp::HTuple hv_row11;//分割小图row
		HalconCpp::HTuple hv_col1Indices;//没有找到mark的小图索引
		std::vector<HalconCpp::HObject> hv_Minimage;//腐蚀后的模板图像
		std::vector<HalconCpp::HObject> hv_Maximage;//膨胀后的模板图像
		std::vector<HalconCpp::HObject> hvec_Dregion;//检测区域
		std::vector<HalconCpp::HObject> grid;//网格区域
		int mask;//图像腐蚀膨胀像素个数
		HalconCpp::HTuple Disx=HalconCpp::HTuple(1.0);//粗定位模板图mark种X方向距离

	};
}

#endif // ZKHYPROJECTHUANAN_TYPES_H