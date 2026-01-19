
/**************************************************
* @author: FuFa
* @email: alanmathisonturing@163.com
* @date: 2024/03/22 13:43
* @description: todo，详细内容:todo
***************************************************/
#ifndef ZKHYPROJECTHUANAN_LOADMODEL_H
#define ZKHYPROJECTHUANAN_LOADMODEL_H

#include "alglibrary/alglibCore.h"
#include "alglibrary/zkhyProjectHuaNan/types.h"


namespace alglib::ops::zkhyProHN {
	//! 参数结构体


	//! 参数信息结构体


	//! 输入结构体


	//! 输出结构体


	//! 错误信息结构体

	//----------------------------------------------加载基于模板图查找异常的模型-------------------------------------//
	struct LoadModelInput {
		SaveShapeModelPath1 loadPath1;//在创建形状匹配模型算子的 路径输入
	};
	struct LoadModelOutput {
		ShapeModelInfo1 shapeInfo1;   //模板图的形状匹配结果
	};
	struct AlgErrorMsgInspectShapeModel
	{
		std::vector<std::string> msgs;
		AlgErrorMsgInspectShapeModel()
		{
			msgs.push_back("0: 成功");
			msgs.push_back("1: 载入图像为空");
			msgs.push_back("2: 未绘制创建模型ROI");
			msgs.push_back("3: 查找模型ROI小于两个");
			msgs.push_back("4: 参数设置不合理，参数有负值");
			msgs.push_back("5: 未知错误");
			msgs.push_back("6: 算法超时");
			msgs.push_back("7: 未载入模板图或模板图为空");
			msgs.push_back("8: 亮暗阈值设置不合理");
			msgs.push_back("9: ROI有交集");

		}
	};
	struct AlgErrorMsgCreatShapeModel
	{
		std::vector<std::string> msgs;
		AlgErrorMsgCreatShapeModel()
		{
			msgs.push_back("0: 成功");
			msgs.push_back("1:  载入图像为空");
			msgs.push_back("2: 输入ROI区域小于两个");
			msgs.push_back("3: 输入ROI区域小于两个");
			msgs.push_back("4: 参数设置不合理");
			msgs.push_back("5: 未知错误");
			msgs.push_back("6: 算法超时");
			msgs.push_back("7: 未绘制模板图ROI");
			msgs.push_back("8: ROI有交集，绘制问题或膨胀值过大");
		}
	};
	struct AlgParamMsgShapeModel
	{
		//初始化data大小
		//格式以$分开，空格不计
		//数据类型 Int  Double
	//控件类型 D_SpinBox I_Spinbox Combox CheckBox
		//                 “算子类型 0-正常参数 1-界面勾选特征 $参数标识（只看不用）   $数据类型  $控件       $默认值  $参数名称               $参数意义  ￥下拉框的定义 $在界面显示顺序，不同算子的显示顺序是独立的 互补影响”
		std::vector<std::string> msgs;
		AlgParamMsgShapeModel()
		{
			msgs.push_back("numMatches      $Int    $I_Spinbox   $1       $匹配目标个数                        $匹配目标个数，当前图有多个目标时候，则填入对应目标的个数 $10");
			msgs.push_back("angleStart      $Double $D_Spinbox   $0.5     $起始旋转角度(单位：角度值)          $默认值-20。创建模板的起始角度 $20");
			msgs.push_back("angleEnd        $Double $D_Spinbox   $20      $终止旋转角度(单位：角度值)          $默认值20。创建模板的终止角度，在内部用是弧度 角度范围为终止-起始角度 $30");
			msgs.push_back("minScore        $Double $D_Spinbox   $0.5     $匹配置信度                          $匹配模型 $40");
			msgs.push_back("scal            $Double $D_Spinbox   $0.5     $缩放比例                            $图像缩放比例 $50");
			msgs.push_back("contrstH            $Double $D_Spinbox   $250     $模型高阈值                      $默认值150，范围：[0,255]。模板图进行模板轮廓获取的高灰度值。$60");
			msgs.push_back("contrstL           $Double $D_Spinbox   $100       $模型低阈值                       $默认值255，范围：[0,255]。模板图进行模板轮廓获取的低灰度值。$70");
			msgs.push_back("contourNumber           $Double $D_Spinbox   $1     $模板轮廓个数                  $默认值1，范围：[0,10]。  创建模板时轮廓个数。$80");
			msgs.push_back("minArea         $Double $D_Spinbox   $0.5     $缺陷最小面积(单位：像素)            $检测缺陷的最小面积。 $90");
			msgs.push_back("fusionDist      $Double $D_Spinbox   $5       $缺陷融合距离(单位：像素)            $检测缺陷的最小融合距离 $100");
			msgs.push_back("threshold1      $Double $D_Spinbox   $40      $亮阈值                        $检测亮的缺陷比正常灰度值高多少 $110");
			msgs.push_back("threshold4      $Double $D_Spinbox   $100     $暗阈值                        $检测暗的缺陷比正常灰度值低多少 $120");
			msgs.push_back("opening         $Double $D_Spinbox    $2       $开运算(单位：像素)                  $缺陷区域开运算半径 $130");
			msgs.push_back("overTimeTh      $Double $D_Spinbox    $3000    $算法超时设定(单位：ms)              $默认值3000，范围：[0,3000]。当算法超过次设定时间时，返回异常。$140");
			msgs.push_back("FindRoi     $Int   $I_Spinbox     $150  $区域膨胀值(单位：像素             $默认值500，范围：[0,1000]。创建mark点时所绘制的ROI膨胀量。 $150");
			msgs.push_back("StrucTure   $Int   $I_Spinbox     $2    $检测结构值              $默认值2，范围：[0,1000]。基于图像相减时的腐蚀膨胀大小。 $160");
			msgs.push_back("NumLevels   $Int   $I_Spinbox     $4    $金字塔层数              $默认值4，范围：[0,10]。基于模板创建匹配时的金字塔层数。 $170");

		}
		//----------------------------------------------基于模板图异常查找的缺陷检测算子(end)-------------------------------------//
	};


	//! 接口

	namespace alg {
		
		//---------------------------------------------加载纹理模型算子loadModel ----------------------------------//
		//名称：loadModel
		//功能： 模型初始化，加载模型ID，模型偏移量，检测区域，模板图像。
		//返回值： 0-正常
		_ALGLIBRARY_API int loadModel(const LoadModelInput& input, LoadModelOutput& output);
	}

}

#endif	// ZKHYPROJECTHUANAN_LOADMODEL_H
