#include "plugin.h"
#include "InputManager.h"
#include "CObject.h"
#include "CWorld.h"
#include "RenderWare.h"
#include "CTheScripts.h"
#include "CModelInfo.h"
#include "CTimer.h"
#include "CStreaming.h"
using namespace plugin;

class Objects {
public:
    Objects() {
        // Initialise your plugin here
        if (AllocConsole())
        {
            freopen("CONIN$", "r", stdin);
            freopen("CONOUT$", "w", stdout);
            freopen("CONOUT$", "w", stderr);
        }

        Events::initGameEvent += [&]
        {
            std::printf("Events::initGameEvent called\n");
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Direct Input Initialization Failed", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(std::bind(&Objects::OnKeyPress, this,
                std::placeholders::_1, std::placeholders::_2));
        };

        // Initialise your plugin here
        Events::gameProcessEvent += [&]
        {
            theInputManager.ProcessInput();
        };
    };

    /*
    void CEntity_UpdateRW_(CEntity * pEntity)
    {
    if (pEntity->m_pRwObject)
    {
    if (pEntity->m_matrix)
    {
    pEntity->m_matrix->UpdateRW(&((RwFrame*)pEntity->m_pRwObject->parent)->modelling);
    }
    else
    {
    RwMatrix theMatrix;
    pEntity->m_placement.UpdateRwMatrix(&theMatrix);
    }
    }
    }*/

    void OnKeyPress(unsigned char key, PressState state)
    {
        if (state == PRESS_DOWN)
        {
            switch (key)
            {
            case DIK_1:
            {
                CPlayerPed* pLocalPlayer = FindPlayerPed();
                if (pLocalPlayer)
                {
                    std::printf("ok craete now\n");
                    DWORD objectID = 591; // SA: 3110, VC: 591, III: 

                    CStreaming::RequestModel(objectID, 22);
                    CStreaming::LoadAllRequestedModels(0);

                    CVector position = pLocalPlayer->m_placement.pos; //->TransformFromObjectSpace(CVector(0.0f, 5.0f, 0.0f));
                    position.y += 5.0f;

                    CObject* pObject = new CObject(objectID, true);
                    pObject->m_nObjectType = OBJECT_MISSION;
                    if (position.z <= -100.0)
                    {
                        position.z = CWorld::FindGroundZForCoord(position.x, position.y);
                    }
                    position.z = pObject->GetDistanceFromCentreOfMassToBaseOfModel() + position.z;
                    pObject->m_placement.SetRotate(0.0, 0.0, 0.0);
                    pObject->m_placement.pos = position;
                    pObject->m_placement.UpdateRW();
                    pObject->UpdateRwFrame();

                    std::printf("ped:  %f, %f, %f\n", pLocalPlayer->m_placement.pos.x, pLocalPlayer->m_placement.pos.y, pLocalPlayer->m_placement.pos.z);
                    std::printf("created object:  %f, %f, %f\n", pObject->m_placement.pos.x, pObject->m_placement.pos.y, pObject->m_placement.pos.z);

                    CBaseModelInfo* pModelInfo = CModelInfo::ms_modelInfoPtrs[objectID];
                    unsigned char modelType = CModelInfo::ms_modelInfoPtrs[objectID]->m_nType;
                    if ((modelType == MODEL_INFO_SIMPLE || modelType == MODEL_INFO_TIME))
                    {
                        CSimpleModelInfo* pSimpleModelInfo = reinterpret_cast<CSimpleModelInfo*>(pModelInfo);
                        if (pSimpleModelInfo->m_nFlags.bIsBigBuilding)
                        {
                            pObject->SetupBigBuilding();
                        }
                    }

                    
                    CWorld::Add(pObject);
                }
                break;
            }
            }
        }
    }


private:
    InputManager      theInputManager;
} objects;
