// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveActor.h"
#include "Widgets/SWeakWidget.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"

#if WITH_OPENCV
#include "PreOpenCVHeaders.h"
#include "opencv2/opencv.hpp"
#include "PostOpenCVHeaders.h"
#endif



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
	win = SNew(SWindow)
		.Title(FText::FromString(TEXT("Scatter window")))
		.ClientSize(FVector2D(200, 600))
		.ScreenPosition(FVector2D(100, 100))
		[
			widget.ToSharedRef()
		];
	FSlateApplication::Get().AddWindow(win.ToSharedRef());
	//win->BringToFront(true);


	widget.Get()->divide_image_button.BindLambda([this](UTexture2D* t) {
		divide_image = DivideArea(t);
		divide_image = t;
		});

	widget.Get()->sub_area_combobox.BindLambda([this](FString subarea) {
		//current_info = i;
		for (auto i : infos)
		{
			if (i.subarea == subarea)
				current_info = i;
		}
		});

	widget.Get()->fill_image_button.BindLambda([this](UTexture2D* t) {
		current_info.fill_image = t;
		FillArea(current_info);
		});

	widget.Get()->distribute_spinbox.BindLambda([this](FString category, double value) {
		int a = 1;
		*current_info.textrue_para.Find(category) = value;
		});

	widget.Get()->transform_spinbox.BindLambda([this](FString category, double value) {
		*current_info.mesh_para.Find(category) = value;
		});
	
	widget.Get()->calculate_texture_button.BindLambda([this]() {
		CalculateTexture(current_info);

		


		//FString MaterialPath = TEXT("Material'/Game/DivideImage/0001.0001'");
		//material = LoadObject<UMaterialInterface>(nullptr, *MaterialPath);
		});
}

void AInteractiveActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	FSlateApplication::Get().RequestDestroyWindow(win.ToSharedRef());
	win.Reset();
}

// Called every frame
void AInteractiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
}

UTexture2D* AInteractiveActor::DivideArea(UTexture2D* t)
{
	//�˴���opencv�Դ���t������ɫ���࣬��ɺ󷵻��������飬�ֱ�Ϊlabel�����Ϣ��int�������ر���RGB
	//SubAreaInfo����label�����ݵ��������

	//�˴���Ҫ��ʼ��infos����Ҫ��kmeans����������Ϊue�е�texture2D������

	//ԭͼ��Mat
	cv::Mat src;// = UTexture2D; //��UE����ת���� cv::Mat
	int clusterCount = 4;  //��Ҫ������ĸ�����Ϊ4


	int width = src.cols;
	int height = src.rows;
	int dims = src.channels();

	// ��ʼ������
	int sampleCount = width * height;  //�����ص�
	cv::Mat points(sampleCount, dims, CV_32F, cv::Scalar(0));
	//points(���ص�����������ص����ھ�����������������ͣ���ɫ)
	// ���� cols = ���ص��������rows = ���ص����ھ������������ = 3����ʹ��rgb����ͨ������
	//Scalar�ǽ�ͼ�����óɵ�һ�ҶȺ���ɫ
	//���磺Scalar(0)Ϊ��ɫ��Scalar(255)Ϊ��ɫ��Scalar(0, 255, 0)Ϊ��ɫ
	cv::Mat labels; //�洢���������id����������Ϊ int 
	cv::Mat centers(clusterCount, 1, points.type());  //�����洢���������ĵ�

	// ��ͼƬ����ͨ��RBG����ת�����������У����������ظ�����ֵ
	int index = 0;
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			index = row * width + col;  //Ϊÿ�����ص㸳����ֵ��һ��һ�еĽ���
			cv::Vec3b bgr = src.at<cv::Vec3b>(row, col);// 8U ���͵� RGB ��ɫͼ�����ʹ�� <Vec3b>
			points.at<float>(index, 0) = static_cast<int>(bgr[0]);  //ͨ����B����
			points.at<float>(index, 1) = static_cast<int>(bgr[1]);
			points.at<float>(index, 2) = static_cast<int>(bgr[2]);
			//cv::mat�ĳ�Ա������ .at(int y�� int x)����������ȡͼ���ж�Ӧ����Ϊ��x��y����Ԫ�����ꡣ
		}
	}

	// ����K-Means
	cv::TermCriteria criteria = cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 10, 0.1);

		// 1��TermCriteria::COUNT ������������
		// 2��TermCriteria::EPS Ҫ���������ֵ��
		// 3��TermCriteria::COUNT + TermCriteria::EPS �ﵽ2������֮һ���ɡ�
		// maxCount����������������
		// epsilon����Ҫ���������ֵ��
	cv::kmeans(points, clusterCount, labels, criteria, 3, cv::KMEANS_PP_CENTERS, centers);

	// ��ʾͼ��ָ������������зֺ�������ظ�ֵ��resultͼƬ����ͨ����ֵ
	cv::Mat result(sampleCount, dims, CV_32S); //����������ԭ��ͼƬ�Ķ�ά����
	for (int row = 0; row < height; row++)
	{
		for (int col = 0; col < width; col++)
		{
			result.at<int>(row,col) = labels.at<int>(row * width + col, 0);
		}
	}

	//��д�����ȴ��������벹ȫ
	SubAreaInfo info1;
	info1.id = 0;
	info1.subarea = FString("SubArea 1");
	infos.Add(info1);

	return nullptr;
	

	
	//SubAreaInfo info2;
	//info1.subarea = FString("SubArea 2");
	//infos.Add(info2);

}

void AInteractiveActor::FillArea(SubAreaInfo info)
{
	//��currentinfo.fillimage��divide_image�󽻣��õ�������ͼinfo.samples_texture

}

void AInteractiveActor::CalculateTexture(SubAreaInfo info)
{
	//ͨ��info�и��ֲ�����info.samples_texture������������scatter_texture
	scatter_texture = divide_image;
}

UTexture2D* AInteractiveActor::LoadTexture2D(const FString path)
{
	UTexture2D* Texture = nullptr;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path))
	{
		return nullptr;
	}
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *path))
	{
		return nullptr;
	}
	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(path);
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray<uint8> UncompressedRGBA;
		if (ImageWrapper->GetRaw(ERGBFormat::RGBA, 8, UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);
			if (Texture != nullptr)
			{
				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}
	return Texture;
}

TArray<FString> AInteractiveActor::GetFolderFiles(FString path)
{
	TArray<FString> files;
	FPaths::NormalizeDirectoryName(path);
	IFileManager& FileManager = IFileManager::Get();
	FString FinalPath = path / TEXT("*");
	FileManager.FindFiles(files, *FinalPath, true, true);
	return files;
}

TArray<UTexture2D*> AInteractiveActor::GetAllImageFromFiles(FString Paths)
{
	TArray<FString> ImgPaths = GetFolderFiles(Paths);
	TArray<UTexture2D*> Texture2DArr;
	for (auto path : ImgPaths)
	{
		UTexture2D* Texture2D = LoadTexture2D(Paths + "/" + path);
		Texture2DArr.Add(Texture2D);
	}
	return Texture2DArr;
}

TSharedPtr<class IImageWrapper> AInteractiveActor::GetImageWrapperByExtention(const FString Path)
{
	IImageWrapperModule& module = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (Path.EndsWith(".png"))
	{
		return module.CreateImageWrapper(EImageFormat::PNG);
	}
	if (Path.EndsWith(".jpg"))
	{
		return module.CreateImageWrapper(EImageFormat::JPEG);
	}
	return nullptr;
}
