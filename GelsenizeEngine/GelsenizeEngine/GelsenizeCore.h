#ifndef _GELSENIZEMANAGER_HG_
#define _GELSENIZEMANAGER_HG_

#include "externals.h"
#include <string>

#include <map>

//main game loop manager
class GelsenizeCore
{
public:
	//Singleton
	static GelsenizeCore& sharedInstance()
	{
		static GelsenizeCore instance;
		return instance;
	}
	float GetGlobalClock(void);
	float GetPlaygroundScale(void);
	float GetPlayerSpeed(void);
	float GetAttentionRadius(void);
	float GetDangerRadius(void);

	//TODO: awaiting abstraction
	GLFWwindow* GetWindow(void);
	GLuint GetModelLocation(std::string shaderName = "default");
	GLuint GetViewLocation(std::string shaderName = "default");
	GLuint GetProjLocation(std::string shaderName = "default");

	GLuint ObjectAlphaID(std::string shaderName = "default");
	GLuint DiscardTextureBlackID(std::string shaderName = "default");
	GLuint TexSamp2DID(int index, std::string shaderName = "default");
	GLuint UsedTexNumID(std::string shaderName = "default");
	GLuint CubemapSkyBoxSamplerID(std::string shaderName = "default");
	GLuint IsCubeMapObjectID(std::string shaderName = "default");
	GLuint UseLightingID(std::string shaderName = "default");
	GLuint IsImposterBlackIsAlphaID(std::string shaderName = "default");
	GLuint UseTextureAsDiffuseID(std::string shaderName = "default");
	GLuint AlphaID(std::string shaderName = "default");
	GLuint ModelColorID(std::string shaderName = "default");
	GLuint isWireframeID(std::string shaderName = "default");
	GLuint UseGlobalAlphaValue(std::string shaderName = "default");
	GLuint GlobalAlphaValue(std::string shaderName = "default");
	GLuint DebugColorID(std::string shaderName = "default");
	GLuint EyeLocationID(std::string shaderName = "default");
	GLuint TextureExtraID(std::string shaderName = "default");
	GLuint ClothPass(std::string shaderName = "default");
	GLuint BackFaceDrawPassMode(std::string shaderName = "default");
	GLuint BackFaceColor(std::string shaderName = "default");
	GLuint BackFaceOffset(std::string shaderName = "default");

	GLuint IsASkinnedMesh(std::string shaderName = "default");
	GLuint BoneID(int index, std::string shaderName = "default");
	GLuint NumberOfBonesUsed(std::string shaderName = "default");
	GLuint SlideDistance(std::string shaderName = "default");

	GLuint GlowColor();
	GLuint HasGlow();
	GLuint ExplodeTime();
	GLuint ReflectsSkybox();

	void SetAIProps(float val1, float val2, float val3);
	void SetPlaygroundScale(float pgScale) { this->playgroundScale = pgScale; };

	void StartGameLoop(void);

	float mousePositionX;
	float mousePositionY;
private:
	GelsenizeCore() {};
	GelsenizeCore(GelsenizeCore const&); //dont implement - to build
	void operator=(GelsenizeCore const&); //dont implement - to build
	
	void LoadGameObjects(void);

	void SetupFBO(void);
	void SetupDirectionalShadow(void);
	void SetupBloomFBO(void);

	void GameLoop(void);

	//playground
	float playgroundScale;
	float playerSpeed;
	float attentionRadius;
	float dangerRadius;

	//accumulates ticks
	
	const float GLOBAL_TICK = 0.01f;
	float globalClock;
	float tickAccumulator;
	float GetDeltaTime(void);
	float tickTime;

	//?>stay
	GLFWwindow* window;

	struct shaderUniformGroup
	{
		GLuint locID_matModel;
		GLuint locID_matView;
		GLuint locID_matProj;
		GLuint locID_lightPosition;
		GLuint locID_lightDiffuse;

		GLuint uniformId_ModelColor;// = 0;
		GLuint uniformId_isWireframe;// = 0;
		GLuint uniformId_userDebugColorsNoLighting;// = 0;
		GLuint uniformId_eyeLocation;// = 0;
		GLuint uniformId_alpha;
		GLuint uniformId_use_alpha;
		GLuint uniformId_object_alpha;
		GLuint uniformId_cloth_pass;
		GLuint uniformId_textureExtra;
		GLuint uniformLoc_backFaceDrawPassMode;
		GLuint uniformLoc_backFaceColor;
		GLuint uniformLoc_backFaceOffset;

		//texture
		unsigned int textureCount = 4;
		GLint uniformLoc_texSamp2D[4] = { -1,-1,-1,-1 };

		GLuint uniformLoc_bUseTextureAsDiffuse = -1;
		GLuint uniformLoc_bUseLighting = -1;
		GLuint uniformLoc_bIsDiscardTextureBlack = -1;

		GLuint uniformLoc_bIsImposterBlackIsAlpha = -1;

		GLuint uniformLoc_bIsCubeMapObject = -1;
		GLuint uniformLoc_myCubemapSkyBoxSampler = -1;
		GLuint uniformLoc_usedTexNum = -1;

		//for skinned mesh
		GLuint uniformLoc_bIsASkinnedMesh = -1;
		GLuint uniformLoc_bones[100];
		GLuint uniformLoc_numberOfBonesUsed = -1;
		GLuint uniformLoc_slideDistance = -1;

		GLuint uniformLoc_materialDiffuse = -1;
		GLuint uniformLoc_materialSpecular = -1;

	};
	std::map<unsigned int, shaderUniformGroup> mapShaderUniform;

	GLuint locID_matModel; 
	GLuint locID_matView;
	GLuint locID_matProj;
	GLuint locID_lightPosition;
	GLuint locID_lightDiffuse;

	GLuint uniformId_ModelColor;// = 0;
	GLuint uniformId_isWireframe;// = 0;
	GLuint uniformId_userDebugColorsNoLighting;// = 0;
	GLuint uniformId_eyeLocation;// = 0;
	GLuint uniformId_alpha;
	GLuint uniformId_use_alpha;
	GLuint uniformId_object_alpha;

	//texture
	unsigned int textureCount = 6;
	GLint uniformLoc_texSamp2D[6] = {-1,-1,-1,-1,-1,-1};

	GLuint uniformLoc_bUseTextureAsDiffuse = -1;
	GLuint uniformLoc_bUseLighting = -1;
	GLuint uniformLoc_bIsDiscardTextureBlack = -1;

	GLuint uniformLoc_bIsImposterBlackIsAlpha = -1;

	GLuint uniformLoc_bIsCubeMapObject = -1;
	GLuint uniformLoc_myCubemapSkyBoxSampler = -1;
	GLuint uniformLoc_usedTexNum = -1;

	//Graphics 2
	// For the 2nd pass rendering.
	GLint uniformLoc_offscreenColorTexture = -1;				// Actual texture sampler
	GLint uniformLoc_offscreenBloomTexture = -1;				// bloom blur texture sampler
	GLint uniformLoc_bIsSecondRenderPass = -1;		// Flag to inticate it's the 2nd pass in the shader
	GLint uniformLoc_secondPassMode = -1;

	GLuint FBO_bloom[2];
	GLuint FBO_bloom_colorTexture[2];
	GLuint FBO_bloom_depthTexture;
	GLuint FBO_bloomBlur[2];
	GLuint FBO_bloomBlur_colorTexture[2];

	GLuint FBO[4];
	GLuint FBO_colourTexture[4];
	GLuint FBO_depthTexture[4];
	GLint  FBO_pixelSize = 2048;
	
	//Directional Shadow
	GLuint FBO_depthMap;
	GLuint FBO_depthMap_depthTexture;
	GLuint directionalLight_shadowSampler;
	GLuint uniformLoc_diretionalLight_position = -1;
	GLuint uniformLoc_directionalShadowPass = -1;
	GLuint uniformLoc_directionalLight_spaceMatrix = -1;

	//bloom
	GLuint uniformLoc_isGlowing = -1;
	GLuint uniformLoc_glowColor = -1;
	GLuint uniformLoc_isHorizontal = -1;
	GLuint uniformLoc_bloomWeights[5];
	GLuint uniformLoc_hasBloom = -1;
	GLuint uniformLoc_exposure = -1;

	//Geometry shader
	GLuint uniformLoc_explodeTime = -1;

	GLuint uniformLoc_reflectsSkybox = -1;
};
#endif // !_GELSENIZEMANAGER_HG_

