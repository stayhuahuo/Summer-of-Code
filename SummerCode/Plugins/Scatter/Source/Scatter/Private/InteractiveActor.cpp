// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"
#include "Widgets/SWeakWidget.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#if WITH_OPENCV
#include "PreOpenCVHeaders.h"
#include "opencv2/opencv.hpp"
#include "PostOpenCVHeaders.h"
#endif



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
	//win->BringToFront(true);


	widget.Get()->divide_image_button.BindLambda([this](UTexture2D* t) {
		divide_image = DivideArea(t);
		//divide_image = t;
		});

	widget.Get()->sub_area_combobox.BindLambda([this](FString subarea) {
		//current_info = i;
		for (auto i : infos)
		{
			if (i.subarea == subarea)
				current_info = i;
		}
		});

	widget.Get()->fill_image_button.BindLambda([this](UTexture2D* t) {
		current_info.fill_image = t;
		FillArea(current_info);
		});

	widget.Get()->distribute_spinbox.BindLambda([this](FString category, double value) {
		int a = 1;
		*current_info.textrue_para.Find(category) = value;
		});

	widget.Get()->transform_spinbox.BindLambda([this](FString category, double value) {
		*current_info.mesh_para.Find(category) = value;
		});
	
	widget.Get()->calculate_texture_button.BindLambda([this]() {
		CalculateTexture(current_info);

		


		//FString MaterialPath = TEXT("Material'/Game/DivideImage/0001.0001'");
		//material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
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

UTexture2D* AInteractiveActor::DivideArea(UTexture2D* t)
{
	//此处调opencv对传入t进行颜色聚类，完成后返回两个数组，分别为label类别信息（int）、像素本身RGB
	//SubAreaInfo数由label中数据的种类决定

	//此处需要初始化infos，需要将kmeans输出数组编码为ue中的texture2D并返回
	//从UTexture2D读取字节数据
	FTexture2DMipMap* mip_map = &t->PlatformData->Mips[0];
	FByteBulkData* raw_ImData = &mip_map->BulkData;
	uint8* pixels = static_cast<uint8*>(raw_ImData->Lock(LOCK_READ_ONLY));

	//原图的Mat
	cv::Mat src = { t->GetSizeX(),t->GetSizeY(),CV_8UC4,pixels };// = UTexture2D; //从UE纹理转化成 cv::Mat
	//默认为RGBA
	cv::cvtColor(src,src,cv::COLOR_RGBA2RGB);
	
	//不同的图大小不一样，需要做Resize
	float scale = 2048.0f / src.cols;
	cv::resize(src,src,cv::Size(2048,src.rows*scale));
	
	
	raw_ImData->Unlock();
	
	int clusterCount = 4;  //需要被分类的个数，为4



	int width = src.cols;
	int height = src.rows;
	int dims = src.channels();

	// 初始化定义
	int sampleCount = width * height;  //总像素点
	cv::Mat points;
	src.convertTo(points, CV_32F);
	points = points.reshape(1, sampleCount);
	//points(像素点的数量，像素点用于聚类的属性数量，类型，颜色)
	// 最终 cols = 像素点的数量，rows = 像素点用于聚类的属性数量 = 3，即使用rgb三个通道聚类
	//Scalar是将图像设置成单一灰度和颜色
	//例如：Scalar(0)为黑色，Scalar(255)为白色，Scalar(0, 255, 0)为绿色
	cv::Mat labels; //存储聚类后的类别id，数据类型为 int 
	cv::Mat centers;  //用来存储聚类后的中心点

	// 运行K-Means
	cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 0.1);

		// 1）TermCriteria::COUNT 最大迭代次数；
		// 2）TermCriteria::EPS 要求的收敛阈值；
		// 3）TermCriteria::COUNT + TermCriteria::EPS 达到2个条件之一即可。
		// maxCount：即最大迭代次数。
		// epsilon：即要求的收敛阈值。
	cv::kmeans(points, clusterCount, labels, criteria, 3, cv::KMEANS_PP_CENTERS, centers);
	centers = centers.reshape(dims, clusterCount);
	// 显示图像分割结果：将样本中分好类的像素赋值给result图片，三通道赋值
	cv::Mat result(width, height, CV_32S); //将分类结果还原回图片的二维数组
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			result.at<int>(row,col) = labels.at<int>(row * width + col, 0);
		}
	}
	// 创建聚类结果图像
	cv::Mat output(src.size(), src.type());
	for (int32 y = 0; y < src.rows; y++)
	{
		for (int32 x = 0; x < src.cols; x++)
		{
			const int32 cluster_idx = labels.at<int32>(y * src.cols + x);
			auto&& center = centers.at<cv::Vec3f>(cluster_idx);
			auto pixel = { static_cast<uchar>(center[0]), static_cast<uchar>(center[1]), static_cast<uchar>(center[2]) };
			output.at<cv::Vec3b>(y, x) = pixel;
		}
	}
	
	//此处可以查看结果
	imwrite("./res.png", output);
	//重新调回颜色格式
	cv::cvtColor(output, output, cv::COLOR_RGB2RGBA);
	//先写死，等待上述代码补全
	SubAreaInfo info1;
	info1.id = 0;
	info1.subarea = FString("SubArea 1");
	infos.Add(info1);

	//将结果重新作为UTexture2D返回
	UTexture2D* Texture = UTexture2D::CreateTransient(width, height, PF_R8G8B8A8);
	if (Texture != nullptr)
	{
		void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, output.data, width*height*4);
		
		Texture->PlatformData->Mips[0].BulkData.Unlock();
		Texture->UpdateResource();
	}
	return Texture;
   
	
	//SubAreaInfo info2;
	//info1.subarea = FString("SubArea 2");
	//infos.Add(info2);

}

void AInteractiveActor::FillArea(SubAreaInfo info)
{
	//用currentinfo.fillimage和divide_image求交，得到采样点图info.samples_texture

}

void AInteractiveActor::CalculateTexture(SubAreaInfo info)
{
	//通过info中各种参数及info.samples_texture计算最终纹理scatter_texture
	scatter_texture = divide_image;
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
