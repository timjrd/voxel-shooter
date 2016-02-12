#pragma once

#include <OgreVector3.h>

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <vector>

class Random
{
private:
   unsigned long long Previous;

public:
   Random(unsigned long long seed);
   
   int Next(int min, int max);
};

class Model
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
   
   std::vector<uint8_t> Voxels;
   size_t Width  = 0;
   size_t Height = 0;
   size_t Depth  = 0;
   
   size_t MeshSize = 0;

public:
   Model(size_t meshSize, size_t width, size_t height, size_t depth, MeshListener * listener);
   ~Model();

   void Generate(size_t meshSize, size_t width, size_t height, size_t depth, unsigned long long seed);
   void BlurThreshold();

   static uint8_t & at(size_t x, size_t y, size_t z, std::vector<uint8_t> & voxels, size_t width, size_t height, size_t depth);
   uint8_t & At(size_t x, size_t y, size_t z);

   void SetSphere(float cx, float cy, float cz, float r, bool set);
   void SetSphere(Ogre::Vector3 pos, float r, bool set);
   void SetEllipsoid(float cx, float cy, float cz, float a, float b, float c, bool set);
   void DSetEllipsoid(int cx, int cy, int cz, int a, int b, int c, bool set);

   bool BoxIntersects(Ogre::Vector3 min, Ogre::Vector3 max);
   bool PointIntersects(Ogre::Vector3 pos);

   void UpdateMeshes(int fromX, int fromY, int fromZ, int toX, int toY, int toZ);
   void ExtractMesh(size_t x, size_t y, size_t z, std::vector<Quad> & res);
};
