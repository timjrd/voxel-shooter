#include "View.hh"

#include <OgreManualObject.h>
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
   setLightAttenuation(*light, 300); // !!
   PlayerNode->attachObject(light);
   
   SceneManager->setAmbientLight(Ogre::ColourValue(0, 0, 0));

   MyCeguiView = new CeguiView();
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
      MyCeguiView->SetFpsText(to_string(fps) + " FPS");

      NbFrame = 0;
      TimeAtFirstFrame = time;
   }

   NbFrame++;
}

void View::UpdateCharger(int chargeur, int total)
{
  MyCeguiView->SetStockMunitions(chargeur, total);
}

void View::ProjectileFired(Projectile & p)
{
   p.InitView(*SceneManager);
}

void View::UpdatePlayer(const Ogre::Vector3 & pos, const Ogre::Quaternion & orientation)
{
   //std::cout << "UpdatePlayer: " << pos << " " << orientation << std::endl;
   
   PlayerNode->setPosition(pos);
   PlayerNode->setOrientation(orientation);
}

void View::UpdateSize(long meshSize, long width, long height, long depth)
{
   MeshesWidth   = width  / meshSize + 1;
   MeshesHeight  = height / meshSize + 1;
   MeshesDepth   = depth  / meshSize + 1;

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
