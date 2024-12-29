// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Card.h"

#include "CasinoPlayer.generated.h"

UCLASS()
class BERKUSHSPLAYGROUND_API ACasinoPlayer : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACasinoPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	TArray<ACard*> PlayersHand;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Casino)
	bool bIsNormalGuy = false;

};
