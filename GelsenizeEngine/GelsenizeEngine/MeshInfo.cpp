#include "MeshInfo.h"

MeshInfo::MeshInfo()
{
	this->ID = 0;
	this->name = "NOT_SET";
	this->textureName_0 = "NOT_SET";
	this->textureName_1 = "NOT_SET";
	this->textureName_2 = "NOT_SET";
	this->textureName_3 = "NOT_SET";
	this->usedTextureNum = -1;

	//animationTime = 0.0f;
	//animationTimeMultiplier = 1.0f;
	//animationTimeIncrement = 0.01f;
	//animationTimeMax = 1.0f;
	this->isASkinnedMesh = false;
}

MeshInfo::~MeshInfo()
{
}