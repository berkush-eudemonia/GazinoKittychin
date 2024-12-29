// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.


#include "CasinoGamemode.h"

#include "BerkushsPlayground/MultiplayerGameInstance.h"
//variable = (condition) ? expressionTrue : expressionFalse;


ACasinoGamemode::ACasinoGamemode()
{
	static ConstructorHelpers::FClassFinder<APawn> VRPawnBPClass(TEXT("/Game/KrayzKarts/Blueprints/BP_GoKart"));
	static ConstructorHelpers::FClassFinder<APawn> RegularPawnBPClass(TEXT("/Game/KrayzKarts/Blueprints/BP_GoKart"));

	if (VRPawnBPClass.Class != NULL)
	{
		PlayerClassVR = VRPawnBPClass.Class;
	}
	if (RegularPawnBPClass.Class != NULL)
	{
		PlayerClass = RegularPawnBPClass.Class;
	}
}

void ACasinoGamemode::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = NewPlayer;
	//SpawnInfo.Instigator = GetInstigator();
	SpawnInfo.Name = *NewPlayer->GetName();
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	bool bIsVrPlayer = Cast<UMultiplayerGameInstance>(NewPlayer->GetGameInstance())->bIsVRPlayer;
	ACasinoPlayer* CurrentPlayer = Cast<ACasinoPlayer>(GWorld->SpawnActor(
		(bIsVrPlayer) ? PlayerClassVR : PlayerClass, &PlayerSpawnPositionActor->GetTransform(), SpawnInfo));
	NewPlayer->Possess(CurrentPlayer);
}
