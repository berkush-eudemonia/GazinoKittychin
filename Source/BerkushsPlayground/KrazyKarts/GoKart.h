// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GoKartMovingComponent.h"
#include "GoKartMovementReplicator.h"

#include "GoKart.generated.h"

UCLASS()
class BERKUSHSPLAYGROUND_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** Called for movement input */
	void MoveKart(const FInputActionValue& Value);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGoKartMovingComponent* MovingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGoKartMovementReplicator* MovementReplicator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UBoxComponent* BoxComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USceneComponent* MeshOffsetRootComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USkeletalMeshComponent* SkeletalMeshComponent;
};
