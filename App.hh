#pragma once

#include "Model.hh"

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class App : public Ogre::WindowEventListener, public Ogre::FrameListener, public Model::Observer
{
  private:
   Model * Model_ = nullptr;
   Ogre::ManualObject ** Meshes = nullptr;
   size_t Width;
   size_t Height;
   size_t Depth;
   size_t MeshSize;
   
   Ogre::Root* Root = nullptr;
   Ogre::RenderWindow* Window = nullptr;
   Ogre::SceneManager* SceneMgr = nullptr;
   Ogre::Camera* Camera = nullptr;
   Ogre::SceneNode * PlayerNode = nullptr;

   Ogre::Vector3 PlayerDirection;
   Ogre::Vector3 PlayerPos;

   OIS::InputManager* InputManager = nullptr;
   OIS::Mouse* Mouse = nullptr;
   OIS::Keyboard* Keyboard = nullptr;
   
   Ogre::String ResourcesCfg;
   Ogre::String PluginsCfg;

   void windowResized(Ogre::RenderWindow* rw) override;
   void windowClosed(Ogre::RenderWindow* rw)  override;
   bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
   
  public:
   App();
   virtual ~App();
 
   bool Go();

   void UpdatePlayer(const Model::Player &) override;
   void UpdateMesh(size_t x, size_t y, size_t z, const std::vector<Model::Quad> &) override;
};
