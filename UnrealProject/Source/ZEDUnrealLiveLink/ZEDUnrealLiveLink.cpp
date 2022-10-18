// Fill out your copyright notice in the Description page of Project Settings.

#include "ZEDUnrealLiveLink.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, ZEDUnrealLiveLink, "ZEDUnrealLiveLink" );

//IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, MyGame, "MyGame" );
class FZEDUnrealLiveLinkModule : public FDefaultGameModuleImpl
{
#if    WITH_EDITOR
	virtual void StartupModule() override
	{
		FModuleManager::Get().LoadModule(TEXT("BlueprintGraph"));
		FModuleManager::Get().LoadModule(TEXT("AnimGraph"));
		FModuleManager::Get().LoadModule(TEXT("ZEDUnrealLiveLinkEditor"));
	}
#endif
};