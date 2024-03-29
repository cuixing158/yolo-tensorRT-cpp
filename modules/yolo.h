/**
MIT License

Copyright (c) 2018 NVIDIA CORPORATION. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*
*/
// yolo为基类，下面的yolov2/v3/v4/v5都是继承此类
#ifndef _YOLO_H_
#define _YOLO_H_

#include <stdint.h>
#include <string>
#include <vector>

// tensorRT 库和CUDA库
#include "NvInfer.h"
#include "NvInferPlugin.h"
#include "NvInferRuntimeCommon.h"
#include "cuda_runtime_api.h"

// opencv 库
#include "opencv2/opencv.hpp"

// 项目使用的
#include "calibrator.h"
#include "plugin_factory.h"
#include "trt_utils.h"
#include "class_timer.hpp"
#include "detect.h"
//#include "logging.h"

/**
 * Holds all the file paths required to build a network.
 */
struct NetworkInfo
{
    std::string networkType;
    std::string configFilePath;
    std::string wtsFilePath;
    std::string labelsFilePath;
    std::string precision;
    std::string deviceType;
    std::string calibrationTablePath;
    std::string enginePath;
    std::string inputBlobName;
	std::string data_path;
};

/**
 * Holds information about runtime inference params.
 */
struct InferParams
{
    bool printPerfInfo;
    bool printPredictionInfo;
    std::string calibImages; // txt文本路径文件，里面每行保存一副校验图像路径
    std::string calibImagesPath; // txt文本路径文件的前缀目录
    float probThresh;
    float nmsThresh;
};

/**
 * Holds information about an output tensor of the yolo network.
 */
struct TensorInfo
{
    std::string blobName;// 输出到yolo最后一层的名字，比如第一个yolo层名字设定为yolo_0
    uint32_t stride{0}; // stride_h等同，是下采样率,典型的如32
    uint32_t stride_h{0};
    uint32_t stride_w{0};
    uint32_t gridSize{0};// grid_h等同，是特征图的大小
	uint32_t grid_h{ 0 }; // 当前yolo层特征图的height
	uint32_t grid_w{ 0 }; // 当前yolo层特征图的width
    uint32_t numClasses{0};// 总的类别数量，比如yolov3/v4共80类别
    uint32_t numBBoxes{0}; // yolov3/v4层的mask数量，yolov2的anchors数量，大小为N，见parseConfigBlocks（）
    uint64_t volume{0}; // 当前特征图的输出bs*na*h*w*(5+nc)的数量，其中bs大小来自于cfg文件中的batch
    std::vector<uint32_t> masks; // masks,依次存储cfg中的masks
    std::vector<float> anchors;// anchors，依次存储cfg文件中的anchors
    int bindingIndex{-1};
    float* hostBuffer{nullptr};// 输出的tensor数据,多维数组,该类初始化会调用parseConfigBlocks（），然后此变量初始化
};


class Yolo
{
public:
    std::string getNetworkType() const { return m_NetworkType; }
    float getNMSThresh() const { return m_NMSThresh; }
    std::string getClassName(const int& label) const { return m_ClassNames.at(label); }
    int getClassId(const int& label) const { return m_ClassIds.at(label); }
    uint32_t getInputH() const { return m_InputH; }
    uint32_t getInputW() const { return m_InputW; }
    uint32_t getNumClasses() const { return static_cast<uint32_t>(m_ClassNames.size()); }
    bool isPrintPredictions() const { return m_PrintPredictions; }
    bool isPrintPerfInfo() const { return m_PrintPerfInfo; }
    void doInference(const unsigned char* input, const uint32_t batchSize);
    std::vector<BBoxInfo> decodeDetections(const int& imageIdx,
											const int& imageH,
                                           const int& imageW);

    virtual ~Yolo();

protected:
    Yolo( const NetworkInfo& networkInfo, const InferParams& inferParams); // 具体到初始化某个yolo类型时候，比如初始化一个yolov4对象时候，子类开始调用父类方法
    std::string m_EnginePath;
    const std::string m_NetworkType;// class_detector.h中定义的YOLOV2，YOLOV2_TINY，...
    const std::string m_ConfigFilePath;
    const std::string m_WtsFilePath;
    const std::string m_LabelsFilePath;
    const std::string m_Precision; // 传入的 "kINT8","kHALF","kFLOAT"中的一个，字符串类型，_vec_precision 
    const std::string m_DeviceType;
    const std::string m_CalibImages; // 实际传进来的为校订图像路径文本文件
    const std::string m_CalibImagesFilePath;// 为传经来的前缀，在calibrator.cpp中Int8EntropyCalibrator（）用到，目前为"" 
    std::string m_CalibTableFilePath; //  class_yolo_detector.hpp中传入
    const std::string m_InputBlobName;
    std::vector<TensorInfo> m_OutputTensors;// 一个所有输出yolo层的tensor的集合
    std::vector<std::map<std::string, std::string>> m_configBlocks;// 解析cfg文件中的key,value，如果遇到层,就是type，value
    uint32_t m_InputH; // cfg文件中定义的height,此类初始化就加载.cfg文件解析，parseConfigBlocks（）子函数里面
    uint32_t m_InputW; //  cfg文件中定义的width
    uint32_t m_InputC;
    uint64_t m_InputSize;// 输入一副图像的像素个数
	uint32_t _n_classes = 0;
	float _f_depth_multiple = 0;
	float _f_width_multiple = 0;
    const float m_ProbThresh;
    const float m_NMSThresh;
    std::vector<std::string> m_ClassNames;
    // Class ids for coco benchmarking
    const std::vector<int> m_ClassIds{
        1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 13, 14, 15, 16, 17, 18, 19, 20, 21,
        22, 23, 24, 25, 27, 28, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44,
        46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65,
        67, 70, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 84, 85, 86, 87, 88, 89, 90};
    const bool m_PrintPerfInfo;
    const bool m_PrintPredictions;
    // TRT specific members
	//Logger glogger;
    uint32_t m_BatchSize = 1; // 默认为1，实际后面来源于cfg文件中的输入 [net] 的batch值，见parseConfigBlocks（）函数

	// 以下为来自tensorRT官方的库定义
    nvinfer1::INetworkDefinition* m_Network;
    nvinfer1::IBuilder* m_Builder ;// 根据网络定义构建一个推理引擎
    nvinfer1::IHostMemory* m_ModelStream;
    nvinfer1::ICudaEngine* m_Engine;
    nvinfer1::IExecutionContext* m_Context;
    std::vector<void*> m_DeviceBuffers;
    int m_InputBindingIndex;
    cudaStream_t m_CudaStream;
    PluginFactory* m_PluginFactory;
    std::unique_ptr<YoloTinyMaxpoolPaddingFormula> m_TinyMaxpoolPaddingFormula;

	// 需重写
    virtual std::vector<BBoxInfo> decodeTensor(const int imageIdx, const int imageH,
                                               const int imageW, const TensorInfo& tensor) // 子类需要实现
        = 0;

    inline void addBBoxProposal(const float bx, const float by, const float bw, const float bh,
                                const uint32_t stride, const float scalingFactor, const float xOffset,
                                const float yOffset, const int maxIndex, const float maxProb,
		const uint32_t 	image_w, const uint32_t image_h,
                                std::vector<BBoxInfo>& binfo)
    {
        BBoxInfo bbi;
        bbi.box = convertBBoxNetRes(bx, by, bw, bh, stride, m_InputW, m_InputH);
        if ((bbi.box.x1 > bbi.box.x2) || (bbi.box.y1 > bbi.box.y2))
        {
            return;
        }
        convertBBoxImgRes(scalingFactor, m_InputW,m_InputH,image_w,image_h, bbi.box);
        bbi.label = maxIndex;
        bbi.prob = maxProb;
        bbi.classId = getClassId(maxIndex);
        binfo.push_back(bbi);
    }

	BBox convert_bbox_res(const float& bx, const float& by, const float& bw, const float& bh,
		const uint32_t& stride_h_, const uint32_t& stride_w_, const uint32_t& netW, const uint32_t& netH)
	{
		BBox b;
		// Restore coordinates to network input resolution
		float x = bx * stride_w_;
		float y = by * stride_h_;

		b.x1 = x - bw / 2;
		b.x2 = x + bw / 2;

		b.y1 = y - bh / 2;
		b.y2 = y + bh / 2;

		b.x1 = clamp(b.x1, 0, netW);
		b.x2 = clamp(b.x2, 0, netW);
		b.y1 = clamp(b.y1, 0, netH);
		b.y2 = clamp(b.y2, 0, netH);

		return b;
	}
	inline void add_bbox_proposal(const float bx, const float by, const float bw, const float bh,
		const uint32_t stride_h_, const uint32_t stride_w_, const int maxIndex, const float maxProb,
		const uint32_t 	image_w, const uint32_t image_h,
		std::vector<BBoxInfo>& binfo)
	{
		BBoxInfo bbi;
		bbi.box = convert_bbox_res(bx, by, bw, bh, stride_h_, stride_w_, m_InputW, m_InputH);
		if ((bbi.box.x1 > bbi.box.x2) || (bbi.box.y1 > bbi.box.y2))
		{
			return;
		}
		convertBBoxImgRes(0, m_InputW, m_InputH, image_w, image_h, bbi.box);
		bbi.label = maxIndex;
		bbi.prob = maxProb;
		bbi.classId = getClassId(maxIndex);
		binfo.push_back(bbi);
	};
private:
    Logger m_Logger;
    void createYOLOEngine(const nvinfer1::DataType dataType = nvinfer1::DataType::kFLOAT,
                          Int8EntropyCalibrator* calibrator = nullptr);
	void create_engine_yolov5(const nvinfer1::DataType dataType = nvinfer1::DataType::kFLOAT,
		Int8EntropyCalibrator* calibrator = nullptr);
    std::vector<std::map<std::string, std::string>> parseConfigFile(const std::string cfgFilePath);
    void parseConfigBlocks();
	void parse_cfg_blocks_v5(const  std::vector<std::map<std::string, std::string>> &vec_block_);
    void allocateBuffers();
    bool verifyYoloEngine();
    void destroyNetworkUtils(std::vector<nvinfer1::Weights>& trtWeights);
    void writePlanFileToDisk();

private:
	Timer _timer;
	void load_weights_v5(const std::string s_weights_path_, std::map<std::string, std::vector<float>> &vec_wts_);

	int _n_yolo_ind = 0; // 输出为region或者yolo层的序号，从0开始计数
};

#endif // _YOLO_H_
