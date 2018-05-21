#pragma once
#include "eShapeType.h"
#include "shapes.h"

#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	// base class/interface for all shapes
	class iShape
	{
	public:
		virtual ~iShape() {}
		// Use GetShapeType()
		// if this isn't the right shape, it will do nothing
		// and return false
		virtual bool GetSphere(nShapes::sSphere& sphereOut) { return false; }
		virtual bool GetPlane(nShapes::sPlane& planeOut) { return false; }
		virtual bool GetBox(nShapes::sBox& boxOut) { return false;  }


		virtual btCollisionShape* GetShape(void) { return 0; }
		//virtual bool GetShape(btCollisionShape * shapeOut) { return false; }
		// eShapeType
		// what shape am I?
		inline eShapeType GetShapeType() const { return mShapeType; }
	protected:
		// protected constructors
		// so nothing can create this object exept for a subclass
		iShape(eShapeType shapeType) : mShapeType(shapeType) {}
		iShape(const iShape& other) {}
		iShape& operator=(const iShape& other) { return *this; }
	
	private:
		eShapeType mShapeType;
	};
}