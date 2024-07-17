#pragma once

#include "RenderGraph.h"

//BlueprintLibraryHeader
#include "CoreMinimal.h"
#include "DataDrivenShaderPlatformInfo.h"
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


	// BEGIN_GLOBAL_SHADER_PARAMETER_STRUCT(FSimpleUniformStructParameters, )
	// 	SHADER_PARAMETER(FVector4f, SelectColor)
	// END_GLOBAL_SHADER_PARAMETER_STRUCT()
	
class FSimpleRDGComputeShader : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FSimpleRDGComputeShader);
	SHADER_USE_PARAMETER_STRUCT(FSimpleRDGComputeShader, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		// SHADER_PARAMETER_STRUCT_REF(FSimpleUniformStructParameters, SimpleUniformStruct)
		SHADER_PARAMETER(FVector4f, SelectColor)
		// SHADER_PARAMETER_TEXTURE(Texture2D<float4>, KMTexture)
		// SHADER_PARAMETER_TEXTURE(Texture2D<float4>, InTexture)
		// SHADER_PARAMETER_SAMPLER(SamplerState, KMTextureSampler)
		// SHADER_PARAMETER_SAMPLER(SamplerState, InTextureSampler)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutTexture)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM6);; // 
	}
};


/////////////////////// RDG Method /////////////////////////////
void RDGCompute(FRHICommandListImmediate& RHIImmCmdList, FTexture2DRHIRef RenderTargetRHI, FLinearColor SelectColor);
// void RDGCompute(FRHICommandListImmediate& RHIImmCmdList, FTexture2DRHIRef KMTexture, FTexture2DRHIRef InAreaTexture, FTexture2DRHIRef RenderTargetRHI, FSimpleShaderParameter InParameter);