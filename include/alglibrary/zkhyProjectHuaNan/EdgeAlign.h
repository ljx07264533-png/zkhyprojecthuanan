
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_EDGEALIGN_H
#define ZKHYPROJECTHUANAN_EDGEALIGN_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------EdgeAlign 边缘对齐算子------------------------------------------//
	struct EdgeAlignInput
	{
		cv::Mat matImg;                                //mat图
		//HalconCpp::HObject detICRegion;                      //分割出的产品区域，isComputeFlaw = 0时可以为空
		std::vector<alglib::core::RectangleROI> rectangelROIs;        //扫描矩形ROI
		alglib::core::CSYSTransPara  templateTransPara;        //模板图中特征坐标系参数
		//alglib::core::CSYSTransPara  layoutTransPara;          //Layout特征坐标系参数
		alglib::core::AlgrithmParams algPara;					   //算法参数
	};
	struct EdgeAlignOutput
	{
		alglib::core::CSYSTransPara template2DetectTransPara;  //模板图到待测图的坐标变换关系
		alglib::core::CSYSTransPara layout2DetectTransPara;    //Layout到待测图的坐标变换关系
		//alglib::core::CSYSTransPara layout2TemplateTransPara;  //Layout到模板图的坐标变换关系
		HalconCpp::HObject ICRegion;                       //根据计算的点生成的芯片区域
		alglib::core::CSYSTransPara templateTransPara;         //创建模板图时需要记录的模板图位置信息
		std::vector<cv::Point2f>  vertexs;            //产品四个顶点和中心点信息，顺序为 左上-左下-右下-右上-中心
		std::vector<std::vector<cv::Point2f>> ICContour;     //芯片轮廓
		std::vector<alglib::core::FlawInfoStruct>  flawsData;               //缺陷信息
		std::vector<alglib::core::FlawInfoStruct>  flawStatic;			   //缺陷信息统计
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct IntParamsEdgeAlign//int型算法参数，独立出来方便使用默认构造函数
	{
		int isDepthImage = 0;          //输入是否为深度图
		//int iPackagingType = 0;         //封装类型
		int isComputeFlaw = 0;          //是否计算Body缺陷信息,0-不计算，1-计算
		int isPreprocessing = 0;		//是否进行二值化，0-无，1-二值化
		int iThreshold = 0;				//二值化阈值，默认值50
		int iEdgePolarity = 0;			//图像边缘属性，0-ALL，1-黑到白，2-白到黑
		int iMinValue = 12;				//灰度最小梯度(边缘检测阈值)
		int iPointCompoundSentence = 1;	//边缘点复判，0-无，1-复判
		int iOffset = 10;				//复判宽度
		int iFitLinePara = 1;			//fitline模式
		int iCoarseStep = 5;			//粗定位步长
		int iEssenceStep = 1;			//精定位步长
	};
	struct DoubleParamsEdgeAlign//double型算法参数，独立出来方便使用默认构造函数
	{
		double dMultiple = 2.0;				//复判灰度倍数
		double dValueDif = 10.0;			//复判灰度绝对差值
		double dRansacDis = 2.0;			//直线拟合Ransac距离
		double dTimeOutThreshold = 1000;               // 算法超时设定
	};
	struct AlgParamEdgeAlign
	{
		union
		{
			IntParamsEdgeAlign block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsEdgeAlign block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgEdgeAlign
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgEdgeAlign()
		{
			msgs.push_back("isDepthImage      $Int  $CheckBox   $0   $是否为深度图                      $默认为否，勾选时为是，输入为深度图会对图像灰度值进行拉伸  $1");
			msgs.push_back("isComputeFlaw			$ Int    $ CheckBox		$ 0 	$ 是否计算缺陷信息				            $ 当勾选时，计算产品Body缺陷信息，不勾选时，不进行二值化 $20");
			msgs.push_back("isPreprocessing			$ Int    $ CheckBox		$ 0 	$ 是否进行二值化				            $ 当勾选，进行二值化，不勾选时，不进行二值化 $40");
			msgs.push_back("iThreshold				$ Int    $ I_SpinBox	$ 50 	$ 二值化阈值(单位：灰度值)					$ 默认值50，范围：[0,255]。前景背景的交界灰度值 $60");
			msgs.push_back("iEdgePolarity			$ Int    $ Combox	    $ 0		$ 图像边缘属性        					    $ 待检测的点沿着扫描线的方向灰度变化 ￥ALL ￥黑到白 ￥白到黑 $80");
			msgs.push_back("iMinValue				$ Int    $ I_SpinBox	$ 15 	$ 边缘灰度阈值(单位：梯度值)				$ 默认值15，范围：[0,255]。灰度最小梯度(边缘检测阈值) $100");
			msgs.push_back("iPointCompoundSentence      $Int  CheckBox   $0   $是否边缘点复判                      $当勾选，进行边缘点复判，不勾选时，不进行边缘点复判  $125");
			msgs.push_back("iOffset					$ Int    $ I_SpinBox	$ 10 	$ 边缘点复判宽度(单位：像素)				$ 默认值10，范围：[1,∞]。边缘点复判宽度，由于板材下边界成像虚焦，所以此参数可以稍微大一些 $120");
			msgs.push_back("dMultiple				$ Double $ D_SpinBox	$ 2.0  	$ 边缘点复判灰度倍数			            $ 默认值2，边缘点前后法线方向的灰度倍数值 $140");
			msgs.push_back("dValueDif				$ Double $ D_SpinBox	$ 10.0  $ 复判灰度绝对差值(单位：灰度值)			$ 默认值10，范围：[0,255]。边缘点前后法线方向的灰度绝对差值 $70");
			msgs.push_back("dRansacDis				$ Double $ D_SpinBox	$ 1.0	$ 直线拟合Ransac距离(单位：像素)			$ 默认值1，范围：[1,∞]。拟合直线用于Ransac的数值 $160");
			msgs.push_back("iFitLinePara			$ Int    $ Combox	    $ 1		$ fitline模式					            $ 拟合直线的距离类型 ￥DIST_L1 ￥DIST_L2 ￥DIST_L12 ￥DIST_FAIR ￥DIST_WELSCH ￥DIST_HUBER $165");
			msgs.push_back("iCoarseStep				$ Int    $ I_SpinBox	$ 5 	$ 粗定位(单位：像素)						$ 默认值5，范围：[1,∞]。获取粗定位边缘的步长，用于缩短检测时间 $180");
			msgs.push_back("iEssenceStep			$ Int    $ I_SpinBox	$ 1 	$ 精定位(单位：像素)						$ 默认值1，范围：[1,∞]。获取精定位边缘的步长，控制输出轮廓点的精度 $200");
			msgs.push_back("dTimeOutThreshold      $Double  $D_Spinbox   $1000.0   $算法超时设定(单位：ms)                      $默认值1000.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常 $220");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgEdgeAlign
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgEdgeAlign() {
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgEdgeAlign
	{
		std::vector<std::string> msgs;
		AlgErrMsgEdgeAlign()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入Roi异常不在当前图像内");
			msgs.push_back("2:ROI过小不符合规范");
			msgs.push_back("3:边缘点失败");
			msgs.push_back("4:拟合直线失败");
			msgs.push_back("5:拟合直线与ROI求交点失败");
			msgs.push_back("6:拟合直线与图片边缘求交点失败");
			msgs.push_back("7:拟合直线输入参数异常");
			msgs.push_back("8:输入矩形ROI个数应为4个");
			msgs.push_back("9:输入的拟合直线参数应为1个或4个");
			msgs.push_back("10:构造交点失败");
			msgs.push_back("11:输入Mat图像为空");
			msgs.push_back("12:输入区域为空");
			msgs.push_back("13:算法超时参数输入有误");
			msgs.push_back("14:算法超时");
			msgs.push_back("15:不支持的深度图类型");
			msgs.push_back("16:未知错误");
			msgs.push_back("17:输入参数异常,边缘灰度阈值超出范围限定(0,128)");
			msgs.push_back("18:输入ROI为空");
			msgs.push_back("19:输入参数异常,复判宽度超出范围限定[1,∞)");
			msgs.push_back("20:输入参数异常,复判灰度倍数超出范围限定(0,∞)");
			msgs.push_back("21:输入参数异常,复判灰度绝对差值超出范围限定[0,255]");
			msgs.push_back("22:输入参数异常,二值化阈值超出范围限定[0,255]");
			msgs.push_back("23:输入参数异常,直线拟合Ransac距离超出范围限定(0,∞)");
			msgs.push_back("24:输入参数异常,粗定位步长超出范围限定[1,∞)");
			msgs.push_back("25:输入参数异常,精定位步长超出范围限定[1,∞)");
			msgs.push_back("26:输入图像为彩色图，需要预处理转换为灰度图");
			msgs.push_back("27:输入参数异常，是否进行图像预处理参数输入异常");
			msgs.push_back("28:输入参数异常，图像边缘属性参数输入异常");
			msgs.push_back("29:输入参数异常，图像边缘提取方式参数输入异常");
			msgs.push_back("30:输入参数异常，边缘点筛选参数输入异常");
			msgs.push_back("31:输入参数异常，边缘点复判参数输入异常");
			msgs.push_back("32:输入参数异常，fitline模式参数输入异常");
			msgs.push_back("33:输入参数异常，标定参数输入异常");
		}
	};

	//! 接口

	namespace alg {
		
		//-----------------------------EdgeAlign------------------------------------------//
		//函数说明：采用拟合产品四条边缘的方式构建坐标系，计算待测图到Layout以及待测图到模板图的坐标变换关系
		//EdgeAlignInput &input;			输入：待测产品的Mat图像，四个扫描矩形ROI以及高精度拟合直线的参数
		//EdgeAlignOutput &output; 			输出：模板图到待测图、Layout到待测图、Layout到模板图的坐标变换关系、产品四个顶点的坐标及角度信息，可以计算产品的垂直度，平行度，封装宽度和高度等特征(可选)
		_ALGLIBRARY_API int EdgeAlign(const EdgeAlignInput& input, EdgeAlignOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_EDGEALIGN_H
