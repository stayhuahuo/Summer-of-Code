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
