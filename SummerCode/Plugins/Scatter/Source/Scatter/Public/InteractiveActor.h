// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SScatterWidget.h"
#include "InteractiveActor.generated.h"

struct SubAreaInfo
{
	//����������
	FString subarea;

	//������ͼ
	//FTexture after_divide_texture;

	//������ͼ
	//FTexture samples_texture;

	//���ͼ��
	FString fill_image_path;
	UTexture2D* fill_image = nullptr;

	//Ⱥ���ܶ�
	double community_density_value = 1;

	//�۴�
	double poly_size = 1;//��С
	double poly_noise = 1;//����
	double poly_blur = 1;//ģ��

	//����任
	//����
	double random_scale_min = 0.5;
	double random_scale_max = 1.5;

	//��ת
	double random_X_rotation_min = -5;
	double random_X_rotation_max = 5;
	double random_Y_rotation_min = -5;
	double random_Y_rotation_max = 5;
	double random_Z_rotation_min = 0;
	double random_Z_rotation_max = 360;

	//ƫ��
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
