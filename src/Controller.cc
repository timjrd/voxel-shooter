#include "Controller.hh"

#include "Model.hh"
#include "View.hh"

#include <OgreRenderWindow.h>

void Controller::Init(OIS::Mouse & mouse, OIS::Keyboard & keyboard)
{
   Mouse    = &mouse;
   Keyboard = &keyboard;
}

void Controller::SetModel(Model & model)
{
   MyModel = &model;
}

void Controller::SetView(View & view)
{
   MyView = &view;
}


//Adjust mouse clipping area
void Controller::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    const OIS::MouseState &ms = Mouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

bool Controller::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
   Time = Timer.getMilliseconds() /(float) 1000;
   
   Keyboard->capture();
   Mouse->capture();
 
   if(Keyboard->isKeyDown(OIS::KC_ESCAPE))
      return false;

   Ogre::Vector3 translate(0,0,0);
    
   if (Keyboard->isKeyDown(OIS::KC_Z) and not Keyboard->isKeyDown(OIS::KC_S))
      translate.z = -1;
   else if (Keyboard->isKeyDown(OIS::KC_S) and not Keyboard->isKeyDown(OIS::KC_Z))
      translate.z = 1;

   if (Keyboard->isKeyDown(OIS::KC_Q) and not Keyboard->isKeyDown(OIS::KC_D))
      translate.x = -1;
   else if (Keyboard->isKeyDown(OIS::KC_D) and not Keyboard->isKeyDown(OIS::KC_Q))
      translate.x = 1;

   if (Keyboard->isKeyDown(OIS::KC_SPACE) and not Keyboard->isKeyDown(OIS::KC_LSHIFT))
      translate.y = 1;
   else if (Keyboard->isKeyDown(OIS::KC_LSHIFT) and not Keyboard->isKeyDown(OIS::KC_SPACE))
      translate.y = -1;

   translate.normalise();
   MyModel->TranslatePlayer(translate * evt.timeSinceLastFrame * 40);

    
   MyModel->Tick(Time);
   MyView->OnFrame(Time);
    
   return true;
}

bool Controller::mouseMoved(const OIS::MouseEvent& me)
{
   MyModel->YawPlayer(-me.state.X.rel*0.0015);
   MyModel->PitchPlayer(-me.state.Y.rel*0.0015);

   return true;
}

bool Controller::mousePressed(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{
   if (id == OIS::MB_Left)
      MyModel->Fire(new FastProjectile(), Time, true);
   else if (id == OIS::MB_Right)
      MyModel->Fire(new FastProjectile(), Time, false);

   return true;
}

bool Controller::mouseReleased(const OIS::MouseEvent& me, OIS::MouseButtonID id)
{

   return true;
}

