// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(DivideImageButtonClick, FString);
DECLARE_DELEGATE_OneParam(SelectSubArea, FString);
DECLARE_DELEGATE_OneParam(FillImageButtonClick, FString);
DECLARE_DELEGATE_TwoParams(DistributionParams, FString, double);
DECLARE_DELEGATE_TwoParams(TransformParams, FString, double);
DECLARE_DELEGATE(CalculateTextureButtonClick);

/**
 * 
 */
class SCATTER_API SScatterWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SScatterWidget)
	{}
	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	//TSharedRef<ITableRow> OnGenerateListView(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable);
	TArray <TSharedPtr<FString>>* GetSubAreas();
	FText GetCurSubArea() const;
	void OnSelectedSubAreaChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type Info);

	//给InteractiveActor调用的委托
	DivideImageButtonClick divide_image_button;
	SelectSubArea sub_area_combobox;
	FillImageButtonClick fill_image_button;
	DistributionParams distribute_spinbox;
	TransformParams transform_spinbox;
	CalculateTextureButtonClick calculate_texture_button;



	//相关参数
	TArray<TSharedPtr<FString>> area_array;
	FText selected_sub_area;

	double community_density_value = 1;
	double poly_size = 1;
	double poly_noise = 1;
	double poly_blur = 1;
	double random_scale_min = 0.5;
	double random_scale_max = 1.5;
	double random_X_rotation_min = -5;
	double random_X_rotation_max = 5;
	double random_Y_rotation_min = -5;
	double random_Y_rotation_max = 5;
	double random_Z_rotation_min = 0;
	double random_Z_rotation_max = 360;

	double random_X_displacement_min = -25;
	double random_X_displacement_max = 25;
	double random_Y_displacement_min = -25;
	double random_Y_displacement_max = 25;
	double random_Z_displacement_min = 0;
	double random_Z_displacement_max = 0;

};
