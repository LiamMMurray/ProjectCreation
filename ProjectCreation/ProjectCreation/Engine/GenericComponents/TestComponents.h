#pragma once
#include "../ECS/Component.h"
namespace ECSTests
{
        class SomeOtherClass
        {
            public:
                float other_data;

                void Foo()
                {
                        other_data += 2;
                }
        };


        class Transform : public Component<Transform>, public SomeOtherClass
        {
            public:
                float some_data;

                void Bar()
                {
                        some_data = 42;
                }
        };

        class Mesh : public Component<Mesh>
        {
            public:
                float some_data;

                void Init()
                {
                        some_data = 27;
                }
        };

} // namespace ECSTests
