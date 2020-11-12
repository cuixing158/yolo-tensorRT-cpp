#include <memory>
#include <thread>
#include "class_timer.hpp"
#include "class_detector.h"
#include "../CmFile/CmFile.h"

int main()
{
	Config config_v3;
	config_v3.net_type = ModelType::YOLOV3;
	config_v3.file_model_cfg = "E:/allModel/yolov3.cfg";
	config_v3.file_model_weights = "E:/allModel/yolov3.weights";
	config_v3.calibration_image_list_file_txt = "../configs/calibration_images.txt";
	config_v3.inference_precison = Precision::FP32;

	Config config_v3_tiny;
	config_v3_tiny.net_type = ModelType::YOLOV3_TINY;
	config_v3_tiny.detect_thresh = 0.7;
	config_v3_tiny.file_model_cfg = "../configs/yolov3-tiny.cfg";
	config_v3_tiny.file_model_weights = "../configs/yolov3-tiny.weights";
	config_v3_tiny.calibration_image_list_file_txt = "../configs/calibration_images.txt";
	config_v3_tiny.inference_precison = Precision::FP32;

	Config config_v4;
	config_v4.net_type = ModelType::YOLOV4;
	config_v4.file_model_cfg = "../configs/yolov4.cfg";
	config_v4.file_model_weights = "E:/allModel/yolov4.weights";
	config_v4.inference_precison = Precision::FP32;

	Config config_v4_tiny;
	config_v4_tiny.net_type = ModelType::YOLOV4_TINY;
	config_v4_tiny.detect_thresh = 0.5;
	config_v4_tiny.file_model_cfg = "../configs/yolov4-tiny.cfg";
	config_v4_tiny.file_model_weights = "E:/allModel/yolov4-tiny.weights";
	config_v4_tiny.calibration_image_list_file_txt = "../configs/calibration_images.txt";
	config_v4_tiny.inference_precison = Precision::FP32;

	Config config_v5;
	config_v5.net_type = ModelType::YOLOV5;
	config_v5.detect_thresh = 0.5;
	config_v5.file_model_cfg = "../configs/yolov5-3.0/yolov5s.cfg";
	config_v5.file_model_weights = "../configs/yolov5-3.0/yolov5s.weights";
	config_v5.inference_precison = Precision::FP32;

	Config myconfig_v4;
	myconfig_v4.net_type = ModelType::YOLOV4_TINY;
	myconfig_v4.detect_thresh = 0.5;
	myconfig_v4.file_model_cfg = "D:/VS_2015_work/ActivityRecognitionCom/ActivityRecognitionCom/model/person_ball_detect.cfg";
	myconfig_v4.file_model_weights = "D:/VS_2015_work/ActivityRecognitionCom/ActivityRecognitionCom/model/person_ball_detect.weights";
	myconfig_v4.calibration_image_list_file_txt = "../configs/calibration_images.txt";
	myconfig_v4.inference_precison = Precision::INT8;

	std::unique_ptr<Detector> detector(new Detector());
	detector->init(myconfig_v4);
	bool isuseOurmodel = true;
	cv::Mat image0 = cv::imread("../configs/dog.jpg", cv::IMREAD_COLOR);
	cv::Mat image1 = cv::imread("../configs/person.jpg", cv::IMREAD_COLOR);
	std::vector<BatchResult> batch_res;
	std::vector<std::string> vecImgs;
	CmFile::GetImageFromFolderAndSub("E:/sportsBallPlayerBase/test", vecImgs,".*g"); // "E:/coco2014/train2014/train2014/train2014"  "E:/sportsBallPlayerBase/no_detect_person"

	Timer timer;
	for (size_t i = 0 ;i<vecImgs.size();i++)
	{
		//prepare batch data
		std::vector<cv::Mat> batch_img;
		cv::Mat temp0 = cv::imread(vecImgs[i]);
		cv::Mat temp1 = image1.clone();
		batch_img.push_back(temp0);
		//batch_img.push_back(image1);

		//detect
		timer.reset();
		detector->detect(batch_img, batch_res, isuseOurmodel);
		timer.out("detect");

		//disp
		for (int i=0;i<batch_img.size();++i)
		{
			for (const auto &r : batch_res[i])
			{
				std::cout <<"batch "<<i<< " id:" << r.id << " prob:" << r.prob << " rect:" << r.rect << std::endl;
				cv::rectangle(batch_img[i], r.rect, cv::Scalar(255, 0, 0), 2);
				std::stringstream stream;
				stream << std::fixed << std::setprecision(2) << "id:" << r.id << "  score:" << r.prob;
				cv::putText(batch_img[i], stream.str(), cv::Point(r.rect.x, r.rect.y - 5), 0, 0.5, cv::Scalar(0, 0, 255), 2);
			}
			cv::imshow("image"+std::to_string(i), batch_img[i]);
		}
		cv::waitKey(0);
	}
}
