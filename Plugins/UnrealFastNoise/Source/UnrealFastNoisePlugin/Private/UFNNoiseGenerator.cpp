#include "UFNNoiseGenerator.h"


UUFNNoiseGenerator::UUFNNoiseGenerator(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

float UUFNNoiseGenerator::GetNoise2D(float aX, float aY)
{
	return -2.0f;
}

float UUFNNoiseGenerator::GetNoise3D(float aX, float aY, float aZ)
{
	return -2.0f;
}

float UUFNNoiseGenerator::GetNoise2DInRange(float aX, float aY, float min, float max)
{
	return (max - min)*((GetNoise2D(aX, aY) + 1.0f) * 0.5f) + min;
}

float UUFNNoiseGenerator::GetNoise3DInRange(float aX, float aY, float aZ, float min, float max)
{
	return (max - min)*((GetNoise3D(aX, aY, aZ) + 1.0f) * 0.5f) + min;
}
