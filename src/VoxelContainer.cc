#include "VoxelContainer.hh"
#include "utils.hh"

#include <cmath>
#include <algorithm>

using namespace std;

VoxelContainer::VoxelColour::VoxelColour(Random & r)
{
   Red   = r.Next(50,255);
   Green = r.Next(50,255);
   Blue  = r.Next(50,255);
}
VoxelContainer::VoxelColour VoxelContainer::VoxelColour::LinearInterpolationTo(const VoxelColour & o, int x, int xMax) const
{
   VoxelColour res;

   res.Red   = linearInterpolation(Red  , o.Red  , x, xMax);
   res.Green = linearInterpolation(Green, o.Green, x, xMax);
   res.Blue  = linearInterpolation(Blue , o.Blue , x, xMax);

   return res;
}
VoxelContainer::VoxelColour VoxelContainer::VoxelColour::Blend(const VoxelColour & o) const
{
   VoxelColour res;

   res.Red   = ((int)Red   + (int)o.Red  ) / 2;
   res.Green = ((int)Green + (int)o.Green) / 2;
   res.Blue  = ((int)Blue  + (int)o.Blue ) / 2;

   return res;
}
void VoxelContainer::VoxelColour::operator+=(int d)
{
   Red   = between(0, Red+d  , 255);
   Green = between(0, Green+d, 255);
   Blue  = between(0, Blue+d , 255);
}
Ogre::ColourValue VoxelContainer::VoxelColour::ToColourValue() const
{
   return Ogre::ColourValue(Red/(float)255, Green/(float)255, Blue/(float)255);
}


VoxelContainer::VoxelContainer(MeshListener * listener)
   : Listener(listener)
{}
VoxelContainer::~VoxelContainer()
{}

/*uint8_t & VoxelContainer::at(size_t x, size_t y, size_t z, std::vector<uint8_t> & voxels, size_t width, size_t height, size_t depth)
{
   return voxels[z*width*height + y*width + x];
   }*/

VoxelContainer::Voxel & VoxelContainer::At(size_t x, size_t y, size_t z)
{
   return Voxels[z*Width*Height + y*Width + x];
}
bool VoxelContainer::FilledAt(size_t x, size_t y, size_t z)
{
   return At(x,y,z).Value;
}


struct Cave {
   int x, y, z, a, b, c;
   int vx, vy, vz, va, vb, vc;
   int nx, ny, nz, na, nb, nc;
};
void VoxelContainer::Generate(size_t meshSize, size_t width, size_t height, size_t depth, unsigned long long seed, int minRadius)
{
   MeshSize = meshSize;
   Width    = MeshSize * (width/MeshSize);
   Height   = MeshSize * (height/MeshSize);
   Depth    = MeshSize * (depth/MeshSize);

   Voxels.resize(Width*Height*Depth);

   // --
   
   Random random(seed);

   const VoxelColour xStart(random);
   const VoxelColour yStart(random);
   const VoxelColour zStart(random);

   const VoxelColour xEnd(random);
   const VoxelColour yEnd(random);
   const VoxelColour zEnd(random);

   for (int x=0; x < Width; x++)
      for (int y=0; y < Height; y++)
         for (int z=0; z < Depth; z++)
         {
            Voxel & v = At(x,y,z);
            v.Value = 255;
            const VoxelColour xColour = xStart.LinearInterpolationTo(xEnd, x, Width-1);
            const VoxelColour yColour = yStart.LinearInterpolationTo(yEnd, y, Height-1);
            const VoxelColour zColour = zStart.LinearInterpolationTo(zEnd, z, Depth-1);

            //VoxelColour colour = xColour.Blend(yColour).Blend(zColour);

            VoxelColour colour;
            colour.Red   = linearInterpolation(50, 200, x, Width-1);
            colour.Green = linearInterpolation(50, 200, y, Height-1);
            colour.Blue  = linearInterpolation(50, 200, z, Depth-1);
            colour += random.Next(-20,20);
            
            v.Colour = colour;
         }
   
   const int iterations = max(Width, max(Height, Depth)) * 4;
   const int period = 5 + iterations/80;
   
   const int maxRadius = min(Width, min(Height, Depth)) / 7;
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

   UpdateMeshes(0,0,0,Width-1,Height-1,Depth-1);
}

/*
void VoxelContainer::BlurThreshold()
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

/*
void VoxelContainer::SetSphere(Ogre::Vector3 pos, float r, bool set) {
   SetSphere(pos.x, pos.y, pos.z, r, set);
}
void VoxelContainer::SetSphere(float cx, float cy, float cz, float r, bool set) {
   SetEllipsoid(cx,cy,cz,r,r,r,set);
}

void VoxelContainer::SetEllipsoid(float cx, float cy, float cz, float a, float b, float c, bool set)
{
   const int fromX = between(0, (int) (cx-a), ((int)Width)-1);
   const int fromY = between(0, (int) (cy-b), ((int)Height)-1);
   const int fromZ = between(0, (int) (cz-c), ((int)Depth)-1);

   const int toX = between(0, (int) (cx+a), ((int)Width)-1);
   const int toY = between(0, (int) (cy+b), ((int)Height)-1);
   const int toZ = between(0, (int) (cz+c), ((int)Depth)-1);

   //std::cout << "SetEllipsoid: " << fromX << "," << fromY << "," << fromZ << std::endl;
   
   for (int x=fromX; x<=toX; x++)
      for (int y=fromY; y<=toY; y++)
         for (int z=fromZ; z<=toZ; z++)
         {
            const float tx = x+0.5 - cx;
            const float ty = y+0.5 - cy;
            const float tz = z+0.5 - cz;

            if ( (tx*tx)/(a*a) + (ty*ty)/(b*b) + (tz*tz)/(c*c) <= 1 )
               At(x,y,z) = set;
         }

   UpdateMeshes(fromX,fromY,fromZ,toX,toY,toZ);
}
*/

void VoxelContainer::BrushEllipsoid(int cx, int cy, int cz, int a, int b, int c)
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


void VoxelContainer::UpdateMeshes(int fromX, int fromY, int fromZ, int toX, int toY, int toZ)
{
   if (Listener)
      for (int x=fromX/MeshSize; x<=toX/MeshSize; x++)
         for (int y=fromY/MeshSize; y<=toY/MeshSize; y++)
            for (int z=fromZ/MeshSize; z<=toZ/MeshSize; z++)
            {
               //std::cout << "UpdateMesh !" << std::endl;
               vector<Quad> mesh;
               ExtractMesh(x,y,z,mesh);
               Listener->UpdateMesh(x,y,z,mesh);
            }
}


void VoxelContainer::ExtractMesh(size_t mx, size_t my, size_t mz, std::vector<Quad> & res)
{
   for(size_t x=mx*MeshSize; x < mx*MeshSize+MeshSize; x++)
      for(size_t y=my*MeshSize; y < my*MeshSize+MeshSize; y++)
         for(size_t z=mz*MeshSize; z < mz*MeshSize+MeshSize; z++)
         {
            const Voxel & v = At(x,y,z);
            if (v.Value)
            {
               const float fx = x;
               const float fy = y;
               const float fz = z;

               Ogre::ColourValue colour = v.Colour.ToColourValue();
               
               if (x>0 and not FilledAt(x-1,y,z)) {
                  res.emplace_back();
                  res.back().A = {fx, fy  , fz  };
                  res.back().B = {fx, fy  , fz+1};
                  res.back().C = {fx, fy+1, fz+1};
                  res.back().D = {fx, fy+1, fz  };
                  res.back().Normal = {-1,0,0};
                  res.back().Colour = colour;
               }

               if (x<Width-1 and not FilledAt(x+1,y,z)) {
                  res.emplace_back();
                  res.back().A = {fx+1, fy  , fz  };
                  res.back().B = {fx+1, fy+1, fz  };
                  res.back().C = {fx+1, fy+1, fz+1};
                  res.back().D = {fx+1, fy  , fz+1};
                  res.back().Normal = {1,0,0};
                  res.back().Colour = colour;
               }

               if (y>0 and not FilledAt(x,y-1,z)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy, fz  };
                  res.back().B = {fx+1, fy, fz  };
                  res.back().C = {fx+1, fy, fz+1};
                  res.back().D = {fx  , fy, fz+1};
                  res.back().Normal = {0,-1,0};
                  res.back().Colour = colour;
               }

               if (y<Height-1 and not FilledAt(x,y+1,z)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy+1, fz};
                  res.back().B = {fx  , fy+1, fz+1};
                  res.back().C = {fx+1, fy+1, fz+1};
                  res.back().D = {fx+1, fy+1, fz};
                  res.back().Normal = {0,1,0};
                  res.back().Colour = colour;
               }
               
               if (z>0 and not FilledAt(x,y,z-1)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy  , fz};
                  res.back().B = {fx  , fy+1, fz};
                  res.back().C = {fx+1, fy+1, fz};
                  res.back().D = {fx+1, fy  , fz};
                  res.back().Normal = {0,0,-1};
                  res.back().Colour = colour;
               }

               if (z<Depth-1 and not FilledAt(x,y,z+1)) {
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


bool VoxelContainer::BoxIntersects(Ogre::Vector3 min, Ogre::Vector3 max)
{
   const int fromX = between(0, (int) min.x, ((int)Width)-1);
   const int fromY = between(0, (int) min.y, ((int)Height)-1);
   const int fromZ = between(0, (int) min.z, ((int)Depth)-1);

   const int toX = between(0, (int) max.x, ((int)Width)-1);
   const int toY = between(0, (int) max.y, ((int)Height)-1);
   const int toZ = between(0, (int) max.z, ((int)Depth)-1);

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

bool VoxelContainer::PointIntersects(Ogre::Vector3 pos)
{
   const int x = pos.x;
   const int y = pos.y;
   const int z = pos.z;

   if ( x < 0 or x > Width-1
        or y < 0 or y > Height-1
        or z < 0 or z > Depth-1 )
      return false;

   else return FilledAt(x,y,z);
}
