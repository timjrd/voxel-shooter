#include "Model.hh"
#include "Projectile.hh"
#include "utils.hh"
#include "fix16.hpp"

#include <OgreMath.h>

#include <cmath>
#include <algorithm>

#include <iostream> // DEBUG

using namespace std;

void Model::Player::Yaw(Fix16 yaw) {
   YawValue = (YawValue + yaw).mod(Fix16(2)*FIX_PI);
}

void Model::Player::Pitch(Fix16 pitch) {
   PitchValue = between(-FIX_PI/Fix16(2), PitchValue + pitch, FIX_PI/Fix16(2));
}

FixVector3    Model::Player::GetDirection() const
{
   return GetOrientation() * FixVector3::NEGATIVE_UNIT_Z;
}

FixQuaternion Model::Player::GetOrientation() const
{
   const FixQuaternion yaw  (YawValue  , FixVector3(0,1,0));
   const FixQuaternion pitch(PitchValue, FixVector3(1,0,0));

   return yaw * pitch;
}


void Model::Player::Hit(Fix16 damages)
{
    Life -= damages;
    if(GetLife() < Fix16(0))
       Life = Fix16(0);
}

void Model::Player::Recharge(Model::Observer *ob)
{
   if(Ammunitions > 0)
   {
        int manquant = ChargerCapacity - CurrentCharger;
        if(Ammunitions <= manquant) {
            CurrentCharger += Ammunitions;
            Ammunitions = 0;
        }
        else
        {
           Ammunitions -= manquant;
           CurrentCharger = ChargerCapacity;
        }
        ob->UpdateCharger(CurrentCharger, Ammunitions);
   }
}

void Model::Player::InitCharger()
{
    CurrentCharger = ChargerCapacity;
    Ammunitions -= ChargerCapacity;

}

void Model::RechargeArmePlayer()
{
    MyPlayer.Recharge(MyObserver);
}

Model::VoxelColour::VoxelColour(Random & r)
{
   Red   = r.Next(50,255);
   Green = r.Next(50,255);
   Blue  = r.Next(50,255);
}
Model::VoxelColour Model::VoxelColour::LinearInterpolationTo(const VoxelColour & o, int x, int xMax) const
{
   VoxelColour res;

   res.Red   = linearInterpolation(Red  , o.Red  , x, xMax);
   res.Green = linearInterpolation(Green, o.Green, x, xMax);
   res.Blue  = linearInterpolation(Blue , o.Blue , x, xMax);

   return res;
}
Model::VoxelColour Model::VoxelColour::Blend(const VoxelColour & o) const
{
   VoxelColour res;

   res.Red   = ((int)Red   + (int)o.Red  ) / 2;
   res.Green = ((int)Green + (int)o.Green) / 2;
   res.Blue  = ((int)Blue  + (int)o.Blue ) / 2;

   return res;
}
void Model::VoxelColour::operator+=(int d)
{
   Red   = between(0, Red+d  , 255);
   Green = between(0, Green+d, 255);
   Blue  = between(0, Blue+d , 255);
}
Ogre::ColourValue Model::VoxelColour::ToColourValue() const
{
   return Ogre::ColourValue(Red/255.0, Green/255.0, Blue/255.0);
}


Model::Model()
{
    MyPlayer.InitCharger();
}
Model::~Model()
{}

/*uint8_t & Model::at(size_t x, size_t y, size_t z, std::vector<uint8_t> & voxels, size_t width, size_t height, size_t depth)
{
   return voxels[z*width*height + y*width + x];
   }*/

void Model::SetObserver(Observer * observer)
{
   MyObserver = observer;
   MyObserver->UpdateCharger(MyPlayer.GetCharger(), MyPlayer.GetTotalAmmunitions());
}

void Model::Tick(Fix16 time)
{
   for(auto i = Projectiles.begin(); i != Projectiles.end();)
   {
      if(not (*i)->Update(time))
      {
         delete (*i);
         i = Projectiles.erase(i);
      }
      else
         ++i;
   }
}

void Model::Fire(Projectile* p, Fix16 time, bool left)
{
   int chargeur = MyPlayer.GetCharger();
   if(chargeur > 0) {

       MyPlayer.Shoot();

       FixVector3 x(1,0,0);
       if (left) x *= -Fix16(1);
       x = MyPlayer.GetOrientation() * x;

       p->Init(*this, MyPlayer.Position + x, MyPlayer.GetDirection(), time);
       MyObserver->ProjectileFired(*p);
       MyObserver->UpdateCharger(MyPlayer.GetCharger(), MyPlayer.GetTotalAmmunitions());

       Projectiles.push_back(p);
   }
}

void Model::SetPlayerPosition(const FixVector3 & pos)
{
   MyPlayer.Position = pos;
   UpdatePlayer();
}

void Model::TranslatePlayer(const FixVector3 & dir)
{
   const FixVector3 d(PlayerSize, PlayerSize, PlayerSize);

   const FixVector3 t = MyPlayer.GetOrientation() * dir;

   FixVector3 & pos = MyPlayer.Position;

   const FixVector3 x(t.x,0,0);
   const FixVector3 y(0,t.y,0);
   const FixVector3 z(0,0,t.z);

   if (not BoxIntersects(pos+z - d, pos+z + d))
      pos += z;
   
   if (not BoxIntersects(pos+x - d, pos+x + d))
      pos += x;
   
   if (not BoxIntersects(pos+y - d, pos+y + d))
      pos += y;

   UpdatePlayer();
}

void Model::YawPlayer(Fix16 x) {
   MyPlayer.Yaw(x);
   UpdatePlayer();
}
void Model::PitchPlayer(Fix16 x) {
   MyPlayer.Pitch(x);
   UpdatePlayer();
}


Model::Voxel & Model::At(size_t x, size_t y, size_t z)
{
   return Voxels[z*Width*Height + y*Width + x];
}
bool Model::FilledAt(size_t x, size_t y, size_t z)
{
   return At(x,y,z).Value;
}


void Model::FillRainbow()
{
   Random random(424242424242);
   
   for (int x=0; x < Width; x++)
      for (int y=0; y < Height; y++)
         for (int z=0; z < Depth; z++)
         {
            Voxel & v = At(x,y,z);
            v.Value = 255;

            VoxelColour colour;
            colour.Red   = linearInterpolation(50, 160, x, Width-1);
            colour.Green = linearInterpolation(50, 160, y, Height-1);
            colour.Blue  = linearInterpolation(50, 160, z, Depth-1);
            colour += random.Next(-30,30);
            
            v.Colour = colour;
         }
}

struct Cave {
   int x, y, z, a, b, c;
   int vx, vy, vz, va, vb, vc;
   int nx, ny, nz, na, nb, nc;
};
void Model::GenerateCave(size_t meshSize, size_t width, size_t height, size_t depth, unsigned long long seed)
{
   MeshSize = meshSize;
   Width    = MeshSize * (width/MeshSize);
   Height   = MeshSize * (height/MeshSize);
   Depth    = MeshSize * (depth/MeshSize);

   Voxels.resize(Width*Height*Depth);

   // --
   
   Random random(seed);

   FillRainbow();
   
   const int iterations = max(Width, max(Height, Depth)) * 4;
   const int period = 5 + iterations/80;
   
   const int maxRadius = min(Width, min(Height, Depth)) / 7;
   const int minRadius = (PlayerSize*4).toInt();
   const int d = max(1, minRadius/3);
   
   std::vector<Cave> caves(1);
   for (Cave & c : caves)
   {
      c.x = Width/2;
      c.y = Height/2;
      c.z = Depth/2;
      
      c.nx = random.Next(-d,d);
      c.ny = random.Next(-d,d);
      c.nz = random.Next(-d,d);

      c.a = random.Next(minRadius,maxRadius);
      c.b = random.Next(minRadius,maxRadius);
      c.c = random.Next(minRadius,maxRadius);
      
      c.na = random.Next(-5,5);
      c.nb = random.Next(-5,5);
      c.nc = random.Next(-5,5);
   }
   
   for(int i=0; i < iterations; i++)
   {
      const int periodProgress = ((i%period)*100) / period;

      if (periodProgress == 0)
      {
         for (Cave & c : caves)
         {
            c.vx = c.nx;
            c.vy = c.ny;
            c.vz = c.nz;

            c.va = c.na;
            c.vb = c.nb;
            c.vc = c.nc;

            c.nx = random.Next(-d,d);
            c.ny = random.Next(-d,d);
            c.nz = random.Next(-d,d);
            
            c.na = random.Next(-5,5);
            c.nb = random.Next(-5,5);
            c.nc = random.Next(-5,5);
         }
      }
      
      for (Cave & c : caves)
      {
         c.a = between(minRadius, c.a + linearInterpolation(c.va, c.na, periodProgress, 100), maxRadius);
         c.b = between(minRadius, c.b + linearInterpolation(c.vb, c.nb, periodProgress, 100), maxRadius);
         c.c = between(minRadius, c.c + linearInterpolation(c.vc, c.nc, periodProgress, 100), maxRadius);

         c.x = between(c.a+1, c.x + linearInterpolation(c.vx, c.nx, periodProgress, 100), ((int)Width)-1-c.a-1);
         c.y = between(c.b+1, c.y + linearInterpolation(c.vy, c.ny, periodProgress, 100), ((int)Height)-1-c.b-1);
         c.z = between(c.c+1, c.z + linearInterpolation(c.vz, c.nz, periodProgress, 100), ((int)Depth)-1-c.c-1);

         BrushEllipsoid(c.x, c.y, c.z, c.a, c.b, c.c);
      }
   }

   //BrushEllipsoid(100, 100, 100, 50, 50, 50);
   //BrushEllipsoid(160, 100, 100, 50, 50, 50);
   
   for(Voxel & v : Voxels)
      v.Value = (v.Value > 100);

   SetPlayerPosition(FixVector3((int)Width/2, (int)Height/2, (int)Depth/2));

   UpdateSize();
   UpdateMeshes(0,0,0,Width-1,Height-1,Depth-1);
}

void Model::GenerateMengerSponge(int meshSize, int size, int iterations)
{
   const int spongeSize = pow(3,iterations);
   int scaledSize;
   if (size > spongeSize)
      scaledSize = spongeSize * (size/spongeSize);
   else
      scaledSize = spongeSize;
   
   Width = Height = Depth = scaledSize;
   MeshSize = meshSize;
   
   Voxels.resize(Width*Height*Depth);

   // --
   
   FillRainbow();
   MengerSponge(0,0,0, scaledSize, 1, iterations);
   
   // --

   SetPlayerPosition(FixVector3((int)Width/2, (int)Height/2, (int)Depth/2));

   UpdateSize();
   UpdateMeshes(0,0,0,Width-1,Height-1,Depth-1);
}

void Model::MengerSponge(int x, int y, int z, int size, int it, int iterations)
{
   if (it > iterations)
      return;
   
   const int h = size/3;

   SetCube(x+h, y+h, z    , h, false); // arrière 
   SetCube(x+h, y+h, z+h  , h, false); // centre
   SetCube(x+h, y+h, z+h+h, h, false); // avant

   SetCube(x+h+h, y+h, z+h, h, false); // droite
   SetCube(x    , y+h, z+h, h, false); // gauche

   SetCube(x+h, y+h+h, z+h, h, false); // haut
   SetCube(x+h, y    , z+h, h, false); // bas

   // ligne arrière bas
   MengerSponge(x    , y, z, h, it+1, iterations);
   MengerSponge(x+h  , y, z, h, it+1, iterations);
   MengerSponge(x+h+h, y, z, h, it+1, iterations);

   // ligne avant bas
   MengerSponge(x    , y, z+h+h, h, it+1, iterations);
   MengerSponge(x+h  , y, z+h+h, h, it+1, iterations);
   MengerSponge(x+h+h, y, z+h+h, h, it+1, iterations);

   // ligne arrière haut
   MengerSponge(x    , y+h+h, z, h, it+1, iterations);
   MengerSponge(x+h  , y+h+h, z, h, it+1, iterations);
   MengerSponge(x+h+h, y+h+h, z, h, it+1, iterations);

   // ligne avant haut
   MengerSponge(x    , y+h+h, z+h+h, h, it+1, iterations);
   MengerSponge(x+h  , y+h+h, z+h+h, h, it+1, iterations);
   MengerSponge(x+h+h, y+h+h, z+h+h, h, it+1, iterations);

   // croix gauche
   MengerSponge(x, y+h  , z    , h, it+1, iterations);
   MengerSponge(x, y+h+h, z+h  , h, it+1, iterations);
   MengerSponge(x, y+h  , z+h+h, h, it+1, iterations);
   MengerSponge(x, y    , z+h  , h, it+1, iterations);

   // croix droite
   MengerSponge(x+h+h, y+h  , z    , h, it+1, iterations);
   MengerSponge(x+h+h, y+h+h, z+h  , h, it+1, iterations);
   MengerSponge(x+h+h, y+h  , z+h+h, h, it+1, iterations);
   MengerSponge(x+h+h, y    , z+h  , h, it+1, iterations);
}


/*
void Model::BlurThreshold()
{
   const int blurSize  = 5;
   const int threshold = 4; // x / 10

   const unsigned long b = blurSize*2+1;
   const unsigned long maskVolume = b*b*b;
   const unsigned long threshold_ = (maskVolume * threshold) / 10;
   
   std::vector<uint8_t> result(Voxels.size());

   for (int x=0; x < Width; x++)
      for (int y=0; y < Height; y++)
         for (int z=0; z < Depth; z++)
         {
            const int fromX = between(0, x-blurSize, ((int)Width)-1);
            const int fromY = between(0, y-blurSize, ((int)Height)-1);
            const int fromZ = between(0, z-blurSize, ((int)Depth)-1);

            const int toX = between(0, x+blurSize, ((int)Width)-1);
            const int toY = between(0, y+blurSize, ((int)Height)-1);
            const int toZ = between(0, z+blurSize, ((int)Depth)-1);

            unsigned long count = 0;
            for (int mx=fromX; mx<=toX; mx++)
               for (int my=fromY; my<=toY; my++)
                  for (int mz=fromZ; mz<=toZ; mz++)
                     if (At(mx,my,mz))
                        count++;

            const unsigned long dx = toX - fromX + 1;
            const unsigned long dy = toY - fromY + 1;
            const unsigned long dz = toZ - fromZ + 1;

            at(x,y,z, result, Width,Height,Depth) = count + (maskVolume-dx*dy*dz) >= threshold_;
         }

   Voxels = std::move(result);
}
*/


void Model::SetSphere(int cx, int cy, int cz, int r, bool set) {
   SetEllipsoid(cx,cy,cz,r,r,r,set);
}

void Model::SetVoxel(int x, int y, int z, bool set)
{
   At(x,y,z).Value = set;
   UpdateMeshes(x-1, y-1, z-1, x+1, y+1, z+1);
}
void Model::SetEllipsoid(int cx, int cy, int cz, int a, int b, int c, bool set)
{
   const int fromX = between(0, cx-a, ((int)Width)-1);
   const int fromY = between(0, cy-b, ((int)Height)-1);
   const int fromZ = between(0, cz-c, ((int)Depth)-1);

   const int toX = between(0, cx+a, ((int)Width)-1);
   const int toY = between(0, cy+b, ((int)Height)-1);
   const int toZ = between(0, cz+c, ((int)Depth)-1);

   //std::cout << "BrushEllipsoid: " << fromX << "," << fromY << "," << fromZ << std::endl;
   
   for (int x=fromX; x<=toX; x++)
      for (int y=fromY; y<=toY; y++)
         for (int z=fromZ; z<=toZ; z++)
         {
            const long long tx = x - cx;
            const long long ty = y - cy;
            const long long tz = z - cz;

            if ((tx*tx*1000)/(a*a) + (ty*ty*1000)/(b*b) + (tz*tz*1000)/(c*c) <= 1000)
               At(x,y,z).Value = set;
         }

   UpdateMeshes(fromX,fromY,fromZ,toX,toY,toZ);
}

void Model::SetCube(int x, int y, int z, int size, bool set)
{
   SetBox(x,y,z, x+size, y+size, z+size, set);
}
void Model::SetBox(int fromX, int fromY, int fromZ, int toX, int toY, int toZ, bool set)
{
   fromX = between(0, fromX, (int)Width);
   fromY = between(0, fromY, (int)Height);
   fromZ = between(0, fromZ, (int)Depth);

   toX = between(0, toX, (int)Width);
   toY = between(0, toY, (int)Height);
   toZ = between(0, toZ, (int)Depth);

   for (int x = fromX; x < toX; x++)
      for (int y = fromY; y < toY; y++)
         for (int z = fromZ; z < toZ; z++)
            At(x,y,z).Value = set;
}


void Model::BrushEllipsoid(int cx, int cy, int cz, int a, int b, int c)
{
   const int fromX = between(0, cx-a, ((int)Width)-1);
   const int fromY = between(0, cy-b, ((int)Height)-1);
   const int fromZ = between(0, cz-c, ((int)Depth)-1);

   const int toX = between(0, cx+a, ((int)Width)-1);
   const int toY = between(0, cy+b, ((int)Height)-1);
   const int toZ = between(0, cz+c, ((int)Depth)-1);

   //std::cout << "BrushEllipsoid: " << fromX << "," << fromY << "," << fromZ << std::endl;
   
   for (int x=fromX; x<=toX; x++)
      for (int y=fromY; y<=toY; y++)
         for (int z=fromZ; z<=toZ; z++)
         {
            const long long tx = x - cx;
            const long long ty = y - cy;
            const long long tz = z - cz;

            const long long brush = max(0ll, 1000 - ((tx*tx*1000)/(a*a) + (ty*ty*1000)/(b*b) + (tz*tz*1000)/(c*c)));

            uint8_t & value = At(x,y,z).Value;
            value = max(0ll, (long long)value - (brush*brush*brush)/3921569);
         }
}


void Model::UpdatePlayer()
{
   if (MyObserver)
      MyObserver->UpdatePlayer(MyPlayer.Position.toVector3(), MyPlayer.GetOrientation().toQuaternion());
}

void Model::UpdateSize()
{
   if (MyObserver)
      MyObserver->UpdateSize(MeshSize, Width, Height, Depth);
}

void Model::UpdateMeshes(int fromX, int fromY, int fromZ, int toX, int toY, int toZ)
{
   if (MyObserver)
      for (int x=fromX/MeshSize; x<=toX/MeshSize; x++)
         for (int y=fromY/MeshSize; y<=toY/MeshSize; y++)
            for (int z=fromZ/MeshSize; z<=toZ/MeshSize; z++)
            {
               //std::cout << "UpdateMesh !" << std::endl;
               vector<Quad> mesh;
               ExtractMesh(x,y,z,mesh);
               MyObserver->UpdateMesh(x,y,z,mesh);
            }
}


void Model::ExtractMesh(long mx, long my, long mz, std::vector<Quad> & res)
{
   const size_t fromX = mx*MeshSize;
   const size_t fromY = my*MeshSize;
   const size_t fromZ = mz*MeshSize;

   const size_t toX = min(Width , mx*MeshSize+MeshSize);
   const size_t toY = min(Height, my*MeshSize+MeshSize);
   const size_t toZ = min(Depth , mz*MeshSize+MeshSize);

   
   for(size_t x=fromX; x < toX; x++)
      for(size_t y=fromY; y < toY; y++)
         for(size_t z=fromZ; z < toZ; z++)
         {
            const Voxel & v = At(x,y,z);
            if (v.Value)
            {
               const float fx = x;
               const float fy = y;
               const float fz = z;

               Ogre::ColourValue colour = v.Colour.ToColourValue();
               
               if (x==0 or not FilledAt(x-1,y,z)) {
                  res.emplace_back();
                  res.back().A = {fx, fy  , fz  };
                  res.back().B = {fx, fy  , fz+1};
                  res.back().C = {fx, fy+1, fz+1};
                  res.back().D = {fx, fy+1, fz  };
                  res.back().Normal = {-1,0,0};
                  res.back().Colour = colour;
               }

               if (x==Width-1 or not FilledAt(x+1,y,z)) {
                  res.emplace_back();
                  res.back().A = {fx+1, fy  , fz  };
                  res.back().B = {fx+1, fy+1, fz  };
                  res.back().C = {fx+1, fy+1, fz+1};
                  res.back().D = {fx+1, fy  , fz+1};
                  res.back().Normal = {1,0,0};
                  res.back().Colour = colour;
               }

               if (y==0 or not FilledAt(x,y-1,z)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy, fz  };
                  res.back().B = {fx+1, fy, fz  };
                  res.back().C = {fx+1, fy, fz+1};
                  res.back().D = {fx  , fy, fz+1};
                  res.back().Normal = {0,-1,0};
                  res.back().Colour = colour;
               }

               if (y==Height-1 or not FilledAt(x,y+1,z)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy+1, fz};
                  res.back().B = {fx  , fy+1, fz+1};
                  res.back().C = {fx+1, fy+1, fz+1};
                  res.back().D = {fx+1, fy+1, fz};
                  res.back().Normal = {0,1,0};
                  res.back().Colour = colour;
               }
               
               if (z==0 or not FilledAt(x,y,z-1)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy  , fz};
                  res.back().B = {fx  , fy+1, fz};
                  res.back().C = {fx+1, fy+1, fz};
                  res.back().D = {fx+1, fy  , fz};
                  res.back().Normal = {0,0,-1};
                  res.back().Colour = colour;
               }

               if (z==Depth-1 or not FilledAt(x,y,z+1)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy  , fz+1};
                  res.back().B = {fx+1, fy  , fz+1};
                  res.back().C = {fx+1, fy+1, fz+1};
                  res.back().D = {fx  , fy+1, fz+1};
                  res.back().Normal = {0,0,1};
                  res.back().Colour = colour;
               }
            }
         }
}


bool Model::BoxIntersects(FixVector3 min, FixVector3 max)
{
   const int fromX = between(0, min.x.toInt(), ((int)Width)-1);
   const int fromY = between(0, min.y.toInt(), ((int)Height)-1);
   const int fromZ = between(0, min.z.toInt(), ((int)Depth)-1);

   const int toX = between(0, max.x.toInt(), ((int)Width)-1);
   const int toY = between(0, max.y.toInt(), ((int)Height)-1);
   const int toZ = between(0, max.z.toInt(), ((int)Depth)-1);

   for (int x=fromX; x<=toX; x++)
      for (int y=fromY; y<=toY; y++)
         for (int z=fromZ; z<=toZ; z++)
            if ( FilledAt(x,y,z)
                 and min.x < x+1 and max.x > x
                 and min.y < y+1 and max.y > y
                 and min.z < z+1 and max.z > z )
               return true;

   return false;
}

bool Model::PointIntersects(FixVector3 pos)
{
   const int x = pos.x.toInt();
   const int y = pos.y.toInt();
   const int z = pos.z.toInt();

   if ( x < 0 or x > Width-1
        or y < 0 or y > Height-1
        or z < 0 or z > Depth-1 )
      return false;

   else return FilledAt(x,y,z);
}


long Model::GetWidth()    { return Width;    }
long Model::GetHeight()   { return Height;   }
long Model::GetDepth()    { return Depth;    }
long Model::GetMeshSize() { return MeshSize; }




