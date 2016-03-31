#pragma once

//#include "View.hh"
//#include "App.hh"
#include "fix16.hpp"
#include "FixVector3.hh"
#include "FixQuaternion.hh"
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
      virtual void UpdateCharger(int chargeur, int total) = 0;
      virtual void ProjectileFired(Projectile &) = 0;
      virtual void UpdatePlayer(const Ogre::Vector3 & pos, const Ogre::Quaternion & orientation) = 0;
      virtual void UpdateSize(long meshSize, long width, long height, long depth) = 0;
      virtual void UpdateMesh(long x, long y, long z, const std::vector<Model::Quad> &) = 0;
   };


   class Player
   {
   private:
      Fix16 YawValue   = 0;
      Fix16 PitchValue = 0;

      Fix16 Life = 100;
      int Ammunitions = 100;
      int ChargerCapacity = 20;
      int CurrentCharger = 0;

   public:
      FixVector3 Position;

      void Yaw(Fix16 yaw);
      void Pitch(Fix16 pitch);
      FixVector3    GetDirection()   const;
      FixQuaternion GetOrientation() const;

      Fix16 GetLife()          { return Life;            }
      int GetCharger()       { return CurrentCharger; }
      int GetTotalAmmunitions() { return Ammunitions;     }

      void Hit(Fix16 damages);

      void Shoot()            { CurrentCharger--; }
      void Recharge(Model::Observer *);
      void InitCharger();
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
   Fix16  PlayerSize = 3;

   std::list<Projectile*> Projectiles;
   
   std::vector<Voxel> Voxels;
   long Width  = 0;
   long Height = 0;
   long Depth  = 0;
   
   long MeshSize = 0;


   void MengerSponge(int x, int y, int z, int size, int it, int iterations);
   void BrushEllipsoid(int cx, int cy, int cz, int a, int b, int c);
   void ExtractMesh(long x, long y, long z, std::vector<Quad> & res);
   void UpdateMeshes(int fromX, int fromY, int fromZ, int toX, int toY, int toZ);
   void UpdatePlayer();
   void UpdateSize();
   
public:
   Model();
   ~Model();

   void SetObserver(Observer *);

   void Tick(Fix16 time);

   void Fire(Projectile*, Fix16 time, bool left);
   
   void TranslatePlayer(const FixVector3 &);
   void SetPlayerPosition(const FixVector3 &);
   void YawPlayer(Fix16);
   void PitchPlayer(Fix16);

   void FillRainbow();
   void GenerateCave(size_t meshSize, size_t width, size_t height, size_t depth, unsigned long long seed);
   void GenerateMengerSponge(int meshSize, int size, int iterations);

   //static uint8_t & at(size_t x, size_t y, size_t z, std::vector<uint8_t> & voxels, size_t width, size_t height, size_t depth);
   Voxel & At(size_t x, size_t y, size_t z);
   bool FilledAt(size_t x, size_t y, size_t z);

   void SetVoxel(int x, int y, int z, bool set);
   void SetSphere(int cx, int cy, int cz, int r, bool set);
   void SetEllipsoid(int cx, int cy, int cz, int a, int b, int c, bool set);
   void SetCube(int x, int y, int z, int size, bool set);
   void SetBox(int fromX, int fromY, int fromZ, int toX, int toY, int toZ, bool set);

   bool BoxIntersects(FixVector3 min, FixVector3 max);
   bool PointIntersects(FixVector3 pos);

   long GetWidth();
   long GetHeight();
   long GetDepth();
   long GetMeshSize();

   void RechargeArmePlayer();
};
