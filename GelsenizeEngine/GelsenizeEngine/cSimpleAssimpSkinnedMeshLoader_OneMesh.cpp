#include "cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <glad/glad.h>

#include <sstream>
#include <iostream>

//#define OUTPUT_STUFF 1


glm::mat4 AIMatrixToGLMMatrix(const aiMatrix4x4 &mat)
{
	return glm::mat4(mat.a1, mat.b1, mat.c1, mat.d1,	
					mat.a2, mat.b2, mat.c2, mat.d2,
					mat.a3, mat.b3, mat.c3, mat.d3,
					mat.a4, mat.b4, mat.c4, mat.d4);
}


void cSimpleAssimpSkinnedMesh::cullRotation(glm::quat frozenOrientation, glm::mat4 & objectBoneTransformation_)
{
	glm::vec4 translationFinal = objectBoneTransformation_[3];
	objectBoneTransformation_ = glm::mat4_cast(frozenOrientation);

	objectBoneTransformation_[3].x = translationFinal.x;
	objectBoneTransformation_[3].y = translationFinal.y;
	objectBoneTransformation_[3].z = translationFinal.z;
	objectBoneTransformation_[3].w = translationFinal.w;
}

cSimpleAssimpSkinnedMesh::cSimpleAssimpSkinnedMesh(void)
{
	this->pScene = 0;
	this->mNumBones = 0;

	this->m_VBO_ID = 0;				// Vertex buffer object 
	this->m_numberOfVertices = 0;
	this->m_indexBuf_ID = 0;		// Index buffer referring to VBO
	this->m_numberOfIndices = 0;
	this->m_numberOfTriangles = 0;
	this->m_bVBO_created = false;

	this->m_VAO_ID = 0;				// Vertex Array Object
	this->m_bVAO_created = false;
	this->m_shaderIDMatchingVAO = 0;

	fixRootBone = false;

	return;
}

///*static*/ 
//const int cSimpleAssimpSkinnedMesh::MAX_BONES_PER_VERTEX = 4;


cSimpleAssimpSkinnedMesh::~cSimpleAssimpSkinnedMesh(void)
{
	this->ShutErDown();

	return;
}

void cSimpleAssimpSkinnedMesh::ShutErDown(void)
{
	if ( this->m_pVertexData )
	{
		delete [] this->m_pVertexData;
	}
	// TODO: Delete the OpenGL buffers, too??

	return;
}

bool cSimpleAssimpSkinnedMesh::LoadMeshFromFile(const std::string &filename)
{
	unsigned int Flags = aiProcess_Triangulate | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph | aiProcess_JoinIdenticalVertices | aiProcess_GenSmoothNormals | aiProcess_CalcTangentSpace;
	this->pScene = this->mImporter.ReadFile(filename.c_str(), Flags);

	std::cout << this->mImporter.GetErrorString() << std::endl;

	aiMesh* pM0 = this->pScene->mMeshes[0];
	//aiMesh* pM1 = this->mpScene->mMeshes[1];
	//aiMesh* pM2 = this->mpScene->mMeshes[2];
	//aiMesh* pM3 = this->mpScene->mMeshes[3];

	if ( this->pScene )
	{
		this->mGlobalInverseTransformation = AIMatrixToGLMMatrix( pScene->mRootNode->mTransformation );
		this->mGlobalInverseTransformation = glm::inverse(this->mGlobalInverseTransformation);
		return this->Initialize();
	}
	return false;
}

bool cSimpleAssimpSkinnedMesh::Initialize(void)
{
	this->m_numberOfVertices = this->pScene->mMeshes[0]->mNumVertices;
	m_Animation = this->pScene->mAnimations[0];
	// This is the vertex information for JUST the bone stuff
	this->vecVertexBoneData.resize(this->m_numberOfVertices);

	this->LoadBones( this->pScene->mMeshes[0], this->vecVertexBoneData );

	return true;
}


void cSimpleAssimpSkinnedMesh::sVertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for ( unsigned int Index = 0; Index < sizeof(this->ids) / sizeof(this->ids[0]); Index++ )
	{
		if ( this->weights[Index] == 0.0f )
		{
			this->ids[Index]		= BoneID;
			this->weights[Index]	= Weight;
			return;
		}
	}
}

// In the original code, these vectors are being passed out into the "character" object.
// It's unclear what the Globals matrices are actually for...
void cSimpleAssimpSkinnedMesh::BoneTransform( float TimeInSeconds, 
                                              std::vector<glm::mat4> &FinalTransformation, 
								              std::vector<glm::mat4> &Globals, 
								              std::vector<glm::mat4> &Offsets,
											  std::vector<std::string> &Names)
{
	glm::mat4 Identity(1.0f);

	float TicksPerSecond = static_cast<float>( this->pScene->mAnimations[0]->mTicksPerSecond != 0 ?
	                                           this->pScene->mAnimations[0]->mTicksPerSecond : 25.0 );

	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = fmod(TimeInTicks, (float)this->pScene->mAnimations[0]->mDuration);
	

	this->ReadNodeHeirarchy(AnimationTime, this->pScene->mRootNode, Identity);

	FinalTransformation.resize(this->mNumBones);
	Globals.resize(this->mNumBones);
	Offsets.resize(this->mNumBones);
	Names.resize(this->mNumBones);

	for ( unsigned int BoneIndex = 0; BoneIndex < this->mNumBones; BoneIndex++ )
	{
		FinalTransformation[BoneIndex] = this->mBoneInfo[BoneIndex].FinalTransformation;
		Globals[BoneIndex] = this->mBoneInfo[BoneIndex].ObjectBoneTransformation;
		Offsets[BoneIndex] = this->mBoneInfo[BoneIndex].BoneOffset;
		Names[BoneIndex] = this->mBoneInfo[BoneIndex].BoneName;
	}
}

void cSimpleAssimpSkinnedMesh::LoadBones(const aiMesh* Mesh, std::vector<sVertexBoneData> &vertexBoneData)
{
	for ( unsigned int boneIndex = 0; boneIndex != Mesh->mNumBones; boneIndex++ )
	{
		unsigned int BoneIndex = 0;
		std::string BoneName(Mesh->mBones[boneIndex]->mName.data);

		bool rootMotion = false;
		if (1)
		{
			if (BoneName == "B_Pelvis")
			{
				rootMotion = true;
			}
		}

		//	std::map<std::string /*BoneName*/, unsigned int /*BoneIndex*/> mMapping;
		// 	std::vector<sBoneInfo> mInfo;

		std::map<std::string, unsigned int>::iterator it = this->m_mapBoneNameToBoneIndex.find(BoneName);
		if ( it == this->m_mapBoneNameToBoneIndex.end() )
		{
			BoneIndex = this->mNumBones;
			this->mNumBones++;
			sBoneInfo bi;
			this->mBoneInfo.push_back(bi);

			this->mBoneInfo[BoneIndex].BoneOffset = AIMatrixToGLMMatrix(Mesh->mBones[boneIndex]->mOffsetMatrix);
			this->m_mapBoneNameToBoneIndex[BoneName] = BoneIndex;
			this->mBoneInfo[BoneIndex].BoneName = BoneName;
		}
		else
		{
			BoneIndex = it->second;
		}

		for ( unsigned int WeightIndex = 0; WeightIndex != Mesh->mBones[boneIndex]->mNumWeights; WeightIndex++ )
		{
			unsigned int VertexID = /*mMeshEntries[MeshIndex].BaseVertex +*/ Mesh->mBones[boneIndex]->mWeights[WeightIndex].mVertexId;
			float Weight = Mesh->mBones[boneIndex]->mWeights[WeightIndex].mWeight;

			if (rootMotion)
			{
				Weight = 0.0f;
			}

			std::string boneName(Mesh->mBones[boneIndex]->mName.data);
			//if (boneName == "B_L_Hand")
			//{
			//	//Weight *= 0.5f;
			//}
			//else if (boneName == "B_R_Hand")
			//{
			//	//Weight *= 0.5f;
			//}

			//if (boneName == "B_R_Finger0" ||
			//	boneName == "B_R_Finger1" ||
			//	boneName == "B_R_Finger3" ||
			//	boneName == "B_R_Finger4" )
			//{
			//	//Weight = 0.0f;
			//}


			vertexBoneData[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
	return;
}

const aiNodeAnim* cSimpleAssimpSkinnedMesh::FindNodeAnimationChannel(const aiAnimation* pAnimation, aiString boneName)
{
	for ( unsigned int ChannelIndex = 0; ChannelIndex != pAnimation->mNumChannels; ChannelIndex++ )
	{
		if ( pAnimation->mChannels[ChannelIndex]->mNodeName == boneName )
		{
			return pAnimation->mChannels[ChannelIndex];
		}
	}
	return 0;
}


unsigned int cSimpleAssimpSkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for ( unsigned int RotationKeyIndex = 0; RotationKeyIndex != pNodeAnim->mNumRotationKeys - 1; RotationKeyIndex++ )
	{
		if ( AnimationTime < (float)pNodeAnim->mRotationKeys[RotationKeyIndex+1].mTime )
		{
			return RotationKeyIndex;
		}
	}

	return 0;
}

unsigned int cSimpleAssimpSkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for ( unsigned int PositionKeyIndex = 0; PositionKeyIndex != pNodeAnim->mNumPositionKeys - 1; PositionKeyIndex++ )
	{
		if ( AnimationTime < (float)pNodeAnim->mPositionKeys[PositionKeyIndex+1].mTime )
		{
			return PositionKeyIndex;
		}
	}

	return 0;
}

unsigned int cSimpleAssimpSkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for ( unsigned int ScalingKeyIndex = 0; ScalingKeyIndex != pNodeAnim->mNumScalingKeys - 1; ScalingKeyIndex++ )
	{
		if ( AnimationTime < (float)pNodeAnim->mScalingKeys[ScalingKeyIndex+1].mTime )
		{
			return ScalingKeyIndex;
		}
	}

	return 0;
}

void cSimpleAssimpSkinnedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, 
												 const glm::mat4 &ParentTransformMatrix)
{
	aiString NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = this->pScene->mAnimations[0];
	//aiMatrix4x4 NodeTransformation;

	// Transformation of the node in bind pose
	glm::mat4 NodeTransformation = AIMatrixToGLMMatrix( pNode->mTransformation );

	const aiNodeAnim* pNodeAnim = this->FindNodeAnimationChannel(pAnimation, NodeName);

	if ( pNodeAnim )
	{
		// Get interpolated scaling
		glm::vec3 scale;
		this->CalcGLMInterpolatedScaling(AnimationTime, pNodeAnim, scale);
		glm::mat4 ScalingM = glm::scale(glm::mat4(1.0f), scale);

		// Get interpolated rotation (quaternion)
		glm::quat ori;
		this->CalcGLMInterpolatedRotation(AnimationTime, pNodeAnim, ori);
		glm::mat4 RotationM = glm::mat4_cast(ori);
		
		// Get interpolated position 
		glm::vec3 pos;
		this->CalcGLMInterpolatedPosition(AnimationTime, pNodeAnim, pos);
		glm::mat4 TranslationM = glm::translate(glm::mat4(1.0f), pos);
		
		
		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}
	//else
	//{
	//	// If there's no bone corresponding to this node, then it's something the animator used to 
	//	//	help make or position the model or animation. The point is that it DOESN'T change
	//	//	based on the animation frame, so take it just as is
	//	NodeTransformation = AIMatrixToGLMMatrix( pNode->mTransformation );
	//}

	glm::mat4 ObjectBoneTransformation = ParentTransformMatrix * NodeTransformation;

	/*if (std::string(NodeName.data) == "Motion_$AssimpFbx$_Translationasdf" ||
		std::string(NodeName.data) == "Motion_$AssimpFbx$_Rotationasdf"    || 
		std::string(NodeName.data) == "Motion_$AssimpFbx$_PreRotationasdf" ||
		std::string(NodeName.data) == "Motion_$AssimpFbx$_Scalingadsf" )
	{
		ObjectBoneTransformation = glm::mat4(1.0f);
	}*/

	std::map<std::string, unsigned int>::iterator it = this->m_mapBoneNameToBoneIndex.find(std::string(NodeName.data));
	if ( it != this->m_mapBoneNameToBoneIndex.end() )
	{
		unsigned int BoneIndex = it->second;
		this->mBoneInfo[BoneIndex].ObjectBoneTransformation = ObjectBoneTransformation;
		this->mBoneInfo[BoneIndex].FinalTransformation = this->mGlobalInverseTransformation 
		                                                 * ObjectBoneTransformation 
			                                             * this->mBoneInfo[BoneIndex].BoneOffset;
		//this->mBoneInfo[BoneIndex].FinalTransformation = GlobalTransformation 
		//	                                             * this->mBoneInfo[BoneIndex].BoneOffset;
	}
	else
	{
		int breakpoint = 0;
	}

	for ( unsigned int ChildIndex = 0; ChildIndex != pNode->mNumChildren; ChildIndex++ )
	{
		//Cull every another timeslice's B_Pelvis->ObjectBoneTransformation
		//to the B_Pelvis->ObjectBoneTransformation of the timeslice where AnimationTime is 0.0f)
		//I first tried the bone animation in Blendr after removing the B_Pelvis "motion"

		//This results in an acceptable rate of distortion for the original intended animation because of minimal rotation issues.
		//The character stays in place while looping nonetheless
		std::string boneName(pNode->mName.data);
		if (fixRootBone)
		{
			
			if (boneName == "B_Pelvis")
			{
				if (AnimationTime == 0.0f)
				{
					this->rootBoneZeroTransform = ObjectBoneTransformation;
				}

				//ObjectBoneTransformation = this->rootBoneZeroTransform;

				//instead keep the scale and rotatios, replace translation
				ObjectBoneTransformation[3].x = this->rootBoneZeroTransform[3].x;
				ObjectBoneTransformation[3].y = this->rootBoneZeroTransform[3].y;
				ObjectBoneTransformation[3].z = this->rootBoneZeroTransform[3].z;
				ObjectBoneTransformation[3].w = this->rootBoneZeroTransform[3].w;
			}
		}

		/*if (boneName == "B_L_Hand")
		{
			if (AnimationTime == 0.0f)
				this->leftHandBoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->leftHandBoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_R_Hand")
		{
			if (AnimationTime == 0.0f)
				this->rightHandBoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->rightHandBoneOrientation, ObjectBoneTransformation);
		}


		if (boneName == "B_L_Finger0")
		{
			if (AnimationTime == 0.0f)
				this->leftFinger0BoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->leftFinger0BoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_L_Finger1")
		{
			if (AnimationTime == 0.0f)
				this->leftFinger1BoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->leftFinger1BoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_L_Finger2")
		{
			if (AnimationTime == 0.0f)
				this->leftFinger2BoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->leftFinger2BoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_L_Finger3")
		{
			if (AnimationTime == 0.0f)
				this->leftFinger3BoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->leftFinger3BoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_L_Finger4")
		{
			if (AnimationTime == 0.0f)
				this->leftFinger4BoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->leftFinger4BoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_R_Finger0")
		{
			if (AnimationTime == 0.0f)
				this->rightFinger0HandBoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->rightFinger0HandBoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_R_Finger1")
		{
			if (AnimationTime == 0.0f)
				this->rightFinger1HandBoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->rightFinger1HandBoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_R_Finger2")
		{
			if (AnimationTime == 0.0f)
				this->rightFinger2HandBoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->rightFinger2HandBoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_R_Finger3")
		{
			if (AnimationTime == 0.0f)
				this->rightFinger3HandBoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->rightFinger3HandBoneOrientation, ObjectBoneTransformation);
		}
		else
		if (boneName == "B_R_Finger4")
		{
			if (AnimationTime == 0.0f)
				this->rightFinger4HandBoneOrientation = glm::quat_cast(ObjectBoneTransformation);

			this->cullRotation(this->rightFinger4HandBoneOrientation, ObjectBoneTransformation);
		}*/




		this->ReadNodeHeirarchy(AnimationTime, pNode->mChildren[ChildIndex], ObjectBoneTransformation);
	}

}

void cSimpleAssimpSkinnedMesh::CalcInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, aiQuaternion &out)
{
	if ( pNodeAnim->mNumRotationKeys == 1 ) 
	{
		out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = this->FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(out, StartRotationQ, EndRotationQ, Factor);
	out = out.Normalize();

	return;
}

void cSimpleAssimpSkinnedMesh::CalcInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D &out)
{
	if ( pNodeAnim->mNumPositionKeys == 1 )
	{
		out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = this->FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	out = (EndPosition - StartPosition) * Factor + StartPosition;

	return;
}

void cSimpleAssimpSkinnedMesh::CalcInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, aiVector3D &out)
{
	if ( pNodeAnim->mNumScalingKeys == 1 )
	{
		out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = this->FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	out = (EndScale - StartScale) * Factor + StartScale;

	return;
}

void cSimpleAssimpSkinnedMesh::CalcGLMInterpolatedRotation(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::quat &out)
{
	if ( pNodeAnim->mNumRotationKeys == 1 ) 
	{
		out.w = pNodeAnim->mRotationKeys[0].mValue.w;
		out.x = pNodeAnim->mRotationKeys[0].mValue.x;
		out.y = pNodeAnim->mRotationKeys[0].mValue.y;
		out.z = pNodeAnim->mRotationKeys[0].mValue.z;
		return;
	}

	unsigned int RotationIndex = this->FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	if ( Factor < 0.0f ) Factor = 0.0f;
	if ( Factor > 1.0f ) Factor = 1.0f;
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	
	glm::quat StartGLM = glm::quat(StartRotationQ.w, StartRotationQ.x, StartRotationQ.y, StartRotationQ.z);
	glm::quat EndGLM = glm::quat(EndRotationQ.w, EndRotationQ.x, EndRotationQ.y, EndRotationQ.z);

	out = glm::slerp(StartGLM, EndGLM, Factor);
	out = glm::normalize(out);

	return;
}

void cSimpleAssimpSkinnedMesh::CalcGLMInterpolatedPosition(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3 &out)
{
	if ( pNodeAnim->mNumPositionKeys == 1 )
	{
		out.x = pNodeAnim->mPositionKeys[0].mValue.x;
		out.y = pNodeAnim->mPositionKeys[0].mValue.y;
		out.z = pNodeAnim->mPositionKeys[0].mValue.z;
		return;
	}

	unsigned int PositionIndex = this->FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	if ( Factor < 0.0f ) Factor = 0.0f;
	if ( Factor > 1.0f ) Factor = 1.0f;
	const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	glm::vec3 start = glm::vec3(StartPosition.x, StartPosition.y, StartPosition.z);
	glm::vec3 end = glm::vec3(EndPosition.x, EndPosition.y, EndPosition.z);
	out = (end - start) * Factor + start;

	return;
}

void cSimpleAssimpSkinnedMesh::CalcGLMInterpolatedScaling(float AnimationTime, const aiNodeAnim* pNodeAnim, glm::vec3 &out)
{
	if ( pNodeAnim->mNumScalingKeys == 1 )
	{
		out.x = pNodeAnim->mScalingKeys[0].mValue.x;
		out.y = pNodeAnim->mScalingKeys[0].mValue.y;
		out.z = pNodeAnim->mScalingKeys[0].mValue.z;
		return;
	}

	unsigned int ScalingIndex = this->FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	if ( Factor < 0.0f ) Factor = 0.0f;
	if ( Factor > 1.0f ) Factor = 1.0f;
	const aiVector3D& StartScale = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& EndScale = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	glm::vec3 start = glm::vec3(StartScale.x, StartScale.y, StartScale.z);
	glm::vec3 end = glm::vec3(EndScale.x, EndScale.y, EndScale.z);
	out = (end - start) * Factor + start;

	return;
}

float cSimpleAssimpSkinnedMesh::GetDuration(void)
{
	return (float)(this->pScene->mAnimations[0]->mDuration / this->pScene->mAnimations[0]->mTicksPerSecond);
}

// Creates a VBO, loads the current mesh, then creates a VAO for the current VBO+shader
//bool cAssimpMesh::CreateVBOandVOAfromCurrentMesh(int shaderID, unsigned int &VBO_ID, unsigned int &VAO_ID )
bool cSimpleAssimpSkinnedMesh::CreateVBOfromCurrentMesh(/* unsigned int &VBO_ID, unsigned int &numVertices,
                                            unsigned int &indexBufferID, unsigned int &numIndices*/ )
{
	this->m_numberOfVertices = this->pScene->mMeshes[0]->mNumVertices;
	

	this->m_pVertexData = new GelVertex[this->m_numberOfVertices];

	float minX = 0.0f;
	float minY = 0.0f;
	float minZ = 0.0f;

	float maxX = 0.0f;
	float maxY = 0.0f;
	float maxZ = 0.0f;

	float radius = 0.0f;
	float boundingBoxSize = 10.0f;

	for ( unsigned int vertexID = 0; vertexID != this->m_numberOfVertices; vertexID++ )
	{
		GelVertex* pCurVert = &(this->m_pVertexData[vertexID]);
		aiVector3D* pCurAIVert = &(this->pScene->mMeshes[0]->mVertices[vertexID]);
		aiVector3D* pCurAINormals = &(this->pScene->mMeshes[0]->mNormals[vertexID]);
		aiVector3D* pCurAITangents = &(this->pScene->mMeshes[0]->mTangents[vertexID]);
		aiVector3D* pCurAIBitangents = &(this->pScene->mMeshes[0]->mBitangents[vertexID]);

		pCurVert->x = pCurAIVert->x;
		pCurVert->y = pCurAIVert->y;
		pCurVert->z = pCurAIVert->z;

		pCurVert->nX = pCurAINormals->x;
		pCurVert->nY = pCurAINormals->y;
		pCurVert->nZ = pCurAINormals->z;
		
		pCurVert->tanX = pCurAITangents->x;
		pCurVert->tanY = pCurAITangents->y;
		pCurVert->tanZ = pCurAITangents->z;

		pCurVert->bitanX = pCurAIBitangents->x;
		pCurVert->bitanY = pCurAIBitangents->y;
		pCurVert->bitanZ = pCurAIBitangents->z;

		float x, y, z;
		x = pCurAIVert->x;
		y = pCurAIVert->y;
		z = pCurAIVert->z;
		//vecVertices.push_back(coraVertex);

		if (maxX < x) maxX = x;
		if (maxY < y) maxY = y;
		if (maxZ < z) maxZ = z;

		if (minX > x) minX = x;
		if (minY > y) minY = y;
		if (minZ > z) minZ = z;

		// Bone IDs are being passed OK
		pCurVert->boneID[0] = this->vecVertexBoneData[vertexID].ids[0];
		pCurVert->boneID[1] = this->vecVertexBoneData[vertexID].ids[1];
		pCurVert->boneID[2] = this->vecVertexBoneData[vertexID].ids[2];
		pCurVert->boneID[3] = this->vecVertexBoneData[vertexID].ids[3];

		// Weights are being passed OK
		pCurVert->boneWeight[0] = this->vecVertexBoneData[vertexID].weights[0];
		pCurVert->boneWeight[1] = this->vecVertexBoneData[vertexID].weights[1];
		pCurVert->boneWeight[2] = this->vecVertexBoneData[vertexID].weights[2];
		pCurVert->boneWeight[3] = this->vecVertexBoneData[vertexID].weights[3];

		pCurVert->r = 1.0f;
		pCurVert->g = 1.0f;
		pCurVert->b = 1.0f;
		pCurVert->a = 1.0f;


	} //for ( unsigned int vertexID;

	//scale time

	float distX = abs(maxX - minX);
	float distY = abs(maxY - minY);
	float distZ = abs(maxZ - minZ);
	float max2Radius = 0.0f;
	if (distX > distY)
	{
		max2Radius = distX;
	}
	else
	{
		max2Radius = distY;
	}
	if (max2Radius < distZ)
	{
		max2Radius = distZ;
	}
	std::cout << "max 2radius for assimp file = " << max2Radius << std::endl;
	radius = max2Radius / 2.0f;

	bool scaleIntoBoundingBox = true;
	if (scaleIntoBoundingBox)
	{
		float scaleValue = boundingBoxSize / max2Radius;
		radius *= scaleValue;
	}

	//apply scale
	float xmaxnew = 0.0f;
	float ymaxnew = 0.0f;
	float zmaxnew = 0.0f;
	float xminnew = 0.0f;
	float yminnew = 0.0f;
	float zminnew = 0.0f;


	// This will scale everything to a set bounding box scale
	float scaleValue = boundingBoxSize / (max2Radius);
	if (scaleIntoBoundingBox)
	{
		for (int i = 0; i <  this->m_numberOfVertices; i++)
		{
			//point test for collision 
			float test_x = this->m_pVertexData[i].x * (1.0f / boundingBoxSize) * scaleValue;
			float test_y = this->m_pVertexData[i].y * (1.0f / boundingBoxSize) * scaleValue;
			float test_z = this->m_pVertexData[i].z * (1.0f / boundingBoxSize) * scaleValue;

			//this->m_pVertexData[i].x *= scaleValue;
			//this->m_pVertexData[i].y *= scaleValue;
			//this->m_pVertexData[i].z *= scaleValue;

			//this->m_pVertexData[i].boneWeight[0] *= scaleValue;
			//this->m_pVertexData[i].boneWeight[1] *= scaleValue;
			//this->m_pVertexData[i].boneWeight[2] *= scaleValue;
			//this->m_pVertexData[i].boneWeight[3] *= scaleValue;

			//vertexArray[i].nX *= -scaleValue;
			//vertexArray[i].nY *= -scaleValue;
			//vertexArray[i].nZ *= -scaleValue;

			//new max min
			if (xmaxnew < test_x) xmaxnew = test_x;
			if (ymaxnew < test_y) ymaxnew = test_y;
			if (zmaxnew < test_z) zmaxnew = test_z;

			if (xminnew > test_x) xminnew = test_x;
			if (yminnew > test_y) yminnew = test_y;
			if (zminnew > test_z) zminnew = test_z;
		}

		glm::vec3 minPoint;
		minPoint.x = xminnew;
		minPoint.y = yminnew;
		minPoint.z = zminnew;

		distX = abs(xmaxnew - xminnew);
		distY = abs(ymaxnew - yminnew);
		distZ = abs(zmaxnew - zminnew);
		float new2r = 0.0f;
		if (distX > distY)
		{
			new2r = distX;
		}
		else
		{
			new2r = distY;
		}
		if (new2r < distZ)
		{
			new2r = distZ;
		}
		new2r /= 2;

		/*for (int i = 0; i < faceCount; i++)
		{
			
			triangleArray[i].v1.x *= scaleValue;
			triangleArray[i].v1.y *= scaleValue;
			triangleArray[i].v1.z *= scaleValue;

			triangleArray[i].v2.x *= scaleValue;
			triangleArray[i].v2.y *= scaleValue;
			triangleArray[i].v2.z *= scaleValue;

			triangleArray[i].v3.x *= scaleValue;
			triangleArray[i].v3.y *= scaleValue;
			triangleArray[i].v3.z *= scaleValue;
		}
*/
	}


	// Create a vertex buffer
	glGenBuffers(1, &(this->m_VBO_ID) );  
	// Brings the particular vertex buffer into context
	glBindBuffer(GL_ARRAY_BUFFER, this->m_VBO_ID);

	int vertexBufferSizeInBytes = sizeof( GelVertex ) * this->m_numberOfVertices;

	glBufferData(GL_ARRAY_BUFFER, 
					vertexBufferSizeInBytes, 
					this->m_pVertexData,		// Pointer to vertex array
					GL_STATIC_DRAW);

	// Now the index buffer...
	this->m_numberOfTriangles = this->pScene->mMeshes[0]->mNumFaces;
	unsigned int numIndicesPerFace = this->pScene->mMeshes[0]->mFaces->mNumIndices;	// Will ALWAYS be 3 if we are using "triangulate"

	std::vector< unsigned int > vecTriIndices;
	this->m_numberOfIndices = this->m_numberOfTriangles * numIndicesPerFace; /*'always' 3*/
	vecTriIndices.reserve( this->m_numberOfIndices );

	for ( int faceIndex = 0; faceIndex != this->m_numberOfTriangles; faceIndex++ )
	{	
		aiFace* pCurAIFace = &(this->pScene->mMeshes[0]->mFaces[faceIndex]);

		vecTriIndices.push_back( pCurAIFace->mIndices[0] );
		vecTriIndices.push_back( pCurAIFace->mIndices[1] );
		vecTriIndices.push_back( pCurAIFace->mIndices[2] );
	}

	glGenBuffers( 1, &(this->m_indexBuf_ID) );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->m_indexBuf_ID );

	unsigned int sizeOfIndexArrayInBytes = static_cast<unsigned int>(vecTriIndices.size()) * sizeof( GLuint );

	unsigned int* pStartOfIndexBuffer = &(vecTriIndices[0]);
	glBufferData ( GL_ELEMENT_ARRAY_BUFFER, sizeOfIndexArrayInBytes, pStartOfIndexBuffer, GL_STATIC_DRAW);

	this->m_bVBO_created = true;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	return true;
}

bool cSimpleAssimpSkinnedMesh::CreateVBOandVOAfromCurrentMesh( int shaderProgID, std::string &error )
{
	this->m_shaderIDMatchingVAO = shaderProgID;

	glUseProgram( this->m_shaderIDMatchingVAO );

	glGenVertexArrays(1, &(this->m_VAO_ID) );	
	glBindVertexArray( this->m_VAO_ID );
	
	// Associate a particular VBO to this VAO...
	glBindBuffer( GL_ARRAY_BUFFER, this->m_VBO_ID );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, this->m_indexBuf_ID );

	unsigned int sizeOfVertexInBytes = sizeof(GelVertex);

	{// vPosition
		GLint vPosition_location = glGetAttribLocation(this->m_shaderIDMatchingVAO, "vPosition");	
		if ( vPosition_location < 0 )	
		{ error = "Error: can't find vPosition vertex attrib."; return false; }

		glEnableVertexAttribArray(vPosition_location);
		int offsetInBytesToPosition = offsetof(GelVertex, x);
		glVertexAttribPointer( vPosition_location, 3, 
								GL_FLOAT, GL_FALSE,
								sizeOfVertexInBytes,	 
								reinterpret_cast<void*>(static_cast<uintptr_t>(offsetInBytesToPosition)) );	// For 64 bit issues
	}// vPosition

	{// vColour
		GLint vColour_location = glGetAttribLocation(this->m_shaderIDMatchingVAO, "vColour");	
		if ( vColour_location < 0 )	
		{ error = "Error: can't find vColour vertex attrib."; }

		glEnableVertexAttribArray(vColour_location);
		int offsetInBytesToColour = offsetof(GelVertex, r );
		glVertexAttribPointer( vColour_location, 4, GL_FLOAT, GL_FALSE,
								sizeOfVertexInBytes,	
								reinterpret_cast<void*>(static_cast<uintptr_t>(offsetInBytesToColour)) );	// For 64 bit issues
	}// vColour

	{// vNormal
		GLint vNormal_location = glGetAttribLocation(this->m_shaderIDMatchingVAO, "vNormal");	
		if ( vNormal_location < 0 )	
		{ error = "Error: can't find vNormal vertex attrib."; return false; }

		glEnableVertexAttribArray(vNormal_location);
 		int offsetInBytesToNormal = offsetof(GelVertex, nX );
		glVertexAttribPointer( vNormal_location, 3, GL_FLOAT, GL_TRUE,
								sizeOfVertexInBytes,	 
								reinterpret_cast<void*>(static_cast<uintptr_t>(offsetInBytesToNormal)) );		// For 64 bit issues
	}// vNormal


	{// vTexSTU
		GLint vTexSTU_location = glGetAttribLocation(this->m_shaderIDMatchingVAO, "vTexSTU");	
		if ( vTexSTU_location < 0 )	
		{ error = "Error: can't find vTexSTU vertex attrib."; return false; }

		glEnableVertexAttribArray(vTexSTU_location);
 		int offsetInBytesToSTU = offsetof(GelVertex, s );
		glVertexAttribPointer( vTexSTU_location, 3, GL_FLOAT, GL_TRUE,
								sizeOfVertexInBytes,	
								reinterpret_cast<void*>(static_cast<uintptr_t>(offsetInBytesToSTU)) );		// For 64 bit issues
	}// vTexSTU

	{// vTangent
		GLint vTangent_location = glGetAttribLocation(this->m_shaderIDMatchingVAO, "vTangent");	
		if ( vTangent_location < 0 )	
		{ error = "Error: can't find vTangent vertex attrib.";  }

		glEnableVertexAttribArray(vTangent_location);
 		int offsetInBytesToTangent = offsetof(GelVertex, tanX);
		glVertexAttribPointer( vTangent_location, 3, GL_FLOAT, GL_TRUE,
								sizeOfVertexInBytes,	
								reinterpret_cast<void*>(static_cast<uintptr_t>(offsetInBytesToTangent)) );		// For 64 bit issues
	}// vTangent

	{//vBitangent
		GLint vBitangent_location = glGetAttribLocation(this->m_shaderIDMatchingVAO, "vBitangent");	
		if ( vBitangent_location < 0 )	
		{ error = "Error: can't find vBitangent vertex attrib.";  }

		glEnableVertexAttribArray(vBitangent_location);
		int offsetInBytesToBiTangent = offsetof(GelVertex, bitanX );
		glVertexAttribPointer( vBitangent_location, 3, GL_FLOAT, GL_TRUE,
								sizeOfVertexInBytes,	
								reinterpret_cast<void*>(static_cast<uintptr_t>(offsetInBytesToBiTangent)) );		// For 64 bit issues
	}//vBitangent

	{//vBoneIDs_x4
		GLint vBoneIDs_location = glGetAttribLocation(this->m_shaderIDMatchingVAO, "vBoneIDs_x4");	
		if ( vBoneIDs_location < 0 )	
		{ error = "Error: can't find vBoneIDs_x4 vertex attrib."; return false; }

		glEnableVertexAttribArray(vBoneIDs_location);
		int offsetInBytesToBoneIDs = (int)offsetof(GelVertex, boneID[0]);
		//glVertexAttribPointer( vBoneIDs_location, 4, GL_INT, GL_FALSE,
		glVertexAttribPointer( vBoneIDs_location, 4, GL_FLOAT, GL_FALSE,
								sizeOfVertexInBytes,	
								reinterpret_cast<void*>(static_cast<uintptr_t>(offsetInBytesToBoneIDs)) );		// For 64 bit issues
	}//vBoneIDs_x4

	{//vBoneWeights_x4
		GLint vBoneWeights_location = glGetAttribLocation(this->m_shaderIDMatchingVAO, "vBoneWeights_x4");	
		if ( vBoneWeights_location < 0 )	
		{ error = "Error: can't find vBoneWeights_x4 vertex attrib."; return false; }

		glEnableVertexAttribArray(vBoneWeights_location);
		int offsetInBytesToBoneWeights = (int)offsetof(GelVertex, boneWeight[0] );
		glVertexAttribPointer( vBoneWeights_location, 4, GL_FLOAT, GL_TRUE,
								sizeOfVertexInBytes,	
								reinterpret_cast<void*>(static_cast<uintptr_t>(offsetInBytesToBoneWeights)) );		// For 64 bit issues
	}//vBoneWeights_x4

	this->m_bVAO_created = true;

	// Unbind everything
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glUseProgram(0);


	return true;
}



