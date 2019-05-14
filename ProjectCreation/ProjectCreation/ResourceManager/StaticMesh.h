#pragma once

#include "Resource.h"

struct StaticMesh : public Resource<StaticMesh>
{
        virtual void Release() override;
};