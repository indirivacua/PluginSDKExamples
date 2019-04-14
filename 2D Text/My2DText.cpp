#include "plugin.h"
#include "common.h"
#include "CFont.h"

using namespace plugin;

class My2DText {
public:
    My2DText() {
        // Initialise your plugin here
        
        Events::drawingEvent += [] {

            CPlayerPed* pLocalPlayer = FindPlayerPed();
            if (pLocalPlayer)
            {
                CVector position = pLocalPlayer->GetPosition();
                position.y += 5.0f;

                plugin::gamefont::PrintAt3d(position, 
                    Format("%.2f %.2f %.2f", position.x, position.y, position.z),
                    0, 0, FONT_DEFAULT, 2, 2, color::Pink, true, gamefont::AlignCenter);

                plugin::gamefont::Print(plugin::gamefont::ScreenSide::LeftTop, gamefont::AlignLeft,
                    Format("Hello, my friend"), SCREEN_COORD_LEFT(300.0f), SCREEN_COORD_TOP(200.0f), 
                    FONT_DEFAULT, 2, 2, CRGBA(255, 255, 255, 255),
                    1.0f, CRGBA(0, 0, 0, 255), false, 9999.0f, true, false);
            }

            CFont::SetColor(CRGBA(255, 255, 255, 255));
            CFont::SetDropShadowPosition(0);

#ifdef GTASA
            static char myText[30];
            CFont::SetBackground(false, false);
            CFont::SetProportional(true);
            CFont::SetOrientation(ALIGN_CENTER);
            CFont::SetFontStyle(1);
#else
            static wchar_t myText[30];
            CFont::SetBackgroundOff();
            CFont::SetPropOn();
            CFont::SetCentreOn();
            CFont::SetFontStyle(0);
#endif

            CFont::SetCentreSize(SCREEN_COORD_MAX_X);
            CFont::SetScale(SCREEN_MULTIPLIER(1.2f), SCREEN_MULTIPLIER(2.4f));

            float fX = SCREEN_COORD_MAX_X / 2;
            float fY = SCREEN_COORD_MAX_Y / 2;

            float fHealth = pLocalPlayer ? pLocalPlayer->m_fHealth : 0.0f;

#ifdef GTASA
            sprintf(myText, "Health = %f", fHealth);
            CFont::PrintString(fX, fY, myText);
#else
            swprintf(myText, L"Health = %f", fHealth);
            CFont::PrintString(fX, fY, myText);
#endif
        };
    }
} my2DText;
