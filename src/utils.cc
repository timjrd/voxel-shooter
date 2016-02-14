#include "utils.hh"

void setLightAttenuation(Ogre::Light & light, const float range)
{
   const float L   = 0;
   const float min = 0.04;
   const float max = 1;
   
   const float K   = 1/max;
   const float q   = (1/min - 1/max - L*range) / (range*range);

   light.setAttenuation(range,K,L,q);
}

int linearInterpolation(int from, int to, int x, int xMax)
{
   return ((to-from) * x)/xMax + from;
}

Random::Random(unsigned long long seed)
   : Previous(seed)
{}
   
int Random::Next(int min, int max)
{
   Previous = (1103515245 * Previous + 12345) % 2147483648;

   return (Previous % (max-min+1)) + min;
}

