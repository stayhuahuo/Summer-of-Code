#pragma once

#include "RenderGraph.h"

//BlueprintLibraryHeader
#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "ScatterRendering.generated.h"

class FRHICommandListImmediate;
struct IPooledRenderTarget;

USTRUCT(BlueprintType, meta = (ScriptName = "ScatterRenderingExample"))
struct FSimpleShaderParameter
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta = (WorldContext = "WorldContextObject"))
	FLinearColor SelectColor;

};

UCLASS(MinimalAPI, meta = (ScriptName = "ScatterRenderingExample"))
class USimpleRenderingExampleBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "ScatterRenderingExample", meta = (WorldContext = "WorldContextObject"))
	static void UseRDGCompute(const UObject* WorldContextObject, UTexture2D* KMTexture, UTexture2D* InAreaTexture, UTextureRenderTarget2D* OutputRenderTarget, FSimpleShaderParameter Parameter);

};

namespace ScatterRendering
{
	BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FSimpleUniformStructParameters, )
	SHADER_PARAMETER(FVector4f, SelectColor)
	END_GLOBAL_SHADER_PARAMETER_STRUCT()


/////////////////////// RDG Method /////////////////////////////
void RDGCompute(FRHICommandListImmediate& RHIImmCmdList, FTexture2DRHIRef KMTexture, FTexture2DRHIRef InAreaTexture, FTexture2DRHIRef RenderTargetRHI, FSimpleShaderParameter InParameter);
};