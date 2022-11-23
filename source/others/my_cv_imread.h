#ifndef MARKMAKER_MY_CV_IMREAD_H
#define MARKMAKER_MY_CV_IMREAD_H
#include<opencv2/opencv.hpp>
namespace my {
    //和opencv的一些格式转换
    static inline cv::Mat imread(const cv::String &filename, int flags = cv::IMREAD_COLOR) {
        cv::Mat rtv;
        rtv = cv::imread(filename, flags);
        if (rtv.channels() == 3) {
            cv::cvtColor(rtv, rtv, cv::COLOR_BGR2RGB);
        } else if (rtv.channels() == 4) {
            cv::cvtColor(rtv, rtv, cv::COLOR_BGRA2RGBA);
        }
        return rtv;
    }

    static inline bool imwrite(const cv::String &filename, cv::Mat img, const std::vector<int> &params = std::vector<int>()) {
        if (img.channels() == 3) {
            cv::cvtColor(img, img, cv::COLOR_RGB2BGR);
        } else if (img.channels() == 4) {
            cv::cvtColor(img, img, cv::COLOR_RGBA2BGRA);
        }
        return cv::imwrite(filename, img, params);
    }
}
#endif //MARKMAKER_MY_CV_IMREAD_H
