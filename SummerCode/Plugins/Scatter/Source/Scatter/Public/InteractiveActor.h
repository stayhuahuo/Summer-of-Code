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

	//群落密度
	double community_density_value = 1;

	//聚丛
	double poly_size = 1;//大小
	double poly_noise = 1;//噪声
	double poly_blur = 1;//模糊

	//随机变换
	//缩放
	double random_scale_min = 0.5;
	double random_scale_max = 1.5;

	//旋转
	double random_X_rotation_min = -5;
	double random_X_rotation_max = 5;
	double random_Y_rotation_min = -5;
	double random_Y_rotation_max = 5;
	double random_Z_rotation_min = 0;
	double random_Z_rotation_max = 360;

	//偏移
	double random_X_displacement_min = -25;
	double random_X_displacement_max = 25;
	double random_Y_displacement_min = -25;
	double random_Y_displacement_max = 25;
	double random_Z_displacement_min = 0;
	double random_Z_displacement_max = 0;
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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void DivideArea(FString path);

	void FillArea(SubAreaInfo info);

	void CalculateTexture(SubAreaInfo info);

public:
	TSharedPtr<SScatterWidget> widget;

	TArray<SubAreaInfo> infos;
	SubAreaInfo current_info;

	UPROPERTY(BlueprintReadWrite)
	TArray<UTexture*> all_images;

	UPROPERTY(BlueprintReadWrite)
	UTexture* texture = nullptr;

	UPROPERTY(BlueprintReadWrite)
	UMaterialInterface* material = nullptr;



};
