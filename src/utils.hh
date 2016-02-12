#pragma once

#include <OgreLight.h>

template<typename T>
T between(T a, T x, T b) {
   return std::max(a, std::min(x, b));
}

void setLightAttenuation(Ogre::Light & light, const float range);
