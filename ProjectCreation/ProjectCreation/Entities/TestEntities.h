#pragma once
#include "../ECS/Entity.h"
namespace ECSTests
{
        class Wisp : public Entity<Wisp>
        {};

        class StaticModel : public Entity<StaticModel>
        {};

        class Creature : public Entity<Creature>
        {};

        class Key : public Entity<Key>
        {};

        class Crate : public Entity<Crate>
        {};

        class Skeleton : public Entity<Skeleton>
        {};

        class Target : public Entity<Target>
        {};
} // namespace ECSTests