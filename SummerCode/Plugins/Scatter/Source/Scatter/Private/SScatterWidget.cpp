// Fill out your copyright notice in the Description page of Project Settings.


#include "SScatterWidget.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SSpinBox.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SScatterWidget::Construct(const FArguments& InArgs)
{
	//分区选择下拉框
	TSharedPtr<SComboBox<TSharedPtr<FString>>> sub_areas_combobox = 
		SNew(SComboBox<TSharedPtr<FString>>)
		.OptionsSource(SScatterWidget::GetSubAreas())
		.OnSelectionChanged(this, &SScatterWidget::OnSelectedSubAreaChanged)
		.OnGenerateWidget_Lambda([](TSharedPtr<FString> Value)->TSharedRef<SWidget>
			{
				return SNew(STextBlock).Text(FText::FromString(*Value));
			})
		[
			SNew(STextBlock).Text(this, &SScatterWidget::GetCurSubArea)
		];


	ChildSlot
	[
		SNew(SBorder)
			.DesiredSizeScale(FVector2D(200,800))
			[
				SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Top)
					.AutoHeight()
					[
						SNew(SVerticalBox)
							+ SVerticalBox::Slot()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Top)
							.AutoHeight()
							[
								SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SButton)
											.Text(FText::FromString("Select Divide Image"))
											.DesiredSizeScale(1)
											.OnClicked_Lambda([this]()->FReply {
											FString p = FPaths::ProjectDir() + TEXT("Content/DivideImage/Colormask_0_01.png");
											t = LoadTextureFromFile(p);
											set_texture.Execute(t);

											//OnDivideButtonClicked();
											return FReply::Handled();
												})
										//SNew(SButton)
										//	.Text(FText::FromString("Select Divide Image"))
										//	.DesiredSizeScale(1)
										//	.OnClicked_Lambda([this]()->FReply {
										//	divide_image_button.Execute(TEXT("Divide Image Path"));
										//	return FReply::Handled();
										//		})
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										sub_areas_combobox.ToSharedRef()
									]
							]
							+ SVerticalBox::Slot()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Top)
							.AutoHeight()
							[
								SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SButton)
											.Text(FText::FromString("Select Scatter Content"))
											.DesiredSizeScale(1)
											.OnClicked_Lambda([this]()->FReply {
											//divide_image_button.Execute(1);
											return FReply::Handled();
												})
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SButton)
											.Text(FText::FromString("Select Fill Image"))
											.DesiredSizeScale(1)
											.OnClicked_Lambda([this]()->FReply {
											fill_image_button.Execute(TEXT("Fill Image Path"));
											return FReply::Handled();
												})
									]
							]
							+ SVerticalBox::Slot()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Top)
							.AutoHeight()
							[
								SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(STextBlock).Text(FText::FromString("community density"))
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SSpinBox<double>)
											.MinValue(1)
											.MaxValue(10000)
											.Value(1)
											.OnEndSliderMovement_Lambda([this](double value) {
											distribute_spinbox.Execute(FString("density"), value);
											//community_density_value = value;
												})
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(STextBlock).Text(FText::FromString("poly size"))
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SSpinBox<double>)
											.MinValue(1)
											.MaxValue(10000)
											.Value(1)
											.OnEndSliderMovement_Lambda([this](double value) {
											distribute_spinbox.Execute(FString("size"), value);
											//poly_size = value;
												})
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(STextBlock).Text(FText::FromString("poly noise"))
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SSpinBox<double>)
											.MinValue(1)
											.MaxValue(10000)
											.Value(1)
											.OnEndSliderMovement_Lambda([this](double value) {
											distribute_spinbox.Execute(FString("noise"), value);
											//poly_noise = value;
												})
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(STextBlock).Text(FText::FromString("poly blur"))
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SSpinBox<double>)
											.MinValue(1)
											.MaxValue(10000)
											.Value(1)
											.OnEndSliderMovement_Lambda([this](double value) {
											distribute_spinbox.Execute(FString("blur"), value);
											//poly_blur = value;
												})
									]
							]
							+ SVerticalBox::Slot()
							.HAlign(HAlign_Center)
							.VAlign(VAlign_Top)
							.AutoHeight()
							[
								SNew(SVerticalBox)
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(STextBlock).Text(FText::FromString("random transformation"))
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SVerticalBox)
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Top)
											.AutoHeight()
											[
												SNew(STextBlock).Text(FText::FromString("random scale"))
											]
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Top)
											.AutoHeight()
											[
												SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(STextBlock).Text(FText::FromString("Min"))
															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(STextBlock).Text(FText::FromString("Max"))
															]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(0.1)
																	.MaxValue(1)
																	.Value(0.5)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("scalemin"), value);
																	//random_scale_min = value;
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(1)
																	.MaxValue(2)
																	.Value(1.5)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("scalemax"), value);
																	//random_scale_max = value;
																		})
															]
													]
											]
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Top)
											.AutoHeight()
											[
												SNew(STextBlock).Text(FText::FromString("random rotation"))
											]
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Top)
											.AutoHeight()
											[
												SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(STextBlock).Text(FText::FromString("Min"))
															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(STextBlock).Text(FText::FromString("Max"))
															]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(-360)
																	.MaxValue(0)
																	.Value(-5)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("rotationXmin"), value);
																	//random_X_rotation_min = value;
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(360)
																	.Value(5)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("rotationXmax"), value);
																	//random_X_rotation_max = value;
																		})
															]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(-360)
																	.MaxValue(0)
																	.Value(-5)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("rotationYmin"), value);
																	//random_Y_rotation_min = value;
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(360)
																	.Value(5)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("rotationYmax"), value);
																	//random_Y_rotation_max = value;
																		})
															]
													] 
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(-360)
																	.MaxValue(0)
																	.Value(0)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("rotationZmin"), value);
																	//random_Z_rotation_min = value;
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(360)
																	.Value(360)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("rotationZmax"), value);
																	//random_Z_rotation_max = value;
																		})
															]
													]
											]
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Top)
											.AutoHeight()
											[
												SNew(STextBlock).Text(FText::FromString("random displacement"))
											]
											+ SVerticalBox::Slot()
											.HAlign(HAlign_Center)
											.VAlign(VAlign_Top)
											.AutoHeight()
											[
												SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(STextBlock).Text(FText::FromString("Min"))
															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(STextBlock).Text(FText::FromString("Max"))
															]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(-100)
																	.MaxValue(0)
																	.Value(-25)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("displacementXmin"), value);
																	//random_X_displacement_min = value;
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(100)
																	.Value(25)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("displacementXmax"), value);
																	//random_X_displacement_max = value;
																		})
															]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(-100)
																	.MaxValue(0)
																	.Value(-25)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("displacementYmin"), value);
																	//random_Y_displacement_min = value;
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(100)
																	.Value(25)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("displacementYmax"), value);
																	//random_Y_displacement_max = value;
																		})
															]
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SHorizontalBox)
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(-100)
																	.MaxValue(0)
																	.Value(0)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("displacementZmin"), value);
																	//random_Z_displacement_min = value;
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.AutoWidth()
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(100)
																	.Value(0)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("displacementZmax"), value);
																	//random_Z_displacement_max = value;
																		})
															]
													]
											]
									]
							]
							+ SVerticalBox::Slot()
								.HAlign(HAlign_Center)
								.VAlign(VAlign_Top)
								.AutoHeight()
								[
									SNew(SButton)
										.Text(FText::FromString("Calculate Texture"))
										.DesiredSizeScale(1)
										.OnClicked_Lambda([this]()->FReply {
										calculate_texture_button.Execute();
										return FReply::Handled();
											})
								]
					]
			]
	];
}
//TSharedRef<ITableRow> SScatterWidget::OnGenerateListView(TSharedPtr<FString> InItem, const TSharedRef<STableViewBase>& OwnerTable)
//{
//	//return SNew(SScatterWidget,OwnerTable),Item
//}
TArray<TSharedPtr<FString>>* SScatterWidget::GetSubAreas()
{
	area_array.Add(MakeShared<FString>(TEXT("SubArea 1")));
	area_array.Add(MakeShared<FString>(TEXT("SubArea 2")));
	return &area_array;
}
FText SScatterWidget::GetCurSubArea() const
{
	return this->selected_sub_area;
}
void SScatterWidget::OnSelectedSubAreaChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type Info)
{
	this->selected_sub_area = FText::FromString(*NewValue);
	sub_area_combobox.Execute(*NewValue);
}

void SScatterWidget::OnDivideButtonClicked()
{
	TSharedRef<SWindow> NewWindow = SNew(SWindow)
		.Title(FText::FromString("Select an Image"))
		.ClientSize(FVector2D(800, 600))
		.SupportsMinimize(false)
		.SupportsMaximize(false);

	// 获取本地文件路径中的图片资源
	TArray<FString> ImagePaths = { TEXT("Content/DivideImage/Colormask_0_01.png"), 
		TEXT("Content/DivideImage/Colormask_0_03.png") }; 

	TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);

	for (FString& ImagePath : ImagePaths)
	{
		TSharedPtr<SImage> ImageWidget;
		FString path = FPaths::ProjectDir() + ImagePath;
		UTexture2D* Texture = LoadTextureFromFile(path);

		if (Texture)
		{
			TSharedPtr<FSlateBrush> ImageBrush = MakeShareable(new FSlateBrush());
			ImageBrush->SetResourceObject(Texture);

			HorizontalBox->AddSlot()
				[
					SNew(SBox)
						.WidthOverride(200)
						.HeightOverride(200)
						[
							SNew(SImage)
								.Image(ImageBrush.Get())
						]
				];
		}
	}

	NewWindow->SetContent(HorizontalBox);

	// 将窗口添加到 Slate 应用程序中
	FSlateApplication::Get().AddWindow(NewWindow);

	// 确保窗口在最前方显示
	NewWindow->BringToFront();
}

UTexture2D* SScatterWidget::LoadTextureFromFile(FString& path)
{
	UTexture2D* Texture = nullptr;

	//加载图片为纹理
	TArray<uint8> RawFileData;
	if (FFileHelper::LoadFileToArray(RawFileData, *path))
	{
		IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
		EImageFormat ImageFormat = ImageWrapperModule.DetectImageFormat(RawFileData.GetData(), RawFileData.Num());

		if (ImageFormat != EImageFormat::Invalid)
		{
			TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(ImageFormat);
			if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
			{
				TArray<uint8> UncompressedBGRA;
				if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedBGRA))
				{
					Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight());
					void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
					FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
					Texture->PlatformData->Mips[0].BulkData.Unlock();
					Texture->UpdateResource();
				}
			}
		}
	}
	return Texture;
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION
