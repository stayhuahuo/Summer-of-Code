// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SScatterWidget.h"
#include "InteractiveActor.generated.h"

struct SubAreaInfo
{
	//子区域名字
	FString subarea;

	//子区域图
	//FTexture after_divide_texture;

	//采样点图
	//FTexture samples_texture;

	//填充图像
	FString fill_image_path;
	UTexture2D* fill_image = nullptr;

	//影响texture参数：群落密度、聚丛大小、聚丛噪声、聚丛模糊
	TMap<FString, double> textrue_para = { {FString("community_density_value"),1} ,{FString("poly_size"),1},
											{FString("poly_noise"),1}, {FString("poly_blur"),1} };

	//影响mesh参数：缩放、旋转、偏移
	TMap<FString, double> mesh_para = { {FString("random_scale_min"),0.5} ,{FString("random_scale_max"),1.5},
										{FString("random_X_rotation_min"),-5}, {FString("random_X_rotation_max"),5},
										{FString("random_Y_rotation_min"),-5}, {FString("random_Y_rotation_max"),5},
										{FString("random_Z_rotation_min"),0}, {FString("random_Z_rotation_max"),360},
										{FString("random_X_rotation_min"),-25}, {FString("random_X_rotation_max"),25},
										{FString("random_Y_rotation_min"),-25}, {FString("random_Y_rotation_max"),25},
										{FString("random_Z_rotation_min"),0}, {FString("random_Z_rotation_max"),0} };

};

UCLASS(Blueprintable)
class SCATTER_API AInteractiveActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInteractiveActor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DivideArea(FString path);

	void FillArea(SubAreaInfo info);

	void CalculateTexture(SubAreaInfo info);

	//通过路径获取单张图片将之转化成Texture2D
	UFUNCTION(BlueprintCallable, Category = "Image")
	UTexture2D* LoadTexture2D(const FString path);

	//获取指定路径下的所有图片的名称，类似于XX.JPG
	UFUNCTION(BlueprintCallable, Category = "Image")
	TArray<FString> GetFolderFiles(FString path);

	//将指定路径下的所有图片转化成Texture2D
	UFUNCTION(BlueprintCallable, Category = "Image")
	TArray<UTexture2D*> GetAllImageFromFiles(FString Paths);

	//判断图片类型，png,jpg
	TSharedPtr<class IImageWrapper> GetImageWrapperByExtention(const FString Path);


public:
	TSharedPtr<SScatterWidget> widget;

	TArray<SubAreaInfo> infos;
	SubAreaInfo current_info;
	
	TSharedPtr<SWindow> win;

	UPROPERTY(BlueprintReadWrite)
	UTexture2D* texture = nullptr;

	UPROPERTY(BlueprintReadWrite)
	TArray<UTexture*> all_images;


	UPROPERTY(BlueprintReadWrite)
	UMaterialInterface* material = nullptr;



};
