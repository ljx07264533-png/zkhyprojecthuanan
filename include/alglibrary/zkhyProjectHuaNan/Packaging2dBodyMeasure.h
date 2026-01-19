
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_PACKAGING2DBODYMEASURE_H
#define ZKHYPROJECTHUANAN_PACKAGING2DBODYMEASURE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//-----------------------------Packaging2dBodyMeasure 获取封测产品尺寸信息 算子------------------------------------------//
	struct Packaging2dBodyMeasureInput
	{
		//HalconCpp::HObject detICRegion;                       //分割出的产品区域
		alglib::core::CSYSTransPara  detTransPara;               //待测图的中心点和角度信息
		std::vector<cv::Point2f> vertexs;                   //四边形的四个顶点信息，依次为左上，左下，右下，右上
		std::vector<double> crossAngles;                //四边形四个顶点对应的内角信息
		alglib::core::AlgrithmParams algPara;					   //算法参数
	};
	struct Packaging2dBodyMeasureOutput
	{
		std::vector<alglib::core::FlawInfoStruct>  flawsData;               //缺陷信息
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct IntParamsPackaging2dBodyMeasure//int型算法参数，独立出来方便使用默认构造函数
	{
		int iPackagingType = 0;                   //封装类型
	};
	struct DoubleParamsPackaging2dBodyMeasure//double型算法参数，独立出来方便使用默认构造函数
	{
		double dTimeOutThreshold = 1000;               // 算法超时设定
	};
	struct AlgParamPackaging2dBodyMeasure
	{
		union
		{
			IntParamsPackaging2dBodyMeasure block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsPackaging2dBodyMeasure block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgPackaging2dBodyMeasure
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgPackaging2dBodyMeasure()
		{
			msgs.push_back("iPackagingType      $Int  $Combox   $0   $封装类型                      $默认为BGA封装，支持BGA、QFN封装 ￥BGA　￥QFN $10");
			msgs.push_back("dTimeOutThreshold      $Double  $D_Spinbox   $1000.0   $算法超时设定(单位：ms)                      $默认值1000.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常 $20");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgPackaging2dBodyMeasure
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgPackaging2dBodyMeasure() {
			// 增加获取封测产品尺寸信息算子的扩增特征信息
			exmsgs.push_back("bodyParallelism          $Double	   $平行度	$封装边缘和标准矩形的平行度          $90   $CF ");
			exmsgs.push_back("bodyOrthogonality          $Double	   $垂直度	$封装四个内角的最大值         $91   $CF ");
			exmsgs.push_back("bodySizeX          $Double	   $封装宽度	$封装宽度          $92   $CF ");
			exmsgs.push_back("bodySizeY          $Double	   $封装高度	$封装高度          $93   $CF ");
			exmsgs.push_back("bodyEdgeStraightness          $Double	   $封装边缘线性度	$封装边缘线性度          $94   $CF ");
		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgPackaging2dBodyMeasure
	{
		std::vector<std::string> msgs;
		AlgErrMsgPackaging2dBodyMeasure()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:算法超时参数输入有误");
			msgs.push_back("2:算法超时");
			msgs.push_back("3:未知错误");
		}
	};

	//! 接口

	namespace alg {
		//-----------------------------Packaging2dBodyMeasure------------------------------------------//
		//函数说明：计算封测产品的垂直度，平行度，封装宽度和高度信息
		//Packaging2dBodyMeasureInput &input;				输入：产品的四个顶点及其对应的四个内角角度信息、产品的封装形式
		//Packaging2dBodyMeasureOutput &output; 			输出：封测产品的垂直度，平行度，封装宽度和高度
		_ALGLIBRARY_API int Packaging2dBodyMeasure(const Packaging2dBodyMeasureInput& input, Packaging2dBodyMeasureOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_PACKAGING2DBODYMEASURE_H
