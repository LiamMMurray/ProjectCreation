#pragma once
#include "../ECS/Component.h"
namespace ECSTests
{
        namespace Components
        {
                class NotCRTPComponent
                {
                    public:
                        float other_data;

                        void Foo()
                        {
                                other_data += 2;
                        }
                };


                class Transform : public Component<Transform>, public NotCRTPComponent
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

                class TexasSwagger : public Component<TexasSwagger>
                {};


                class Monday : public Component<Monday>
                {};

                class Brocoli : public Component<Brocoli>
                {};
        } // namespace Components
} // namespace ECSTests
