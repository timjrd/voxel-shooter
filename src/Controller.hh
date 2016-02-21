#pragma once

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class Model;
class View;

class Controller : public Ogre::WindowEventListener, public Ogre::FrameListener
{
private:
   Model * MyModel;
   View  * MyView;
   
   OIS::Mouse    * Mouse;
   OIS::Keyboard * Keyboard;

   Ogre::Timer Timer;
   
public:
   void Init(OIS::Mouse &, OIS::Keyboard &);
   void SetModel(Model &);
   void SetView(View &);
   
   void windowResized(Ogre::RenderWindow* rw) override;
   bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;
};
