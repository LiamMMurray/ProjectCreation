#pragma once

#include <d3d11.h>
#include <Component.h>
#include "../../Events/Event.h"
#include "../../Events/EventDelegate.h"
#include "../../Events/EventManager.h"
#include "UICollision.h"

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
        DirectX::SimpleMath::Vector2 mScreenOffset;
        DirectX::SimpleMath::Vector2 mOrigin;
        unsigned int                 mHeight;
        unsigned int                 mWidth;
        float                        mScaleX;
        float                        mScaleY;

		DirectX::XMVECTOR currColor = {1,1,1,1};
		DirectX::XMVECTOR desiredColor = {1,1,1,1};

        ID3D11ShaderResourceView* mTexture;
        UI::UIRect                mRectangle;

        bool mEnabled;

        void SetPosition(float x, float y);

        UIMouseEventManager OnMouseDown;
        UIMouseEventManager OnMouseUp;
        UIMouseEventManager OnMouseOver;
        UIMouseEventManager OnMouseOut;
};