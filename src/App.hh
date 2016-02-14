#pragma once

#include "VoxelContainer.hh"
#include "Projectile.hh"

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <CEGUI/CEGUI.h>

#include <vector>
#include <tuple>

class App : public Ogre::WindowEventListener, public Ogre::FrameListener, public VoxelContainer::MeshListener
{
  private:
   VoxelContainer Voxels;
   std::vector<std::pair<Ogre::SceneNode*, Ogre::ManualObject*>> Meshes;
   size_t Width;
   size_t Height;
   size_t Depth;
   size_t MeshSize;

	CEGUI::Window *fpsViewer;
   
   Projectile *ProjectileArray[20000];
   int fireId = 0;
   Ogre::Timer LastShot;

   Ogre::Root* Root = nullptr;
   Ogre::RenderWindow* Window = nullptr;
   Ogre::SceneManager* SceneMgr = nullptr;
   Ogre::Camera* Camera = nullptr;

   int NbFrame = 0;
   float FrameDurationSum = 0;

   int NbQuads = 0;
   
   float PlayerRadius;
   Ogre::SceneNode * PlayerNode = nullptr;
   Ogre::SceneNode * YawNode = nullptr;
   Ogre::SceneNode * AlterNode = nullptr;

   OIS::InputManager* InputManager = nullptr;
   OIS::Mouse* Mouse = nullptr;
   OIS::Keyboard* Keyboard = nullptr;
   
   Ogre::String ResourcesCfg;
   Ogre::String PluginsCfg;

   void windowResized(Ogre::RenderWindow* rw) override;
   void windowClosed(Ogre::RenderWindow* rw)  override;
   bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

  
   void Translate(Ogre::Vector3);
   
  public:
   App();
   virtual ~App();
 
   bool Go();

   void UpdateMesh(size_t x, size_t y, size_t z, const std::vector<VoxelContainer::Quad> &) override;
};
