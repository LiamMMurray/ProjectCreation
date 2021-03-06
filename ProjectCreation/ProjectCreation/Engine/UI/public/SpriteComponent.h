#pragma once

#include <Component.h>
#include <Event.h>
#include <EventDelegate.h>
#include <EventManager.h>
#include <UICollision.h>
#include <d3d11.h>

#include <SimpleMath.h>
#include <SpriteFont.h>

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
        DirectX::SimpleMath::Vector2 mScreenOffset;
        DirectX::SimpleMath::Vector2 mOrigin;
        unsigned int                 mHeight;
        unsigned int                 mWidth;
        float                        mScaleX;
        float                        mScaleY;

        DirectX::XMVECTOR currColor    = {1, 1, 1, 1};
        DirectX::XMVECTOR desiredColor = {1, 1, 1, 1};

        ID3D11ShaderResourceView* mTexture;
        ID3D11ShaderResourceView* mAltTexture = nullptr;
        UI::UIRect                mRectangle;

        bool mEnabled;

        void SetPosition(float x, float y);

        UIMouseEventManager OnMouseDown;
        UIMouseEventManager OnMouseUp;
        UIMouseEventManager OnMouseOver;
        UIMouseEventManager OnMouseOut;
};