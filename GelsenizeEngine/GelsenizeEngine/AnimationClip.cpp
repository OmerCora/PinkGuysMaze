#include "AnimationClip.h"

#include "cSimpleAssimpSkinnedMeshLoader_OneMesh.h"
#include "gelMeshManager.h"
#include "GelsenizeCore.h"

#include <iostream>

AnimationClip::AnimationClip()
{
}

AnimationClip::AnimationClip(std::string name_, float rate_, int loopCount_)
{
	this->startedTicking = false;
	this->name = name_;
	this->rate = rate_;
	this->initialName = name_;
	this->initialRate = rate_;
	this->loopCount = loopCount_;

	this->localClock = 0.0f;
	this->internalClock = 0.0f;
	this->animationTime = 0.0f;

	this->overtake = false;
	this->playedOnce = false;

	this->hardStart = 0.0f;

	this->duration = 0.0f;
	cSimpleAssimpSkinnedMesh *skinnedMesh = gelMeshManager::sharedInstance().GetSkinnedMesh(name_);
	if (skinnedMesh)
	{
		this->duration = skinnedMesh->GetDuration();
	}

}

void AnimationClip::CopyAnimationClip(AnimationClip *animationToCopy)
{
	this->startedTicking = false;
	this->name = animationToCopy->name;
	this->rate = animationToCopy->rate;
	this->initialName = animationToCopy->initialName;
	this->initialRate = animationToCopy->rate;
	this->loopCount = animationToCopy->loopCount;
	this->overtake = animationToCopy->overtake;

	this->localClock = animationToCopy->localClock;
	this->internalClock = animationToCopy->internalClock;
	this->animationTime = animationToCopy->animationTime;

	this->duration = 0.0f;
	cSimpleAssimpSkinnedMesh *skinnedMesh = gelMeshManager::sharedInstance().GetSkinnedMesh(animationToCopy->name);
	if (skinnedMesh)
	{
		this->duration = skinnedMesh->GetDuration();
	}
}

AnimationClip::~AnimationClip()
{
}

void AnimationClip::Tick()
{
	if (this->startedTicking == false)
	{

		float globalClock = GelsenizeCore::sharedInstance().GetGlobalClock();

		this->startedTicking = true;

		this->startTimeGlobal = globalClock + 0.0f;
		this->endTimeGlobal = startTimeGlobal + this->duration;

		this->internalClock = startTimeGlobal;

		if (this->name == "CHAR_JUMP")
		{
			this->internalClock += 0.02f;
		}
	}
	else
	{
		this->internalClock += 0.01f;// *(rate / rate);
	}

	if(this->duration == 0.0f)
	{
		return;
	}
	
	/*if (rate < 0.0f)
	{
		if (this->internalClock < 0.0f)
		{
			this->internalClock = 1.0f;
		}
		else if (this->internalClock > 1.0f)
		{

		}
	}*/


	if (this->name == "CHAR_CAST_1")
	{
		int x = 0;
	}

	{
		if (this->loopCount == 1)
		{
			//play once
			this->localClock = glm::clamp((this->internalClock - this->startTimeGlobal) * this->rate, 0.0f, endTimeGlobal - startTimeGlobal);
		}
		else if (this->loopCount == -1)
		{
			//loop inf
			this->localClock = (this->internalClock - this->startTimeGlobal) * this->rate;
			while (this->localClock >= this->duration)
			{
				this->localClock -= this->duration;
			}
		}
		else if (this->loopCount > 1)
		{
			//loop X times
			this->localClock = glm::clamp(((this->internalClock - this->startTimeGlobal) * this->rate), startTimeGlobal, endTimeGlobal);
			while (this->localClock >= endTimeGlobal)
			{
				this->loopCount -= 1;
				this->localClock -= this->duration;
			}
		}
	}

	if (localClock >= endTimeGlobal - startTimeGlobal)
	{
		playedOnce = true;
	}

	float localClock_ = localClock;
}

void AnimationClip::Reconstruct(AnimationClipInfo animationClipInfo)
{
	this->startedTicking = false;
	this->localClock = 0.0f;
	this->internalClock = 0.0f;
	this->animationTime = 0.0f;
	
	this->overtake = animationClipInfo.overtake;
	this->playedOnce = false;
	this->name = animationClipInfo.name;
	this->rate = animationClipInfo.rate;
	this->loopCount = animationClipInfo.loopCount;


	if (this->name == "CHAR_JUMP")
	{

	}

	cSimpleAssimpSkinnedMesh *skinnedMesh = gelMeshManager::sharedInstance().GetSkinnedMesh(animationClipInfo.name);
	if (skinnedMesh)
	{
		this->duration = skinnedMesh->GetDuration();
	}
	//this->Tick();
}

//void AnimationClip::BlendIntoAnimation(std::string name_, float rate_, float duration_, int loopCount_, bool overtake)
//{
//	if (duration == 0.0f)
//	{
//		//instant swap
//		this->startedTicking = false;
//		this->name = name_;
//		this->rate = rate_;
//		this->loopCount = loopCount_;
//
//		this->duration = 0.0f;
//		this->internalClock = 0.0f;
//		this->animationTime = 0.0f;
//
//		cSimpleAssimpSkinnedMesh *skinnedMesh = gelMeshManager::sharedInstance().GetSkinnedMesh(name_);
//		if (skinnedMesh)
//		{
//			this->duration = skinnedMesh->GetDuration();
//		}
//	}
//	else
//	{
//		//proper blending
//	}
//}

bool AnimationClip::BoneTransform(std::vector<glm::mat4>& FinalTransformation, std::vector<glm::mat4>& Globals, std::vector<glm::mat4>& Offsets, std::vector<std::string> & Names)
{
	if (this->startedTicking == false)
	{
		//this animation has not ticked yet
		//we could also return 0.0f local clock transform too though
		this->localClock = 0.0f;
	}

	if (this->name == "CHAR_JUMP")
	{
		hardStart = 0.0f;
	}
	else
	{
		hardStart = 0.0f;
	}

	float animTime = hardStart + this->localClock;

	if (rate >= 0.0f)
	{
		
	}
	else
	{
		animTime = 1 - animTime;
	}

	cSimpleAssimpSkinnedMesh *skinnedMesh = gelMeshManager::sharedInstance().GetSkinnedMesh(this->name);
	if (!skinnedMesh)
	{
		return false;
	}

	//animTime = 0.0f;

	skinnedMesh->BoneTransform(animTime, FinalTransformation, Globals, Offsets, Names);
	//std::cout << animTime << std::endl;
	return true;
}
