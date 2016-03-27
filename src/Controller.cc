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
   // on décompose pour pas overflow
   unsigned long t = Timer.getMilliseconds();
   int s  = t / 1000;
   int ms = t % 1000;
   Time = Fix16(s) + Fix16::div(ms,1000);

   Fix16 timeSinceLastFrame = Time - LastFrameAt;
   
   Keyboard->capture();
   Mouse->capture();
 
   if(Keyboard->isKeyDown(OIS::KC_ESCAPE))
      return false;

   FixVector3 translate(0,0,0);
    
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

   if(Keyboard->isKeyDown(OIS::KC_R))
       MyModel->RechargeArmePlayer();

   translate.normalise();
   MyModel->TranslatePlayer(translate * timeSinceLastFrame * 50);

    
   MyModel->Tick(Time);
   MyView->OnFrame(Time.toFloat());

   LastFrameAt = Time;
   return true;
}

bool Controller::mouseMoved(const OIS::MouseEvent& me)
{
   MyModel->YawPlayer(Fix16::div(15,10000) * -me.state.X.rel);
   MyModel->PitchPlayer(Fix16::div(15,10000) * -me.state.Y.rel);

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

