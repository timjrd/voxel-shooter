#include "Projectile.hh"
#include "utils.hh"

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreString.h>
#include <OgreBillboardSet.h>
#include <OgreEntity.h>
#include <OgreRibbonTrail.h>
/*
Projectile::Projectile(Ogre::Vector3 playerPosition,  Ogre::Quaternion orientation, Ogre::SceneManager *sceneMgr,
                       Ogre::Real projectileId)
{	
	std::stringstream nomComplet;
	nomComplet << "Projectile" << projectileId;
	uniqueName = nomComplet.str(); 
	this->sceneMgr = sceneMgr;
	
	// BillBoard
	projectileBbs = sceneMgr->createBillboardSet();
        projectileBbs->setDefaultDimensions(2, 2);
	projectileBbs->setMaterialName("Examples/Flare");
        projectileBbs->createBillboard(0,0,0, Ogre::ColourValue::Green);

	time = 0;
	projectileVelocity = 500.f;

	projectileNode = sceneMgr->getRootSceneNode()->createChildSceneNode(playerPosition);
	projectileNode->setOrientation(orientation);

	//projectileNode->attachObject(projectileBbs);

        Trail = sceneMgr->createRibbonTrail();
        Trail->setMaterialName("Examples/LightRibbonTrail");
        Trail->setTrailLength(50);
        Trail->setInitialColour(0, 0.0, 1.0, 0.0);
        Trail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
        Trail->setMaxChainElements(20);
        Trail->setInitialWidth(0, 0.5);
        Trail->addNode(projectileNode);
        sceneMgr->getRootSceneNode()->attachObject(Trail);


        Light = sceneMgr->createLight();
        Light->setDiffuseColour(0, 1, 0);
        setLightAttenuation(*Light, 40);
        projectileNode->attachObject(Light);
}

Projectile::~Projectile() 
{
	sceneMgr->destroyBillboardSet(projectileBbs);
        sceneMgr->destroyRibbonTrail(Trail);
	sceneMgr->destroySceneNode(projectileNode);
        sceneMgr->destroyLight(Light);
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

*/




