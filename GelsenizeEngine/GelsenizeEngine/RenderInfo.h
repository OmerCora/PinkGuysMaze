#ifndef _RENDERINFO_HG_
#define _RENDERINFO_HG_

#include "externals.h"

#include "GelStructures.h"

class RenderInfo
{
public:
	RenderInfo();
	~RenderInfo();

	bool useDebugColorsNoLighting;
	bool isDrawn;
	bool isWireframe;
	bool isAABBDrawn;
	bool useTextureAsDiffuse;
	bool discardTextureBlack;

	std::string shaderProgramName;

	glm::mat4x4 transform;

	glm::vec4 color;
	float alpha;
	void ToggleColor(glm::vec4 color);
	void ResetToggleColor(void);

	GelRotationPre rotationPre;
	//GelRotation rotation;
	GelOrientation orientation; //glm::quat

	GelPosition position;
	GelPosition previousPosition;
	GelScale scale; //float
	GelScale lastDrawnActualScale;

	//bloom
	bool hasGlow;
	glm::vec4 glowColor;

	bool exploding;
	float explodeTime; // 0.0f is inactive on shader logic, if flag is false, 0.0f is set per object

	bool reflectsSkybox;

	float GetRadius(void);
	void SetRadius(float newRadius);
	float AABBwidth;
private:
	float radius;

	glm::vec4 toggle_color;
};

#endif // !_RENDERINFO_HG_
