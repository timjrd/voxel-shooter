#include "App.hh"
 
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreRenderWindow.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgreManualObject.h>

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
   
   //for (size_t i=0; i<Width*Height*Depth; i++)
      //if (Meshes[i])
         //SceneMgr->destroyManualObject(Meshes[i]);
   
   delete [] Meshes;
   delete Root;
   delete Voxels;
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


    YawNode->yaw(Degree(-Mouse->getMouseState().X.rel*0.1), Node::TS_LOCAL);

    const Radian d = Degree(-Mouse->getMouseState().Y.rel*0.1);
    const Radian newPitch = Ogre::Math::Abs(PlayerNode->getOrientation().getPitch() + d);
    if(newPitch <= Ogre::Radian(Ogre::Math::PI/2))
       PlayerNode->pitch(d, Node::TS_LOCAL);


    if (Mouse->getMouseState().buttonDown(OIS::MB_Left))
       Voxels->SetSphere(AlterNode->_getDerivedPosition(), 6, true);
    else if (Mouse->getMouseState().buttonDown(OIS::MB_Right))
       Voxels->SetSphere(AlterNode->_getDerivedPosition(), 6, false);
    

    Vector3 translate(0,0,0);
    const float speed = 1;
    
    if (Keyboard->isKeyDown(OIS::KC_Z) and not Keyboard->isKeyDown(OIS::KC_S))
       translate.z = -speed;
    else if (Keyboard->isKeyDown(OIS::KC_S) and not Keyboard->isKeyDown(OIS::KC_Z))
       translate.z = speed;

    if (Keyboard->isKeyDown(OIS::KC_Q) and not Keyboard->isKeyDown(OIS::KC_D))
       translate.x = -speed;
    else if (Keyboard->isKeyDown(OIS::KC_D) and not Keyboard->isKeyDown(OIS::KC_Q))
       translate.x = speed;

    if (Keyboard->isKeyDown(OIS::KC_SPACE) and not Keyboard->isKeyDown(OIS::KC_LSHIFT))
       translate.y = speed;
    else if (Keyboard->isKeyDown(OIS::KC_LSHIFT) and not Keyboard->isKeyDown(OIS::KC_SPACE))
       translate.y = -speed;

    Translate(translate);
    
    return true;
}

void App::Translate(Vector3 dir)
{
   const Vector3 d(PlayerRadius, PlayerRadius, PlayerRadius);

   const Vector3 t = YawNode->getOrientation() * PlayerNode->getOrientation() * dir;

   Vector3 pos = PlayerNode->_getDerivedPosition();

   //YawNode->setPosition(pos+t); return;

   const Vector3 x(t.x,0,0);
   const Vector3 y(0,t.y,0);
   const Vector3 z(0,0,t.z);

   if (not Voxels->BoxIntersects(pos+z - d, pos+z + d))
      pos += z;
   
   if (not Voxels->BoxIntersects(pos+x - d, pos+x + d))
      pos += x;
   
   if (not Voxels->BoxIntersects(pos+y - d, pos+y + d))
      pos += y;

   YawNode->setPosition(pos);

   /*
   // inutile...
     
   const Vector3 ___ = pos;
   const Vector3 __z = pos + Vector3(0  , 0  , t.z);
   const Vector3 _y_ = pos + Vector3(0  , t.y, 0  );
   const Vector3 _yz = pos + Vector3(0  , t.y, t.z);
   const Vector3 x__ = pos + Vector3(t.x, 0  , 0  );
   const Vector3 x_z = pos + Vector3(t.x, 0  , t.z);
   const Vector3 xy_ = pos + Vector3(t.x, t.y, 0  );
   const Vector3 xyz = pos + t;
   
   if (not Voxels->BoxIntersects(xyz - d, xyz + d))
      YawNode->setPosition(xyz);

   else if (not Voxels->BoxIntersects(x_z - d, x_z + d))
      YawNode->setPosition(x_z);

   else if (not Voxels->BoxIntersects(_yz - d, _yz + d))
      YawNode->setPosition(_yz);

   else if (not Voxels->BoxIntersects(xy_ - d, xy_ + d))
      YawNode->setPosition(xy_);

   else if (not Voxels->BoxIntersects(__z - d, __z + d))
      YawNode->setPosition(__z);

   else if (not Voxels->BoxIntersects(x__ - d, x__ + d))
      YawNode->setPosition(x__);

   else if (not Voxels->BoxIntersects(_y_ - d, _y_ + d))
      YawNode->setPosition(_y_);
   */
}


bool App::Go()
{
   ResourcesCfg = "resources.cfg";
   PluginsCfg   = "plugins.cfg";
   Root = new Ogre::Root(PluginsCfg);

   PlayerRadius = 3;
   
   MeshSize = 30;
   Width = Height = Depth = 10;
   Voxels = new VoxelContainer(MeshSize, Width, Height, Depth, this);

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
   Camera->setNearClipDistance(0.1);

   Ogre::Viewport* vp = Window->addViewport(Camera);
 
   vp->setBackgroundColour(Ogre::ColourValue(0,0,0));
 
   Camera->setAspectRatio(
      Ogre::Real(vp->getActualWidth()) / 
      Ogre::Real(vp->getActualHeight()));


   YawNode = SceneMgr->getRootSceneNode()->createChildSceneNode();

   PlayerNode = YawNode->createChildSceneNode();
   PlayerNode->attachObject(Camera);

   AlterNode = PlayerNode->createChildSceneNode();
   AlterNode->setPosition(0,0,-100);
   
   const float w = Width * MeshSize;
   const float h = Height * MeshSize;
   const float d = Depth * MeshSize;
   ManualObject* borders = SceneMgr->createManualObject("Borders");
   borders->begin("Ogre/Skin", RenderOperation::OT_TRIANGLE_LIST);

   borders->position(0,0,0);
   borders->normal(1,0,0);

   borders->position(0,h,0);
   borders->normal(1,0,0);

   borders->position(0,h,d);
   borders->normal(1,0,0);

   borders->position(0,0,d);
   borders->normal(1,0,0);

   borders->index(0);
   borders->index(1);
   borders->index(2);
   
   borders->index(2);
   borders->index(3);
   borders->index(0);

   // --
   
   borders->position(0,0,0);
   borders->normal(0,1,0);

   borders->position(0,0,d);
   borders->normal(0,1,0);

   borders->position(w,0,d);
   borders->normal(0,1,0);

   borders->position(w,0,0);
   borders->normal(0,1,0);

   borders->index(4);
   borders->index(5);
   borders->index(6);
   
   borders->index(6);
   borders->index(7);
   borders->index(4);

   // --
   
   borders->position(w,0,0);
   borders->normal(-1,0,0);

   borders->position(w,0,d);
   borders->normal(-1,0,0);

   borders->position(w,h,d);
   borders->normal(-1,0,0);

   borders->position(w,h,0);
   borders->normal(-1,0,0);

   borders->index(8);
   borders->index(9);
   borders->index(10);
   
   borders->index(10);
   borders->index(11);
   borders->index(8);

   // --
   
   borders->position(0,h,0);
   borders->normal(0,-1,0);

   borders->position(w,h,0);
   borders->normal(0,-1,0);

   borders->position(w,h,d);
   borders->normal(0,-1,0);

   borders->position(0,h,d);
   borders->normal(0,-1,0);

   borders->index(12);
   borders->index(13);
   borders->index(14);
   
   borders->index(14);
   borders->index(15);
   borders->index(12);

   // --
   
   borders->position(0,0,0);
   borders->normal(0,0,1);

   borders->position(w,0,0);
   borders->normal(0,0,1);

   borders->position(w,h,0);
   borders->normal(0,0,1);

   borders->position(0,h,0);
   borders->normal(0,0,1);

   borders->index(16);
   borders->index(17);
   borders->index(18);
   
   borders->index(18);
   borders->index(19);
   borders->index(16);

   // --
   
   borders->position(0,0,d);
   borders->normal(0,0,-1);

   borders->position(0,h,d);
   borders->normal(0,0,-1);

   borders->position(w,h,d);
   borders->normal(0,0,-1);

   borders->position(w,0,d);
   borders->normal(0,0,-1);

   borders->index(20);
   borders->index(21);
   borders->index(22);
   
   borders->index(22);
   borders->index(23);
   borders->index(20);
   
   borders->end();
   SceneMgr->getRootSceneNode()->attachObject(borders);


   Ogre::Light* light = SceneMgr->createLight("Light");
   light->setPosition(w/2,h/2,d/2);
   light->setDiffuseColour(0.5, 0.5, 0.5);

   Ogre::Light* plight = SceneMgr->createLight("PlayerLight");
   plight->setDiffuseColour(0.2, 0.2, 0.2);
   PlayerNode->attachObject(plight);

   YawNode->setPosition(w/2, h/2, d/2);
   
   
   //Voxels->SetSphere(0,0,0,10,true);
   const unsigned long long seed = time(NULL);
   std::cout << "\n\n-------------------------\nSEED: " << seed << "\n-------------------------\n\n";
   Voxels->Generate(MeshSize,Width,Height,Depth,seed);
   
   /*
   Voxels->SetSphere(500,200,200,50,true);
   Voxels->SetSphere(500,300,200,10,true);
   Voxels->SetSphere(500,400,200,5,true);

   Voxels->SetSphere(200,500,200,50,true);
   Voxels->SetSphere(220,500,200,35,false);

   Voxels->SetSphere(200,100,200,50,true);
   Voxels->SetSphere(220,100,200,35,false);
   Voxels->SetSphere(180,100,200,35,false);

   //Voxels->SetEllipsoid(600,600,600,50,10,30,true);

   //Voxels->SetEllipsoid(600,600,600,200,200,200,true);
   */
   
 
   SceneMgr->setAmbientLight(Ogre::ColourValue(0, 0, 0));
   //SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);
   
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

float random(float min, float max) {
   return (rand()/(float)RAND_MAX)*(max-min) + min;
}
void App::UpdateMesh(size_t x, size_t y, size_t z, const std::vector<VoxelContainer::Quad> & quads)
{
   ManualObject* & mesh = Meshes[z*Width*Height + y*Width + x];

   //std::cout << "\n\n-------------------------------------\nUpdateMesh(" << x << "," << y << "," << z << ") with " << quads.size() << " quads\n" << std::endl;
   
   if (quads.empty())
   {
      if (mesh)
      {
         SceneMgr->destroyManualObject(mesh);
         mesh = nullptr;
      }
         
      return;
   }

   if (mesh == nullptr)
   {
      mesh = SceneMgr->createManualObject();
      SceneMgr->getRootSceneNode()->attachObject(mesh);
      mesh->setDynamic(true);
      mesh->begin("voxel", RenderOperation::OT_TRIANGLE_LIST);
   }
   else
   {
      mesh->beginUpdate(0);
   }

   unsigned int i = 0;
   for (const VoxelContainer::Quad & q : quads)
   {
      float d = random(-0.1,0.1);
      
      ColourValue colour(0.45+d, 0.42+d, 0.31+d);
      /*if (q.Normal.x != 0 or q.Normal.z != 0 or q.Normal.y == -1) {
         colour = ColourValue(0.45,0.42,0.31);
      }
      else {
         colour = ColourValue(0.17,0.37,0.04);
         }*/
      
      mesh->position(q.A);
      mesh->colour(colour);
      mesh->normal(q.Normal);

      mesh->position(q.B);
      mesh->colour(colour);
      mesh->normal(q.Normal);

      mesh->position(q.C);
      mesh->colour(colour);
      mesh->normal(q.Normal);

      mesh->position(q.D);
      mesh->colour(colour);
      mesh->normal(q.Normal);

      mesh->index(i);
      mesh->index(i+1);
      mesh->index(i+2);

      mesh->index(i+2);
      mesh->index(i+3);
      mesh->index(i);

      i += 4;
   }

   mesh->end();
}


int main()
{
   srand(time(NULL));
   
   App app;
   app.Go();
   
   return EXIT_SUCCESS;
}
