#include "Scatter/Public/ScatterRendering.h"
#include "Engine/TextureRenderTarget2D.h"

#include "PipelineStateCache.h"

#include "GlobalShader.h"

#include "RenderGraphUtils.h"
#include "RenderTargetPool.h"
#include "RHIStaticStates.h"
#include "ShaderParameterUtils.h"
#include "PixelShaderUtils.h"

namespace ScatterRendering
{
	// /*
	//  * Vertex Resource
	//  */
	// TGlobalResource<FTextureVertexDeclaration> GTextureVertexDeclaration;
	// TGlobalResource<FRectangleVertexBuffer> GRectangleVertexBuffer;
	// TGlobalResource<FRectangleIndexBuffer> GRectangleIndexBuffer;

	class FSimpleRDGComputeShader : public FGlobalShader
	{
	public:
		DECLARE_GLOBAL_SHADER(FSimpleRDGComputeShader);
		SHADER_USE_PARAMETER_STRUCT(FSimpleRDGComputeShader, FGlobalShader);

		BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			SHADER_PARAMETER_STRUCT_REF(FSimpleUniformStructParameters, SimpleUniformStruct)
			SHADER_PARAMETER_TEXTURE(Texture2D<float4>, KMTexture)
			SHADER_PARAMETER_TEXTURE(Texture2D<float4>, InTexture)
			SHADER_PARAMETER_SAMPLER(SamplerState, KMTextureSampler)
			SHADER_PARAMETER_SAMPLER(SamplerState, InTextureSampler)
			SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, OutTexture)
			END_SHADER_PARAMETER_STRUCT()

		static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Parameters)
		{
			return true; // 
		}
	};

	IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FSimpleUniformStructParameters, "SimpleUniformStruct");
	IMPLEMENT_GLOBAL_SHADER(FSimpleRDGComputeShader, "/Scatter/Private/Scatter.usf", "MainCS", SF_Compute);
	/*
 * Render Function
 */
	void RDGCompute(FRHICommandListImmediate& RHIImmCmdList, FTexture2DRHIRef KMTexture, FTexture2DRHIRef InAreaTexture, FTexture2DRHIRef RenderTargetRHI, FSimpleShaderParameter InParameter)
	{
		check(IsInRenderingThread());

		//Create RenderTargetDesc
		const FRDGTextureDesc& RenderTargetDesc = FRDGTextureDesc::Create2D(RenderTargetRHI->GetSizeXY(), RenderTargetRHI->GetFormat(), FClearValueBinding::Black, TexCreate_RenderTargetable | TexCreate_ShaderResource | TexCreate_UAV);

		TRefCountPtr<IPooledRenderTarget> PooledRenderTarget;

		//RDG Begin
		FRDGBuilder GraphBuilder(RHIImmCmdList);
		FRDGTextureRef RDGRenderTarget = GraphBuilder.CreateTexture(RenderTargetDesc, TEXT("RDGRenderTarget"));

		//Setup uniform Parameters
		FSimpleUniformStructParameters StructParameters;
		StructParameters.SelectColor = InParameter.SelectColor;

		FSimpleRDGComputeShader::FParameters* Parameters = GraphBuilder.AllocParameters<FSimpleRDGComputeShader::FParameters>();
		FRDGTextureUAVDesc UAVDesc(RDGRenderTarget);
		Parameters->SimpleUniformStruct = TUniformBufferRef<FSimpleUniformStructParameters>::CreateUniformBufferImmediate(StructParameters, UniformBuffer_SingleFrame);
		Parameters->KMTexture = KMTexture;
		Parameters->InTexture = InAreaTexture;
		Parameters->OutTexture = GraphBuilder.CreateUAV(UAVDesc);

		//Get ComputeShader From GlobalShaderMap
		const ERHIFeatureLevel::Type FeatureLevel = GMaxRHIFeatureLevel; //ERHIFeatureLevel::SM5
		FGlobalShaderMap* GlobalShaderMap = GetGlobalShaderMap(FeatureLevel);
		TShaderMapRef<FSimpleRDGComputeShader> ComputeShader(GlobalShaderMap);

		//Compute Thread Group Count
		FIntVector ThreadGroupCount(
			RenderTargetRHI->GetSizeX() / 32,
			RenderTargetRHI->GetSizeY() / 32,
			1);

		//ValidateShaderParameters(PixelShader, Parameters);
		//ClearUnusedGraphResources(PixelShader, Parameters);

		GraphBuilder.AddPass(
			RDG_EVENT_NAME("RDGCompute"),
			Parameters,
			ERDGPassFlags::Compute,
			[Parameters, ComputeShader, ThreadGroupCount](FRHICommandList& RHICmdList) {
				FComputeShaderUtils::Dispatch(RHICmdList, ComputeShader, *Parameters, ThreadGroupCount);
			});

		GraphBuilder.QueueTextureExtraction(RDGRenderTarget, &PooledRenderTarget);
		GraphBuilder.Execute();

		//Copy Result To RenderTarget Asset
		RHIImmCmdList.CopyTexture(PooledRenderTarget->GetRHI()->GetTexture2D(), RenderTargetRHI->GetTexture2D(), FRHICopyTextureInfo());
		//RHIImmCmdList.CopyToResolveTarget(PooledRenderTarget->GetRenderTargetItem().ShaderResourceTexture, RenderTargetRHI->GetTexture2D(), FResolveParams());
	}

}

void USimpleRenderingExampleBlueprintLibrary::UseRDGCompute(const UObject* WorldContextObject,UTexture2D* KMTexture, UTexture2D* InAreaTexture, UTextureRenderTarget2D* OutputRenderTarget, FSimpleShaderParameter Parameter)
{
	check(IsInGameThread());

	FTexture2DRHIRef RenderTargetRHI = OutputRenderTarget->GameThread_GetRenderTargetResource()->GetRenderTargetTexture();
	FTexture2DRHIRef KMTextureRHI = KMTexture->GetResource()->TextureRHI->GetTexture2D();
	FTexture2DRHIRef InTextureRHI = InAreaTexture->GetResource()->TextureRHI->GetTexture2D();
	ENQUEUE_RENDER_COMMAND(CaptureCommand)
		(
			[RenderTargetRHI, KMTextureRHI, InTextureRHI, Parameter](FRHICommandListImmediate& RHICmdList) {
				ScatterRendering::RDGCompute(RHICmdList, KMTextureRHI , InTextureRHI, RenderTargetRHI, Parameter);
			});
}

