
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_SAVEDEFECTIMAGE_H
#define ZKHYPROJECTHUANAN_SAVEDEFECTIMAGE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体


	//---------------------存储缺陷小图算子 BEGIN-----------------//
	struct saveDefectImageInput
	{
		cv::Mat srcImg;		//原图
		std::vector <alglib::core::FlawInfo> flawInfos;		//缺陷信息
		std::vector <alglib::core::FlawInfoStruct> flawsInfoStructs;		//缺陷信息
		std::vector <Logic_Flaw_ResultAlg> flawsFilter;		//筛选后的缺陷信息
		char imageSavePath[MAX_PATH] = "";		//缺陷小图存储路径
		char jsonSavePath[MAX_PATH] = "";		//缺陷信息存储路径

		char productInfo[2048] = "";		//产品信息
		char environmentInfo[2048] = "";		//运行环境信息

		std::string description = "";		//描述信息

		std::string descriptionCustom = "";		//输入描述信息

		alglib::core::AlgrithmParams algPara;		//算法参数
	};
	struct saveDefectImageOutput
	{
		char* strJson;		//输出json字符串
	};
	//Step2 算子参数 结构体定义 int和double分开写
	struct IntParamsSaveDefectImage//int型算法参数，独立出来方便使用默认构造函数
	{
		int iImageWidth = 128;		// 缺陷小图宽度
		int iImageHeight = 128;		// 缺陷小图高度
		int iLineWidth = 1;		// 绘制轮廓的线宽
		int iCharSize = 30;		// 绘制文字的大小
		int iIsShowSize = 1;	// 是否显示长宽
		int iIsShowArea = 1;	// 是否显示面积
		int iIsShowScore = 1;	// 是否显示分数
		int iIsStoreSeparately = 0;	// 是否分开存储代标注与不带标注的缺陷图以及缺陷大图
		int iBoundingDist = 50;	// 缺陷框与图像边界的距离
		int iIsStoreImageFull = 0;	// 是否存储缺陷大图
		int iIsStoreJson = 0;	// 是否存储缺陷信息
		int iSaveModel = 0;		// 存储模式
		int iMaxImageSavedNum = 100;	// 存储缺陷小图最大数量
		int isUse = 0;					// 是否启用算子
		int iThreadNumWriteImage = 0;					// 用于存储缺陷小图的多线程数量
	};
	struct DoubleParamsSaveDefectImage//double型算法参数，独立出来方便使用默认构造函数
	{
		double dTimeOutThreshold = 3000;		// 算法超时设定
		double dMaxArea = 500;						// 存储缺陷的最大面积
	};
	struct AlgParamSaveDefectImage
	{
		union
		{
			IntParamsSaveDefectImage block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			DoubleParamsSaveDefectImage block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	//Step3 算子参数描述
	struct AlgParaMsgSaveDefectImage
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
		//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgSaveDefectImage()
		{
			msgs.push_back("iImageWidth	$Int			$I_Spinbox	$128  				$缺陷小图宽度		$缺陷小图宽度，默认值128，范围[1,∞]	$10");
			msgs.push_back("iImageHeight	$Int			$I_Spinbox	$128  				$缺陷小图高度		$缺陷小图高度，默认值128，范围[1,∞]	$20");
			msgs.push_back("iLineWidth   $Int			$I_Spinbox	$1			$轮廓线宽	 $绘制轮廓的线宽，默认值1，范围[1,∞]	$30");
			msgs.push_back("iCharSize    $Int			$I_Spinbox	$30			$文字大小	 $绘制文字的大小，默认值30，范围[1,∞]	$40");
			msgs.push_back("iIsShowSize  $Int			$CheckBox	$1			$是否显示长宽	 $是否显示长宽，默认值1	$50");
			msgs.push_back("iIsShowArea  $Int			$CheckBox	$1			$是否显示面积	 $是否显示面积，默认值1	$60");
			msgs.push_back("iIsShowScore $Int			$CheckBox	$1			$是否显示分数	 $是否显示分数，默认值1	$70");
			msgs.push_back("iIsStoreSeparately $Int	$CheckBox	$0			$是否分开存储	 $是否分开存储代标注与不带标注的缺陷图以及缺陷大图，默认值0	$80");
			msgs.push_back("iBoundingDist $Int			$I_Spinbox	$50			$缺陷框与图像边界的距离	 $缺陷框与图像边界的距离，仅在缺陷尺寸大于设置缺陷小图尺寸时有效。默认值50，范围[0,∞]	$90");
			msgs.push_back("dTimeOutThreshold      $Double  $D_Spinbox   $3000.0   $算法超时设定(单位：ms)                      $默认值3000.0，范围[0,∞]。算子运行时间大于此参数时将触发超时熔断机制，返回超时异常 $100");
			msgs.push_back("iIsStoreImageFull $Int	$CheckBox	$1			$是否存储缺陷大图	 $是否存储缺陷大图，默认值0	$110");
			msgs.push_back("iIsStoreJson $Int			$CheckBox	$1			$是否存储缺陷信息	 $是否存储缺陷信息，默认值0	$120");
			msgs.push_back("iSaveModel $Int			$Combox		$0			$存储模式，用于不同项目定制化需求	 $存储模式，默认值0：￥默认 ￥MINILED ￥卷带包装机	$130");
			msgs.push_back("iMaxImageSavedNum $Int			$Combox		$0			$存储缺陷小图最大数量	 $存储缺陷小图最大数量，默认值100	$140");
			msgs.push_back("iThreadNumWriteImage $Int			$Combox		$2			$多线程数量	 $用于存储缺陷小图的多线程数量，默认值2	$150");
		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgSaveDefectImage
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgSaveDefectImage() {
		}
	};
	//Step5 异常返回值描述 锡球特征检测错误信息结构体
	struct AlgErrMsgSaveDefectImage
	{
		std::vector<std::string> msgs;
		AlgErrMsgSaveDefectImage()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:缺陷信息与缺陷筛选信息不匹配");
			msgs.push_back("3:存储路径异常");
			msgs.push_back("4:图像宽度参数输入有误");
			msgs.push_back("5:图像高度参数输入有误");
			msgs.push_back("6:线宽参数输入有误");
			msgs.push_back("7:字体大小参数输入有误");
			msgs.push_back("8:算法超时参数输入有误");
			msgs.push_back("9:算法超时");
			msgs.push_back("10:未知异常");
			msgs.push_back("11:输入产品信息异常");
			msgs.push_back("12:输入运行信息异常");
			msgs.push_back("13:时间戳格式错误");
		}
	};
	//---------------------存储缺陷小图算子 END-----------------//


	//! 接口

	namespace alg {
		
		//-----------------------------saveDefectImage-----------------------------//
		//函数说明：存储缺陷小图
		//saveDefectImageInput &input;				输入：筛选前后的缺陷信息、缺陷小图存储路径、Mat图像
		//saveDefectImageOutput &output; 			输出：
		_ALGLIBRARY_API int saveDefectImage(const saveDefectImageInput& input, saveDefectImageOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_SAVEDEFECTIMAGE_H
