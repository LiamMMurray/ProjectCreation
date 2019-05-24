#include "Vector.h"

using namespace DirectX;

const XMVECTOR VectorConstants::Forward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
const XMVECTOR VectorConstants::Right   = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
const XMVECTOR VectorConstants::Up      = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
const XMVECTOR VectorConstants::Zero    = XMVectorSet(.0f, 0.0f, 0.0f, 0.0f);