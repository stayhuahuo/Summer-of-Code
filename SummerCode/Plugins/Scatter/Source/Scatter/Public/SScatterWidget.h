// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

DECLARE_DELEGATE_OneParam(DivideImageButtonClick, UTexture2D*);
DECLARE_DELEGATE_OneParam(SelectSubArea, FString);
DECLARE_DELEGATE_OneParam(FillImageButtonClick, UTexture2D*);
DECLARE_DELEGATE_OneParam(ChangeDivideParam, int);
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
	TArray <TSharedPtr<FString>>* GetSubAreas(int num = 0);
	FText GetCurSubArea() const;
	void OnSelectedSubAreaChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type Info);
	void OnDivideButtonClicked();
	UTexture2D* LoadTextureFromFile(FString& path);


	UPROPERTY()
	UTexture2D* divide_texture;
	UPROPERTY()
	UTexture2D* fill_texture;



	TSharedPtr<FSlateBrush> divide_brush;
	TSharedPtr<FSlateBrush> fill_brush;




	//给InteractiveActor调用的委托
	DivideImageButtonClick divide_image_button;
	SelectSubArea sub_area_combobox;
	FillImageButtonClick fill_image_button;
	ChangeDivideParam set_k_spinbox;
	DistributionParams distribute_spinbox;
	TransformParams transform_spinbox;
	CalculateTextureButtonClick calculate_texture_button;

	//相关参数
	TArray<TSharedPtr<FString>> area_array;
	FText selected_sub_area;

};
