#include "game/objects/traps/midas_touch.h"

#include "game/input.h"
#include "game/inventory.h"
#include "game/items.h"
#include "game/lara.h"
#include "game/objects/common.h"
#include "game/overlay.h"
#include "global/const.h"
#include "global/vars.h"

#define EXTRA_ANIM_PLACE_BAR 0
#define EXTRA_ANIM_DIE_GOLD 1
#define LF_PICKUP_GOLD_BAR 113

static int16_t m_MidasBounds[12] = {
    -700,
    +700,
    +384 - 100,
    +384 + 100 + 512,
    -700,
    +700,
    -10 * PHD_DEGREE,
    +10 * PHD_DEGREE,
    -30 * PHD_DEGREE,
    +30 * PHD_DEGREE,
    -10 * PHD_DEGREE,
    +10 * PHD_DEGREE,
};

void MidasTouch_Setup(OBJECT_INFO *obj)
{
    obj->collision = MidasTouch_Collision;
    obj->draw_routine = Object_DrawDummyItem;
}

void MidasTouch_Collision(
    int16_t item_num, ITEM_INFO *lara_item, COLL_INFO *coll)
{
    ITEM_INFO *item = &g_Items[item_num];

    if (lara_item->current_anim_state == LS_USE_MIDAS) {
        if (Item_TestFrameEqual(lara_item, LF_PICKUP_GOLD_BAR)) {
            Overlay_AddPickup(O_PUZZLE_ITEM1);
            Inv_AddItem(O_PUZZLE_ITEM1);
        }
    }

    if (!lara_item->gravity_status && lara_item->current_anim_state == LS_STOP
        && lara_item->pos.x > item->pos.x - 512
        && lara_item->pos.x < item->pos.x + 512
        && lara_item->pos.z > item->pos.z - 512
        && lara_item->pos.z < item->pos.z + 512) {
        lara_item->current_anim_state = LS_DIE_MIDAS;
        lara_item->goal_anim_state = LS_DIE_MIDAS;
        Item_SwitchToObjAnim(lara_item, EXTRA_ANIM_DIE_GOLD, 0, O_LARA_EXTRA);
        lara_item->hit_points = -1;
        lara_item->gravity_status = 0;
        g_Lara.air = -1;
        g_Lara.gun_status = LGS_HANDS_BUSY;
        g_Lara.gun_type = LGT_UNARMED;
        g_Camera.type = CAM_CINEMATIC;
        g_CineFrame = 0;
        g_CinePosition = lara_item->pos;
        return;
    }

    if ((g_InvChosen == -1 && !g_Input.action)
        || g_Lara.gun_status != LGS_ARMLESS || lara_item->gravity_status
        || lara_item->current_anim_state != LS_STOP) {
        return;
    }

    uint16_t quadrant = (uint16_t)(lara_item->pos.y_rot + PHD_45) / PHD_90;
    switch (quadrant) {
    case DIR_NORTH:
        item->pos.y_rot = 0;
        break;
    case DIR_EAST:
        item->pos.y_rot = PHD_90;
        break;
    case DIR_SOUTH:
        item->pos.y_rot = -PHD_180;
        break;
    case DIR_WEST:
        item->pos.y_rot = -PHD_90;
        break;
    }

    if (!Lara_TestPosition(item, m_MidasBounds)) {
        return;
    }

    if (g_InvChosen == -1) {
        Inv_Display(INV_KEYS_MODE);
    }

    if (g_InvChosen == O_LEADBAR_OPTION) {
        Inv_RemoveItem(O_LEADBAR_OPTION);
        lara_item->current_anim_state = LS_USE_MIDAS;
        lara_item->goal_anim_state = LS_USE_MIDAS;
        Item_SwitchToObjAnim(lara_item, EXTRA_ANIM_PLACE_BAR, 0, O_LARA_EXTRA);
        g_Lara.gun_status = LGS_HANDS_BUSY;
    }
}
