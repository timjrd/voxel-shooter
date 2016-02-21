#pragma once

#include <OgreMath.h>
#include <OgreLight.h>

//typedef long long integer;

const float PI = Ogre::Math::PI;

void setLightAttenuation(Ogre::Light & light, const float range);
int linearInterpolation(int from, int to, int x, int xMax);

template<typename T>
T between(T a, T x, T b) {
   return std::max(a, std::min(x, b));
}

class Random
{
private:
   unsigned long long Previous;

public:
   Random(unsigned long long seed);   
   int Next(int min, int max);
};
