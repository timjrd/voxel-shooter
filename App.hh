#pragma once

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class App : public Ogre::WindowEventListener, public Ogre::FrameListener
{
  private:
   Ogre::Root* Root = nullptr;
   Ogre::RenderWindow* Window = nullptr;
   Ogre::SceneManager* SceneMgr = nullptr;
   Ogre::Camera* Camera = nullptr;

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
};
