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
		divide_image = t;
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


	//先写死，等待上述代码补全
	SubAreaInfo info1;
	info1.id = 0;
	info1.subarea = FString("SubArea 1");
	infos.Add(info1);

	return nullptr;
	

	
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
