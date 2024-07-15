// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"
#include "Widgets/SWeakWidget.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#if WITH_OPENCV
#include "PreOpenCVHeaders.h"
#include<filesystem>
#include "opencv2/opencv.hpp"
#include "PostOpenCVHeaders.h"
#endif

class NousImage
{
private:
    cv::Mat m_image;
    std::filesystem::path m_file_path;

public:
    explicit NousImage(const std::filesystem::path& file_path)
    {
        m_file_path = file_path;
        load_image();
    }
    explicit NousImage(const cv::Mat& image) { m_image = image; }

    explicit NousImage(const FString& f_file_path) {
        std::string msg = TCHAR_TO_UTF8(*f_file_path);
        m_file_path=std::filesystem::u8path(msg.begin(), msg.end());
        load_image();
    }
    NousImage(const NousImage& other) = default;
    NousImage& load_image()
    {
        m_image = cv::imread(m_file_path.generic_string(), cv::IMREAD_COLOR);
        return *this;
    }

public:
    static void save(const cv::Mat& image, const std::string& file_path) { imwrite(file_path, image); }
    void save(const std::string& file_path) const { imwrite(file_path, m_image); }


public:
    bool is_valid() const { return m_image.data!=nullptr; }
    int32 width() const { return m_image.cols; }
    int32 height() const { return m_image.rows; }
    int32 channels() const { return m_image.channels(); }
    unsigned char* data() const { return m_image.data; }

public:


public:// convert
    NousImage& hsv_to_bgr()
    {
        cv::cvtColor(m_image, m_image, cv::COLOR_HSV2BGR);
        return *this;
    }
    NousImage& bgr_to_hsv()
    {
        cv::cvtColor(m_image, m_image, cv::COLOR_BGR2HSV);
        return *this;
    }
    NousImage& bgr_to_gray()
    {
        cv::cvtColor(m_image, m_image, cv::COLOR_BGR2GRAY);
        return *this;
    }
    NousImage& rgb_to_gray()
    {
        cvtColor(m_image, m_image, cv::COLOR_RGB2GRAY);
        return *this;
    }
    NousImage& gray_to_rgb()
    {
        cvtColor(m_image, m_image, cv::COLOR_GRAY2RGB);
        return *this;
    }

private:// read image by stb_img


private:
    struct CompareVec3b
    {
        bool operator()(const cv::Vec3b& a, const cv::Vec3b& b) { return a[0] < b[0] || (a[0] == b[0] && (a[1] < b[1] || (a[1] == b[1] && a[2] < b[2]))); };
    };
    /**
     * @brief get most frequent pixel value in the kernel_size×kernel_size
     */
    /*
    cv::Vec3b get_frequency_pixel(const cv::Mat& src_image, int32 x, int32 y, int32 kernel_size)
    {

    }    
    */

    int32 get_color_block(const cv::Mat& image, cv::Mat& color_block, int32 x, int32 y)
    {
        // 当前像素没有颜色分类
        constexpr int32 dx[8] = { 1, -1, 0, 0, -1, -1, 1, 1 };
        constexpr int32 dy[8] = { 0, 0, 1, -1, -1, 1, -1, 1 };
        // constexpr int32 dx[4] = { 1, -1, 0, 0 };
        // constexpr int32 dy[4] = { 0, 0, 1, -1 };

        auto current_color = image.at<cv::Vec3b>(x, y);
        for (auto i = 0; i < 8; ++i)
        {
            const auto nx = x + dx[i];
            const auto ny = y + dy[i];
            if (nx < 0 || nx >= image.rows || ny < 0 || ny >= image.cols)
            {
                continue;
            }

            if (!color_block.at<int32>(nx, ny))// 如果相邻的像素没有颜色分类
            {
                continue;
            }

            auto neighbor_color = image.at<cv::Vec3b>(nx, ny);
            if ((int32)current_color[0] == (int32)neighbor_color[0] && (int32)current_color[1] == (int32)neighbor_color[1] &&
                (int32)current_color[2] == (int32)neighbor_color[2])
            {
                return color_block.at<int32>(nx, ny);
            }
        }
        return 0;
    }

public:
    NousImage& resize(uint32 x, uint32 y)
    {
        cv::resize(m_image, m_image, cv::Size(x, y));
        return *this;
    }
    NousImage& extract_edge(double threshold1, double threshold2)
    {
        cv::Canny(m_image, m_image, threshold1, threshold2);
        return *this;
    }
    NousImage& gauss_blur(uint32 iter_num, uint32 x_kernel_num, uint32 y_kernel_num, uint32 sigma)
    {
        // params check
        iter_num = (iter_num < 1 || iter_num > 20) ? 3 : iter_num;
        x_kernel_num = (x_kernel_num < 1 || x_kernel_num > 21) ? 5 : x_kernel_num;
        y_kernel_num = (y_kernel_num < 1 || y_kernel_num > 21) ? 5 : y_kernel_num;
        sigma = (sigma < 1 || sigma > 20) ? 3 : sigma;
        x_kernel_num = (x_kernel_num % 2 == 0) ? x_kernel_num + 1 : x_kernel_num;
        y_kernel_num = (y_kernel_num % 2 == 0) ? y_kernel_num + 1 : y_kernel_num;
        for (size_t i = 0; i < iter_num; i++)
        {
            GaussianBlur(m_image, m_image, cv::Size(x_kernel_num, y_kernel_num), sigma);
        }
        return *this;
    }
    /**
     * @brief  Finds centers of clusters and groups input samples around the clusters.
     * @param K Number of clusters to split the set by
     * @param max_iterations the maximum number
     * @param epsilon The accuracy is specified as criteria.epsilon. As soon as each of the cluster centers moves by less than criteria.epsilon on
     * some iteration, the algorithm stops.
     */
    NousImage& kmeans(uint32 k, uint32 max_iterations, float epsilon)
    {
        // params check
        k = (k < 1 || k > 30) ? 3 : k;
        max_iterations = (max_iterations < 1 || max_iterations > 10) ? 5 : max_iterations;
        epsilon = (epsilon < 0.1 || epsilon > 10) ? 1.0 : epsilon;

        // 转换图像数据格式以适应kmeans
        cv::Mat data;
        m_image.convertTo(data, CV_32F);
        data = data.reshape(1, m_image.rows * m_image.cols);

        cv::Mat labels, centers;
        cv::kmeans(data, k, labels, cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, max_iterations, epsilon), 5, cv::KMEANS_PP_CENTERS, centers);

        // 处理聚类中心，将其转换为正确的形状
        centers = centers.reshape(m_image.channels(), k);

        // 创建聚类结果图像
        cv::Mat output(m_image.size(), m_image.type());

        auto tp = m_image.type();
        for (int32 y = 0; y < m_image.rows; y++)
        {
            for (int32 x = 0; x < m_image.cols; x++)
            {
                const int32 cluster_idx = labels.at<int32>(y * m_image.cols + x);
                auto&& center = centers.at<cv::Vec3f>(cluster_idx);
                auto pixel = { static_cast<uchar>(center[0]), static_cast<uchar>(center[1]), static_cast<uchar>(center[2]) };
                output.at<cv::Vec3b>(y, x) = pixel;
            }
        }
        m_image = output;
        return *this;
    }
    /**
     * @brief make edge aliasing
     */
    NousImage& anti_aliasing_filter(uint32 kernel_size)
    {

        return *this;
    }
    /**
     * @brief frequency_filter
     */
    NousImage& frequency_filter(uint32 kernel_size)
    {
        return *this;
    }
    /**
     * @brief color_adsorption to neighbor pixel
     */
    NousImage& color_adsorption(uint32 threshold)
    {
        return *this;
    }

    NousImage& color_mask(const FString & f_file_path)
    {
        std::string msg = TCHAR_TO_UTF8(*f_file_path);
        std::filesystem::path mask_file_path = std::filesystem::u8path(msg.begin(), msg.end());
        cv::Mat mask_image = cv::imread(mask_file_path.generic_string(), cv::IMREAD_COLOR);
        cv::resize(mask_image, mask_image, cv::Size(width(), height()));
        cv::bitwise_and(mask_image,m_image,m_image);
        return *this;
    }
    NousImage& color_mask(NousImage& mask_image) {
        if (height() != mask_image.height() || width() != mask_image.width()) {
            mask_image.resize(width(),height());
        }
        cv::bitwise_and(mask_image.m_image,m_image,m_image);
        return *this;
    }
};


// Sets default values
AInteractiveActor::AInteractiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
}

// Called when the game starts or when spawned
void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	
	
	widget = SNew(SScatterWidget);
	win = SNew(SWindow)
		.Title(FText::FromString(TEXT("Scatter window")))
		.ClientSize(FVector2D(200, 600))
		.ScreenPosition(FVector2D(100, 100))
		[
			widget.ToSharedRef()
		];
	FSlateApplication::Get().AddWindow(win.ToSharedRef());
	win->BringToFront(true);


	widget.Get()->divide_image_button.BindLambda([this](FString path) {
		DivideArea(path);
		});

	widget.Get()->set_texture.BindLambda([this](UTexture2D* t) {
		GEngine->AddOnScreenDebugMessage(1, 5, FColor::Black, TEXT("e"));
		int a = 0;
		texture = t;
		});

	widget.Get()->sub_area_combobox.BindLambda([this](FString subarea) {
		for (auto i : infos)
		{
			if (i.subarea == subarea)
				current_info = i;
		}
		});

	widget.Get()->fill_image_button.BindLambda([this](FString path) {
		current_info.fill_image_path = path;
		FillArea(current_info);
		});

	widget.Get()->distribute_spinbox.BindLambda([this](FString category, double value) {
		*current_info.textrue_para.Find(category) = value;
		int a = 1;
		});

	widget.Get()->transform_spinbox.BindLambda([this](FString category, double value) {
		*current_info.mesh_para.Find(category) = value;
		int a = 1;
		});
	
	widget.Get()->calculate_texture_button.BindLambda([this]() {
		CalculateTexture(current_info);

		


		FString MaterialPath = TEXT("Material'/Game/DivideImage/0001.0001'");
		material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
		});
}

void AInteractiveActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	FSlateApplication::Get().RequestDestroyWindow(win.ToSharedRef());
	win.Reset();
}

// Called every frame
void AInteractiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

void AInteractiveActor::DivideArea(FString path)
{
	//输入图片路径，解析图片后，传给ComputeShader
	
	//ComputeShader计算后返回一张分区图
	//再将这张分区图划分为若干区域图（暂定2张

	//SubAreaInfo info1;
	//info1.subarea = FString("SubArea 1");
	//infos.Add(info1);
	//SubAreaInfo info2;
	//info1.subarea = FString("SubArea 2");
	//infos.Add(info2);

}

void AInteractiveActor::FillArea(SubAreaInfo info)
{
	FString path = info.fill_image_path;
	//通过path，读本地图给info.fillimage
	//用fillimage和子区域图求交，得到采样点图samples_texture
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path))
	{
		UE_LOG(LogTemp,Error,TEXT("the file is not exist"));
	}
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *path))
	{
		UE_LOG(LogTemp, Error, TEXT("can't init the rawfileData"));
	}
    path = "C:\\Users\\Administrator\\Desktop\\code\\2.png";
   // NousImage image(std::filesystem::u8path(path.begin(), path.end()));
    //image.save("C:\\Users\\Administrator\\Desktop\\code\\res.png");
    NousImage image(path);
    image.color_mask("D:\\Project\\Summer-of-Code\\SummerCode\\Content\\FillImage\\patches1_clamp_mask.png");
    image.save("C:\\Users\\Administrator\\Desktop\\code\\res.png");
}

void AInteractiveActor::CalculateTexture(SubAreaInfo info)
{
	//通过info中各种参数计算最终纹理
}

UTexture2D* AInteractiveActor::LoadTexture2D(const FString path)
{
	UTexture2D* Texture = nullptr;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path))
	{
		return nullptr;
	}
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *path))
	{
		return nullptr;
	}
	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(path);
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> UncompressedRGBA;
		if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);
			if (Texture != nullptr)
			{
				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}
	return Texture;
}

TArray<FString> AInteractiveActor::GetFolderFiles(FString path)
{
	TArray<FString> files;
	FPaths::NormalizeDirectoryName(path);
	IFileManager& FileManager = IFileManager::Get();
	FString FinalPath = path / TEXT("*");
	FileManager.FindFiles(files, *FinalPath, true, true);
	return files;
}

TArray<UTexture2D*> AInteractiveActor::GetAllImageFromFiles(FString Paths)
{
	TArray<FString> ImgPaths = GetFolderFiles(Paths);
	TArray<UTexture2D*> Texture2DArr;
	for (auto path : ImgPaths)
	{
		UTexture2D* Texture2D = LoadTexture2D(Paths + "/" + path);
		Texture2DArr.Add(Texture2D);
	}
	return Texture2DArr;
}

TSharedPtr<class IImageWrapper> AInteractiveActor::GetImageWrapperByExtention(const FString Path)
{
	IImageWrapperModule& module = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (Path.EndsWith(".png"))
	{
		return module.CreateImageWrapper(EImageFormat::PNG);
	}
	if (Path.EndsWith(".jpg"))
	{
		return module.CreateImageWrapper(EImageFormat::JPEG);
	}
	return nullptr;
}
