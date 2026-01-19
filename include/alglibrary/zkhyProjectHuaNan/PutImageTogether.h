
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_PUTIMAGETOGETHER_H
#define ZKHYPROJECTHUANAN_PUTIMAGETOGETHER_H


#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//————————————————————————PutImageTogether 拼图算子 Begin————————————————//
	//Step1 输入结构体
	struct PutImageTogetherInput
	{
		std::vector<cv::Mat> smallImgs;						//输入小图
		alglib::core::AlgrithmParams algPara;			    //算法参数
	};
	//Step2 输出结构体
	struct PutImageTogetherOutput
	{
		cv::Mat img;				//拼图结果
		std::vector<cv::Point2f> pts;		//检测输出轮廓

	};
	//Step3 算法参数（int double）
	struct AlgParaPutImageTogetherInt			//int型算法参数，独立出来方便使用默认构造函数
	{
		//算法参数
		int mode = 0;						//拼图模式：0纵向拼，1横向拼
		int rotate = 0;						//旋转角度：0不旋，1顺时针90，2逆时针90
		int mirror = 0;						//镜像：0不镜像，1水平镜像，2垂直镜像
		int ovcerlapNum = 0;
		int roi_lefttop_cols = 0;
		int roi_lefttop_rows = 0;
		int roi_rightdown_cols = 0;
		int roi_rightdown_rows = 0;
	};
	struct AlgParaPutImageTogetherDouble    //double型算法参数，独立出来方便使用默认构造函数
	{

	};
	struct AlgParaPutImageTogether
	{
		union
		{
			AlgParaPutImageTogetherInt block;
			int data[PARAMS_MAXNUM];
		}i_params;
		union
		{
			AlgParaPutImageTogetherDouble block;
			double data[PARAMS_MAXNUM];
		}d_params;
	};
	struct AlgParaMsgPutImageTogether
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型   Int  Double
	//控件类型   D_SpinBox  I_Spinbox Combox CheckBox
		//“参数标识（只看不用）  $ 数据类型  $ 控件    $ 默认值$ 参数名称   $ 参数意义  ￥下拉框的定义”
		std::vector<std::string> msgs;
		AlgParaMsgPutImageTogether()
		{
			msgs.push_back("mode			$Int		$Combox	 $0  	 $拼图模式			 $0从上往下，1从左往右，2从下往上，3从右往左	￥从上往下 ￥从左往右 ￥从下往上  ￥从右往左	$10");
			msgs.push_back("rotate			$Int		$Combox	 $0  	 $旋转角度			 $0不旋转，1顺时针90，2逆时针90	￥不旋转 	￥顺时针90 ￥逆时针90	 $20");
			msgs.push_back("mirror			$Int		$Combox	 $0  	 $镜像				 $0不镜像，1水平镜像，2垂直镜像	￥不镜像  ￥水平镜像	￥垂直镜像   $30");
			msgs.push_back("overlapNum		$Int		$I_Spinbox	 $0  $拼接方向重叠大小	 $0拼接方向的重叠区域大小 $40");
			msgs.push_back("roi_lefttop_cols		$Int		$I_Spinbox	 $0  $重叠区域左上列坐标	 $0重叠区域左上列坐标 $50");
			msgs.push_back("roi_lefttop_rows		$Int		$I_Spinbox	 $0  $重叠区域左上行坐标	 $0重叠区域左上行坐标 $60");
			msgs.push_back("roi_rightdown_cols		$Int		$I_Spinbox	 $0  $重叠区域右下列坐标	 $0重叠区域右下列坐标 $70");
			msgs.push_back("roi_rightdown_rows		$Int		$I_Spinbox	 $0  $重叠区域右下行坐标	 $0重叠区域右下行坐标 $80");

		}
	};
	//Step4 特征参数描述（新增的特征变量 需要在 zkhy_dataType的ExtendParameters特征中追加，新增加的特征描述需要在alglib::core::FlawInfoExtendMsgs追加）
	struct AlgFeatureMsgPutImageTogether
	{

		std::vector<std::string> msgs;	//标准特征（与获取缺陷特征 一致的特征变量 描述字段顺序 与之前定义的保持一致 这样在分类器中位置就可以保持一致）
		std::vector<std::string> exmsgs;  //扩增特征  （包括已有的扩展特征 以及新增的特征变量）
		AlgFeatureMsgPutImageTogether()
		{
			// 如果需要新增缺陷特征，在这里面写

		}
	};
	//Step5 异常返回值描述
	struct AlgErrMsgPutImageTogether
	{
		std::vector<std::string> msgs;
		AlgErrMsgPutImageTogether()
		{
			//高精拟合直线的结果
			msgs.push_back("0:成功");
			msgs.push_back("1:输入图像个数不为10");
			msgs.push_back("2:拼接失败");

		}
	};
	//————————————————————————PutImageTogether 拼图算子 End————————————————//
	// 

	//! 接口

	namespace alg {
		
		_ALGLIBRARY_API int PutImageTogether(const PutImageTogetherInput& input, PutImageTogetherOutput& output);
	}


}

#endif	// ZKHYPROJECTHUANAN_PUTIMAGETOGETHER_H
