#pragma once
#include "iShape.h"
#include "shapes.h"

#include <btBulletDynamicsCommon.h>

namespace nPhysics
{
	class cSphereShape : public iShape
	{
	public:
		cSphereShape(float radius) : iShape(SPHERE_SHAPE), mSphere(radius) {};
		cSphereShape(const nShapes::sSphere& sphere) : iShape(SPHERE_SHAPE), mSphere(sphere) {};
		~cSphereShape() {};
		
		virtual bool GetSphere(nShapes::sSphere& sphereOut)
		{
			sphereOut = mSphere;
			return true;
		}

	private:
		nShapes::sSphere mSphere;
	};

	class cPlaneShape : public iShape
	{
	public:
		cPlaneShape(glm::vec3 normal) : iShape(PLANE_SHAPE), mPlane(normal) {};
		cPlaneShape(const nShapes::sPlane& plane) : iShape(PLANE_SHAPE), mPlane(plane) {};
		~cPlaneShape() {};

		virtual bool GetPlane(nShapes::sPlane& planeOut)
		{
			planeOut = mPlane;
			return true;
		}

	private:
		nShapes::sPlane mPlane;
	};



	class bulletSphereShape : public iShape
	{
	public:
		bulletSphereShape(float radius) : iShape(SPHERE_SHAPE) {
			mSphere = new btSphereShape(radius);
		};
		bulletSphereShape(const nShapes::sSphere& sphere) : iShape(SPHERE_SHAPE) {
			mSphere = new btSphereShape(sphere.Radius /2.0f);
		};
		~bulletSphereShape() {};

		/*virtual bool GetSphere(btCollisionShape* sphereOut)
		{
			sphereOut = mSphere;
			return true;
		}*/
		virtual btCollisionShape* GetShape(void)
		{
			return mSphere;
		}

	private:
		btCollisionShape *mSphere;
	};

	class bulletPlaneShape : public iShape
	{
	public:
		bulletPlaneShape(glm::vec3 normal) : iShape(PLANE_SHAPE) {
			mPlane = new btStaticPlaneShape(btVector3(normal.x, normal.y, normal.z), 0.f);
		};
		bulletPlaneShape(const nShapes::sPlane& plane) : iShape(PLANE_SHAPE){
			mPlane = new btStaticPlaneShape(btVector3(plane.Normal.x, plane.Normal.y, plane.Normal.z), 0.f);
		};
		~bulletPlaneShape() {};

		/*virtual bool GetPlane(btCollisionShape * planeOut)
		{
			planeOut = mPlane;
			return true;
		}
*/
		virtual btCollisionShape* GetShape(void)
		{
			return mPlane;
		}

	private:
		btCollisionShape *mPlane;
	};


	class bulletBoxShape : public iShape
	{
	public:
		bulletBoxShape(glm::vec3 halfWidth) : iShape(BOX_SHAPE) {



			mBox = new btBoxShape(btVector3(halfWidth.x, halfWidth.y, halfWidth.z));
		};
		bulletBoxShape(const nShapes::sBox& box) : iShape(BOX_SHAPE) {
			mBox = new btBoxShape(btVector3(box.HalfWidth.x, box.HalfWidth.y, box.HalfWidth.z));
		};
		~bulletBoxShape() {};

		virtual btCollisionShape* GetShape(void)
		{
			return mBox;
		}

	private:
		btCollisionShape *mBox;
	};


	class bulletCapsuleShape : public iShape
	{
	public:
		bulletCapsuleShape(float width, float height) : iShape(CAPSULE_SHAPE) {
			mCapsule = new btCapsuleShape(width / 2.0f, height);
		};
		~bulletCapsuleShape() {};

		virtual btCollisionShape* GetShape(void)
		{
			return mCapsule;
		}

	private:
		btCollisionShape *mCapsule;
	};

	class bulletCylinderShape : public iShape
	{
	public:
		bulletCylinderShape(float radius, float height) : iShape(CYLINDER_SHAPE) {
			mCylinder = new btCylinderShape(btVector3(radius, height/2.0f, radius));
		};
		~bulletCylinderShape() {};

		virtual btCollisionShape* GetShape(void)
		{
			return mCylinder;
		}

	private:
		btCollisionShape *mCylinder;
	};
	
}