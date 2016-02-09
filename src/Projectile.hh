#ifndef PROJECTILE_H_
#define PROJECTILE_H_

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreBillboardSet.h>
#include <OgreString.h>

class Projectile 
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


#endif


 

