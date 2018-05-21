#pragma once

#include <glm/vec3.hpp>

namespace nPhysics
{
	namespace nShapes
	{
		/*maybe these should be in a glm extension folder?*/

		struct sSphere
		{
			sSphere() : Radius(1.f) {}
			sSphere(float radius) : Radius(radius) {}
			float Radius;
		};
		struct sPlane
		{
			sPlane() : Normal(0.f, 1.f, 0.f) {}
			sPlane(const glm::vec3& normal) : Normal(normal) {}
			sPlane(float nX, float nY, float nZ) : Normal(nX, nY, nZ) {}
			//glm::vec3 Position; // the body will have the position
			glm::vec3 Normal;
			//float DotProduct;  // maybe...
		};
		struct sBox
		{
			sBox() : HalfWidth(0.5f, 0.5f, 0.5f) {}
			sBox(float halfWidths) : HalfWidth(halfWidths, halfWidths, halfWidths) {}
			sBox(float hwX, float hwY, float hwZ) : HalfWidth(hwX, hwY, hwZ) {}
			glm::vec3 HalfWidth;
		};
		struct sCapsule
		{
			sCapsule() : width(1.0f), height(2.0f) {}
			sCapsule(float width_, float height_) : width(width_), height(height_) {}
			float width;
			float height;
		};
		struct sCylinder
		{
			sCylinder() : radius(1.0f), height(1.0f) {}
			sCylinder(float radius_, float height_) : radius(radius_), height(height_) {}
			float radius;
			float height;
		};
	}
}