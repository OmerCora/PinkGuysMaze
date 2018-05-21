#include "AnimationController.h"

#include "externals.h"

#include "GelsenizeCore.h"
#include "gelCameraManager.h"

AnimationController::AnimationController()
{
	this->animationClip_1 = new AnimationClip("no_animation", 0.0f, 0);
	this->animationClip_2 = new AnimationClip("no_animation", 0.0f, 0);	
	this->animationClip_3 = new AnimationClip("no_animation", 0.0f, 0);
	this->animationClip_BlendingInto = new AnimationClip("no_animation", 0.0f, 0);
	this->animationClip_BlendingWith_1 = new AnimationClip("no_animation", 0.0f, 0);
	this->animationClip_BlendingWith_2 = new AnimationClip("no_animation", 0.0f, 0);
	this->animationClip_BlendingWith_3 = new AnimationClip("no_animation", 0.0f, 0);

	this->blendInto_weight = 0.0f;
	this->blendWith_weight_1 = 0.0f;
	this->blendWith_weight_2 = 0.0f;
	this->blendWith_weight_3 = 0.0f;

	this->blendInto_tick = 0.0f;
	this->blendWith_tick_1 = 0.0f;
	this->blendWith_tick_2 = 0.0f;
	this->blendWith_tick_3 = 0.0f;

	this->blendWith_1_pass = false;
	this->blendWith_2_pass = false;
	this->blendWith_3_pass = false;

	this->state = NO_ANIMATION;
}

AnimationController::~AnimationController()
{
}

std::string AnimationController::GetSkinnedMeshName(ANIMATION_CHANNEL channel)
{
	AnimationClip *animationClip;

	if (channel == MAIN_1)
	{
		animationClip = this->animationClip_1;
	}
	else if (channel == MAIN_2)
	{
		animationClip = this->animationClip_2;
	}
	else if (channel == MAIN_3)
	{
		animationClip = this->animationClip_3;
	}
	else if (channel == BLENDING_INTO)
	{
		animationClip = this->animationClip_BlendingInto;
	}
	else if (channel == BLENDING_WITH_1)
	{
		animationClip = this->animationClip_BlendingWith_1;
	}
	else if (channel == BLENDING_WITH_2)
	{
		animationClip = this->animationClip_BlendingWith_2;
	}
	else if (channel == BLENDING_WITH_3)
	{
		animationClip = this->animationClip_BlendingWith_3;
	}
	else
	{
		//NO ANIMATION
		return "no_animation";
	}
	return animationClip->name;
}

void AnimationController::PlayAnimation(AnimationClipInfo animationClipInfo, ANIMATION_CHANNEL channel)
{
	AnimationClip *animationClip;

	if (channel == MAIN_1)
	{
		this->state = PLAYING_ANIMATION_1;
		animationClip = this->animationClip_1;
	}
	else if (channel == MAIN_2)
	{
		this->state = PLAYING_ANIMATION_2;
		animationClip = this->animationClip_2;
	}
	else if (channel == MAIN_3)
	{
		this->state = PLAYING_ANIMATION_3;
		animationClip = this->animationClip_3;
	}
	else if (channel == BLENDING_INTO)
	{
		this->state = BLENDING_INTO;
		animationClip = this->animationClip_BlendingInto;
	}
	else if (channel == BLENDING_WITH_1)
	{
		this->state = BLENDING_WITH_1;
		animationClip = this->animationClip_BlendingWith_1;
	}
	else if (channel == BLENDING_WITH_2)
	{
		this->state = BLENDING_WITH_2;
		animationClip = this->animationClip_BlendingWith_2;
	}
	else if (channel == BLENDING_WITH_3)
	{
		this->state = BLENDING_WITH_3;
		animationClip = this->animationClip_BlendingWith_3;
	}
	else
	{
		//NO ANIMATION
		return;
	}

	animationClip->Reconstruct(animationClipInfo);
}

void AnimationController::BlendIntoAnimation(AnimationClipInfo animationClipInfo, float blend_tick)
{
	if (animationClipInfo.name == "CHAR_CAST_1")
	{
		int x = 0;
	}

	if (this->state == BLENDING_WITH_1)
	{
		if (!this->animationClip_BlendingWith_1->playedOnce)
		{
			return;
		}
	}
	
	if (this->state == BLENDING_INTO)
	{
		if (this->animationClip_BlendingInto->name == animationClipInfo.name)
		{
			//already blending into the same animation
			return;
		}

		if (this->animationClip_BlendingInto->overtake)
		{
			//this animation cannot be blended into;
			return;
		}
	}
	else if (this->state == PLAYING_ANIMATION_1)
	{
		if (this->animationClip_1->name == animationClipInfo.name)
		{
			//already playing the same animation
			return;
		}
	}
	else if (this->state == NO_ANIMATION)
	{
		this->state = PLAYING_ANIMATION_1;
		this->PlayAnimation(animationClipInfo, MAIN_1);
		return;
	}

	//AnimationClip *animationClip = this->animationClip_BlendingInto;

	this->blendInto_weight = 0.0f;
	this->blendInto_tick = blend_tick;

	this->animationClip_BlendingInto->Reconstruct(animationClipInfo);

	this->state = BLENDING_INTO;
}

void AnimationController::BlendWithAnimation(AnimationClipInfo animationClipInfo, float blend_tick)
{
	if (this->state == BLENDING_WITH_1)
	{
		if (this->animationClip_BlendingWith_1->name == animationClipInfo.name)
		{
			//already playing the same animation
			return;
		}
	}
	this->blendWith_weight_1 = 0.0f;
	this->blendWith_tick_1 = blend_tick;

	this->animationClip_BlendingWith_1->Reconstruct(animationClipInfo);
	this->state = BLENDING_WITH_1;
	blendWith_1_pass = false;
}

void AnimationController::Tick()
{
	if (this->state != BLENDING_INTO)
	{
		this->animationClip_1->Tick();
		this->animationClip_BlendingInto->Tick();
	}
	this->animationClip_2->Tick();
	this->animationClip_3->Tick();
	
	this->animationClip_BlendingWith_1->Tick();
	this->animationClip_BlendingWith_2->Tick();
	this->animationClip_BlendingWith_3->Tick();

	//advance blending ticks
	this->blendInto_weight   += this->blendInto_tick;

	this->blendWith_weight_1 += this->blendWith_tick_1;
	this->blendWith_weight_2 += this->blendWith_tick_2;
	this->blendWith_weight_3 += this->blendWith_tick_3;
}

void AnimationController::BoneTransform(std::vector<glm::mat4>& FinalTransformation, std::vector<glm::mat4>& Globals, std::vector<glm::mat4>& Offsets, std::vector<std::string> &Names)
{
	if (this->animationClip_BlendingWith_1->name == "CHAR_CAST_1")
	{
		int x = 0.0f;
	}
	if (this->animationClip_1->name == "CHAR_JUMP")
	{
		int x = 0.0f;
	}

	if (this->state == PLAYING_ANIMATION_1)
	{
		//Just get the main animation

		this->animationClip_1->BoneTransform(FinalTransformation, Globals, Offsets, Names);

		for (int i = 0; i != FinalTransformation.size(); i++)
		{
			std::string mainBoneName = Names[i];
			if (mainBoneName == "B_Head")
			{
				rotateHeadWithMouse(FinalTransformation[i]);
			}
		}

			
	}
	else if (this->state == PLAYING_ANIMATION_2)
	{

	}
	else if (this->state == PLAYING_ANIMATION_3)
	{
		//
	}
	else if (this->state == BLENDING_INTO)
	{
		float weight = this->blendInto_weight;
		if (weight >= 1.0f)
		{
			this->state = PLAYING_ANIMATION_1;

			if (this->animationClip_BlendingInto->loopCount != -1)
			{
				//non looping complete, return
				return;
			}

			//blending complete, set the state to normal animation and set blending amination as main animation
			this->animationClip_1->CopyAnimationClip(this->animationClip_BlendingInto);
		}

		std::vector<glm::mat4> FinalTransformation_Main;
		std::vector<glm::mat4> FinalTransformation_Blending;

		std::vector<glm::mat4> globals_main;
		std::vector<glm::mat4> globals_blending;

		std::vector<glm::mat4> offsets_main;
		std::vector<glm::mat4> offsets_blending;

		std::vector<std::string> names_main;
		std::vector<std::string> names_blending;

		this->animationClip_1->BoneTransform(FinalTransformation_Main, globals_main, offsets_main, names_main);
		this->animationClip_BlendingInto->BoneTransform(FinalTransformation_Blending, globals_blending, offsets_blending, names_blending);

		Globals = globals_blending;
		Offsets = offsets_blending;

		if (FinalTransformation_Main.size() != FinalTransformation_Blending.size())
		{
			return;
		}

		for (int i = 0; i != FinalTransformation_Main.size(); i++)
		{
			//check if the main animation's bone is a finger, 
			//if so remove weight temporarily

			std::string mainBoneName = names_main[i];

			float cullValue = 1.0f;
			float finalWeight = weight;

			//if (mainBoneName == "B_R_Finger0" || mainBoneName == "B_R_Finger01" || mainBoneName == "B_R_Finger02" ||
			//	mainBoneName == "B_R_Finger1" || mainBoneName == "B_R_Finger11" || mainBoneName == "B_R_Finger12" ||
			//	mainBoneName == "B_R_Finger2" || mainBoneName == "B_R_Finger21" || mainBoneName == "B_R_Finger22" ||
			//	mainBoneName == "B_R_Finger3" || mainBoneName == "B_R_Finger31" || mainBoneName == "B_R_Finger32" ||
			//	mainBoneName == "B_R_Finger4" || mainBoneName == "B_R_Finger41" || mainBoneName == "B_R_Finger42" ||
			//	mainBoneName == "B_L_Finger0" || mainBoneName == "B_L_Finger01" || mainBoneName == "B_L_Finger02" ||
			//	mainBoneName == "B_L_Finger1" || mainBoneName == "B_L_Finger11" || mainBoneName == "B_L_Finger12" ||
			//	mainBoneName == "B_L_Finger2" || mainBoneName == "B_L_Finger21" || mainBoneName == "B_L_Finger22" ||
			//	mainBoneName == "B_L_Finger3" || mainBoneName == "B_L_Finger31" || mainBoneName == "B_L_Finger32" ||
			//	mainBoneName == "B_L_Finger4" || mainBoneName == "B_L_Finger41" || mainBoneName == "B_L_Finger42"  )
			//{
			//	//finalWeight = 1.0f;
			//}

				/*||
				mainBoneName == "B_L_Hand" ||
				mainBoneName == "B_R_Hand" ||
				mainBoneName == "B_L_Forearm" ||
				mainBoneName == "B_L_UpperArm" ||
				mainBoneName == "B_R_Forearm" ||
				mainBoneName == "B_R_UpperArm"*/

			if (finalWeight >= 1.0f)
			{
				finalWeight = 1.0f;
			}

			glm::mat4 mainMatrix = FinalTransformation_Main[i];
			glm::mat4 blendMatrix = FinalTransformation_Blending[i];

			glm::mat4 testMatrix; //our RPG char animations blends like no tomorrow with this, slerp makes the fingers trippy
			testMatrix[0][0] = glm::mix(mainMatrix[0][0], blendMatrix[0][0], finalWeight);
			testMatrix[0][1] = glm::mix(mainMatrix[0][1], blendMatrix[0][1], finalWeight);
			testMatrix[0][2] = glm::mix(mainMatrix[0][2], blendMatrix[0][2], finalWeight);
			testMatrix[0][3] = glm::mix(mainMatrix[0][3], blendMatrix[0][3], finalWeight);

			testMatrix[1][0] = glm::mix(mainMatrix[1][0], blendMatrix[1][0], finalWeight);
			testMatrix[1][1] = glm::mix(mainMatrix[1][1], blendMatrix[1][1], finalWeight);
			testMatrix[1][2] = glm::mix(mainMatrix[1][2], blendMatrix[1][2], finalWeight);
			testMatrix[1][3] = glm::mix(mainMatrix[1][3], blendMatrix[1][3], finalWeight);

			testMatrix[2][0] = glm::mix(mainMatrix[2][0], blendMatrix[2][0], finalWeight);
			testMatrix[2][1] = glm::mix(mainMatrix[2][1], blendMatrix[2][1], finalWeight);
			testMatrix[2][2] = glm::mix(mainMatrix[2][2], blendMatrix[2][2], finalWeight);
			testMatrix[2][3] = glm::mix(mainMatrix[2][3], blendMatrix[2][3], finalWeight);

			testMatrix[3][0] = glm::mix(mainMatrix[3][0], blendMatrix[3][0], finalWeight);
			testMatrix[3][1] = glm::mix(mainMatrix[3][1], blendMatrix[3][1], finalWeight);
			testMatrix[3][2] = glm::mix(mainMatrix[3][2], blendMatrix[3][2], finalWeight);
			testMatrix[3][3] = glm::mix(mainMatrix[3][3], blendMatrix[3][3], finalWeight);

			if (mainBoneName == "B_Head")
			{
				rotateHeadWithMouse(testMatrix);
			}

			FinalTransformation.push_back(testMatrix);
			continue;

			//int modulatedIndex_1 = (i+0) % FinalTransformation_Main.size();
			//int modulatedIndex_2 = (i+0) % FinalTransformation_Main.size();

			//glm::vec4 translationMain = FinalTransformation_Main[modulatedIndex_1][3];
			//glm::vec4 translationBlend = FinalTransformation_Blending[modulatedIndex_2][3];

			////glm::vec4 translationFinal = ((1.0f - weight) * translationMain) + (weight * translationBlend);
			//glm::vec4 translationFinal = glm::mix(translationMain, translationBlend, finalWeight );
			//
			//glm::quat rotationMain = glm::normalize(glm::quat_cast(FinalTransformation_Main[modulatedIndex_1]));
			//glm::quat rotationBlend = glm::normalize(glm::quat_cast(FinalTransformation_Blending[modulatedIndex_2]));

			//glm::quat rotationFinal = glm::normalize( glm::slerp(rotationMain, rotationBlend, finalWeight) );

			//glm::mat4 matRotation(rotationFinal);

			//glm::mat4 finalMatrix(1.0f);
			//finalMatrix = glm::mat4_cast(rotationFinal);

			////finalMatrix = glm::translate(finalMatrix, glm::vec3(translationFinal.x, translationFinal.y, translationFinal.z));
			////finalMatrix = finalMatrix * matRotation;
			////matRotation[3].x = translationFinal.x;
			////matRotation[3].y = translationFinal.y;
			////matRotation[3].z = translationFinal.z;
			////matRotation[3].w = translationFinal.w;
			//
			////finalMatrix = glm::translate(finalMatrix, glm::vec3(translationFinal.x, translationFinal.y, translationFinal.z));

			//finalMatrix[3].x = translationFinal.x;
			//finalMatrix[3].y = translationFinal.y;
			//finalMatrix[3].z = translationFinal.z;
			//finalMatrix[3].w = translationFinal.w;

			//FinalTransformation.push_back(finalMatrix);
		}

		
	}
	else if (this->state == BLENDING_WITH_1)
	{
		//blend into an animation, perform it end blend back to the original animation

		float weight = this->blendWith_weight_1;
		if (weight >= 1.0f)
		{

		}

		if (this->animationClip_BlendingWith_1->playedOnce)
		{
			//it played once

			//blend back to original 
			/*AnimationClipInfo info;
			info.name = this->animationClip_1->name;
			info.rate = this->animationClip_1->rate;
			info.loopCount = this->animationClip_1->loopCount;
			blendWith_1_pass = true;

			this->blendInto_weight = 0.0f;
			this->animationClip_1 = this->animationClip_BlendingWith_1;
			this->BlendIntoAnimation(info, 0.5f);*/
			return;
		}

		std::vector<glm::mat4> FinalTransformation_Main;
		std::vector<glm::mat4> FinalTransformation_Blending;

		std::vector<glm::mat4> globals_main;
		std::vector<glm::mat4> globals_blending;

		std::vector<glm::mat4> offsets_main;
		std::vector<glm::mat4> offsets_blending;

		std::vector<std::string> names_main;
		std::vector<std::string> names_blending;

		this->animationClip_1->BoneTransform(FinalTransformation_Main, globals_main, offsets_main, names_main);
		this->animationClip_BlendingWith_1->BoneTransform(FinalTransformation_Blending, globals_blending, offsets_blending, names_blending);

		Globals = globals_blending;
		Offsets = offsets_blending;

		if (FinalTransformation_Main.size() != FinalTransformation_Blending.size())
		{
			return;
		}

		for (int i = 0; i != FinalTransformation_Main.size(); i++)
		{
			//check if the main animation's bone is a finger, 
			//if so remove weight temporarily

			std::string mainBoneName = names_main[i];

			float cullValue = 1.0f;
			float finalWeight = weight;

			if (finalWeight >= 1.0f)
			{
				finalWeight = 1.0f;
			}

			glm::mat4 mainMatrix = FinalTransformation_Main[i];
			glm::mat4 blendMatrix = FinalTransformation_Blending[i];

			glm::mat4 testMatrix; //our RPG char animations blends like no tomorrow with this, slerp makes the fingers trippy
			testMatrix[0][0] = glm::mix(mainMatrix[0][0], blendMatrix[0][0], finalWeight);
			testMatrix[0][1] = glm::mix(mainMatrix[0][1], blendMatrix[0][1], finalWeight);
			testMatrix[0][2] = glm::mix(mainMatrix[0][2], blendMatrix[0][2], finalWeight);
			testMatrix[0][3] = glm::mix(mainMatrix[0][3], blendMatrix[0][3], finalWeight);

			testMatrix[1][0] = glm::mix(mainMatrix[1][0], blendMatrix[1][0], finalWeight);
			testMatrix[1][1] = glm::mix(mainMatrix[1][1], blendMatrix[1][1], finalWeight);
			testMatrix[1][2] = glm::mix(mainMatrix[1][2], blendMatrix[1][2], finalWeight);
			testMatrix[1][3] = glm::mix(mainMatrix[1][3], blendMatrix[1][3], finalWeight);

			testMatrix[2][0] = glm::mix(mainMatrix[2][0], blendMatrix[2][0], finalWeight);
			testMatrix[2][1] = glm::mix(mainMatrix[2][1], blendMatrix[2][1], finalWeight);
			testMatrix[2][2] = glm::mix(mainMatrix[2][2], blendMatrix[2][2], finalWeight);
			testMatrix[2][3] = glm::mix(mainMatrix[2][3], blendMatrix[2][3], finalWeight);

			testMatrix[3][0] = glm::mix(mainMatrix[3][0], blendMatrix[3][0], finalWeight);
			testMatrix[3][1] = glm::mix(mainMatrix[3][1], blendMatrix[3][1], finalWeight);
			testMatrix[3][2] = glm::mix(mainMatrix[3][2], blendMatrix[3][2], finalWeight);
			testMatrix[3][3] = glm::mix(mainMatrix[3][3], blendMatrix[3][3], finalWeight);

			if (mainBoneName == "B_Head")
			{
				rotateHeadWithMouse(testMatrix);
			}

			FinalTransformation.push_back(testMatrix);
		}
	}
	else if (this->state == BLENDING_WITH_2)
	{
		//
	}
	else if (this->state == BLENDING_WITH_3)
	{
		//
	}
}

void AnimationController::rotateHeadWithMouse(glm::mat4 & finalTransformation)
{
	float mousePositionX = GelsenizeCore::sharedInstance().mousePositionX;
	float mousePositionY = GelsenizeCore::sharedInstance().mousePositionY;

	int window_Width;
	int window_Height;
	gelCameraManager::sharedInstance().mainCamera->GetWindowSize(window_Width, window_Height);

	float xLeftPercent = mousePositionX / window_Width;
	float yTopPercent = mousePositionY / window_Height;

	bool leftSide = xLeftPercent <= 0.5f;
	bool topSide = yTopPercent <= 0.5;

	const glm::vec3 maxHeadRotation = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 adjustedHeadRotation = maxHeadRotation;

	bool invertControls = false;
	if (invertControls)
	{
		adjustedHeadRotation.z *= -1.0f;
		adjustedHeadRotation.y *= -1.0f;
	}


	//adjust x
	if (leftSide)
	{
		xLeftPercent = 0.5f - xLeftPercent;
		xLeftPercent *= 2; //total contribute when 50% of whole screen represents 100%
	}
	else
	{
		xLeftPercent = xLeftPercent - 0.5f;
		xLeftPercent *= 2; //total contribute when 50% of whole screen represents 100%

		adjustedHeadRotation.z *= -1.0f;
	}

	if (topSide)
	{
		yTopPercent = 0.5f - yTopPercent;
		yTopPercent *= 2; //total contribute when 50% of whole screen represents 100%
	}
	else
	{
		yTopPercent = yTopPercent - 0.5f;
		yTopPercent *= 2; //total contribute when 50% of whole screen represents 100%

		adjustedHeadRotation.y *= -1.0f;
	}

	adjustedHeadRotation.z *= xLeftPercent;
	adjustedHeadRotation.x *= yTopPercent;

	glm::mat4 headTransform = finalTransformation;



	//rotate
	glm::quat headRotateOrientation(adjustedHeadRotation);

	//glm::mat4 headRotateMatrix(headRotateOrientation);
	//headTransform = headTransform * headRotateMatrix; //apply head rotate
	//FinalTransformation[i] = headTransform;

	glm::vec4 translationMain = finalTransformation[3];
	glm::quat rotationMain = glm::normalize(glm::quat_cast(finalTransformation));
	glm::mat4 rotationalMainMat(rotationMain);
	glm::mat4 rotationalMat(headRotateOrientation);

	glm::mat4 mister(1.0f);

	mister = glm::translate(mister, glm::vec3(finalTransformation[3].x, finalTransformation[3].y, finalTransformation[3].z));
	mister *= rotationalMainMat;
	mister *= rotationalMat;

	finalTransformation = mister;
}
