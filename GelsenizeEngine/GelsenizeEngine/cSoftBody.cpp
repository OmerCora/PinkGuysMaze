#include "cSoftBody.h"

namespace nPhysics
{
	cSoftBody::cConnection::cConnection(cNode* nA, cNode* nB)
	{
		NodeA = nA;
		NodeB = nB;
		
		stableDistance = glm::distance(nA->position, nB->position);
	}

	cSoftBody::cNode* cSoftBody::cConnection::GetOther(cSoftBody::cNode* n)
	{
		return n == NodeA ? NodeB : NodeA;
	}

	void cSoftBody::cConnection::Update(float deltaTime)
	{
		glm::vec3 A_to_B = this->NodeB->position - this->NodeA->position;
		float currentDistance = glm::distance(this->NodeA->position, this->NodeB->position); // current distance between p1 and p2

		if (currentDistance > 30.0f)
		{
			int x = 0;
		}

		glm::vec3 stableOffset = A_to_B * (1.0f - (this->stableDistance / currentDistance)); // The offset vector that could moves p1 into a distance of rest_distance to p2

		//move both nodes half the distance
		glm::vec3 stableOffsetHalf = stableOffset * 0.5f; 

		if (glm::length(stableOffset) >= 100.0f)
		{
			int x = 0;
		}

		NodeA->AddToPosition(stableOffsetHalf); // correctionVectorHalf is pointing from p1 to p2, so the length should move p1 half the length needed to satisfy the constraint.
		NodeB->AddToPosition(-stableOffsetHalf); // we must move p2 the negative direction of correctionVectorHalf since it points from p2 to p1, and not p1 to p2.
	}

	void cSoftBody::cNode::AddConnectionTo(cSoftBody::cNode* other)
	{
		if (HasConnectionTo(other))
			return;
		Connections.push_back(new cConnection(this, other));
	}

	bool cSoftBody::cNode::HasConnectionTo(cSoftBody::cNode* other)
	{
		std::vector<cConnection*>::iterator it = Connections.begin();
		while (it != Connections.end())
		{
			if ((*it)->GetOther(this) == other)
				return true;
			it++;
		}
		return false;
	}

	void cSoftBody::cNode::AddToPosition(glm::vec3 pos)
	{
		if (!isStatic)
		{
			glm::vec3 tempPos = pos;
			glm::vec3 tempPos2 = position;
			if (glm::length(position) != glm::length(position))
			{
				int x = 0;
			}
			position += pos;
			if (glm::length(position) != glm::length(position))
			{
				int x = 0;
			}
		}
		else
		{
			int x = 0;
		}
	}

	void cSoftBody::cNode::Update(float deltaTime)
	{
		//verlet integration but just impulse acceleration no intrinsic velocity
		if (!isStatic)
		{
			glm::vec3 temp = position;
			glm::vec3 dampedDistance = (position - previousPosition)* 0.99f;
			this->position += dampedDistance + acceleration * deltaTime;

			previousPosition = temp;
			acceleration = glm::vec3(0.0f, 0.0f, 0.0f); 
		}
		else
		{
			int x = 0;
		}
	}

	glm::vec3 cSoftBody::cNode::GetNormal(void)
	{
		return glm::normalize(smoothNormal);
	}

	void cSoftBody::cNode::AddToNormal(glm::vec3 normal)
	{
		smoothNormal += glm::normalize(normal);
	}

	void cSoftBody::GetSpecialScale(glm::vec3 & specialScale)
	{
	}

	glm::vec3 cSoftBody::GetNodePosition(int idxNode)
	{
		if (idxNode >= mNodes.size())
		{
			return glm::vec3(0.0f, 0.0f, 0.0f);
		}
		cNode *nodeWithIndex = mNodes[idxNode];
		return nodeWithIndex->position;
	}

	cSoftBody::cNode* cSoftBody::GetNode(int x, int y)
	{
		int calculatedIndex = y*clothWidth_nodeCount + x;

		if (calculatedIndex >= mNodes.size())
		{
			return 0;
		}

		return mNodes[calculatedIndex];
	}

	void cSoftBody::AddConnection(cNode * n1, cNode * n2)
	{
		cConnection *connection = new cConnection(n1, n2);
		this->mConnections.push_back(connection);
	}


	int cSoftBody::NumberOfNodes()
	{
		return mNodes.size();
	}

	void cSoftBody::ApplyForce(const glm::vec3 & impulse)
	{
		std::vector<cNode*>::iterator iterator;
		for (iterator = this->mNodes.begin(); iterator != this->mNodes.end(); iterator++)
		{
			//1.0f is inverse mass;
			cNode *node = (*iterator);
			node->acceleration = node->acceleration + impulse * 1.0f; // add the forces to each particle
		}
	}


	/* This is a important constructor for the entire system of particles and constraints*/
	void cSoftBody::GenerateRectangleCloth(glm::vec3 initialPosition, float width, float height, int width_nodeCount, int height_nodeCount)
	{
		this->mConnections.clear();
		this->mNodes.clear();

		this->clothWidth = width;
		this->clothHeight = height;

		this->clothWidth_nodeCount = width_nodeCount;
		this->clothHeight_nodeCount = height_nodeCount;

		this->mNodes.resize(width_nodeCount*height_nodeCount);

		//create nodes basing 0,0 to top left corner

		for (int x = 0; x != clothWidth_nodeCount; x++)
		{
			for (int y = 0; y != clothHeight_nodeCount; y++)
			{
				//glm::vec3 initialPosition(10.0f, 10.0f, 10.0f);
				
				glm::vec3 position = glm::vec3(
					+this->clothWidth  * (x / (float)clothWidth_nodeCount ),
					-this->clothHeight * (y / (float)clothHeight_nodeCount),
					0.0f);

				cNode *node = new cNode();
				node->position = position + initialPosition;
				node->previousPosition = node->position;
				
				//place node, considering 2 dimensions
				this->mNodes[y*width_nodeCount + x] = node;
			}
		}

		//Connect immediate nodes to each other 

		//(left / top / bottom / cross) pieces of a rectangle, next's iteration's left, fills this one's right to fill pattern 

		for (int x = 0; x != clothWidth_nodeCount; x++)
		{
			for (int y = 0; y != clothHeight_nodeCount; y++)
			{
				if (x < clothWidth_nodeCount - 1)
				{
					this->AddConnection(GetNode(x, y), GetNode(x + 1, y));
				}
				if (y < clothHeight_nodeCount - 1)
				{
					this->AddConnection(GetNode(x, y), GetNode(x, y + 1));
				}
				if (x < clothWidth_nodeCount - 1 && y < clothHeight_nodeCount - 1)
				{
					this->AddConnection(GetNode(x, y), GetNode(x + 1, y + 1));
				}
				if (x < clothWidth_nodeCount - 1 && y < clothHeight_nodeCount - 1)
				{
					this->AddConnection(GetNode(x + 1, y), GetNode(x, y + 1));
				}
			}
		}

		//Same as above but with 2 immediate neighbours are marked
		for (int x = 0; x != clothWidth_nodeCount; x++)
		{
			for (int y = 0; y != clothHeight_nodeCount; y++)
			{
				if (x < clothWidth_nodeCount - 2)
				{
					this->AddConnection(GetNode(x, y), GetNode(x + 2, y));
				}
				if (y < clothHeight_nodeCount - 2)
				{
					this->AddConnection(GetNode(x, y), GetNode(x, y + 2));
				}
				if (x < clothWidth_nodeCount - 2 && y < clothHeight_nodeCount - 2)
				{
					this->AddConnection(GetNode(x, y), GetNode(x + 2, y + 2));
				}
				if (x < clothWidth_nodeCount - 2 && y < clothHeight_nodeCount - 2)
				{
					this->AddConnection(GetNode(x + 2, y), GetNode(x, y + 2));
				}
			}
		}


		// making the upper left most three and right most three particles unmovable
		for (int i = 0; i < 3; i++)
		{
			cNode *node1 = GetNode(0 + i, 0);
			node1->AddToPosition(glm::vec3(+0.5f, 0.0f, 0.0f));
			node1->isStatic = true;
			
			cNode *node2 = GetNode(width_nodeCount - 1 - i, 0);
			node2->AddToPosition(glm::vec3(-0.5f, 0.0f, 0.0f));
			node2->isStatic = true;
		}
	}
}