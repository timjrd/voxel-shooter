#include "Model.hh"

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



VoxelContainer::VoxelContainer(size_t meshSize, size_t width, size_t height, size_t depth, MeshListener * listener)
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

VoxelContainer::~VoxelContainer()
{}

uint8_t & VoxelContainer::At(size_t x, size_t y, size_t z)
{
   return Voxels[z*Width*Height + y*Width + x];
}


struct Cave {
   int x, y, z, a, b, c;
   int vx, vy, vz, va, vb, vc;
   int nx, ny, nz, na, nb, nc;
};
void VoxelContainer::Generate(size_t meshSize, size_t width, size_t height, size_t depth, unsigned long long seed)
{
   MeshSize = meshSize;
   Width    = MeshSize*width;
   Height   = MeshSize*height;
   Depth    = MeshSize*depth;

   Voxels.resize(Width*Height*Depth);
   for (uint8_t & v : Voxels)
      v = true;

   //UpdateMeshes(0,0,0,Width-1,Height-1,Depth-1); return;
   
   // --
   
   Random random(seed);
   
   const int iterations = 1200;
   const int period = 15;
   
   const int maxRadius = 40;// min(Width, min(Height, Depth)) / 7;
   const int minRadius = 10;
   
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

   UpdateMeshes(0,0,0,Width-1,Height-1,Depth-1);
}

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

   std::cout << "\n" << std::endl;
   
   std::cout << "SetEllipsoid: " << fromX << "," << fromY << "," << fromZ << std::endl;
   
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

void VoxelContainer::DSetEllipsoid(int cx, int cy, int cz, int a, int b, int c, bool set)
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


void VoxelContainer::UpdateMeshes(int fromX, int fromY, int fromZ, int toX, int toY, int toZ)
{
   if (Listener)
      for (int x=fromX/MeshSize; x<=toX/MeshSize; x++)
         for (int y=fromY/MeshSize; y<=toY/MeshSize; y++)
            for (int z=fromZ/MeshSize; z<=toZ/MeshSize; z++)
            {
               std::cout << "UpdateMesh !" << std::endl;
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
            if (At(x,y,z))
            {
               if (x>0 and not At(x-1,y,z)) {
                  res.emplace_back();
                  res.back().A = {x,y,z};
                  res.back().B = {x,y,z+1};
                  res.back().C = {x,y+1,z+1};
                  res.back().D = {x,y+1,z};
                  res.back().Normal = {-1,0,0};
               }

               if (x<Width-1 and not At(x+1,y,z)) {
                  res.emplace_back();
                  res.back().A = {x+1,y,z};
                  res.back().B = {x+1,y+1,z};
                  res.back().C = {x+1,y+1,z+1};
                  res.back().D = {x+1,y,z+1};
                  res.back().Normal = {1,0,0};
               }

               if (y>0 and not At(x,y-1,z)) {
                  res.emplace_back();
                  res.back().A = {x,y,z};
                  res.back().B = {x+1,y,z};
                  res.back().C = {x+1,y,z+1};
                  res.back().D = {x,y,z+1};
                  res.back().Normal = {0,-1,0};
               }

               if (y<Height-1 and not At(x,y+1,z)) {
                  res.emplace_back();
                  res.back().A = {x,y+1,z};
                  res.back().B = {x,y+1,z+1};
                  res.back().C = {x+1,y+1,z+1};
                  res.back().D = {x+1,y+1,z};
                  res.back().Normal = {0,1,0};
               }
               
               if (z>0 and not At(x,y,z-1)) {
                  res.emplace_back();
                  res.back().A = {x,y,z};
                  res.back().B = {x,y+1,z};
                  res.back().C = {x+1,y+1,z};
                  res.back().D = {x+1,y,z};
                  res.back().Normal = {0,0,-1};
               }

               if (z<Depth-1 and not At(x,y,z+1)) {
                  res.emplace_back();
                  res.back().A = {x,y,z+1};
                  res.back().B = {x+1,y,z+1};
                  res.back().C = {x+1,y+1,z+1};
                  res.back().D = {x,y+1,z+1};
                  res.back().Normal = {0,0,1};
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
            if ( At(x,y,z)
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

   else return At(x,y,z);
}
