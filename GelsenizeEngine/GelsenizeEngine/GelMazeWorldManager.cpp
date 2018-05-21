#include "GelMazeWorldManager.h"
#include "managers.h"

#include "GelPhysicsManager.h"

bool GenerateMaze(std::string fileName);

//update
void GelMazeWorldManager::Update(float deltaTime)
{
	
}

void GelMazeWorldManager::Setup()
{
	mazeNodeSize = 12;
	generated = false;
	GenerateMaze("resources/gameFile/mazeFile.bmp");
}

//generate
void GelMazeWorldManager::AllocateMazeWorldWithSize(unsigned int size)
{
	this->mazeWorldSize = size; //allocate
	this->mazeWorldNodes = new MazeNode[size*size];

	//fill all empty tile world
	for (int i = 0; i != size; i++)
	{
		MazeNode node = MazeNode();
		this->mazeWorldNodes[i] = node;
	}

	//release guard
	generated = true;
}

void GelMazeWorldManager::AddNodeToWorld(int  xNode, int  zNode, int red, int green, int blue)
{
	if (!generated) return; //guard

	iShape *tile = GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(6.0f, 0.4f, 6.0f));
	iShape *tile_jump = GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(2.0f, 0.4f, 2.0f));


	int index = GenerateMazeNodeIndex(xNode, zNode);
	if (index < this->mazeWorldSize * this->mazeWorldSize)
	{
		//index safety level: paranoia
		this->mazeWorldNodes[index].x = xNode;
		this->mazeWorldNodes[index].z = zNode;
		this->mazeWorldNodes[index].type = GetNodeTypeForColour(red, green, blue);

		if (this->mazeWorldNodes[index].type == START)
		{
			startNode = this->mazeWorldNodes[index];
		}
		else if (this->mazeWorldNodes[index].type == MONSTER_SPAWN)
		{
			monsterSpawnNode = this->mazeWorldNodes[index];
		}
		else if (this->mazeWorldNodes[index].type == GOAL)
		{
			goalNode = this->mazeWorldNodes[index];
		}
		else if (this->mazeWorldNodes[index].type == ABYSS)
		{
			return; //without adding
		}

		//add collision bodies for tiles
		iGameObject *mazeObject = gelObjectFactory::sharedInstance().createGameObject("object");

		mazeObject->SetObjectName("mazeObject");

		RenderInfo renderMazeObject;
		MeshInfo meshMazeObject;
		meshMazeObject.name = "cube";
		renderMazeObject.useTextureAsDiffuse = false;
		renderMazeObject.useDebugColorsNoLighting = false;
		glm::vec3 position;
		GetPositionForNode(this->mazeWorldNodes[index], position);
		renderMazeObject.position = position;


		renderMazeObject.color.r = 0.0f;
		renderMazeObject.color.g = 0.0f;
		renderMazeObject.color.b = 0.61f;

		if (this->mazeWorldNodes[index].type == EMPTY_TILE)
		{
			renderMazeObject.color.r = 0.0f;
			renderMazeObject.color.g = 0.0f;
			renderMazeObject.color.b = 0.61f;
		}
		else if (this->mazeWorldNodes[index].type == EMPTY_TILE_LEVEL_2)
		{
			renderMazeObject.color.r = 0.80f;
			renderMazeObject.color.g = 0.89f;
			renderMazeObject.color.b = 0.45f;
		}
		else if (this->mazeWorldNodes[index].type == EMPTY_TILE_LEVEL_3)
		{
			renderMazeObject.color.r = 0.55f;
			renderMazeObject.color.g = 0.0f;
			renderMazeObject.color.b = 0.0f;
		}

		

		renderMazeObject.reflectsSkybox = true;

		iShape *currentShape;
		if (this->mazeWorldNodes[index].type == JUMP_TILE)
		{
			currentShape = tile_jump;
			renderMazeObject.scale = 4.0f;

			renderMazeObject.hasGlow = true;
			renderMazeObject.glowColor = glm::vec4(0.02f, 1.0f, 0.04f, 1.0f);
		}
		else if ( this->mazeWorldNodes[index].type == JUMP_TILE_LEVEL_2)
		{
			currentShape = tile_jump;
			renderMazeObject.scale = 4.0f;

			renderMazeObject.hasGlow = true;
			renderMazeObject.glowColor = glm::vec4(1.02f, 0.4f, 0.04f, 1.0f);
		}
		else
		{
			currentShape = tile;
			renderMazeObject.scale = 12.0f;

			renderMazeObject.hasGlow = false;
			renderMazeObject.glowColor = glm::vec4(0.1f, 10.0f, 0.1f, 5.0f);
		}

		

		float meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshMazeObject.name);
		renderMazeObject.SetRadius(meshRadius);
		mazeObject->SetRender(renderMazeObject);
		mazeObject->SetMesh(meshMazeObject);

		//rigid body definition
		cRigidBodyDef bodyDef;
		bodyDef.Static = false;

		bodyDef.Mass = 0.0f;

		bodyDef.friction = 1.0f;
		bodyDef.restitution = 2.0f;
		//bodyDef.linearDamping = 0.2f;
		//bodyDef.angularDamping = 0.6f;
		bodyDef.Position = renderMazeObject.position;
		bodyDef.Velocity.x = 0.0f;// OCHelper::getRandFloat(25.0f, 65.0f);
		bodyDef.Velocity.y = 0.0f;// OCHelper::getRandFloat(0.0f, 3.0f);
		bodyDef.Velocity.z = 0.0f;// OCHelper::getRandFloat(25.0f, 65.0f);
		bodyDef.ownerID = mazeObject->getObjectID();
		bodyDef.ownerObject = mazeObject;
		//generic rigid body class
		iCollisionBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(bodyDef, currentShape);
		PhysicsInfo spherePhyiscsInfo = mazeObject->GetPhysics();
		spherePhyiscsInfo.enabled = true;
		spherePhyiscsInfo.collisionBody = sphereRigidBody;
		GelPhysicsManager::sharedInstance().GetWorld()->AddBody(sphereRigidBody);

		spherePhyiscsInfo.velocity.x = bodyDef.Velocity.x;
		spherePhyiscsInfo.velocity.y = bodyDef.Velocity.y;
		spherePhyiscsInfo.velocity.z = bodyDef.Velocity.z;

		mazeObject->SetPhysics(spherePhyiscsInfo);

		gelObjectManager::sharedInstance().AddGameObject(mazeObject);
		this->mazeWorldNodes[index].gameObject = mazeObject;
	}
}

void GelMazeWorldManager::GetPossibleAdjacentNodes(float xPosition, float zPosition, std::vector<MazeNode> &maze)
{
	int xNode = ceil(xPosition / this->mazeNodeSize);
	int zNode = ceil(zPosition / this->mazeNodeSize);

	//try 4 adjacent
	int adjacentSize = 0;
	//for x,y
	int index = GenerateMazeNodeIndex(xNode, zNode);

	//directions chosen randomly

	//local left  => if index+0 % size != 0  then  (x-1),(z) 
	if (index % this->mazeWorldSize != 0)
	{
		int leftIndex = GenerateMazeNodeIndex(xNode - 1, zNode); //get left tile
		if (leftIndex > mazeWorldSize*mazeWorldSize) { return; }
		MazeNode node = this->mazeWorldNodes[leftIndex];
		if (NodePassable(node.type))
		{
			adjacentSize++;
			maze.push_back(node);
		}
	}

	//local right => if index+1 % size != 0  then  (x+1),(z)
	if ((index + 1) % this->mazeWorldSize != 0)
	{
		int rightIndex = GenerateMazeNodeIndex(xNode + 1, zNode); //get right tile
		if (rightIndex > mazeWorldSize*mazeWorldSize) { return; }
		MazeNode node = this->mazeWorldNodes[rightIndex];
		if (NodePassable(node.type))
		{
			adjacentSize++;
			maze.push_back(node);
		}
	}

	//local front => if index      >= size      then (x),(z-1)
	if (index >= this->mazeWorldSize)
	{
		int frontIndex = GenerateMazeNodeIndex(xNode, zNode - 1); //get front tile
		if (frontIndex > mazeWorldSize*mazeWorldSize) { return; }
		MazeNode node = this->mazeWorldNodes[frontIndex];
		if (NodePassable(node.type))
		{
			adjacentSize++;
			maze.push_back(node);
		}
	}

	//local back  => if index+size >= size*size then (x),(z+1)
	if (index + this->mazeWorldSize <= this->mazeWorldSize*this->mazeWorldSize)
	{
		int backIndex = GenerateMazeNodeIndex(xNode, zNode + 1); //get back tile
		if (backIndex > mazeWorldSize*mazeWorldSize) { return; }
		MazeNode node = this->mazeWorldNodes[backIndex];
		if (NodePassable(node.type))
		{
			adjacentSize++;
			maze.push_back(node);
		}
	}


	////fill the array
	//if (adjacentSize > 0) //unlikely to fail
	//{
	//	MazeNode * passableAdjacentNodes = new MazeNode[adjacentSize];
	//	for (int i = 0; i != tempNodeVector.size(); i++)
	//	{
	//		passableAdjacentNodes[i] = tempNodeVector[i];
	//	}
	//	//write
	//	maze = passableAdjacentNodes;
	//}
	//else
	//{
	//	maze = 0;
	//}
}

void GelMazeWorldManager::GetPossibleAdjacentNodes(MazeNode & node, std::vector<MazeNode>& maze)
{
	glm::vec3 currentNodePosition;
	GetPositionForNode(node, currentNodePosition);
	if (currentNodePosition.x < 0.0f || currentNodePosition.z < 0.0f)
	{
		int x = 3;
	}
	GetPossibleAdjacentNodes(currentNodePosition.x, currentNodePosition.z, maze);
}

//access
void GelMazeWorldManager::GetMazeWorld(MazeNode *& maze)
{
	maze = this->mazeWorldNodes; //set maze array by reference
}

int GelMazeWorldManager::GetNodeIndex(MazeNode & mazeNode)
{
	return this->GenerateMazeNodeIndex(mazeNode.x, mazeNode.z);
}

int GelMazeWorldManager::GetNodeID(MazeNode & mazeNode)
{
	return this->GenerateMazeNodeID(mazeNode.x, mazeNode.z);
}

unsigned int GelMazeWorldManager::GetMazeSize(void)
{
	return this->mazeWorldSize;
}

int GelMazeWorldManager::GetNodeSize(void)
{
	return this->mazeNodeSize;
}

void GelMazeWorldManager::GetStartNode(MazeNode & node)
{
	node.x = this->startNode.x;
	node.z = this->startNode.z;
	node.type = START;
}

void GelMazeWorldManager::GetMonsterSpawnNode(MazeNode & node)
{
	node.x = this->monsterSpawnNode.x;
	node.z = this->monsterSpawnNode.z;
	node.type = START;
}

void GelMazeWorldManager::GetGoalNode(MazeNode & node)
{
	node.x = this->goalNode.x;
	node.z = this->goalNode.z;
	node.type = START;
}

void GelMazeWorldManager::GetNodeAt(int nodeID, MazeNode & node)
{
	int base = 1'00'00'00; //base of mask 1-yy-xx-zz
	nodeID -= base;
	int xNode = nodeID / 1000;
	int zNode = nodeID % 100;

	//int nodeID = GenerateMazeNodeID(xNode, zNode); //dictionary style
	int nodeIndex = GenerateMazeNodeIndex(xNode, zNode); //array style - using this

	node.x = this->mazeWorldNodes[nodeIndex].x;
	node.z = this->mazeWorldNodes[nodeIndex].z;
	node.extra_1 = this->mazeWorldNodes[nodeIndex].extra_1;
	node.extra_2 = this->mazeWorldNodes[nodeIndex].extra_2;
	node.type = this->mazeWorldNodes[nodeIndex].type;

	//node = this->mazeWorldNodesMap[nodeID];

}

void GelMazeWorldManager::GetNodeAt(int xNode, int zNode, MazeNode & node)
{

	int nodeID = GenerateMazeNodeID(xNode, zNode); //dictionary style
	int nodeIndex = GenerateMazeNodeIndex(xNode, zNode); //array style - using this

	node.x = this->mazeWorldNodes[nodeIndex].x;
	node.z = this->mazeWorldNodes[nodeIndex].z;
	node.extra_1 = this->mazeWorldNodes[nodeIndex].extra_1;
	node.extra_2 = this->mazeWorldNodes[nodeIndex].extra_2;
	node.type = this->mazeWorldNodes[nodeIndex].type;

	//node = this->mazeWorldNodesMap[nodeID];

}

void GelMazeWorldManager::GetNodeAt(float xPosition, float zPosition, MazeNode & node)
{
	int xNode = floor(xPosition / this->mazeNodeSize);
	int zNode = floor(zPosition / this->mazeNodeSize);

	int nodeID = GenerateMazeNodeID(xNode, zNode); //dictionary style
	int nodeIndex = GenerateMazeNodeIndex(xNode, zNode); //array style - using this

	node.x = this->mazeWorldNodes[nodeIndex].x;
	node.z = this->mazeWorldNodes[nodeIndex].z;
	node.extra_1 = this->mazeWorldNodes[nodeIndex].extra_1;
	node.extra_2 = this->mazeWorldNodes[nodeIndex].extra_2;
	node.type = this->mazeWorldNodes[nodeIndex].type;

	//node = this->mazeWorldNodesMap[nodeID];

}

void GelMazeWorldManager::GetPositionForNode(MazeNode node, glm::vec3 & position)
{
	float offset;
	if (node.type == JUMP_TILE ||
		node.type == JUMP_TILE_LEVEL_2)
	{
		offset = 12.0f;
	}
	else
	{
		offset = 12.0f;
	}

	position.x = (node.x * this->mazeNodeSize) + offset / 2.0f;
	position.y = 0.0f;
	position.z = (node.z * this->mazeNodeSize) + offset / 2.0f;
}

int GelMazeWorldManager::GetDistanceBetweenNodes(MazeNode & node1, MazeNode & node2)
{
	return abs(node1.x - node2.x) + abs(node1.z - node2.z);
}

//help
GelMazeWorldManager::MazeNodeType GelMazeWorldManager::GetNodeTypeForColour(int red, int green, int blue)
{
	if (red == 255 &&
		green == 0 &&
		blue == 0)
	{
		return MazeNodeType::EMPTY_TILE;
	}
	else if (
		red == 0 &&
		green == 255 &&
		blue == 0)
	{
		return MazeNodeType::EMPTY_TILE_LEVEL_2;
	}
	else if (
		red == 0 &&
		green == 0 &&
		blue == 255)
	{
		return MazeNodeType::EMPTY_TILE_LEVEL_3;
	}
	else if (
		red == 255 &&
		green == 0 &&
		blue == 255)
	{
		return MazeNodeType::JUMP_TILE;
	}
	else if (
		red == 0 &&
		green == 255 &&
		blue == 255)
	{
		return MazeNodeType::JUMP_TILE_LEVEL_2;
	}
	else if (
		red == 255 &&
		green == 255 &&
		blue == 0)
	{
		return MazeNodeType::PORTAL;
	}
	else if (
		red == 255 &&
		green == 255 &&
		blue == 255)
	{
		return MazeNodeType::START;
	}
	else if (
		red == 0 &&
		green == 0 &&
		blue == 0)
	{
		return MazeNodeType::ABYSS;
	}
	else
	{
		return MazeNodeType::START;
	}
}

int GelMazeWorldManager::GenerateMazeNodeIndex(int  xNode, int  zNode)
{
	//this will be adjusted depending on the order bmp is read
	//right now:
	return (int)xNode + (int)zNode*this->mazeWorldSize;
}

int GelMazeWorldManager::GenerateMazeNodeID(int  xNode, int  zNode)
{
	//1'yy'xx'zz; 
	unsigned int base = 1'00'00'00;
	unsigned int zPart = (unsigned int)(zNode) * 1;
	//unsigned int yPart = (unsigned int)(yNode) * 1000;
	unsigned int xPart = (unsigned int)(xNode) * 1000;
	unsigned int ID = base + xPart + zPart;
	return ID;
}

bool GelMazeWorldManager::NodePassable(MazeNodeType type)
{
	if (type == EMPTY_TILE     ||
		type == EMPTY_TILE_LEVEL_2||
		type == PORTAL         ||
		//type == GOAL         ||
		type == START)
	{
		return true;
	}
	return false;
}

//bmp part



// These functions shift numbers by one, two, and three bytes.
unsigned long ShiftAndAdd_Byte_to_ULong(unsigned long theULong, char theByte, int bytesToShift)
{
	// Too many bytes to shift (there are only 4 bytes in a long, so max is 3)?
	if (bytesToShift > 3)
	{
		bytesToShift = 0;
	}
	unsigned short bitsToShift = bytesToShift * 8;	// 8 bits / byte.
													// Now, convert the uChar to a uLong, 
													//	shift it the number of bits,
													//	then or it (add it) to the long
	unsigned char uByte = theByte;
	theULong = theULong | (static_cast<unsigned long>(uByte) << bitsToShift);
	return theULong;
}

unsigned short ShiftAndAdd_Byte_to_UShort(unsigned short theUShort, char theByte, int bytesToShift)
{
	// Too many bytes to shift (there is only 2 bytes in a long, so max is 1)?
	if (bytesToShift > 1)
	{
		bytesToShift = 0;
	}
	unsigned short bitsToShift = bytesToShift * 8;	// 8 bits / byte.
													// Now, convert the uChar to a uLong, 
													//	shift it the number of bits,
													//	then or it (add it) to the long
	unsigned char uByte = theByte;
	theUShort = theUShort | (static_cast<unsigned long>(uByte) << bitsToShift);
	return theUShort;
}

// These are used to read the data from a file (LoadBMP2 method).
// * index is incremented appropriate amount (to next char)
// * no bounds checking for array
char ReadNextChar(char* data, unsigned long &index)
{
	char returnVal = static_cast<unsigned char>(data[index]);
	index++;
	return returnVal;
}

unsigned short ReadNextUnsignedShort(char* data, unsigned long &index)
{
	unsigned short usTheReturnVal = 0;
	usTheReturnVal = ShiftAndAdd_Byte_to_UShort(usTheReturnVal, static_cast<unsigned char>(data[index]), 0);
	index++;
	usTheReturnVal = ShiftAndAdd_Byte_to_UShort(usTheReturnVal, static_cast<unsigned char>(data[index]), 1);
	index++;
	return usTheReturnVal;
}

unsigned long ReadNextUnsignedLong(char* data, unsigned long &index)
{
	unsigned long ulTheReturnVal = 0;
	// Read four bytes, but return them in reverse order.
	ulTheReturnVal = ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(data[index]), 0);
	index++;
	ulTheReturnVal = ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(data[index]), 1);
	index++;
	ulTheReturnVal = ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(data[index]), 2);
	index++;
	ulTheReturnVal = ShiftAndAdd_Byte_to_ULong(ulTheReturnVal, static_cast<unsigned char>(data[index]), 3);
	index++;
	return ulTheReturnVal;
}

bool GenerateMaze(std::string fileName)//from bmp
{
	bool m_bIsCubeMap;
	bool m_bIs2DTexture;
	unsigned long m_Height;
	unsigned long m_Width;
	unsigned long m_OriginalHeight;
	unsigned long m_OriginalWidth;
	unsigned short m_reserved1;
	unsigned short m_reserved2;
	unsigned long m_FileSize;
	unsigned long m_offsetInBits;
	unsigned long m_headerSize;
	unsigned long m_numberOfRows;
	unsigned long m_numberOfColumns;
	unsigned short m_numberOfPlanes;
	unsigned short m_bitPerPixel;	    // 1, 4, 8, 15, 24, 32, or 64
	unsigned long m_compressionMode;	// 0 if NOT compressed (i.e. 24 bit bitmap)
	unsigned long m_imageSizeInBytes;	// Total bytes in image - may be zero
	unsigned long m_PixelsPerMeterX;
	unsigned long m_PixelsPerMeterY;
	unsigned long m_numberOfLookUpTableEntries;	// Could be 0 for 24 bit format
	unsigned long m_numberOfImportantColours;	// 0 if all colours are important

												//C24BitBMPpixel* m_p_theImages;

												// This is for error handling:
	int m_lastErrorNum;
	enum ERORRCONSTANTS
	{
		ERROR_NO_ERROR = 0,
		ERORR_FILE_WONT_OPEN = 1,
		ERROR_NOT_A_BMP_FILE = 2,
		ERROR_NOT_A_24_BIT_BITMAP = 3,
		ERROR_NOT_ENOUGHT_MEMORY_FOR_BITMAP = 4
	};
	//
	bool m_bHave_cout_output = false;

	std::ifstream theFile;
	if (m_bHave_cout_output)
	{
		std::cout << "Reading texture file: " << fileName;
	}
	theFile.open(fileName.c_str(), std::ios_base::binary);
	if (!theFile)
	{
		m_lastErrorNum = ERORRCONSTANTS::ERORR_FILE_WONT_OPEN;
		return false;
	}
	// Scan to the end to get the size
	theFile.seekg(0, std::ios::end);
	std::ios::pos_type fileSize = theFile.tellg();
	// Return to start
	theFile.seekg(0, std::ios::beg);

	char* pRawData = new char[static_cast<unsigned int>(fileSize)];

	theFile.read(pRawData, fileSize);
	theFile.close();

	// Now go through and decode the BMP file.
	unsigned long curIndex = 0;
	char letter1 = ReadNextChar(pRawData, curIndex);
	char letter2 = ReadNextChar(pRawData, curIndex);
	if ((letter1 != 'B') && (letter2 != 'M'))
	{
		m_lastErrorNum = ERORRCONSTANTS::ERROR_NOT_A_BMP_FILE;
		return false;
	}
	// File is BMP
	m_FileSize = ReadNextUnsignedLong(pRawData, curIndex);
	m_reserved1 = ReadNextUnsignedShort(pRawData, curIndex);
	m_reserved2 = ReadNextUnsignedShort(pRawData, curIndex);
	m_offsetInBits = ReadNextUnsignedLong(pRawData, curIndex);
	m_headerSize = ReadNextUnsignedLong(pRawData, curIndex);
	m_numberOfRows = ReadNextUnsignedLong(pRawData, curIndex);
	m_Height = m_OriginalHeight = m_numberOfRows;
	m_numberOfColumns = ReadNextUnsignedLong(pRawData, curIndex);
	m_Width = m_OriginalWidth = m_numberOfColumns;
	m_numberOfPlanes = ReadNextUnsignedShort(pRawData, curIndex);
	m_bitPerPixel = ReadNextUnsignedShort(pRawData, curIndex);
	// Is is a 24 bit bitmap?
	if (m_bitPerPixel != 24)
	{
		m_lastErrorNum = ERORRCONSTANTS::ERROR_NOT_A_24_BIT_BITMAP;
		return false;
	}

	m_compressionMode = ReadNextUnsignedLong(pRawData, curIndex);
	m_imageSizeInBytes = ReadNextUnsignedLong(pRawData, curIndex);
	m_PixelsPerMeterX = ReadNextUnsignedLong(pRawData, curIndex);
	m_PixelsPerMeterY = ReadNextUnsignedLong(pRawData, curIndex);
	m_numberOfLookUpTableEntries = ReadNextUnsignedLong(pRawData, curIndex);
	m_numberOfImportantColours = ReadNextUnsignedLong(pRawData, curIndex);

	// 
	long bytesPerRow = ((3 * m_numberOfRows + 3) / 4) * 4;
	long numberOfPaddingBytes = bytesPerRow - 3 * m_numberOfColumns;

	// Allocate enough space...
	//m_p_theImages = new C24BitBMPpixel[m_numberOfRows * m_numberOfColumns];

	// Did we run out of memory?
	//if (!m_p_theImages)
	//{
	//	m_lastErrorNum = ERORRCONSTANTS::ERROR_NOT_ENOUGHT_MEMORY_FOR_BITMAP;
	//	return false;
	//}


	// Read the bitmap into memory...
	unsigned long bytesRead = 0;
	unsigned long totalBytesInFile = m_numberOfRows * m_numberOfColumns;
	unsigned long pixelCount = 0;

	//init maze world
	GelMazeWorldManager::sharedInstance().AllocateMazeWorldWithSize(m_numberOfColumns);

	for (unsigned long row = 0; row < m_numberOfRows; row++)
	{
		for (unsigned long col = 0; col < m_numberOfColumns; col++)
		{
			int theblue = (int)ReadNextChar(pRawData, curIndex);
			int thegreen = (int)ReadNextChar(pRawData, curIndex);
			int thered = (int)ReadNextChar(pRawData, curIndex);

			//m_p_theImages[pixelCount].redPixel = static_cast<unsigned char>(thered);
			//m_p_theImages[pixelCount].greenPixel = static_cast<unsigned char>(thegreen);
			//m_p_theImages[pixelCount].bluePixel = static_cast<unsigned char>(theblue);
			//m_p_theImages[pixelCount].alphaPixel = 255;

			//write these into game world.
			GelMazeWorldManager::sharedInstance().AddNodeToWorld(m_numberOfColumns - (col + 1), row, (int)(unsigned char)thered, (int)(unsigned char)thegreen, (int)(unsigned char)theblue);
			
			pixelCount++;
			bytesRead += 3;	// Add three bytes



		}// for (unsigned long col
		 // We are at the end of the row and there will 
		 //	still be bytes padding up to a multiple of 4...
		 // Read and discard these...
		for (int discardedBytes = 0; discardedBytes < numberOfPaddingBytes; discardedBytes++)
		{
			char tempChar = ReadNextChar(pRawData, curIndex);
		}
		// How much have we read?
		// NOTE: This really isn't percent, but it does the job of 
		//	printing out fewer dots.
		int percentRead = static_cast<int>((static_cast<double>(bytesRead) / static_cast<double>(totalBytesInFile)) * 100);
		if ((percentRead % 25) == 0)
		{
			if (m_bHave_cout_output)
			{
				std::cout << ".";
			}
		}
	}// for (unsigned long row...
	if (m_bHave_cout_output)
	{
		std::cout << "complete." << std::endl;
	}
	// BMP file is read....

	return true;
}