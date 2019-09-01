#pragma once
#include "../../Engine/Animation/AnimationContainers.h"
#include "../../Engine/CollisionLibary/CollisionResult.h"
#include "../../Engine/MathLibrary/ColorConstants.h"
#include "../../Engine/MathLibrary/MathLibrary.h"
#include <Vertex.h>

// Interface to the debug renderer
namespace debug_renderer
{
        void add_line(const DirectX::XMVECTOR& point_a,
                      const DirectX::XMVECTOR& point_b,
                      const DirectX::XMFLOAT4& color_a,
                      const DirectX::XMFLOAT4& color_b);

        inline void add_line(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& q, const DirectX::XMFLOAT4& color)
        {
                add_line(p, q, color, color);
        }

		void AddGrid(DirectX::XMVECTOR center, float width, int segments, DirectX::XMFLOAT4 color);

        void AddBoneHierarchy(const Animation::FSkeleton& skel,
                              const DirectX::XMMATRIX*    globaltransforms,
                              const DirectX::XMMATRIX&    parent,
                              DirectX::XMFLOAT4           color,
                              float                       axisLength = 1.0f);

        void AddTransform(const FTransform& transform, const DirectX::XMMATRIX& parent, float axisLength = 1.0f);
        void AddMatrix(const DirectX::XMMATRIX& transform, float axisLength = 1.0f);

        void AddBox(const Shapes::FAabb& aabb, DirectX::XMMATRIX parent, const DirectX::XMFLOAT4& color);

        void AddSphere(const Shapes::FSphere& sphere, int segments, DirectX::XMMATRIX parent);

        void AddSphereColored(const Shapes::FSphere&   sphere,
                              int                                segments,
                              DirectX::XMMATRIX                  parent,
                              const DirectX::XMFLOAT4&           color = ColorConstants::White);

        void clear_lines();

        const FDebugVertex* get_line_verts();

        size_t get_line_vert_count();

        size_t get_line_vert_capacity();
} // namespace debug_renderer