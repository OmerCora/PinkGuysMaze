#ifndef _cSimpleAssimpSkinnedMesh_HG
#define _cSimpleAssimpSkinnedMesh_HG

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <map>
#include <vector>
#include <array>

#include <glm/glm.hpp>

#include "GelStructures.h"

class cSimpleAssimpSkinnedMesh 
{
private:
	static const int MAX_BONES_PER_VERTEX = 4;

	struct sVertexBoneData
	{
		//std::array<unsigned int, MAX_BONES_PER_VERTEX> ids;
		std::array<float, MAX_BONES_PER_VERTEX> ids;
		std::array<float, MAX_BONES_PER_VERTEX> weights;

		void AddBoneData(unsigned int BoneID, float Weight);
	};

	struct sBoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
		glm::mat4 ObjectBoneTransformation;
		std::string BoneName;
	};

	void cullRotation(glm::quat frozenOrientation, glm::mat4 & objectBoneTransformation_);

public:
	// Mesh and VAO and VBO infor
	unsigned int m_VBO_ID;				// Vertex buffer object 
	unsigned int m_numberOfVertices;
	unsigned int m_indexBuf_ID;			// Index buffer referring to VBO
	unsigned int m_numberOfIndices;
	unsigned int m_numberOfTriangles;
	bool m_bVBO_created;
	// Remember: a VAO is a VBO+shaderProgram combination (i.e. you need 1 VAO ++PER++ VBO+shaderID)
	// (this is only an issue if you are using mulitple shaders; if you are using a single "uber" shader, you're fine)
	unsigned int m_VAO_ID;				// Vertex Array Object
	unsigned int m_shaderIDMatchingVAO;	// The shader prod ID matching this VBO
	bool m_bVAO_created;
	// Actual vertex data
	GelVertex* m_pVertexData;

	aiAnimation* m_Animation;
	glm::mat4 rootBoneZeroTransform;

	//cull rotation
	glm::quat leftHandBoneOrientation;
	glm::quat rightHandBoneOrientation;

	glm::quat leftFinger0BoneOrientation;
	glm::quat rightFinger0HandBoneOrientation;

	glm::quat leftFinger1BoneOrientation;
	glm::quat rightFinger1HandBoneOrientation;

	glm::quat leftFinger2BoneOrientation;
	glm::quat rightFinger2HandBoneOrientation;

	glm::quat leftFinger3BoneOrientation;
	glm::quat rightFinger3HandBoneOrientation;

	glm::quat leftFinger4BoneOrientation;
	glm::quat rightFinger4HandBoneOrientation;

	bool fixRootBone;
public:
	cSimpleAssimpSkinnedMesh(void);
	virtual ~cSimpleAssimpSkinnedMesh(void);

	bool LoadMeshFromFile(const std::string &filename);

	// Creates a VBO, loads the current mesh, then creates a VAO for the current VBO+shader
	bool CreateVBOfromCurrentMesh(void);
	bool CreateVBOandVOAfromCurrentMesh( int shaderProgID, std::string &error );

	void BoneTransform(float TimeInSeconds, 
					   std::vector<glm::mat4> &FinalTransformation, 
					   std::vector<glm::mat4> &Globals, 
					   std::vector<glm::mat4> &Offsets,
					   std::vector<std::string> &Names);
	
	unsigned int GetNums(void) const { return this->mNumBones; }

	float GetDuration(void);
	
	std::vector<sVertexBoneData> vecVertexBoneData;	//Bones;
	glm::mat4 mGlobalInverseTransformation;
	const aiScene* pScene;

	bool Initialize(void);
	
	void CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion& out);
	void CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);
	void CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D& out);

	void CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat& out);
	void CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);
	void CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3& out);

	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);

	const aiNodeAnim* FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString nodeOrBoneName);
	
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const glm::mat4 &parentTransformMatrix);

	void LoadBones(const aiMesh* Mesh, std::vector<sVertexBoneData> &Bones);
	
	void ShutErDown(void);

//	std::vector<sMeshEntry> mMeshEntries;
	std::map<std::string /*BoneName*/, unsigned int /*BoneIndex*/> m_mapBoneNameToBoneIndex;	//mMapping;
	std::vector<sBoneInfo> mBoneInfo;
	unsigned int mNumBones;	//mNums;

	Assimp::Importer mImporter;
};

#endif
