#pragma once

#include <cstddef>
#include <algorithm>

const float PI = 3.1415927;

template<typename T>
T between(T a, T x, T b) {
   std::max(a, std::min(x, b));
}

class Model
{
public:   
   struct Vec3
   {
      float X;
      float Y;
      float Z;

      Vec3 operator+(const Vec3 &) const;
      Vec3 operator*(const float) const;
      void operator+=(const Vec3 & a);
   };
   struct Player
   {
      Vec3 Pos;

      float Theta;
      float Phi;
      // coordonnées sphériques
      // on n'utilise pas la convention mathématique :
      // X -> Z
      // Y -> X
      // Z -> Y
   };
   struct Quad
   {
      Vec3 A;
      Vec3 B;
      Vec3 C;
      Vec3 D;

      Vec3 Normal;
   };

   class Observer
   {
   public:
      virtual void UpdatePlayer(const Player &) {};
      virtual void UpdateMesh(size_t x, size_t y, size_t z, const std::vector<Quad> &) {};
   };
   
   static Vec3 toCartesian(float roll, float yaw);
   
private:
   Observer * Observer_ = nullptr;
   
   bool * Voxels = nullptr;
   size_t Width  = 0;
   size_t Height = 0;
   size_t Depth  = 0;
   
   size_t MeshSize = 0;

   Player Player_;
   float PlayerSpeed  = 10; // unit/second
   float PlayerRadius = 2;

   bool & At(size_t x, size_t y, size_t z);
   void SetSphere(float x, float y, float z, float r, bool b);
   void SetSphere(Vec3 pos, float r, bool b);
   
public:
   Model(size_t meshSize, size_t width, size_t height, size_t depth, Observer * observer);
   ~Model();

   void PlayerFill();
   void PlayerEmpty();

   void ExtractMesh(size_t x, size_t y, size_t z, std::vector<Quad> & res);

   void MovePlayer(float roll, float yaw, float time);
   void RotatePlayer(float roll, float yaw);
   
   void MovePlayerLeft(float time);
   void MovePlayerRight(float time);
   void MovePlayerForward(float time);
   void MovePlayerBackward(float time);
   void MovePlayerUp(float time);
   void MovePlayerDown(float time);
};
