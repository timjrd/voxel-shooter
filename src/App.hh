#pragma once

#include "Model.hh"
#include "View.hh"
#include "Controller.hh"

#include <OgreRoot.h>
#include <OgreWindowEventUtilities.h>

#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

class App
{
private:
   Model      MyModel;
   View       MyView;
   Controller MyController;
   
   Ogre::Root         * Root         = nullptr;
   Ogre::RenderWindow * Window       = nullptr;
   OIS::InputManager  * InputManager = nullptr;
   OIS::Mouse         * Mouse        = nullptr;
   OIS::Keyboard      * Keyboard     = nullptr;

public:
   App();
   ~App();
 
   bool Init(const std::string & pluginsFile = "plugins.cfg", const std::string & resourcesFile = "resources.cfg");
   void Start();
};
