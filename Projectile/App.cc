#include "App.hh"
 
#include <OgreException.h>
#include <OgreConfigFile.h>
#include <OgreRenderWindow.h>
#include <OgreViewport.h>
#include <OgreEntity.h>
#include <OgreWindowEventUtilities.h>
#include <OgreManualObject.h>
#include <OgreRibbonTrail.h>
#include <OgreBillboard.h>
#include <OgreBillboardSet.h>
#include <OgreEntity.h>

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
    if(newPitch <= Ogre::Radian(Ogre::Math::PI/2)){
       PlayerNode->pitch(d, Node::TS_LOCAL);
	}


    Vector3 translate(0,0,0);
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
	    
	Translate(translate);


   if (Mouse->getMouseState().buttonDown(OIS::MB_Left)) {
        ProjectileArray[fireId] = new Projectile(YawNode->getPosition() + Vector3(15, 2, 0), PlayerNode->_getDerivedOrientation(), SceneMgr, fireId);
		fireId += 1;
		std::cout << "FIRE ID **** *" << fireId << std::endl;;
	}
   else if (Mouse->getMouseState().buttonDown(OIS::MB_Right))  {
		std::cout << "BOUTON DROIT " << std::endl;
       Voxels->SetSphere(AlterNode->_getDerivedPosition(), 6, false);
	}

	for(int n =0; n< 20000; n++) 
	{
		if(ProjectileArray[n] != nullptr) 
		{
			if(!ProjectileArray[n]->Update(evt.timeSinceLastFrame))
			{
				delete ProjectileArray[n];
				ProjectileArray[n] = nullptr;
			}	
		}
	}

	if(fireId == 20000)
		fireId = 0;
	
	//mYellowLightAnimState->addTime(evt.timeSinceLastFrame);

	//UpdateAnimation();

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

}

bool App::Go()
{
   ResourcesCfg = "resources.cfg";
   PluginsCfg   = "plugins.cfg";
   Root = new Ogre::Root(PluginsCfg);

   PlayerRadius = 3;
   
   MeshSize = 30;
   Width = Height = Depth = 30;
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
   
   Ogre::Light* light = SceneMgr->createLight("PlayerLight");
   //light->setPosition(300,300,300);
   PlayerNode->attachObject(light);


   YawNode->setPosition(300,300,300);
   

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

   Voxels->SetSphere(500,200,200,50,true);
   Voxels->SetSphere(500,300,200,10,true);
   Voxels->SetSphere(500,400,200,5,true);

   Voxels->SetSphere(200,500,200,50,true);
   Voxels->SetSphere(220,500,200,35,false);

   Voxels->SetSphere(200,100,200,50,true);
   Voxels->SetSphere(220,100,200,35,false);
   Voxels->SetSphere(180,100,200,35,false);

   SceneMgr->setAmbientLight(Ogre::ColourValue(0.1, 0.1, 0.1));
   //SceneMgr->setShadowTechnique(Ogre::SHADOWTYPE_STENCIL_ADDITIVE);q

	for(int i=0; i<20000; i++)
		ProjectileArray[i] = nullptr;	


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

void App::setupTrailLights()
{

		SceneNode* node;
		Animation* anim;
		Light* light;
		BillboardSet* bbs;

		// create a ribbon trail that our lights will leave behind
		NameValuePairList params;
		params["numberOfChains"] = "1";
		params["maxElements"] = "80";
		
		trail = (RibbonTrail*) SceneMgr->createMovableObject("RibbonTrail", &params);
	
		SceneMgr->getRootSceneNode()->attachObject(trail);
		
		trail->setMaterialName("Examples/LightRibbonTrail");
		trail->setTrailLength(400);
		
		// create a light node
		node = SceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(50, 30, 0));

		// create a 14 second animation with spline interpolation
		anim = SceneMgr->createAnimation("Path1", 7);
		anim->setInterpolationMode(Animation::IM_SPLINE);

		track = anim->createNodeTrack(1, node);  // create a node track for our animation

		// enter keyframes for our track to define a path for the light to follow
		Vector3 playerPos = YawNode->getPosition();

		track->createNodeKeyFrame(0)->setTranslate(Vector3(playerPos.x + 10, playerPos.y, playerPos.z));
		track->createNodeKeyFrame(2)->setTranslate(Vector3(0, 0, 0));

		// create an animation state from the animation and enable it
		mYellowLightAnimState = SceneMgr->createAnimationState("Path1");
		mYellowLightAnimState->setEnabled(true);

		// set initial settings for the ribbon trail and add the light node
		trail->setInitialColour(0, 1.0, 0.8, 0);
		trail->setColourChange(0, 0.5, 0.5, 0.5, 0.5);
		trail->setInitialWidth(0, 5);
		trail->addNode(node);

		// attach a light with the same colour to the light node
		light = SceneMgr->createLight();
		light->setDiffuseColour(trail->getInitialColour(0));
		node->attachObject(light);

		// attach a flare with the same colour to the light node
		bbs = SceneMgr->createBillboardSet(1);
		bbs->createBillboard(Vector3::ZERO, trail->getInitialColour(0));
		bbs->setMaterialName("Examples/Flare");
		node->attachObject(bbs);
}

void App::UpdateAnimation() {
		Vector3 playerPos = YawNode->getPosition();

		track->createNodeKeyFrame(0)->setTranslate(Vector3(playerPos.x + 10, playerPos.y, playerPos.z));
		track->createNodeKeyFrame(2)->setTranslate(Vector3(0, 0, 0));
}

void App::UpdateMesh(size_t x, size_t y, size_t z, const std::vector<VoxelContainer::Quad> & quads)
{
   ManualObject* & mesh = Meshes[z*Width*Height + y*Width + x];

   std::cout << "\n\n-------------------------------------\nUpdateMesh(" << x << "," << y << "," << z << ") with " << quads.size() << " quads\n" << std::endl;
   
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
      mesh->position(q.A);
      mesh->normal(q.Normal);

      mesh->position(q.B);
      mesh->normal(q.Normal);

      mesh->position(q.C);
      mesh->normal(q.Normal);

      mesh->position(q.D);
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
   App app;
   app.Go();
   
   return EXIT_SUCCESS;
}
