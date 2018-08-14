/**
* @file		接口文件
* @author	eapil
*/

#ifndef _EPRENDERER_H_
#define _EPRENDERER_H_

#ifdef _WIN32
#   ifdef EPRENDER_EXPORT
#       define EPRENDER_LIB __declspec(dllexport)
#   elif defined(EPRENDER_IMPORT)
#       define EPRENDER_LIB __declspec(dllimport)
#   else
#       define EPRENDER_LIB
#   endif
#else
#   define EPRENDER_LIB
#endif

#include "GLRenderHead.h"

#if defined(Q_OS_ANDROID)
#include "EapilTemplateParser/EpTemplateHead.h"
#else
#include "EpTemplateHead.h"
#endif

#include <string>

class GLRenderControl;

class GLRenderPBO;

#if !(defined(Q_OS_IOS) || defined(Q_OS_ANDROID))
class EPRENDER_LIB EpRenderer
#else
class  EpRenderer
#endif
{
public:
	/**
	* @brief	构造函数
	*/
	EpRenderer();

	/**
	* @brief	析构函数
	*/
	~EpRenderer();

	/**
	* @brief	初始化opengl相关资源
	*/
	void InitOpenGL();

	/**
	* @brief	释放opengl相关资源
	*/
	void UnInitOpenGL();

	/**
	* @brief 渲染
	* @return true：成功 false：失败
	*/
	bool Render();

	/**
	* @brief 左键摁下消息
	* @param x 鼠标横坐标
	* @param y 鼠标纵坐标
	*/
	void OnLButtonDown(int x, int y);

	/**
	* @brief 左键抬起消息
	*/
	void OnLButtonUp();

	/**
	* @brief 鼠标移动消息
	* @param x 鼠标横坐标
	* @param y 鼠标纵坐标
	*/
	void OnMouseMove(int x, int y);

	/**
	* @brief 鼠标滚轮消息
	* @param delta 滚轮滑动值
	*/
	void OnMouseWheel(short delta);

	/**
	* @brief 传输YUV数据
	* @param yuvData 数据指针
	* @param width 数据宽
	* @param height 数据高
	*/
	void TranslateVideoData(unsigned char * yuvData, int width, int height);

	/**
	* @brief 传入欧拉角
	* @param yaw 绕x轴旋转角度（单位：度）
	* @param pitch 绕y轴旋转角度（单位：度）
	* @param roll 绕z轴旋转角度（单位：度）
	*/
	void TranslateEulerRotate(float yaw, float pitch, float roll);

	/**
	* @brief 传输RGBA数据
	* @param RGBA 数据指针
	* @param width 数据宽
	* @param height 数据高
	*/
	void TranslateImageData(unsigned char * rgbaData, int width, int height);

	/**
	* @brief 设置窗口宽高
	* @param width		窗口宽
	* @param height		窗口高
	*/
	void SetWindow(int width, int height);

	/**
	* @brief 加载模板数据
	* @param path 模板数据
	* @param type 模板类型
	*/
	void LoadTemplate(std::string path, TemplateType type = CAMERAEN);

	/**
	* @brief 加载模板数据
	* @param path 模板数据
	* @param type 模板类型
	*/
	void LoadTemplate(char * path, TemplateType type = CAMERAEN);

	/**
	* @brief 设置渲染帧率
	* @param rate 帧率
	*/
	void SetTargetFrameRate(float rate);

	/**
	* @brief 设置播放模式
	* @param type 播放模式
	*/
	void SetPlayerType(PlayerType type);

	/**
	* @brief	设置VR模式下旋转的绝对角度
	* @param rotateX 绕x轴旋转绝对角度(单位：度)
	* @param rotateY 绕x轴旋转绝对角度(单位：度)
	* @param rotateZ 绕x轴旋转绝对角度(单位：度)
	*/
	void SetRotateVR(float rotateX, float rotateY, float rotateZ);

	/**
	* @brief 设置fbo画布id
	* @param id fboid
	*/
	void TransOpenGLFboID(int id);

	/**
	* @brief 判断渲染器是否支持此图片解析
	* @param fileName 图片文件名
	* @return true：支持 false：不支持
	*/
	bool WhetherSupportImage(std::string name);

	/**
	* @brief 开启录屏
	*/
	void EnableRecordScreen();

	/**
	* @brief 关闭录屏
	*/
	void DisableRecordScreen();

	/**
	* @brief 设置录屏数据回调
	* @param callBack 回调函数指针
	* @param arg 数据参数
	*/
	void SetRecordScreenCallBack(RecordScreenCallBack callBack, void *arg);

	/**
	* @brief 截屏
	* @param fileName 存储路径名
	* @param wideScreen true：宽屏截取，false：截取当前屏幕
	* @return 0：成功 1：失败
	*/
	int  SaveScreenImage(std::string fileName, bool wideScreen = true);

	/**
	* @brief 将当前屏幕图像存人内存
	* @return 0：成功 1：失败
	*/
	int  SaveScreenImageToMemory();

	/**
	* @brief 将存入内存的图片存入指定硬盘位置
	* @param fileName 存储路径名
	* @return 0：成功 1：失败
	*/
	int  SaveScreenImageToFile(std::string fileName);

	/**
	* @brief	进入标定模式
	* @return true：成功 false：失败
	*/
	bool EnterStandardizationState();

	/**
	* @brief	离开标定模式
	* @return true：成功 false：失败
	*/
	bool LeaveStandardizationState();

	/**
	* @brief	取消标定模式
	* @return true：成功 false：失败
	*/
	bool CancelStandardizationState();

	/**
	* @brief	进入自动旋转模式
	* @param speed 旋转速度（°/s）
	*/
	void EnterAutoRotationState(float speed = 0.3);

	/**
	* @brief	退出自动旋转模式
	*/
	void LeaveAutoRotationState();

	/**
	* @brief	设置标定时的旋转速度
	* @param	rate 旋转速度
	*/
	void SetStandardizationRate(float rate);

	/**
	* @brief 设置数据渲染通道类型
	* @param colorType 通道类型 0：RGB,1：BGR
	*/
	void SetColorType(ColorType colorType);

	/**
	* @brief	设置GUI渲染回调函数
	* @param	callBack 函数指针
	*/
	void SetGUIRenderCallBack(CommonCallBack callBack);

	/**
	* @brief 重新打开视频时重置播放状态（清除视频数据）
	*/
	void ResetPlayState();
	
	/**
	* @brief 判断当前是否是球模式渲染
	* @return true：球模式 false：非球模式
	*/
	bool IsRenderBall();

	/**
	* @brief 获取当前标定矩阵和球旋转缩放状态数据
	* @return 返回矩阵和球旋转缩放状态
	*/
	PlayerMatrixState GetPlayerMatrixState(PlayerType type, int num);

	/**
	* @brief 设置当前标定矩阵和球旋转缩放状态数据
	* @param state 标定矩阵和旋转缩放状态数据
	*/
	void SetPlayerMatrixState(PlayerMatrixState * state, PlayerType type, int num);

	/**
	* @brief 设置图像是否可以缩放
	* @param scale true：开启缩放, false：关闭缩放
	*/
    void SetImageScalable(bool scale = false);

	/**
	* @brief 设置图像缩放值（用于宽屏模式）
	* @param scale 缩放量
	*/
	void SetImageScaleForWS(float scale);

	/**
	* @brief 宽屏模式下设置纵向拖动方式
	* @param state 0 旋转拖动  1 平动 2 不能动
	*/
    void SetWideScreenYCanMove(int state);

	/**
	* @brief 清除模板
	*/
	void ClearTemplete();

	/**
	* @brief 获取NV12数据指针
	* @param data 数据指针
	* @param width 数据宽
	* @param height 数据高
	* @return true：成功 false：失败
	*/
	bool GetNV12Data(unsigned char *&data, int &width, int &height);

	/**
	* @brief 传入IJK纹理
	* @param width 纹理宽
	* @param height 纹理高
	*/
	void IJKTranslateTexture(int width, int height);

	/**
	* @brief 设置颜色转换矩阵
	* @param mat 矩阵
	*/
	void IJKSetUm3ColorConversion(void * mat);

	/**
	* @brief 设置图像转换类型（为了适配ijk）
	* @param type 具体类型参照shader
	*/
	void IJKSetTextureType(int type);

	/**
	* @brief 设置陀螺仪输入是否开启
	* @param state true：开启 false：关闭
	*/
	void SetGyroInputState(bool state);

	/**
	* @brief 获取播放模式
	* @return 播放模式
	*/
	PlayerType GetPlayerType();

	/**
	* @brief 设置球缩放范围和初始缩放大小
	* @param minDisToEye 最小缩放值
	* @param maxDisToEye 最大缩放值
	* @param initDis	 当前初始缩放值
	*/
	void SetBallPosRange(float minDisToEye, float maxDisToEye, float initDis);

	/**
	* @brief 设置指定模式缩放范围和初始缩放大小
	* @param type 播放模式
	* @param minDisToEye 最小缩放值
	* @param maxDisToEye 最大缩放值
	* @param initDis	 当前初始缩放值
	*/
	void SetBallPosRangeType(PlayerType type, float minDisToEye, float maxDisToEye, float initDis);

	/**
	* @brief 获取模视矩阵和投影矩阵
	* @param modelViewMatrix 模视矩阵
	* @param projectMatrix 投影矩阵
	*/
	void GetCurrentMVPMatrix(float * modelViewMatrix, float * projectMatrix);
	
	/**
	* @brief 设置标定图标是否显示
	* @param show true：显示 false：不显示
	*/
	void SetStandardIcon(bool show);

	/**
	* @brief 设置截屏类型
	* @param type 0：宽屏 1：八面体
	*/
	void SetSaveScreenType(SaveScreenType type);

	/**
	* @brief 设置适配类型
	* @param type 适配类型
	*/
	void SetAdaptationType(AdaptationType type);

	/**
	* @brief 重置渲染状态（重置渲染矩阵等）
	*/
	void ResetRendererState();

	/**
	* @brief 设置是否能截屏
	* @param canSaveImage true：能截屏 false：不能截屏
	*/
    void SetCanSaveImage(bool canSaveImage);
    
	/**
	* @brief 获取SDK版本号
	* @return 版本号字符串
	*/
	std::string GetSDKVersion();

	/**
	* @brief 设置当前是否处于标定状态
	* @param open true：处于标定状态 false：处于非标定状态
	*/
    void SetCalibrateMode(bool open);
    
	/**
	* @brief 设置是否开启输出当前屏幕图像为nv12格式
	* @param open true：开启 false：关闭
	*/
    void SetNV12Output(bool open);

	/**
	* @brief 设置过期验证key
	* @param key key字符串
	*/
    void SetExpireKey(char * key);

#if !(defined(Q_OS_IOS) || defined(Q_OS_ANDROID))
	/**
	* @brief 设置渲染结束回调指针
	* @param state 回调函数指针
	*/
	void SetRenderStateCallBack(RenderStateCallBackBase * state);
#endif
	/**
	* @brief 设置是否90度旋转屏幕
	* @param mode 0：不旋转屏幕,1：旋转屏幕
	*/
	void SetRotateScreenMode(PlayerType type, int mode);

	/**
	* @brief 设置NV12纹理输出宽高
	* @param width 纹理宽
	* @param height 纹理高
	*/
	void SetNV12TextureWidth(int width, int height);
	
	/**
	* @brief 获取相机类型
	* @return 相机类型
	*/
	int  GetVideoCameraType();

	/**
	* @brief 设置截屏结束回调
	* @param callback 回调指针
	* @param arg 用户参数
	*/
	void SetCallBackForSaveScreen(EapilCallBack callback, void * arg);

	/**
	* @brief 设置宽屏模式下的横向纵向偏移
	* @param lon 横向偏移
	* @param lat 纵向偏移
	*/
	void SetWideScreenLonLat(float lon, float lat);

	/**
	* @brief 加密处理
	* @param templateData 需要加密的字符串
	* @return 返回加密后的字符串
	*/
	std::string EncryptedTemplate(std::string templateData);

	/**
	* @brief 设置是否支持mediaplayer
	* @param support true：支持，false：不支持
	*/
	void SetMediaPlayer(bool support);

	/**
	* @brief 设置初始相机的安装角度
	* @param yaw 绕y轴角度
	* @param pitch 绕x轴角度
	* @param roll 绕z轴角度
	*/
	void InitCameraAttitude(float yaw, float pitch, float roll);

	/**
	* @brief 设置是否显示logo
	* @param visable true：显示 false：不显示
	*/
	void SetLogoVisable(bool visable);

	/**
	* @brief 设置VR模式下旋转的偏移量
	* @param rotateX 绕x轴旋转的偏移量(单位：度)
	* @param rotateY 绕y轴旋转绝对角度(单位：度)
	* @param rotateZ 绕z轴旋转绝对角度(单位：度)
	*/
	void SetRotateVROffset(float rotateX, float rotateY, float rotateZ);

	/**
	* @brief 设置是否显示字幕
	* @param show true：显示 false：不显示
	*/
	void SetShowSubtitleState(bool show);

	/**
	* @brief 设置字幕位置信息(左上角为坐标原点)
	* @param dstX 字幕显示位置横坐标
	* @param dstY 字幕显示位置纵坐标
	* @param size 字幕大小
	* @param srcBX 字幕在源图上的起始横坐标（像素坐标）
	* @param srcBY 字幕在源图上的起始纵坐标（像素坐标）
	* @param srcEX 字幕在源图上的截止横坐标（像素坐标）
	* @param srcEY 字幕在源图上的截止纵坐标（像素坐标）
	*/
	void SetSubtitlePosSize(float dstX, float dstY, float size, float srcBX = -1.0, float srcBY = -1.0, float srcEX = -1.0, float srcEY = -1.0);
	/**
	* @brief 进入沉浸模式
	* @param canDrag 是否能拖动
	*/
	void EnterImmerseMode(bool canDrag);

	/**
	* @brief 退出沉浸模式
	*/
	void LeaveImmerseMode();

	/**
	* @brief 设置旋转矩阵（只有开启沉浸模式此接口才有效）
	* @param rotateMatrix 旋转矩阵
	*/
	void SetImmerseRotateMatrix(float * rotateMatrix);

	/**
	* @brief 设置四元数（只有开启沉浸模式此接口才有效）
	* @param rotateMatrix 旋转四元数
	*/
	void SetImmerseRotateQuat(float * rotateQuat);

	/**
	* @brief 设置分屏是否联动
	* @param state true：联动 false：不联动
	*/
	void SetSplitScreenLinkage(PlayerType type, bool state);

	/**
	* @brief 设置球是否可以缩放
	* @param value true：可缩放 false：不可缩放
	*/
	void SetCanZoom(PlayerType type, bool value);

	/**
	* @brief 设置VR模式是否可以拖动
	* @param value true：可拖动,false：不可拖动
	*/
	void SetVRCanMove(bool value);

	/**
	* @brief 重新计算模板
	*/
	void RecalculationTemplate();

	/**
	* @brief 双鱼眼图上的坐标转换到全景图上的坐标（左上角为坐标原点）
	* @param x 双鱼眼图上的横坐标（0-360）
	* @param y 双鱼眼图上的纵坐标（-90-90）
	* @param outPutX 全景图上的横坐标（0-360）
	* @param outPutY 全景图上的纵坐标（-90-90）
	* @return true：成功，false：失败
	*/
	bool DoubleBallToPanorama(double x, double y, double & outPutX, double & outPutY);

	/**
	* @brief 获取对应的分屏的范围和中心坐标
	* @param centerRange 0 1存储中心坐标 2 3 存储范围
	* @param num 分屏号
	*/
	bool GetCenterAndRange(double * centerRange, int & num);
	
	/**
	* @brief 设置对应分屏的中心坐标
	* @param num 分屏号
	* @param lon 中心对应的横向坐标（0-360）
	* @param lat 中心对应的纵向坐标（-90-90）
	*/
	void SetBallCenterLonLat(int num, float lon, float lat);

	/**
	* @brief 设置分屏播放时候的布局类型（目前主要针对两分屏）
	* @param type 播放模式
	* @param layout 分屏类型 0：左右分，1：上下分
	*/
	void SetShowLayoutType(PlayerType type, int layout = 0);

	/**
	* @brief 设置分割线宽度和颜色
	* @param type 播放模式
	* @param width 宽度
	* @param color 颜色
	*/
	void SetSegmentingLine(PlayerType type, float width, float r, float g, float b);

	/**
	* @brief 是否开启缩放惯性
	* @param type 播放模式
	* @param enable true：开启，false：关闭
	*/
	void SetScaleInertiaEnable(PlayerType type, bool enable);

	/**
	* @brief 设置视场角
	* @param fov 纵向视场角（度）
	* @param type 播放模式
	* @return true：成功，false：失败
	*/
	bool SetRenderFov(PlayerType type, float fov);

	/**
	* @brief 设置成功渲染第一帧回调
	* @param callback 回调函数
	*/
	void SetRenderFirstFrameSuccessCallBack(CommonCallBack callback);

	/**
	* @brief 设置用户logo文件数据
	* @param rgbaData 文件数据
	* @param width 数据宽
	* @param height 数据高
	*/
	void SetUserLogoFileData(unsigned char * rgbaData, int width, int height);

	/**
	* @brief 是否显示用户logo 默认不显示
	* @param visiable true 可见， false 不可见
	*/
	void SetUserLogoVisiable(bool visiable);

	/**
	* @brief 设置logo显示位置和缩放值
	* @param centerSize 显示位置和缩放值
	*/
	void SetUserLogoCenterSize(float centerX, float centerY, float size);

	/**
	* @brief 设置输入图像是否被翻转
	* @param state true 被翻转， false没有翻转
	*/
	void SetImageTurnedOverState(bool state);

	/**
	* @brief 缩放球到目标位置
	* @param pos 距离圆心的位置距离
	*/
	void ScaleTheBall(float pos);

	/**
	* @brief 设置宽屏模式下滑动角度
	* @param lon 横向角度
	* @param lat 纵向角度
	*/
	void SetWideScreenMDLonLat(float lon, float lat);

	/**
	* @brief 获取宽屏模式下滑动角度
	* @param lon 横向角度
	* @param lat 纵向角度
	*/
	void GetWideScreenMDLonLat(float & lon, float & lat);

	/**
	* @brief 根据框位置坐标计算出map图
	* @param _regionOnWide 框位置坐标（在归一化的图上的坐标， 左上角为坐标原点）
	* @param lon 横向旋转角度
	* @param lat 纵向旋转角度
	* @param outMap map图
	* @param outWidth map图宽
	* @param outHeight map图高
	* return true 成功 false 失败
	*/
	bool MDComputeAreaMap(std::vector<EpRect2d> &_regionOnWide, float lon, float lat, unsigned char *&outMap, int & outWidth, int & outHeight);

	/***********************************分割线***********************************************/
	/**
	* @brief 更新检测框数据
	* @param detectResult 检测框数据指针
	* @param num 检测框个数
	*/
	void UpdateDynamicDetection(DynamicDetection * detectResult, int num);

	/**
	* @brief 设置是否显示动态侦测框
	* @param visible true：显示，false：不显示
	*/
	void SetDynamicDetectionVisible(bool visible);

	/**
	* @brief 设置检测框颜色
	* @param r 颜色值r通道(0-1)
	* @param g 颜色值g通道(0-1)
	* @param b 颜色值b通道(0-1)
	*/
	void SetDynamicDetectionColor(float r, float g, float b);

	/**
	* @brief 设置检测框宽度
	* @param width 检测框渲染宽度
	*/
	void SetDynamicDetectionWidth(float width);

	/**
	* @brief 初始化全景拼接
	*/
	static void PanoMakerInit();

	/**
	* @brief 设置输出纹理大小
	* @param width 输出纹理宽
	* @param height 输出纹理高
	*/
	static void PanoMakerSetOutputPicture(int width, int height);


	/**
	* @brief 设置球缩放范围和初始缩放大小
	* @param minDisToEye 最小缩放值
	* @param maxDisToEye 最大缩放值
	* @param initDis	 当前初始缩放值
	*/
	static void PanoMakerSetBallPosRange(float minDisToEye, float maxDisToEye, float initDis);

	/**
	* @brief 设置球形播放器旋转角度和视场角
	* @param lon 横向旋转角度(单位：度)
	* @param lat 纵向旋转角度(单位：度)
	* @param fov 视场角(单位：度)
	*/
	static void PanoMakerSetRotateFov(float lon, float lat, float fov);

	/**
	* @brief 设置播放模式
	* @param type 播放模式
	*/
	static void PanoMakerSetCurrentPlayerType(PlayerType type);

	/**
	* @brief 开启渲染线程
	*/
	static void PanoMakerStratThread();
	 
	/**
	* @brief 获取当前输出的纹理数据
	* @param rgbaData 输出的纹理数据
	* @param width 纹理宽
	* @param height 纹理高
	* @return 0：成功获取数据，1：无数据输入，2：正在处理
	*/
	static int  PanoMakerGetCurrentTypePicture(unsigned char * * rgbaData, int * width, int * height);
	 
	/**
	* @brief 设置数据处理完的回调
	* @param callback 数据处理完回调指针
	*/
	static void PanoMakerSetSaveScreenCallBack(SaveScreenCallBack callback);
	 
	/**
	* @brief 结束渲染线程
	*/
	static bool PanoMakerQuitThread();
	 
	/**
	* @brief 传输YUV数据
	* @param yuvData 数据指针
	* @param width 数据宽
	* @param height 数据高
	*/
	static void PanoMakerTransYUVData(unsigned char * yuvData, int width, int height);
	 
	/**
	* @brief 传输RGBA数据
	* @param RGBA 数据指针
	* @param width 数据宽
	* @param height 数据高
	*/
	static void PanoMakerTransRGBAData(unsigned char * rgbaData, int width, int height);
	 
	/**
	* @brief 加载模板
	* @param path 模板路径
	* @param type 模板类型
	*/
	static void PanoMakerLoadTemplate(unsigned char* name, int type);
	 
	/**
	* @brief 渲染一帧数据
	* @param yuvData YUV数据
	* @param width 数据宽
	* @param height 数据高
	*/
	static void PanoMakerRenderYUVData(unsigned char * yuvData, int width, int height);
	 
	/**
	* @brief 渲染一帧数据
	* @param rgbaData RGBA数据
	* @param width 数据宽
	* @param height 数据高
	*/
	static void PanoMakerRenderRGBAData(unsigned char * rgbaData, int width, int height);
	 
	/**
	* @brief 初始化OpenGL相关资源
	*/
	static void PanoMakerInitOpenGL();
	 
	/**
	* @brief 释放OpenGL相关资源
	*/
	static void PanoMakerUninitOpenGL();
	 
	/**
	* @brief 传入渲染器所用的FBO ID
	* @param id FBO ID
	*/
	static void PanoMakerTransOpenGLFboID(int id);
	 
	/**
	* @brief 设置窗口宽高
	* @param width 窗口宽
	* @param height 窗口高
	*/
	static void PanoMakerSetWindow(int width, int height);
	 
	/**
	* @brief 设置是否能截屏
	* @param canSaveImage true：能截屏 false：不能截屏
	*/
	static void PanoMakerSetCanSaveImage(bool canSaveImage);
	  
	/**
	* @brief 设置渲染环绕规则
	* @param canSaveImage 0：GL_CCW 1：GL_CW
	*/
	static void PanoMakerSetRenderFace(int face);

	/**
	* @brief 获取转换后的欧拉角
	* @param x 绕x轴旋转角度
	* @param y 绕y轴旋转角度
	* @param z 绕z轴旋转角度
	* @param w 保留值
	* @param nx 转换后绕x轴旋转角度
	* @param ny 转换后绕y轴旋转角度
	* @param nz 转换后绕z轴旋转角度
	* @param nw 暂无定义
	*/
	void GetNewEular(float x, float y, float z, float w, float *nx, float *ny, float *nz,float * nw);

private:
	/**
	* @brief 渲染器管理类
	*/
    GLRenderControl *	m_pRender;

	/**
	* @brief PBO用于图像拼接
	*/
	static GLRenderPBO *m_pPBORenderer;
};


#endif  // _EPRENDERER_H_
