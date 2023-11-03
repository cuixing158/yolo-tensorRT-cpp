**本项目工程属于tensorRT yolov3/v4/v5 C++量化版本！**
## OverView
本工程含有2个项目，一个用于dll_detector产生dll或者so库文件，另一个为测试库文件的项目,yolov3/v4需要事先准备cfg,weights文件， [yolov5](https://github.com/ultralytics/yolov5 )需要事先准备yolov5s.yaml和yolov5s.pt文件。C++代码组织非常好，可以学习参考，另外关于tensorRT的量化过程也不错。此库非常适合windows10,ubuntu,嵌入式jetson环境部署。<br>

## TensorRT 量化流程
 量化工作原理为：先判断是否有校订table文件存在，有的话直接读取，没有就对data/目录下的图像进行calibrate生成table，先调用函数readCalibrationCache,然后getBatch,最后writeCalibrationCache，getBatch()在校验过程中
 调用多次，其他函数调用一次。所有模型文件都转换为cfg,weights,解析是使用tensorRT C++ 自定义的API。
 
 
## 更新记录
2020.9.27记录：tensorRT量化进度，审阅代码到calibrator流程，是定义Int8EntropyCalibrator 继承tensorRT库下的 public nvinfer1::IInt8EntropyCalibrator，重写calibrator类.明天需要完成自己的球员网球检测器在量化后的表现<br>
2020.9.28记录：量化了网球球员检测模型，速度10ms一帧，320×320，速度并未提高？校准表是中间生成？<br>
2020.10.9 记录：弄清楚量化接口的调用过程，以便于部署其他模型的推理量化。在PC上测评tensorRT性能结果见[此项目](https://github.com/cuixing158/yolov3-yolov4) <br>
2020.10.10记录：项目中使用engine推理图像大小是在cfg文件中定义的width,height进行的，而非实际输入图像大小。暂时终止此项目，改用onnxruntime进行推理，因为onnxruntime已经[集成了tensorRT推理引擎](https://github.com/microsoft/onnxruntime/blob/master/docs/execution_providers/TensorRT-ExecutionProvider.md)或者[onnx-tensorrt](https://github.com/onnx/onnx-tensorrt)<br>
或移步到我的[face_jetson_pytorch](https://github.com/cuixing158/jetson_faceTrack_pytorch)<br>
2020.11.11记录：量化部分仍旧回到本库C++ TensorRT模式！环境定向为cuda10.2+cudnn7.4.1+vs2019

![](./configs/yolo-trt.png)
- [x] yolov5s , yolov5m , yolov5l , yolov5x [tutorial](yolov5_tutorial.md)
- [x] yolov4 , yolov4-tiny
- [x] yolov3 , yolov3-tiny

## Features

- [x] inequal net width and height
- [x] batch inference
- [x] support FP32,FP16,INT8
- [ ] daynamic input size

## WRAPPER

Prepare the pretrained __.weights__ and __.cfg__ model. 

```c++
Detector detector;
Config config;

std::vector<BatchResult> res;
detector.detect(vec_image, res)
```


### windows10
- cuda环境dependency：tensorRT,cuda,cudnn版本要对应，TensorRT6.0.1.5+cuda10.1+cudnn7.6.4.38 或者 TensorRT 7.1.3.4 +cuda 11.0 + cudnn 8.0 
或者 TensorRT7.0+cuda10.2+cudnn7.6.4.38 

- 软件环境dependency : opencv4 , vs2015或其他版本
- build:
  
    open MSVC _sln/sln.sln_ file 
    - dll project : the trt yolo detector dll
    - demo project : test of the dll
![图片](configs/config.jpg)<br>

### ubuntu & L4T (jetson)

The project generate the __libdetector.so__ lib, and the sample code.
**_If you want to use the libdetector.so lib in your own project,this [cmake file](https://github.com/enazoe/yolo-tensorrt/blob/master/scripts/CMakeLists.txt) perhaps could help you ._**


```bash
git clone https://github.com/enazoe/yolo-tensorrt.git
cd yolo-tensorrt/
mkdir build
cd build/
cmake ..
make
./yolo-trt
```
## API

```c++
struct Config
{
	std::string file_model_cfg = "configs/yolov4.cfg";

	std::string file_model_weights = "configs/yolov4.weights";

	float detect_thresh = 0.9;

	ModelType net_type = YOLOV4;

	Precision inference_precison = INT8;
	
	int gpu_id = 0;

	std::string calibration_image_list_file_txt = "configs/calibration_images.txt";

	int n_max_batch = 4;	
};

class API Detector
{
public:
	explicit Detector();
	~Detector();

	void init(const Config &config);

	void detect(const std::vector<cv::Mat> &mat_image,std::vector<BatchResult> &vec_batch_result);

private:
	Detector(const Detector &);
	const Detector &operator =(const Detector &);
	class Impl;
	Impl *_impl;
};
```

## 关于量化的一些原理知识
对于任意一个实数，量化为整数类型，省略bias，计算公式如下：<br>
RealWorldValue = StoredInteger ✕ 2^(−FractionLength) <br>
在matlab中使用定点计算可以表述上式，例如pi，可以用以下代码量化其值：<br>
```matlab
ntBP = numerictype(1,8,4);% 定义一种有符号8位并小数位占4位的符号位对象类型
x_BP = fi(pi,true,8) % 有符号8位定点数,小数位长度由软件自动推算
pi_cal = double(x_BP.storedInteger)*2^(-x_BP.FractionLength)+x_BP.Bias % 验证量化公式

yBP1 = quantize(x_BP,ntBP) % 指定x_BP为ntBP类型
```
output:<br>
```text
x_BP = 
   3.156250000000000

          DataTypeMode: Fixed-point: binary point scaling
            Signedness: Signed
            WordLength: 8
        FractionLength: 5
pi_cal =
   3.156250000000000
yBP1 = 
   3.125000000000000

          DataTypeMode: Fixed-point: binary point scaling
            Signedness: Signed
            WordLength: 8
        FractionLength: 4
```



## REFERENCE

- https://github.com/enazoe/yolo-tensorrt
- [tensorRTX库重点](https://github.com/wang-xinyu/tensorrtx/tree/master/yolov4 )
- https://github.com/mj8ac/trt-yolo-app_win64
- https://github.com/NVIDIA-AI-IOT/deepstream_reference_apps
- [matlab量化背景1](https://www.mathworks.com/help/fixedpoint/ug/data-types-and-scaling-in-digital-hardware.html#bu22l3v-1 )
- [What Is int8 Quantization and Why Is It Popular for Deep Neural Networks?](https://www.mathworks.com/company/newsletters/articles/what-is-int8-quantization-and-why-is-it-popular-for-deep-neural-networks.html)




