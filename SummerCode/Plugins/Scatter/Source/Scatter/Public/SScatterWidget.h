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

	//����combobox����
	TArray <TSharedPtr<FString>>* GetSubAreas(int num = 0);
	FText GetCurSubArea() const;
	void OnSelectedSubAreaChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type Info);
	
	//��̬����divide image������д�Ĳ���
	//void OnDivideButtonClicked();

	//��ָ��·����ͼΪtexture
	UTexture2D* LoadTextureFromFile(FString& path);

	//����������
	void ReSetSubAreas();

	//divide_image��fill_image��Ӧ��д�����鶯̬����
	TSharedPtr<UTexture2D*> divide_texture;
	UTexture2D* fill_texture;
	UTexture2D* fill_texture_1;

	TSharedPtr<FSlateBrush> divide_brush;
	TSharedPtr<FSlateBrush> fill_brush;
	TSharedPtr<FSlateBrush> fill_brush_1;


	//��InteractiveActor���õ�ί��
	DivideImageButtonClick divide_image_button;
	SelectSubArea sub_area_combobox;
	FillImageButtonClick fill_image_button;
	ChangeDivideParam set_k_spinbox;
	DistributionParams distribute_spinbox;
	TransformParams transform_spinbox;

	//������������
	TSharedPtr<SComboBox<TSharedPtr<FString>>> sub_areas_combobox;

	//�������������б�
	TArray<TSharedPtr<FString>> area_array;

	//ѡ�е�������
	FText selected_sub_area;

};
