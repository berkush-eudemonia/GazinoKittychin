// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.


#include "LobbyGameMode.h"

#include "TimerManager.h"

#include "MultiplayerGameInstance.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	++NumberOfPlayers;
/*
	if (NumberOfPlayers >= 2)
	{
		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ALobbyGameMode::StartGame,10.f, false);
	}
*/
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);
	--NumberOfPlayers;
}

void ALobbyGameMode::StartGame()
{
	auto GameInstance = Cast<UMultiplayerGameInstance>(GetGameInstance());

	if (GameInstance == nullptr) return;

	GameInstance->StartSession();
	
	UWorld* World = GetWorld();
	if (!ensure(World!=nullptr)) return;

	bUseSeamlessTravel = true;
	World->ServerTravel("/Game/KrayzKarts/VehicleExampleMap?listen");
}