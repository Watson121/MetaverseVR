// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuoyancyComponent.generated.h"

USTRUCT()
struct FBuoyancyPoint {

	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FVector localOffset;

	UPROPERTY(EditAnywhere)
	float radius = 50.0f;

};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAVERSEVR_API UBuoyancyComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBuoyancyComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetInputs(float forward, float turn);

private:

	UPROPERTY(EditAnywhere, Category = "Buoyancy")
	float fluidDensity = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy")
	float gravity = 980.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy")
	float dampingCoefficient = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy")
	float dragCoefficient = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy")
	float angularDampingCoefficient = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy")
	TArray<FBuoyancyPoint> buoyancyPoints;

	UPROPERTY(EditAnywhere, Category = "Buoyancy")
	float waterLevel = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Buoyancy")
	float correctiveForceMultiplier = 1000.0f;

	UPROPERTY()
	float forwardInput = 0.0f;

	UPROPERTY()
	float turnInput = 0.0f;

	UStaticMeshComponent* MeshComp;

	void ApplyBuoyancyAndMovement();
	void ApplyBuoyancy();
	float CalculateSubmersionDepth(const FVector& WorldLocation, float Radius);
		
};
