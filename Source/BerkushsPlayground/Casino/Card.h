// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "PaperSprite.h"
#include "PaperSpriteComponent.h"
#include "Engine/DataTable.h"

#include "Card.generated.h"

USTRUCT(BlueprintType)
struct FCardSprites : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** montage **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSprite* Sprite;
};

UCLASS()
class BERKUSHSPLAYGROUND_API ACard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACard();

	UFUNCTION(BlueprintCallable)
	virtual void MyInitialize();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta =(ExposeOnSpawn = true))
	uint8 CardSymbol;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta =(ExposeOnSpawn = true))
	uint8 CardPower;

	UPROPERTY(BlueprintReadOnly)
	int32 ShowFrontFace = 0; //0 = No One, 1 = Owner, 2 = Desk Card (Everyone)
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly)
	UPaperSprite* CardFrontSprite;
	UPROPERTY(BlueprintReadOnly)
	UPaperSprite* CardBackSprite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* CardFront;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperSpriteComponent* CardBack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UDataTable* SpriteTable;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
