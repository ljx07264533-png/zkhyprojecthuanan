#include "alglibrary/zkhyProjectHuaNan/PutImageTogether.h"

#define LOG alglib::core::LoggerManager::getInstance()

using namespace std;
using namespace cv;
using namespace HalconCpp;
using namespace alglib::core;
using namespace alglib::ops::zkhyProHN;

namespace alglib::ops::zkhyProHN {
    struct algErrMsg
    {
        bool bError = false;	// 是否有异常
        string strErrMsg = "";	// 错误提示信息
    };

    class zkhy_traces
    {

    public:
        static void StitchWithoutTracesL(Mat& dstimg, const Mat& patchImg, Mat& wMat, int serialNum, int overlapCol, int beginCol, int addedCol, int beginRow, int endRow);

        static void StitchWithoutTracesR(Mat& dstimg, const Mat& patchImg, Mat& wMat, int serialNum, int overlapCol, int endCol, int addedCol, int beginRow, int endRow);

        static void StitchWithoutTracesU(Mat& dstimg, const Mat& patchImg, Mat& wMat, int serialNum, int overlapRow, int beginRow, int addedRow, int beginCol, int endCol);

        static void StitchWithoutTracesD(Mat& dstimg, const Mat& patchImg, Mat& wMat, int serialNum, int overlapRow, int endRow, int addedRow, int beginCol, int endCol);

        static void StitchInterface_Single(Mat& dstImg, const Mat& patchImg, Mat& wMat, int directFlag, int serialNum, cv::Rect rect, int overlapNum);

        /**
         * @brief 获取拼接后图像的大小
         * @param singleImgSize 图像大小
         * @param rect 截取图像的ROI
         * @param imgCount 图像个数
         * @param directFlag 最终图像拼接方向 0-纵向，1-横向
         * @return 拼接后图像的大小
         */
        static cv::Size getImgSize(const cv::Size& singleImgSize, const cv::Rect& rect, const int& imgCount, const int& directFlag);

        /**
         * 拼接图像接口
         * @param dstImg 输出拼接后的图
         * @param vecImg 图像源
         * @param rect 截取的ROI
         * @param directFlag 方向
         * @param overlapNum 重叠行数或列数
         */
        static void StitchInterface(Mat& dstImg, const vector<Mat>& vecImg, const cv::Rect& rect, const int& directFlag, int overlapNum, algErrMsg& errMsg);
    };

    void zkhy_traces::StitchWithoutTracesL(Mat& dstimg, const Mat& patchImg, Mat& wMat, int serialNum, int overlapCol, int beginCol, int addedCol, int beginRow, int endRow)
    {
        int dstRows = dstimg.rows;
        int dstCols = dstimg.cols;
        int patchRows = patchImg.rows;
        int patchCols = patchImg.cols;
        if (serialNum == 0)
        {
            patchImg(Range(beginRow, endRow + 1), Range(beginCol, patchCols)).copyTo(dstimg(Range::all(), Range(dstCols - patchCols + beginCol, dstCols)));
        }
        else
        {
            int startColOld = dstCols - patchCols + beginCol - (serialNum - 1) * addedCol;
            patchImg(Range(beginRow, endRow + 1), Range(beginCol, beginCol + addedCol)).copyTo(dstimg(Range::all(), Range(startColOld - addedCol, startColOld)));
            if (overlapCol > 0)
            {
                Mat overlapImgNew = patchImg(Range(beginRow, endRow + 1), Range(beginCol + addedCol, beginCol + addedCol + overlapCol));
                Mat overlapImgOld = dstimg(Range::all(), Range(startColOld, startColOld + overlapCol));
                Mat overlapImgNewF, overlapImgOldF;
                overlapImgOld.convertTo(overlapImgOldF, CV_32F);
                overlapImgNew.convertTo(overlapImgNewF, CV_32F);
                Mat weightedOverlapImgF = wMat.mul(overlapImgOldF) + (1 - wMat).mul(overlapImgNewF);
                Mat weightedOverlapImg;
                weightedOverlapImgF.convertTo(weightedOverlapImg, CV_8U);
                weightedOverlapImg.copyTo(dstimg(Range::all(), Range(startColOld, startColOld + overlapCol)));
            }
        }
    }

    void zkhy_traces::StitchWithoutTracesR(Mat& dstimg, const Mat& patchImg, Mat& wMat, int serialNum, int overlapCol, int endCol, int addedCol, int beginRow, int endRow)
    {
        int dstRows = dstimg.rows;
        int dstCols = dstimg.cols;
        int patchRows = patchImg.rows;
        int patchCols = patchImg.cols;
        endCol += 1;
        if (serialNum == 0)
        {
            patchImg(Range(beginRow, endRow + 1), Range(0, endCol)).copyTo(dstimg(Range::all(), Range(0, endCol)));
        }
        else
        {
            int endColOld = endCol + (serialNum - 1) * addedCol;
            patchImg(Range(beginRow, endRow + 1), Range(endCol - addedCol, endCol)).copyTo(dstimg(Range::all(), Range(endColOld, endColOld + addedCol)));
            if (overlapCol > 0)
            {
                Mat overlapImgNew = patchImg(Range(beginRow, endRow + 1), Range(endCol - addedCol - overlapCol, endCol - addedCol));
                Mat overlapImgOld = dstimg(Range::all(), Range(endColOld - overlapCol, endColOld));
                Mat overlapImgNewF, overlapImgOldF;
                overlapImgOld.convertTo(overlapImgOldF, CV_32F);
                overlapImgNew.convertTo(overlapImgNewF, CV_32F);
                Mat weightedOverlapImgF = wMat.mul(overlapImgOldF) + (1 - wMat).mul(overlapImgNewF);
                Mat weightedOverlapImg;
                weightedOverlapImgF.convertTo(weightedOverlapImg, CV_8U);
                weightedOverlapImg.copyTo(dstimg(Range::all(), Range(endColOld - overlapCol, endColOld)));
            }
        }
    }

    void zkhy_traces::StitchWithoutTracesU(Mat& dstimg, const Mat& patchImg, Mat& wMat, int serialNum, int overlapRow, int beginRow, int addedRow, int beginCol, int endCol)
    {
        int dstRows = dstimg.rows;
        int dstCols = dstimg.cols;
        int patchRows = patchImg.rows;
        int patchCols = patchImg.cols;
        if (serialNum == 0)
        {
            patchImg(Range(beginRow, patchRows), Range(beginCol, endCol + 1)).copyTo(dstimg(Range(dstRows - patchRows + beginRow, dstRows), Range::all()));
        }
        else
        {
            int startRowOld = dstRows - patchRows + beginRow - (serialNum - 1) * addedRow;
            patchImg(Range(beginRow, beginRow + addedRow), Range(beginCol, endCol + 1)).copyTo(dstimg(Range(startRowOld - addedRow, startRowOld), Range::all()));
            if (overlapRow > 0)
            {
                Mat overlapImgNew = patchImg(Range(beginRow + addedRow, beginRow + addedRow + overlapRow), Range(beginCol, endCol + 1));
                Mat overlapImgOld = dstimg(Range(startRowOld, startRowOld + overlapRow), Range::all());
                Mat overlapImgNewF, overlapImgOldF;
                overlapImgOld.convertTo(overlapImgOldF, CV_32F);
                overlapImgNew.convertTo(overlapImgNewF, CV_32F);
                Mat weightedOverlapImgF = wMat.mul(overlapImgOldF) + (1 - wMat).mul(overlapImgNewF);
                Mat weightedOverlapImg;
                weightedOverlapImgF.convertTo(weightedOverlapImg, CV_8U);
                weightedOverlapImg.copyTo(dstimg(Range(startRowOld, startRowOld + overlapRow), Range::all()));
            }
        }
    }

    void zkhy_traces::StitchWithoutTracesD(Mat& dstimg, const Mat& patchImg, Mat& wMat, int serialNum, int overlapRow, int endRow, int addedRow, int beginCol, int endCol)
    {
        int dstRows = dstimg.rows;
        int dstCols = dstimg.cols;
        int patchRows = patchImg.rows;
        int patchCols = patchImg.cols;
        endRow += 1;
        if (serialNum == 0)
        {
            patchImg(Range(0, endRow), Range(beginCol, endCol + 1)).copyTo(dstimg(Range(0, endRow), Range::all()));
        }
        else
        {
            int endRowOld = endRow + (serialNum - 1) * addedRow;
            patchImg(Range(endRow - addedRow, endRow), Range(beginCol, endCol + 1)).copyTo(dstimg(Range(endRowOld, endRowOld + addedRow), Range::all()));
            if (overlapRow > 0)
            {
                Mat overlapImgNew = patchImg(Range(endRow - addedRow - overlapRow, endRow - addedRow), Range(beginCol, endCol + 1));
                Mat overlapImgOld = dstimg(Range(endRowOld - overlapRow, endRowOld), Range::all());
                Mat overlapImgNewF, overlapImgOldF;
                overlapImgOld.convertTo(overlapImgOldF, CV_32F);
                overlapImgNew.convertTo(overlapImgNewF, CV_32F);
                Mat weightedOverlapImgF = wMat.mul(overlapImgOldF) + (1 - wMat).mul(overlapImgNewF);
                Mat weightedOverlapImg;
                weightedOverlapImgF.convertTo(weightedOverlapImg, CV_8U);
                weightedOverlapImg.copyTo(dstimg(Range(endRowOld - overlapRow, endRowOld), Range::all()));
            }
        }
    }

    void zkhy_traces::StitchInterface_Single(Mat& dstImg, const Mat& patchImg, Mat& wMat, int directFlag, int serialNum, cv::Rect rect, int overlapNum)
    {
        int beginRow = rect.tl().y;
        int endRow = rect.br().y - 1;
        int beginCol = rect.tl().x;
        int endCol = rect.br().x - 1;
        int addedRow = endRow - beginRow + 1;
        int addedCol = endCol - beginCol + 1;
        switch (directFlag)
        {
        case 1:
            StitchWithoutTracesL(dstImg, patchImg, wMat, serialNum, overlapNum, beginCol, addedCol, beginRow, endRow);
            break;
        case 3:
            StitchWithoutTracesR(dstImg, patchImg, wMat, serialNum, overlapNum, endCol, addedCol, beginRow, endRow);
            break;
        case 0:
            StitchWithoutTracesU(dstImg, patchImg, wMat, serialNum, overlapNum, beginRow, addedRow, beginCol, endCol);
            break;
        case 2:
            StitchWithoutTracesD(dstImg, patchImg, wMat, serialNum, overlapNum, endRow, addedRow, beginCol, endCol);
            break;
        default:
            break;
        }
    }

    cv::Size zkhy_traces::getImgSize(const cv::Size& singleImgSize, const cv::Rect& rect, const int& imgCount, const int& directFlag)
    {
        Size sz;
        if (directFlag == 1 || directFlag == 3) //左右运动
        {
            sz.height = rect.height;
            sz.width = (imgCount - 1) * rect.width + singleImgSize.width;
        }
        else
        {
            sz.width = rect.width;
            sz.height = (imgCount - 1) * rect.height + singleImgSize.height;
        }
        return sz;
    }

    void zkhy_traces::StitchInterface(Mat& dstImg, const vector<Mat>& vecImg, const cv::Rect& rect, const int& directFlag, int overlapNum, algErrMsg& errMsg)
    {
        int imgCount = vecImg.size();
        if (imgCount == 0) return;
        int x = rect.br().x;
        if (0 != overlapNum) {
            switch (directFlag)
            {
            case 1:
                if (rect.br().x + overlapNum >= vecImg[0].cols) {
                    errMsg.bError = true;
                    // errMsg.strErrMsg = "盖板在图像中向右运动，roi右侧索引与设置的重叠列数之和应小于子图列数";
                    //string str = "异常信息：配置文件中startPos最大值与重叠列数overlapRows之和大于图像宽度.";
                    //errMsg.strErrMsg.append(str);
                    //str = "异常处理:设定重叠列数overlapRows应小于" + std::to_string(vecImg[0].cols - rect.br().x);
                    //errMsg.strErrMsg.append(str);
                    overlapNum = 0;
                }
                break;
            case 2:
                if (rect.tl().x < overlapNum) {
                    errMsg.bError = true;
                    // errMsg.strErrMsg = "盖板在图像中向左运动，roi左侧索引应不小于设置的重叠列数";
                    //string str = "异常信息：配置文件中startPos最小值小于重叠列数overlapRows.";
                    //errMsg.strErrMsg.append(str);
                    //str = "异常处理：设定重叠列数overlapRows应小于" + std::to_string(rect.tl().x);
                    //errMsg.strErrMsg.append(str);
                    //overlapNum = 0;
                }
                break;;
            case 3:
                if (rect.br().y + overlapNum >= vecImg[0].rows) {
                    errMsg.bError = true;
                    // errMsg.strErrMsg = "盖板在图像中向下运动，roi下侧索引与设置的重叠行数之和应小于子图行数";
                    //string str = "异常信息：配置文件中startPos最大值与重叠行数overlapRows之和大于图像高度";
                    //errMsg.strErrMsg.append(str);
                    //str = "异常处理：设定重叠列数overlapRows应小于" + std::to_string(vecImg[0].rows - rect.br().y);
                    //errMsg.strErrMsg.append(str);
                    //overlapNum = 0;
                }
                break;
            case 4:
                if (rect.tl().y < overlapNum) {
                    errMsg.bError = true;
                    // errMsg.strErrMsg = "盖板在图像中向上运动，roi上侧索引应不小于设置的重叠行数";
                    //string str = "异常信息：配置文件中startPos最小值小于重叠行数overlapRows";
                    //errMsg.strErrMsg.append(str);
                    //str = "异常处理：设定重叠列数overlapRows应小于" + std::to_string(rect.tl().y);
                    //errMsg.strErrMsg.append(str);
                    //overlapNum = 0;
                }
                break;
            default:
                break;
            }
        }

        Size sz = getImgSize(vecImg[0].size(), rect, imgCount, directFlag);
        dstImg = Mat::zeros(sz, CV_8U);
        Mat wMat;
        switch (directFlag)
        {
        case 1:	// 图像中盖板从左往右刷新
            wMat = Mat::zeros(Size(overlapNum, rect.height), CV_32F);
            for (int col = 0; col < overlapNum; col++) {
                float w = (col + 1) / float(overlapNum + 1);
                for (int row = 0; row < rect.height; row++)
                    wMat.at<float>(row, col) = w;
            }
            break;
        case 3:	// 图像中盖板从右往左刷新
            wMat = Mat::zeros(Size(overlapNum, rect.height), CV_32F);
            for (int col = 0; col < overlapNum; col++) {
                float w = 1 - (col + 1) / float(overlapNum + 1);
                for (int row = 0; row < rect.height; row++)
                    wMat.at<float>(row, col) = w;
            }
            break;
        case 0:	// 图像中盖板从上往下刷新
            wMat = Mat::zeros(Size(rect.width, overlapNum), CV_32F);
            for (int row = 0; row < overlapNum; row++)
            {
                float w = (row + 1) / float(overlapNum + 1);
                for (int col = 0; col < rect.width; col++)
                    wMat.at<float>(row, col) = w;
            }
            break;
        case 2:	// 图像中盖板从下往上刷新
            wMat = Mat::zeros(Size(rect.width, overlapNum), CV_32F);
            for (int row = 0; row < overlapNum; row++)
            {
                float w = 1 - (row + 1) / float(overlapNum + 1);
                for (int col = 0; col < rect.width; col++)
                    wMat.at<float>(row, col) = w;
            }
            break;
        default:
            break;
        }
        for (int i = 0; i < imgCount; i++)
            StitchInterface_Single(dstImg, vecImg[i], wMat, directFlag, i, rect, overlapNum);
    }

}

namespace alglib::ops::zkhyProHN::alg {
    int PutImageTogether(const PutImageTogetherInput& input, PutImageTogetherOutput& output)
    {
        std::vector<cv::Mat> smallImgs;
        cv::Mat& resultImg = output.img;

        smallImgs = input.smallImgs;
        if (smallImgs.size() < 1)
            return 1;

        // 解析参数
        AlgParaPutImageTogether* alg = (AlgParaPutImageTogether*)&input.algPara;
        //算法参数
        int mode = alg->i_params.block.mode;						    //拼图模式
        int rotate = alg->i_params.block.rotate;						//旋转角度
        int mirror = alg->i_params.block.mirror;						//镜像
        int ovcerlapNum = alg->i_params.block.ovcerlapNum;//拼接方向的重叠区域大小，建议根据实际设置
        int top_col = alg->i_params.block.roi_lefttop_cols;
        int top_row = alg->i_params.block.roi_lefttop_rows;
        int right_col = alg->i_params.block.roi_rightdown_cols;
        int right_row = alg->i_params.block.roi_rightdown_rows;
        algErrMsg errMsg;
        Rect img_rect = Rect(0, 0, smallImgs[0].cols, smallImgs[0].rows);
        Rect roi = Rect(Point2i(top_col, top_row), Point2i(right_col, right_row));//非重叠区域的左上右下坐标
        if (img_rect != (img_rect | roi))
            return 1;
        zkhy_traces zkhy_traces_;
        try {
            if (mode == 0) {//纵向拼接，从上往下
                if (ovcerlapNum == 0)//硬拼(原图数组拼接)
                    vconcat(smallImgs, resultImg);
                else//消除拼接缝（带有重叠区域的权重融合）
                {
                    zkhy_traces_.StitchInterface(resultImg, smallImgs, roi, mode, ovcerlapNum, errMsg);
                }
            }
            else if (mode == 1) {//横向拼接，从左往右
                if (ovcerlapNum == 0)
                    hconcat(smallImgs, resultImg);
                else
                {
                    zkhy_traces_.StitchInterface(resultImg, smallImgs, roi, mode, ovcerlapNum, errMsg);
                }
            }
            else if (mode == 2) {//纵向拼接，从下往上
                if (ovcerlapNum == 0)
                {
                    reverse(smallImgs.begin(), smallImgs.end());
                    hconcat(smallImgs, resultImg);
                }
                else
                {
                    zkhy_traces_.StitchInterface(resultImg, smallImgs, roi, mode, ovcerlapNum, errMsg);
                }
            }
            else if (mode == 3) {//横向拼接，从右往左
                if (ovcerlapNum == 0)
                {
                    reverse(smallImgs.begin(), smallImgs.end());
                    hconcat(smallImgs, resultImg);
                }
                else
                {
                    zkhy_traces_.StitchInterface(resultImg, smallImgs, roi, mode, ovcerlapNum, errMsg);
                }
            }

            if (rotate == 1) {
                cv::rotate(resultImg, resultImg, ROTATE_90_CLOCKWISE);
            }
            else if (rotate == 2) {
                cv::rotate(resultImg, resultImg, ROTATE_90_COUNTERCLOCKWISE);
            }

            if (mirror == 1) {
                cv::flip(resultImg, resultImg, false);
            }
            else if (mirror == 2) {
                cv::flip(resultImg, resultImg, true);
            }
        }
        catch (...)
        {
            return 2;
        }
        return 0;
    }

}
