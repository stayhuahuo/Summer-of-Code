// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"
#include "Widgets/SWeakWidget.h"

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
	TSharedRef<SWindow> win = SNew(SWindow)
		.Title(FText::FromString(TEXT("hello")))
		.ClientSize(FVector2D(200, 600))
		.ScreenPosition(FVector2D(100, 100))
		[
			widget.ToSharedRef()
		];
	FSlateApplication::Get().AddWindow(win);

	widget.Get()->divide_image_button.BindLambda([this](FString path) {
		DivideArea(path);
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
		if (category == "density")
			current_info.community_density_value = value;
		else if(category == "size")
			current_info.poly_size = value;
		else if (category == "noise")
			current_info.poly_noise = value;
		else if (category == "blur")
			current_info.poly_blur = value;
		});

	widget.Get()->transform_spinbox.BindLambda([this](FString category, double value) {
		if (category == "scalemin")
			current_info.random_scale_min = value;
		else if (category == "scalemax")
			current_info.random_scale_max = value;
		else if (category == "rotationXmin")
			current_info.random_X_rotation_min = value;
		else if (category == "rotationXmax")
			current_info.random_X_rotation_max= value;
		else if (category == "rotationYmin")
			current_info.random_Y_rotation_min = value;
		else if (category == "rotationYmax")
			current_info.random_Y_rotation_max = value;
		else if (category == "rotationZmin")
			current_info.random_Z_rotation_min = value;
		else if (category == "rotationZmax")
			current_info.random_Z_rotation_max = value;
		else if (category == "displacementXmin")
			current_info.random_X_displacement_min = value;
		else if (category == "displacementXmax")
			current_info.random_X_displacement_max = value;
		else if (category == "displacementYmin")
			current_info.random_Y_displacement_min = value;
		else if (category == "displacementYmax")
			current_info.random_Y_displacement_max = value;
		else if (category == "displacementZmin")
			current_info.random_Z_displacement_min = value;
		else if (category == "displacementZmax")
			current_info.random_Z_displacement_max = value;
		});

	widget.Get()->calculate_texture_button.BindLambda([this]() {
		CalculateTexture(current_info);




		FString MaterialPath = TEXT("Material'/Game/DivideImage/0001.0001'");
		material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
		});
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

