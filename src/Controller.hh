#pragma once

#include "fix16.hpp"

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class Model;
class View;

class Controller : public Ogre::WindowEventListener, public Ogre::FrameListener, public OIS::MouseListener
{
private:
   Model * MyModel;
   View  * MyView;
   
   OIS::Mouse    * Mouse;
   OIS::Keyboard * Keyboard;

   Ogre::Timer Timer;
   Fix16       Time = 0;
   Fix16       LastFrameAt = 0;
   
public:
   void Init(OIS::Mouse &, OIS::Keyboard &);
   void SetModel(Model &);
   void SetView(View &);
   
   void windowResized(Ogre::RenderWindow* rw) override;
   bool frameRenderingQueued(const Ogre::FrameEvent& evt) override;

   bool mouseMoved(const OIS::MouseEvent& me) override;
   bool mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id) override;
   bool mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id) override;
};
