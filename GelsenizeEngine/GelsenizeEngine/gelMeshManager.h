#ifndef _GELMESHMANAGER_HG_
#define _GELMESHMANAGER_HG_

#include "GelStructures.h"
#include <string>
#include <vector>
#include <map>

#include "cSimpleAssimpSkinnedMeshLoader_OneMesh.h"
#include "externals.h"
#include "iManager.h"

// assimp is isolated here
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class gelMeshManager : iManager 
{
public:
	//assimp
	bool LoadModelFromFile(std::string alias, std::string filename);
	bool LoadAddAssimpModel(std::string alias, std::string filename, bool fixRootBone);
	cSimpleAssimpSkinnedMesh* GetSkinnedMesh(std::string alias);
	
	void AddLine(glm::vec3 start, glm::vec3 end, glm::vec3 colour);
	void ClearBuffer(void);

	bool GetDebugLineBufferForRender(unsigned int &VAO_ID, unsigned int &numberOfVertices);
	bool CreateDebugLineBuffer(unsigned int maxNumberOfVertices, GLuint shaderProgramID);
	//bool LoadModelFromFileBumpMap(char* sFilePath, char* sColorMap, char* sNormalMap);
	void SetBasePath(std::string basepath);

	//Singleton  
	static gelMeshManager& sharedInstance()
	{
		static gelMeshManager instance;
		return instance;   
	}

	class Texture
	{
	public:
		Texture(GLenum TextureTarget, const std::string& FileName);

		bool Load();

		void Bind(GLenum TextureUnit);

	private:
		std::string m_fileName;
		GLenum m_textureTarget;
		GLuint m_textureObj;
		//Magick::Image m_image;
		//Magick::Blob m_blob;
	};

	class Mesh
	{
	public:
		struct MeshEntry {
			MeshEntry();

			~MeshEntry();

			void Init(const std::vector<GelVertex>& Vertices,
				const std::vector<unsigned int>& Indices);

			GLuint VB;
			GLuint IB;
			unsigned int NumIndices;
			unsigned int MaterialIndex;
		};
		Mesh();

		~Mesh();

		bool LoadMesh(std::string alias, std::string Filename);

		bool getEntries(std::vector<Mesh::MeshEntry>& entryList);

		//get vao info like method here instead
		//void Render();

	private:
		bool InitFromScene(const aiScene* pScene, const std::string& Filename);
		void InitMesh(unsigned int Index, const aiMesh* paiMesh);
		bool InitMaterials(const aiScene* pScene, const std::string& Filename);
		void Clear();

#define INVALID_MATERIAL 0xFFFFFFFF

		std::vector<MeshEntry> m_Entries;
		std::vector<Texture*> m_Textures;
	};

	void ScaleEverythingToBoundingBoxOnLoad(bool scale, float boxWidth);

	//Load a Ply file into the GL buffer, third is the short name
	bool LoadPlyIntoGLBuffer(unsigned int programID, std::string alias, std::string plyFile, bool isCollisionModel = false, int collisionScale = 1.0f);//added new flag and scale.
	bool LookUpMeshInfo(std::string name, unsigned int &VAO_ID, int &numberOfIndices, float &unitScale);
	float LookUpMeshRadius(std::string name);
	void AddMeshWithName(std::string alias, gelMeshManager::Mesh* mesh);
	gelMeshManager::Mesh* GetMeshWithName(std::string alias);

	//trying different kinds of stuff to access that huge triangle array/map
	bool GetCollisionModelTriangleInfoForMesh(std::string meshName, float& halfExtend, glm::vec3& minPoint);
	bool GetCollisionModelTriangleArrayForMesh(std::string meshName, int AABBid, std::vector<GelTriangle> &vecTriangle);
	bool GetCollisionModelTriangleMapForMesh(std::string meshName, std::map<int, std::vector<GelTriangle> > &mapTriangle);

	bool GetTriangleArrayForMesh(std::string meshName, std::vector<GelTriangle> &vecTriangle);

	virtual void Update(float deltaTime);
	void Setup();
	//~GelMeshManager();
private:
	gelMeshManager() {};
	gelMeshManager(gelMeshManager const&); //dont implement - to build
	void operator=(gelMeshManager const&); //dont implement - to build

	bool ReadPlyFileHeaders(std::string fileName, int& vertexCount, int& faceCount, int& propCount);
	bool LoadPlyFile(GelVertex* vertexArray, int* indexArray, std::string alias, std::string fileName, int vertexCount, int faceCount, int propCount, float& radius, bool isCollisionModel, int collisionScale);

	bool scaleToBoundingBoxOnLoad;	// = false by default
	float boundingBoxScalingSize;	// = 1.0f by default

	std::string m_basePath;

	enum enumTEXCOORDBIAS
	{
		POSITIVE_X, POSITIVE_Y, POSITIVE_Z
	};

	struct VAOInfo
	{
		VAOInfo() : VAO_ID(0), numberOfIndices(0), radius(1.0f), unitScaleValue(1.0f) {}
		std::string meshName;
		std::string name;
		unsigned int VAO_ID; //refactor pls b0ss
		unsigned int numberOfIndices;
		unsigned int materialIndex;
		float radius; //for collision
		float unitScaleValue; //multiply by this scale and object 1.0f
		//std::vector m_Entries;
		//std::vector<Texture> m_Textures;
	};


	struct CollisionTriangleInfo
	{
		CollisionTriangleInfo() : alias(""), aabbHalfExtend(5.0f), collisionModelScale(1.0f) {}
		float collisionModelScale;
		float aabbHalfExtend; //multiply by this scale and object 1.0f
		glm::vec3 minPoint;
		std::string alias;
		std::map<int, std::vector<GelTriangle> > aabbTriangleMap;
	};

	// Map... aka "dictionay" "look up table"
	std::map< std::string, VAOInfo > meshMap;
	std::map < std::string, Mesh* > meshMapV2;

	std::map< std::string, CollisionTriangleInfo > collisionMap;

	//this will be moved to elsewhere, added for physics
	std::map< std::string, std::vector<GelTriangle> > triangleMap;
	std::map< std::string, std::vector<GelTriangle> > triangleMap_CollisionModels;

	std::map < std::string, std::map<int, std::vector<GelTriangle> > > map_aliasTo_AABBToTriangleArray;
	std::map<int, std::vector<GelTriangle> > temp_collision_triangles;
	
	//skinned mesh map
	std::map<std::string, cSimpleAssimpSkinnedMesh* > skinnedMeshMap;

	GelVertex* m_p_vertArrayLines;
	unsigned int m_sizeOfDebugLineArrayBufferInBytes;
	unsigned int m_nextLineIndexStart;
	GLuint m_line_VAO_ID;
	GLuint m_line_vert_buffer_ID;
};


#endif // !_GELMESHMANAGER_HG_
