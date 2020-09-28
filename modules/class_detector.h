#ifndef CLASS_DETECTOR_H_
#define CLASS_DETECTOR_H_
// 此类Detector只是个包装封装类而已，实际起作用的是YoloDectector类

#include <iostream>
#include <opencv2/opencv.hpp>
#include "API.h"

struct Result
{
	int		 id		= -1;
	float	 prob	= 0.f;
	cv::Rect rect;
};

typedef std::vector<Result> BatchResult;

enum class ModelType
{
	YOLOV2 = 0,
	YOLOV3,
	YOLOV2_TINY,
	YOLOV3_TINY,
	YOLOV4,
	YOLOV4_TINY,
	YOLOV5
};

enum class Precision // 注意小心重定义 https://blog.csdn.net/wb175208/article/details/78772773
{
	INT8 = 0,
	FP16,
	FP32
};

struct Config
{
	std::string file_model_cfg					= "configs/yolov3.cfg";

	std::string file_model_weights				= "configs/yolov3.weights";

	float detect_thresh							= 0.9;

	ModelType	net_type						= ModelType::YOLOV3;

	Precision	inference_precison				= Precision::FP32;
	
	int	gpu_id									= 0;

	std::string calibration_image_list_file_txt = "configs/calibration_images.txt";

};

class API Detector
{
public:
	explicit Detector();

	~Detector();

	void init(const Config &config);

	void detect(const std::vector<cv::Mat> &mat_image, std::vector<BatchResult> &vec_batch_result);

private:
	
	Detector(const Detector &);
	const Detector &operator =(const Detector &);
	class Impl;
	Impl *_impl;
};

#endif // !CLASS_QH_DETECTOR_H_