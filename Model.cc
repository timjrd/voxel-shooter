#include "Model.hh"

#include <cmath>

using namespace std;

Model::Vec3 Model::Vec3::operator+(const Vec3 & a) const {
   return {X+a.X, Y+a.Y, Z+a.Z};
}
void Model::Vec3::operator+=(const Vec3 & a) {
   X += a.X;
   Y += a.Y;
   Z += a.Z;
}
Model::Vec3 Model::Vec3::operator*(const float a) const {
   return {X*a, Y*a, Z*a};
}

Model::Vec3 Model::toCartesian(float theta, float phi)
{
   return { sin(phi) * sin(theta),
            cos(phi),
            sin(phi) * cos(theta)
         };
}

Model::Model(size_t meshSize, size_t width, size_t height, size_t depth, Observer * observer)
   : Observer_(observer)
   , Width(meshSize*width)
   , Height(meshSize*height)
   , Depth(meshSize*depth)
   , MeshSize(meshSize)
{
   Voxels = new bool[Width*Height*Depth];
   for (size_t i=0; i<Width*Height*Depth; i++)
      Voxels[i] = false;

   Player_.Pos = {0,0};// {Width/2.f, Height/2.f, Depth/2.f};
   Player_.Theta = 0;
   Player_.Phi   = -PI/2;
}

Model::~Model()
{
   delete [] Voxels;
}

bool & Model::At(size_t x, size_t y, size_t z)
{
   return Voxels[z*Width*Height + y*Width + x];
}

void Model::SetSphere(float sx, float sy, float sz, float r, bool b)
{
   const int fromX = between(0, (int) (sx-r-1), ((int)Width)-1);
   const int fromY = between(0, (int) (sy-r-1), ((int)Height)-1);
   const int fromZ = between(0, (int) (sz-r-1), ((int)Depth)-1);

   const int toX = between(0, (int) (sx+r+1), ((int)Width)-1);
   const int toY = between(0, (int) (sy+r+1), ((int)Height)-1);
   const int toZ = between(0, (int) (sz+r+1), ((int)Depth)-1);

   for (int x=fromX; x<=toX; x++)
      for (int y=fromY; y<=toY; y++)
         for (int z=fromZ; z<=toZ; z++)
         {
            const float dx = x+0.5 - sx;
            const float dy = y+0.5 - sy;
            const float dz = z+0.5 - sz;
            const float d = sqrt(sx*sx + sy*sy + sz*sz);

            At(x,y,z) = d <= r;
         }

   if (Observer_)
      for (int x=fromX/MeshSize; x<=toX/MeshSize; x++)
         for (int y=fromY/MeshSize; y<=toY/MeshSize; y++)
            for (int z=fromZ/MeshSize; z<=toZ/MeshSize; z++)
            {
               vector<Quad> mesh;
               ExtractMesh(x,y,z,mesh);
               Observer_->UpdateMesh(x,y,z,mesh);
            }
}
void Model::SetSphere(Vec3 pos, float r, bool b) {
   SetSphere(pos.X, pos.Y, pos.Z, r, b);
}

void Model::PlayerFill() {
   SetSphere(Player_.Pos + toCartesian(Player_.Theta,Player_.Phi) * 18, 6, true);
}
void Model::PlayerEmpty() {
   SetSphere(Player_.Pos + toCartesian(Player_.Theta,Player_.Phi) * 18, 6, false);
}


void Model::ExtractMesh(size_t mx, size_t my, size_t mz, std::vector<Quad> & res)
{
   for(size_t x=mx*MeshSize; x < mx*MeshSize+MeshSize; x++)
      for(size_t y=my*MeshSize; y < my*MeshSize+MeshSize; y++)
         for(size_t z=mx*MeshSize; z < mz*MeshSize+MeshSize; z++)
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




void Model::MovePlayer(float theta, float phi, float time)
{
   const float theta_ = Player_.Theta + theta;
   const float phi_   = Player_.Phi   + phi;

   Player_.Pos += toCartesian(theta_, phi_) * time*PlayerSpeed;

   if (Observer_)
      Observer_->UpdatePlayer(Player_);
}

void Model::RotatePlayer(float theta, float phi)
{
   float _;
   Player_.Theta = modf((Player_.Theta + theta)/(2*PI), &_)*2*PI;
   Player_.Phi   = modf((Player_.Phi + phi)/(2*PI), &_)*2*PI;

   //Player_.Theta += theta;
   //Player_.Phi   += phi;
   
   if (Observer_)
      Observer_->UpdatePlayer(Player_);
}

void Model::MovePlayerLeft(float time) {
   MovePlayer(PI/2, 0, time);
}
void Model::MovePlayerRight(float time) {
   MovePlayer(-PI/2, 0, time);
}
void Model::MovePlayerForward(float time) {
   MovePlayer(0, 0, time);
}
void Model::MovePlayerBackward(float time) {
   MovePlayer(0, PI, time);
}
void Model::MovePlayerUp(float time) {
   MovePlayer(0, -PI/2, time);
}
void Model::MovePlayerDown(float time) {
   MovePlayer(0, PI/2, time);
}
