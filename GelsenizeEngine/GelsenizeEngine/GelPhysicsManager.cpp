#include "GelPhysicsManager.h"

#include "OCHelper.h"

void GelPhysicsManager::Update(float deltaTime)
{
	this->world->TimeStep(deltaTime);
}

void collisionCallback(iGameObject * object1, iGameObject * object2);

void GelPhysicsManager::Setup(void)
{
	//Setup physics world
	factory = new nPhysics::bulletPhysicsFactory();
	//g_myPhysicsFactory = new nPhysics::cPhysicsFactory();



	world = factory->CreatePhysicsWorld();
	world->RegisterCollisionListener(collisionCallback);
}


void collisionCallback(iGameObject* object1, iGameObject* object2)
{
	return;

	std::string mesh1 = object1->GetMesh().name;
	std::string mesh2 = object2->GetMesh().name;

	if (mesh1 == mesh2 && mesh1 == "cube")
	{
		RenderInfo render1 = object1->GetRender();
		RenderInfo render2 = object2->GetRender();

		/*if (render1.color == render2.color && render1.color != glm::vec4(1.0f,0.0f,0.0f,1.0f))
		{
		return;
		}*/

		glm::vec4 randomColor;
		randomColor.x = OCHelper::getRandFloat(0.5f, 1.0f);
		randomColor.y = OCHelper::getRandFloat(0.5f, 1.0f);
		randomColor.z = OCHelper::getRandFloat(0.5f, 1.0f);
		randomColor.a = 1.0f;

		render1.color = randomColor;
		render2.color = randomColor;

		object1->SetRender(render1);
		object2->SetRender(render2);
		return;
	}

	if (mesh1 == mesh2 && mesh1 == "cylinder")
	{
		RenderInfo render1 = object1->GetRender();
		RenderInfo render2 = object2->GetRender();

		/*if (render1.color == render2.color && render1.color != glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
		{
		return;
		}*/

		glm::vec4 randomColor;
		randomColor.x = OCHelper::getRandFloat(0.0f, 0.5f);
		randomColor.y = OCHelper::getRandFloat(0.0f, 0.5f);
		randomColor.z = OCHelper::getRandFloat(0.0f, 0.5f);
		randomColor.a = 1.0f;

		render1.color = randomColor;
		render2.color = randomColor;

		object1->SetRender(render1);
		object2->SetRender(render2);
		return;
	}
}
