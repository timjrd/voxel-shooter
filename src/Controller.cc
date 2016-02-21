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
    Keyboard->capture();
    Mouse->capture();
 
    if(Keyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;

    MyModel->YawPlayer(-Mouse->getMouseState().X.rel*0.0015);
    MyModel->PitchPlayer(-Mouse->getMouseState().Y.rel*0.0015);

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

    MyView->OnFrame(Timer.getMilliseconds() /(float) 1000);
    
    return true;
}
