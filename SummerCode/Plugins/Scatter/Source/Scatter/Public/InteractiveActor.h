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

	//������id
	int id;

	//����������
	FString subarea;

	//��������ɫ
	UPROPERTY(EditAnywhere, BlueprintReadWrite)

	FVector3f color;

	//���ͼ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* fill_image = nullptr;

	//������ͼ
	//UTexture2D* samples_texture = nullptr;

	//Ӱ��texture������Ⱥ���ܶȡ��۴Դ�С���۴��������۴�ģ��
	TMap<FString, double> textrue_para = { {FString("community_density_value"),1} ,{FString("poly_size"),1},
											{FString("poly_noise"),1}, {FString("poly_blur"),1} };

	//Ӱ��mesh���������š���ת��ƫ��
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

	//kmeans�㷨��ɫ�ۺϣ�Ĭ��ΪkΪ4
	UTexture2D* DivideArea(UTexture2D* t, int k = 4);

	void InitSubInfos(TMap<int, FVector3f>& map);

	void ReSetSubAreas();

	////ͨ��·����ȡ����ͼƬ��֮ת����Texture2D
	//UFUNCTION(BlueprintCallable, Category = "Image")
	//UTexture2D* LoadTexture2D(const FString path);

	////��ȡָ��·���µ�����ͼƬ�����ƣ�������XX.JPG
	//UFUNCTION(BlueprintCallable, Category = "Image")
	//TArray<FString> GetFolderFiles(FString path);

	////��ָ��·���µ�����ͼƬת����Texture2D
	//UFUNCTION(BlueprintCallable, Category = "Image")
	//TArray<UTexture2D*> GetAllImageFromFiles(FString Paths);

	////�ж�ͼƬ���ͣ�png,jpg
	//TSharedPtr<class IImageWrapper> GetImageWrapperByExtention(const FString Path);

	//RTתUTexture
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
