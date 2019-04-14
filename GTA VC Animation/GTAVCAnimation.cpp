#include "plugin.h"
#include "InputManager.h"
#include "CAnimManager.h"
#include "eAnimations.h"
#include "detours.h"
#include "CStreaming.h"

using namespace plugin;

typedef bool(__cdecl* hCStreaming_RemoveModel) (int);
static auto OLD_CStreaming_RemoveModel = (hCStreaming_RemoveModel)0x40D6E0;

class GTAVCAnimation {
public:
    GTAVCAnimation() {
        // Initialise your plugin here
        Events::initGameEvent += [&]
        {
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Failed to initialize input manager", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(
                std::bind(&GTAVCAnimation::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2)
            );
        };

        Events::gameProcessEvent += [&]
        {
            theInputManager.ProcessInput();
        };

        PatchFunction();
        HookFunctions();
    }

    void PatchFunction()
    {
        // mov al, 01
        // ret
        // nop (2x)
        BYTE AreAnimsUsedByRequestedModels_Patch[] = {0xB0, 0x01, 0xC3, 0x90, 0x90};
        MemCpy((void*)0x40C9C0, AreAnimsUsedByRequestedModels_Patch, sizeof(AreAnimsUsedByRequestedModels_Patch));

        // ret
        // nop (3x)
        BYTE CAnimManager_RemoveAnimBlockRef_Patch[] = {0xC3, 0x90, 0x90, 0x90};
        MemCpy((void*)0x404950, CAnimManager_RemoveAnimBlockRef_Patch, sizeof(CAnimManager_RemoveAnimBlockRef_Patch));
    }

    void HookFunctions()
    {
        DetourRestoreAfterWith();
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());

        DetourAttach(&(PVOID&)OLD_CStreaming_RemoveModel, NEW_CStreaming_RemoveModel);

        DetourTransactionCommit();
    }

    static bool NEW_CStreaming_RemoveModel(int modelID)
    {
        // Check if it's IFP animation block
        if (modelID >= 7916 && modelID <= 7950)
        {
            // Do not unload the animation block
            return true;
        }
        return OLD_CStreaming_RemoveModel(modelID);
    }

    void PlayAnimation(RpClump* pClump, int animGroup, int animID, float blend)
    {
        if (animGroup < CAnimManager::ms_numAnimAssocDefinitions)
        {
            CAnimationStyleDescriptor* pAnimDef = &CAnimManager::ms_aAnimAssocDefinitions[animGroup];
            if (pAnimDef)
            {
                if (!LoadAnimationBlock(pAnimDef->blockName))
                {
                    return;
                }
            }
        }

        CAnimBlendAssociation* pAnimAssoc = RpAnimBlendClumpGetFirstAssociation(pClump);
        while (pAnimAssoc)
        {
            if (pAnimAssoc->m_nAnimId == animID && pAnimAssoc->m_nAnimGroup == animGroup)
            {
                // Destroy the animation
                pAnimAssoc->~CAnimBlendAssociation();
                break;
            }
            pAnimAssoc = RpAnimBlendGetNextAssociation(pAnimAssoc);
        }
        pAnimAssoc = CAnimManager::BlendAnimation(pClump, animGroup, animID, blend);
        pAnimAssoc->m_nFlags = ANIMATION_STARTED | ANIMATION_MOVEMENT | ANIMATION_LOOPED;
    }

    bool LoadAnimationBlock(const char* szBlockName)
    {
        CAnimBlock* pAnimBlock = CAnimManager::GetAnimationBlock(szBlockName);
        if (pAnimBlock)
        {
            if (!pAnimBlock->bLoaded)
            {
                int animIndex = ((unsigned char*)pAnimBlock - (unsigned char*)CAnimManager::ms_aAnimBlocks) / 32;
                CStreaming::RequestModel(7916 + animIndex, 0x20 | MISSION_REQUEST | PRIORITY_REQUEST);
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
                if (pLocalPlayer && pLocalPlayer->m_pRwClump)
                {
                    PlayAnimation(pLocalPlayer->m_pRwClump, ANIM_GROUP_PLAYIDLES, ANIM_PLAYIDLES_STRETCH, 1.0f);
                }
            }
            }
        }
    }

private:
    InputManager theInputManager;
} gTAVCAnimation;
