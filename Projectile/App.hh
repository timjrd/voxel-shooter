#pragma once

#include "Model.hh"
#include "Projectile.hh"

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class App : public Ogre::WindowEventListener, public Ogre::FrameListener, public VoxelContainer::MeshListener
{
  private:
   VoxelContainer * Voxels = nullptr;
   Ogre::ManualObject ** Meshes = nullptr;
   size_t Width;
   size_t Height;
   size_t Depth;
   size_t MeshSize;

	Projectile *ProjectileArray[20000];
	int fireId = 0;

	Ogre::NodeAnimationTrack* track;
	Ogre::RibbonTrail* trail;
   
   Ogre::Root* Root = nullptr;
   Ogre::RenderWindow* Window = nullptr;
   Ogre::SceneManager* SceneMgr = nullptr;
   Ogre::Camera* Camera = nullptr;

   float PlayerRadius;
   Ogre::SceneNode * PlayerNode = nullptr;
   Ogre::SceneNode * YawNode = nullptr;
   Ogre::SceneNode * AlterNode = nullptr;

	Ogre::SceneNode * rayonNode = nullptr;

   OIS::InputManager* InputManager = nullptr;
   OIS::Mouse* Mouse = nullptr;
   OIS::Keyboard* Keyboard = nullptr;
   
   Ogre::String ResourcesCfg;
   Ogre::String PluginsCfg;

	Ogre::AnimationState* mYellowLightAnimState;

   void windowResized(Ogre::RenderWindow* rw) override;
   void windowClosed(Ogre::RenderWindow* rw)  override;
   bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

	void setupTrailLights();
	
	void UpdateAnimation();
	
   void Translate(Ogre::Vector3);
   
  public:
   App();
   virtual ~App();
 
   bool Go();

   void UpdateMesh(size_t x, size_t y, size_t z, const std::vector<VoxelContainer::Quad> &) override;
};