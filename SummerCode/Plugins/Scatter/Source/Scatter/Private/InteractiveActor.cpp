// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"
#include "Widgets/SWeakWidget.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "ScatterRendering.h"

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
		.Title(FText::FromString(TEXT("Scatter")))
		.ClientSize(FVector2D(250, 650))
		[
			widget.ToSharedRef()
		];
	FSlateApplication::Get().AddWindow(win.ToSharedRef());
	//win->BringToFront(true);


	widget.Get()->divide_image_button.BindLambda([this](UTexture2D* t) {
		//if (divide_image == nullptr)
		origin_divide_image = t;
		divide_image = DivideArea(t);
		});

	widget.Get()->set_k_spinbox.BindLambda([this](int k) {
		divide_image = DivideArea(nullptr, k);
		});

	widget.Get()->sub_area_combobox.BindLambda([this](FString subarea) {
		for (auto& i : infos)
		{
			if (i.subarea == subarea)
			{
				current_info = &i;
				break;
			}
		}
		});

	widget.Get()->fill_image_button.BindLambda([this](UTexture2D* t) {
		fill_texture = t;
		current_info->fill_image = t;
		//FillArea(*current_info);
		});

	

	widget.Get()->distribute_spinbox.BindLambda([this](FString category, double value) {
		*current_info->textrue_para.Find(category) = value;
		});

	widget.Get()->transform_spinbox.BindLambda([this](FString category, double value) {
		*current_info->mesh_para.Find(category) = value;
		});
	
	widget.Get()->calculate_texture_button.BindLambda([this]() {
		CalculateTexture();
		color = current_info->color;
		


		//FString MaterialPath = TEXT("Material'/Game/DivideImage/0001.0001'");
		//material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
		});
	//UTexture2D* test = UTexture2D::CreateTransient(2048, 2048, PF_R8G8B8A8);
	//test->MipGenSettings = TMGS_NoMipmaps;
	//test->CompressionSettings = TC_EditorIcon;
	//test->SRGB = true;
	//test->AddToRoot();
	//FTexture2DMipMap& Mip = test->PlatformData->Mips[0];
	//void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	//FColor* FormData = static_cast<FColor*>(Data);
	//for (int32 Y = 0; Y < 2048; Y++)
	//{
	//	for (int32 X = 0; X < 2048; X++)
	//	{
	//		FormData[Y * 2048 + X] = FColor::White;
	//	}
	//}
	//Mip.BulkData.Unlock();
	//test->UpdateResource();
	//te = MakeShareable(test);
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

UTexture2D* AInteractiveActor::DivideArea(UTexture2D* t, int k)
{
	if (t == nullptr)
	{
		t = origin_divide_image;
		ReSetSubAreas();
	}
	//此处调opencv对传入t进行颜色聚类，完成后返回两个数组，分别为label类别信息（int）、像素本身RGB
	//SubAreaInfo数由label中数据的种类决定

	//此处需要初始化infos，需要将kmeans输出数组编码为ue中的texture2D并返回
	//从UTexture2D读取字节数据
	FTexture2DMipMap* mip_map = &t->PlatformData->Mips[0];
	FByteBulkData* raw_ImData = &mip_map->BulkData;
	uint8* pixels = static_cast<uint8*>(raw_ImData->Lock(LOCK_READ_ONLY));

	//原图的Mat
	cv::Mat src = { t->GetSizeX(),t->GetSizeY(),CV_8UC4,pixels };// = UTexture2D; //从UE纹理转化成 cv::Mat
	raw_ImData->Unlock();
	//默认为RGBA
	cv::cvtColor(src,src,cv::COLOR_RGBA2RGB);
	
	
	//不同的图大小不一样，需要做Resize
	float scale = 2048.0f / src.cols;
	cv::resize(src,src,cv::Size(2048,src.rows*scale));
	//模糊操作
	int iter_num= 10;
	int kernal_size = 11;
	for (size_t i = 0; i < iter_num; i++)
	{
		cv::GaussianBlur(src, src, cv::Size(kernal_size, kernal_size), 3);
	}
	//聚类操作
	int clusterCount = k;  //需要被分类的个数，默认为4
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
	
	TMap<int,FVector3f> map;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			map.Add(labels.at<int>(row * width + col), FVector3f(0, 0, 0));
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
			if (*map.Find(cluster_idx) == FVector3f(0, 0, 0))
			{
				*map.Find(cluster_idx) = FVector3f(center[0], center[1], center[2]);

			}
			auto pixel = { static_cast<uchar>(center[0]), static_cast<uchar>(center[1]), static_cast<uchar>(center[2]) };
			output.at<cv::Vec3b>(y, x) = pixel;
		}
	}
	
	//动态设置下拉框,初始化子区域信息
	InitSubInfos(map);

	//此处可以查看结果
	imwrite("C:/Users/User/Desktop/LyraResearch-main/res.png", output);
	//重新调回颜色格式
	cv::cvtColor(output, output, cv::COLOR_RGB2BGRA);

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
}

void AInteractiveActor::InitSubInfos(TMap<int, FVector3f>& map)
{
	//设置下拉框
	widget.Get()->GetSubAreas(map.Num());

	for (auto m : map)
	{
		FSubAreaInfo info;
		info.id = m.Key;
		info.color = m.Value;
		info.subarea = FString::Printf(TEXT("SubArea %d"), m.Key + 1);
		infos.Add(info);
	}
}

void AInteractiveActor::ReSetSubAreas()
{
	infos.Empty();
	widget.Get()->ReSetSubAreas();
}

//void AInteractiveActor::FillArea(FSubAreaInfo info)
//{
//	//用currentinfo.fillimage和divide_image求交，得到采样点图info.samples_texture
//	//UTexture2D* t=UKismetRenderingLibrary::RenderTargetCreateStaticTexture2DEditorOnly(GetWorld(),Rendert,)
//}

void AInteractiveActor::CalculateTexture()
{
	//通过info中各种参数及info.samples_texture计算最终纹理scatter_texture
	scatter_texture = divide_image;
	current_info;
	//return;
	//UTextureRenderTarget2D* RenderTarget = NewObject<UTextureRenderTarget2D>();
	//RenderTarget->InitAutoFormat(1024, 1024);
	//RenderTarget->UpdateResourceImmediate(true); 

	//FString p = FPaths::ProjectDir() + TEXT("Content/DivideImage/Colormask_0_03.png");
	//te = LoadTexture2D(p);

	for (auto& i : infos)
	{
		if (i.fill_image == nullptr)
			continue;
		//UTexture2D* in_kmeans_texture2D = NewObject<UTexture2D>(scatter_texture);
		//FString TextureName = "Test";
		//FString PackName = FString::Printf(TEXT("/Game/%s"), *TextureName);
		//UPackage* Package = CreatePackage(*PackName);
		//Package->FullyLoad();
		//UTexture2D* Texture = NewObject<UTexture2D>(Package, *TextureName, RF_Public | RF_Standalone | RF_MarkAsRootSet);

		//static const int32 TextureWidth = 2048;
		//static const int32 TextureHeight = 2048;
		//TArray<FColor> SourceColors;
		//SourceColors.Init(FColor::White, TextureWidth * TextureHeight);

		////Source 
		//Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
		//Texture->Source.Init(TextureWidth, TextureHeight, 1, 1, ETextureSourceFormat::TSF_BGRA8);
		////Texture->Source.Init(TextureWidth, TextureHeight, 1, 1, ETextureSourceFormat::TSF_BGRA8);
		//uint8* SourceData = Texture->Source.LockMip(0);
		//FMemory::Memcpy(SourceData, SourceColors.GetData(), sizeof(FColor) * SourceColors.Num());
		//Texture->Source.UnlockMip(0);

		////PlatformData
		//Texture->PlatformData = new FTexturePlatformData();
		//Texture->PlatformData->SizeX = TextureWidth;
		//Texture->PlatformData->SizeY = TextureHeight;
		//Texture->PlatformData->PixelFormat = EPixelFormat::PF_R8G8B8A8;
		////Texture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;
		//FTexture2DMipMap* NewMipMap = new FTexture2DMipMap();
		//Texture->PlatformData->Mips.Add(NewMipMap);
		//NewMipMap->SizeX = TextureWidth;
		//NewMipMap->SizeY = TextureHeight;

		//Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		//uint8* MipData = (uint8*)Texture->PlatformData->Mips[0].BulkData.Realloc(sizeof(FColor) * TextureWidth * TextureHeight);
		//FMemory::Memcpy(MipData, SourceColors.GetData(), sizeof(FColor) * SourceColors.Num());
		//Texture->PlatformData->Mips[0].BulkData.Unlock();

		//Texture->UpdateResource();
		//Texture->PostEditChange();
		//Texture->MarkPackageDirty();
		//FAssetRegistryModule::AssetCreated(Texture);


		//USimpleRenderingExampleBlueprintLibrary::UseRDGCompute(GetWorld(), scatter_texture, fill_texture, *&Texture, FLinearColor(i.color));
		//scatter_texture = Texture;
		//scatter_texture = UKismetRenderingLibrary::RenderTargetCreateStaticTexture2DEditorOnly(RenderTarget);
	}
	
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

UTexture2D* AInteractiveActor::RenderTarget2Textrue(UTextureRenderTarget2D* InputRenderTarget, UTexture2D* OutTexture)
{
	// 获取 Render Target 的大小
	FTextureRenderTargetResource* RenderTargetResource = InputRenderTarget->GameThread_GetRenderTargetResource();
	int32 Width = InputRenderTarget->SizeX;
	int32 Height = InputRenderTarget->SizeY;

	// 创建一个新的 Texture2D 对象
	OutTexture = UTexture2D::CreateTransient(Width, Height, PF_B8G8R8A8);
	//if (!Texture)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Failed to create Texture2D."));
	//	return;
	//}

	// 锁定并读取 Render Target 的像素数据
	TArray<FColor> OutBMP;
	RenderTargetResource->ReadPixels(OutBMP);

	// 填充 Texture2D 数据
	void* TextureData = OutTexture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(TextureData, OutBMP.GetData(), Width * Height * sizeof(FColor));
	OutTexture->PlatformData->Mips[0].BulkData.Unlock();

	// 更新 Texture2D 资源
	OutTexture->UpdateResource();

	return OutTexture;
}
