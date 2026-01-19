
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_EMPTYCHECKERBLOB_H
#define ZKHYPROJECTHUANAN_EMPTYCHECKERBLOB_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//——-----------------------------EmptyCheckerBlob 判断芯片有无 Begin————————————//
	struct EmptyCheckerBlobInput
	{
		HalconCpp::HObject srcImage;						//检测图像
		HalconCpp::HObject ho_region;						//检测区域
		std::vector<cv::Point2f> detectROI;          //检测ROI
		alglib::core::AlgrithmParams algPara;					//算法参数
	};
	struct EmptyCheckerBlobOutput
	{
		cv::Mat detectImg;                          //二值化后的图像
		std::vector<alglib::core::FlawInfoStruct> flaws;               //缺陷信息
	};
	struct AlgParaEmptyCheckerBlobInt          //int型算法参数，独立出来方便使用默认构造函数
	{
		int flagOutputShowImg;                  //是否显示二值化图像
		int lowThreshold = 105;					// 阈值化的低高阈值
		int highThreshold = 255;
		int minblobArea = 30;					 // 筛选面积小的噪声，保留字符
	};
	struct AlgParaEmptyCheckerBlob
	{
		union
		{
			AlgParaEmptyCheckerBlobInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			struct
			{
			}block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgEmptyCheckerBlob
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgEmptyCheckerBlob()
		{
			msgs.push_back("flagOutputShowImg	$Int       $CheckBox	 $0  	 $是否显示二值化图像		$正常跑料时，不需要勾选该项				$1");
			msgs.push_back("lowThreshold		$Int       $I_Spinbox	 $150  	 $blob低灰度阈值				$字符低阈值				$10");
			msgs.push_back("highThreshold		$Int       $I_Spinbox	 $255  	 $blob高灰度阈值				$字符高阈值				$20");
			msgs.push_back("minblobArea			$Int       $I_Spinbox	 $280  	 $blob最小面积				$小于blob最小面积阈值的不参与统计				$30");
		}
	};
	struct AlgFeatureMsgEmptyCheckerBlob
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgEmptyCheckerBlob()
		{
			exmsgs.push_back("emptyCheckerBlob          $Double	   $emptyCheckerBlob			$blob个数				$100   $CF ");
		}
	};
	//Step4 异常返回值描述
	struct AlgErrMsgEmptyCheckerBlob
	{
		std::vector<std::string> msgs;
		AlgErrMsgEmptyCheckerBlob()
		{
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:算子异常");
		}
	};

	//——-----------------------------EmptyCheckerBlob 判断芯片有无 End————————————//


	//! 接口

	namespace alg {
		
		//-----------------------------emptyCheckerBlob------------------------------------------//
		//函数说明：判断产品有无
		//EmptyCheckerBlobInput &input;				输入：
		//EmptyCheckerBlobInput &output; 			输出：物料中的blob个数
		_ALGLIBRARY_API int emptyCheckerBlob(const EmptyCheckerBlobInput& input, EmptyCheckerBlobOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_EMPTYCHECKERBLOB_H
