/*
 * FireRed/LeafGreen build (CTR_FRLG): what the units the FireRed build does
 * not have yet provide in the Emerald build, so the rest links and behaves as
 * the original game does.
 *
 *  - 3ds_bottom_ui.c, the bottom screen's interface: laid out over Emerald's
 *    own screens (PokéNav, its bag and Pokédex). Until FireRed's version
 *    exists, the bottom screen stays empty and the game keeps its START menu,
 *    battle menus and field menus on the top screen.
 *  - 3ds_rfu_adapter.c, the emulated Wireless Adapter: no adapter answers.
 *  - 3ds_encounter_tables.c, the timed wild encounters of Hoenn's routes:
 *    the original tables.
 *
 * docs/FIRERED_PORT.md tracks each of them.
 */
#include "global.h"
#include "main.h"
#include "wild_encounter.h"
#include "3ds_bottom.h"
#include "3ds_extras.h"
#include "3ds_encounters.h"

/* ---- bottom screen (3ds_bottom_ui.c) -------------------------------------- */

void CtrBottom_Init(void) {}
void CtrBottom_Frame(void) {}
void CtrBottom_SoftReset(void) {}
void CtrBottom_OnResume(void) {}
uint16_t CtrBottom_FilterKeys(uint16_t held) { return held; }
uint16_t CtrBottom_InjectedKeys(void) { return 0; }
void CtrBottom_KeepWorld(void) {}
bool8 CtrBottom_LeavePending(void) { return FALSE; }
MainCallback CtrBottom_LeaveTo(MainCallback field) { return field; }
bool8 CtrBottom_NativeMenus(void) { return FALSE; } /* classic menus */
void CtrBottom_OpenFlyMap(u8 slot, bool8 fromBag) { (void)slot; (void)fromBag; }
bool8 CtrPlayer_RunAlways(void) { return FALSE; }
void CtrExtras_ShowScreen(unsigned page, unsigned screen) { (void)page; (void)screen; }

/* The battle's touch menus: inactive, the game's own menus take the keys. */
bool8 CtrBattleMenu_Active(void) { return FALSE; }
void CtrBattleMenu_Begin(void) {}
void CtrBattleMenu_ShowPrompt(void) {}
void CtrBattleMenu_ShowAction(void) {}
void CtrBattleMenu_ShowMove(void) {}
void CtrBattleMenu_ActionInput(u8 *cursor, bool8 safari) { (void)cursor; (void)safari; }
void CtrBattleMenu_MoveInput(u8 *cursor, const u16 *moves) { (void)cursor; (void)moves; }
void CtrBattleMenu_TargetInput(void) {}
bool8 CtrBattleMenu_TakeQuickBall(void) { return FALSE; }

/* ---- wireless adapter (3ds_rfu_adapter.c) ---------------------------------- */

s32 PortRfu_CheckID(void) { return 0; }
void PortRfu_Command(void) {}
void PortRfu_Frame(void) {}
bool8 PlatformLocalLinkEnabled(void) { return FALSE; }
bool8 PlatformLocalLinkActive(void) { return FALSE; }
void PlatformLinkBattleReset(void) {}
void PlatformLinkBattleSend(const void *data, u16 size, bool8 toSelf) { (void)data; (void)size; (void)toSelf; }
u16 PlatformLinkBattleRecv(void *out, u16 max) { (void)out; (void)max; return 0; }
void PlatformLinkBlockSend(const void *data, u16 size) { (void)data; (void)size; }
u16 PlatformLinkBlockTake(u8 playerIndex, void *out, u16 max) { (void)playerIndex; (void)out; (void)max; return 0; }

/* ---- timed encounters (3ds_encounter_tables.c) ----------------------------- */

const struct WildPokemonInfo *CtrEncounters_Land(const struct WildPokemonInfo *original,
                                                 unsigned mapGroup, unsigned mapNum)
{
    (void)mapGroup;
    (void)mapNum;
    return original;
}
