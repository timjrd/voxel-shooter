#include "App.hh"
 
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreRenderWindow.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>

#include <iostream>

using namespace Ogre;

App::App()
   : ResourcesCfg(Ogre::StringUtil::BLANK)
   , PluginsCfg(Ogre::StringUtil::BLANK)
{
}
 
App::~App()
{
   Ogre::WindowEventUtilities::removeWindowEventListener(Window, this);
   windowClosed(Window);
   
   for (size_t i=0; i<Width*Height*Depth; i++)
      if (Meshes[i])
         SceneMgr->destroyManualObject(Meshes[i]);
   
   delete [] Meshes;
   delete Root;
   delete Model_;
}

//Adjust mouse clipping area
void App::windowResized(Ogre::RenderWindow* rw)
{
    unsigned int width, height, depth;
    int left, top;
    rw->getMetrics(width, height, depth, left, top);
 
    const OIS::MouseState &ms = Mouse->getMouseState();
    ms.width = width;
    ms.height = height;
}
 
//Unattach OIS before window shutdown (very important under Linux)
void App::windowClosed(Ogre::RenderWindow* rw)
{
    //Only close for window that created OIS (the main window in these demos)
    if(rw == Window)
    {
        if(InputManager)
        {
            InputManager->destroyInputObject( Mouse );
            InputManager->destroyInputObject( Keyboard );
 
            OIS::InputManager::destroyInputSystem(InputManager);
            InputManager = nullptr;
        }
    }
}

bool App::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
    if(Window->isClosed())
        return false;
 
    //Need to capture/update each device
    Keyboard->capture();
    Mouse->capture();
 
    if(Keyboard->isKeyDown(OIS::KC_ESCAPE))
        return false;


    PlayerNode->yaw(Degree(-Mouse->getMouseState().X.rel*0.1), Node::TS_WORLD);
    PlayerNode->pitch(Degree(-Mouse->getMouseState().Y.rel*0.1), Node::TS_LOCAL);
    
    if (Keyboard->isKeyDown(OIS::KC_Z) and not Keyboard->isKeyDown(OIS::KC_S))
       PlayerNode->translate(0,0,-1,Ogre::Node::TS_LOCAL);
    else if (Keyboard->isKeyDown(OIS::KC_S) and not Keyboard->isKeyDown(OIS::KC_Z))
       PlayerNode->translate(0,0,1,Ogre::Node::TS_LOCAL);

    if (Keyboard->isKeyDown(OIS::KC_Q) and not Keyboard->isKeyDown(OIS::KC_D))
       PlayerNode->translate(-1,0,0,Ogre::Node::TS_LOCAL);
    else if (Keyboard->isKeyDown(OIS::KC_D) and not Keyboard->isKeyDown(OIS::KC_Q))
       PlayerNode->translate(1,0,0,Ogre::Node::TS_LOCAL);

 
    return true;
}

bool App::Go()
{
   ResourcesCfg = "resources.cfg";
   PluginsCfg   = "plugins.cfg";
   Root = new Ogre::Root(PluginsCfg);

   MeshSize = 10;
   Width = Height = Depth = 90;
   Model_ = new Model(MeshSize, Width, Height, Depth, this);
   
   Meshes = new Ogre::ManualObject*[Width*Height*Depth];
   for (size_t i=0; i<Width*Height*Depth; i++)
      Meshes[i] = nullptr;
   
   Ogre::ConfigFile cf;
   cf.load(ResourcesCfg);

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

   SceneMgr = Root->createSceneManager(Ogre::ST_GENERIC);

   Camera = SceneMgr->createCamera("PlayerCam");
 
   Camera->setDirection(0, 0, -1);
   Camera->setNearClipDistance(5);

   Ogre::Viewport* vp = Window->addViewport(Camera);
 
   vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
 
   Camera->setAspectRatio(
      Ogre::Real(vp->getActualWidth()) / 
      Ogre::Real(vp->getActualHeight()));


   PlayerNode = SceneMgr->getRootSceneNode()->createChildSceneNode();
   PlayerNode->setPosition(0,0,0);
   PlayerNode->attachObject(Camera);

   PlayerDirection = Ogre::Vector3(0,0,-1);
   PlayerPos = Ogre::Vector3(0,0,0);

   Ogre::Light* light = SceneMgr->createLight("PlayerLight");
   PlayerNode->attachObject(light);

   //Model_->MovePlayer(0,0,0); // Update PlayerNode
   
   Ogre::Entity* ogreEntity = SceneMgr->createEntity("ogrehead.mesh");
 
   Ogre::SceneNode* ogreNode = SceneMgr->getRootSceneNode()->createChildSceneNode();
   ogreNode->attachObject(ogreEntity);
   ogreNode->setPosition(0,0,-40);
 
   SceneMgr->setAmbientLight(Ogre::ColourValue(.3, .3, .3));
   
   Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
   OIS::ParamList pl;
   size_t windowHnd = 0;
   Window->getCustomAttribute("WINDOW", &windowHnd);
   pl.insert(std::make_pair(std::string("WINDOW"), std::to_string(windowHnd)));
 
   InputManager = OIS::InputManager::createInputSystem( pl );
   Keyboard = static_cast<OIS::Keyboard*>(InputManager->createInputObject( OIS::OISKeyboard, false ));
   Mouse = static_cast<OIS::Mouse*>(InputManager->createInputObject( OIS::OISMouse, false ));

   windowResized(Window);
   Ogre::WindowEventUtilities::addWindowEventListener(Window, this);
   
   Root->addFrameListener(this);

   Root->startRendering();
   
   return true;
}

void App::UpdatePlayer(const Model::Player & p)
{
   //PlayerNode->setPosition(p.Pos.X, p.Pos.Y, p.Pos.Z);

   //Model::Vec3 dir = Model::toCartesian(p.Theta,p.Phi);
   //PlayerNode->setDirection(dir.X,dir.Y,dir.Z, Ogre::Node::TransformSpace::TS_WORLD);

   //PlayerNode->setDirection(PlayerDirection, Ogre::Node::TransformSpace::TS_WORLD);
}

void App::UpdateMesh(size_t x, size_t y, size_t z, const std::vector<Model::Quad> &)
{
}



int main()
{
   App app;
   app.Go();
   
   return EXIT_SUCCESS;
}
