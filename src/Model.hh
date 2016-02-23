#pragma once

//#include "View.hh"
//#include "App.hh"
#include "utils.hh"

#include <OgreVector3.h>

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <vector>
#include <list>

class Projectile;

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
      Ogre::ColourValue Colour;
   };

   class Observer
   {
   public:
      virtual void UpdateChargeur(int chargeur, int total) = 0;
      virtual void ProjectileFired(Projectile &) = 0;
      virtual void UpdatePlayer(const Ogre::Vector3 & pos, const Ogre::Quaternion & orientation) = 0;
      virtual void UpdateSize(long meshSize, long width, long height, long depth) = 0;
      virtual void UpdateMesh(long x, long y, long z, const std::vector<Model::Quad> &) = 0;
   };


   class Player
   {
   private:
      float YawValue   = 0;
      float PitchValue = 0;

      float Vie = 100;
      int NbMunitions = 100;
      int QuantiteChargeur = 20;
      int ChargeurEnCours = 0;

   public:
      Ogre::Vector3 Position;

      void Yaw(float yaw);
      void Pitch(float pitch);
      Ogre::Vector3    GetDirection()   const;
      Ogre::Quaternion GetOrientation() const;

      float GetVie()          { return Vie;             }
      int GetChargeur()       { return ChargeurEnCours; }
      int GetTotalMunitions() { return NbMunitions;     }

      void  Decremente(float degats);

      void tirer()            { ChargeurEnCours--; }
      void Recharger(Model::Observer *);
      void InitChargeur();
   };

private:
   class VoxelColour
   {
   public:
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
   
   Observer * MyObserver = nullptr;

   Player MyPlayer;
   float  PlayerSize = 3;

   std::list<Projectile*> Projectiles;
   
   std::vector<Voxel> Voxels;
   long Width  = 0;
   long Height = 0;
   long Depth  = 0;
   
   long MeshSize = 0;


   void BrushEllipsoid(int cx, int cy, int cz, int a, int b, int c);
   void ExtractMesh(size_t x, size_t y, size_t z, std::vector<Quad> & res);
   void UpdateMeshes(int fromX, int fromY, int fromZ, int toX, int toY, int toZ);
   void UpdatePlayer();
   void UpdateSize();
   
public:
   Model();
   ~Model();

   void SetObserver(Observer *);

   void Tick(float time);

   void Fire(Projectile*, float time, bool left);
   
   void TranslatePlayer(const Ogre::Vector3 &);
   void SetPlayerPosition(const Ogre::Vector3 &);
   void YawPlayer(float);
   void PitchPlayer(float);

   void Generate(size_t meshSize, size_t width, size_t height, size_t depth, unsigned long long seed);

   //static uint8_t & at(size_t x, size_t y, size_t z, std::vector<uint8_t> & voxels, size_t width, size_t height, size_t depth);
   Voxel & At(size_t x, size_t y, size_t z);
   bool FilledAt(size_t x, size_t y, size_t z);

   void SetVoxel(int x, int y, int z, bool set);
   void SetSphere(float cx, float cy, float cz, float r, bool set);
   void SetSphere(Ogre::Vector3 pos, float r, bool set);
   void SetEllipsoid(float cx, float cy, float cz, float a, float b, float c, bool set);

   bool BoxIntersects(Ogre::Vector3 min, Ogre::Vector3 max);
   bool PointIntersects(Ogre::Vector3 pos);

   long GetWidth();
   long GetHeight();
   long GetDepth();
   long GetMeshSize();

   void RechargeArmePlayer();
};
