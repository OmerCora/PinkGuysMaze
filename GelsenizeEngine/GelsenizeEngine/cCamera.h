#ifndef _CCAMERA_HG_
#define _CCAMERA_HG_

#include "externals.h"
#include "iGameObject.h"

class cCamera
{
public:
	cCamera();
	~cCamera();

	enum MODE
	{
		FREECAM, //attached to no_model player
		FOLLOWING_TARGET,
		STATIONARY

	};
	void SetUniforms(std::string programName);
	void SetCameraMode(cCamera::MODE newMode);

	void GetProjectionMatrix(glm::mat4& projOut);
	void GetViewMatrix(glm::mat4& viewOut);
	void GetEyePosition(glm::vec4& eyeOut);
	void SetEyePosition(glm::vec4& eyeIn);
	void SetAtPosition(glm::vec3& atNew);
	void WindowResize(int width, int height);
	void GetWindowSize(int &width_, int &height_) { width_ = this->width; height_ = this->width; };

	void UpdateView(float deltaTime);

	void Update(float deltaTime);
	void SetTargetObject(iGameObject* newTargetObject);
	void SetOffset(glm::vec3& offset);

	void GetOffsetLookAt(glm::vec3& offsetLookAt) { offsetLookAt = this->offset_lookAt; };
	void SetOffsetLookAt(glm::vec3& offsetLookAt) { this->offset_lookAt = offsetLookAt; };
private:
	iGameObject* targetObject;

	int width;
	int height;
	float maxFollowSpeed;
	float followDistanceMin;
	float followDistanceMax;
	glm::vec3 eye;
	glm::vec3 at;
	glm::vec3 target_at;
	glm::mat4 targetTranform;

	glm::vec3 offset;
	glm::vec3 offset_lookAt;
	float zoomSpeed;
	float panSpeed;
	float pan;
	float maxPan;
	float minPan;
	float distance;
	float maxDistance;
	float minDistance;
	glm::mat4 viewMatrix;
	glm::mat4 projMatrix;
	
	MODE mode;
};

#endif // !_CCAMERA_HG_

