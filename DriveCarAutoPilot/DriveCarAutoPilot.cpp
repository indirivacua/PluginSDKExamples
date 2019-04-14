
#include "plugin.h"
#include "CStreaming.h"
#include "CWorld.h"
#include "CAutomobile.h"
#include "CCarCtrl.h"
#include "eEntityStatus.h"
#include "InputManager.h"
#include "CTimer.h"

using namespace plugin;

class DriveCarAutoPilot {
public:
    DriveCarAutoPilot() {
        // Initialise your plugin here
        Events::initGameEvent += [&]
        {
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Failed to initialize input manager", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(
                std::bind(&DriveCarAutoPilot::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2)
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
                    CVehicle* pVehicle = CreateCar(411, 5.0f);
#elif GTAVC 
                    CVehicle* pVehicle = CreateCar(162, 5.0f); // tank
#else
                    CVehicle* pVehicle = CreateCar(105, 5.0f);
#endif
                    CVector position = pVehicle->GetPosition();
                    position.y += 400.0f;
                    position.z += 100.0f;

                    DriveCar(pVehicle, position);
                }
            }
            }
        }
    }

    void DriveCar(CVehicle* pVehicle, CVector position)
    {
        position.z += pVehicle->GetDistanceFromCentreOfMassToBaseOfModel();
        eCarMission carMission = pVehicle->m_autoPilot.m_nCarMission;

        bool engineOn = true;
        bool minimumCruiseSpeed = 1;
#ifdef GTA3
        minimumCruiseSpeed = 6;
#endif

#ifdef GTASA
        bool bSuccess = CCarCtrl::JoinCarWithRoadSystemGotoCoors(pVehicle, position, false, false);
        if (!bSuccess)
        {
            if (carMission != MISSION_39 && carMission != 58)
            {
                carMission = MISSION_GOTOCOORDS_ACCURATE;
            }
        }
        else
        {
            if (carMission != MISSION_39 && carMission != 58)
            {
                carMission = MISSION_GOTOCOORDS_STRAIGHT_ACCURATE;
            }
        }
        pVehicle->m_autoPilot.m_nCarMission = carMission;
        pVehicle->m_nStatus = STATUS_PHYSICS;
        engineOn = pVehicle->m_nVehicleFlags.bEngineBroken ? false : true;
#else
        bool bSuccess = CCarCtrl::JoinCarWithRoadSystemGotoCoors(pVehicle, position, false);
        pVehicle->m_autoPilot.m_nCarMission = bSuccess ? MISSION_GOTOCOORDS_STRAIGHT : MISSION_GOTOCOORDS;
        pVehicle->m_nState = STATUS_PHYSICS;
#endif
        pVehicle->m_nVehicleFlags.bEngineOn = engineOn;
        pVehicle->m_nType &= 7;

        if (pVehicle->m_autoPilot.m_nCruiseSpeed < minimumCruiseSpeed)
        {
            pVehicle->m_autoPilot.m_nCruiseSpeed = minimumCruiseSpeed;
        }

        pVehicle->m_autoPilot.m_nTimeToStartMission = CTimer::m_snTimeInMilliseconds;
    }

    CVehicle* CreateCar(int vehicleID, float yOffset)
    {
        CVehicle* pVehicle = nullptr;
        CPlayerPed* pLocalPlayer = FindPlayerPed();
        if (pLocalPlayer)
        {
            CVector position = pLocalPlayer->GetPosition();
            position.y += yOffset;
#ifdef GTASA
            CStreaming::RequestModel(vehicleID, 0x16);
            CStreaming::LoadAllRequestedModels(0);
            pVehicle = new CAutomobile(vehicleID, MISSION_VEHICLE, true);
            pVehicle->SetOrientation(0.0f, 0.0f, 0.0f);
            pVehicle->SetPosn(position);
            pVehicle->m_nDoorLock = 1; // unlocked
            pVehicle->m_nStatus = 4;

#elif GTAVC
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
private:
    InputManager theInputManager;
} driveCarAutoPilot;
