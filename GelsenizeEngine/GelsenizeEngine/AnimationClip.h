#pragma once

#include "GelStructures.h"
#include <string>
#include <vector>

struct AnimationClipInfo
{
	std::string name;
	float rate;
	float duration;
	int loopCount; 
	bool overtake;
};

class AnimationClip
{
public:
	AnimationClip(std::string name_, float rate_, int loopCount = -1);
	void CopyAnimationClip(AnimationClip *animationToCopy);
	~AnimationClip();
	void Tick();

	void Reconstruct(AnimationClipInfo animationClipInfo);
	
	//void BlendIntoAnimation(std::string name_, float rate_, float duration_, int loopCount_ = -1, bool overtake = true); //later ease in/out

	bool BoneTransform( /* Get local_clock time sample */
		std::vector<glm::mat4> &FinalTransformation,
		std::vector<glm::mat4> &Globals,
		std::vector<glm::mat4> &Offsets,
		std::vector<std::string> &Names);

	std::string name;
	
	float startTimeGlobal;
	float endTimeGlobal;
	float internalClock;

	float animationTime; //calculated time from last tick

	float localClock; // global clock + number of ticks passed ;    CLAMPED    ;     0-> 0%             1-> 100%
	float weight; //blending weight
	float duration;
	float rate;
	float hardStart;

	bool overtake;
	bool playedOnce;
	
	std::string blendingAnimation_name;
	float blendingAnimation_rate;
	float blendingAnimation_duration;

	int loopCount; // -1 infinite, 0 non-looping, N loops
	bool startedTicking;
private:
	std::string initialName;
	float initialRate;
	AnimationClip();
};