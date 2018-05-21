#include "cPlayer.h"

#include "externals.h"
#include "managers.h"
#include "GelsenizeCore.h"

//Update
void cPlayer::Update(float deltaTime)
{
	return;
	//Player Controls
	bool pressW = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_W) == GLFW_PRESS;
	bool pressS = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_S) == GLFW_PRESS;

	bool pressA = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_A) == GLFW_PRESS;
	bool pressD = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_D) == GLFW_PRESS;

	bool pressE = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_E) == GLFW_PRESS;
	bool pressQ = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_Q) == GLFW_PRESS;

	bool pressLeft = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS;
	bool pressRight = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS;

	bool pressLeftShift = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	//mOffset = glm::vec3(0.f, 0.5f, -1.f);
	if (pressE != pressQ)
	{
		float goUpward = pressE ? 18.0f : -18.0f;
		RenderInfo playerRender = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender();
		PhysicsInfo	playerPhysics = gelObjectManager::sharedInstance().GetMainPlayer()->GetPhysics();
		
		glm::vec4 unnormalizedPlayerDirection;// = glm::vec4(playerRender.transform[3].x, playerRender.transform[3].y, playerRender.transform[3].z, 0.0f);
		glm::vec4 forward(0.0f, goUpward, 0.0f, 0.0f);
		unnormalizedPlayerDirection = playerRender.transform * forward;
		unnormalizedPlayerDirection = glm::normalize(unnormalizedPlayerDirection);
		//playerPhysics.velocity.x += unnormalizedPlayerDirection.x * deltaTime;
		//playerPhysics.velocity.z += unnormalizedPlayerDirection.z * deltaTime;

		playerRender.position.x += unnormalizedPlayerDirection.x * deltaTime * 0.8f;
		playerRender.position.y += unnormalizedPlayerDirection.y * deltaTime * 0.8f;
		playerRender.position.z += unnormalizedPlayerDirection.z * deltaTime * 0.8f;

		glm::vec4 tempNormal = glm::normalize(unnormalizedPlayerDirection);
		glm::vec3 normalPlayerDirection(tempNormal.x, tempNormal.y, tempNormal.z);

		if (playerPhysics.collisionBody && playerPhysics.collisionBody->GetBodyType() == eBodyType::RIGID_BODY_TYPE)
		{
			/*iRigidBody* body = dynamic_cast<iRigidBody*>(playerPhysics.collisionBody);
			body->ApplyImpulseAtPoint(forward * deltaTime * 2.0f, playerRender.position);
			glm::vec3 vel;
			body->GetVelocity(vel);
			playerPhysics.velocity = vel;*/
		}
		

		if (playerRender.position.y - 10.0f < 0.0f)
		{
			//playerRender.position.y = 10.0f;
		}

		this->renderInfo = playerRender;
		this->physicsInfo = playerPhysics;
	}

	if ((pressW != pressS))
	{
		float goForward = pressW ? 1.6f : -1.6f;
		RenderInfo playerRender = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender();
		PhysicsInfo	playerPhysics = gelObjectManager::sharedInstance().GetMainPlayer()->GetPhysics();
		glm::mat4x4 playerTransform = glm::mat4x4(1.0f);

		playerTransform = glm::translate(playerTransform, glm::vec3(playerRender.position.x, playerRender.position.y, playerRender.position.z));
		glm::mat4 matRotation(playerRender.orientation);
		playerTransform = playerTransform * matRotation;
		//playerTransform = glm::rotate(playerTransform, playerRender.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
		//playerTransform = glm::rotate(playerTransform, playerRender.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
		//playerTransform = glm::rotate(playerTransform, playerRender.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
		playerTransform = glm::scale(playerTransform, glm::vec3(playerRender.lastDrawnActualScale, playerRender.lastDrawnActualScale, playerRender.lastDrawnActualScale));

		glm::vec4 unnormalizedPlayerDirection = glm::vec4(playerTransform[3].x, playerTransform[3].y, playerTransform[3].z, 0.0f);
		glm::vec4 forward(0.0f, 0.0f, goForward, 0.0f);
		glm::vec4 forwardPositive(0.0f, -1.0f, abs(goForward), 0.0f);
		unnormalizedPlayerDirection = playerTransform * forward;
		glm::vec4 unnormalizedPlayerDirection_positive = playerTransform * forwardPositive;
		glm::vec4 normalizedPlayerDirection = glm::normalize(unnormalizedPlayerDirection);

		this->lightInfo.direction = glm::normalize(unnormalizedPlayerDirection_positive);

		//float playerSpeedMultiplier = GelsenizeCore::sharedInstance().GetPlayerSpeed();

		//playerPhysics.rigidBody->ApplyImpulseAtPoint(forward * deltaTime * 2.0f, playerRender.position);
		if (playerPhysics.collisionBody && playerPhysics.collisionBody->GetBodyType() == eBodyType::RIGID_BODY_TYPE)
		{
			/*iRigidBody* body = dynamic_cast<iRigidBody*>(playerPhysics.collisionBody);
			body->ApplyImpulseAtPoint(forward * deltaTime * 2.0f, playerRender.position);
			glm::vec3 vel;
			body->GetVelocity(vel);
			playerPhysics.velocity = vel;*/
		}

		//playerPhysics.acceleration.x = normalizedPlayerDirection.x *playerSpeedMultiplier*2;
		//playerPhysics.acceleration.z = normalizedPlayerDirection.z *playerSpeedMultiplier*2;

		//playerPhysics.velocity.x = normalizedPlayerDirection.x *playerSpeedMultiplier;
		//playerPhysics.velocity.z = normalizedPlayerDirection.z *playerSpeedMultiplier;

		//this->meshInfo.mainAnimationName = "CHAR_WALK_SLOW";

		//this->meshInfo.animationClip = this->animation_walk_slow;
		
		float sprintMod = 4.2f;
		AnimationClipInfo animationInfo;
		if(pressLeftShift)
		{
			animationInfo.rate = 1.5f;
			animationInfo.name = "CHAR_RUN";
		}
		else
		{
			animationInfo.rate = 1.5f;
			sprintMod = 1.0f;
			animationInfo.name = "CHAR_WALK_SLOW";
		}

		if (!pressW)
		{
			animationInfo.rate *= -1.0f;
		}

		animationInfo.loopCount = -1;
		this->meshInfo.animationController->BlendIntoAnimation(animationInfo, 0.15f);

		unnormalizedPlayerDirection = glm::normalize(unnormalizedPlayerDirection);
		playerRender.position.x += unnormalizedPlayerDirection.x * deltaTime * 1.8f * sprintMod;
		playerRender.position.y += unnormalizedPlayerDirection.y * deltaTime * 1.8f * sprintMod;
		playerRender.position.z += unnormalizedPlayerDirection.z * deltaTime * 1.8f * sprintMod;

		this->renderInfo = playerRender;
		this->physicsInfo = playerPhysics;
	}
	else //if (!(pressW || pressS))
	{

		AnimationClipInfo animationInfo;
		animationInfo.name = "CHAR_IDLE";
		animationInfo.rate = 1.5f;
		animationInfo.loopCount = -1;
		this->meshInfo.animationController->BlendIntoAnimation(animationInfo, 0.20f);

		//this->meshInfo.mainAnimationName = "CHAR_IDLE";

		//this->meshInfo.animationClip = this->animation_idle;
		
		//this->physicsInfo.velocity.x *= 0.9f;
		//this->physicsInfo.velocity.z *= 0.9f;
		//this->physicsInfo.acceleration.x = 0.0f;
		//this->physicsInfo.acceleration.z = 0.0f;
	}

	if (pressA != pressD)
	//if (pressLeft != pressRight)
	{
		iGameObject *player = gelObjectManager::sharedInstance().GetMainPlayer();
		RenderInfo render = player->GetRender();
		float modifier_pressD = pressD ? -1.0f : +1.0f;
		//render.rotation.y = render.rotation.y + (modifier_pressD * 0.02f);

		//render.orientation.y += (modifier_pressD * 0.6f) *deltaTime;

		// Represents the change over 1 second.
		glm::quat qRotChangeZeroSeconds;		// No rotational change
		glm::quat qRotChange(glm::vec3(0.0f, (modifier_pressD * 0.6f), 0.0f));
		glm::quat qRotChangeThisFrame = glm::slerp(qRotChangeZeroSeconds, qRotChange, deltaTime);

		render.orientation *= qRotChangeThisFrame;

		player->SetRender(render);
	}


	//if (pressA != pressD)
	//{
	//	float goLeft = pressA ? 1.6f : -1.6f;
	//	RenderInfo playerRender = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender();
	//	PhysicsInfo	playerPhysics = gelObjectManager::sharedInstance().GetMainPlayer()->GetPhysics();
	//	glm::mat4x4 playerTransform = glm::mat4x4(1.0f);

	//	//this will be abstracted
	//	playerTransform = glm::translate(playerTransform, glm::vec3(playerRender.position.x, playerRender.position.y, playerRender.position.z));
	//	glm::mat4 matRotation(playerRender.orientation);
	//	playerTransform = playerTransform * matRotation;
	//	//playerTransform = glm::rotate(playerTransform, playerRender.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
	//	//playerTransform = glm::rotate(playerTransform, playerRender.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
	//	//playerTransform = glm::rotate(playerTransform, playerRender.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
	//	playerTransform = glm::scale(playerTransform, glm::vec3(playerRender.lastDrawnActualScale, playerRender.lastDrawnActualScale, playerRender.lastDrawnActualScale));

	//	glm::vec4 unnormalizedPlayerDirection = glm::vec4(playerTransform[3].x, playerTransform[3].y, playerTransform[3].z, 0.0f);
	//	glm::vec4 left(goLeft, 0.0f, 0.0f, 0.0f);
	//	glm::vec4 leftPositive(abs(goLeft), -1.0f, 0.0f, 0.0f);
	//	unnormalizedPlayerDirection = playerTransform * left;
	//	glm::vec4 unnormalizedPlayerDirection_positive = playerTransform * leftPositive;
	//	glm::vec4 normalizedPlayerDirection = glm::normalize(unnormalizedPlayerDirection);

	//	float playerSpeedMultiplier = GelsenizeCore::sharedInstance().GetPlayerSpeed();

	//	//playerPhysics.rigidBody->ApplyImpulseAtPoint(left * deltaTime * 2.0f, playerRender.position);
	//	if (playerPhysics.collisionBody && playerPhysics.collisionBody->GetBodyType() == eBodyType::RIGID_BODY_TYPE)
	//	{
	//		/*iRigidBody* body = dynamic_cast<iRigidBody*>(playerPhysics.collisionBody);
	//		body->ApplyImpulseAtPoint(left * deltaTime * 2.0f, playerRender.position);
	//		glm::vec3 vel;
	//		body->GetVelocity(vel);
	//		playerPhysics.velocity = vel;*/
	//	}

	//	//playerPhysics.acceleration.x = normalizedPlayerDirection.x *playerSpeedMultiplier*2;
	//	//playerPhysics.acceleration.z = normalizedPlayerDirection.z *playerSpeedMultiplier*2;

	//	this->renderInfo = playerRender;
	//	this->physicsInfo = playerPhysics;
	//}

	if (!pressW && !pressA && !pressS && !pressD)
	{
		//this->physicsInfo.acceleration.x *= 0.5f;
		//this->physicsInfo.acceleration.z *= 0.5f;
	}
}

std::string cPlayer::ObjectType(void)
{
	return "player";
}

std::string cPlayer::GetObjectName(void)
{
	return this->objectName;
}

void cPlayer::SetObjectName(std::string name)
{
	this->objectName = name;
}

//Info setter-getters

LightInfo cPlayer::GetLight(void)
{
	return this->lightInfo;
}

MeshInfo cPlayer::GetMesh(void)
{
	return this->meshInfo;
}

RenderInfo cPlayer::GetRender(void)
{
	return this->renderInfo;
}

PhysicsInfo cPlayer::GetPhysics(void)
{
	return this->physicsInfo;
}

CollisionInfo cPlayer::GetCollision(void)
{
	return this->collisionInfo;
}

void cPlayer::SetCollision(CollisionInfo info)
{
	this->collisionInfo = info;
}

void cPlayer::SetLight(LightInfo info)
{
	this->lightInfo = info;
}

void cPlayer::SetMesh(MeshInfo info)
{
	this->meshInfo = info;
}
void cPlayer::SetRender(RenderInfo info)
{
	this->renderInfo = info;
}
void cPlayer::SetPhysics(PhysicsInfo info)
{
	this->physicsInfo = info;
}

//get object id

int cPlayer::getObjectID()
{
	return this->objectID;
}

//constructer, destructer

cPlayer::cPlayer()
{
	this->objectID = gelObjectManager::sharedInstance().GetNewGameObjectID();
	this->objectName = "player";
	this->GetLight();
	this->GetPhysics();
	this->GetMesh();
	this->GetRender();

	this->animation_idle = new AnimationClip("CHAR_IDLE", 4.0f);
	this->animation_walk_slow = new AnimationClip("CHAR_WALK_SLOW", 4.0f);

}

cPlayer::~cPlayer()
{
}
