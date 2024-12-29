// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "MenuSystem/MenuInterface.h"

#include "MultiplayerGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BERKUSHSPLAYGROUND_API UMultiplayerGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()
	
public:
	UMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void LoadMenuWidget();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void InGameLoadMenu();
	
	UFUNCTION(Exec)
	void Host(FString ServerName) override;
	
	UFUNCTION(Exec)
	void Join(uint32 Index) override;

	void StartSession();

	virtual void LoadMainMenu() override;

	void RefreshServerList() override;
	
	bool bIsVRPlayer = false;
private:
	TSubclassOf<class UUserWidget> MenuClass; //class yazmak forward decklarition oluyor galiba, include u burada yapmadik diye belirttik, normalde gerek yok
	TSubclassOf<class UUserWidget> InGameMenuClass;
	
	class UMainMenu* Menu;

	IOnlineSessionPtr SessionInterface;
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);	
	void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	void OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

	FString DesiredServerName;
	void CreateSession();
	
};
