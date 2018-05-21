#ifndef _MESHINFO_HG_
#define _MESHINFO_HG_

#include "GelStructures.h"
#include <string>
#include "AnimationController.h"

class MeshInfo
{
public:
	MeshInfo();
	~MeshInfo();

	
	int ID;				     	// Which model we are drawing
	int usedTextureNum;
	std::string name;		// "bunny"
	std::string textureName_0;
	std::string textureName_1;
	std::string textureName_2;
	std::string textureName_3;
	
	//add animation controller structure
	//std::string mainAnimationName;
	//float animationTime;
	//float animationTimeMultiplier;
	//float animationTimeIncrement;
	//float animationTimeMax;

	AnimationController *animationController;

	bool isASkinnedMesh;
	
	//cSimpleAssimpSkinnedMesh* skinnedMesh;
private:

};



#endif // !_MESHINFO_HG_
