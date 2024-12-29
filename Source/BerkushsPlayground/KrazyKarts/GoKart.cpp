// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.

#include "GoKart.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/BoxComponent.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
    if (BoxComponent==nullptr) return;
	SetRootComponent(BoxComponent);
	MeshOffsetRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("MeshOffsetComponent"));
	if (MeshOffsetRootComponent==nullptr) return;
	MeshOffsetRootComponent->SetupAttachment(GetRootComponent());
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
	if (SkeletalMeshComponent==nullptr) return;
	SkeletalMeshComponent->SetupAttachment(MeshOffsetRootComponent);
	
	MovingComponent = CreateDefaultSubobject<UGoKartMovingComponent>(TEXT("MovingComponent"));
	MovementReplicator = CreateDefaultSubobject<UGoKartMovementReplicator>(TEXT("MovementReplicator"));
	if (MovementReplicator==nullptr) return;
	MovementReplicator->SetMeshOffsetRoot(MeshOffsetRootComponent);
	MovementReplicator->SetIsReplicated(true);
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		NetUpdateFrequency = 3; //Saniyedeki yenelinme miktari
	}

	SetReplicateMovement(false); //Begin playe yazmaliymisiz da boyle olunca bp default value olarak gormuyor amk
}

// Called every frame
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	DrawDebugString(GetWorld(), FVector(0,0,100), UEnum::GetValueAsString(GetLocalRole()),this, FColor::White, DeltaTime);
}

// Called to bind functionality to input
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		//EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGoKart::MoveKart);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AGoKart::MoveKart);

		// Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMultiplayerCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GoKart sinifinin Enhanced InputComponent'i hatali!"));
	}
}

void AGoKart::MoveKart(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();
	/*
	//Legacy yoldan cozmeye calisacagim
	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
	*/
	if (MovingComponent == nullptr) return;
	
	MovingComponent->SetThrottle(MovementVector.Y);
	MovingComponent->SetSteeringThrow(MovementVector.X);
}