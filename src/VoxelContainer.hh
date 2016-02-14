#pragma once

#include "utils.hh"

#include <OgreVector3.h>

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <vector>


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
      Ogre::ColourValue Colour;
   };

   struct VoxelColour
   {
      uint8_t Red;
      uint8_t Green;
      uint8_t Blue;

      VoxelColour() = default;
      VoxelColour(Random &);
      
      void operator+=(int);
      VoxelColour LinearInterpolationTo(const VoxelColour &, int x, int xMax) const;
      VoxelColour Blend(const VoxelColour &) const;

      Ogre::ColourValue ToColourValue() const;
   };
   
   struct Voxel
   {
      uint8_t Value;

      VoxelColour Colour;
   };

   class MeshListener
   {
   public:
      virtual void UpdateMesh(size_t x, size_t y, size_t z, const std::vector<Quad> &) {};
   };
   
private:
   MeshListener * Listener = nullptr;
   
   std::vector<Voxel> Voxels;
   size_t Width  = 0;
   size_t Height = 0;
   size_t Depth  = 0;
   
   size_t MeshSize = 0;

public:
   VoxelContainer(MeshListener * listener);
   ~VoxelContainer();

   void Generate(size_t meshSize, size_t width, size_t height, size_t depth, unsigned long long seed, int minRadius);

   //static uint8_t & at(size_t x, size_t y, size_t z, std::vector<uint8_t> & voxels, size_t width, size_t height, size_t depth);
   Voxel & At(size_t x, size_t y, size_t z);
   bool FilledAt(size_t x, size_t y, size_t z);

   //void SetSphere(float cx, float cy, float cz, float r, bool set);
   //void SetSphere(Ogre::Vector3 pos, float r, bool set);
   //void SetEllipsoid(float cx, float cy, float cz, float a, float b, float c, bool set);
   void BrushEllipsoid(int cx, int cy, int cz, int a, int b, int c);

   bool BoxIntersects(Ogre::Vector3 min, Ogre::Vector3 max);
   bool PointIntersects(Ogre::Vector3 pos);

   void UpdateMeshes(int fromX, int fromY, int fromZ, int toX, int toY, int toZ);
   void ExtractMesh(size_t x, size_t y, size_t z, std::vector<Quad> & res);
};
