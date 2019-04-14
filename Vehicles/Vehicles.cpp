#include "plugin.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "CAutoMobile.h"
#include "InputManager.h"

using namespace plugin;

class Vehicles {
public:
    Vehicles() {
        // Initialise your plugin here
        Events::initGameEvent += [&]
        {
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Failed to initialize input manager", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(
                std::bind(&Vehicles::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2)
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
                    CVector position = pLocalPlayer->GetPosition();
                    position.y += 5.0f;

#ifdef GTASA
                    int vehicleID = 432; // tank
                    CStreaming::RequestModel(vehicleID, 0x16);
                    CStreaming::LoadAllRequestedModels(0);
                    CVehicle* pVehicle = new CAutomobile(vehicleID, MISSION_VEHICLE, true);
                    pVehicle->SetOrientation(0.0f, 0.0f, 0.0f);
                    pVehicle->SetPosn(position);
                    pVehicle->m_nDoorLock = 1; // unlocked
                    pVehicle->m_nStatus = 4;
#elif GTAVC
                    int vehicleID = 162; // tank
                    CStreaming::RequestModel(vehicleID, 0x16);
                    CStreaming::LoadAllRequestedModels(0);
                    CVehicle* pVehicle = new CAutomobile(vehicleID, MISSION_VEHICLE);
                    pVehicle->m_placement.SetRotate(0.0f, 0.0f, 0.0f);
                    pVehicle->m_placement.pos = position;
                    pVehicle->m_nLockStatus = 1; //unlocked
                    pVehicle->m_nState = 4;
#else
                    int vehicleID = 122; // tank
                    CStreaming::RequestModel(vehicleID, 0x16);
                    CStreaming::LoadAllRequestedModels(0);
                    CVehicle* pVehicle = new CAutomobile(vehicleID, MISSION_VEHICLE);
                    pVehicle->SetOrientation(0.0f, 0.0f, 0.0f);
                    pVehicle->SetPosition(position);
                    pVehicle->m_nDoorLock = 1; // unlocked
                    pVehicle->m_nState = 4;
#endif
                    pVehicle->m_nType &= 7;
                    CWorld::Add(pVehicle);
                }
            }
            }
        }
    }

private:
    InputManager theInputManager;
} vehicles;
