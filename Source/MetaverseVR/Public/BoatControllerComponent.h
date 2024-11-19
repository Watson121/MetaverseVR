// Fill out your copyright notice in the Description page of Project Settings.

/*

This component is a boat controller component. Attach it to your boat, and adjust the settings
so you can have a drivable boat. 

*/

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoatControllerComponent.generated.h"

USTRUCT()
struct FBuoyancyPoint {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector localOffset;

	UPROPERTY(EditAnywhere)
	float radius = 50.0f;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAVERSEVR_API UBoatControllerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBoatControllerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetInputs(float forward, float turn);

private:

#pragma region Bouyancy Settings

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	float fluidDensity = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	float gravity = 980.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	float dampingCoefficient = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	float dragCoefficient = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	float angularDampingCoefficient = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	float correctiveForceMultiplier = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	float submergedVolume = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	TArray<FBuoyancyPoint> buoyancyPoints;

	UPROPERTY(EditAnywhere, Category = "Buoyancy Settings")
	float waterLevel = 0.0f;

#pragma endregion

	UPROPERTY()
	float forwardInput = 0.0f;

	UPROPERTY()
	float turnInput = 0.0f;

#pragma region Speed Settings

	UPROPERTY(EditAnywhere, Category = "Speed Settings")
	float forwardForceMultiplier = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Speed Settings")
	float reverseForceMultiplier = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Speed Settings")
	float maxForwardSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Speed Settings")
	float turnTorqueMultiplier = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Speed Settings")
	float maxTurningSpeed = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Speed Settings")
	float laterialDampingMultiplier = 10.0f;

#pragma endregion




	UStaticMeshComponent* MeshComp;
	
	UFUNCTION()
	void ApplyBuoyancyAndMovement();
	
	UFUNCTION()
	void ApplyForwardMovement(const FVector& forwardDirection, const FVector& velocity);

	UFUNCTION()
	void ApplyTurningMovement(const FVector& velocity);

	UFUNCTION()
	void ApplyBuoyancy(float MaxBuoyantForce);

	UFUNCTION()
	float CalculateMaxBuoyantForce();

	UFUNCTION()
	FVector ApplyCorrectiveForce(FVector ActorLocation, FVector Velocity);

	UFUNCTION()
	void ApplyDamping();

	UFUNCTION()
	void ApplyLaterialDamping(const FVector& velocity, const FVector& forwardDirection, const FVector& rightDirection);

	UFUNCTION()
	float CalculateSubmersionDepth(const FVector& WorldLocation, float Radius);
		
};
