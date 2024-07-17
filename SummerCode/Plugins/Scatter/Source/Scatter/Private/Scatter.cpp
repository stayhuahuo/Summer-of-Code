// Copyright Epic Games, Inc. All Rights Reserved.

#include "Scatter.h"

#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FScatterModule"

void FScatterModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	FString PluginShaderDir = FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("Scatter"))->GetBaseDir(), TEXT("Shaders"));
	// Assuming you want to print the PluginShaderDir to the output log
	// printf("Plugin Shader Directory: %s", *PluginShaderDir);
	UE_LOG(LogTemp, Warning, TEXT("Plugin Shader Directory: %s"), *PluginShaderDir);
	AddShaderSourceDirectoryMapping(TEXT("/ScatterShaders"), PluginShaderDir);
}

void FScatterModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FScatterModule, Scatter)