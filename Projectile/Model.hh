#pragma once

#include <OgreVector3.h>

#include <cstddef>
#include <algorithm>

template<typename T>
T between(T a, T x, T b) {
   return std::max(a, std::min(x, b));
}

class VoxelContainer
{
public:   
   struct Quad
   {
      Ogre::Vector3 A;
      Ogre::Vector3 B;
      Ogre::Vector3 C;
      Ogre::Vector3 D;

      Ogre::Vector3 Normal;
   };

   class MeshListener
   {
   public:
      virtual void UpdateMesh(size_t x, size_t y, size_t z, const std::vector<Quad> &) {};
   };
   
private:
   MeshListener * Listener = nullptr;
   
   bool * Voxels = nullptr;
   size_t Width  = 0;
   size_t Height = 0;
   size_t Depth  = 0;
   
   size_t MeshSize = 0;

public:
   VoxelContainer(size_t meshSize, size_t width, size_t height, size_t depth, MeshListener * listener);
   ~VoxelContainer();

   bool & At(size_t x, size_t y, size_t z);
   void SetSphere(float x, float y, float z, float r, bool b);
   void SetSphere(Ogre::Vector3 pos, float r, bool b);

   bool BoxIntersects(Ogre::Vector3 min, Ogre::Vector3 max);
   bool PointIntersects(Ogre::Vector3 pos);
   
   void ExtractMesh(size_t x, size_t y, size_t z, std::vector<Quad> & res);
};
