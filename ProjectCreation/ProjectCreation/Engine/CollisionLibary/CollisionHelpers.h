#pragma once

#include "../../ECS/ECSTypes.h"
#include "Shapes.h"

namespace Collision
{


        void CreateSphere(const Shapes::FSphere& fSphere,
                          EntityHandle           entityH,
                          ComponentHandle*       sphereHandle = nullptr,
                          ComponentHandle*       aabbHAndle   = nullptr);
        void CreateAABB(const Shapes::FAabb& fAABB, EntityHandle entityH, ComponentHandle* aabbHandle = nullptr);
        void CreateCapsule(const Shapes::FCapsule& fCapsule,
                           EntityHandle            entityH,
                           ComponentHandle*        capsuleHandle = nullptr,
                           ComponentHandle*        aabbHAndle    = nullptr);

} // namespace Collision