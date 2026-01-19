
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_DETFENGBIANHEIGHT_H
#define ZKHYPROJECTHUANAN_DETFENGBIANHEIGHT_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"
#include "alglibrary/alglibLocation/gatherLineHighPrecision.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//————————————————————————DetFengBianHeight 板材封边高低检测算子 Begin————————————————//
	//Step1 输入结构体
	struct DetFengBianHeightInput
	{
		cv::Mat src;						//输入灰度图像
		alglib::core::RectangleROI Roi;					//高精拟合直线的ROI
		alglib::core::AlgrithmParams algPara;			    //算法参数
	};
	//Step2 输出结构体
	struct DetFengBianHeightOutput
	{
		std::vector<alglib::core::FlawInfo>	flawinfo;				//缺陷信息
	};
	//Step3 算法参数（int double）
	struct AlgParaDetFengBianHeightInt			//int型算法参数，独立出来方便使用默认构造函数
	{
		//拟合直线参数
		int isBinary = 0;						//拟合直线是否二值化找点
		int binThreshold = 70;					//二值化阈值
		int edgeThreshold = 15;					//找点梯度阈值
		int nEssenceOffset = 40;				//精定位宽度

		//算法参数
		int offset = 50;						//找异常点的y向起始范围，从ransac之后的靠近封边带那一端的最后一个点往上offset开始，往下找异常点
	};
	struct AlgParaDetFengBianHeightDouble    //double型算法参数，独立出来方便使用默认构造函数
	{
		//拟合直线参数
		double ransacDist = 1;					//ransac距离
		//算法参数
		double threshold = 0.2;					//检规，单位mm
		double pixelAcc = 0.038;				//像素精度（像素当量）
		double angle = 45.0;					//激光与相机夹角，单位是角度
	};
	struct AlgParaDetFengBianHeight
	{
		union
		{
			AlgParaDetFengBianHeightInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaDetFengBianHeightDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgDetFengBianHeight
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgDetFengBianHeight()
		{
			msgs.push_back("isBinary				$Int		$Checkbox	 $0  	 $是否二值化			 $是否二值化后再找边缘点		$10");
			msgs.push_back("binThreshold			$Int		$I_Spinbox	 $70  	 $二值化阈值			 $在勾选二值化时才生效			$20");
			msgs.push_back("edgeThreshold			$Int		$I_Spinbox	 $15  	 $边缘点梯度阈值		 $非二值化下找边缘点的阈值		$30");
			msgs.push_back("ransacDist				$double		$D_Spinbox	 $1  	 $ransac距离			 $ransac距离					$40");
			msgs.push_back("nEssenceOffset			$int		$I_Spinbox	 $40  	 $精定位宽度			 $精定位宽度					$41");
			msgs.push_back("offset					$Int		$I_Spinbox	 $50  	 $起始点偏移量			 $找异常点的y向起始范围，\
				从ransac之后的靠近封边带那一端的最后一个点往上offset开始，往下找异常点		$50");
			msgs.push_back("threshold				$double		$D_Spinbox	 $0.2  	 $封边高低检规			 $单位mm					$60");
			msgs.push_back("pixelAcc				$double		$D_Spinbox	 $0.065  	 $像素当量				 $像素当量					$70");
			msgs.push_back("angle					$double		$D_Spinbox	 $30  	 $相机与激光的夹角		 $单位mm					$80");

		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgDetFengBianHeight
	{
		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgDetFengBianHeight()
		{
			// 如果需要新增缺陷特征，在这里面写


		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgDetFengBianHeight
	{
		std::vector<std::string> msgs;
		AlgErrMsgDetFengBianHeight()
		{
			//高精拟合直线的结果
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像为空");
			msgs.push_back("2:ROI过小不符合规范");
			msgs.push_back("3:边缘点失败");
			msgs.push_back("4:拟合直线失败");
			msgs.push_back("5:拟合直线与ROI求交点失败");
			msgs.push_back("6:拟合直线与图片边缘求交点失败");
			msgs.push_back("7:输入参数异常,边缘灰度阈值超出范围限定(0,128)");
			msgs.push_back("8:输入ROI为空");
			msgs.push_back("9:输入参数异常,复判宽度超出范围限定[1,∞)");
			msgs.push_back("10:输入参数异常,复判灰度倍数超出范围限定(0,∞)");
			msgs.push_back("11:输入参数异常,复判灰度绝对差值超出范围限定[0,255]");
			msgs.push_back("12:输入参数异常,二值化阈值超出范围限定[0,255]");
			msgs.push_back("13:输入参数异常,直线拟合Ransac距离超出范围限定(0,∞)");
			msgs.push_back("14:输入参数异常,粗定位步长超出范围限定[1,∞)");
			msgs.push_back("15:输入参数异常,精定位步长超出范围限定[1,∞)");
			msgs.push_back("16:输入图像为彩色图，需要预处理转换为灰度图");
			msgs.push_back("17:输入参数异常，是否进行图像预处理参数输入异常");
			msgs.push_back("18:输入参数异常，图像边缘属性参数输入异常");
			msgs.push_back("19:输入参数异常，图像边缘提取方式参数输入异常");
			msgs.push_back("20:输入参数异常，边缘点筛选参数输入异常");
			msgs.push_back("21:输入参数异常，边缘点复判参数输入异常");
			msgs.push_back("22:输入参数异常，fitline模式参数输入异常");
			msgs.push_back("23:输入参数异常，标定参数输入异常");
			msgs.push_back("24:输入参数异常,精检测宽度超出范围限定[1,∞)");
			msgs.push_back("25:输入参数异常,扫描线滤波宽度大于等于精检测宽度，无意义");
			//封边高低对外算子错误码
			msgs.push_back("26:输入图像为空");	//与上面的不同，这里是算子开始处的
			msgs.push_back("27:拟合直线找到的点小于50");
			msgs.push_back("28:封边高低检测失败");
			msgs.push_back("29:算子运行异常");

		}
	};
	//————————————————————————DetFengBianHeight 板材封边高低检测算子 End————————————————//

	//封边带高度检测输入参数
	struct FBHeightDetInput
	{
		alglib::ops::location::GatherLineHighPrecisionOutput& output;	//直线拟合（fitLine()）的输出结果
		float threshold = 0.2;					//检规，单位mm
		float pixelAcc = 0.038;				//像素精度（像素当量）
		float angle = 45.0;						//激光与相机夹角，单位是角度
		//float fengBianDiThresholdY = 10.0;		//封边低异常y向阈值，单位是像素
		int offset = 50;					//找异常点的y向起始范围，从ransac之后的靠近封边带那一端的最后一个点往上offset开始，往下找异常点
	};

	//封边带高度检测输出参数
	struct FBHeightDetOutput
	{
		float height = 0.0;
		cv::Rect box = cv::Rect(0, 0, 0, 0);
		//alglib::core::FlawInfoStruct flaw;
	};

	//! 接口

	namespace alg {
		
		//--------------------------------------------DetFengBianHeight算子-----------------------//
		//名称：  DetFengBianHeight
		//功能：  封边带高低检测，对外接口
		//返回值： 0：无异常
		//        26：输入图像为空
		//		  27: 拟合直线找到的点小于50
		//		  28：fengBianHeightDet失败
		//		  29：此算子运行异常
		//------------------------------------------------------------------------------------------//
		_ALGLIBRARY_API int DetFengBianHeight(const DetFengBianHeightInput& input, DetFengBianHeightOutput& result);
	}


	//--------------------------------------------fitLine-----------------------//
	//名称：  fitLine
	//功能：  在设定的roi内寻找边缘点集并拟合直线，
	//返回值： 0-成功    非0-失败
	//参数列表：采集的原图像
	//			拟合直线的返回结果，包括原始点集和ransac之后的点集
	//			是否二值化（检测封边高低用二值化，检测立面不用）
	//			二值化阈值
	//			杳找边缘点的梯度阈值
	//			ransac距离
	//------------------------------------------------------------------------------------------//
	int fitLine(const cv::Mat & src, alglib::ops::location::GatherLineHighPrecisionOutput& output, alglib::core::RectangleROI ROI, const bool binary, const int binThreshold, const int gradThreshold, const double ransacDist, const int essenceOffset);

	//--------------------------------------------fengBianHeightDet-----------------------//
	//名称：  fengBianHeightDet
	//功能：  封边带高低异常检测
	//返回值： 0：无异常
	//        1：封边带高
	//		  2: 封边带低
	//------------------------------------------------------------------------------------------//
	int fengBianHeightDet(const FBHeightDetInput& input, FBHeightDetOutput& result);


}

#endif	// ZKHYPROJECTHUANAN_DETFENGBIANHEIGHT_H
