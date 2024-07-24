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

	//重载combobox函数
	TArray <TSharedPtr<FString>>* GetSubAreas(int num = 0);
	FText GetCurSubArea() const;
	void OnSelectedSubAreaChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type Info);
	
	//动态设置divide image数量，写的不对
	//void OnDivideButtonClicked();

	//从指定路径读图为texture
	UTexture2D* LoadTextureFromFile(FString& path);

	//重置下拉框
	void ReSetSubAreas();

	//divide_image和fill_image，应该写成数组动态设置
	TSharedPtr<UTexture2D*> divide_texture;
	UTexture2D* fill_texture;
	UTexture2D* fill_texture_1;

	TSharedPtr<FSlateBrush> divide_brush;
	TSharedPtr<FSlateBrush> fill_brush;
	TSharedPtr<FSlateBrush> fill_brush_1;


	//给InteractiveActor调用的委托
	DivideImageButtonClick divide_image_button;
	SelectSubArea sub_area_combobox;
	FillImageButtonClick fill_image_button;
	ChangeDivideParam set_k_spinbox;
	DistributionParams distribute_spinbox;
	TransformParams transform_spinbox;

	//子区域下拉框
	TSharedPtr<SComboBox<TSharedPtr<FString>>> sub_areas_combobox;

	//子区域下拉框列表
	TArray<TSharedPtr<FString>> area_array;

	//选中的子区域
	FText selected_sub_area;

};
