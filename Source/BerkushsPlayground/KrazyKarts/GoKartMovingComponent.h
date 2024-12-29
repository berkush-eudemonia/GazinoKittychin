// 2024'te R. Berk Sozgen tarafindan gelistirilmeye baslanmistir.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovingComponent.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;
	UPROPERTY()
	float SteeringThrow;
	
	UPROPERTY()
	float DeltaTime;
	UPROPERTY()
	float Time;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BERKUSHSPLAYGROUND_API UGoKartMovingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMovingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SimulateMove(const FGoKartMove& Move);

	FVector GetVelocity() const { return Velocity; };
	void SetVelocity(const FVector& Value) { Velocity = Value; };

	void SetThrottle(const float& Value) { Throttle = Value; };
	void SetSteeringThrow(const float& Value) { SteeringThrow = Value; };

	FGoKartMove GetLastMove() const { return LastMove; };
	
private:

	FGoKartMove CreateMove(float DeltaTime);
	
	FVector GetAirResistance();
	FVector GetRollingResistance();

	void ApplyRotation(float DeltaTime, float _SteeringThrow);
	void UpdateLocationFromVelocity(float DeltaTime);
	
	//The mass of the car (kg).
	UPROPERTY(EditAnywhere)
	float Mass = 1000.f;

	//The force applied to the car when the throttle is fully down (N).
	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000.f;

	//Minimum radius of the car turning circle at full lock (m).
	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10.f;

	//Higher means more drag.
	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16.f;

	//Higher means more Rolling Resistance.
	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015f;

	FVector Velocity;
	
	float Throttle;
	float SteeringThrow;

	class AGameStateBase* GameState;

	bool bIsOwnerLocallyControlled = false;

	FGoKartMove LastMove;
};
