// Fill out your copyright notice in the Description page of Project Settings.


#include "SScatterWidget.h"
#include "SlateOptMacros.h"
#include "Widgets/Input/SSpinBox.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SScatterWidget::Construct(const FArguments& InArgs)
{
	divide_brush = MakeShareable(new FSlateBrush());
	fill_brush = MakeShareable(new FSlateBrush());
	fill_brush_1 = MakeShareable(new FSlateBrush());

	FString p = FPaths::ProjectDir() + TEXT("Content/DivideImage/testpic.jpg");
	divide_texture = MakeShareable(new(UTexture2D*)(LoadTextureFromFile(p)));
	divide_brush->SetResourceObject(*divide_texture);
	divide_brush->ImageSize = FVector2D(32, 32);

	p = FPaths::ProjectDir() + TEXT("Content/FillImage/patches1_clamp_mask.png");
	fill_texture = LoadTextureFromFile(p);
	fill_brush->SetResourceObject(fill_texture);
	fill_brush->ImageSize = FVector2D(32, 32);

	p = FPaths::ProjectDir() + TEXT("Content/FillImage/bands1_clamp_mask.png");
	fill_texture_1 = LoadTextureFromFile(p);
	fill_brush_1->SetResourceObject(fill_texture_1);
	fill_brush_1->ImageSize = FVector2D(32, 32);


	//分区选择下拉框
	sub_areas_combobox = 
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
			.DesiredSizeScale(FVector2D(250,650))
			[
				SNew(SVerticalBox)
					+ SVerticalBox::Slot()
					.HAlign(HAlign_Center)
					.VAlign(VAlign_Center)
					.FillHeight(1)
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
										SNew(STextBlock)
											.Text(FText::FromString("Select Divide Image"))
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SHorizontalBox)
											+ SHorizontalBox::Slot()
											.HAlign(HAlign_Left)
											.VAlign(VAlign_Top)
											.AutoWidth()
											[
												SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SImage).Image(divide_brush.Get())
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SButton)
														.Text(FText::FromString("select"))
														.DesiredSizeScale(1)
														.OnClicked_Lambda([this]()->FReply {
														divide_image_button.Execute(*divide_texture);
														return FReply::Handled();
															})
													]
											]
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SHorizontalBox)
											+ SHorizontalBox::Slot()
											.HAlign(HAlign_Left)
											.VAlign(VAlign_Top)
											.FillWidth(1)
											[
												SNew(STextBlock)
													.Text(FText::FromString("Set Zone:    "))
											]
											+ SHorizontalBox::Slot()
											.HAlign(HAlign_Fill)
											.VAlign(VAlign_Top)
											.FillWidth(2)
											[
												SNew(SSpinBox<int>)
													.MinValue(1)
													.MaxValue(10)
													.Value(4)
													.MinDesiredWidth(100)
													.OnEndSliderMovement_Lambda([this](int value) {
													set_k_spinbox.Execute(value);
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
											.HAlign(HAlign_Left)
											.VAlign(VAlign_Top)
											.AutoWidth()
											[
												SNew(STextBlock)
													.Text(FText::FromString("Sub Area:    "))
											]
											+ SHorizontalBox::Slot()
												.HAlign(HAlign_Right)
												.VAlign(VAlign_Top)
											.AutoWidth()
											[
												sub_areas_combobox.ToSharedRef()
											]
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
										SNew(STextBlock)
											.Text(FText::FromString("Select Fill Image"))
									]
									+ SVerticalBox::Slot()
									.HAlign(HAlign_Center)
									.VAlign(VAlign_Top)
									.AutoHeight()
									[
										SNew(SHorizontalBox)
											+SHorizontalBox::Slot()
											.HAlign(HAlign_Left)
											.VAlign(VAlign_Top)
											.AutoWidth()
											[
												SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SImage).Image(fill_brush.Get())
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[

														SNew(SButton)
															.Text(FText::FromString("select"))
															.DesiredSizeScale(1)
															.OnClicked_Lambda([this]()->FReply {
															fill_image_button.Execute(fill_texture);
															return FReply::Handled();
																})
													]
											]
											+ SHorizontalBox::Slot()
											.HAlign(HAlign_Left)
											.VAlign(VAlign_Top)
											.AutoWidth()
											[
												SNew(SVerticalBox)
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[
														SNew(SImage).Image(fill_brush_1.Get())
													]
													+ SVerticalBox::Slot()
													.HAlign(HAlign_Center)
													.VAlign(VAlign_Top)
													.AutoHeight()
													[

														SNew(SButton)
															.Text(FText::FromString("select"))
															.DesiredSizeScale(1)
															.OnClicked_Lambda([this]()->FReply {
															fill_image_button.Execute(fill_texture_1);
															return FReply::Handled();
																})
													]
											]
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
											.MinDesiredWidth(100)
											.OnEndSliderMovement_Lambda([this](double value) {
											distribute_spinbox.Execute(FString("community_density_value"), value);
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
											.MinDesiredWidth(100)
											.OnEndSliderMovement_Lambda([this](double value) {
											distribute_spinbox.Execute(FString("poly_size"), value);
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
											.MinDesiredWidth(100)
											.OnEndSliderMovement_Lambda([this](double value) {
											distribute_spinbox.Execute(FString("poly_noise"), value);
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
											.MinDesiredWidth(100)
											.OnEndSliderMovement_Lambda([this](double value) {
											distribute_spinbox.Execute(FString("poly_blur"), value);
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
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(0.1)
																	.MaxValue(1)
																	.Value(0.5)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_scale_min"), value);
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(1)
																	.MaxValue(2)
																	.Value(1.5)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_scale_max"), value);
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
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(-360)
																	.MaxValue(0)
																	.Value(-5)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_X_rotation_min"), value);
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(360)
																	.Value(5)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_X_rotation_max"), value);
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
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(-360)
																	.MaxValue(0)
																	.Value(-5)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_Y_rotation_min"), value);
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(360)
																	.Value(5)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_Y_rotation_max"), value);
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
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(-360)
																	.MaxValue(0)
																	.Value(0)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_Z_rotation_min"), value);
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(360)
																	.Value(360)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_Z_rotation_max"), value);
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
															.FillWidth(1)
															[
																SNew(STextBlock).Text(FText::FromString("Min"))
															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.FillWidth(1)
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
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(-100)
																	.MaxValue(0)
																	.Value(-25)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_X_rotation_min"), value);
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(100)
																	.Value(25)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_X_rotation_max"), value);
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
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(-100)
																	.MaxValue(0)
																	.Value(-25)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_Y_rotation_min"), value);
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(100)
																	.Value(25)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_Y_rotation_max"), value);
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
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(-100)
																	.MaxValue(0)
																	.Value(0)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_Z_rotation_min"), value);
																		})

															]
															+ SHorizontalBox::Slot()
															.HAlign(HAlign_Center)
															.VAlign(VAlign_Top)
															.FillWidth(1)
															[
																SNew(SSpinBox<double>)
																	.MinValue(0)
																	.MaxValue(100)
																	.Value(0)
																	.MinDesiredWidth(50)
																	.OnEndSliderMovement_Lambda([this](double value) {
																	transform_spinbox.Execute(FString("random_Z_rotation_max"), value);
																		})
															]
													]
											]
									]
							]
					]
			]
	];
}

TArray<TSharedPtr<FString>>* SScatterWidget::GetSubAreas(int num)
{
	for (int32 a = 1; a < num + 1; a++)
	{
		area_array.Add(MakeShared<FString>(FString::Printf(TEXT("SubArea %d"), a)));
	}
	return &area_array;
}

FText SScatterWidget::GetCurSubArea() const
{
	return this->selected_sub_area;
}

void SScatterWidget::OnSelectedSubAreaChanged(TSharedPtr<FString> NewValue, ESelectInfo::Type Info)
{
	if (NewValue != nullptr)
	{
		this->selected_sub_area = FText::FromString(*NewValue);
		sub_area_combobox.Execute(*NewValue);
	}
}

//void SScatterWidget::OnDivideButtonClicked()
//{
//	TSharedRef<SWindow> NewWindow = SNew(SWindow)
//		.Title(FText::FromString("Select an Image"))
//		.ClientSize(FVector2D(800, 600))
//		.SupportsMinimize(false)
//		.SupportsMaximize(false);
//
//	// 获取本地文件路径中的图片资源
//	TArray<FString> ImagePaths = { TEXT("Content/DivideImage/Colormask_0_01.png"), 
//		TEXT("Content/DivideImage/Colormask_0_03.png") }; 
//
//	TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);
//
//	for (FString& ImagePath : ImagePaths)
//	{
//		TSharedPtr<SImage> ImageWidget;
//		FString path = FPaths::ProjectDir() + ImagePath;
//		UTexture2D* Texture = LoadTextureFromFile(path);
//
//		if (Texture)
//		{
//			TSharedPtr<FSlateBrush> ImageBrush = MakeShareable(new FSlateBrush());
//			ImageBrush->SetResourceObject(Texture);
//
//			HorizontalBox->AddSlot()
//				[
//					SNew(SBox)
//						.WidthOverride(200)
//						.HeightOverride(200)
//						[
//							SNew(SImage)
//								.Image(ImageBrush.Get())
//						]
//				];
//		}
//	}
//
//	NewWindow->SetContent(HorizontalBox);
//
//	// 将窗口添加到 Slate 应用程序中
//	FSlateApplication::Get().AddWindow(NewWindow);
//
//	// 确保窗口在最前方显示
//	NewWindow->BringToFront();
//}

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

void SScatterWidget::ReSetSubAreas()
{
	area_array.Empty();
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
