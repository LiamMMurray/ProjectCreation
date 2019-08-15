
        // GamePad is connected
        if (GamePad::Get()->CheckConnection() == true)
        {
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::MainMenu,
                                  E_FONT_TYPE::Calibri,
                                  "Press the Back Button to continue. . .",
                                  0.04f,
                                  0.0f,
                                  0.15f,
                                  true,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::MainMenu,
                                  E_FONT_TYPE::Calibri,
                                  "Hold Right Trigger to Move",
                                  0.04f,
                                  0.0f,
                                  0.15f,
                                  false,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::MainMenu,
                                  E_FONT_TYPE::Calibri,
                                  "Hold the B Button to collect Red lights",
                                  0.04f,
                                  0.0f,
                                  0.15f,
                                  false,
                                  false);
                instance->AddText(instance->m_RenderSystem->m_Device,
                                  instance->m_RenderSystem->m_Context,
                                  E_MENU_CATEGORIES::MainMenu,
                                  E_FONT_TYPE::Calibri,
                                  "Hold the A Button to collect Green lights",
                                  0.04f,
                                  0.0f,
                                  0.15f,
                                  false,
                                  false);
                                        XMFLOAT2 cursorCoords = {e.mouseX, e.mouseY};
                                        XMVECTOR point        = UI::ConvertScreenPosToNDC(cursorCoords, instance->m_ScreenSize);