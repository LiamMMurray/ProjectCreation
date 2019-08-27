#include "debug_renderer.h"
#include <array>
#include "../../Engine/MathLibrary/ColorConstants.h"
#include "../../Utility/MemoryDefines.h"

using namespace DirectX;

// Anonymous namespace
namespace
{
        // Declarations in an anonymous namespace are global BUT only have internal linkage.
        // In other words, these variables are global but are only visible in this source file.

        // Maximum number of debug lines at one time (i.e: Capacity)
        constexpr size_t MAX_LINE_VERTS = MB(4) / sizeof(FDebugVertex);

        // CPU-side buffer of debug-line verts
        // Copied to the GPU and reset every frame.
        size_t       line_vert_count = 0;
        FDebugVertex line_verts[MAX_LINE_VERTS];
} // namespace

namespace debug_renderer
{
        void add_line(const DirectX::XMVECTOR& point_a,
                      const DirectX::XMVECTOR& point_b,
                      const DirectX::XMFLOAT4& color_a,
                      const DirectX::XMFLOAT4& color_b)
        {
                // Add points to debug_verts, increments debug_vert_count
                XMFLOAT3 pA;
                XMFLOAT3 pB;

                XMStoreFloat3(&pA, point_a);
                XMStoreFloat3(&pB, point_b);

                line_verts[line_vert_count]     = FDebugVertex(pA, color_a);
                line_verts[line_vert_count + 1] = FDebugVertex(pB, color_b);

                line_vert_count += 2;
        }

        void AddGrid(DirectX::XMVECTOR center, float width, int segments, DirectX::XMFLOAT4 color)
        {
                using namespace DirectX;
                XMFLOAT3 _center;

                XMStoreFloat3(&_center, center);
                // vertical lines

                for (int i = 0; i < segments; i++)
                {
                        XMFLOAT3 a = _center;
                        a.x += -width / 2 + i * (width / (segments - 1));
                        a.z += -width / 2;
                        XMFLOAT3 b = _center;
                        b.x += -width / 2 + i * (width / (segments - 1));
                        b.z += width / 2;
                        add_line(XMLoadFloat3(&a), XMLoadFloat3(&b), color);
                }

                // horizontal lines
                for (int i = 0; i < segments; i++)
                {
                        XMFLOAT3 a = _center;
                        a.z += -width / 2 + i * (width / (segments - 1));
                        a.x += -width / 2;
                        XMFLOAT3 b = _center;
                        b.z += -width / 2 + i * (width / (segments - 1));
                        b.x += width / 2;
                        add_line(XMLoadFloat3(&a), XMLoadFloat3(&b), color);
                }
        }

        void AddBoneHierarchy(const Animation::FSkeleton& skel,
                              const DirectX::XMMATRIX*    globaltransforms,
                              const DirectX::XMMATRIX&    parent,
                              DirectX::XMFLOAT4           color,
                              float                       axisLength)
        {
                int n = (int)skel.jointTransforms.size();

                for (int i = 0; i < n; i++)
                {
                        int32_t parentID = skel.jointTransforms[i].parent_index;
                        AddMatrix(globaltransforms[i] * parent, axisLength);
                        if (parentID != -1)
                        {
                                XMVECTOR start = XMVector3Transform(globaltransforms[i].r[3], parent);
                                XMVECTOR end   = XMVector3Transform(globaltransforms[parentID].r[3], parent);

                                add_line(start, end, color);
                        }
                }
        }

        void AddTransform(const FTransform& transform, const DirectX::XMMATRIX& parent, float axisLength)
        {
                AddMatrix(transform.CreateMatrix() * parent, axisLength);
        }

        void AddMatrix(const DirectX::XMMATRIX& transform, float axisLength)
        {
                add_line(transform.r[3], transform.r[3] + transform.r[0] * axisLength, ColorConstants::Red);
                add_line(transform.r[3], transform.r[3] + transform.r[1] * axisLength, ColorConstants::Green);
                add_line(transform.r[3], transform.r[3] + transform.r[2] * axisLength, ColorConstants::Blue);
        }

        void AddBox(const Shapes::FAabb& aabb, DirectX::XMMATRIX parent, const DirectX::XMFLOAT4& color)
        {
                using namespace DirectX;

                XMFLOAT3 extents;
                XMStoreFloat3(&extents, aabb.extents);
                XMVECTOR points[8];

                points[0] = -aabb.extents;
                points[1] = points[0] + XMVectorSet(extents.x, 0.0f, 0.0f, 0.0f);
                points[2] = points[0] + XMVectorSet(0.0f, extents.y, 0.0f, 0.0f);
                points[3] = points[0] + XMVectorSet(extents.x, extents.y, 0.0f, 0.0f);

                points[4] = points[0] + XMVectorSet(0.0f, 0.0f, extents.z, 0.0f);
                points[5] = points[1] + XMVectorSet(0.0f, 0.0f, extents.z, 0.0f);
                points[6] = points[2] + XMVectorSet(0.0f, 0.0f, extents.z, 0.0f);
                points[7] = points[3] + XMVectorSet(0.0f, 0.0f, extents.z, 0.0f);

                for (int i = 0; i < 8; ++i)
                {
                        points[i] = XMVector3Transform(points[i], parent) + aabb.center;
                }

                add_line(points[0], points[1], color);
                add_line(points[0], points[2], color);
                add_line(points[1], points[3], color);
                add_line(points[2], points[3], color);

                add_line(points[0], points[4], color);
                add_line(points[1], points[5], color);
                add_line(points[2], points[6], color);
                add_line(points[3], points[7], color);

                add_line(points[4], points[5], color);
                add_line(points[4], points[6], color);
                add_line(points[5], points[7], color);
                add_line(points[6], points[7], color);
        }

        void AddSphere(const Shapes::FSphere& sphere, int segments, DirectX::XMMATRIX parent)
        {
                using namespace DirectX;

                float step = 2 * XM_PI / segments;

                float    prevX = 0.0f;
                float    prevY = sphere.radius;
                XMVECTOR points[6];
                for (float angle = 0; angle <= 360.0f; angle += 2 * XM_PI / segments)
                {
                        float currX = sin(angle) * sphere.radius;
                        float currY = cos(angle) * sphere.radius;

                        points[0] = XMVectorSet(0.0f, prevX, prevY, 0.0f);
                        points[1] = XMVectorSet(0.0f, currX, currY, 0.0f);

                        points[2] = XMVectorSet(prevX, 0.0f, prevY, 0.0f);
                        points[3] = XMVectorSet(currX, 0.0f, currY, 0.0f);

                        points[4] = XMVectorSet(prevX, prevY, 0.0f, 0.0f);
                        points[5] = XMVectorSet(currX, currY, 0.0f, 0.0f);

                        for (int i = 0; i < 6; ++i)
                        {
                                points[i] = XMVector3Transform(points[i], parent) + sphere.center;
                        }

                        add_line(points[0], points[1], ColorConstants::Red);
                        add_line(points[2], points[3], ColorConstants::Green);
                        add_line(points[4], points[5], ColorConstants::Blue);

                        prevX = currX;
                        prevY = currY;
                }
        }

        void AddSphereColored(const Shapes::FSphere&   sphere,
                              int                      segments,
                              DirectX::XMMATRIX        parent,
                              const DirectX::XMFLOAT4& color)
        {
                using namespace DirectX;

                float step = 2 * XM_PI / segments;

                float    prevX = 0.0f;
                float    prevY = 1.0f;
                XMVECTOR points[6];
                for (float angle = 0; angle <= 360.0f; angle += 2 * XM_PI / segments)
                {
                        float currX = sin(angle) * sphere.radius;
                        float currY = cos(angle) * sphere.radius;

                        points[0] = XMVectorSet(0.0f, prevX, prevY, 0.0f);
                        points[1] = XMVectorSet(0.0f, currX, currY, 0.0f);

                        points[2] = XMVectorSet(prevX, 0.0f, prevY, 0.0f);
                        points[3] = XMVectorSet(currX, 0.0f, currY, 0.0f);

                        points[4] = XMVectorSet(prevX, prevY, 0.0f, 0.0f);
                        points[5] = XMVectorSet(currX, currY, 0.0f, 0.0f);

                        for (int i = 0; i < 6; ++i)
                        {
                                points[i] = XMVector3Transform(points[i], parent) + sphere.center;
                        }

                        add_line(points[0], points[1], color);
                        add_line(points[2], points[3], color);
                        add_line(points[4], points[5], color);

                        prevX = currX;
                        prevY = currY;
                }
        }

        void clear_lines()
        {
                // Resets debug_vert_count
                line_vert_count = 0;
        }

        const FDebugVertex* get_line_verts()
        {
                // Does just what it says in the name
                return line_verts;
        }

        size_t get_line_vert_count()
        {
                // Does just what it says in the name
                return line_vert_count;
        }

        size_t get_line_vert_capacity()
        {
                // Does just what it says in the name
                return MAX_LINE_VERTS;
        }
} // namespace debug_renderer