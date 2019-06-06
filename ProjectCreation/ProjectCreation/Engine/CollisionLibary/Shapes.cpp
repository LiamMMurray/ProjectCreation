#include "Shapes.h"
#include "BVH.h"
#include "CollisionHelpers.h"
#include "CollisionLibary.h"
using namespace Shapes;
using namespace DirectX;

Shapes::FCapsule::FCapsule()
{}

Shapes::FCapsule::FCapsule(DirectX::XMVECTOR startPoint, DirectX::XMVECTOR endPoint, float radius)
{
        FCapsule::startPoint = startPoint;
        FCapsule::endPoint   = endPoint;
        FCapsule::radius     = radius;
}

float FCapsule::GetCapsuleDistance()
{
        return XMVectorGetX(XMVector3Dot(FCapsule::startPoint, FCapsule::endPoint));
}

ECollisionObjectTypes Shapes::FCapsule::GetID() const
{
        return ECollisionObjectTypes::Capsule;
}

FSphere Shapes::CreateFSphere(DirectX::XMVECTOR center, float radius)
{
        FSphere          output = FSphere(center, radius);
        FAabb            box    = CollisionLibary::CreateBoundingBoxFromShpere(output);

        return output;
}

FAabb Shapes::CreateFAabb(DirectX::XMVECTOR center, DirectX::XMVECTOR extents)
{
        FAabb output = FAabb(center, extents);
        return output;
}

FCapsule Shapes::CreateFCapsule(DirectX::XMVECTOR startPoint, DirectX::XMVECTOR endPoint, float radius)
{

        FCapsule output = FCapsule(startPoint, endPoint, radius);
        FAabb    temp   = CollisionLibary::CreateBoundingBoxFromCapsule(output);
        return output;
}

Shapes::FSphere::FSphere()
{}

Shapes::FSphere::FSphere(DirectX::XMVECTOR center, float radius)
{
        FSphere::center = center;
        FSphere::radius = radius;
}

ECollisionObjectTypes Shapes::FSphere::GetID() const
{
        return ECollisionObjectTypes::Sphere;
}

Shapes::FAabb::FAabb()
{}

Shapes::FAabb::FAabb(DirectX::XMVECTOR center, DirectX::XMVECTOR extents)
{
        FAabb::center  = center;
        FAabb::extents = extents;
}

ECollisionObjectTypes Shapes::FAabb::GetID() const
{
        return ECollisionObjectTypes::Aabb;
}

ECollisionObjectTypes Shapes::FPlane::GetID() const
{
        return ECollisionObjectTypes::Plane;
}
