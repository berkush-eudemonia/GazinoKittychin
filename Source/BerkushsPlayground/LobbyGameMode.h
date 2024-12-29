// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerGameMode.h"
#include "LobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class BERKUSHSPLAYGROUND_API ALobbyGameMode : public AMultiplayerGameMode
{
	GENERATED_BODY()

public:

	void PostLogin(APlayerController* NewPlayer) override;

	void Logout(AController* Exiting) override;

	UFUNCTION(BlueprintCallable)
	void StartGame();
	
private:

	FTimerHandle GameStartTimer;

	uint32 NumberOfPlayers = 0;
	
};
//push comment