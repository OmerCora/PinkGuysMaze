#pragma once
#include "eBodyType.h"

namespace nPhysics
{
	// base class/interface for all bodies
	class iCollisionBody
	{
	public:
		virtual ~iCollisionBody() {}

		// eBodyType
		// what kind of body am I? rigid? soft?
		inline eBodyType GetBodyType() const { return mBodyType; }

		virtual void GetSpecialScale(glm::vec3& specialScale) = 0;

	protected:
		// protected constructors
		// so nothing can create this object except for a subclass
		iCollisionBody(eBodyType bodyType) : mBodyType(bodyType) {}
		iCollisionBody(const iCollisionBody& other) {}
		iCollisionBody& operator=(const iCollisionBody& other) { return *this; }
	private:
		eBodyType mBodyType;
	};
}