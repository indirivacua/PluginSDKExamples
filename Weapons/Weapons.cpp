#include "plugin.h"
#include "CPed.h"
#include "CStreaming.h"
#include "eWeaponType.h"
#ifndef GTASA
#include "eWeaponModel.h"
#endif
#include "InputManager.h"

using namespace plugin;

class Weapons {
public:
    Weapons() {
        // Initialise your plugin here
        Events::initGameEvent += [&]
        {
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Failed to initialize input manager", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(
                std::bind(&Weapons::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2)
            );
        };

        Events::gameProcessEvent += [&]
        {
            theInputManager.ProcessInput();
        };
    }

    void OnKeyPress(unsigned char key, PressState state)
    {
        if (state == PRESS_DOWN)
        {
            switch (key)
            {
            case DIK_0:
            {
                CPlayerPed* pLocalPlayer = FindPlayerPed();
                if (pLocalPlayer)
                {
#ifdef GTASA
                    int weaponID = MODEL_MICRO_UZI;
                    eWeaponType weaponType = WEAPON_MICRO_UZI;
                    CStreaming::RequestModel(weaponID, 0xC);
#else
                    int weaponID = MODEL_UZI;
                    eWeaponType weaponType = WEAPONTYPE_UZI;
                    CStreaming::RequestModel(weaponID, 0x16);
#endif
                    CStreaming::LoadAllRequestedModels(0);

#ifdef GTA3
                    pLocalPlayer->GiveWeapon(weaponType, 1000);
#else
                    pLocalPlayer->GiveWeapon(weaponType, 1000, true);
#endif
                    pLocalPlayer->SetCurrentWeapon(weaponType);
                    CStreaming::SetModelIsDeletable(weaponID);
                }
            }
            }
        }
    }

private:
    InputManager theInputManager;
} weapons;
