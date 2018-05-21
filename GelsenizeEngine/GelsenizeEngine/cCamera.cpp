#include "cCamera.h"

#include "GelsenizeCore.h"
#include "managers.h"

#include "OCHelper.h"

cCamera::cCamera()
{
	mode = FOLLOWING_TARGET;
	targetObject = 0;
	width = 1920;  // 1440;
	height = 1080; // 960;

	zoomSpeed = 60.0f;
	minDistance = 10.0f;
	distance = 25.0f;
	maxDistance = 150.0f;

	maxFollowSpeed = 10.0f;
	followDistanceMax = 5.0f;
	followDistanceMin = 0.0f;

	pan = 0.0f;
	maxPan = 0.6f;
	minPan = -0.6f;
	panSpeed = 1.0f;

	offset = glm::vec3(4.f, 2.5f, -5.f);
	offset = glm::normalize(offset);

	offset_lookAt = glm::vec3(2.0f, 0.0f, 0.0f);

	projMatrix = glm::perspective(0.8f, (float)width / (float)height, 0.1f, 10000.f);
}

cCamera::~cCamera()
{
}

void cCamera::SetCameraMode(cCamera::MODE newMode)
{
	this->mode = newMode;
	if (this->mode != MODE::FOLLOWING_TARGET)
	{
		this->targetObject = 0;
	}
}

void cCamera::GetProjectionMatrix(glm::mat4& projOut)
{
	// set up the projection matrix
	projOut = projMatrix;
}

void cCamera::GetViewMatrix(glm::mat4& viewOut)
{
	viewOut = viewMatrix;
}

void cCamera::SetEyePosition(glm::vec4& eyeIn)
{
	this->eye = eyeIn;
}

void cCamera::SetAtPosition(glm::vec3 & atNew)
{
	this->at = atNew;
}

void cCamera::GetEyePosition(glm::vec4& eyeOut)
{
	eyeOut.x = this->eye.x;
	eyeOut.y = this->eye.y;
	eyeOut.z = this->eye.z;
	eyeOut.w = 1.0f;
}

void cCamera::WindowResize(int width_, int height_)
{
	width = width_;
	height = height_;
	projMatrix = glm::perspective(0.8f, (float)width / (float)height, 0.1f, 10000.f);
}

void cCamera::Update(float deltaTime)
{
	bool pressUp = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_UP) == GLFW_PRESS;
	bool pressDown = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_DOWN) == GLFW_PRESS;
	bool pressLeft = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_LEFT) == GLFW_PRESS;
	bool pressRight = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_RIGHT) == GLFW_PRESS;

	bool pressI = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_I) == GLFW_PRESS;
	bool pressK = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_K) == GLFW_PRESS;

	/*bool pressA = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_A) == GLFW_PRESS;
	bool pressD = glfwGetKey(GelsenizeCore::sharedInstance().GetWindow(), GLFW_KEY_D) == GLFW_PRESS;

	if (pressA != pressD)
	{
		float newPanSpeed = panSpeed;
		if (pressLeft) newPanSpeed *= -1.0f;
		pan += newPanSpeed *deltaTime;
		pan = glm::clamp(pan, minPan, maxPan);
	}*/

	if (1)
	{
		//mOffset = glm::vec3(0.f, 0.5f, -1.f);
		//if (pressI != pressK)
		//{
		//	float dirZoomSpeed = zoomSpeed  * 4.0f;
		//	if (pressK) dirZoomSpeed *= -1.0f;
		//	dirZoomSpeed *= 0.2f;
		//	offset.y += dirZoomSpeed * deltaTime;
		//	offset.y = glm::clamp(offset.y, -2.0f, +2.0f);
		//	//if (offset.y < -minDistance) offset.y = -minDistance;
		//	//if (offset.y > maxDistance) offset.y = maxDistance;
		//}

		if (pressDown != pressUp)
		{
			float dirZoomSpeed = zoomSpeed / 5.0f;
			if (pressUp) dirZoomSpeed *= -1.0f;
			distance += dirZoomSpeed * deltaTime;
			if (distance < minDistance) distance = minDistance;
			if (distance > maxDistance) distance = maxDistance;
		}

		if ((pressLeft != pressRight))
		{
			//not used
			float newPanSpeed = panSpeed;
			if (pressLeft) newPanSpeed *= -1.0f;
			pan += newPanSpeed *deltaTime;
			pan = glm::clamp(pan, minPan, maxPan);
		}
		else
		{
			if (pan != 0.0f)
			{
				if (pan > 0.0f)
				{
					pan -= 2 * panSpeed *deltaTime;
					if (pan < 0)
					{
						//it went too far on its own, set it to 0
						pan = 0;
					}
				}
				else
				{
					pan += 2 * panSpeed *deltaTime;
					if (pan > 0)
					{
						//it went too far on its own, set it to 0
						pan = 0;
					}
				}
			}
		}
	}
	UpdateView(deltaTime);
}

void cCamera::SetTargetObject(iGameObject * newTargetObject)
{
	this->targetObject = newTargetObject;
	//RenderInfo targetRender = this->targetObject->GetRender();
	//glm::vec3 desiredEye;
	//glm::mat4x4 m = glm::mat4x4(1.0f);
	//m = glm::translate(targetRender.transform, glm::vec3(offset.x, offset.y, offset.z));
	//desiredEye = m[3];
	//this->eye = glm::vec3(desiredEye.x, desiredEye.y, desiredEye.z);
}

void cCamera::UpdateView(float deltaTime)
{
	if (!this->targetObject && mode != FOLLOWING_TARGET)
	{
		//do free mode on player

		// set up the view matrix
		//RenderInfo playerRender = gelObjectManager::sharedInstance().GetMainPlayer()->GetRender();
		//glm::vec4 playerPosition = playerRender.transform[3];
		glm::vec3 up(0.f, 1.f, 0.f);
		
		projMatrix = glm::perspective(0.8f, (float)width / (float)height, 0.1f, 10000.f);
		viewMatrix = glm::lookAtRH(eye, at, up); // at is target

		glUniform3f(GelsenizeCore::sharedInstance().EyeLocationID(), eye.x, eye.y, eye.z);

		//we can move this from here and only update when we change the aspect ratio or sizew of screen.
		glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetProjLocation(), 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(projMatrix));

		glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetViewLocation(), 1, GL_FALSE,
			(const GLfloat*)glm::value_ptr(viewMatrix));

		return;
	}

	glm::vec3 targetPosition;

	RenderInfo targetRender = this->targetObject->GetRender();
	targetPosition = targetRender.position;//glm::vec4(targetRender.transform[3]);// targetRender.position, 0.0f);

	glm::vec3 up(0.f, 1.f, 0.f);

	glm::vec3 calcOffset3 = glm::normalize(offset);
	calcOffset3 *= distance;

	glm::vec4 backwards(0.0f, 0.0f, +1.0f, 0.0f);
	glm::vec4 upwards(0.0f, +1.0f, 0.0f, 0.0f);
	glm::vec4 left(+1.0f, 0.0f, 0.0f, 0.0f);

	glm::vec4 targetWorld(targetRender.position.x, targetRender.position.y, targetRender.position.z, 1.0f);
	glm::vec4 targetBackwards = glm::normalize(targetRender.transform * backwards);
	glm::vec4 targetUpwards   = glm::normalize(targetRender.transform * upwards);
	glm::vec4 targetLeft      = glm::normalize(targetRender.transform * left);

	glm::vec4 sexyUp   = glm::normalize(glm::vec4(targetRender.transform[1].x, targetRender.transform[1].y, targetRender.transform[1].z, 0.0f));
	glm::vec4 sexyBack = glm::normalize(glm::vec4(targetRender.transform[2].x, targetRender.transform[2].y, targetRender.transform[2].z, 0.0f));
	glm::vec4 sexyLeft = glm::normalize(glm::vec4(targetRender.transform[0].x, targetRender.transform[0].y, targetRender.transform[0].z, 0.0f));

	glm::vec3 cameraPosition;
	glm::mat4x4 cameraTransform(targetRender.transform);
	cameraTransform = glm::translate(cameraTransform, glm::normalize(offset)*distance);


	//add mouse offset
	glm::vec4 at_ = targetWorld + (targetBackwards * offset_lookAt.z) + (targetUpwards * offset_lookAt.y) + (targetLeft * offset_lookAt.x);
	this->at = glm::vec3(at_.x, at_.y, at_.z);
	//this->at = targetPosition;

	//this->at.y += 5.0f;
	glm::vec4 desiredEye = targetWorld + (targetBackwards * calcOffset3.z) + (targetUpwards * calcOffset3.y) + (targetLeft * calcOffset3.x);

	glm::vec3 distToTarget;
	distToTarget.x = std::abs(desiredEye.x - this->eye.x);
	distToTarget.y = std::abs(desiredEye.y - this->eye.y);
	distToTarget.z = std::abs(desiredEye.z - this->eye.z);

	// Calculate the relative speeds for each axis (could be moving fast on one axis and slow on another)
	//	using the smooth step calcualtion (GT max distance = max speed, LT min distance = 0);
	glm::vec3 moveStep;
	moveStep.x = OCHelper::smootherStep(this->followDistanceMin, this->followDistanceMax, distToTarget.x);
	moveStep.y = OCHelper::smootherStep(this->followDistanceMin, this->followDistanceMax, distToTarget.y);
	moveStep.z = OCHelper::smootherStep(this->followDistanceMin, this->followDistanceMax, distToTarget.z);

	// Scale for time step
	moveStep.x *= this->maxFollowSpeed *deltaTime * 10.0f;
	moveStep.y *= this->maxFollowSpeed *deltaTime * 10.0f;
	moveStep.z *= this->maxFollowSpeed *deltaTime * 10.0f;

	// If camera is LT position, reverse the moving direction (it's positive because of ABS distance)
	if (desiredEye.x < this->eye.x) { moveStep.x = -moveStep.x; }
	if (desiredEye.y < this->eye.y) { moveStep.y = -moveStep.y; }
	if (desiredEye.z < this->eye.z) { moveStep.z = -moveStep.z; }

	// If we are outside of the min distance, move the camera
	if (distToTarget.x > this->followDistanceMin) { this->eye.x += moveStep.x; }
	if (distToTarget.y > this->followDistanceMin) { this->eye.y += moveStep.y; }
	if (distToTarget.z > this->followDistanceMin) { this->eye.z += moveStep.z; }
	
	// set up the view matrix
	viewMatrix = glm::lookAtRH(eye, at, up); // at is target

	//gelShaderManager::sharedInstance().useShaderProgram("default");

	////is this correct?
	//glUniform3f(GelsenizeCore::sharedInstance().EyeLocationID(), eye.x,eye.y,eye.z);

	////we can move this from here and only update when we change the aspect ratio or sizew of screen.
	//glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetProjLocation(), 1, GL_FALSE,
	//	(const GLfloat*)glm::value_ptr(projMatrix));

	//glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetViewLocation(), 1, GL_FALSE,
	//	(const GLfloat*)glm::value_ptr(viewMatrix));

	//gelShaderManager::sharedInstance().useShaderProgram("glass");

	//glUniform3f(GelsenizeCore::sharedInstance().EyeLocationID("glass"), eye.x, eye.y, eye.z);

	////we can move this from here and only update when we change the aspect ratio or sizew of screen.
	//glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetProjLocation("glass"), 1, GL_FALSE,
	//	(const GLfloat*)glm::value_ptr(projMatrix));

	//glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetViewLocation("glass"), 1, GL_FALSE,
	//	(const GLfloat*)glm::value_ptr(viewMatrix));
}

void cCamera::SetUniforms(std::string programName)
{
	//gelShaderManager::sharedInstance().useShaderProgram(programName);

	glUniform3f(GelsenizeCore::sharedInstance().EyeLocationID(programName), eye.x, eye.y, eye.z);

	//we can move this from here and only update when we change the aspect ratio or sizew of screen.
	glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetProjLocation(programName), 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(projMatrix));

	glUniformMatrix4fv(GelsenizeCore::sharedInstance().GetViewLocation(programName), 1, GL_FALSE,
		(const GLfloat*)glm::value_ptr(viewMatrix));
}

void cCamera::SetOffset(glm::vec3& newOffset)
{
	this->offset = newOffset;//glm::normalize(newOffset);
}
