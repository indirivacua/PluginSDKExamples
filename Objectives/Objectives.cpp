#include "plugin.h"
#include "eObjective.h"
#include "CCivilianPed.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "eWeaponType.h"
#include "CWeaponInfo.h"
#include "InputManager.h"

using namespace plugin;

class Objectives {
public:
    Objectives() {
        // Initialise your plugin here
        Events::initGameEvent += [&]
        {
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Failed to initialize input manager", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(
                std::bind(&Objectives::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2)
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
                    CPed* pPed = CreatePed(4, 2.0f);
                    GiveWeapon(pPed, WEAPONTYPE_UZI);

                    pPed->m_nPedFlags.bHasObjectiveCompleted = false;
                    pPed->SetObjective(eObjective::OBJECTIVE_KILL_CHAR_ANY_MEANS, pLocalPlayer);
                }
                break;
            }
            case DIK_1:
            {

                CPed* pPed = CreatePed(4, 2.0f);
                GiveWeapon(pPed, WEAPONTYPE_UZI);

                pPed->m_nPedFlags.bHasObjectiveCompleted = false;
                pPed->SetObjective(eObjective::OBJECTIVE_STEAL_ANY_CAR);
                break;
            }
            case DIK_2:
            {
#ifdef GTAVC
                CVehicle * pVehicle = CreateCar(141, 5.0f);
#else
                CVehicle * pVehicle = CreateCar(105, 5.0f);
#endif
                CPed* pPed = CreatePed(4, 2.0f);
                GiveWeapon(pPed, WEAPONTYPE_UZI);

                pPed->m_nPedFlags.bHasObjectiveCompleted = false;
                pPed->SetObjective(eObjective::OBJECTIVE_DESTROY_CAR, pVehicle);
                break;
            }
            case DIK_3:
            {
#ifdef GTAVC
                CVehicle * pVehicle = CreateCar(141, 5.0f);
#else
                CVehicle * pVehicle = CreateCar(105, 5.0f);
#endif
                CPed* pPed = CreatePed(4, 2.0f);
                GiveWeapon(pPed, WEAPONTYPE_UZI);

                pPed->m_nPedFlags.bHasObjectiveCompleted = false;
                pPed->SetObjective(eObjective::OBJECTIVE_ENTER_CAR_AS_DRIVER, pVehicle);
                break;
            }
            case DIK_4:
            {
#ifdef GTAVC
                CVehicle * pVehicle = CreateCar(141, 5.0f);
#else
                CVehicle * pVehicle = CreateCar(105, 5.0f);
#endif
                CPed* pPed = CreatePed(4, 2.0f);
                GiveWeapon(pPed, WEAPONTYPE_UZI);

                pPed->m_nPedFlags.bHasObjectiveCompleted = false;
                pPed->SetObjective(eObjective::OBJECTIVE_ENTER_CAR_AS_PASSENGER, pVehicle);
                break;
            }
            }
        }
    }

    CVehicle* CreateCar(int vehicleID, float yOffset)
    {
        CVehicle* pVehicle = nullptr;
        CPlayerPed* pLocalPlayer = FindPlayerPed();
        if (pLocalPlayer)
        {
            CVector position = pLocalPlayer->GetPosition();
            position.y += yOffset;
#ifdef GTAVC
            CStreaming::RequestModel(vehicleID, 0x16);
            CStreaming::LoadAllRequestedModels(0);
            pVehicle = new CAutomobile(vehicleID, MISSION_VEHICLE);
            pVehicle->m_placement.SetRotate(0.0f, 0.0f, 0.0f);
            pVehicle->m_placement.pos = position;
            pVehicle->m_nLockStatus = 1; //unlocked
            pVehicle->m_nState = 4;
#else
            CStreaming::RequestModel(vehicleID, 0x16);
            CStreaming::LoadAllRequestedModels(0);
            pVehicle = new CAutomobile(vehicleID, MISSION_VEHICLE);
            pVehicle->SetOrientation(0.0f, 0.0f, 0.0f);
            pVehicle->SetPosition(position);
            pVehicle->m_nDoorLock = 1; // unlocked
            pVehicle->m_nState = 4;
#endif
            pVehicle->m_nType &= 7;
            CWorld::Add(pVehicle);
        }
        return pVehicle;
    }

    void GiveWeapon(CPed* pPed, eWeaponType weaponType)
    {
        CWeaponInfo* pWeaponInfo = CWeaponInfo::GetWeaponInfo(weaponType);
        int weaponID = pWeaponInfo->m_nModelId;
        CStreaming::RequestModel(weaponID, 0x16);
        CStreaming::LoadAllRequestedModels(0);

#ifdef GTA3
        pPed->GiveWeapon(weaponType, 1000);
#else
        pPed->GiveWeapon(weaponType, 1000, true);
#endif
        pPed->SetCurrentWeapon(weaponType);
        CStreaming::SetModelIsDeletable(weaponID);
    }

    CPed* CreatePed(int modelID, float yOffset)
    {
        CPed* pPed = nullptr;
        CPlayerPed* pLocalPlayer = FindPlayerPed();
        if (pLocalPlayer)
        {
            CStreaming::RequestModel(modelID, 0x16);
            CStreaming::LoadAllRequestedModels(0);
            pPed = new CCivilianPed(PEDTYPE_CIVMALE, modelID);
            if (pPed)
            {
                CVector position = pLocalPlayer->GetPosition();
                position.y += yOffset;

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
        return pPed;
    }
private:
    InputManager theInputManager;
} objectives;
