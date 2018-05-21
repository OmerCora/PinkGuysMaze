//GELSENIZE ENGINE

//GELSENIZE GELSENIZE GELSENIZE

//Aimed to be developed in parallel with the progress of GDP classes.
//Main intention is to create a "game engine" on the go, with 0 game development experience.
//Reusing bunch of lines of code from the classes =)

//Omer Cora, 2016

#include "GelsenizeCore.h"

#include "managers.h"
#include "cContact.h"

//TODO: config
#define GELSENIZE_BUILD 62
#define GELSENIZE_VER NULL

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>		// "String Stream", used for setting the windows title

#include <vector>
#include <fstream>
#include <string>
#include "iGameObject.h"

#include "gelObjectBuilder.h"
#include "gelWorldManager.h" //world object mediator
#include "GelCommandBuilder.h"
//JSON
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <iostream>

#include "Utilities.h"
#include "OCHelper.h"
#include "commands.h"

#include "cFlock.h"

#include "ExecuteCommands.h"

#include "LuaBrain.h"

//custom cloth physics 
#include "cSoftBody.h"

#include "GameFileReader.h"
#include "GelMazeWorldManager.h"

//
////abstraction soon
//void PhysicsStep(float deltaTime);
//void CollisionStep(float deltaTime);
//float getRandFloat(float LO, float HI);
//std::vector< cContact > g_vec_Contacts;

//gelMeshManager::Mesh* g_meshx;
iGameObject *g_ambientLight0;
iGameObject *g_uppersnake;
int regularDraw = 3;
int FPS_COUNTER;
const GLuint SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
GLuint soilTex;

bool g_bloomActive = true;

glm::vec4 g_lightDir(10.0f, -5.0f, 10.0f, 0.0f);

static void error_callback(int error, const char* description)
{
	fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GelPhysicsManager::sharedInstance().GetWorld()->KeyboardCallback(key, action);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	//Wireframe mode toggle for each game object
	if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		std::vector<iGameObject*> gameObjects = gelObjectManager::sharedInstance().GetGameObjects();

		for (int index = 0; index != gameObjects.size(); index++)
		{
			RenderInfo render = gameObjects[index]->GetRender();
			render.isWireframe = !render.isWireframe;
			gameObjects[index]->SetRender(render);
		}
	}
	if (key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		GelCommandManager::sharedInstance().FlushCommands();
	}
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		regularDraw = (regularDraw + 1) % 4;

	}
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
	{

	}
	if (key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		iGameObject* player = gelObjectManager::sharedInstance().GetMainPlayer();
		glm::vec3 playerPosition = player->GetRender().position;

		float sphereRadius = OCHelper::getRandFloat(1.0f, 4.0f);
		iGameObject *sphere = gelObjectFactory::sharedInstance().createGameObject("object");
		RenderInfo renderSphere;
		MeshInfo meshSphere;
		meshSphere.name = "sphere_nice";
		renderSphere.useTextureAsDiffuse = false;
		renderSphere.useDebugColorsNoLighting = false;
		renderSphere.position = playerPosition;

		renderSphere.color.r = 1.0f;
		renderSphere.color.g = 0.0f;
		renderSphere.color.b = 0.0f;
		renderSphere.scale = 0.5f;
		float meshRadius;
		meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshSphere.name);
		renderSphere.SetRadius(meshRadius);
		sphere->SetRender(renderSphere);
		sphere->SetMesh(meshSphere);

		//rigid body definition
		cRigidBodyDef bodyDef;
		bodyDef.Static = false;
		bodyDef.Mass = 5.0f; //same as radius
		bodyDef.friction = 5.0f;
		bodyDef.restitution = 0.0f;

		glm::vec4 forward(0.0f, 0.0f, 100.0f, 0.0f);
		glm::vec3 impulse = player->GetRender().transform * forward;

		//bodyDef.linearDamping = 0.2f;
		//bodyDef.angularDamping = 0.6f;
		bodyDef.Position = renderSphere.position;
		bodyDef.ownerID = sphere->getObjectID();
		bodyDef.ownerObject = sphere;
		//generic rigid body class
		iCollisionBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
			bodyDef,
			GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sSphere(0.5f)));

		PhysicsInfo spherePhyiscsInfo = sphere->GetPhysics();
		spherePhyiscsInfo.enabled = true;
		spherePhyiscsInfo.collisionBody = sphereRigidBody;
		spherePhyiscsInfo.velocity = bodyDef.Velocity;
		GelPhysicsManager::sharedInstance().GetWorld()->AddBody(sphereRigidBody);

		impulse = glm::normalize(impulse);
		((iRigidBody*)sphereRigidBody)->ApplyImpulseAtPoint(impulse, renderSphere.position);

		sphere->SetPhysics(spherePhyiscsInfo);

		gelObjectManager::sharedInstance().AddGameObject(sphere);


		//gelSoundManager::sharedInstance().IncreasePan();
	}
	if (key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		//g_lightDir.z += 0.2;
		//g_bloomActive = !g_bloomActive;
		//gelSoundManager::sharedInstance().DecreasePitch();
	}
	if (key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		//g_lightDir.z -= 0.2;
		//gelSoundManager::sharedInstance().IncreasePitch();
	}
	if (key == GLFW_KEY_7 && action == GLFW_PRESS)
	{
		//g_lightDir.x += 0.2;
		//gelSoundManager::sharedInstance().IncreasePitch();
	}
	if (key == GLFW_KEY_8 && action == GLFW_PRESS)
	{
		//g_lightDir.x -= 0.2;
		//gelSoundManager::sharedInstance().IncreasePitch();
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		
	}

	glm::vec4 eyeOut;
	gelCameraManager::sharedInstance().mainCamera->GetEyePosition(eyeOut);
	GelPosition playerPosition = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender().position;
	GelPosition vehiclePosition = gelObjectManager::sharedInstance().GetVehicle()->GetRender().position;
	

	std::stringstream ssTitle;

	ssTitle << "FPS: " << FPS_COUNTER
		<< "  Player Position: "
		<< playerPosition.x << ", "
		<< playerPosition.y << ", "
		<< playerPosition.z;
	/*ssTitle << "Camera: "
		<< eyeOut.x << ", "
		<< eyeOut.y << ", "
		<< eyeOut.z
		<< "  Player: "
		<< playerPosition.x << ", "
		<< playerPosition.y << ", "
		<< playerPosition.z;*/
	glfwSetWindowTitle(window, ssTitle.str().c_str());

	return;
}

static void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	GelsenizeCore::sharedInstance().mousePositionX = (float)xPos;
	GelsenizeCore::sharedInstance().mousePositionY = (float)yPos;
	GelPhysicsManager::sharedInstance().GetWorld()->MousePositionCallback((float)xPos, (float)yPos);
}



// game loop below

void GelsenizeCore::StartGameLoop(void)
{
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	window = glfwCreateWindow(1920, 1080, "Gelsenize Engine", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);

	std::cout << glGetString(GL_VENDOR) << " "
		<< glGetString(GL_RENDERER) << ", "
		<< glGetString(GL_VERSION) << std::endl;
	std::cout << "Shader language version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

	//	std::cout << glGetString(GL_EXTENSIONS) << std::endl;

	//read shader files and compile the shaders
	ProcessShaderFiles();

	GLuint programID = gelShaderManager::sharedInstance().getIDWithName("default");
	
	shaderUniformGroup groupDefault;

	groupDefault.locID_matModel = glGetUniformLocation(programID, "matModel");
	groupDefault.locID_matView = glGetUniformLocation(programID, "matView");
	groupDefault.locID_matProj = glGetUniformLocation(programID, "matProj");

	//this turned into light manager's setup
	//locID_lightPosition = glGetUniformLocation(programID, "myLightPosition");
	//locID_lightDiffuse = glGetUniformLocation(programID, "myLightDiffuse");
	//now new code
	gelLightManager::sharedInstance().SetupUniforms(programID);

	//uniformId_isWireframe = glGetUniformLocation(programID, "isWireframe"); //unused for now
	groupDefault.uniformId_ModelColor = glGetUniformLocation(programID, "objectColour");
	groupDefault.uniformId_userDebugColorsNoLighting = glGetUniformLocation(programID, "bUseDebugDiffuseNoLighting");
	groupDefault.uniformId_eyeLocation = glGetUniformLocation(programID, "eyeLocation");
	groupDefault.uniformId_textureExtra = glGetUniformLocation(programID, "textureExtra");

	//uniformId_alpha = glGetUniformLocation(programID, "globalAlphaValue");
	//uniformId_use_alpha= glGetUniformLocation(programID, "useGlobalAlphaValue");

	groupDefault.uniformLoc_texSamp2D[0] = glGetUniformLocation(programID, "texSamp2D_00");
	groupDefault.uniformLoc_texSamp2D[1] = glGetUniformLocation(programID, "texSamp2D_01");
	groupDefault.uniformLoc_texSamp2D[2] = glGetUniformLocation(programID, "texSamp2D_02");
	groupDefault.uniformLoc_texSamp2D[3] = glGetUniformLocation(programID, "texSamp2D_03");
	groupDefault.uniformLoc_texSamp2D[4] = glGetUniformLocation(programID, "texSamp2D_04");
	groupDefault.uniformLoc_texSamp2D[5] = glGetUniformLocation(programID, "texSamp2D_05");

	groupDefault.uniformLoc_bUseTextureAsDiffuse = glGetUniformLocation(programID, "bUseTextureAsDiffuse");
	groupDefault.uniformLoc_bUseLighting = glGetUniformLocation(programID, "bUseLighting");
	groupDefault.uniformLoc_bIsDiscardTextureBlack = glGetUniformLocation(programID, "bIsDiscardTextureBlack");
	//uniformLoc_bIsImposterBlackIsAlpha = glGetUniformLocation(programID, "bIsImposterBlackIsAlpha");

	groupDefault.uniformLoc_bIsCubeMapObject = glGetUniformLocation(programID, "bIsCubeMapObject");
	groupDefault.uniformLoc_myCubemapSkyBoxSampler = glGetUniformLocation(programID, "myCubemapSkyBoxSampler");

	groupDefault.uniformLoc_usedTexNum = glGetUniformLocation(programID, "usedTexNum");
	groupDefault.uniformId_object_alpha = glGetUniformLocation(programID, "alpha");

	groupDefault.uniformId_cloth_pass = glGetUniformLocation(programID, "clothPass");
	groupDefault.uniformLoc_backFaceDrawPassMode = glGetUniformLocation(programID, "backFaceDrawPassMode"); //1 = back face, 2 = white, 3 = regular draw 
	groupDefault.uniformLoc_backFaceColor = glGetUniformLocation(programID, "backFaceColor");
	groupDefault.uniformLoc_backFaceOffset = glGetUniformLocation(programID, "backFaceOffset");

	//skinned mesh shader info
	groupDefault.uniformLoc_bIsASkinnedMesh = glGetUniformLocation(programID, "bIsASkinnedMesh");
	groupDefault.uniformLoc_numberOfBonesUsed = glGetUniformLocation(programID, "numberOfBonesUsed");
	groupDefault.uniformLoc_slideDistance = glGetUniformLocation(programID, "slideDistance");

	groupDefault.uniformLoc_materialDiffuse = glGetUniformLocation(programID, "materialDiffuse");
	groupDefault.uniformLoc_materialSpecular = glGetUniformLocation(programID, "materialSpecular");

	// For the 2nd pass rendering.
	this->uniformLoc_offscreenColorTexture = glGetUniformLocation(programID, "offscreenColorTexture");
	this->uniformLoc_bIsSecondRenderPass = glGetUniformLocation(programID, "bIsSecondRenderPass");
	this->uniformLoc_secondPassMode = glGetUniformLocation(programID, "secondPassMode");

	{//directional shadow
		uniformLoc_diretionalLight_position = glGetUniformLocation(programID, "diretionalLight_position");
		uniformLoc_directionalShadowPass = glGetUniformLocation(programID, "directionalShadowPass");
		uniformLoc_directionalLight_spaceMatrix = glGetUniformLocation(programID, "directionalLight_spaceMatrix");
		directionalLight_shadowSampler = glGetUniformLocation(programID, "directionalShadowSampler");
	}

	{//bloom
		uniformLoc_isGlowing = glGetUniformLocation(programID, "isGlowing");
		uniformLoc_glowColor = glGetUniformLocation(programID, "glowColor");
		uniformLoc_isHorizontal = glGetUniformLocation(programID, "isHorizontal");
		uniformLoc_bloomWeights[0] = glGetUniformLocation(programID, "bloomWeights[0]");
		uniformLoc_bloomWeights[1] = glGetUniformLocation(programID, "bloomWeights[1]");
		uniformLoc_bloomWeights[2] = glGetUniformLocation(programID, "bloomWeights[2]");
		uniformLoc_bloomWeights[3] = glGetUniformLocation(programID, "bloomWeights[3]");
		uniformLoc_bloomWeights[4] = glGetUniformLocation(programID, "bloomWeights[4]");
		uniformLoc_hasBloom = glGetUniformLocation(programID, "hasBloom");
		uniformLoc_exposure = glGetUniformLocation(programID, "exposure");
		uniformLoc_offscreenBloomTexture = glGetUniformLocation(programID, "offscreenBloomTexture");
	}

	{//geometry shader effects
		uniformLoc_explodeTime = glGetUniformLocation(programID, "explodeTime");
		
	}

	uniformLoc_reflectsSkybox  = glGetUniformLocation(programID, "reflectsSkybox");

	const int MAXNUMBEROFBONES = 100;
	for (int i = 0; i < MAXNUMBEROFBONES; i++)
	{
		std::ostringstream ss;
		ss << "bones[" << i << "]";
		std::string base = ss.str();

		groupDefault.uniformLoc_bones[i] = glGetUniformLocation(programID, base.c_str());
	}

	this->mapShaderUniform[programID] = groupDefault;

	this->SetupBloomFBO();
	this->SetupDirectionalShadow();
	//this->SetupFBO();

	//GLuint programID2 = gelShaderManager::sharedInstance().getIDWithName("glass");

	//shaderUniformGroup groupGlass;

	//groupGlass.locID_matModel = glGetUniformLocation(programID2, "matModel");
	//groupGlass.locID_matView = glGetUniformLocation(programID2, "matView");
	//groupGlass.locID_matProj = glGetUniformLocation(programID2, "matProj");

	//gelLightManager::sharedInstance().SetupUniforms(programID2);

	////uniformId_isWireframe = glGetUniformLocation(programID, "isWireframe"); //unused for now
	//groupGlass.uniformId_ModelColor = glGetUniformLocation(programID2, "objectColour");
	//groupGlass.uniformId_userDebugColorsNoLighting = glGetUniformLocation(programID2, "bUseDebugDiffuseNoLighting");
	//groupGlass.uniformId_eyeLocation = glGetUniformLocation(programID2, "eyeLocation");
	//groupGlass.uniformLoc_bUseLighting = glGetUniformLocation(programID2, "bUseLighting");

	//this->mapShaderUniform[programID2] = groupGlass;


	glEnable(GL_DEPTH_TEST);

	//setup before reading objects
	gelObjectManager::sharedInstance().Setup();
	gelSoundManager::sharedInstance().Setup();
	gelMeshManager::sharedInstance().Setup();
	gelRenderManager::sharedInstance().Setup();
	GelPhysicsManager::sharedInstance().Setup();

	GelMazeWorldManager::sharedInstance().Setup();


	std::string soundAlias = "jump";
	std::string soundPath = "resources/media/jump.wav";
	float minDist = 0.1f;
	float maxDist = 100.0f;
	gelSoundManager::sharedInstance().CreateSampleSound(soundAlias, soundPath, minDist, maxDist);

	soundAlias = "scream";
	soundPath = "resources/media/scream.wav";
	minDist = 0.1f;
	maxDist = 100.0f;
	gelSoundManager::sharedInstance().CreateSampleSound(soundAlias, soundPath, minDist, maxDist);

	soundAlias = "explode";
	soundPath = "resources/media/explode.wav";
	minDist = 0.1f;
	maxDist = 100.0f;
	gelSoundManager::sharedInstance().CreateSampleSound(soundAlias, soundPath, minDist, maxDist);

	float meshRadius;

	//create cloth
	{
		//Create Sphere
		iGameObject *cloth = gelObjectFactory::sharedInstance().createGameObject("object");
		RenderInfo renderCloth;
		cloth->SetRender(renderCloth);

		int width = 10;
		int height = 6;
		int widthNode = 50;
		int heightNode = 30;
		GLuint bufferSize = (widthNode) * (heightNode) * 4 * 2; // 4 lines for each node except for the last row and column (and 2 per line)

		glm::vec3 initialPosition(6.0f, 15.0f, 10.0f);

		cSoftBody *clothBody = new cSoftBody();
		clothBody->GenerateRectangleCloth(initialPosition, width, height, widthNode, heightNode);
		
		PhysicsInfo clothPhyiscsInfo = cloth->GetPhysics();
		clothPhyiscsInfo.enabled = true;
		clothPhyiscsInfo.collisionBody = clothBody;

		cloth->SetPhysics(clothPhyiscsInfo);
		cloth->SetObjectName("cloth");
		gelObjectManager::sharedInstance().AddGameObject(cloth);

		//for drawing

		gelMeshManager::sharedInstance().CreateDebugLineBuffer(bufferSize, programID);

	}

	//for (int i = 0; i != sphereCount; i++)
	//{
	//	for (int k = 0; k != innerCount; k++)
	//	{
	//		float sphereRadius = OCHelper::getRandFloat(1.0f, 4.0f);
	//		iGameObject *sphere = gelObjectFactory::sharedInstance().createGameObject("object");
	//		RenderInfo renderSphere;
	//		MeshInfo meshSphere;
	//		meshSphere.name = "cube";
	//		renderSphere.useTextureAsDiffuse = false;
	//		renderSphere.useDebugColorsNoLighting = false;
	//		renderSphere.position.x = 4.0f + i;//OCHelper::getRandFloat(4.0f, 5.0f);
	//		renderSphere.position.y = k * 1.5f + 20.0f; //OCHelper::getRandFloat(4.0f, 25.0f);
	//		renderSphere.position.z = 3.0f;//OCHelper::getRandFloat(4.0f, 5.0f);
	//		renderSphere.color.r = 0.0f;
	//		renderSphere.color.g = 1.0f;
	//		renderSphere.color.b = 0.0f;
	//		renderSphere.scale = 1.0f;
	//		meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshSphere.name);
	//		renderSphere.SetRadius(meshRadius);
	//		sphere->SetRender(renderSphere);
	//		sphere->SetMesh(meshSphere);

	//		//rigid body definition
	//		cRigidBodyDef bodyDef;
	//		bodyDef.Static = false;
	//		bodyDef.Mass = sphereRadius*2.0f; //same as radius
	//		bodyDef.friction = 5.0f;
	//		bodyDef.restitution = 0.0f;
	//		//bodyDef.linearDamping = 0.2f;
	//		//bodyDef.angularDamping = 0.6f;
	//		bodyDef.Position = renderSphere.position;
	//		bodyDef.Velocity.x = OCHelper::getRandFloat(-5.0f, 15.0f);
	//		bodyDef.Velocity.y = OCHelper::getRandFloat(0.0f, 3.0f);
	//		bodyDef.Velocity.z = OCHelper::getRandFloat(-15.0f, 5.0f);
	//		bodyDef.ownerID = sphere->getObjectID();
	//		bodyDef.ownerObject = sphere;
	//		//generic rigid body class
	//		iCollisionBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
	//			bodyDef,
	//			GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(0.5f, 0.1f, 1.0f))); //sphere radius 1.0f
	//																		 //g_myPhysicsFactory->CreateShape(nPhysics::nShapes::sSphere(sphereRadius))); //sphere radius 1.0f
	//		PhysicsInfo spherePhyiscsInfo = sphere->GetPhysics();
	//		spherePhyiscsInfo.enabled = true;
	//		spherePhyiscsInfo.collisionBody = sphereRigidBody;
	//		GelPhysicsManager::sharedInstance().GetWorld()->AddBody(sphereRigidBody);

	//		spherePhyiscsInfo.velocity.x = bodyDef.Velocity.x;
	//		spherePhyiscsInfo.velocity.y = bodyDef.Velocity.y;
	//		spherePhyiscsInfo.velocity.z = bodyDef.Velocity.z;

	//		sphere->SetPhysics(spherePhyiscsInfo);

	//		GelPhysicsManager::sharedInstance().GetWorld()->AddHingeConstraint(sphereRigidBody, glm::vec3(bodyDef.Position.x, bodyDef.Position.y + 5.0f, bodyDef.Position.z));

	//		gelObjectManager::sharedInstance().AddGameObject(sphere);
	//	}
	//}

	int sphereCount = 1;
	int innerCount = 1;

	for (int i = 0; i != sphereCount; i++)
	{
		for (int k = 0; k != innerCount; k++)
		{
			
			float sphereRadius = OCHelper::getRandFloat(1.0f, 4.0f);
			iGameObject *sphere = gelObjectFactory::sharedInstance().createGameObject("object");
			RenderInfo renderSphere;
			MeshInfo meshSphere;
			meshSphere.name = "cube";
			renderSphere.useTextureAsDiffuse = false;
			renderSphere.useDebugColorsNoLighting = false;
			renderSphere.position.x = 25.0f + i;//OCHelper::getRandFloat(4.0f, 5.0f);
			renderSphere.position.y = k * 1.5f + 20000.0f; //OCHelper::getRandFloat(4.0f, 25.0f);
			renderSphere.position.z = 10.0f;//OCHelper::getRandFloat(4.0f, 5.0f);
			renderSphere.color.r = 1.0f;
			renderSphere.color.g = 1.0f;
			renderSphere.color.b = 0.0f;
			renderSphere.scale = 1.0f;
			meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshSphere.name);
			renderSphere.SetRadius(meshRadius);
			sphere->SetRender(renderSphere);
			sphere->SetMesh(meshSphere);

			//rigid body definition
			cRigidBodyDef bodyDef;
			bodyDef.Static = false;
			bodyDef.Mass = sphereRadius*2.0f; //same as radius
			bodyDef.friction = 5.0f;
			bodyDef.restitution = 0.0f;
			//bodyDef.linearDamping = 0.2f;
			//bodyDef.angularDamping = 0.6f;
			bodyDef.Position = renderSphere.position;
			bodyDef.Velocity.x = OCHelper::getRandFloat(25.0f, 65.0f);
			bodyDef.Velocity.y = OCHelper::getRandFloat(0.0f, 3.0f);
			bodyDef.Velocity.z = OCHelper::getRandFloat(25.0f, 65.0f);
			bodyDef.ownerID = sphere->getObjectID();
			bodyDef.ownerObject = sphere;
			//generic rigid body class
			iCollisionBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
				bodyDef,
				GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(1.0f, 0.2f, 0.5f))); //sphere radius 1.0f
																							 //g_myPhysicsFactory->CreateShape(nPhysics::nShapes::sSphere(sphereRadius))); //sphere radius 1.0f
			PhysicsInfo spherePhyiscsInfo = sphere->GetPhysics();
			spherePhyiscsInfo.enabled = true;
			spherePhyiscsInfo.collisionBody = sphereRigidBody;
			GelPhysicsManager::sharedInstance().GetWorld()->AddBody(sphereRigidBody);

			spherePhyiscsInfo.velocity.x = bodyDef.Velocity.x;
			spherePhyiscsInfo.velocity.y = bodyDef.Velocity.y;
			spherePhyiscsInfo.velocity.z = bodyDef.Velocity.z;

			sphere->SetPhysics(spherePhyiscsInfo);

			GelPhysicsManager::sharedInstance().GetWorld()->AddSliderConstraint(sphereRigidBody, glm::vec3(bodyDef.Position.x, bodyDef.Position.y, bodyDef.Position.z - 5.0f));

			gelObjectManager::sharedInstance().AddGameObject(sphere);
		}
	}

	iCollisionBody* previousBody = 0;
	glm::vec3 previousPosition;

	iShape *bridgePlankShape = GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(1.0f, 0.2f, 6.0f));
	iShape *tileShape = GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(6.0f, 0.4f, 6.0f));
	iShape *tileShape_medium = GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(3.0f, 0.4f, 3.0f));
	iShape *tileShape_small = GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(1.5f, 0.4f, 1.5f));

	innerCount = 24;
	for (int i = 0; i != sphereCount; i++)
	{
		for (int k = 0; k != innerCount; k++)
		{
			
			iGameObject *sphere = gelObjectFactory::sharedInstance().createGameObject("object");
			RenderInfo renderSphere;
			MeshInfo meshSphere;
			meshSphere.name = "cube";
			renderSphere.useTextureAsDiffuse = false;
			renderSphere.useDebugColorsNoLighting = false;
			renderSphere.position.x = (k) * 2.4f + 10.0f;//OCHelper::getRandFloat(4.0f, 5.0f);
			renderSphere.position.y = 1500.0f; //OCHelper::getRandFloat(4.0f, 25.0f);
			renderSphere.position.z = 25.0f;//OCHelper::getRandFloat(4.0f, 5.0f);
			renderSphere.color.r = 1.0f;
			renderSphere.color.g = 0.0f;
			renderSphere.color.b = 1.0f;
			renderSphere.scale = 0.5f;

			meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshSphere.name);
			renderSphere.SetRadius(meshRadius);
			sphere->SetRender(renderSphere);
			sphere->SetMesh(meshSphere);

			//rigid body definition
			cRigidBodyDef bodyDef;
			bodyDef.Static = false;

			if (k == innerCount - 1 || k == 0 )
			{
				bodyDef.Mass = 0.0f;
			}
			else
			{
				bodyDef.Mass = 6.f; 
			}

			bodyDef.friction = 1.0f;
			bodyDef.restitution = 2.0f;
			//bodyDef.linearDamping = 0.2f;
			//bodyDef.angularDamping = 0.6f;
			bodyDef.Position = renderSphere.position;
			bodyDef.Velocity.x = 0.0f;// OCHelper::getRandFloat(25.0f, 65.0f);
			bodyDef.Velocity.y = 0.0f;// OCHelper::getRandFloat(0.0f, 3.0f);
			bodyDef.Velocity.z = 0.0f;// OCHelper::getRandFloat(25.0f, 65.0f);
			bodyDef.ownerID = sphere->getObjectID();
			bodyDef.ownerObject = sphere;
			//generic rigid body class
			iCollisionBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
				bodyDef,
				bridgePlankShape); //sphere radius 1.0f
																							 //g_myPhysicsFactory->CreateShape(nPhysics::nShapes::sSphere(sphereRadius))); //sphere radius 1.0f
			PhysicsInfo spherePhyiscsInfo = sphere->GetPhysics();
			spherePhyiscsInfo.enabled = true;
			spherePhyiscsInfo.collisionBody = sphereRigidBody;
			GelPhysicsManager::sharedInstance().GetWorld()->AddBody(sphereRigidBody);

			spherePhyiscsInfo.velocity.x = bodyDef.Velocity.x;
			spherePhyiscsInfo.velocity.y = bodyDef.Velocity.y;
			spherePhyiscsInfo.velocity.z = bodyDef.Velocity.z;

			sphere->SetPhysics(spherePhyiscsInfo);

			if (k > 0)
			{
				GelPhysicsManager::sharedInstance().GetWorld()->AddPoint2PointConstraint(previousBody, sphereRigidBody, glm::vec3(1.25f, 0.0f, -3.0f), glm::vec3(-1.25f, 0.0f, -3.0f));
				GelPhysicsManager::sharedInstance().GetWorld()->AddPoint2PointConstraint(previousBody, sphereRigidBody, glm::vec3(1.25f, 0.0f, 3.0f), glm::vec3(-1.25f, 0.0f, 3.0f));
			}

			previousPosition = renderSphere.position;
			previousBody = sphereRigidBody;

			gelObjectManager::sharedInstance().AddGameObject(sphere);
			g_uppersnake = sphere;
		}
		
		//g_myPhysicsWorld->AddPoint2PointConstraint(previousBody, glm::vec3(previousPosition.x, previousPosition.y+2.0f, previousPosition.z));
	}

	glm::vec3 finalBridgePos((innerCount-1) * 2.4f + 10.0f + 8.0f + 0.2f, 15.0f, 25.0f);

	std::vector<glm::vec3> tilePositions;
	tilePositions.push_back(finalBridgePos);
	tilePositions.push_back(glm::vec3(4.0f-0.2f,15.0f,25.0f));
	tilePositions.push_back(glm::vec3(4.0f, 11.0f, 16.0f));
	tilePositions.push_back(glm::vec3(4.0f, 7.0f,  12.0f));
	tilePositions.push_back(glm::vec3(-2.0f, 3.0f,  12.0f));

	std::vector<iShape*> tileShapes;
	tileShapes.push_back(tileShape);
	tileShapes.push_back(tileShape);
	tileShapes.push_back(tileShape_small);
	tileShapes.push_back(tileShape_medium);
	tileShapes.push_back(tileShape_small);

	for (int k = 0; k != tilePositions.size(); k++)
	{
		break;
		iGameObject *sphere = gelObjectFactory::sharedInstance().createGameObject("object");
		if (k == 0)
		{
			sphere->SetObjectName("triggerTile");
		}
		RenderInfo renderSphere;
		MeshInfo meshSphere;
		meshSphere.name = "cube";
		renderSphere.useTextureAsDiffuse = false;
		renderSphere.useDebugColorsNoLighting = false;
		renderSphere.position = tilePositions[k];
		renderSphere.color.r = 1.0f;
		renderSphere.color.g = 0.0f;
		renderSphere.color.b = 1.0f;
		renderSphere.scale = 0.5f;

		renderSphere.hasGlow = true;
		renderSphere.glowColor = glm::vec4(0.9f, 0.9f, 1.0f, 1.0f);

		meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshSphere.name);
		renderSphere.SetRadius(meshRadius);
		sphere->SetRender(renderSphere);
		sphere->SetMesh(meshSphere);

		//rigid body definition
		cRigidBodyDef bodyDef;
		bodyDef.Static = false;

		bodyDef.Mass = 0.0f;

		bodyDef.friction = 1.0f;
		bodyDef.restitution = 2.0f;
		//bodyDef.linearDamping = 0.2f;
		//bodyDef.angularDamping = 0.6f;
		bodyDef.Position = renderSphere.position;
		bodyDef.Velocity.x = 0.0f;// OCHelper::getRandFloat(25.0f, 65.0f);
		bodyDef.Velocity.y = 0.0f;// OCHelper::getRandFloat(0.0f, 3.0f);
		bodyDef.Velocity.z = 0.0f;// OCHelper::getRandFloat(25.0f, 65.0f);
		bodyDef.ownerID = sphere->getObjectID();
		bodyDef.ownerObject = sphere;
		//generic rigid body class
		iCollisionBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
			bodyDef,
			tileShapes[k]); //sphere radius 1.0f
							   //g_myPhysicsFactory->CreateShape(nPhysics::nShapes::sSphere(sphereRadius))); //sphere radius 1.0f
		PhysicsInfo spherePhyiscsInfo = sphere->GetPhysics();
		spherePhyiscsInfo.enabled = true;
		spherePhyiscsInfo.collisionBody = sphereRigidBody;
		GelPhysicsManager::sharedInstance().GetWorld()->AddBody(sphereRigidBody);

		spherePhyiscsInfo.velocity.x = bodyDef.Velocity.x;
		spherePhyiscsInfo.velocity.y = bodyDef.Velocity.y;
		spherePhyiscsInfo.velocity.z = bodyDef.Velocity.z;

		sphere->SetPhysics(spherePhyiscsInfo);

		previousPosition = renderSphere.position;
		previousBody = sphereRigidBody;

		gelObjectManager::sharedInstance().AddGameObject(sphere);
		g_uppersnake = sphere;
	}

	//innerCount = 0;
	//for (int i = 0; i != sphereCount; i++)
	//{
	//	for (int k = 0; k != innerCount; k++)
	//	{
	//		iGameObject *sphere = gelObjectFactory::sharedInstance().createGameObject("object");
	//		RenderInfo renderSphere;
	//		MeshInfo meshSphere;
	//		meshSphere.name = "cube";
	//		renderSphere.useTextureAsDiffuse = false;
	//		renderSphere.useDebugColorsNoLighting = false;
	//		renderSphere.position.x = (k) * 3.0f + 5.0f;//OCHelper::getRandFloat(4.0f, 5.0f);
	//		renderSphere.position.y = 5.0f; //OCHelper::getRandFloat(4.0f, 25.0f);
	//		renderSphere.position.z = 35.0f;//OCHelper::getRandFloat(4.0f, 5.0f);
	//		renderSphere.color.r = 0.0f;
	//		renderSphere.color.g = 0.0f;
	//		renderSphere.color.b = 1.0f;
	//		renderSphere.scale = 0.5f;
	//		meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshSphere.name);
	//		renderSphere.SetRadius(meshRadius);
	//		sphere->SetRender(renderSphere);
	//		sphere->SetMesh(meshSphere);

	//		//rigid body definition
	//		cRigidBodyDef bodyDef;
	//		bodyDef.Static = false;

	//		if (k == innerCount - 1 || k == 0)
	//		{
	//			bodyDef.Mass = 0.0f;
	//		}
	//		else
	//		{
	//			bodyDef.Mass = 1.0f;
	//		}

	//		bodyDef.friction = 1.0f;
	//		bodyDef.restitution = 2.0f;
	//		//bodyDef.linearDamping = 0.2f;
	//		//bodyDef.angularDamping = 0.6f;
	//		bodyDef.Position = renderSphere.position;
	//		bodyDef.Velocity.x = 0.0f;// OCHelper::getRandFloat(25.0f, 65.0f);
	//		bodyDef.Velocity.y = 0.0f;// OCHelper::getRandFloat(0.0f, 3.0f);
	//		bodyDef.Velocity.z = 0.0f;// OCHelper::getRandFloat(25.0f, 65.0f);
	//		bodyDef.ownerID = sphere->getObjectID();
	//		bodyDef.ownerObject = sphere;
	//		//generic rigid body class
	//		iCollisionBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
	//			bodyDef,
	//			GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(1.0f, 0.2f, 1.0f))); //sphere radius 1.0f
	//																						 //g_myPhysicsFactory->CreateShape(nPhysics::nShapes::sSphere(sphereRadius))); //sphere radius 1.0f
	//		PhysicsInfo spherePhyiscsInfo = sphere->GetPhysics();
	//		spherePhyiscsInfo.enabled = true;
	//		spherePhyiscsInfo.collisionBody = sphereRigidBody;
	//		GelPhysicsManager::sharedInstance().GetWorld()->AddBody(sphereRigidBody);

	//		spherePhyiscsInfo.velocity.x = bodyDef.Velocity.x;
	//		spherePhyiscsInfo.velocity.y = bodyDef.Velocity.y;
	//		spherePhyiscsInfo.velocity.z = bodyDef.Velocity.z;

	//		sphere->SetPhysics(spherePhyiscsInfo);

	//		GelPhysicsManager::sharedInstance().GetWorld()->AddConeTwistConstraint(sphereRigidBody, glm::vec3(renderSphere.position.x, renderSphere.position.y + 5.0f, renderSphere.position.z));

	//		previousPosition = renderSphere.position;
	//		previousBody = sphereRigidBody;

	//		gelObjectManager::sharedInstance().AddGameObject(sphere);
	//	}
	//}

	{
		//Create ground
		iGameObject *goal = gelObjectFactory::sharedInstance().createGameObject("object");
		RenderInfo renderGoal;
		MeshInfo meshGoal;
		meshGoal.name = "sphere_nice";
		renderGoal.useTextureAsDiffuse = false;
		renderGoal.useDebugColorsNoLighting = false;
		renderGoal.position = glm::vec3(192.0f, 4.0f, 373.5f);
		renderGoal.color.r = 0.1f;
		renderGoal.color.g = 0.0f;
		renderGoal.color.b = 1.0f;
		renderGoal.scale = 2.0f;

		renderGoal.hasGlow = true;
		renderGoal.glowColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

		//float meshRadius;
		meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshGoal.name);
		renderGoal.SetRadius(meshRadius);
		goal->SetRender(renderGoal);
		goal->SetMesh(meshGoal);
		gelObjectManager::sharedInstance().AddGameObject(goal);
	}
	

	//Create ground
	iGameObject *ground = gelObjectFactory::sharedInstance().createGameObject("object");
	RenderInfo renderGround;
	MeshInfo meshGround;
	ground->SetObjectName("groundPlane");
	meshGround.name = "cube";
	renderGround.useTextureAsDiffuse = false;
	renderGround.useDebugColorsNoLighting = false;
	renderGround.position.x = 0.0f;
	renderGround.position.y = -1.0f;
	renderGround.position.z = 0.0f;
	renderGround.color.r = 0.1f;
	renderGround.color.g = 0.0f;
	renderGround.color.b = 1.0f;
	renderGround.scale = 1000.0f;
	//float meshRadius;
	meshRadius = gelMeshManager::sharedInstance().LookUpMeshRadius(meshGround.name);
	renderGround.SetRadius(meshRadius);
	ground->SetRender(renderGround);
	ground->SetMesh(meshGround);

	//rigid body definition
	cRigidBodyDef bodyDefG;
	bodyDefG.Static = false;
	bodyDefG.Mass = 0.0f;
	bodyDefG.friction = 5.0f;
	bodyDefG.restitution = 0.0f;
	
	bodyDefG.Position = renderGround.position;
	bodyDefG.ownerObject = ground;

	glm::vec3 groundSize(500.0f, 1.0f, 500.0f);
	groundSize *= 0.5f;

	//generic rigid body class
	iRigidBody* groundRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
		bodyDefG,
		GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sBox(groundSize.x, groundSize.y, groundSize.z)));// (nPhysics::nShapes::sPlane())); //sphere radius 1.0f
	PhysicsInfo groundPhyiscsInfo = ground->GetPhysics();
	groundPhyiscsInfo.enabled = false;
	groundPhyiscsInfo.collisionBody = groundRigidBody;
	ground->SetPhysics(groundPhyiscsInfo);
	//GelPhysicsManager::sharedInstance().GetWorld()->AddBody(groundRigidBody);
	//gelObjectManager::sharedInstance().AddGameObject(ground);

	gelMeshManager::sharedInstance().ScaleEverythingToBoundingBoxOnLoad(true, 10.0f);

	//read /resources/gameFile/gameFile.txt to get:
	//  the mesh list and load them into the buffer
	//  player model, position, physics, etc
	//  object list and their info
	//ReadGameFiles(); //old .txt fileformat
	ReadGameFiles_2();// json


	//try new model loading
	//"resources/phoenix_ugv.md2"

	//add custom lights
	g_ambientLight0 = gelObjectFactory::sharedInstance().createGameObject("light_source");
	iGameObject *ambientLight1 = gelObjectFactory::sharedInstance().createGameObject("light_source");
	iGameObject *ambientLight2 = gelObjectFactory::sharedInstance().createGameObject("light_source");
	iGameObject *ambientLight3 = gelObjectFactory::sharedInstance().createGameObject("light_source");

	ambientLight3->SetObjectName("LIGHT_0");
	ambientLight2->SetObjectName("LIGHT_1");

	RenderInfo render;
	MeshInfo mesh;
	mesh.name = "isosphere";
	float addThis = -3.0f;
	render.position.x = 20.0f;
	render.position.y = 3.0f;
	render.position.z = -20.0f;
	render.color.r = 1.0f;
	render.color.g = 1.0f;
	render.color.b = 1.0f;
	render.scale = 0.2f;
	float model_radius = gelMeshManager::sharedInstance().LookUpMeshRadius(mesh.name);
	render.SetRadius(model_radius);
	g_ambientLight0->SetRender(render);
	g_ambientLight0->SetMesh(mesh);
	
	render.position.x = this->playgroundScale/10.0f +addThis;
	render.position.z = 0.0f -addThis;

	ambientLight1->SetRender(render);
	ambientLight1->SetMesh(mesh);

	render.position.x = 0.0f - addThis;
	render.position.z = this->playgroundScale / 10.0f + addThis;
	ambientLight3->SetRender(render);
	ambientLight3->SetMesh(mesh);

	//render.position.x *= -1.0f;
	//render.position.z *= -1.0f;

	//ambientLight2->SetRender(render);
	//ambientLight2->SetMesh(mesh);

	LightInfo lightAmb;
	lightAmb.bIsEnabled = true;
	lightAmb.position = glm::vec3(render.position.x, render.position.y, render.position.z);	// Likely not needed as vec3 c'tor sets to zeros
	lightAmb.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	lightAmb.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
	lightAmb.specular = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
	lightAmb.attenConst = 0.0f;
	lightAmb.attenLinear = 0.01f;
	lightAmb.attenQuad = 0.0f;

	lightAmb.typeAndParams.x = 0; //set to point
	lightAmb.typeAndParams.y = 0.9999f;
	lightAmb.typeAndParams.z = 0.7000f;
	lightAmb.direction = glm::vec4(1.0f, -4.0f, 1.0f, 0.0f);

	g_ambientLight0->SetLight(lightAmb);
	ambientLight1->SetLight(lightAmb);

	lightAmb.bIsEnabled = true;
	lightAmb.position = glm::vec3(render.position.x, render.position.y, render.position.z);	// Likely not needed as vec3 c'tor sets to zeros
	lightAmb.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	lightAmb.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
	lightAmb.specular = glm::vec4(0.4f, 0.4f, 0.4f, 0.7f);
	lightAmb.attenConst = 1.0f;
	lightAmb.attenLinear = 0.002f;
	lightAmb.attenQuad = 0.0001f;
	lightAmb.direction = glm::vec4(4.0f, 0.0f, 4.0f, 0.0f);
	lightAmb.typeAndParams.x = 0.0f;
	lightAmb.typeAndParams.w = 1.0f;

	render.position.x = 0.0f;
	render.position.y = 40.0f;
	render.position.z = 0.0f;
	lightAmb.position = render.position;

	render.scale = 1.0f;

	ambientLight3->SetRender(render);
	ambientLight3->SetLight(lightAmb);

	render.position.x += 30.0f;
	lightAmb.position = render.position;

	lightAmb.bIsEnabled = false;
	ambientLight2->SetRender(render);
	ambientLight2->SetLight(lightAmb);

	//gelObjectManager::sharedInstance().AddGameObject(g_ambientLight0);
	//gelObjectManager::sharedInstance().AddGameObject(ambientLight1);
	//gelObjectManager::sharedInstance().AddGameObject(ambientLight2);
	gelObjectManager::sharedInstance().AddGameObject(ambientLight3);

	gelLightManager::sharedInstance().LoadLights();


	glm::vec3 characterPosition(190.0f, 3.0f, 0.0f);
	glm::vec3 vehiclePosition(-100.0f, -100.0f, 100.0f);
	GelPhysicsManager::sharedInstance().GetWorld()->WarpCharacter(characterPosition);
	GelPhysicsManager::sharedInstance().GetWorld()->WarpVehicle(vehiclePosition);
	gelCameraManager::sharedInstance().AttachToGameObject(gelObjectManager::sharedInstance().GetMainPlayer());
	//gelCameraManager::sharedInstance().AttachToGameObject(gelObjectManager::sharedInstance().GetVehicle());
	gelCameraManager::sharedInstance().mainCamera->SetEyePosition(glm::vec4(60.0f, 300.0f, -150.0f, 0.0f));
	//ExecuteCommands_beforeGameLoop();

	//GelsenizeLuaStartUp();

	tickTime = 1.0f;
	tickAccumulator = 0.0f;
	//game loop
	GameLoop();

	//GelsenizeLuaShutDown();

	gelSoundManager::sharedInstance().CleanUp();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void GelsenizeCore::GameLoop(void)
{
	float collisionDelta = 0.0f;

	cAverageSample timerAverage;
	double lastTime = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		static const float MAXDELTATIME = 0.1f;	// 100 ms
		float deltaTime = static_cast<float>(glfwGetTime() - lastTime);
		lastTime = glfwGetTime();
		if (deltaTime > MAXDELTATIME)
		{
			deltaTime = MAXDELTATIME;
		}
		timerAverage.AddSample(deltaTime);
		deltaTime = static_cast<float>(timerAverage.getAverage());
		FPS_COUNTER = (int)(1.0f / deltaTime);
		globalClock += deltaTime;
		
		//update AI (gameobject control)
		gelObjectManager::sharedInstance().Update(deltaTime);
		float yPos = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender().position.y;
		if (yPos <= -150.0f)
		{
			glm::vec3 characterPosition(180.0f, 20.0f, 12.0f);
			GelPhysicsManager::sharedInstance().GetWorld()->WarpCharacter(characterPosition);
		}
		else if (yPos <= -1.0f)
		{
			gelSoundManager::sharedInstance().PlaySoundWithName("scream");
		}

		//Commands Scripts
		GelCommandManager::sharedInstance().Update(deltaTime);

		//Physics
		GelPhysicsManager::sharedInstance().Update(deltaTime);

		//Camera
		gelCameraManager::sharedInstance().Update(deltaTime);

		//not used 
		//Update Light Uniforms Values
		glUniform3f(locID_lightPosition, 0.0f, 10.0f, 0.0f); //light position
		glUniform3f(locID_lightDiffuse, 1.0f, 1.0f, 1.0f); //white light

		gelLightManager::sharedInstance().Update(deltaTime); 

		float ratio;
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);
		ratio = width / (float)height;
		gelCameraManager::sharedInstance().mainCamera->WindowResize(width, height);

		glEnable(GL_BLEND);		// Enables "blending"
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// "turns on" transparency
		//glColorMask(false, false, false, true);
		//glClearColor(1, 0, 1, 0);
		//glClear(GL_COLOR_BUFFER_BIT);
		//glColorMask(true, true, true, true);

		//regularDraw = 3;
		if (regularDraw == 0)
		{
			//glClearDepth(1.0f);
			//glClearColor(0.0f, 0.41f, 0.55f, 1.0f);
			//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			///*RENDER BEGIN*/

			//Draw Scene Directly to Screen
			//Draw to screen
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Set the uniform to the 2nd pass
			glUniform1i(this->uniformLoc_bIsSecondRenderPass, FALSE);

			gelRenderManager::sharedInstance().Update(deltaTime);

			///*RENDER END*/
		}
		else if (regularDraw == 1)
		{

			//Offscreen rendering
			glUniform1i(this->uniformLoc_bIsSecondRenderPass, FALSE);

			// Point rendering to the off-screen frame buffer object for player1
			glBindFramebuffer(GL_FRAMEBUFFER, this->FBO_bloom[0]);

			// Clear the offscreen framebuffer
			glViewport(0, 0, this->FBO_pixelSize, this->FBO_pixelSize);
			GLfloat	zero = 0.0f;
			GLfloat one = 1.0f;
			glClearBufferfv(GL_COLOR, 0, &zero);
			glClearBufferfv(GL_DEPTH, 0, &one);


			width = this->FBO_pixelSize;
			height = this->FBO_pixelSize;

			ratio = width / (float)height;

			glViewport(0, 0, width, height);

			//gelCameraManager::sharedInstance().mainCamera->WindowResize(width, height);

			//Draw Scene into FBO[0]
			gelRenderManager::sharedInstance().Update(deltaTime);


			/////////////////////

			//Draw to screen
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glfwGetFramebufferSize(window, &width, &height);
			ratio = width / (float)height;
			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Set the uniform to the 2nd pass
			glUniform1i(this->uniformLoc_bIsSecondRenderPass, TRUE);

			gelCameraManager::sharedInstance().offScreenCamera->UpdateView(deltaTime);

			// Set the texture for the 2nd pass
			// Pick texture unit 20
			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, FBO_bloom_colorTexture[0]);
			glUniform1i(this->uniformLoc_offscreenColorTexture, 20);

			glUniform1i(this->uniformLoc_secondPassMode, 0);

			//Draw the screen quad
			gelRenderManager::sharedInstance().DrawFullScreenQuad();
		}
		else if (regularDraw == 2)
		{
			iGameObject *lightThatCastsShadow = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("LIGHT_0");
			LightInfo light = lightThatCastsShadow->GetLight();
			RenderInfo lightRender = lightThatCastsShadow->GetRender();
			glm::vec3 lightPos = lightRender.position;

			glm::mat4 lightProjection, lightView;
			glm::mat4 lightSpaceMatrix;
			GLfloat near_plane = 1.0f, far_plane = 100.0f;
			lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
			
			glm::vec3 lightDirection(light.direction.x, light.direction.y, light.direction.z);
			
			glm::vec3 playerPosition = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender().position;

			lightView = glm::lookAt(lightPos, lightDirection, glm::vec3(0.0, 1.0, 0.0));
			lightSpaceMatrix = lightProjection * lightView;
			
			//directional shadow pass
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, this->FBO_depthMap);
			glClear(GL_DEPTH_BUFFER_BIT);

			glUniformMatrix4fv(this->uniformLoc_directionalLight_spaceMatrix, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
			glUniform3f(this->uniformLoc_diretionalLight_position, lightPos.x, lightPos.y, lightPos.z);

			glUniform1i(this->uniformLoc_directionalShadowPass, TRUE);
			gelRenderManager::sharedInstance().SetDirectionalShadowPass(true);
			gelRenderManager::sharedInstance().Update(deltaTime);
			gelRenderManager::sharedInstance().SetDirectionalShadowPass(false);
			glUniform1i(this->uniformLoc_directionalShadowPass, FALSE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			//regular render pass
			glfwGetFramebufferSize(window, &width, &height);
			ratio = width / (float)height;
			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->FBO_depthMap_depthTexture);
			glUniform1i(this->directionalLight_shadowSampler, 1);

			gelRenderManager::sharedInstance().Update(deltaTime);
		}
		else if (regularDraw == 3)
		{
			{//shadow pass

				//1

				iGameObject *lightThatCastsShadow = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("LIGHT_0");
				LightInfo light = lightThatCastsShadow->GetLight();
				RenderInfo lightRender = lightThatCastsShadow->GetRender();
				glm::vec3 lightPos = lightRender.position;

				glm::mat4 lightProjection, lightView;
				glm::mat4 lightSpaceMatrix;
				GLfloat near_plane = 1.0f, far_plane = 100.0f;
				lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);

				glm::vec3 lightDirection(light.direction.x, light.direction.y, light.direction.z);
				
				//lightDirection = g_lightDir;

				glm::vec3 playerPosition = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender().position;

				lightRender.position = playerPosition;
				lightRender.position.y += 20.0f;
				lightRender.position.z += 5.0f;
				lightRender.position.x -= 5.0f;
				lightThatCastsShadow->SetRender(lightRender);

				lightView = glm::lookAt(lightPos, playerPosition, glm::vec3(0.0, 1.0, 0.0));
				lightSpaceMatrix = lightProjection * lightView;

				//directional shadow pass
				glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
				glBindFramebuffer(GL_FRAMEBUFFER, this->FBO_depthMap);
				glClear(GL_DEPTH_BUFFER_BIT);

				glUniformMatrix4fv(this->uniformLoc_directionalLight_spaceMatrix, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
				glUniform3f(this->uniformLoc_diretionalLight_position, lightPos.x, lightPos.y, lightPos.z);

				glUniform1i(this->uniformLoc_directionalShadowPass, TRUE);
				gelRenderManager::sharedInstance().SetDirectionalShadowPass(true);
				gelRenderManager::sharedInstance().Update(deltaTime);
				gelRenderManager::sharedInstance().SetDirectionalShadowPass(false);
				glUniform1i(this->uniformLoc_directionalShadowPass, FALSE);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);


				//2

				//normal render
				glUniform1i(this->uniformLoc_bIsSecondRenderPass, FALSE);

				// Point rendering to the off-screen frame buffer object for player1
				glBindFramebuffer(GL_FRAMEBUFFER, this->FBO_bloom[0]);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				// Clear the offscreen framebuffer
				glViewport(0, 0, this->FBO_pixelSize, this->FBO_pixelSize);
				GLfloat	zero = 0.0f;
				GLfloat one = 1.0f;
				glClearBufferfv(GL_COLOR, 0, &zero);
				glClearBufferfv(GL_DEPTH, 0, &one);

				width = this->FBO_pixelSize;
				height = this->FBO_pixelSize;

				ratio = width / (float)height;

				glViewport(0, 0, width, height);

				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, this->FBO_depthMap_depthTexture);
				glUniform1i(this->directionalLight_shadowSampler, 1);

				//gelCameraManager::sharedInstance().mainCamera->WindowResize(width, height);

				//Draw Scene into FBO[0]
				gelRenderManager::sharedInstance().Update(deltaTime);


				//quad drawing from here on
				//3

				//blur generation = secondpassmode=1

				glUniform1i(this->uniformLoc_bIsSecondRenderPass, TRUE);

				GLboolean horizontal = true, first_iteration = true;
				GLuint amount = 10;
				for (GLuint i = 0; i < amount; i++)
				{
					glBindFramebuffer(GL_FRAMEBUFFER, this->FBO_bloomBlur[horizontal]);
					glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
					glUniform1i(this->uniformLoc_isHorizontal, horizontal);
					//first texture is the brightness filtered

					glActiveTexture(GL_TEXTURE20);
					glBindTexture(GL_TEXTURE_2D, first_iteration ? this->FBO_bloom_colorTexture[1] : this->FBO_bloomBlur_colorTexture[!horizontal]);
					glUniform1i(this->uniformLoc_offscreenColorTexture, 20);

					glUniform1i(this->uniformLoc_secondPassMode, 1);

					gelRenderManager::sharedInstance().DrawFullScreenQuad();

					horizontal = !horizontal;
					if (first_iteration)
						first_iteration = false;
				}

				//4

				//hdr rendering
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
				glfwGetFramebufferSize(window, &width, &height);
				ratio = width / (float)height;
				glViewport(0, 0, width, height);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				// Set the uniform to the 2nd pass
				
				glActiveTexture(GL_TEXTURE20);
				glBindTexture(GL_TEXTURE_2D, FBO_bloom_colorTexture[0]);
				glUniform1i(this->uniformLoc_offscreenColorTexture, 20);

				glActiveTexture(GL_TEXTURE21);
				glBindTexture(GL_TEXTURE_2D, FBO_bloomBlur_colorTexture[!horizontal]);
				glUniform1i(this->uniformLoc_offscreenBloomTexture, 21);

				glUniform1f(this->uniformLoc_exposure, 1.0f);
				glUniform1i(this->uniformLoc_hasBloom, g_bloomActive);

				glUniform1i(this->uniformLoc_bIsSecondRenderPass, TRUE);
				glUniform1i(this->uniformLoc_secondPassMode, 2);

				//Draw the screen quad
				gelRenderManager::sharedInstance().DrawFullScreenQuad();

				glUniform1i(this->uniformLoc_bIsSecondRenderPass, FALSE);
			}
		}
		else if (regularDraw == 4)
		{
			//1

			iGameObject *lightThatCastsShadow = gelObjectManager::sharedInstance().FindGameObjectWithObjectName("LIGHT_0");
			LightInfo light = lightThatCastsShadow->GetLight();
			RenderInfo lightRender = lightThatCastsShadow->GetRender();
			glm::vec3 lightPos = lightRender.position;

			glm::mat4 lightProjection, lightView;
			glm::mat4 lightSpaceMatrix;
			GLfloat near_plane = 1.0f, far_plane = 250.0f;
			lightProjection = glm::ortho(-125.0f, 125.0f, -125.0f, 125.0f, near_plane, far_plane);

			glm::vec3 lightDirection(light.direction.x, light.direction.y, light.direction.z);

			glm::vec3 playerPosition = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender().position;

			lightView = glm::lookAt(lightPos, lightDirection, glm::vec3(0.0, 1.0, 0.0));
			lightSpaceMatrix = lightProjection * lightView;

			//directional shadow pass
			glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
			glBindFramebuffer(GL_FRAMEBUFFER, this->FBO_depthMap);
			glClear(GL_DEPTH_BUFFER_BIT);

			glUniformMatrix4fv(this->uniformLoc_directionalLight_spaceMatrix, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
			glUniform3f(this->uniformLoc_diretionalLight_position, lightPos.x, lightPos.y, lightPos.z);

			glUniform1i(this->uniformLoc_directionalShadowPass, TRUE);
			gelRenderManager::sharedInstance().SetDirectionalShadowPass(true);
			gelRenderManager::sharedInstance().Update(deltaTime);
			gelRenderManager::sharedInstance().SetDirectionalShadowPass(false);
			glUniform1i(this->uniformLoc_directionalShadowPass, FALSE);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			//2

			//normal render
			glUniform1i(this->uniformLoc_bIsSecondRenderPass, FALSE);

			// Point rendering to the off-screen frame buffer object for player1
			glBindFramebuffer(GL_FRAMEBUFFER, this->FBO_bloom[0]);

			// Clear the offscreen framebuffer
			glViewport(0, 0, this->FBO_pixelSize, this->FBO_pixelSize);
			GLfloat	zero = 0.0f;
			GLfloat one = 1.0f;
			glClearBufferfv(GL_COLOR, 0, &zero);
			glClearBufferfv(GL_DEPTH, 0, &one);

			width = this->FBO_pixelSize;
			height = this->FBO_pixelSize;

			ratio = width / (float)height;

			glViewport(0, 0, width, height);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, this->FBO_depthMap_depthTexture);
			glUniform1i(this->directionalLight_shadowSampler, 1);

			//gelCameraManager::sharedInstance().mainCamera->WindowResize(width, height);

			//Draw Scene into FBO[0]
			gelRenderManager::sharedInstance().Update(deltaTime);


			//render brightness
			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			glfwGetFramebufferSize(window, &width, &height);
			ratio = width / (float)height;
			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Set the uniform to the 2nd pass

			glActiveTexture(GL_TEXTURE20);
			glBindTexture(GL_TEXTURE_2D, FBO_bloom_colorTexture[1]);
			glUniform1i(this->uniformLoc_offscreenColorTexture, 20);

			glUniform1i(this->uniformLoc_bIsSecondRenderPass, TRUE);
			glUniform1i(this->uniformLoc_secondPassMode, 1);

			//Draw the screen quad
			gelRenderManager::sharedInstance().DrawFullScreenQuad();

			glUniform1i(this->uniformLoc_bIsSecondRenderPass, FALSE);
		}

		//Update Sounds, maybe somewhere else?
		gelSoundManager::sharedInstance().Update(deltaTime);


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}


//game loop above



void GelsenizeCore::LoadGameObjects(void)
{
}

float GelsenizeCore::GetGlobalClock(void)
{
	return ((float)((int)this->globalClock));
}

float GelsenizeCore::GetDeltaTime(void)
{
	static double lastTime = 0;
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);
	lastTime = glfwGetTime();
	return deltaTime;
}

void GelsenizeCore::SetupBloomFBO(void)
{
	static const GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };

	//glCreateFramebuffers( 1, &g_FBO );		// 4.5 only
	glGenFramebuffers(1, &FBO_bloom[0]);			// every other GL
	glBindFramebuffer(GL_FRAMEBUFFER, FBO_bloom[0]);

	glGenTextures(2, &FBO_bloom_colorTexture[0]);
	for (int k = 0; k != 2; k++)
	{
		glBindTexture(GL_TEXTURE_2D, FBO_bloom_colorTexture[k]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, FBO_pixelSize, FBO_pixelSize);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture(GL_FRAMEBUFFER,
			attachments[k],
			FBO_bloom_colorTexture[k], 0);
	}


	glGenTextures(1, &FBO_bloom_depthTexture);
	glBindTexture(GL_TEXTURE_2D, FBO_bloom_depthTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, FBO_pixelSize, FBO_pixelSize);

	glFramebufferTexture(GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		FBO_bloom_depthTexture, 0);

	static const GLenum draw_buffers[] = { attachments[0], attachments[1] };
	glDrawBuffers(2, draw_buffers);

	// Point back to default frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, 0);



	//ping pong blur's fbo
	glGenFramebuffers(2, FBO_bloomBlur);
	glGenTextures(2, FBO_bloomBlur_colorTexture);
	for (GLuint i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, FBO_bloomBlur[i]);
		glBindTexture(GL_TEXTURE_2D, FBO_bloomBlur_colorTexture[i]);
		glTexImage2D(
			GL_TEXTURE_2D, 0, GL_RGB16F, FBO_pixelSize, FBO_pixelSize, 0, GL_RGB, GL_FLOAT, NULL
		);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, FBO_bloomBlur_colorTexture[i], 0
		);
	}

}

void GelsenizeCore::SetupFBO(void)
{
	static const GLenum attachments[4] = { 
		GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4, GL_COLOR_ATTACHMENT5 };

	for (int i = 0; i < 4; i++)
	{
		//glCreateFramebuffers( 1, &g_FBO );		// 4.5 only
		glGenFramebuffers(1, &FBO[i]);			// every other GL
		glBindFramebuffer(GL_FRAMEBUFFER, FBO[i]);

		glGenTextures(1, &FBO_colourTexture[i]);
		for (int k = 0; k != 2; k++)
		{
			
		}

		glBindTexture(GL_TEXTURE_2D, FBO_colourTexture[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, FBO_pixelSize, FBO_pixelSize);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture(GL_FRAMEBUFFER,
			attachments[i],
			FBO_colourTexture[i], 0);

		glGenTextures(1, &FBO_depthTexture[i]);
		glBindTexture(GL_TEXTURE_2D, FBO_depthTexture[i]);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, FBO_pixelSize, FBO_pixelSize);

		glFramebufferTexture(GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			FBO_depthTexture[i], 0);

		static const GLenum draw_buffers[] = { attachments[i] };
		glDrawBuffers(1, draw_buffers);

		// Point back to default frame buffer
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}


	return;
}

void GelsenizeCore::SetupDirectionalShadow(void)
{
	

	GLuint depthMapFBO;
	glGenFramebuffers(1, &this->FBO_depthMap);
	// - Create depth texture
	GLuint depthMap;
	glGenTextures(1, &this->FBO_depthMap_depthTexture);
	glBindTexture(GL_TEXTURE_2D, this->FBO_depthMap_depthTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	//GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, this->FBO_depthMap);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->FBO_depthMap_depthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}





////////////////////////


/////// UNIFORMS ///////


////////////////////////



float GelsenizeCore::GetPlaygroundScale(void)
{
	return this->playgroundScale;
}

float GelsenizeCore::GetPlayerSpeed(void)
{
	return this->playerSpeed;
}

float GelsenizeCore::GetAttentionRadius(void)
{
	return this->attentionRadius;
}

float GelsenizeCore::GetDangerRadius(void)
{
	return this->dangerRadius;
}

GLFWwindow * GelsenizeCore::GetWindow(void)
{
	return this->window;
}

GLuint GelsenizeCore::GetModelLocation(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].locID_matModel;
}

GLuint GelsenizeCore::GetViewLocation(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].locID_matView;
}

GLuint GelsenizeCore::GetProjLocation(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].locID_matProj;
}

GLuint GelsenizeCore::ObjectAlphaID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_object_alpha;
}

GLuint GelsenizeCore::DiscardTextureBlackID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_bIsDiscardTextureBlack;
}

GLuint GelsenizeCore::TexSamp2DID(int index, std::string shaderName)
{
	if (index >= (int)textureCount)
	{
		GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
		return this->mapShaderUniform[programID].uniformLoc_texSamp2D[0];
	}
	else
	{
		GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
		return this->mapShaderUniform[programID].uniformLoc_texSamp2D[index];
	}
}

GLuint GelsenizeCore::UsedTexNumID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_usedTexNum;
}

GLuint GelsenizeCore::CubemapSkyBoxSamplerID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_myCubemapSkyBoxSampler;
}

GLuint GelsenizeCore::IsCubeMapObjectID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_bIsCubeMapObject;
}

GLuint GelsenizeCore::UseLightingID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_bUseLighting;
}

GLuint GelsenizeCore::IsImposterBlackIsAlphaID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_bIsImposterBlackIsAlpha;
}

GLuint GelsenizeCore::UseTextureAsDiffuseID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_bUseTextureAsDiffuse;
}

GLuint GelsenizeCore::AlphaID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_alpha;
}

GLuint GelsenizeCore::ModelColorID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_ModelColor;
}

GLuint GelsenizeCore::isWireframeID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_isWireframe;
}

GLuint GelsenizeCore::UseGlobalAlphaValue(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_use_alpha;
}

GLuint GelsenizeCore::GlobalAlphaValue(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_alpha;
}

GLuint GelsenizeCore::DebugColorID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_userDebugColorsNoLighting;
}

GLuint GelsenizeCore::EyeLocationID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_eyeLocation;
}

GLuint GelsenizeCore::TextureExtraID(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_textureExtra;
}

GLuint GelsenizeCore::ClothPass(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformId_cloth_pass;
}

GLuint GelsenizeCore::BackFaceDrawPassMode(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_backFaceDrawPassMode;
}

GLuint GelsenizeCore::BackFaceColor(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_backFaceColor;
}

GLuint GelsenizeCore::BackFaceOffset(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_backFaceOffset;
}

GLuint GelsenizeCore::IsASkinnedMesh(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_bIsASkinnedMesh;
}

GLuint GelsenizeCore::BoneID(int index, std::string shaderName)
{
	if (index >= 100)
	{
		return -1;
	}

	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_bones[index];
}

GLuint GelsenizeCore::NumberOfBonesUsed(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_numberOfBonesUsed;
}

GLuint GelsenizeCore::SlideDistance(std::string shaderName)
{
	GLuint programID = gelShaderManager::sharedInstance().getIDWithName(shaderName);
	return this->mapShaderUniform[programID].uniformLoc_slideDistance;
}

GLuint GelsenizeCore::GlowColor()
{
	return this->uniformLoc_glowColor;
}

GLuint GelsenizeCore::HasGlow()
{
	return this->uniformLoc_isGlowing;
}

GLuint GelsenizeCore::ExplodeTime()
{
	return this->uniformLoc_explodeTime;
}

GLuint GelsenizeCore::ReflectsSkybox()
{
	return this->uniformLoc_reflectsSkybox;
}

void GelsenizeCore::SetAIProps(float val1, float val2, float val3)
{
	this->playerSpeed = val1;
	this->attentionRadius = val2;
	this->dangerRadius = val3;
}