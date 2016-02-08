#include "Projectile.hh"
#include "utils.hh"

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreString.h>
#include <OgreBillboardSet.h>
#include <OgreEntity.h>
#include <OgreRibbonTrail.h>

Projectile::Projectile(Ogre::Vector3 playerPosition,  Ogre::Quaternion orientation, Ogre::SceneManager *sceneMgr,
                       Ogre::Real projectileId)
{	
	std::stringstream nomComplet;
	nomComplet << "Projectile" << projectileId;
	uniqueName = nomComplet.str(); 
	this->sceneMgr = sceneMgr;
	
	// BillBoard
	projectileBbs = sceneMgr->createBillboardSet(uniqueName);
    projectileBbs->setDefaultDimensions(2, 2);
	projectileBbs->setMaterialName("Examples/Flare");
    projectileBbs->createBillboard(0,0,0, Ogre::ColourValue::Green);

	time = 0;
	projectileVelocity = 500.f;

	projectileNode = sceneMgr->getRootSceneNode()->createChildSceneNode(uniqueName, playerPosition);
	projectileNode->setOrientation(orientation);

	//projectileNode->attachObject(projectileBbs);

    Ogre::RibbonTrail *mTrail = sceneMgr->createRibbonTrail(uniqueName);
    mTrail->setMaterialName("Examples/LightRibbonTrail");
    mTrail->setTrailLength(50);
    mTrail->setInitialColour(0, 0.0, 1.0, 0.0);
    mTrail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
    mTrail->setMaxChainElements(20);
    mTrail->setInitialWidth(0, 0.5);
    mTrail->addNode(projectileNode);
    sceneMgr->getRootSceneNode()->attachObject(mTrail);


    Ogre::Light* light = sceneMgr->createLight();
    light->setDiffuseColour(0, 1, 0);
    setLightAttenuation(*light, 50);
    projectileNode->attachObject(light);
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

    projectileNode->setPosition(projectileNode->getPosition() + projectileNode->getOrientation() *
                                            Ogre::Vector3(0.0f,0.0f,-1.0f * projectileVelocity * timeSinceLastFrame));

	return true;

}






