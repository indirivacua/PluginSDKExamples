/*
    IMPORTANT NOTE: Remember to place markhor.png in models folder of your GTA directory. 
*/
#include "plugin.h"
#include "common.h"
#include "CSprite2d.h"
#include "CTxdStore.h"

using namespace plugin;

class Sprites {
public:
    Sprites() {
        // Initialise your plugin here
        Events::initRwEvent += [&]
        {
            int txd = CTxdStore::AddTxdSlot("kings");
            CTxdStore::LoadTxd(txd, "models\\kings.txd");
            CTxdStore::AddRef(txd);
            CTxdStore::PushCurrentTxd();
            CTxdStore::SetCurrentTxd(txd);

            mySprite.SetTexture("markhor", "markhor_alpha");

            CTxdStore::PopCurrentTxd();
        };

        Events::shutdownRwEvent += [&]
        {
            mySprite.Delete();
            CTxdStore::RemoveTxdSlot(CTxdStore::FindTxdSlot("kings"));
        };

        Events::drawingEvent += [&]
        {
#ifdef GTAVC
            mySprite.Draw(CRect(20.0f, 20.0f, 256.0f, 256.0f), CRGBA(255, 255, 255, 255));
#else
            mySprite.Draw(20.0f, 20.0f, 256.0f, 256.0f, CRGBA(255, 255, 255, 255));
#endif
        };
    }
    
    CSprite2d mySprite;
} sprites;
