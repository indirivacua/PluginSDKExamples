#include "plugin.h"
#include "InputManager.h"
#include "CStreaming.h"
#include "CCivilianPed.h"
#include "CWorld.h"

using namespace plugin;

class Peds {
public:
    Peds() {
        // Initialise your plugin here
        Events::initGameEvent += [&]
        {
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Failed to initialize input manager", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(
                std::bind(&Peds::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2)
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
                    int modelID = 15;
                    CStreaming::RequestModel(modelID, 0xC);
                    CStreaming::LoadAllRequestedModels(0);
                    CPed* pPed = new CCivilianPed(PED_TYPE_CIVMALE, modelID);
#else
                    int modelID = 4;
                    CStreaming::RequestModel(modelID, 0x16);
                    CStreaming::LoadAllRequestedModels(0);
                    CPed* pPed = new CCivilianPed(PEDTYPE_CIVMALE, modelID);
#endif
                    if (pPed)
                    {
                        CVector position = pLocalPlayer->GetPosition();
                        position.y += 2;

#ifdef GTAVC
                        pPed->m_placement.SetRotate(0.0f, 0.0f, 0.0f);
                        pPed->m_placement.pos = position;
                        pPed->m_nPedStatus = 2; //script ped
#elif GTASA
                        pPed->SetOrientation(0.0f, 0.0f, 0.0f);
                        pPed->SetPosn(position);
#else
                        pPed->SetOrientation(0.0f, 0.0f, 0.0f);
                        pPed->SetPosition(position.x, position.y, position.z);
                        pPed->m_nPedStatus = 2; //script ped
#endif
                        
                        CWorld::Add(pPed);
                    }
                }
            }
            }
        }
    }

private:
    InputManager theInputManager;
} peds;
