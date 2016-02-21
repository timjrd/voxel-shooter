#pragma once

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreVector3.h>
#include <OgreBillboardSet.h>
#include <OgreString.h>

class Model;

// Projectile interface
class Projectile
{
public:
   virtual ~Projectile() {};
   
   virtual void Init(Model &, const Ogre::Vector3 & pos, const Ogre::Vector3 & dir, float time) = 0;
   virtual void InitView(Ogre::SceneManager &) = 0;
   virtual bool Update(float time) = 0;
};

// Base projectile class
class LaserProjectile : public Projectile
{
protected:
   Ogre::SceneManager * SceneManager;
   Ogre::SceneNode    * Node;
   Ogre::RibbonTrail  * Trail;
   Ogre::Light        * Light;

   Ogre::ColourValue Colour;

   //Ogre::BillboardSet *projectileBbs;

   float FiredAt;

   Ogre::Vector3 FiredFrom;
   Ogre::Vector3 FiredTo;

   Model * MyModel;

public:
   LaserProjectile(const Ogre::ColourValue &);
   ~LaserProjectile() override;

   void Init(Model &, const Ogre::Vector3 & pos, const Ogre::Vector3 & dir, float time) override;
   void InitView(Ogre::SceneManager &) override;
};


class FastProjectile : public LaserProjectile
{
public:
   FastProjectile();
   bool Update(float time) override;
};
 

