// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.


#include "CasinoPlayer.h"

// Sets default values
ACasinoPlayer::ACasinoPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACasinoPlayer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACasinoPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACasinoPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

