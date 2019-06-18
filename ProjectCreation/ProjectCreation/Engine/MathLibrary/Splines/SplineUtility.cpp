#include "SplineUtility.h"
#include "..//MathLibrary.h"

using namespace DirectX;

std::vector<DirectX::XMVECTOR> SplineUtility::BakePointsFromSplineAndTransform(FGoodSpline&             spline,
                                                                               const DirectX::XMVECTOR& start,
                                                                               const DirectX::XMVECTOR& end,
                                                                               float                    width,
                                                                               unsigned int             subdivisions,
                                                                               unsigned int             heightWaveCount,
                                                                               float                    maxHeightVariance)
{
        std::vector<DirectX::XMVECTOR> output;
        output.reserve(subdivisions - 1);
        spline.ResetPointer();

        XMVECTOR   dir      = end - start;
        float      distance = MathLibrary::CalulateVectorLength(dir);
        XMVECTOR   scale    = XMVectorSet(width, 1.0f, distance, 1.0f);
        FTransform transform;
        transform.translation = start;
        transform.rotation    = FQuaternion::LookAt(XMVector3Normalize(dir));
        transform.scale       = scale;

        XMMATRIX mtx        = transform.CreateMatrix();
        float    baseLength = spline.GetLength();
        for (int i = 0; i < subdivisions - 1; ++i)
        {
                spline.AdvanceDistance(baseLength / subdivisions);
                XMVECTOR point = spline.GetCurrentPoint();
                point          = XMVector3Transform(point, mtx);
                point = XMVectorSetY(point, maxHeightVariance * sinf(heightWaveCount * XM_PIDIV2 * (float)i / subdivisions));
                output.push_back(point);
        }
        spline.ResetPointer();
        return output;
}
