// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.

#pragma once

#include "CoreMinimal.h"
#include "CasinoPlayer.h"
#include "BerkushsPlayground//MultiplayerGameMode.h"
#include "CasinoGamemode.generated.h"

/**
 * 
 */

UCLASS()
class BERKUSHSPLAYGROUND_API ACasinoGamemode : public AMultiplayerGameMode
{
	GENERATED_BODY()

	ACasinoGamemode();

public:

	virtual void OnPostLogin(AController* NewPlayer) override;

	TSubclassOf<ACasinoPlayer> PlayerClass;
	TSubclassOf<ACasinoPlayer> PlayerClassVR;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* PlayerSpawnPositionActor;
};
