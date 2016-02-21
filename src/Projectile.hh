#pragma once

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreBillboardSet.h>
#include <OgreString.h>

class Model;

class Projectile
{
public:
   virtual ~Projectile();
   
   virtual void Init(Model &, const Ogre::Vector3 & pos, const Ogre::Vector3 & dir) = 0;
   virtual void InitView(Ogre::SceneManager &) = 0;
   virtual bool Update(float time) = 0;
};

/*
class LaserProjectile : public Projectile
{

private:

	Ogre::SceneNode *projectileNode;
	Ogre::BillboardSet *projectileBbs;
        Ogre::RibbonTrail * Trail;
        Ogre::Light * Light;
	Ogre::SceneManager *sceneMgr;
	Ogre::Real projectileVelocity;
	Ogre::String uniqueName;
	float time;

public:
        Projectile(Ogre::Vector3 playerPosition, Ogre::Quaternion orientation, Ogre::SceneManager *sceneMgr, Ogre::Real projectileId);
	~Projectile();

	bool Update(float timeSinceLastFrame);
};
*/


 

