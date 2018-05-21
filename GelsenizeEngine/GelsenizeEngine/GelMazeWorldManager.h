#pragma once
#ifndef _GELMAZEWORLDMANAGER_HG_
#define _GELMAZEWORLDMANAGER_HG_

#include "iManager.h"
#include "iGameObject.h"

#include <vector>;
#include <string>;

class GelMazeWorldManager : public iManager
{
public:
	//Singleton 
	static GelMazeWorldManager& sharedInstance()
	{
		static GelMazeWorldManager instance;
		return instance;
	}

	enum MazeNodeType
	{
		WALL,             //unpassable object
		WALL_INVISIBLE,   //invisible unpassable object 
		ILLUSIONARY_WALL, //passable, looks like a wall

		EMPTY_TILE,           //walkable terrain
		EMPTY_TILE_LEVEL_2,   //walkable terrain alternative,
		EMPTY_TILE_LEVEL_3,

		JUMP_TILE,
		JUMP_TILE_LEVEL_2,

		MONSTER_SPAWN,
		ABYSS,
		GOAL,             //
		PORTAL,           //takes to 
		START,            //
		BONEFIRE          //monsters cannot get close, a safe haven
	};

	struct MazeNode
	{
		MazeNode() : x(0), z(0), extra_1(0.0f), extra_2(0.0f), type(EMPTY_TILE), cost(0.0f) {}
		bool GelMazeWorldManager::MazeNode::operator<(const MazeNode& right) const;
		int x;
		int z;
		float extra_1;
		float extra_2;
		float cost;
		MazeNodeType type;
		iGameObject *gameObject;
	};

	void AllocateMazeWorldWithSize(unsigned int size);
	void AddNodeToWorld(int xNode, int zNode, int red, int green, int blue);

	void GetPossibleAdjacentNodes(float  xPosition, float zPosition, std::vector<MazeNode> &maze);
	void GetPossibleAdjacentNodes(MazeNode &node, std::vector<MazeNode> &maze);

	void GetStartNode(MazeNode &node);
	void GetMonsterSpawnNode(MazeNode &node);
	void GetGoalNode(MazeNode &node);

	void GetNodeAt(int nodeID, MazeNode & node);
	void GetNodeAt(float xPosition, float  zPosition, MazeNode& node);
	void GetNodeAt(int xNode, int zNode, MazeNode & node);

	void GetPositionForNode(MazeNode node, glm::vec3 &position);
	void GetMazeWorld(MazeNode* &maze);
	int GetDistanceBetweenNodes(MazeNode &node1, MazeNode &node2);
	int GetNodeIndex(MazeNode &mazeNode);
	int GetNodeID(MazeNode &mazeNode);
	unsigned int GetMazeSize(void);
	int GetNodeSize(void);

	//update non-simulation related maybe
	virtual void Update(float deltaTime);
	void Setup();

private:
	GelMazeWorldManager() {};
	GelMazeWorldManager(GelMazeWorldManager const&); //dont implement - to build
	void operator=(GelMazeWorldManager const&); //dont implement - to build

	MazeNodeType GetNodeTypeForColour(int red, int green, int blue);
	int GenerateMazeNodeIndex(int  xNode, int  zNode);
	int GenerateMazeNodeID(int  xNode, int  zNode);
	bool NodePassable(MazeNodeType type);

	MazeNode* mazeWorldNodes; //array
	MazeNode startNode;
	MazeNode goalNode;
	MazeNode monsterSpawnNode;

	int mazeNodeSize; //width of each node square in units
	unsigned int mazeWorldSize; //square world, on X,Z
	bool generated;
};

#endif // !_GELMAZEWORLDMANAGER_HG_
