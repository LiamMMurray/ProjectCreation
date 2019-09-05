#include <FGoodSpline.h>
#include <MathLibrary.h>

FGoodSpline::FGoodSpline(const std::vector<DirectX::XMVECTOR>& points)
{
        m_Spline = Spline<DirectX::XMVECTOR, float>(3, spline::eUNIFORM);

        length      = 0.0f;
        size_t size = points.size();

        for (size_t i = 1; i < size; ++i)
        {
                length += MathLibrary::CalulateDistance(points[i], points[i - 1]);
        }
		
        m_Spline.set_ctrl_points(points);


        pointer = 0.0f;
}

DirectX::XMVECTOR FGoodSpline::GetCurrentPoint()
{
        return m_Spline.eval_f(pointer / length);
}

DirectX::XMVECTOR FGoodSpline::GetPointAtTime(float time)
{
        return m_Spline.eval_f(time);
}

DirectX::XMVECTOR FGoodSpline::GetSpeedAtTime(float time)
{
        return m_Spline.eval_df(time);
}

void FGoodSpline::AdvanceDistance(float distance)
{
        pointer += distance;
        pointer = MathLibrary::clamp(pointer, 0.0f, length);
}
