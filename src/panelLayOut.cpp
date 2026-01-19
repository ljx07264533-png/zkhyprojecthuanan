#include "alglibrary/zkhyProjectHuaNan/panelLayOut.h"
#include "alglibrary/alglibMisc/Mat2HObject.h"
#include "alglibrary/alglibMisc/HObject2Mat.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;
using namespace alglib::ops::misc;
using namespace alglib::ops::misc::alg;

namespace alglib::ops::zkhyProHN {
	//通过得到的信息对panel生成区域并将区域和对应ID进行输出。
	int realPanelLayOut(const LayOutInput& input, LayOutOutput& output)
	{
		AlgParaMuraRegion* alg = (AlgParaMuraRegion*)&input.algPara;
		int galsTrans = 0;
		Mat	src = input.img;
		if (src.rows == 0 || src.cols == 0)
			return 1;
		HObject Hsrc = Mat2HObject(src);

		GlassLayout LayOut = input.layOut;//软件给的数据
		vector<PanelLayouts>   layoutList = LayOut.layoutList;//所有panel数据
		if (layoutList.size() == 0)
			return 2;

		vector<PanelRegion> OutRegion; //输出数据定义
		vector<vector<Point2f>> DisPanel;
		//拟合产品边缘算子得到的左上点和右下点。以及图像长宽
		double Row = input.recpoint[0].x;
		double Col = input.recpoint[0].y;
		double Row1 = input.recpoint[1].x;
		double Col1 = input.recpoint[1].y;
		double Row2 = input.recpoint[2].x;
		double Col2 = input.recpoint[2].y;
		double Row3 = input.recpoint[3].x;
		double Col3 = input.recpoint[3].y;
		double  with1 = Row2 - Row;
		double  hight1 = Col2 - Col;
		//设置gals的长宽
		double  with = LayOut.gPts[2].x - LayOut.gPts[0].x;
		double  hight = LayOut.gPts[2].y - LayOut.gPts[0].y;

		double withScal = 0;
		double hightScal = 0;

		withScal = with / with1;
		hightScal = hight / hight1;

		HObject ProductRegion, HProductImg;
		//GenRegionPolygon(&ProductRegion, [Row, Row1, Row2, Row3, Row], [Col, Col1, Col2, Col3, Col]);
		GenRegionPolygon(&ProductRegion, ((((HTuple(Col * hightScal).Append(Col1 * hightScal)).Append(Col2 * hightScal)).Append(Col3 * hightScal)).Append(Col * hightScal)),
			((((HTuple(Row * withScal).Append(Row1 * withScal)).Append(Row2 * withScal)).Append(Row3 * withScal)).Append(Row * withScal)));
		FillUp(ProductRegion, &ProductRegion);
		//GenRectangle1(&ProductRegion, Col, Row, Col1, Row2);//获取当前glas的区域。
		ReduceDomain(Hsrc, ProductRegion, &HProductImg);
		CropDomain(HProductImg, &HProductImg);
		Mat ProductImg = HObject2Mat(HProductImg);
		for (int i = 0; i < layoutList.size(); i++)
		{
			PanelRegion detectregion;  //检测区域-每个panel的区域
			HObject panelregion;//软件传入的panle区域
			PanelLayouts singlePanel;
			singlePanel = layoutList[i]; //单个panle数据
			vector<Point2f> Panlesize = singlePanel.pPts;//panel的角点
			Panlesize[4].x = input.recpoint[0].x;
			Panlesize[4].y = input.recpoint[0].y;
			detectregion.ID = singlePanel.id;	//当前panle的ID	
			float  contract = singlePanel.contract;

			for (int j = 0; j < 4; j++)
			{
				if (j == 0) {
					Panlesize[j].x = (Panlesize[j].x) * withScal + (contract / 2);
					Panlesize[j].y = Panlesize[j].y * hightScal + (contract / 2);
				}

				if (j == 1) {
					Panlesize[j].x = (Panlesize[j].x) * withScal + (contract / 2);
					Panlesize[j].y = Panlesize[j].y * hightScal - (contract / 2);
				}
				if (j == 2) {
					Panlesize[j].x = (Panlesize[j].x) * withScal - (contract / 2);
					Panlesize[j].y = Panlesize[j].y * hightScal - (contract / 2);
				}
				if (j == 3) {
					Panlesize[j].x = (Panlesize[j].x) * withScal - (contract / 2);
					Panlesize[j].y = Panlesize[j].y * hightScal + (contract / 2);
				}
			}
			double left = Panlesize[0].x;//左上角点X
			double right = Panlesize[0].y;
			double left1 = Panlesize[2].x;
			double right1 = Panlesize[2].y;
			GenRectangle1(&panelregion, right, left, right1, left1);//获取当前panel的区域。	
			ErosionRectangle1(panelregion, &panelregion, contract, contract);
			//detectregion.rPts[4] = input.recpoint[0];//左上点传给每一个panel保证显示在原图中。
			detectregion.rPts = Panlesize;
			detectregion.region = panelregion;
			OutRegion.push_back(detectregion);
			DisPanel.push_back(Panlesize);
		}
		output.pRegion = OutRegion;
		output.himg = HProductImg;
		output.img = ProductImg;
		output.contours = DisPanel;
		return 0;
	}

}

namespace alglib::ops::zkhyProHN::alg {
	int panelLayOut(const LayOutInput& input, LayOutOutput& output)
	{
		try
		{
			return realPanelLayOut(input, output);
		}
		catch (...)
		{
			return 3; //算法内部异常
		}
	}
}
