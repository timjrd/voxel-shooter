
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreString.h>
#include <OgreBillboardSet.h>
#include <OgreEntity.h>

#include "Projectile.hh"

Projectile::Projectile(Ogre::Vector3 playerPosition,  Ogre::Quaternion orientation, Ogre::SceneManager *sceneMgr, Ogre::Real projectileId)
{	
	std::stringstream nomComplet;
	nomComplet << "Projectile" << projectileId;
	uniqueName = nomComplet.str(); 
	this->sceneMgr = sceneMgr;
	
	// BillBoard
	projectileBbs = sceneMgr->createBillboardSet(uniqueName);
	projectileBbs->setDefaultDimensions(5, 5);
	projectileBbs->setMaterialName("Examples/Flare");
	projectileBbs->createBillboard(0,0,0, Ogre::ColourValue::White);

	time = 0;
	projectileVelocity = 500.f;

	projectileNode = sceneMgr->getRootSceneNode()->createChildSceneNode(uniqueName, playerPosition);
	projectileNode->setOrientation(orientation);

	projectileNode->attachObject(projectileBbs);

}

Projectile::~Projectile() 
{
	projectileBbs = nullptr;
	projectileNode = nullptr;
	sceneMgr->destroyBillboardSet(uniqueName);
	sceneMgr->destroySceneNode(uniqueName);
	sceneMgr = nullptr;
}

bool Projectile::Update(float timeSinceLastFrame)
{
	time += timeSinceLastFrame;
	
	if(time > 5) 
		return false;

	projectileNode->setPosition(projectileNode->getPosition() + projectileNode->getOrientation() * Ogre::Vector3(0.0f,0.0f,-1.0f * projectileVelocity * timeSinceLastFrame));

	return true;

}






