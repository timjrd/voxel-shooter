#include "App.hh"
#include "utils.hh"

#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreRenderWindow.h>
#include <OgreWindowEventUtilities.h>

#include <iostream>
#include <ctime>

using namespace std;
using namespace Ogre;

App::App()
{}
 
App::~App()
{
   Ogre::WindowEventUtilities::removeWindowEventListener(Window, &MyController);
   
   InputManager->destroyInputObject( Mouse );
   InputManager->destroyInputObject( Keyboard );
 
   OIS::InputManager::destroyInputSystem(InputManager);

   delete Root;
}


bool App::Init(const string & pluginsFile, const string & resourcesFile)
{
   Root = new Ogre::Root(pluginsFile);

   Ogre::ConfigFile cf;
   cf.load(resourcesFile);

   Ogre::String name, locType;
   Ogre::ConfigFile::SectionIterator secIt = cf.getSectionIterator();
   while (secIt.hasMoreElements())
   {
      Ogre::ConfigFile::SettingsMultiMap* settings = secIt.getNext();
      Ogre::ConfigFile::SettingsMultiMap::iterator it;
      for (it = settings->begin(); it != settings->end(); ++it)
      {
         locType = it->first;
         name = it->second;
         Ogre::ResourceGroupManager::getSingleton().addResourceLocation(name, locType);
      }
   }

   if(!(Root->restoreConfig() || Root->showConfigDialog()))
      return false;

   Window = Root->initialise(true, "voxel-shooter");

   Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
   Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();

   Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
   OIS::ParamList pl;
   size_t windowHnd = 0;
   Window->getCustomAttribute("WINDOW", &windowHnd);
   pl.insert(std::make_pair(std::string("WINDOW"), std::to_string(windowHnd)));

   // désactiver la capture (grab) sous X11
   //pl.insert(std::make_pair(std::string("x11_mouse_grab"), std::string("false")));
   pl.insert(std::make_pair(std::string("x11_keyboard_grab"), std::string("false")));
 
   InputManager = OIS::InputManager::createInputSystem( pl );
   Keyboard = static_cast<OIS::Keyboard*>(InputManager->createInputObject( OIS::OISKeyboard, false ));
   Mouse = static_cast<OIS::Mouse*>(InputManager->createInputObject( OIS::OISMouse, false ));


   MyView.Init(*Root, *Window);
   MyController.Init(*Mouse, *Keyboard);
   MyController.windowResized(Window); // init

   MyModel.SetObserver(&MyView);
   MyView.SetModel(MyModel);
   MyController.SetModel(MyModel);
   MyController.SetView(MyView);

   Ogre::WindowEventUtilities::addWindowEventListener(Window, &MyController);
   Root->addFrameListener(&MyController);

   Mouse->setBuffered(true);
   Mouse->setEventCallback(&MyController);
   //Keyboard->setEventCallback(&MyController);

   //MyModel.GenerateCave(30, 300, 300, 300, time(NULL));
   MyModel.GenerateMengerSponge(150, 10, 4);

   return true;
}

void App::Start()
{
   Root->startRendering();
}
