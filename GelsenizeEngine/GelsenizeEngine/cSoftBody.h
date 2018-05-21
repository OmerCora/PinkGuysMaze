#ifndef _CSOFTBODY_HG_
#define _CSOFTBODY_HG_

#include "externals.h"
#include "iShape.h"
#include "cRigidBodyDef.h"
#include "iRigidBody.h"

#include <vector>

namespace nPhysics
{
	class cSoftBody : public iSoftBody
	{
	public:

		void GenerateRectangleCloth(glm::vec3 initialPosition, float width, float height, int width_nodeCount, int height_nodeCount);

		//not used
		virtual void GetSpecialScale(glm::vec3& specialScale);

		virtual glm::vec3 GetNodePosition(int idxNode);
		virtual int NumberOfNodes();

		virtual void ApplyForce(const glm::vec3& impulse);

		friend class cPhysicsWorld;
		class cNode;
		class cConnection
		{
		public:
			cConnection(cNode* nA, cNode* nB);
			cNode* GetOther(cNode* n);
			cNode* NodeA;
			cNode* NodeB;
			glm::vec3 SeparationA2B;
			float stableDistance;
			
			void Update(float deltaTime);
		};

		class cNode
		{
		public:
			cNode() { 
				isStatic = false; 
				acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
				smoothNormal = glm::vec3(0.0f, 0.0f, 0.0f);
			};
			std::vector<cConnection*> Connections;
			glm::vec3 position;
			glm::vec3 previousPosition; // the position of the particle in the previous time step, used as part of the verlet numerical integration scheme
			glm::vec3 acceleration; // a vector representing the current acceleration of the particle
			glm::vec3 smoothNormal;

			bool isStatic;
			void AddConnectionTo(cNode* other);
			bool HasConnectionTo(cNode* other);
			void AddToPosition(glm::vec3 pos);
			void Update(float deltaTime);
			glm::vec3 GetNormal(void);
			void AddToNormal(glm::vec3 normal);
			void ResetNormal() { smoothNormal = glm::vec3(0.0f, 0.0f, 0.0f); }
		};

		cNode* GetNode(int x, int y);

		enum SOFT_TYPE
		{
			GENERIC,
			CLOTH_SQUARE,
			CLOTH_RECTANGLE
		};
		SOFT_TYPE softType;
		int clothWidth;
		int clothHeight;
		int clothWidth_nodeCount;
		int clothHeight_nodeCount;

		std::vector<cNode*> mNodes;
		std::vector<cConnection*> mConnections;

	private:
		
		void AddConnection(cNode* n1, cNode* n2);
	};
}

#endif // !_CSOFTBODY_HG_
