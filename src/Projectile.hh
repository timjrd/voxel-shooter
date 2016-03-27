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
   
   virtual void Init(Model &, const FixVector3 & pos, const FixVector3 & dir, Fix16 time) = 0;
   virtual void InitView(Ogre::SceneManager &) = 0;
   virtual bool Update(Fix16 time) = 0;
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

   Fix16 FiredAt;

   FixVector3 FiredFrom;
   FixVector3 FiredTo;

   Model * MyModel;

public:
   LaserProjectile(const Ogre::ColourValue &);
   ~LaserProjectile() override;

   void Init(Model &, const FixVector3 & pos, const FixVector3 & dir, Fix16 time) override;
   void InitView(Ogre::SceneManager &) override;
};


class FastProjectile : public LaserProjectile
{
public:
   FastProjectile();
   bool Update(Fix16 time) override;
};
 

