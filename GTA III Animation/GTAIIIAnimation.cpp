#include "plugin.h"
#include "InputManager.h"
#include "CAnimManager.h"
#include "CAnimBlendAssociation.h"
#include "eAnimations.h"

using namespace plugin;

class GTAIIIAnimation {
public:
    GTAIIIAnimation() {
        // Initialise your plugin here

        Events::initGameEvent += [&]
        {
            if (!theInputManager.Initialize())
            {
                MessageBox(0, "Failed to initialize input manager", "Error", MB_OK);
                return;
            }

            theInputManager.RegisterKeyBoardCallBack(
                std::bind(&GTAIIIAnimation::OnKeyPress, this, std::placeholders::_1, std::placeholders::_2)
            );
        };

        Events::gameProcessEvent += [&]
        {
            theInputManager.ProcessInput();
        };
    }

    void PlayAnimation(RpClump* pClump, int animGroup, int animID, float blend)
    {
        CAnimBlendAssociation* pAnimStaticAssoc = CAnimManager::GetAnimAssociation(animGroup, animID);
        CAnimBlendAssociation* pAnimAssoc = RpAnimBlendClumpGetFirstAssociation(pClump);
        while (pAnimAssoc)
        {
            if (pAnimAssoc->m_nAnimID == pAnimStaticAssoc->m_nAnimID && pAnimAssoc->m_pAnimBlendHierarchy == pAnimStaticAssoc->m_pAnimBlendHierarchy)
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
                    PlayAnimation(pLocalPlayer->m_pRwClump, ANIM_GROUP_MAN, ANIM_MAN_WEAPON_SNIPER, 1.0f);
                }
            }
            }
        }
    }

private:
    InputManager theInputManager;
} gTAIIIAnimation;
