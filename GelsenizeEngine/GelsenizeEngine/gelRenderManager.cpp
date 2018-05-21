#include "gelRenderManager.h"

#include "GelsenizeCore.h"
#include "externals.h"
#include "managers.h"
#include "iGameObject.h"

#include "cAABB.h"

#include <algorithm>
#include <sstream>

#include "cContact.h"

#include "cFlock.h"
#include "OCHelper.h"

//void GelRenderManager::DrawScene(void)
//{
//	return;
//}

void gelRenderManager::Update(float deltaTime)
{
	currentDeltaTime = deltaTime;
	deltaAcc_1 += deltaTime;
	deltaAcc_2 += deltaTime;

	if (deltaAcc_1 >= 1.5f)
	{
		deltaAcc_1 = 0.0f;
	}

	if (deltaAcc_1 == 0.0f)
	{
		accFlag_1 = !accFlag_1;
	}


	//draw each game object
	//DrawPlayer(); //this is now done in DrawGameObjects

	DrawGameObjects();

	std::vector<cContact> contactVec;
	GelCollisionManager::sharedInstance().GetContacts(contactVec);

	for (int i = 0; i != contactVec.size(); i++)
	{
		cContact contact = contactVec[i];

		MeshInfo mesh;
		mesh.name = "sphere";
		RenderInfo render;

		render.position = contact.contactXYZ;
		render.scale = 0.1f;
		render.isWireframe = true;
		//render.isAABBDrawn = true;
		//render.AABBwidth = 10.0f;
		render.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		render.useDebugColorsNoLighting = true;

		this->DrawWithInfos(render, mesh, gelCameraManager::sharedInstance().mainCamera, glm::mat4x4(1.0f));
	}

	if (0 && !directionalShadowPass)
	{
		iGameObject* playerObject = gelObjectManager::sharedInstance().GetMainPlayer();
		glm::vec3 specialScale;
		nPhysics::eShapeType shapeType;
		GelPhysicsManager::sharedInstance().GetWorld()->GetCharacterBodyProperties(specialScale, shapeType);
		for (int i = 0; i != 1; i++)
		{
			MeshInfo hitboxMesh;
			CollisionInfo collisionInfo;
			hitboxMesh.name = "cube";
			RenderInfo hitboxRender;

			hitboxRender.position = playerObject->GetRender().position;
			hitboxRender.orientation = playerObject->GetRender().orientation;
			hitboxRender.scale = 1.0f;
			hitboxRender.isWireframe = true;

			hitboxRender.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
			hitboxRender.useDebugColorsNoLighting = true;

			this->DrawWithInfos(hitboxRender, hitboxMesh, gelCameraManager::sharedInstance().mainCamera, glm::mat4x4(1.0f), true, specialScale);
		}
	}

	//draw test
	//g_meshx

	//gelMeshManager::Mesh* meshTest = gelMeshManager::sharedInstance().GetMeshWithName("test");
	
	DrawSkyboxObject();

	//DrawParticleEmitter();
}

void gelRenderManager::Setup(void)
{
	this->directionalShadowPass = false;

	// Add a "debug AABB"
	this->aabb = gelObjectFactory::sharedInstance().createGameObject("object");

	MeshInfo mesh;
	mesh.name = "aabb_cube";

	RenderInfo render;
	render.scale = 1.0f;

	this->aabb->SetMesh(mesh);
	this->aabb->SetRender(render);
	
	this->textureTimeAcc = 0.0f;


	deltaAcc_1 = 0.0f;
	deltaAcc_2 = 0.0f;

	//TODO: do we do this? hmm
	//gelObjectManager::sharedInstance().AddGameObject(aabb);
}

void gelRenderManager::DrawPlayer(void)
{
	globalAlpha = 1.0f;
	useGlobalAlpha = false;
	
	iGameObject *player = gelObjectManager::sharedInstance().GetMainPlayer();
	this->DrawGameObject(player);
}

void gelRenderManager::DrawGameObjects(void)
{
	std::vector<iGameObject*> gameObjects = gelObjectManager::sharedInstance().GetGameObjects();

	glm::vec4 eyePosition0;
	gelCameraManager::sharedInstance().mainCamera->GetEyePosition(eyePosition0);
	glm::vec3 eyePosition = glm::vec3(eyePosition0.x, eyePosition0.y, eyePosition0.z);

	//reorder by eye vector
	struct CompStruct
	{
		int objectID;
		float distanceToEye;

		CompStruct(int id, float s) : objectID(id), distanceToEye(s) {}
	};

	struct less_than_key
	{
		inline bool operator() (const CompStruct& struct1, const CompStruct& struct2)
		{
			return (struct1.distanceToEye < struct2.distanceToEye);
		}
	};

	std::vector<CompStruct> compVector;

	for (int index = 0; index != gameObjects.size(); index++)
	{
		glm::vec3 objPos = gameObjects[index]->GetRender().position;

		float distance = glm::distance(eyePosition, objPos);
		if (gameObjects[index]->GetObjectName() == "groundPlane")
		{
			distance = 0.0f;
		}

		compVector.push_back(CompStruct(gameObjects[index]->getObjectID(), distance));

		/*if (gameObjects[index]->ObjectType() == "particleEmitter")
		{
			this->DrawParticleEmitter((cParticleEmitter*)gameObjects[index]);
		}
		else
		{
			this->DrawGameObject(gameObjects[index]);
		}
		*/
	}
	std::sort(compVector.begin(), compVector.end(), less_than_key());

	for (int index = 0; index != compVector.size(); index++)
	{
		CompStruct compStruct = compVector[index];
		iGameObject* gameObject = gelObjectManager::sharedInstance().FindGameObjectWithID(compStruct.objectID);
		if (gameObject->ObjectType() == "particleEmitter")
		{
			this->DrawParticleEmitter((cParticleEmitter*)gameObject);
		}
		else if (gameObject->ObjectType() == "flock")
		{
			std::vector<iGameObject*> flockMembers;
			((cFlock*)gameObject)->GetMembers(flockMembers);
			for (int k = 0; k != flockMembers.size(); k++)
			{
				DrawGameObject(flockMembers[k]);
			}
		}
		else if (gameObject->GetObjectName() == "cloth")
		{
			if (gameObject->GetPhysics().collisionBody->GetBodyType() == eBodyType::SOFT_BODY_TYPE)
			{
				//DrawCloth(gameObject, dynamic_cast<cSoftBody*>(gameObject->GetPhysics().collisionBody));
			}
		}
		else if (gameObject->GetObjectName() == "vehicle")
		{
			MeshInfo hitboxMesh;
			CollisionInfo collisionInfo;
			hitboxMesh.name = "cube";
			RenderInfo hitboxRender;

			//glm::vec3 specialScale(4.0f, 2.0f, 8.0f);
			//glm::vec3 specialScale(3.4f, 1.4f, 6.6f);
			glm::vec3 specialScale(2.5f, 1.4f, 5.0f);
			//glm::vec3 specialScale(1.6f, 1.2f, 3.0f);
			//specialScale *= 2.0f;

			hitboxRender.position = gameObject->GetRender().position;
			//hitboxRender.position.y += 1.6f;
			hitboxRender.orientation = gameObject->GetRender().orientation;
			hitboxRender.scale = 1.0f;
			hitboxRender.isWireframe = false;

			hitboxRender.color = glm::vec4(0.8f, 0.0f, 0.8f, 1.0f);
			hitboxRender.useDebugColorsNoLighting = false;

			this->DrawWithInfos(hitboxRender, hitboxMesh, gelCameraManager::sharedInstance().mainCamera, glm::mat4x4(1.0f), true, specialScale);

			//get wheels and draw em

			std::vector<glm::vec3> positions;
			std::vector<glm::quat> orientations;
			std::vector<glm::mat4> transformations;

			GelPhysicsManager::sharedInstance().GetWorld()->GetWheelTransforms(positions, orientations, transformations);

			for (int i = 0; i != positions.size(); i++)
			{
				MeshInfo wheelMesh;
				wheelMesh.name = "wheel";
				RenderInfo wheelRender;

				glm::vec3 specialScale(0.5f, 1.5f, 1.5f);
				//specialScale *= 2.0f;

				wheelRender.position = positions[i];
				wheelRender.orientation = orientations[i];
				wheelRender.transform = transformations[i];
				wheelRender.scale = 1.0f;
				wheelRender.isWireframe = false;

				wheelRender.color = glm::vec4(0.10f, 0.1f, 0.10f, 1.0f);
				wheelRender.useDebugColorsNoLighting = false;

				this->DrawWithInfos(wheelRender, wheelMesh, gelCameraManager::sharedInstance().mainCamera, glm::mat4x4(1.0f), true, specialScale);
			}

		}
		/*else if (gameObject->GetMesh().name == "sphere_nice")
		{
			this->DrawGameObjectCelShaded(gameObject);
		}*/
		else
		{
			//this->DrawGameObject(gameObject);
			this->DrawGameObjectCelShaded(gameObject);
		}
	}
}

void gelRenderManager::DrawParticleEmitter(cParticleEmitter *emitter)
{

	glm::vec4 eyePos;
	gelCameraManager::sharedInstance().mainCamera->GetEyePosition(eyePos);
	emitter->particleSettings.imposterEyeLocation.x = eyePos.x;
	emitter->particleSettings.imposterEyeLocation.y = eyePos.y;
	emitter->particleSettings.imposterEyeLocation.z = eyePos.z;

	std::vector<cParticle> vecParticles;
	gelObjectManager::sharedInstance().GetParticleVectorToRender(emitter->getObjectID(), vecParticles);

	int numParticles = emitter->GetParticlesCopy(vecParticles);
	for (int i = 0; i != numParticles; i++)
	{
		DrawParticle(vecParticles[i]);
	}

	//std::vector<cParticleEmitter*> particleEmitters = gelObjectManager::sharedInstance().GetParticleEmitters();
	//for (int index = 0; index != particleEmitters.size(); index++)
	//{

	//	glm::vec4 eyePos;
	//	gelCameraManager::sharedInstance().mainCamera->GetEyePosition(eyePos);
	//	cParticleEmitter *emitter = particleEmitters[index];
	//	emitter->particleSettings.imposterEyeLocation.x = eyePos.x;
	//	emitter->particleSettings.imposterEyeLocation.y = eyePos.y;
	//	emitter->particleSettings.imposterEyeLocation.z = eyePos.z;

	//	std::vector<cParticle> vecParticles;
	//	gelObjectManager::sharedInstance().GetParticleVectorToRender(index, vecParticles);

	//	int numParticles = emitter->GetParticlesCopy(vecParticles);
	//	for (int i = 0; i != numParticles; i++)
	//	{
	//		DrawParticle(vecParticles[i]);
	//	}


	//	//this->DrawGameObject(particleEmitters[index]);
	//}
}

glm::vec3 calcTriangleNormal(cSoftBody::cNode *n1, cSoftBody::cNode *n2, cSoftBody::cNode *n3)
{
	glm::vec3 pos1 = n1->position;
	glm::vec3 pos2 = n2->position;
	glm::vec3 pos3 = n3->position;

	glm::vec3 v1 = pos2 - pos1;
	glm::vec3 v2 = pos3 - pos1;

	return glm::cross(v1,v2);
}

void drawTriangle(cSoftBody::cNode *n1, cSoftBody::cNode *n2, cSoftBody::cNode *n3, glm::vec3 color);

void gelRenderManager::DrawCloth(iGameObject* clothObject, cSoftBody* softBody)
{
	
	//init normals
	std::vector<cSoftBody::cNode*>::iterator it_node;
	for (it_node = softBody->mNodes.begin(); it_node != softBody->mNodes.end(); it_node++)
	{
		(*it_node)->ResetNormal();
	}

	//smooth normal generation add up each node 
	for (int x = 0; x != softBody->clothWidth_nodeCount - 1; x++)
	{
		for (int y = 0; y != softBody->clothHeight_nodeCount - 1; y++)
		{
			glm::vec3 normal = calcTriangleNormal(softBody->GetNode(x + 1, y), softBody->GetNode(x, y), softBody->GetNode(x, y + 1));
			softBody->GetNode(x + 1, y)->AddToNormal(normal);
			softBody->GetNode(x, y)->AddToNormal(normal);
			softBody->GetNode(x, y + 1)->AddToNormal(normal);

			normal = calcTriangleNormal(softBody->GetNode(x + 1, y + 1), softBody->GetNode(x + 1, y), softBody->GetNode(x, y + 1));
			softBody->GetNode(x + 1, y + 1)->AddToNormal(normal);
			softBody->GetNode(x + 1, y)->AddToNormal(normal);
			softBody->GetNode(x, y + 1)->AddToNormal(normal);
		}
	}

	//clear line buffer and refill
	gelMeshManager::sharedInstance().ClearBuffer();
	//
	for (int x = 0; x != softBody->clothWidth_nodeCount - 1; x++)
	{
		for (int y = 0; y != softBody->clothHeight_nodeCount - 1; y++)
		{
			glm::vec3 color(0, 0, 0);
			if (x % 2) // red and white color is interleaved according to which column number
				color = glm::vec3(0.6f, 0.2f, 0.2f);
			else
				color = glm::vec3(1.0f, 1.0f, 1.0f);

			gelMeshManager::sharedInstance().AddLine(softBody->GetNode(x, y)->position, softBody->GetNode(x + 1 , y)->position, color);
			gelMeshManager::sharedInstance().AddLine(softBody->GetNode(x, y)->position, softBody->GetNode(x + 1, y + 1)->position, color);
			gelMeshManager::sharedInstance().AddLine(softBody->GetNode(x, y)->position, softBody->GetNode(x, y + 1)->position, color);
			gelMeshManager::sharedInstance().AddLine(softBody->GetNode(x+1, y)->position, softBody->GetNode(x+1, y+1)->position, color);

			//drawTriangle(softBody->GetNode(x + 1, y), softBody->GetNode(x, y), softBody->GetNode(x, y + 1), color);
			//drawTriangle(softBody->GetNode(x + 1, y + 1), softBody->GetNode(x + 1, y), softBody->GetNode(x, y + 1), color);
		}
	}






	//start drawing triangles using the nodes

	


	// Draw all the debug lines from the "debug line" VAO
	unsigned int LineBuffer_VAO_ID = 0;
	unsigned int numberOfVerticesToDraw = 0;
	if (gelMeshManager::sharedInstance().GetDebugLineBufferForRender(LineBuffer_VAO_ID, numberOfVerticesToDraw))
	{
		if (numberOfVerticesToDraw > 0)
		{
			//clothObject->GetRender().shaderProgramName
			gelShaderManager::sharedInstance().useShaderProgram("default");

			glm::mat4x4 matModel = glm::mat4x4(1.0f);

			iGameObject *player = gelObjectManager::sharedInstance().GetMainPlayer();

			//glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetModelLocation("default"), 1, GL_FALSE, (const GLfloat*)glm::value_ptr(matModel));
			glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetModelLocation("default"), 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(gelObjectManager::sharedInstance().GetMainPlayer()->GetRender().transform));


			glUniform1i(GelsenizeCore::sharedInstance().ClothPass("default"), TRUE);

			glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
				GL_LINE);			// GL_POINT, GL_LINE, or GL_FILL
			glDisable(GL_CULL_FACE);

			//glUniform1i( UniformLoc_ID_bUseDebugDiffuseNoLighting, FALSE );		// 1

			//glUniform4f( UniformLoc_ID_objectColour, 
			//			 1.0f, 1.0f, 1.0f, 1.0f );
			//glUniform1i( UniformLoc_ID_isWireframe, FALSE );		// 1

			// "Point to" (bind) the VAO with the lines in it
			glBindVertexArray(LineBuffer_VAO_ID);
			glDrawArrays(GL_LINES,
				0,	/*start from start of buffer*/
				numberOfVerticesToDraw);
			// Point to (bind) to nothing
			glBindVertexArray(0);

			glUniform1i(GelsenizeCore::sharedInstance().ClothPass("default"), FALSE);
		}
	}//if ( ! ::g_pTheMeshTypeManager->GetDebugLineBufferForRender

	//glBegin(GL_TRIANGLES);
	
	//glEnd();

}


/* A private method used by drawShaded(), that draws a single triangle p1,p2,p3 with a color*/
void drawTriangle(cSoftBody::cNode *n1, cSoftBody::cNode *n2, cSoftBody::cNode *n3, glm::vec3 color)
{
	float scale = 1.0f;

	glm::vec3 n1Position = n1->position *scale;
	glm::vec3 n2Position = n2->position *scale;
	glm::vec3 n3Position = n3->position *scale;

	//n1Position += initialPosition;
	//n2Position += initialPosition;
	//n3Position += initialPosition;

	glColor3fv((GLfloat*)&color);

	glNormal3fv((GLfloat *) &(n2->GetNormal()));
	glVertex3fv((GLfloat *) &(n2Position));

	glNormal3fv((GLfloat *) &(n1->GetNormal()));
	glVertex3fv((GLfloat *) &(n1Position));

	glNormal3fv((GLfloat *) &(n3->GetNormal()));
	glVertex3fv((GLfloat *) &(n3Position));
}

void gelRenderManager::DrawFullScreenQuad(void)
{
	MeshInfo mesh;
	mesh.name = "screenQuad";
	RenderInfo render;

	render.position = glm::vec3(0.0f, 0.0f, 0.0f);
	render.scale = 1.0f;
	render.isWireframe = false;
	render.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	render.useDebugColorsNoLighting = false;

	this->DrawWithInfos(render, mesh, gelCameraManager::sharedInstance().offScreenCamera, glm::mat4x4(1.0f));
}

void gelRenderManager::DrawWithInfos(RenderInfo render, MeshInfo mesh, cCamera* camera, glm::mat4x4 initialTransform, bool useSpecialScale, glm::vec3 specialScale)
{
	glm::mat4x4 m = initialTransform;//glm::mat4x4(1.0f);
	glm::mat4x4 v; camera->GetViewMatrix(v);
	glm::mat4x4 p; camera->GetProjectionMatrix(p);

	std::string meshModelName = mesh.name;

	GLuint VAO_ID = 0;
	int numberOfIndices = 0;
	float unitScale = 1.0f;
	bool draw = render.isDrawn;
	if (!gelMeshManager::sharedInstance().LookUpMeshInfo(meshModelName, VAO_ID, numberOfIndices, unitScale))
	{
		//could not find the mesh info
		draw = false;
	}

	m = glm::translate(m, glm::vec3(render.position.x, render.position.y, render.position.z));

	//m = glm::rotate(m, render.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	//m = glm::rotate(m, render.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	//m = glm::rotate(m, render.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 matRotation(render.orientation);
	m = m * matRotation;

	if (mesh.name == "wheel")
	{
		glm::quat rot(glm::vec3(0.0f, 1.5708f, 0.0f));
		glm::mat4 matRot(rot);
		m = m * matRot;
	}

	float actualScale = render.scale * unitScale;

	if (useSpecialScale)
	{
		specialScale *= unitScale;
		m = glm::scale(m, glm::vec3(specialScale.x, specialScale.y, specialScale.z));
	}
	else
	{
		m = glm::scale(m, glm::vec3(actualScale, actualScale, actualScale));
	}

	
	if (mesh.name != "wheel")
	{
		render.transform = m;
	}
	else
	{
		render.transform = glm::scale(render.transform, glm::vec3(0.1f, 0.2f, 0.2f));
	}
	

	/*if (specialScale.z > 6000.0f)
	{
		m = render.transform;
	}*/

	//render.lastDrawnActualScale = actualScale;
	//pCurGO->SetRender(render);

	//draw aabb for current game object
	if (render.isAABBDrawn)
	{
		DrawAABB(
			glm::vec3(render.position.x, render.position.y, render.position.z),
			render.color,
			render.AABBwidth);
	}

	if (draw)
	{
		if (render.isWireframe)
		{	// Turn off backface culling
			// Enable "wireframe" polygon mode
			glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
				GL_LINE);			// GL_POINT, GL_LINE, or GL_FILL
			glDisable(GL_CULL_FACE);
		}
		else
		{	// "Regular" rendering: 
			// Turn on backface culling
			// Turn polygon mode to solid (Fill)
			if (directionalShadowPass)
			{
				glCullFace(GL_FRONT); // enable culling of back faces
			}
			else
			{
				glCullFace(GL_BACK); // enable culling of back faces
			}
			//glCullFace(GL_BACK);		// GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
				GL_FILL);			// GL_POINT, GL_LINE, or GL_FILL
		}

		gelShaderManager::sharedInstance().useShaderProgram(render.shaderProgramName);

		if (render.exploding)
		{
			glUniform1f(GelsenizeCore::sharedInstance().ExplodeTime(), render.explodeTime);
		}
		else
		{
			glUniform1f(GelsenizeCore::sharedInstance().ExplodeTime(), 0.0f);
		}

		if (render.hasGlow)
		{
			glUniform1i(GelsenizeCore::sharedInstance().HasGlow(), TRUE);
			glUniform3f(GelsenizeCore::sharedInstance().GlowColor(), render.glowColor.x, render.glowColor.y, render.glowColor.z);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().HasGlow(), FALSE);
		}

		glUniform1i(GelsenizeCore::sharedInstance().ReflectsSkybox(), render.reflectsSkybox);

		if (render.useTextureAsDiffuse)
		{
			GelTextureManager::sharedInstance().BindTexture(mesh.textureName_0, 0);
			glUniform1i(GelsenizeCore::sharedInstance().UseTextureAsDiffuseID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseTextureAsDiffuseID(render.shaderProgramName), FALSE);
		}

		if (render.discardTextureBlack)
		{
			glUniform1i(GelsenizeCore::sharedInstance().DiscardTextureBlackID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().DiscardTextureBlackID(render.shaderProgramName), FALSE);
		}

		//mvp = p * v * m;

		glUniform1i(GelsenizeCore::sharedInstance().IsImposterBlackIsAlphaID(render.shaderProgramName), FALSE);

		if (1)
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseLightingID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseLightingID(render.shaderProgramName), TRUE);
		}

		glUniform1i(GelsenizeCore::sharedInstance().BackFaceDrawPassMode(), 4);
		
		glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetModelLocation(render.shaderProgramName), 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(render.transform));
		glUniform4fv(GelsenizeCore::sharedInstance().ModelColorID(render.shaderProgramName), 1, glm::value_ptr(render.color));
		glUniform1i(GelsenizeCore::sharedInstance().isWireframeID(render.shaderProgramName), render.isWireframe);
		//glUniform1i(GelsenizeCore::sharedInstance().UseGlobalAlphaValue(), this->useGlobalAlpha);
		//glUniform1f(GelsenizeCore::sharedInstance().GlobalAlphaValue(), this->globalAlpha);

		glUniform1i(GelsenizeCore::sharedInstance().UsedTexNumID(render.shaderProgramName), mesh.usedTextureNum);

		//// "turns on" transparency
		//glEnable(GL_BLEND);		// Enables "blending"
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1f(GelsenizeCore::sharedInstance().ObjectAlphaID(render.shaderProgramName), render.alpha);

		//added
		if (render.useDebugColorsNoLighting)
		{
			glUniform1i(GelsenizeCore::sharedInstance().DebugColorID(render.shaderProgramName), TRUE); // 1
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().DebugColorID(render.shaderProgramName), FALSE);	// 0
		}

		glBindVertexArray(VAO_ID);
		glDrawElements(GL_TRIANGLES,
			numberOfIndices,
			GL_UNSIGNED_INT,	// Each index size 
			(GLvoid*)0);		// Starting point in buffer
		glBindVertexArray(0);
	}
}

void gelRenderManager::DrawGameObject(iGameObject* gameObject)
{

	//glm::mat4x4 mvp(1.0f);		// Model, View, Projection matrix
	glm::mat4x4 m = glm::mat4x4(1.0f);
	glm::mat4x4 v; gelCameraManager::sharedInstance().mainCamera->GetViewMatrix(v);
	glm::mat4x4 p; gelCameraManager::sharedInstance().mainCamera->GetProjectionMatrix(p);

	iGameObject* pCurGO = gameObject;
	RenderInfo render = pCurGO->GetRender();
	MeshInfo mesh = pCurGO->GetMesh();

	std::string meshModelName = mesh.name;

	if (mesh.name == "square_XY")
	{
		int testpoint = 0;
	}

	GLuint VAO_ID = 0;
	int numberOfIndices = 0;
	float unitScale = 1.0f;
	bool draw = render.isDrawn;
	if (!gelMeshManager::sharedInstance().LookUpMeshInfo(meshModelName, VAO_ID, numberOfIndices, unitScale))
	{
		//could not find the mesh info
		draw = false;
	}

	m = glm::translate(m, glm::vec3(render.position.x, render.position.y, render.position.z));

	//m = glm::rotate(m, render.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	//m = glm::rotate(m, render.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	//m = glm::rotate(m, render.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	glm::mat4 matRotation(render.orientation);
	m = m * matRotation;
 
	float actualScale = render.scale * unitScale;

	if (mesh.name == "cube"    ||
		mesh.name == "capsule" ||
		mesh.name == "cylinder")
	{
		//get half width and scale
		glm::vec3 specialScale;
		gameObject->GetPhysics().collisionBody->GetSpecialScale(specialScale);

		float actualScale = render.scale * unitScale;
		m = glm::scale(m, glm::vec3(specialScale.x, specialScale.y, specialScale.z));
	}
	else
	{
		
		m = glm::scale(m, glm::vec3(actualScale, actualScale, actualScale));
	}
	
	render.transform = m;
	render.lastDrawnActualScale = actualScale;
	pCurGO->SetRender(render);

	//draw aabb for current game object
	if (render.isAABBDrawn)
	{
		DrawAABB(
			glm::vec3(render.position.x, render.position.y, render.position.z),
			render.color,
			render.AABBwidth); 
	}

	if (draw)
	{
		if (render.isWireframe)
		{	// Turn off backface culling
			// Enable "wireframe" polygon mode
			glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
				GL_LINE);			// GL_POINT, GL_LINE, or GL_FILL
			glDisable(GL_CULL_FACE);
		}
		else
		{	// "Regular" rendering: 
			// Turn on backface culling
			// Turn polygon mode to solid (Fill)
			glCullFace(GL_BACK);		// GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
				GL_FILL);			// GL_POINT, GL_LINE, or GL_FILL
		}

		int error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			//std::cout << error << std::endl;
		}
		gelShaderManager::sharedInstance().useShaderProgram(render.shaderProgramName);
		while ((error = glGetError()) != GL_NO_ERROR) {
			//std::cout << error << std::endl;
		}

		textureTimeAcc += 0.001f;
		if (textureTimeAcc > 12 * 3.1419f)
		{
			textureTimeAcc -= 12 * 3.1419f;
		}
		glUniform4fv(GelsenizeCore::sharedInstance().TextureExtraID(), 1, glm::value_ptr(glm::vec4(0.0f, 1.0f, textureTimeAcc, 1.01f)));

		gelCameraManager::sharedInstance().mainCamera->SetUniforms(render.shaderProgramName);

		if (render.useTextureAsDiffuse)
		{
			GelTextureManager::sharedInstance().BindTexture(mesh.textureName_0, 0);
			glUniform1i(GelsenizeCore::sharedInstance().UseTextureAsDiffuseID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseTextureAsDiffuseID(render.shaderProgramName), FALSE);
		}

		if (render.discardTextureBlack)
		{
			glUniform1i(GelsenizeCore::sharedInstance().DiscardTextureBlackID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().DiscardTextureBlackID(render.shaderProgramName), FALSE);
		}

		//mvp = p * v * m;
		
		glUniform1i(GelsenizeCore::sharedInstance().IsImposterBlackIsAlphaID(render.shaderProgramName), FALSE);

		if (1)
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseLightingID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseLightingID(render.shaderProgramName), TRUE);
		}

		glUniform1i(GelsenizeCore::sharedInstance().BackFaceDrawPassMode(), 4);
		
		glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetModelLocation(render.shaderProgramName), 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(m));
		glUniform4fv(GelsenizeCore::sharedInstance().ModelColorID(render.shaderProgramName), 1, glm::value_ptr(render.color));
		glUniform1i(GelsenizeCore::sharedInstance().isWireframeID(render.shaderProgramName), render.isWireframe);
		//glUniform1i(GelsenizeCore::sharedInstance().UseGlobalAlphaValue(), this->useGlobalAlpha);
		//glUniform1f(GelsenizeCore::sharedInstance().GlobalAlphaValue(), this->globalAlpha);

		glUniform1i(GelsenizeCore::sharedInstance().UsedTexNumID(render.shaderProgramName), mesh.usedTextureNum);

		// "turns on" transparency
		//glEnable(GL_BLEND);		// Enables "blending"
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glUniform1f(GelsenizeCore::sharedInstance().ObjectAlphaID(render.shaderProgramName), render.alpha);

		//added
		if (render.useDebugColorsNoLighting)
		{
			glUniform1i(GelsenizeCore::sharedInstance().DebugColorID(render.shaderProgramName), TRUE); // 1
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().DebugColorID(render.shaderProgramName), FALSE);	// 0
		}

		glBindVertexArray(VAO_ID);
		glDrawElements(GL_TRIANGLES,
			numberOfIndices,
			GL_UNSIGNED_INT,	// Each index size 
			(GLvoid*)0);		// Starting point in buffer
		glBindVertexArray(0);

		//return;

		//finally if object collision debug mode allow draw spme spheres,
		//CollisionInfo collisionInfo = pCurGO->GetCollision();
		//if (collisionInfo.debug_mode != CollisionInfo::DEBUG_MODE::NO_DRAW)
		//{
		//	//DRAW 
		//	if (collisionInfo.type == CollisionInfo::TYPE::SPHERE_ARRAY)
		//	{
		//		//try to access sphere array
		//		std::vector<CollisionInfo::CollisionSphere> sphereArray;
		//		if (GelCollisionManager::sharedInstance().GetSphereArrayForAlias(mesh.name, sphereArray))
		//		{
		//			for (int index = 0; index != sphereArray.size(); index++)
		//			{
		//				CollisionInfo::CollisionSphere sphere = sphereArray[index];
		//				glm::vec3 center_offset = sphere.center_offset;
		//				glm::vec3 objPosition = render.position;

		//				MeshInfo sphereMesh;
		//				sphereMesh.name = "sphere";
		//				RenderInfo sphereRender;

		//				//attempt to move sphere to its location
		//				m = glm::translate(render.transform, glm::vec3(center_offset.x, center_offset.y, center_offset.z));
		//				
		//				sphereRender.position = m[3];
		//				sphereRender.scale = actualScale * sphere.radius;
		//				sphereRender.isWireframe = true;
		//				sphereRender.isAABBDrawn = false;
		//				sphereRender.AABBwidth = actualScale * 10.0f;
		//				sphereRender.color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		//				sphereRender.useDebugColorsNoLighting = true;

		//				this->DrawWithInfos(sphereRender, sphereMesh, collisionInfo);
		//			}
		//			
		//		}
		//	}
		//}
	}
	return;
}

void gelRenderManager::DrawGameObjectCelShaded(iGameObject* gameObject)
{
	if (gameObject->GetObjectName() == "mazeObject")
	{
		if (gameObject->GetRender().position.y <= -90)
		{
			return; //dont draw;
		}
	}

	//new struct for multimesh objects
	struct sVAODrawInfo
	{
		sVAODrawInfo() : VAO_ID(0), numberOfIndices(0), unitScale(1.0f) {};
		sVAODrawInfo(GLuint VAO_ID_, int numberOfIndices_, float unitScale_)
		{
			this->VAO_ID = VAO_ID_;
			this->numberOfIndices = numberOfIndices_;
			this->unitScale = unitScale_;
		}
		GLuint VAO_ID;
		int numberOfIndices;
		float unitScale;
	};

	// Object may have more than one mesh, 
	//	this vector stores all the VAO info for drawing...
	std::vector< sVAODrawInfo > vecVAOsToDraw;

	//glm::mat4x4 mvp(1.0f);		// Model, View, Projection matrix
	glm::mat4x4 m = glm::mat4x4(1.0f);
	glm::mat4x4 v; gelCameraManager::sharedInstance().mainCamera->GetViewMatrix(v);
	glm::mat4x4 p; gelCameraManager::sharedInstance().mainCamera->GetProjectionMatrix(p);

	iGameObject* pCurGO = gameObject;
	RenderInfo render = pCurGO->GetRender();
	MeshInfo mesh = pCurGO->GetMesh();

	float unitScale = 1.0f;
	bool draw = render.isDrawn;

	glUniform1i(GelsenizeCore::sharedInstance().IsASkinnedMesh(), FALSE);

	if (mesh.isASkinnedMesh)
	{
		
		GLuint programID = gelShaderManager::sharedInstance().getIDWithName(render.shaderProgramName);
		cSimpleAssimpSkinnedMesh *skinnedMesh = gelMeshManager::sharedInstance().GetSkinnedMesh(mesh.animationController->GetSkinnedMeshName());

		if (skinnedMesh == NULL)
		{
			return;
			//cannot be drawn yet
		}

		if (!skinnedMesh->m_bVAO_created)
		{
			std::string error;
			if (skinnedMesh->CreateVBOandVOAfromCurrentMesh(programID, error))
			{
				std::cout << "Error: Couldn't create VAO for skinned mesh..." << std::endl;
				std::cout << "\t" << error << std::endl;
				// 
				return;		// NO VAO, so can't draw anything
			}
		}

		sVAODrawInfo SM_VAO;
		SM_VAO.VAO_ID = skinnedMesh->m_VAO_ID;
		SM_VAO.numberOfIndices = skinnedMesh->m_numberOfIndices;
		SM_VAO.unitScale = 1.0f;

		vecVAOsToDraw.push_back(SM_VAO);

		glUniform1i(GelsenizeCore::sharedInstance().IsASkinnedMesh(), TRUE);

		float duration = skinnedMesh->GetDuration();

		//
		//mesh.animationClip->Tick();

		//int multiplier = 4;
		//if (mesh.animationTime >= duration)
		//{
		//	//mesh.animationTime = (float)((int)(duration *100.0f) % multiplier) / 100.0f;
		//	mesh.animationTime = 0.0f;
		//}
		//std::cout << mesh.animationTime << std::endl;

		/*const aiNodeAnim* node = skinnedMesh->FindNodeAnimationChannel(skinnedMesh->m_Animation, aiString( "B_Pelvis" ));
		glm::vec3 outPos;
		skinnedMesh->CalcGLMInterpolatedPosition(mesh.animationTime, node, outPos);*/

		// Set up the animation pose:
		std::vector< glm::mat4x4 > vecTransforms;
		std::vector< glm::mat4x4 > vecGlobals;
		std::vector< glm::mat4x4 > vecOffsets;
		std::vector< std::string > vecNames;
		
		//mesh.animationClip->BoneTransform(vecTransforms, vecGlobals, vecOffsets);
		mesh.animationController->BoneTransform(vecTransforms, vecGlobals, vecOffsets, vecNames);
		
		unsigned int numberOfBonesUsed = static_cast< unsigned int >(vecTransforms.size());

		glUniform1i(GelsenizeCore::sharedInstance().NumberOfBonesUsed(), numberOfBonesUsed);

		//moved to bottom

		for (int i = 0; i != numberOfBonesUsed; i++)
		{
			glm::mat4x4 pBoneMatrixArray = vecTransforms[i];

			//pBoneMatrixArray = glm::translate(pBoneMatrixArray, targetForwards3 * -5.0f);
			
			glUniformMatrix4fv(GelsenizeCore::sharedInstance().BoneID(i, "default"), 1, GL_FALSE,
				(const GLfloat*)glm::value_ptr(pBoneMatrixArray));
		}

		// Draw all the bones
		//for (unsigned int boneIndex = 0; boneIndex != numberOfBonesUsed; boneIndex++)
		//{
		//	glm::mat4 boneLocal = vecGlobals[boneIndex];

		//	float scale = 0.05f;
		//	boneLocal = glm::scale(boneLocal, glm::vec3(scale, scale, scale));

		//	glm::vec3 playerPosition = render.position;
		//	glm::vec4 boneBallLocation = boneLocal * glm::vec4(playerPosition.x, playerPosition.y, playerPosition.z, 1.0f);
		//	//boneBallLocation *= scale;
		//	//boneBallLocation += glm::vec4(10.0f, -5.0f, 0.0f, 0.0f);

		//	MeshInfo mesh;
		//	CollisionInfo collisionInfo;
		//	mesh.name = "sphere";
		//	RenderInfo render;

		//	render.position = glm::vec3(boneBallLocation);
		//	render.scale = 0.8f;
		//	render.isWireframe = false;
		//	//render.isAABBDrawn = true;
		//	//render.AABBwidth = 10.0f;
		//	render.color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		//	render.useDebugColorsNoLighting = true;

		//	this->DrawWithInfos(render, mesh, collisionInfo);
		//}
	
		//i remove 4.0f to place the character correctly
		float playerOffset = 1.87f;
		m = glm::translate(m, glm::vec3(render.position.x, render.position.y - playerOffset, render.position.z));
		glm::mat4 matRotation(render.orientation);
		m = m * matRotation;

		float actualScale = 1.0f * unitScale * 0.038f * 0.4f;
		m = glm::scale(m, glm::vec3(actualScale, actualScale, actualScale));

		render.transform = m;
		render.lastDrawnActualScale = actualScale;
	}
	else
	{
		//old draw code - not a skinned mesh

		std::string meshModelName = mesh.name;

		GLuint VAO_ID = 0;
		int numberOfIndices = 0;
		
		
		if (!gelMeshManager::sharedInstance().LookUpMeshInfo(meshModelName, VAO_ID, numberOfIndices, unitScale))
		{
			//could not find the mesh info
			draw = false;
		}


		m = glm::translate(m, glm::vec3(render.position.x, render.position.y, render.position.z));
		glm::mat4 matRotation(render.orientation);
		m = m * matRotation;

		float actualScale = render.scale * unitScale;

		if (mesh.name == "cube" ||
			mesh.name == "capsule" ||
			mesh.name == "cylinder")
		{
			//get half width and scale
			glm::vec3 specialScale;
			gameObject->GetPhysics().collisionBody->GetSpecialScale(specialScale);

			specialScale *= unitScale;
			m = glm::scale(m, glm::vec3(specialScale.x, specialScale.y, specialScale.z));
		}
		else
		{

			m = glm::scale(m, glm::vec3(actualScale, actualScale, actualScale));
		}

		render.transform = m;
		render.lastDrawnActualScale = actualScale;

		sVAODrawInfo curVAOInfo;
		curVAOInfo.unitScale = unitScale;
		curVAOInfo.numberOfIndices = numberOfIndices;
		curVAOInfo.VAO_ID = VAO_ID;
		vecVAOsToDraw.push_back(curVAOInfo);
	}

	//make sure transform matrix is set by this point
	pCurGO->SetRender(render);
	pCurGO->SetMesh(mesh);

	if (draw)
	{
		//if (render.isWireframe)
		//{	// Turn off backface culling
		//	// Enable "wireframe" polygon mode
		//	glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
		//		GL_LINE);			// GL_POINT, GL_LINE, or GL_FILL
		//	glDisable(GL_CULL_FACE);
		//}
		//else
		//{	// "Regular" rendering: 
		//	// Turn on backface culling
		//	// Turn polygon mode to solid (Fill)
		//	glCullFace(GL_BACK);		// GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
		//	glEnable(GL_CULL_FACE);
		//	glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
		//		GL_FILL);			// GL_POINT, GL_LINE, or GL_FILL
		//}

		int error;
		while ((error = glGetError()) != GL_NO_ERROR) {
			//std::cout << error << std::endl;
		}
		gelShaderManager::sharedInstance().useShaderProgram(render.shaderProgramName);
		while ((error = glGetError()) != GL_NO_ERROR) {
			//std::cout << error << std::endl;
		}

		if (render.exploding)
		{
			glUniform1f(GelsenizeCore::sharedInstance().ExplodeTime(), render.explodeTime);
		}
		else
		{
			glUniform1f(GelsenizeCore::sharedInstance().ExplodeTime(), 0.0f);
		}

		if (render.hasGlow)
		{
			glUniform1i(GelsenizeCore::sharedInstance().HasGlow(), TRUE);
			glUniform3f(GelsenizeCore::sharedInstance().GlowColor(), render.glowColor.x, render.glowColor.y, render.glowColor.z);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().HasGlow(), FALSE);
		}

		glUniform1i(GelsenizeCore::sharedInstance().ReflectsSkybox(), render.reflectsSkybox);

		gelCameraManager::sharedInstance().mainCamera->SetUniforms(render.shaderProgramName);

		if (render.useTextureAsDiffuse)
		{
			GelTextureManager::sharedInstance().BindTexture(mesh.textureName_0, 0);
			glUniform1i(GelsenizeCore::sharedInstance().UseTextureAsDiffuseID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseTextureAsDiffuseID(render.shaderProgramName), FALSE);
		}

		if (render.discardTextureBlack)
		{
			glUniform1i(GelsenizeCore::sharedInstance().DiscardTextureBlackID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().DiscardTextureBlackID(render.shaderProgramName), FALSE);
		}

		//mvp = p * v * m;

		glUniform1i(GelsenizeCore::sharedInstance().IsImposterBlackIsAlphaID(render.shaderProgramName), FALSE);

		if (1)
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseLightingID(render.shaderProgramName), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseLightingID(render.shaderProgramName), TRUE);
		}

		glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetModelLocation(render.shaderProgramName), 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(m));
		glUniform4fv(GelsenizeCore::sharedInstance().ModelColorID(render.shaderProgramName), 1, glm::value_ptr(render.color));
		glUniform1i(GelsenizeCore::sharedInstance().isWireframeID(render.shaderProgramName), render.isWireframe);
		//glUniform1i(GelsenizeCore::sharedInstance().UseGlobalAlphaValue(), this->useGlobalAlpha);
		//glUniform1f(GelsenizeCore::sharedInstance().GlobalAlphaValue(), this->globalAlpha);

		glUniform1i(GelsenizeCore::sharedInstance().UsedTexNumID(render.shaderProgramName), mesh.usedTextureNum);

		
		//added
		if (render.useDebugColorsNoLighting)
		{
			glUniform1i(GelsenizeCore::sharedInstance().DebugColorID(render.shaderProgramName), TRUE); // 1
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().DebugColorID(render.shaderProgramName), FALSE);	// 0
		}


		unsigned int numVAOsToDraw = static_cast<unsigned int>(vecVAOsToDraw.size());
		for (unsigned int VAOIndex = 0; VAOIndex != numVAOsToDraw; VAOIndex++)
		{
			sVAODrawInfo* pCurVAOInfo = &(vecVAOsToDraw[VAOIndex]);

			//::g_NumberOfIndicesRendered += pCurVAOInfo->numberOfIndices;

			//draw for cel shading
			glBindVertexArray(pCurVAOInfo->VAO_ID);

			//first draw 
			//   (back face drawing with a larger "pseudo scale", this will make up our siluette)

			//glDisable(GL_BLEND); // Disables "blending"

			//if (!mesh.isASkinnedMesh)
			//{
			//	// RENDER BLACK ENLARGED MESH
			//	glEnable(GL_CULL_FACE); // enable culling
			//	glCullFace(GL_FRONT); // enable culling of front faces
			//	glDepthMask(GL_TRUE); // enable writes to Z-buffer

			//	glm::vec3 edgeColor;
			//	edgeColor = glm::vec3(0.0f, 0.0f, 0.0f);
			//	//edgeColor = glm::vec3(OCHelper::getRandFloat(0.0f, 1.0f), OCHelper::getRandFloat(0.0f, 1.0f), OCHelper::getRandFloat(0.0f, 1.0f));

			//	glUniform3f(GelsenizeCore::sharedInstance().BackFaceColor(), edgeColor.x, edgeColor.y, edgeColor.z); // black colour
			//	glUniform1f(GelsenizeCore::sharedInstance().BackFaceOffset(), +0.08f); // offset along normal
			//	glUniform1i(GelsenizeCore::sharedInstance().BackFaceDrawPassMode(), 1); // phase 1

			//																			//black countour
			//	glDrawElements(GL_TRIANGLES, pCurVAOInfo->numberOfIndices, GL_UNSIGNED_INT, (GLvoid*)0);
			//}

			// "turns on" transparency
			//glEnable(GL_BLEND);		// Enables "blending"
			//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glUniform1f(GelsenizeCore::sharedInstance().ObjectAlphaID(render.shaderProgramName), render.alpha);

			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);
			glEnable(GL_CULL_FACE);

			if (directionalShadowPass)
			{
				glCullFace(GL_FRONT); // enable culling of back faces
			}
			else
			{
				glCullFace(GL_BACK); // enable culling of back faces
			}

			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			glUniform1i(GelsenizeCore::sharedInstance().BackFaceDrawPassMode(), 3); // phase 3

			glDrawElements(GL_TRIANGLES, pCurVAOInfo->numberOfIndices, GL_UNSIGNED_INT, (GLvoid*)0);

			glUniform1i(GelsenizeCore::sharedInstance().BackFaceDrawPassMode(), 4); // switch back to regular draw mode

			glBindVertexArray(0);

			//glBindVertexArray(pCurVAOInfo->VAO_ID);
			//glDrawElements(GL_TRIANGLES,
			//	pCurVAOInfo->numberOfIndices,
			//	GL_UNSIGNED_INT,	// Each index is how big?? 
			//	(GLvoid*)0);		// Starting point in buffer
			//glBindVertexArray(0);
		}


		/*  OLD DRAW SINGLE VAO */

		////draw for cel shading
		//glBindVertexArray(VAO_ID);

		//// "turns on" transparency
		////first draw 
		////   (back face drawing with a larger "pseudo scale", this will make up our siluette)

		//// RENDER BLACK ENLARGED MESH
		//glEnable(GL_CULL_FACE); // enable culling
		//glCullFace(GL_FRONT); // enable culling of front faces
		//glDepthMask(GL_TRUE); // enable writes to Z-buffer

		//glm::vec3 edgeColor;
		///*if (accFlag_1)
		//{
		//	edgeColor = glm::vec3(1.0f, 0.0f, 0.0f);
		//}
		//else
		//{ 
		//	edgeColor = glm::vec3(0.0f, 0.0f, 1.0f);
		//}*/
		//edgeColor = glm::vec3(0.0f, 0.0f, 0.0f);
		////edgeColor = glm::vec3(OCHelper::getRandFloat(0.0f, 1.0f), OCHelper::getRandFloat(0.0f, 1.0f), OCHelper::getRandFloat(0.0f, 1.0f));
		//
		//glUniform3f(GelsenizeCore::sharedInstance().BackFaceColor(), edgeColor.x, edgeColor.y, edgeColor.z); // black colour
		//glUniform1f(GelsenizeCore::sharedInstance().BackFaceOffset(), 0.09f); // offset along normal
		//glUniform1i(GelsenizeCore::sharedInstance().BackFaceDrawPassMode(), 1); // phase 1

		//glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, (GLvoid*)0);
		//
		//// "turns on" transparency
		//glEnable(GL_BLEND);		// Enables "blending"
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//glUniform1f(GelsenizeCore::sharedInstance().ObjectAlphaID(render.shaderProgramName), render.alpha);

		//glEnable(GL_DEPTH_TEST);
		//glDepthMask(GL_TRUE);
		//glEnable(GL_CULL_FACE);	
		//glCullFace(GL_BACK); // enable culling of back faces
		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//
		//glUniform1i(GelsenizeCore::sharedInstance().BackFaceDrawPassMode(), 3); // phase 3

		//glDrawElements(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, (GLvoid*)0);

		//glUniform1i(GelsenizeCore::sharedInstance().BackFaceDrawPassMode(), 4); // switch back to regular draw mode

		//glBindVertexArray(0);
	}
	return;
}


void gelRenderManager::DrawParticle(cParticle particle)
{
	//glm::mat4x4 mvp(1.0f);		// Model, View, Projection matrix
	glm::mat4x4 m = glm::mat4x4(1.0f);
	glm::mat4x4 v; gelCameraManager::sharedInstance().mainCamera->GetViewMatrix(v);
	glm::mat4x4 p; gelCameraManager::sharedInstance().mainCamera->GetProjectionMatrix(p);

	cParticle pCurrentParticle = particle;
	//RenderInfo render = pCurGO.GetRender();
	std::string meshModelName = pCurrentParticle.meshName;

	GLuint VAO_ID = 0;
	int numberOfIndices = 0;
	float unitScale = 10.0f;
	bool draw = true;
	bool isWireframe = false; 
	if (!gelMeshManager::sharedInstance().LookUpMeshInfo(meshModelName, VAO_ID, numberOfIndices, unitScale))
	{
		//could not find the mesh info
		draw = false;
	}

	m = glm::translate(m, glm::vec3(pCurrentParticle.position.x, pCurrentParticle.position.y, pCurrentParticle.position.z));

	m = glm::rotate(m, pCurrentParticle.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	m = glm::rotate(m, pCurrentParticle.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	m = glm::rotate(m, pCurrentParticle.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	//glm::mat4 matRotation(pCurrentParticle.orientation);
	//m = m * matRotation;

	float actualScale = pCurrentParticle.scale * unitScale;
	m = glm::scale(m, glm::vec3(actualScale, actualScale, actualScale));
	//render.transform = m;
	//pCurGO.SetRender(render);

	//draw aabb for current game object
	if (0) //dont draw for particles
	{
		DrawAABB(
			glm::vec3(pCurrentParticle.position.x, pCurrentParticle.position.y, pCurrentParticle.position.z),
			pCurrentParticle.colour,
			10.0f);
	}

	if (draw) //draw
	{
		if (isWireframe)
		{	// Turn off backface culling
			// Enable "wireframe" polygon mode
			glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
				GL_LINE);			// GL_POINT, GL_LINE, or GL_FILL
			glDisable(GL_CULL_FACE);
		}
		else
		{	// "Regular" rendering: 
			// Turn on backface culling
			// Turn polygon mode to solid (Fill)
			glCullFace(GL_BACK);		// GL_FRONT, GL_BACK, or GL_FRONT_AND_BACK
			glEnable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,	// GL_FRONT_AND_BACK is the only thing you can pass here
				GL_FILL);			// GL_POINT, GL_LINE, or GL_FILL
		}
		//mvp = p * v * m;

		gelShaderManager::sharedInstance().useShaderProgram("default");

		glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetModelLocation(), 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(m));
		glUniform4fv(GelsenizeCore::sharedInstance().ModelColorID(), 1, glm::value_ptr(pCurrentParticle.colour));
		glUniform1i(GelsenizeCore::sharedInstance().isWireframeID(), isWireframe);
		//glUniform1i(GelsenizeCore::sharedInstance().UseGlobalAlphaValue(), this->useGlobalAlpha);
		//glUniform1f(GelsenizeCore::sharedInstance().GlobalAlphaValue(), this->globalAlpha);

		glUniform1i(GelsenizeCore::sharedInstance().UsedTexNumID(), 1);

		//added
		if (false)//render.useDebugColorsNoLighting
		{
			glUniform1i(GelsenizeCore::sharedInstance().DebugColorID(), TRUE); // 1
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().DebugColorID(), FALSE);	// 0
		}

		glUniform1i(GelsenizeCore::sharedInstance().IsImposterBlackIsAlphaID(), FALSE);
		
		GelTextureManager::sharedInstance().BindTexture(pCurrentParticle.textureName_0, 0);
		glUniform1i(GelsenizeCore::sharedInstance().UseTextureAsDiffuseID(), TRUE);

		if (1)
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseLightingID(), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().UseLightingID(), TRUE);
		}

		if (particle.discardTextureBlack)
		{
			glUniform1i(GelsenizeCore::sharedInstance().DiscardTextureBlackID(), TRUE);
		}
		else
		{
			glUniform1i(GelsenizeCore::sharedInstance().DiscardTextureBlackID(), FALSE);
		}

		glBindVertexArray(VAO_ID);
		glDrawElements(GL_TRIANGLES,
			numberOfIndices,
			GL_UNSIGNED_INT,	// Each index size 
			(GLvoid*)0);		// Starting point in buffer
		glBindVertexArray(0);
	}
	return;
}

void gelRenderManager::DrawSkyboxObject(void)
{
	// Draw skybox
	// Place the skybox at the same location as the camera 'eye'

	glm::vec4 cameraPosition;
	gelCameraManager::sharedInstance().mainCamera->GetEyePosition(cameraPosition);

	iGameObject* skyboxObject = gelObjectManager::sharedInstance().GetSkyboxObject();
	RenderInfo render = skyboxObject->GetRender();
	render.position.x = cameraPosition.x;
	render.position.y = cameraPosition.y;
	render.position.z = cameraPosition.z;
	skyboxObject->SetRender(render);

	//::g_pSkyBoxObject->x = ::g_CameraX;
	//::g_pSkyBoxObject->y = ::g_CameraY;
	//::g_pSkyBoxObject->z = ::g_CameraZ;
	// There are a number of other things you could do with this, for example:
	// Turn off the depth buffer, and the writing to the depth buffer, 
	//  then draw the sky box FIRST. Then turn on the depth buffer and 
	//  continue to draw the rest of the scene. 
	// This case is more straight-forward, in that we simply make the sky box
	//  an appropriate size for the sky (so big) and be careful not to 
	//  draw anything "behind" it.

	glUniform1i(GelsenizeCore::sharedInstance().IsCubeMapObjectID(), TRUE);

	// Bind the appriprate texture unit... 
	GLuint texNumberSky = GelTextureManager::sharedInstance().GetSkyboxID();//GelTextureManager::sharedInstance().GetTextureIDFromName("skybox");
	// We'll pick a higher value texture unit. 
	// There are at least 80 of them, so we'll pick the last one...
	// (Note the GL_TEXTUREX values only go up to 31, though)
	//const unsigned int TEXTURE_UNIT_90_MAGIC_NUMBER = 90;
	glActiveTexture(GL_TEXTURE0 + texNumberSky);		// GL_TEXTURE0 = 33984
																		//	glBindTexture( GL_TE, texNumberSky );
	glBindTexture(GL_TEXTURE_CUBE_MAP, texNumberSky);
	glUniform1i(GelsenizeCore::sharedInstance().CubemapSkyBoxSamplerID(), texNumberSky);

	glDisable(GL_DEPTH);			// turn off depth test (i.e. just write)
	glDepthMask(GL_FALSE);		// no writing to z or depth buffer
	DrawGameObject(skyboxObject);
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH);

	glUniform1i(GelsenizeCore::sharedInstance().IsCubeMapObjectID(), FALSE);

	return;
}

int gelRenderManager::getAABBID(void)
{
	return this->aabbID;
}

void gelRenderManager::DrawAABB(glm::vec3 position, glm::vec4 colour, float scale)
{
	cAABB pTestAABB;

	pTestAABB.boxWidth = scale;
	pTestAABB.minXYZ.x = floor(position.x / pTestAABB.boxWidth);
	pTestAABB.minXYZ.y = floor(position.y / pTestAABB.boxWidth);
	pTestAABB.minXYZ.z = floor(position.z / pTestAABB.boxWidth);
	pTestAABB.UpdateMaxFromMinAndWidth();

	//keep last drawn id 
	this->aabbID = pTestAABB.CalculateID(pTestAABB.minXYZ);

	// And even more drawing...
	iGameObject* aabbObject = this->aabb;

	RenderInfo aabbRender;
	//set values
	aabbRender.position.x = pTestAABB.minXYZ.x * pTestAABB.boxWidth;
	aabbRender.position.y = pTestAABB.minXYZ.y * pTestAABB.boxWidth;
	aabbRender.position.z = pTestAABB.minXYZ.z * pTestAABB.boxWidth;

	aabbRender.isWireframe = true;
	aabbRender.isDrawn = true;

	aabbRender.color = glm::vec4(colour.r, colour.g, colour.b, 1.0f);
	aabbRender.useDebugColorsNoLighting = true;

	aabbRender.scale = scale;

	aabbObject->SetRender(aabbRender);
	//draw
	this->DrawGameObject(aabbObject);
	
	return;
}


//skinnedMesh->BoneTransform(mesh.animationTime, vecTransforms, vecGlobals, vecOffsets);
//mesh.animationTime += 0.01f * multiplier;

//glm::vec3 outPosNext;
//skinnedMesh->CalcGLMInterpolatedPosition(mesh.animationTime, node, outPosNext);

/*glm::mat4x4* pBoneMatrixArray = &(vecTransforms[0]);
glUniformMatrix4fv(GelsenizeCore::sharedInstance().BoneID(0, "default"), numberOfBonesUsed, GL_FALSE,
(const GLfloat*)glm::value_ptr(*pBoneMatrixArray));*/

//glm::vec4 forwards(0.0f, 0.0f, +1.0f, 0.0f);
//glm::vec4 targetForwards = gameObject->GetRender().transform * forwards;
//glm::vec3 targetForwards3(targetForwards.x, targetForwards.y, targetForwards.z);
//targetForwards3 = glm::normalize(targetForwards3);

//glm::vec4 sexyBack = glm::normalize(glm::vec4(render.transform[2].x, render.transform[2].y, render.transform[2].z, 0.0f));
//glm::vec3 sexyBack3(sexyBack.x, sexyBack.y, sexyBack.z);
//sexyBack3 = glm::normalize(sexyBack3);

//std::vector< glm::mat4x4 > tempvecTransforms;
//std::vector< glm::mat4x4 > tempvecGlobals;
//std::vector< glm::mat4x4 > tempvecOffsets;
//skinnedMesh->BoneTransform(0.0f, tempvecTransforms, tempvecGlobals, tempvecOffsets);

//glm::vec3 dif = vecTransforms[0][3] - tempvecTransforms[0][3];
//glm::vec3 dif = outPosNext - outPos;
//dif *= 0.0f;
//glUniform3f(GelsenizeCore::sharedInstance().SlideDistance(), dif.x, dif.y, dif.z);