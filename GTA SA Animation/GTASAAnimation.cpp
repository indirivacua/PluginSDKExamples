#include "plugin.h"
#include "InputManager.h"
#include "CAnimManager.h"
#include "eAnimations.h"
#include "CStreaming.h"
#include "CTaskSimpleRunNamedAnim.h"

using namespace plugin;

class GTASAAnimation {
public:
    GTASAAnimation() {
        // Initialise your plugin heres
        Events::initGameEvent += [&]
        {
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Failed to initialize input manager", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(
                std::bind(&GTASAAnimation::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2)
            );
        };

        Events::gameProcessEvent += [&]
        {
            theInputManager.ProcessInput();
        };
    }

    bool LoadAnimationBlock(const char* szBlockName)
    {
        CAnimBlock* pAnimBlock = CAnimManager::GetAnimationBlock(szBlockName);
        if (pAnimBlock)
        {
            if (!pAnimBlock->bLoaded)
            {
                int animIndex = CAnimManager::GetAnimationBlockIndex(szBlockName);
                CStreaming::RequestModel(25575 + animIndex, MISSION_REQUIRED | PRIORITY_REQUEST);
                pAnimBlock->usRefs++;
                CStreaming::LoadAllRequestedModels(true);
                if (pAnimBlock->bLoaded)
                {
                    return true;
                }
            }
            else
            {
                return true;
            }
        }
        return false;
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
                    const char* szAnimName = "swim_tread";
                    const char* szBlockName = "swim";
                    if (LoadAnimationBlock(szBlockName))
                    {
                        unsigned short animFlags = ANIMATION_UNLOCK_LAST_FRAME | ANIMATION_PARTIAL | ANIMATION_TRANLSATE_X 
                                                   | ANIMATION_TRANLSATE_Y | ANIMATION_LOOPED;

                        //CTaskSimpleRunNamedAnim(const char* pAnimName, const char* pAnimGroupName, int flags, float fBlendDelta,
                        //    int nTime, bool bDontInterrupt, bool bRunInSequence, bool bOffsetPed, bool bHoldLastFrame);

                        CTaskSimpleRunNamedAnim* pTask = new CTaskSimpleRunNamedAnim(
                            szAnimName, szBlockName, animFlags, 1.0f, -1, false, true, false, false );

                        pLocalPlayer->m_pIntelligence->m_TaskMgr.SetTask(pTask, TASK_PRIMARY_PRIMARY, true);
                    }
                }
            }
            }
        }
    }

private:
    InputManager theInputManager;
} gTASAAnimation;
