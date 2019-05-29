#pragma once

#include <d3d11.h>
#include "../Engine/Events/Event.h"
#include "../Engine/Events/EventDelegate.h"
#include "../Engine/Events/EventManager.h"
#include "../ECS/Component.h"

#include "SimpleMath.h"
#include "SpriteFont.h"
class SpriteComponent;
class UIMouseEvent : public Event<UIMouseEvent>
{
    public:
        SpriteComponent* sprite;
        float            mouseX;
        float            mouseY;
        bool             test;
};
class UIMouseEventDelegate : public EventDelegate<UIMouseEventDelegate, UIMouseEvent>
{
    public:
        UIMouseEventDelegate(void (*delegateFunction)(UIMouseEvent*)) : EventDelegate(delegateFunction)
        {}
};
class UIMouseEventManager : public EventManager<UIMouseEventDelegate, UIMouseEvent>
{};

class SpriteComponent : public Component<SpriteComponent>
{
    public:
        DirectX::SimpleMath::Vector2          mScreenPos;
        DirectX::SimpleMath::Vector2          mOrigin;
        unsigned int                          mHeight;
        unsigned int                          mWidth;
        ID3D11ShaderResourceView*             mTexture;
        RECT							      mRectangle;
        bool                                  mEnabled;
        unsigned int                          mId;

        void MakeRectangle();

        void SetPosition(float x, float y);
        void TransformPosition(float x, float y);
        void ScaleSprite(float scale);
        void RotateSprite(float degree);

        UIMouseEventManager OnMouseDown;
        UIMouseEventManager OnMouseUp;
        UIMouseEventManager OnMouseOver;
        UIMouseEventManager OnMouseOut;
};