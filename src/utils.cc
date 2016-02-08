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

