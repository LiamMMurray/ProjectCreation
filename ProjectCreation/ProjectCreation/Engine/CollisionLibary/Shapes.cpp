#include "Shapes.h"
using namespace Shapes;
using namespace DirectX;

float FCapsule::GetCapsuleDistance()
{
        return XMVectorGetX(XMVector3Dot(FCapsule::startPoint, FCapsule::endPoint));
}
