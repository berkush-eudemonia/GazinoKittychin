// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameInstance.h"

#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSessionSettings.h"


#include "MenuSystem/MainMenu.h"
#include "MenuSystem/MenuWidget.h"
#include "Online/OnlineSessionNames.h"

const static FName SESSION_NAME = NAME_GameSession; //Fixing NULL subsytem
const static FName SERVER_NAME_SETTINGS_KEY = TEXT("ServerName");

UMultiplayerGameInstance::UMultiplayerGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
	if (!ensure(MenuBPClass.Class != nullptr)) return;

	MenuClass = MenuBPClass.Class;
	
	ConstructorHelpers::FClassFinder<UUserWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
	if (!ensure(InGameMenuBPClass.Class != nullptr)) return;

	InGameMenuClass = InGameMenuBPClass.Class;
}

void UMultiplayerGameInstance::Init()
{
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());

		SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UMultiplayerGameInstance::OnJoinSessionComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Found no subsystem!"));
		return;
	}

	if (GEngine!=nullptr)
	{
		GEngine->OnNetworkFailure().AddUObject(this, &UMultiplayerGameInstance::OnNetworkFailure);
	}
}

void UMultiplayerGameInstance::LoadMenuWidget()
{
	if (!ensure(MenuClass != nullptr)) return;
	
	Menu = CreateWidget<UMainMenu>(this, MenuClass);
	if (!ensure(Menu != nullptr)) return;

	Menu->Setup();
	
	Menu->SetMenuInterface(this);
}

void UMultiplayerGameInstance::InGameLoadMenu()
{
	//MenuWidget was -> Menu, rename it cause by fucking msvc!
	if (!ensure(InGameMenuClass != nullptr)) return;
	
	UMenuWidget* MenuWidget = CreateWidget<UMenuWidget>(this, InGameMenuClass);
	if (!ensure(MenuWidget != nullptr)) return;

	MenuWidget->Setup();
	
	MenuWidget->SetMenuInterface(this);
}

void UMultiplayerGameInstance::Host(FString ServerName)
{
	DesiredServerName = ServerName;
	if (SessionInterface.IsValid())
	{
		auto ExistingSession = SessionInterface->GetNamedSession(SESSION_NAME);
		if (ExistingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
}

void UMultiplayerGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		CreateSession();
	}
}

void UMultiplayerGameInstance::OnNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	LoadMainMenu();
}

void UMultiplayerGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100;
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Warning, TEXT("Starting Find Session"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UMultiplayerGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	if (bWasSuccessful && SessionSearch.IsValid() && Menu !=  nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Finished Find Session"));

		TArray<FServerData> ServerDatas;
		for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session Named: %s"), *SearchResult.GetSessionIdStr());
			FServerData ServerData;
			ServerData.Name = SearchResult.GetSessionIdStr();
			ServerData.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
			ServerData.CurrentPlayers = ServerData.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
			ServerData.HostUserName = SearchResult.Session.OwningUserName;
			
			FString ServerName;
			if (SearchResult.Session.SessionSettings.Get(SERVER_NAME_SETTINGS_KEY, ServerName))
			{
				ServerData.Name = ServerName;
			}
			else
			{
				ServerData.Name = TEXT("Could not find name.");
			}
			
			ServerDatas.Add(ServerData);
		}

		Menu->SetServerList(ServerDatas);
	}
}

void UMultiplayerGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings SessionSettings;
		
		SessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
		SessionSettings.NumPublicConnections = 10;
		SessionSettings.bShouldAdvertise = true;
		SessionSettings.bUsesPresence = true;
		SessionSettings.bUseLobbiesIfAvailable = true;

		SessionSettings.Set(SERVER_NAME_SETTINGS_KEY, DesiredServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
		
		SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
	}
}

void UMultiplayerGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if(!bWasSuccessful)
	{
		UE_LOG(LogTemp, Error, TEXT("Could not create session!"));
		return;
	}
	if (Menu!=nullptr)
	{
		Menu->Teardown();
	}
	
	UEngine* Engine = GetEngine();
	if (!ensure(Engine!=nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, TEXT("Hosting"));

	UWorld* World = GetWorld();
	if (!ensure(World!=nullptr)) return;

	World->ServerTravel("/Game/Maps/Lobby?listen");
}

void UMultiplayerGameInstance::Join(uint32 Index)
{
	if(!SessionInterface.IsValid()) return;
	if(!SessionSearch.IsValid()) return;
	
	if (Menu!=nullptr)
	{
		Menu->Teardown();
	}

	SessionInterface->JoinSession(0,SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UMultiplayerGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!SessionInterface.IsValid()) return;

	FString Address;
	if(!SessionInterface->GetResolvedConnectString(SessionName, Address))
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not get connect string"));
		return;
	}

	UEngine* Engine = GetEngine();
	if (!ensure(Engine!=nullptr)) return;

	Engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, FString::Printf(TEXT("Joining %s"), *Address));

	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController!=nullptr)) return;
	PlayerController->ClientTravel(Address,TRAVEL_Absolute);
}

void UMultiplayerGameInstance::StartSession()
{
	if(!SessionInterface.IsValid()) return;

	SessionInterface->StartSession(SESSION_NAME);
}

void UMultiplayerGameInstance::LoadMainMenu()
{
	APlayerController* PlayerController = GetFirstLocalPlayerController();
	if (!ensure(PlayerController!=nullptr)) return;
	PlayerController->ClientTravel("/Game/MenuSystem/MainMenu",TRAVEL_Absolute);
}