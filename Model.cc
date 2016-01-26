#include "Model.hh"

#include <cmath>

using namespace std;

VoxelContainer::VoxelContainer(size_t meshSize, size_t width, size_t height, size_t depth, MeshListener * listener)
   : Listener(listener)
   , Width(meshSize*width)
   , Height(meshSize*height)
   , Depth(meshSize*depth)
   , MeshSize(meshSize)
{
   Voxels = new bool[Width*Height*Depth];
   for (size_t i=0; i<Width*Height*Depth; i++)
      Voxels[i] = false;
}

VoxelContainer::~VoxelContainer()
{
   delete [] Voxels;
}

bool & VoxelContainer::At(size_t x, size_t y, size_t z)
{
   return Voxels[z*Width*Height + y*Width + x];
}

void VoxelContainer::SetSphere(Ogre::Vector3 pos, float r, bool b) {
   SetSphere(pos.x, pos.y, pos.z, r, b);
}
void VoxelContainer::SetSphere(float sx, float sy, float sz, float r, bool b)
{
   const int fromX = between(0, (int) (sx-r-1), ((int)Width)-1);
   const int fromY = between(0, (int) (sy-r-1), ((int)Height)-1);
   const int fromZ = between(0, (int) (sz-r-1), ((int)Depth)-1);

   const int toX = between(0, (int) (sx+r+1), ((int)Width)-1);
   const int toY = between(0, (int) (sy+r+1), ((int)Height)-1);
   const int toZ = between(0, (int) (sz+r+1), ((int)Depth)-1);

   std::cout << "\n" << std::endl;
   
   std::cout << "SetSphere: " << fromX << "," << fromY << "," << fromZ << std::endl;
   
   for (int x=fromX; x<=toX; x++)
      for (int y=fromY; y<=toY; y++)
         for (int z=fromZ; z<=toZ; z++)
         {
            const float dx = x+0.5 - sx;
            const float dy = y+0.5 - sy;
            const float dz = z+0.5 - sz;
            const float d = sqrt(dx*dx + dy*dy + dz*dz);

            if (d <= r)
            {
               At(x,y,z) = b;
               //std::cout << "set sphere voxel !" << std::endl;
            }

         }

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

