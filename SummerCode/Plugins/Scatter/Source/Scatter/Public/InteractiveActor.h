// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SScatterWidget.h"
#include "InteractiveActor.generated.h"

USTRUCT(BlueprintType)
struct FSubAreaInfo
{
	GENERATED_USTRUCT_BODY()

	//子区域id
	int id;

	//子区域名字
	FString subarea;

	//子区域颜色
	UPROPERTY(EditAnywhere, BlueprintReadWrite)

	FVector3f color;

	//填充图像
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* fill_image = nullptr;

	//采样点图
	//UTexture2D* samples_texture = nullptr;

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

	//kmeans算法颜色聚合，默认为k为4
	UTexture2D* DivideArea(UTexture2D* t, int k = 4);

	void InitSubInfos(TMap<int, FVector3f>& map);

	void ReSetSubAreas();

	////通过路径获取单张图片将之转化成Texture2D
	//UFUNCTION(BlueprintCallable, Category = "Image")
	//UTexture2D* LoadTexture2D(const FString path);

	////获取指定路径下的所有图片的名称，类似于XX.JPG
	//UFUNCTION(BlueprintCallable, Category = "Image")
	//TArray<FString> GetFolderFiles(FString path);

	////将指定路径下的所有图片转化成Texture2D
	//UFUNCTION(BlueprintCallable, Category = "Image")
	//TArray<UTexture2D*> GetAllImageFromFiles(FString Paths);

	////判断图片类型，png,jpg
	//TSharedPtr<class IImageWrapper> GetImageWrapperByExtention(const FString Path);

	//RT转UTexture
	UFUNCTION(BlueprintCallable)
	UTexture2D* RenderTarget2Textrue(UTextureRenderTarget2D* InputRenderTarget, UTexture2D* OutTexture);

	UFUNCTION(BlueprintCallable)
	FColor GetColorFromRT(UTextureRenderTarget2D* RenderTarget, FVector2D uv);


public:
	TSharedPtr<SScatterWidget> widget;
	TSharedPtr<SWindow> win;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FSubAreaInfo> infos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* scatter_texture = nullptr;

	FSubAreaInfo* current_info;
	
	UTexture2D* origin_divide_image = nullptr;


};
