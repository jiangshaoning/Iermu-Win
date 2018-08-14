#ifndef _EpMakeTemplete_h_
#define _EpMakeTemplete_h_

#ifdef _WIN32
#   ifdef STITCHER_EXPORT
#       define STITCHER_LIB __declspec(dllexport)
#   elif defined(STITCHER_IMPORT)
#       define STITCHER_LIB __declspec(dllimport)
#   else
#       define STITCHER_LIB
#   endif
#else
#   define STITCHER_LIB
#endif

#include "EpTemplateHead.h"
#include "QSystemDetection.h"

// !!调用流程：在调用任何接口之前，先初始化模板类型initTemplateType

class EpMakeTempletePrivate;
class EpMotionDetection;
class STITCHER_LIB EpMakeTemplete
{
public:
    EpMakeTemplete();

    ~EpMakeTemplete();

    // 获取模板
    bool getTemplateByType(const std::string &input,
                           std::string &output,
                           const TemplateType &type);

    // 加载模板信息
    bool loadTemplateByType(const std::string &input,
                            const TemplateType &type);

#if !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    // 获取加密的模板信息
    bool getEncryptedTemplate(const std::string &ip, std::string &strEncryptedTemplate) const;

    // 获取解密后的模板信息
    bool getDecryptedTemplate(const std::string &ip, std::string &strDecryptedTemplate) const;

	//从未加密旧模板转换成新格式模板
	bool translateTemplate(std::string &inTemplateData, std::string &outTemplateData);

	bool GetCenterFromTemplete(const std::string templateData, int * centerLX, int * centerLY, int * centerRX, int * centerRY);

    bool getDecryptedTemplateByURL(const std::string &url, std::string &strDecryptedTemplate);

	int IsHor(const std::string templateData);
#endif

    bool MakeTemplete(TempleteParameter *output,
                      const std::string &input,
                      const TemplateType &type);
#if !defined(Q_OS_IOS)
    // 保存模板带圆心位置
    bool outPutFile(const std::string &filePath,
                    int seamPositionL,
                    int seamPositionR,
                    int seamWidth,
                    const VIDEO_RES &resolution);

    // 保存模板
    bool WriteTemplateToFile(const std::string &filePath);

    std::string EncryptedTemplate(std::string templateData);

	//从模板获取相机类型
	int GetCameraTypeFromTemplete(const std::string &templateData);
#endif

    // 设置模板信息
    void setEpTemplateCtx(const EpTemplateCtx &context);

    // 获取模板信息
    const EpTemplateCtx &getEpTemplateCtx() const;

    // 获取模板信息
    bool getDecryptContexString(std::string &strDecryptContex) const;

    // 设置模板分辨率
    void setVideoResolution(const VIDEO_RES &resolution);

    // 获取模板分辨率
    const VIDEO_RES getVideoResolution() const;

    // 判断模板完整性type为：STRINGEN 判读加密字符串； STRING 判断非加密字符串
    bool checkTemplateIntegrity(const std::string &strTemplate,
                                const TemplateType &type) const;
	//双鱼眼上的坐标转换为全景图上的坐标
	bool transformImgCoordInv(double & x_dest,
		double & y_dest,
		double x_src,
		double y_src,
		TempleteParameter *tmplate);

	//全景图上的坐标转换为双鱼眼图上的坐标
	bool transformImgCoord(double & x_dest,
		double & y_dest,
		double x_src,
		double y_src,
		TempleteParameter *tmplate);

	bool getDetectionArea();

	bool MDComputeAreaMap(std::vector<EpRect2d> &_regionOnWide, float lon, float lat, unsigned char *&outMap, int & outWidth, int & outHeight);

private:

	void MDUpDataTemplate(std::string _sTemplate);

private:
    EpMakeTempletePrivate *	pEpMakeTempletePrivate;

	EpMotionDetection *		m_pMotionDetection;
};

#endif
