// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NameTypes.h"
#include "PhysicalData.generated.h"

UENUM(BlueprintType)
enum class ELEDColor : uint8
{
	Off,
	White,
	Red,
	Green,
	Blue,
	Purple,
	Orange,
	Yellow,
	Aqua,
	Pink,
};


UCLASS(BlueprintType, Category = "Physical Data", meta = (Keywords = "device physical data magic"))
class LONGTUNNEL_API UPhysicalData: public UObject
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Device Connection"), Category = "Physical Data", meta = (Keywords = "device connection magic"))
		static bool DeviceConnection(FString port);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Data Object"), Category = "Physical Data", meta = (Keywords = "device data object magic"))
		static void DataObject(FString &data);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Device Conclude"), Category = "Physical Data", meta = (Keywords = "data serial magic"))
		static void DeviceConclude();

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Device Output"), Category = "Physical Data", meta = (Keywords = "data serial magic"))
		static void DeviceOutput(FString value);


	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Raw Data"), Category = "Physical Data", meta = (Keywords = "data serial magic"))
		static void RawData(const int port, const FString data, FString &portData);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Orientation"), Category = "Physical Data", meta = (Keywords = "data serial magic"))
		static void Orientation(const FString data, float &xAxis, float &yAxis, float &zAxis);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Button Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static bool Button(const int port, const FString data, int &state);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Joystick Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static bool Joystick(const int port, const FString data, int& xAxis, int& yAxis);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Light Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static bool Light(const int port, const FString data, int& brightness);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Distance Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static bool Distance(const int port, const FString data, float& distance);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Proximity Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static bool Proximity(const int port, const FString data, int& proximity);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Gesture Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static bool Gesture(const int port, const FString data, FString& direction);


	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Tone Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static FString Tone(int port, int pitch);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "Servo Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static FString Servo(int port, int degree);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "LED Module"), Category = "Physical Data - Modules", meta = (Keywords = "data serial magic"))
		static FString LED(int port, ELEDColor color);
};
