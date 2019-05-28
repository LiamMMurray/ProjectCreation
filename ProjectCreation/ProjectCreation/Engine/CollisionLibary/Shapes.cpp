#include "Shapes.h"
#include "BVH.h"
#include "CollisionLibary.h"
using namespace Shapes;
using namespace DirectX;
using namespace Collision;

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

FSphere Shapes::CreateFSphere(DirectX::XMVECTOR center, float radius)
{
        FSphere output = FSphere(center, radius);
        FAabb   box    = CollisionLibary::CreateBoundingBoxFromShpere(output);
        bvhTree.InsertAABB(box, BvhIndex + 1); //
        return output;
}

FAabb Shapes::CreateFAabb(DirectX::XMVECTOR center, DirectX::XMVECTOR extents)
{
        FAabb output = FAabb(center, extents);
        bvhTree.InsertAABB(output, BvhIndex + 1);
        return output;
}

FCapsule Shapes::CreateFCapsule(DirectX::XMVECTOR startPoint, DirectX::XMVECTOR endPoint, float radius)
{

        FCapsule output = FCapsule(startPoint, endPoint, radius);
        FAabb    temp   = CollisionLibary::CreateBoundingBoxFromCapsule(output);
        bvhTree.InsertAABB(temp, BvhIndex + 1);
        return output;
}

Shapes::FSphere::FSphere()
{}

Shapes::FSphere::FSphere(DirectX::XMVECTOR center, float radius)
{
        FSphere::center = center;
        FSphere::radius = radius;
}

Shapes::FAabb::FAabb()
{}

Shapes::FAabb::FAabb(DirectX::XMVECTOR center, DirectX::XMVECTOR extents)
{
        FAabb::center  = center;
        FAabb::extents = extents;
}

int Shapes::BvhIndex = 0;