// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.


#include "Card.h"

#include "Net/UnrealNetwork.h"

// Sets default values
ACard::ACard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root"));
	CardFront = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CardFront"));
	CardFront->SetupAttachment(GetRootComponent());
	CardFront->SetIsReplicated(true);
	CardBack = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("CardBack"));
	CardBack->SetupAttachment(GetRootComponent());
	CardBack->SetIsReplicated(true);
}

void ACard::MyInitialize()
{/*
	
	if (HasAuthority())
	{
		SetActorLabel(FString::Printf(TEXT("Card Symbol = %d, Power = %d"), CardSymbol, CardPower));
		if(UWorld* World = GetWorld())
		{
			float Delay = 5.f;
			FTimerHandle TimerHandle;
			World->GetTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateWeakLambda(this, [this]
			{
				UE_LOG(LogTemp, Display, TEXT("Timer Fired"));
			}),Delay,false);
		}
	}*/
}

void ACard::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


// Called when the game starts or when spawned
void ACard::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

