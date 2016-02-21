#pragma once

#include "Model.hh"
#include "Projectile.hh"

#include <OgreRoot.h>
#include <OgreSceneManager.h>
#include <OgreCamera.h>
#include <OgreRenderWindow.h>

#include <CEGUI/CEGUI.h>

#include <vector>
#include <tuple>

class View : public Model::Observer
{
private:
   Model * MyModel;
   std::vector<std::pair<Ogre::SceneNode*, Ogre::ManualObject*>> Meshes;
   long MeshesWidth;
   long MeshesHeight;
   long MeshesDepth;

   Ogre::RenderWindow * Window;
   Ogre::Viewport     * Viewport;
   Ogre::SceneManager * SceneManager;
   Ogre::Camera       * Camera;

   CEGUI::Window      * FpsViewer;
   float TimeAtFirstFrame = 0;
   int   NbFrame          = 0;

   Ogre::SceneNode * PlayerNode;
   
public:
   View();
   void Init(Ogre::Root &, Ogre::RenderWindow &);
   void SetModel(Model &);
   
   void OnFrame(float time);

   void ProjectileFired(Projectile &) override;
   void UpdatePlayer(const Ogre::Vector3 & pos, const Ogre::Quaternion & orientation) override;
   void UpdateSize(long meshSize, long width, long height, long depth) override;
   void UpdateMesh(long x, long y, long z, const std::vector<Model::Quad> &) override;   
};
