#pragma once

#include "GelStructures.h"
#include <string>
#include <vector>
#include "AnimationClip.h"

#include <queue>

enum ANIMATION_CHANNEL
{
	MAIN_1,
	MAIN_2,
	MAIN_3,

	BLEND_INTO,

	BLEND_WITH_1,
	BLEND_WITH_2,
	BLEND_WITH_3,
};

class AnimationController
{
public:
	AnimationController();
	~AnimationController();

	enum CONTROLLER_STATE
	{
		NO_ANIMATION,
		PLAYING_ANIMATION_1,
		PLAYING_ANIMATION_2,
		PLAYING_ANIMATION_3,

		BLENDING_INTO, //blend and replace (transition)  w: 0->1

		BLENDING_WITH_1, //blend and reset                 w: 0->1->0
		BLENDING_WITH_2, //blend and reset                 w: 0->1->0
		BLENDING_WITH_3, //blend and reset                 w: 0->1->0
	};

	std::string GetSkinnedMeshName(ANIMATION_CHANNEL channel = MAIN_1);
	void PlayAnimation(AnimationClipInfo animationClipInfo, ANIMATION_CHANNEL channel);
	
	void BlendIntoAnimation(AnimationClipInfo animationClipInfo, float blend_tick = 0.2f); //curves here //            1.0f / 0.05f = 20ticks of animation
	void BlendWithAnimation(AnimationClipInfo animationClipInfo, float blend_tick = 0.2f);
	
	void Tick();
	void BoneTransform( /* Get final pose */
		std::vector<glm::mat4> &FinalTransformation,
		std::vector<glm::mat4> &Globals,
		std::vector<glm::mat4> &Offsets,
		std::vector<std::string> &Names);

	float blendInto_weight;
	float blendWith_weight_1;
	float blendWith_weight_2;
	float blendWith_weight_3;

	float blendInto_tick;
	float blendWith_tick_1;
	float blendWith_tick_2;
	float blendWith_tick_3;

	AnimationClip *animationClip_1;
	AnimationClip *animationClip_2;
	AnimationClip *animationClip_3;

	AnimationClip *animationClip_BlendingInto;

	AnimationClip *animationClip_BlendingWith_1;
	AnimationClip *animationClip_BlendingWith_2; //use this as storage mb?
	AnimationClip *animationClip_BlendingWith_3;

	bool blendWith_1_duration_acc;

	bool blendWith_1_pass;
	bool blendWith_2_pass;
	bool blendWith_3_pass;

	std::queue<AnimationClip *> animationQueue;
	CONTROLLER_STATE state;
private:
	void rotateHeadWithMouse(glm::mat4 & finalTransformation);
};

