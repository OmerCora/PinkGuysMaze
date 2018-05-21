
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <sstream>		// "String Stream", used for setting the windows title

#include <vector>
#include <fstream>
#include <string>
#include "iGameObject.h"

#include "GelsenizeCore.h"
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


bool ProcessShaderFiles(void)
{
	gelShaderManager::cShader vertexShader;
	gelShaderManager::cShader geometryShader;
	gelShaderManager::cShader fragmentShader;

	vertexShader.fileName = "resources/shaders/blinn_phongVert.glsl";
	geometryShader.fileName = "resources/shaders/simpleGeom.glsl";
	fragmentShader.fileName = "resources/shaders/blinn_phongFrag.glsl";

	//vertexShader.fileName = "resources/shaders/Shader.vert";
	//fragmentShader.fileName = "resources/shaders/Shader.frag";
	if (!gelShaderManager::sharedInstance().createProgramFromFile("default", vertexShader, geometryShader, fragmentShader))
	{	// Oh no!
		std::cout << gelShaderManager::sharedInstance().getLastError();
		// TODO: Shut it all down... 
		return false;
	}

	////create second shader
	//gelShaderManager::cShader vertexShader2;
	//gelShaderManager::cShader fragmentShader2;

	//vertexShader2.fileName = "resources/shaders/glassVert.glsl";
	//fragmentShader2.fileName = "resources/shaders/glassFrag.glsl";
	//if (!gelShaderManager::sharedInstance().createProgramFromFile("glass", vertexShader2, fragmentShader2))
	//{	// Oh no!
	//	std::cout << gelShaderManager::sharedInstance().getLastError();
	//	// TODO: Shut it all down... 
	//	return false;
	//}

	//no error checking right now
	return true;
}

bool ReadGameFiles_2(void)
{
	//read whole file as string
	std::ifstream gameFile("resources/gameFile/gameFile.json");
	std::string fileContent((std::istreambuf_iterator<char>(gameFile)), std::istreambuf_iterator<char>());
	const char* fileContent_charArr = fileContent.c_str();
	rapidjson::Document document;

	document.Parse(fileContent_charArr);

	//not doing eloborate validation for the json currently, 
	if (document.HasMember("model_list"))
	{
		const rapidjson::Value& modelListValue = document["model_list"];
		if (modelListValue.IsArray())
		{
			for (rapidjson::SizeType i = 0; i < modelListValue.Size(); i++)
			{
				std::string alias = modelListValue[i]["name"].GetString();
				std::string fileName = modelListValue[i]["path"].GetString();

				std::string shader;

				bool isCollisionModel = false;
				float collisionScale = 1.0f;

				if (modelListValue[i].HasMember("generate_collision_data"))
				{
					isCollisionModel = modelListValue[i]["generate_collision_data"].GetBool();
				}
				if (modelListValue[i].HasMember("collision_scale"))
				{
					collisionScale = modelListValue[i]["collision_scale"].GetInt();
				}
				if (modelListValue[i].HasMember("shader"))
				{
					shader = modelListValue[i]["shader"].GetString();
				}
				else
				{
					shader = "default";
				}

				if (modelListValue[i].HasMember("collision_sphere_array"))
				{
					const rapidjson::Value& sphererArrayValueValue = modelListValue[i]["collision_sphere_array"];
					if (sphererArrayValueValue.IsArray())
					{
						std::vector< CollisionInfo::CollisionSphere > collisionSphereVector;
						for (rapidjson::SizeType k = 0; k < sphererArrayValueValue.Size(); k++)
						{
							CollisionInfo::CollisionSphere sphere;
							sphere.center_offset.x = sphererArrayValueValue[k]["offset_center"]["x"].GetFloat();
							sphere.center_offset.y = sphererArrayValueValue[k]["offset_center"]["y"].GetFloat();
							sphere.center_offset.z = sphererArrayValueValue[k]["offset_center"]["z"].GetFloat();
							sphere.radius = sphererArrayValueValue[k]["radius"].GetFloat();
							collisionSphereVector.push_back(sphere);
						}
						GelCollisionManager::sharedInstance().SetSphereArrayForAlias(alias, collisionSphereVector);
					}
				}

				//error check later and different shader
				gelMeshManager::sharedInstance().LoadPlyIntoGLBuffer(gelShaderManager::sharedInstance().getIDWithName(shader), alias, fileName, isCollisionModel, collisionScale);
			}
		}
	}

	int textureCount = 6;
	//get textures
	if (document.HasMember("texture_list"))
	{
		GelTextureManager::sharedInstance().SetBasePath("resources/textures");
		const rapidjson::Value& textureListValue = document["texture_list"];
		if (textureListValue.IsArray())
		{
			unsigned int loopCount = textureListValue.Size();
			if (textureCount < textureListValue.Size())
			{
				loopCount = textureCount;
			}

			for (rapidjson::SizeType i = 0; i < loopCount; i++)
			{
				std::string alias = textureListValue[i]["name"].GetString();
				std::string fileName = textureListValue[i]["path"].GetString();
				//also get shader name too mb
				//error check later and different shader
				
				//this->uniformLoc_texSamp2D[i]
				GelTextureManager::sharedInstance().Create2DTextureFromBMPFile(alias, fileName, GelsenizeCore::sharedInstance().TexSamp2DID(i, "default"), i, true);
			}
		}
	}

	if (document.HasMember("assimp_list"))
	{
		gelMeshManager::sharedInstance().SetBasePath("resources/animli");
		const rapidjson::Value& assimpListValue = document["assimp_list"];
		if (assimpListValue.IsArray())
		{
			unsigned int loopCount = assimpListValue.Size();

			for (rapidjson::SizeType i = 0; i < loopCount; i++)
			{
				std::string alias = assimpListValue[i]["name"].GetString();
				std::string fileName = assimpListValue[i]["path"].GetString();
				bool fixRootBone = assimpListValue[i]["fixRootBone"].GetBool();

				//call 
				gelMeshManager::sharedInstance().LoadAddAssimpModel(alias, fileName, fixRootBone);
			}
		}
	}


	if (document.HasMember("skybox_textures"))
	{
		const rapidjson::Value& skyboxValue = document["skybox_textures"];
		// Clear any gl errors
		GLenum ignoreerorr = glGetError();

		GelTextureManager::sharedInstance().SetBasePath("resources/textures/skybox");
		GLenum glErrorEnum = 0;
		std::string errorString, errorDetails;
		if (!GelTextureManager::sharedInstance().CreateNewCubeTextureFromBMPFiles("skybox",
			skyboxValue["right"].GetString(),
			skyboxValue["left"].GetString(),
			skyboxValue["top"].GetString(),
			skyboxValue["bottom"].GetString(),
			skyboxValue["front"].GetString(),
			skyboxValue["back"].GetString(),
			true, glErrorEnum,
			errorString,
			errorDetails))
		{
			std::cout << "Trouble loading skybox" << std::endl;
			std::cout << "Error: " << errorString << std::endl;
			std::cout << "Details: " << errorDetails << std::endl;
		}
	}

	int curiousCount = 0;
	int angryCount = 0;
	float averageSpeed = 0.0f;

	if (document.HasMember("project_setup"))
	{
		curiousCount = document["project_setup"]["curious_count"].GetInt();
		angryCount = document["project_setup"]["angry_count"].GetInt();
		averageSpeed = document["project_setup"]["average_speed"].GetFloat();
		GelsenizeCore::sharedInstance().SetAIProps(
			document["project_setup"]["player_speed"].GetFloat(),
			document["project_setup"]["attention_radius"].GetFloat(),
			document["project_setup"]["danger_radius"].GetFloat() );

		int flockCount = document["project_setup"]["flock_count"].GetInt();;

		//flock creation

		iGameObject *flock = new cFlock(flockCount, glm::vec3(10.0f, 16.0f, 10.0f));
		RenderInfo renderFlock;
		//PhysicsInfo physicsFlock;
		//physicsFlock.enabled = true;
		renderFlock.position = glm::vec3(10.0f, 16.0f, 10.0f);
		flock->SetRender(renderFlock);
		//flock->SetPhysics(physicsFlock);
		//gelObjectManager::sharedInstance().AddGameObject(flock);

	}

	if (document.HasMember("player"))
	{
		const rapidjson::Value& playerValue = document["player"];
		std::string objectType = playerValue["object_type"].GetString();
		std::string meshName = playerValue["mesh_name"].GetString();

		MeshInfo playerMesh;
		playerMesh.name = meshName;

		RenderInfo playerRender;

		if (playerValue.HasMember("texture_name_0"))
		{
			playerMesh.textureName_0 = playerValue["texture_name_0"].GetString();
			playerMesh.usedTextureNum = 1;
		}
		else
		{
			playerRender.useTextureAsDiffuse = false;
			playerMesh.usedTextureNum = 0;
		}

		if (playerValue.HasMember("position"))
		{
			playerRender.position = GelPosition(
				playerValue["position"]["x"].GetFloat(),
				playerValue["position"]["y"].GetFloat(),
				playerValue["position"]["z"].GetFloat());
		}

		if (playerValue.HasMember("rotation"))
		{
			/*objectRender.rotation = GelRotation(
			playerValue["rotation"]["x"].GetFloat(),
			playerValue["rotation"]["y"].GetFloat(),
			playerValue["rotation"]["z"].GetFloat());*/
		}

		if (playerValue.HasMember("orientation"))
		{
			playerRender.orientation.x = playerValue["orientation"]["x"].GetFloat();
			playerRender.orientation.y = playerValue["orientation"]["y"].GetFloat();
			playerRender.orientation.z = playerValue["orientation"]["z"].GetFloat();
			playerRender.orientation.w = playerValue["orientation"]["w"].GetFloat();
		}


		if (playerValue.HasMember("color"))
		{
			playerRender.color = glm::vec4(
				playerValue["color"]["red"].GetFloat(),
				playerValue["color"]["green"].GetFloat(),
				playerValue["color"]["blue"].GetFloat(),
				playerValue["color"]["alpha"].GetFloat());
		}

		std::string shader;
		if (playerValue.HasMember("shader"))
		{
			shader = playerValue["shader"].GetString();
		}
		else
		{
			shader = "default";
		}
		playerRender.shaderProgramName = shader;

		playerRender.scale = playerValue["scale"].GetFloat();
		playerRender.isWireframe = playerValue["wireframe"].GetBool();

		playerRender.isAABBDrawn = false;

		PhysicsInfo playerPhysics;

		iGameObject* player = gelObjectFactory::sharedInstance().createGameObject(objectType);

		//attach playerObject to characterController (Kinematic)
		//it is now controlled outside of bullet's dynamic world
		GelPhysicsManager::sharedInstance().GetWorld()->CreateCharacter(player);

		if (playerValue.HasMember("physics"))
		{
			const rapidjson::Value& playerPhysicsValue = playerValue["physics"];

			playerPhysics.enabled = playerPhysicsValue["enabled"].GetBool();
			playerPhysics.useSphericalBody = playerPhysicsValue["use_spherical_body"].GetBool();

			//playerPhysics.rotationalVelocity = glm::vec3(0.0f, 2.0f, 0.0f);

			if (playerPhysicsValue.HasMember("velocity"))
			{
				playerPhysics.velocity = GelPosition(
					playerPhysicsValue["velocity"]["x"].GetFloat(),
					playerPhysicsValue["velocity"]["y"].GetFloat(),
					playerPhysicsValue["velocity"]["z"].GetFloat());
			}

			if (playerPhysicsValue.HasMember("acceleration"))
			{
				playerPhysics.acceleration = GelPosition(
					playerPhysicsValue["acceleration"]["x"].GetFloat(),
					playerPhysicsValue["acceleration"]["y"].GetFloat(),
					playerPhysicsValue["acceleration"]["z"].GetFloat());
			}

			cRigidBodyDef bodyDef;
			bodyDef.Static = false;
			bodyDef.Mass = 0.0f;
			bodyDef.ownerObject = player;
			//generic rigid body class
			iRigidBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
				bodyDef,
				GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sSphere(1.0f))); //sphere radius 1.0f
			playerPhysics.collisionBody = sphereRigidBody;
			GelPhysicsManager::sharedInstance().GetWorld()->AddBody(sphereRigidBody);
		}

		LightInfo lightAmb;
		lightAmb.bIsEnabled = false;
		lightAmb.position = glm::vec3(playerRender.position.x, playerRender.position.y, playerRender.position.z);	// Likely not needed as vec3 c'tor sets to zeros
		lightAmb.diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
		lightAmb.ambient = glm::vec3(0.0f, 0.0f, 0.0f);
		lightAmb.specular = glm::vec4(0.7f, 0.7f, 0.7f, 100.0f);
		lightAmb.attenConst = 0.0f;
		lightAmb.attenLinear = 0.001f;
		lightAmb.attenQuad = 0.03f;

		lightAmb.typeAndParams.x = 1; //set to spotlight
		lightAmb.typeAndParams.y = 0.999f;
		lightAmb.typeAndParams.z = 0.990f;
		lightAmb.direction = glm::vec4(1.0f, -1.0f, 0.0f, 0.0f);

		//get model radius
		float model_radius = gelMeshManager::sharedInstance().LookUpMeshRadius(playerMesh.name);
		playerRender.SetRadius(model_radius);


		if (player != 0)
		{
			player->SetObjectName("player");

			//set skinned mesh info
			playerMesh.isASkinnedMesh = true;
			playerMesh.animationController = new AnimationController();
			//playerMesh.mainAnimationName = "CHAR_IDLE";
			//playerMesh.skinnedMesh = new cSimpleAssimpSkinnedMesh();
			//bool loaded = playerMesh.skinnedMesh->LoadMeshFromFile("resources/animli/RPG-Character_Unarmed-Attack-R3(FBX2013).FBX");
			//bool loaded = playerMesh.skinnedMesh->LoadMeshFromFile("resources/animli/RPG-Character_Unarmed-Attack-Kick-L1(FBX2013).FBX");
			//bool loaded = playerMesh.skinnedMesh->LoadMeshFromFile("resources/animli/RPG-Character_Unarmed-Walk-Slow(FBX2013).FBX");

			//bool loaded = playerMesh.skinnedMesh->LoadMeshFromFile("resources/animli/RPG-Character@Idle.FBX");
			//playerMesh.skinnedMesh->CreateVBOfromCurrentMesh();

			//bool loaded = playerMesh.skinnedMesh->LoadMeshFromFile("resources/animli/RPG-Character(FBX2013).FBX");

			CollisionInfo collision;
			collision.type = CollisionInfo::TYPE::SPHERE_ARRAY;
			collision.debug_mode = CollisionInfo::DEBUG_MODE::NO_DRAW;
			std::vector<CollisionInfo::CollisionSphere> sphereArray;
			GelCollisionManager::sharedInstance().GetSphereArrayForAlias(meshName, sphereArray);
			collision.SetCollisionMode_SphereArray(sphereArray);
			player->SetCollision(collision);

			player->SetLight(lightAmb);

			player->SetMesh(playerMesh);
			player->SetRender(playerRender);
			player->SetPhysics(playerPhysics);
			gelObjectManager::sharedInstance().SetMainPlayerID(player->getObjectID()); //1 player only atm
			gelObjectManager::sharedInstance().AddGameObject(player);
		}
	}

	if (document.HasMember("objects"))
	{
		const rapidjson::Value& objectsValue = document["objects"];
		if (objectsValue.HasMember("playground"))
		{
			const rapidjson::Value& playgroundValue = objectsValue["playground"];

			std::string objectType = playgroundValue["object_type"].GetString();
			std::string meshName = playgroundValue["mesh_name"].GetString();

			MeshInfo objectMesh;
			objectMesh.name = meshName;
			objectMesh.textureName_0 = "pebbles";

			//TODO: move those to a "render" or sth like physics
			RenderInfo objectRender;
			objectRender.useTextureAsDiffuse = false;
			if (playgroundValue.HasMember("position"))
			{
				objectRender.position = GelPosition(
					playgroundValue["position"]["x"].GetFloat(),
					playgroundValue["position"]["y"].GetFloat(),
					playgroundValue["position"]["z"].GetFloat());
			}

			if (playgroundValue.HasMember("texture_name_0"))
			{
				objectMesh.textureName_0 = playgroundValue["texture_name_0"].GetString();
			}

			if (playgroundValue.HasMember("rotation"))
			{
				/*objectRender.rotation = GelRotation(
				playgroundValue["rotation"]["x"].GetFloat(),
				playgroundValue["rotation"]["y"].GetFloat(),
				playgroundValue["rotation"]["z"].GetFloat());*/
			}

			if (playgroundValue.HasMember("orientation"))
			{
				objectRender.orientation.x = playgroundValue["orientation"]["x"].GetFloat();
				objectRender.orientation.y = playgroundValue["orientation"]["y"].GetFloat();
				objectRender.orientation.z = playgroundValue["orientation"]["z"].GetFloat();
				objectRender.orientation.w = playgroundValue["orientation"]["w"].GetFloat();
			}

			if (playgroundValue.HasMember("color"))
			{
				objectRender.color = glm::vec4(
					playgroundValue["color"]["red"].GetFloat(),
					playgroundValue["color"]["green"].GetFloat(),
					playgroundValue["color"]["blue"].GetFloat(),
					playgroundValue["color"]["alpha"].GetFloat());
			}

			float playgroundScale = playgroundValue["scale"].GetFloat();
			GelsenizeCore::sharedInstance().SetPlaygroundScale(playgroundScale);
			objectRender.scale = playgroundScale;
			objectRender.isWireframe = playgroundValue["wireframe"].GetBool();

			//get model radius
			float model_radius = gelMeshManager::sharedInstance().LookUpMeshRadius(objectMesh.name);
			objectRender.SetRadius(model_radius);

			iGameObject* object = gelObjectFactory::sharedInstance().createGameObject(objectType);
			if (object != 0)
			{
				PhysicsInfo physics;
				cRigidBodyDef bodyDef;
				bodyDef.Static = false;
				bodyDef.Mass = 1.0f;

				//generic rigid body class
				iRigidBody* sphereRigidBody = GelPhysicsManager::sharedInstance().GetFactory()->CreateRigidBody(
					bodyDef,
					GelPhysicsManager::sharedInstance().GetFactory()->CreateShape(nPhysics::nShapes::sPlane())); //sphere radius 1.0f
				physics.collisionBody = sphereRigidBody;
				object->SetPhysics(physics);

				object->SetObjectName("playground");
				object->SetMesh(objectMesh);
				object->SetRender(objectRender);
				gelObjectManager::sharedInstance().AddGameObject(object);
			}

		}

		if (objectsValue.HasMember("game_object_list"))
		{
			const rapidjson::Value& objectListValue = objectsValue["game_object_list"];
			if (objectListValue.IsArray())
			{
				for (rapidjson::SizeType i = 0; i < objectListValue.Size(); i++)
				{
					const rapidjson::Value& objectValue = objectListValue[i];

					std::string objectType = objectValue["object_type"].GetString();
					std::string meshName = objectValue["mesh_name"].GetString();
					RenderInfo objectRender;
					MeshInfo objectMesh;
					objectMesh.name = meshName;

					if (objectValue.HasMember("texture_name_0"))
					{
						objectMesh.textureName_0 = objectValue["texture_name_0"].GetString();
						objectMesh.usedTextureNum = 0;
						objectRender.useTextureAsDiffuse = true;
					}
					else
					{
						objectRender.useTextureAsDiffuse = false;
					}

					if (objectValue.HasMember("position"))
					{
						objectRender.position = GelPosition(
							objectValue["position"]["x"].GetFloat(),
							objectValue["position"]["y"].GetFloat(),
							objectValue["position"]["z"].GetFloat());
					}

					if (objectValue.HasMember("rotation"))
					{
						/*objectRender.rotation = GelRotation(
						objectValue["rotation"]["x"].GetFloat(),
						objectValue["rotation"]["y"].GetFloat(),
						objectValue["rotation"]["z"].GetFloat());*/
					}

					if (objectValue.HasMember("orientation"))
					{
						objectRender.orientation.x = objectValue["orientation"]["x"].GetFloat();
						objectRender.orientation.y = objectValue["orientation"]["y"].GetFloat();
						objectRender.orientation.z = objectValue["orientation"]["z"].GetFloat();
						objectRender.orientation.w = objectValue["orientation"]["w"].GetFloat();
					}

					if (objectValue.HasMember("color"))
					{
						objectRender.color = glm::vec4(
							objectValue["color"]["red"].GetFloat(),
							objectValue["color"]["green"].GetFloat(),
							objectValue["color"]["blue"].GetFloat(),
							objectValue["color"]["alpha"].GetFloat());
					}

					std::string shader;
					if (objectValue.HasMember("shader"))
					{
						shader = objectValue["shader"].GetString();
					}
					else
					{
						shader = "default";
					}

					objectRender.shaderProgramName = shader;

					objectRender.scale = objectValue["scale"].GetFloat();
					objectRender.isWireframe = objectValue["wireframe"].GetBool();

					PhysicsInfo objectPhysics;

					if (objectValue.HasMember("physics"))
					{
						const rapidjson::Value& objectPhysicsValue = objectValue["physics"];

						objectPhysics.enabled = objectPhysicsValue["enabled"].GetBool();
						objectPhysics.useSphericalBody = objectPhysicsValue["use_spherical_body"].GetBool();

						if (objectPhysicsValue.HasMember("velocity"))
						{
							objectPhysics.velocity = GelVelocity(
								objectPhysicsValue["velocity"]["x"].GetFloat(),
								objectPhysicsValue["velocity"]["y"].GetFloat(),
								objectPhysicsValue["velocity"]["z"].GetFloat());
						}

						if (objectPhysicsValue.HasMember("acceleration"))
						{
							objectPhysics.acceleration = GelAcceleration(
								objectPhysicsValue["acceleration"]["x"].GetFloat(),
								objectPhysicsValue["acceleration"]["y"].GetFloat(),
								objectPhysicsValue["acceleration"]["z"].GetFloat());
						}
					}

					//get model radius
					float model_radius = gelMeshManager::sharedInstance().LookUpMeshRadius(objectMesh.name);
					objectRender.SetRadius(model_radius);

					iGameObject* object = gelObjectFactory::sharedInstance().createGameObject(objectType);
					if (object != 0)
					{
						if (objectValue.HasMember("name"))
						{
							object->SetObjectName(objectValue["name"].GetString());
						}

						object->SetMesh(objectMesh);
						object->SetRender(objectRender);
						object->SetPhysics(objectPhysics);
						gelObjectManager::sharedInstance().AddGameObject(object);
					}
				}
			}
		}

		//get a camera
		cCamera *camera = new cCamera();
		cCamera *cameraOffScreen = new cCamera();

		if (document.HasMember("camera"))
		{
			const rapidjson::Value& cameraValue = document["camera"];
			if (cameraValue.HasMember("offset"))
			{
				glm::vec3 offset(
					cameraValue["offset"]["x"].GetFloat(),
					cameraValue["offset"]["y"].GetFloat(),
					cameraValue["offset"]["z"].GetFloat());
				camera->SetOffset(offset);
			}
		}
		gelCameraManager::sharedInstance().mainCamera = camera;

		cameraOffScreen->SetCameraMode(cCamera::MODE::FREECAM);
		gelCameraManager::sharedInstance().offScreenCamera = cameraOffScreen;
		gelCameraManager::sharedInstance().offScreenCamera->SetEyePosition(glm::vec4(0.0f, 0.0f, 0.0f, 0.0f));
		gelCameraManager::sharedInstance().offScreenCamera->SetAtPosition(glm::vec3(0.0f, 0.0f, 0.0f));

		//now create objects
		for (int i = 0; i < curiousCount; i++)
		{
			iGameObject* gameObject = gelObjectBuilder::sharedInstance().buildSeekerObject("curious", averageSpeed);

			MeshInfo tmesh;
			tmesh.name = "plane1";
			tmesh.textureName_0 = "pebbles";
			tmesh.textureName_1 = "fenceAlphaMask";
			tmesh.usedTextureNum = 2;
			gameObject->SetMesh(tmesh);

			RenderInfo trender = gameObject->GetRender();
			trender.useTextureAsDiffuse = true;
			float randX = OCHelper::getRandFloat(0.0f, GelsenizeCore::sharedInstance().GetPlaygroundScale());
			float randZ = OCHelper::getRandFloat(0.0f, GelsenizeCore::sharedInstance().GetPlaygroundScale());
			trender.position = GelPosition(randX, 5.0f, randZ);
			trender.scale = 2.0f;
			trender.alpha = 1.0f;
			gameObject->SetRender(trender);

			CollisionInfo collision;
			collision.type = CollisionInfo::TYPE::SPHERE_ARRAY;
			std::vector<CollisionInfo::CollisionSphere> sphereArray;
			GelCollisionManager::sharedInstance().GetSphereArrayForAlias(tmesh.name, sphereArray);
			collision.SetCollisionMode_SphereArray(sphereArray);
			gameObject->SetCollision(collision);

			PhysicsInfo tphysics = gameObject->GetPhysics();
			tphysics.enabled = true;
			gameObject->SetPhysics(tphysics);
			gelObjectManager::sharedInstance().AddGameObject(gameObject);
		}
		for (int i = 0; i < angryCount; i++)
		{
			iGameObject* gameObject = gelObjectBuilder::sharedInstance().buildSeekerObject("angry", averageSpeed);

			MeshInfo tmesh;
			tmesh.name = "plane1";
			tmesh.textureName_0 = "brick";
			tmesh.textureName_1 = "fenceAlphaMask";
			tmesh.usedTextureNum = 2;

			gameObject->SetMesh(tmesh);

			RenderInfo trender = gameObject->GetRender();
			trender.useTextureAsDiffuse = false;
			trender.position = GelPosition(OCHelper::getRandFloat(0.0f, GelsenizeCore::sharedInstance().GetPlaygroundScale()), 5.0f, OCHelper::getRandFloat(0.0f, GelsenizeCore::sharedInstance().GetPlaygroundScale()));
			trender.scale = 2.0f;
			trender.alpha = 1.0f;
			//trender.AABBwidth = 10.0f; test
			//trender.isAABBDrawn = true;
			gameObject->SetRender(trender);

			CollisionInfo collision;
			collision.type = CollisionInfo::TYPE::SPHERE_ARRAY;
			std::vector<CollisionInfo::CollisionSphere> sphereArray;
			GelCollisionManager::sharedInstance().GetSphereArrayForAlias(tmesh.name, sphereArray);
			collision.SetCollisionMode_SphereArray(sphereArray);
			gameObject->SetCollision(collision);

			PhysicsInfo tphysics = gameObject->GetPhysics();
			tphysics.enabled = true;
			gameObject->SetPhysics(tphysics);
			gelObjectManager::sharedInstance().AddGameObject(gameObject);
		}

	}
	return true;
	//json file read
	//TODOs
	//read shader file later too, think about this when begin using multiple shaders

	//read assets

	//read game objects 

	//read camera

	//read player

	//read lighting

	//soon-> read textures?
}


//maybe its manager soon?
bool ReadGameFiles(void)
{
	std::string fileName = "resources/gameFile/gameFile.txt";
	// Note: "C-style" string conversion
	std::ifstream gameFile(fileName.c_str());
	if (!gameFile.is_open())
	{
		//std::cout << "Can't open file" << std::endl;
		return false;
	}

	//sound
	int streamSoundCount = 0;
	int sampleSoundCount = 0;

	int modelCount = 0;
	int playerCount = 0;

	std::string readContent;

	//move head to stream_sound
	do {
		gameFile >> readContent;
	} while (readContent != "stream_sound");

	gameFile >> streamSoundCount;

	for (int i = 0; i < streamSoundCount; i++)
	{
		std::string soundAlias;
		std::string soundPath;
		gameFile >> soundAlias >> soundPath;

		gelSoundManager::sharedInstance().CreateStreamSound(soundAlias, soundPath);
	}

	//move head to stream_sound
	do {
		gameFile >> readContent;
	} while (readContent != "sample_sound");

	gameFile >> sampleSoundCount;

	for (int i = 0; i < sampleSoundCount; i++)
	{
		std::string soundAlias;
		std::string soundPath;
		float minDist;
		float maxDist;
		gameFile >> soundAlias >> soundPath >> minDist >> maxDist;

		gelSoundManager::sharedInstance().CreateSampleSound(soundAlias, soundPath, minDist, maxDist);
	}


	//move head to model
	do {
		gameFile >> readContent;
	} while (readContent != "model");

	gameFile >> modelCount;

	for (int i = 0; i < modelCount; i++)
	{
		std::string alias;
		std::string fileName;

		gameFile >> alias >> fileName;

		//error check later and different shader
		gelMeshManager::sharedInstance().LoadPlyIntoGLBuffer(gelShaderManager::sharedInstance().getIDWithName("default"), alias, fileName);
	}

	//move head to player
	do {
		gameFile >> readContent;
	} while (readContent != "player");

	gameFile >> playerCount;

	for (int i = 0; i < playerCount; i++)
	{
		MeshInfo mesh;
		RenderInfo render;
		PhysicsInfo physics;

		std::string str;
		gameFile >> str; //Read "player_start"

		std::string objectType = "";
		//mesh name
		gameFile >> mesh.name >> objectType;

		//position & scale
		//float sound_cd;
		gameFile >> render.position.x >> render.position.y >> render.position.z >> render.scale >> physics.useSphericalBody;// >> sound_cd;

																															//color & wireFrame
		gameFile >> render.color.r >> render.color.g >> render.color.b >> render.color.a >> render.isWireframe;

		//rotation
		gameFile >> render.orientation.x >> render.orientation.y >> render.orientation.z; //>> render.orientation.w;
		render.orientation.w = 1;

		//velocity
		gameFile >> physics.velocity.x >> physics.velocity.y >> physics.velocity.z >> physics.enabled;

		//acceleration
		gameFile >> physics.acceleration.x >> physics.acceleration.y >> physics.acceleration.z;

		//get model radius
		float model_radius = gelMeshManager::sharedInstance().LookUpMeshRadius(mesh.name);
		render.SetRadius(model_radius);

		//mesh.soundCooldown_delta = mesh.soundCooldown;

		iGameObject* player = gelObjectFactory::sharedInstance().createGameObject(objectType);
		if (player != 0)
		{
			player->SetMesh(mesh);
			player->SetRender(render);
			player->SetPhysics(physics);
			//asdf
			gelObjectManager::sharedInstance().SetMainPlayerID(player->getObjectID()); //1 player only atm
			gelObjectManager::sharedInstance().AddGameObject(player);
		}
		gameFile >> str; //Read "player_end"
	}

	do
	{
		gameFile >> readContent;
	} while (readContent != "camera");

	//gameFile >> uselessPlyContent;

	do
	{
		gameFile >> readContent;
	} while (readContent != "object");

	int objectCount = 0;
	gameFile >> objectCount;

	for (int i = 0; i < objectCount; i++)
	{
		std::string str;
		gameFile >> str; //Read "object_start"

		MeshInfo mesh;
		RenderInfo render;
		PhysicsInfo physics;

		//render.useDebugColorsNoLighting = true;

		render.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		std::string objectType = "";

		//mesh name
		gameFile >> mesh.name >> objectType;

		//float sound_cd;
		//position & scale
		gameFile >> render.position.x >> render.position.y >> render.position.z >> render.scale >> physics.useSphericalBody;// >> sound_cd;

																															//color & wireFrame
		gameFile >> render.color.r >> render.color.g >> render.color.b >> render.color.a >> render.isWireframe;

		//rotation
		gameFile >> render.orientation.x >> render.orientation.y >> render.orientation.z; //>> render.orientation.w;
		render.orientation.w = 1;

		//velocity
		gameFile >> physics.velocity.x >> physics.velocity.y >> physics.velocity.z >> physics.enabled;

		//acceleration
		gameFile >> physics.acceleration.x >> physics.acceleration.y >> physics.acceleration.z;

		//get model radius
		float model_radius = gelMeshManager::sharedInstance().LookUpMeshRadius(mesh.name);
		render.SetRadius(model_radius);

		//mesh.soundCooldown_delta = mesh.soundCooldown;
		
		//render.useDebugColorsNoLighting = false;

		iGameObject* gameObject = gelObjectFactory::sharedInstance().createGameObject(objectType);
		if (gameObject != 0)
		{
			gameObject->SetMesh(mesh);
			gameObject->SetRender(render);
			gameObject->SetPhysics(physics);
			gelObjectManager::sharedInstance().AddGameObject(gameObject);
		}

		gameFile >> str; //Read "object_end"
	}
	gameFile.close();
	return true;
}
