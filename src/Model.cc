#include "Model.hh"
#include "utils.hh"

#include <cmath>

using namespace std;

Random::Random(unsigned long long seed)
   : Previous(seed)
{}
   
int Random::Next(int min, int max)
{
   Previous = (1103515245 * Previous + 12345) % 2147483648;

   return (Previous % (max-min+1)) + min;
}



Model::Model(size_t meshSize, size_t width, size_t height, size_t depth, MeshListener * listener)
   : Listener(listener)
   , Width(meshSize*width)
   , Height(meshSize*height)
   , Depth(meshSize*depth)
   , MeshSize(meshSize)
{
   Voxels.resize(Width*Height*Depth);
   for (uint8_t & v : Voxels)
      v = false;
}

Model::~Model()
{}

uint8_t & Model::at(size_t x, size_t y, size_t z, std::vector<uint8_t> & voxels, size_t width, size_t height, size_t depth)
{
   return voxels[z*width*height + y*width + x];
}
uint8_t & Model::At(size_t x, size_t y, size_t z)
{
   return at(x,y,z, Voxels, Width, Height, Depth);
}


struct Cave {
   int x, y, z, a, b, c;
   int vx, vy, vz, va, vb, vc;
   int nx, ny, nz, na, nb, nc;
};
void Model::Generate(size_t meshSize, size_t width, size_t height, size_t depth, unsigned long long seed)
{
   MeshSize = meshSize;
   Width    = MeshSize*width;
   Height   = MeshSize*height;
   Depth    = MeshSize*depth;

   Voxels.resize(Width*Height*Depth);
   for (uint8_t & v : Voxels)
      v = true;

   //SetSphere(200,200,200,10,true); SetSphere(220,200,200,10,true); BlurThreshold(); UpdateMeshes(0,0,0,Width-1,Height-1,Depth-1); return;
   
   // --

   Random random(seed);
   
   const int iterations = 1200;
   const int period = 20;
   
   const int maxRadius = 40;// min(Width, min(Height, Depth)) / 7;
   const int minRadius = 15;
   
   std::vector<Cave> caves(1);
   for (Cave & c : caves)
   {
      c.x = Width/2;
      c.y = Height/2;
      c.z = Depth/2;
      
      c.nx = random.Next(-5,5);
      c.ny = random.Next(-5,5);
      c.nz = random.Next(-5,5);

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

            c.nx = random.Next(-5,5);
            c.ny = random.Next(-5,5);
            c.nz = random.Next(-5,5);
            
            c.na = random.Next(-5,5);
            c.nb = random.Next(-5,5);
            c.nc = random.Next(-5,5);
         }
      }
      
      for (Cave & c : caves)
      {
         c.a = between(minRadius, c.a + ((c.na-c.va) * periodProgress)/100 + c.va, maxRadius);
         c.b = between(minRadius, c.b + ((c.nb-c.vb) * periodProgress)/100 + c.vb, maxRadius);
         c.c = between(minRadius, c.c + ((c.nc-c.vc) * periodProgress)/100 + c.vc, maxRadius);

         c.x = between(c.a+1, c.x + ((c.nx-c.vx) * periodProgress)/100 + c.vx, ((int)Width)-1-c.a-1);
         c.y = between(c.b+1, c.y + ((c.ny-c.vy) * periodProgress)/100 + c.vy, ((int)Height)-1-c.b-1);
         c.z = between(c.c+1, c.z + ((c.nz-c.vz) * periodProgress)/100 + c.vz, ((int)Depth)-1-c.c-1);


         DSetEllipsoid(c.x, c.y, c.z, c.a, c.b, c.c, false);
      }
   }

   //BlurThreshold();
   
   UpdateMeshes(0,0,0,Width-1,Height-1,Depth-1);
}

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

void Model::SetSphere(Ogre::Vector3 pos, float r, bool set) {
   SetSphere(pos.x, pos.y, pos.z, r, set);
}
void Model::SetSphere(float cx, float cy, float cz, float r, bool set) {
   SetEllipsoid(cx,cy,cz,r,r,r,set);
}

void Model::SetEllipsoid(float cx, float cy, float cz, float a, float b, float c, bool set)
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

void Model::DSetEllipsoid(int cx, int cy, int cz, int a, int b, int c, bool set)
{
   const int fromX = between(0, cx-a, ((int)Width)-1);
   const int fromY = between(0, cy-b, ((int)Height)-1);
   const int fromZ = between(0, cz-c, ((int)Depth)-1);

   const int toX = between(0, cx+a, ((int)Width)-1);
   const int toY = between(0, cy+b, ((int)Height)-1);
   const int toZ = between(0, cz+c, ((int)Depth)-1);

   //std::cout << "DSetEllipsoid: " << fromX << "," << fromY << "," << fromZ << std::endl;
   
   for (int x=fromX; x<=toX; x++)
      for (int y=fromY; y<=toY; y++)
         for (int z=fromZ; z<=toZ; z++)
         {
            const int tx = x - cx;
            const int ty = y - cy;
            const int tz = z - cz;

            if ( (tx*tx*100)/(a*a) + (ty*ty*100)/(b*b) + (tz*tz*100)/(c*c) <= 100 )
               At(x,y,z) = set;
         }
}


void Model::UpdateMeshes(int fromX, int fromY, int fromZ, int toX, int toY, int toZ)
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


void Model::ExtractMesh(size_t mx, size_t my, size_t mz, std::vector<Quad> & res)
{
   for(size_t x=mx*MeshSize; x < mx*MeshSize+MeshSize; x++)
      for(size_t y=my*MeshSize; y < my*MeshSize+MeshSize; y++)
         for(size_t z=mz*MeshSize; z < mz*MeshSize+MeshSize; z++)
            if (At(x,y,z))
            {
               const float fx = x;
               const float fy = y;
               const float fz = z;
               
               if (x>0 and not At(x-1,y,z)) {
                  res.emplace_back();
                  res.back().A = {fx, fy  , fz  };
                  res.back().B = {fx, fy  , fz+1};
                  res.back().C = {fx, fy+1, fz+1};
                  res.back().D = {fx, fy+1, fz  };
                  res.back().Normal = {-1,0,0};
               }

               if (x<Width-1 and not At(x+1,y,z)) {
                  res.emplace_back();
                  res.back().A = {fx+1, fy  , fz  };
                  res.back().B = {fx+1, fy+1, fz  };
                  res.back().C = {fx+1, fy+1, fz+1};
                  res.back().D = {fx+1, fy  , fz+1};
                  res.back().Normal = {1,0,0};
               }

               if (y>0 and not At(x,y-1,z)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy, fz  };
                  res.back().B = {fx+1, fy, fz  };
                  res.back().C = {fx+1, fy, fz+1};
                  res.back().D = {fx  , fy, fz+1};
                  res.back().Normal = {0,-1,0};
               }

               if (y<Height-1 and not At(x,y+1,z)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy+1, fz};
                  res.back().B = {fx  , fy+1, fz+1};
                  res.back().C = {fx+1, fy+1, fz+1};
                  res.back().D = {fx+1, fy+1, fz};
                  res.back().Normal = {0,1,0};
               }
               
               if (z>0 and not At(x,y,z-1)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy  , fz};
                  res.back().B = {fx  , fy+1, fz};
                  res.back().C = {fx+1, fy+1, fz};
                  res.back().D = {fx+1, fy  , fz};
                  res.back().Normal = {0,0,-1};
               }

               if (z<Depth-1 and not At(x,y,z+1)) {
                  res.emplace_back();
                  res.back().A = {fx  , fy  , fz+1};
                  res.back().B = {fx+1, fy  , fz+1};
                  res.back().C = {fx+1, fy+1, fz+1};
                  res.back().D = {fx  , fy+1, fz+1};
                  res.back().Normal = {0,0,1};
               }
            }
}


bool Model::BoxIntersects(Ogre::Vector3 min, Ogre::Vector3 max)
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
            if ( At(x,y,z)
                 and min.x < x+1 and max.x > x
                 and min.y < y+1 and max.y > y
                 and min.z < z+1 and max.z > z )
               return true;

   return false;
}

bool Model::PointIntersects(Ogre::Vector3 pos)
{
   const int x = pos.x;
   const int y = pos.y;
   const int z = pos.z;

   if ( x < 0 or x > Width-1
        or y < 0 or y > Height-1
        or z < 0 or z > Depth-1 )
      return false;

   else return At(x,y,z);
}
