#include "Model.hh"
#include "Projectile.hh"
#include "utils.hh"

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreString.h>
#include <OgreBillboardSet.h>
#include <OgreEntity.h>
#include <OgreRibbonTrail.h>

LaserProjectile::LaserProjectile(const Ogre::ColourValue & colour)
   : Colour(colour)
{}

void LaserProjectile::Init(Model & model, const Ogre::Vector3 & pos, const Ogre::Vector3 & dir, float time)
{
   MyModel   = &model;
   FiredFrom = pos;
   FiredTo   = dir;
   FiredAt   = time;
}

void LaserProjectile::InitView(Ogre::SceneManager & sceneManager)
{
   SceneManager = &sceneManager;

   /*
   // BillBoard
   projectileBbs = sceneMgr->createBillboardSet();
   projectileBbs->setDefaultDimensions(2, 2);
   projectileBbs->setMaterialName("Examples/Flare");
   projectileBbs->createBillboard(0,0,0, Ogre::ColourValue::Green);
   */
   
   Node = SceneManager->getRootSceneNode()->createChildSceneNode(FiredFrom);
   Node->setDirection(FiredTo);

   //projectileNode->attachObject(projectileBbs);

   Trail = SceneManager->createRibbonTrail();
   Trail->setMaterialName("Examples/LightRibbonTrail");
   Trail->setTrailLength(40);
   Trail->setInitialColour(0, Colour);
   Trail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
   Trail->setMaxChainElements(20);
   Trail->setInitialWidth(0, 0.5);
   Trail->addNode(Node);
   SceneManager->getRootSceneNode()->attachObject(Trail);

   Light = SceneManager->createLight();
   Light->setDiffuseColour(Colour);
   setLightAttenuation(*Light, 40);
   Node->attachObject(Light);
}

LaserProjectile::~LaserProjectile() 
{
   //sceneManager->destroyBillboardSet(projectileBbs);
   SceneManager->destroyRibbonTrail(Trail);
   SceneManager->destroyLight(Light);
   SceneManager->destroySceneNode(Node);
}


//-----------------------------------------------------
// FastProjectile

FastProjectile::FastProjectile()
   : LaserProjectile(Ogre::ColourValue(0.1,0.1,1))
{}

bool FastProjectile::Update(float time)
{
   const float velocity = 450;
   Ogre::Vector3 pos = FiredFrom + FiredTo * (time - FiredAt)*velocity;

   Node->setPosition(pos);
   
   if (MyModel->PointIntersects(pos))
   {
      MyModel->SetSphere(pos.x, pos.y, pos.z, 5, false);
      return false;
   }
   else
      return true;
}



