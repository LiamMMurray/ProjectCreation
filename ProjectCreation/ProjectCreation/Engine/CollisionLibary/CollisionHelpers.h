#pragma once

#include <ECSTypes.h>
#include <HandleManager.h>
#include "Shapes.h"

class CollisionGrid;
namespace Collision
{


        void CreateSphere(CollisionGrid*         grid,
                          const Shapes::FSphere& fSphere,
                          EntityHandle           entityH,
                          ComponentHandle*       sphereHandle = nullptr,
                          ComponentHandle*       aabbHAndle   = nullptr);
        void CreateAABB(CollisionGrid*       grid,
                        const Shapes::FAabb& fAABB,
                        EntityHandle         entityH,
                        ComponentHandle*     aabbHandle = nullptr);
        void CreateCapsule(CollisionGrid*          grid,
                           const Shapes::FCapsule& fCapsule,
                           EntityHandle            entityH,
                           ComponentHandle*        capsuleHandle = nullptr,
                           ComponentHandle*        aabbHAndle    = nullptr);

} // namespace Collision