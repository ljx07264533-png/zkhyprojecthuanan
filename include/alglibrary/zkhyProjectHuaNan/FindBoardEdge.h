
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_FINDBOARDEDGE_H
#define ZKHYPROJECTHUANAN_FINDBOARDEDGE_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//————————————————————————FindBoardEdge 板材封边高低检测算子 Begin————————————————//
	//Step1 输入结构体
	struct FindBoardEdgeInput
	{
		cv::Mat src;						//输入灰度图像
		//alglib::core::RectangleROI Roi;					//高精拟合直线的ROI
		alglib::core::AlgrithmParams algPara;			    //算法参数
	};
	//Step2 输出结构体
	struct FindBoardEdgeOutput
	{
		std::vector<alglib::core::FlawInfo>	flawinfo;				//缺陷信息

	};
	//Step3 算法参数（int double）
	struct AlgParaFindBoardEdgeInt			//int型算法参数，独立出来方便使用默认构造函数
	{
		//region信息
		int upYBegin = 100;						//上侧region的起始y
		int upWidth = 70;						//上侧region的宽
		int upHeight = 15;						//上侧region的高



		//int downYBegin = 100;					//下侧region的起始y
		//int downHeight = 15;					//下侧region的高

		//拟合直线参数
		int isBinary = 0;						//拟合直线是否二值化找点
		int binThreshold = 70;					//二值化阈值
		int edgeThreshold = 15;					//找点梯度阈值
		int nEssenceOffset = 40;				//精定位宽度

		//算法参数

	};
	struct AlgParaFindBoardEdgeDouble    //double型算法参数，独立出来方便使用默认构造函数
	{
		//拟合直线参数
		double ransacDist = 1;					//ransac距离

		//算法参数
		double UDGrayDiff = 12.0;				//上下region的灰度差阈值
		double LRGrayDiff = 20.0;				//左右region的灰度差阈值

	};
	struct AlgParaFindBoardEdge
	{
		union
		{
			AlgParaFindBoardEdgeInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaFindBoardEdgeDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgFindBoardEdge
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgFindBoardEdge()
		{
			msgs.push_back("isBinary				$Int		$Checkbox	 $0  	 $是否二值化			 $是否二值化后再找边缘点		$10");
			msgs.push_back("binThreshold			$Int		$I_Spinbox	 $70  	 $二值化阈值			 $在勾选二值化时才生效			$20");
			msgs.push_back("edgeThreshold			$Int		$I_Spinbox	 $15  	 $边缘点梯度阈值		 $非二值化下找边缘点的阈值		$30");
			msgs.push_back("ransacDist				$double		$D_Spinbox	 $1  	 $ransac距离			 $ransac距离					$40");
			msgs.push_back("nEssenceOffset			$int		$I_Spinbox	 $40  	 $精定位宽度			 $精定位宽度					$41");
			msgs.push_back("upYBegin				$Int		$I_Spinbox	 $100  	 $上部roi起始y			 $上部roi起始y坐标				$50");
			msgs.push_back("upWidth					$Int		$I_Spinbox	 $3  	 $上部roi宽				 $上部roi宽						$60");
			msgs.push_back("upHeight				$Int		$I_Spinbox	 $100  	 $上部roi高				 $上部roi高						$70");
			msgs.push_back("UDGrayDiff				$double		$D_Spinbox	 $12.0   $上下侧灰度差			 $上下侧灰度差					$80");
			msgs.push_back("LRGrayDiff				$double		$D_Spinbox	 $20.0   $左右侧灰度差			 $左右侧灰度差					$90");

		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgFindBoardEdge
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgFindBoardEdge()
		{
			// 如果需要新增缺陷特征，在这里面写

		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgFindBoardEdge
	{
		std::vector<std::string> msgs;
		AlgErrMsgFindBoardEdge()
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
			msgs.push_back("27:算法运行异常");

		}
	};
	//————————————————————————FindBoardEdge 板材封边高低检测算子 End————————————————//

	//! 接口

	namespace alg {
		
		//--------------------------------------------FindBoardEdge算子-----------------------//
		//名称：  FindBoardEdge
		//功能：  找首尾图与板子边缘位置，对外接口
		//返回值： 0：无异常
		//        26：输入图像为空
		//		  27: 拟合直线找到的点小于50
		//		  28：fengBianHeightDet失败
		//		  29：此算子运行异常
		//------------------------------------------------------------------------------------------//
		_ALGLIBRARY_API int FindBoardEdge(const FindBoardEdgeInput& input, FindBoardEdgeOutput& result);
	}

}

#endif	// ZKHYPROJECTHUANAN_FINDBOARDEDGE_H
