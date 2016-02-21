#include "View.hh"

#include <OgreManualObject.h>

#include <CEGUI/RendererModules/Ogre/Renderer.h>

#include <string>

using namespace std;
using namespace Ogre;

View::View()
{}

void View::Init(Ogre::Root & root, Ogre::RenderWindow & window)
{
   Window = &window;
   SceneManager = root.createSceneManager(Ogre::ST_GENERIC);

   Camera = SceneManager->createCamera("PlayerCamera");
   Camera->setDirection(0, 0, -1);
   Camera->setNearClipDistance(0.1);

   Viewport = Window->addViewport(Camera);
   Viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));

   Camera->setAspectRatio(Viewport->getActualWidth() /(float) Viewport->getActualHeight());

   PlayerNode = SceneManager->getRootSceneNode()->createChildSceneNode();
   PlayerNode->attachObject(Camera);

   Ogre::Light* light = SceneManager->createLight();
   light->setDiffuseColour(1, 1, 1);
   setLightAttenuation(*light, 270);
   PlayerNode->attachObject(light);

   SceneManager->setAmbientLight(Ogre::ColourValue(0, 0, 0));


   // CEGUI FpsViewer
   
   CEGUI::OgreRenderer& myRenderer = CEGUI::OgreRenderer::bootstrapSystem(); 

   CEGUI::SchemeManager::getSingleton().createFromFile( "TaharezLook.scheme" );
   CEGUI::FontManager::getSingleton().createFromFile( "DejaVuSans-10.font" );

   CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultFont( "DejaVuSans-10" );
   CEGUI::System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage( "TaharezLook/MouseArrow" );
   CEGUI::System::getSingleton().getDefaultGUIContext().setDefaultTooltipType( "TaharezLook/Tooltip" );

   CEGUI::WindowManager &wmgr = CEGUI::WindowManager::getSingleton();
   CEGUI::Window *sheet = wmgr.createWindow("DefaultWindow", "CEGUIDemo/Sheet");

   //CEGUI::Window *quit = wmgr.createWindow("TaharezLook/Button", "CEGUIDemo/QuitButton");
   //quit->setText("MyButton Quit");
   //quit->setSize(CEGUI::USize(CEGUI::UDim(0.15, 0), CEGUI::UDim(0.05, 0)));

   FpsViewer = wmgr.createWindow("TaharezLook/Label", "CEGUIDemo/FpsLabel");
   FpsViewer->setText("FPS : ");
   FpsViewer->setSize(CEGUI::USize(CEGUI::UDim(0.10,0), CEGUI::UDim(0.05, 0)));
   FpsViewer->setPosition(CEGUI::UVector2(CEGUI::UDim(0.90,0), CEGUI::UDim(0,5)));

   sheet->addChild(FpsViewer);
   //sheet->addChild(quit);
   CEGUI::System::getSingleton().getDefaultGUIContext().setRootWindow(sheet);
}

void View::SetModel(Model & model)
{
   MyModel = &model;
}

void View::OnFrame(float time)
{
   if (NbFrame == 40)
   {
      const float elapsed = time - TimeAtFirstFrame;
      const int   fps     = NbFrame / elapsed;
      FpsViewer->setText(to_string(fps) + " FPS");

      NbFrame = 0;
      TimeAtFirstFrame = time;
   }

   NbFrame++;
}

void View::ProjectileFired(Projectile & p)
{
   p.InitView(*SceneManager);
}

void View::UpdatePlayer(const Ogre::Vector3 & pos, const Ogre::Quaternion & orientation)
{
   PlayerNode->setPosition(pos);
   PlayerNode->setOrientation(orientation);
}

void View::UpdateSize(long meshSize, long width, long height, long depth)
{
   MeshesWidth   = width  / meshSize;
   MeshesHeight  = height / meshSize;
   MeshesDepth   = depth  / meshSize;

   Meshes.resize(MeshesWidth*MeshesHeight*MeshesDepth, {nullptr, nullptr});
}

void View::UpdateMesh(long x, long y, long z, const std::vector<Model::Quad> & quads)
{
   auto & p = Meshes[z*MeshesWidth*MeshesHeight + y*MeshesWidth + x];
   SceneNode*    & node = p.first;
   ManualObject* & mesh = p.second;

   //std::cout << "\n\n-------------------------------------\nUpdateMesh(" << x << "," << y << "," << z << ") with " << quads.size() << " quads\n" << std::endl;
   
   if (quads.empty())
   {
      if (mesh)
      {
         SceneManager->destroyManualObject(mesh);
         SceneManager->destroySceneNode(node);

         mesh = nullptr;
         node = nullptr;
      }
         
      return;
   }

   if (mesh == nullptr)
   {
      mesh = SceneManager->createManualObject();
      node = SceneManager->getRootSceneNode()->createChildSceneNode();
      node->attachObject(mesh);

      mesh->setDynamic(false);
      mesh->begin("voxel", RenderOperation::OT_TRIANGLE_LIST);
   }
   else
   {
      mesh->beginUpdate(0);
   }

   unsigned int i = 0;
   for (const Model::Quad & q : quads)
   {
      mesh->position(q.A);
      mesh->colour(q.Colour);
      mesh->normal(q.Normal);

      mesh->position(q.B);
      mesh->colour(q.Colour);
      mesh->normal(q.Normal);

      mesh->position(q.C);
      mesh->colour(q.Colour);
      mesh->normal(q.Normal);

      mesh->position(q.D);
      mesh->colour(q.Colour);
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
