#include "game/stats.h"

#include "config.h"
#include "game/carrier.h"
#include "game/clock.h"
#include "game/game.h"
#include "game/gamebuf.h"
#include "game/gameflow.h"
#include "game/input.h"
#include "game/items.h"
#include "game/music.h"
#include "game/output.h"
#include "game/text.h"
#include "global/const.h"
#include "global/types.h"
#include "global/vars.h"
#include "log.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct TOTAL_STATS {
    uint32_t timer;
    uint32_t death_count;
    uint32_t player_kill_count;
    uint16_t player_secret_count;
    uint16_t player_pickup_count;
    uint32_t total_kill_count;
    uint16_t total_secret_count;
    uint16_t total_pickup_count;
} TOTAL_STATS;

static TOTAL_STATS m_TotalStats;
static int32_t m_CachedItemCount = 0;
static FLOOR_INFO **m_CachedFloorArray = NULL;
static int32_t m_LevelPickups = 0;
static int32_t m_LevelKillables = 0;
static int32_t m_LevelSecrets = 0;
static uint32_t m_SecretRoom = 0;
static bool m_KillableItems[MAX_ITEMS] = { 0 };
static bool m_IfKillable[O_NUMBER_OF] = { 0 };
static void Stats_ComputeTotal(GAMEFLOW_LEVEL_TYPE level_type);

int16_t m_PickupObjs[] = { O_PICKUP_ITEM1,   O_PICKUP_ITEM2,  O_KEY_ITEM1,
                           O_KEY_ITEM2,      O_KEY_ITEM3,     O_KEY_ITEM4,
                           O_PUZZLE_ITEM1,   O_PUZZLE_ITEM2,  O_PUZZLE_ITEM3,
                           O_PUZZLE_ITEM4,   O_GUN_ITEM,      O_SHOTGUN_ITEM,
                           O_MAGNUM_ITEM,    O_UZI_ITEM,      O_GUN_AMMO_ITEM,
                           O_SG_AMMO_ITEM,   O_MAG_AMMO_ITEM, O_UZI_AMMO_ITEM,
                           O_EXPLOSIVE_ITEM, O_MEDI_ITEM,     O_BIGMEDI_ITEM,
                           O_SCION_ITEM,     O_SCION_ITEM2,   O_LEADBAR_ITEM,
                           NO_ITEM };

// Pierre and pods have special trigger check
int16_t m_KillableObjs[] = {
    O_WOLF,     O_BEAR,        O_BAT,      O_CROCODILE, O_ALLIGATOR,
    O_LION,     O_LIONESS,     O_PUMA,     O_APE,       O_RAT,
    O_VOLE,     O_TREX,        O_RAPTOR,   O_WARRIOR1,  O_WARRIOR2,
    O_WARRIOR3, O_CENTAUR,     O_MUMMY,    O_TORSO,     O_DINO_WARRIOR,
    O_FISH,     O_LARSON,      O_SKATEKID, O_COWBOY,    O_BALDY,
    O_NATLA,    O_SCION_ITEM3, O_STATUE,   NO_ITEM
};

static void Stats_TraverseFloor(void);
static void Stats_CheckTriggers(
    ROOM_INFO *r, int room_num, int x_floor, int y_floor);
static bool Stats_IsObjectKillable(int32_t obj_num);
static void Stats_IncludeKillableItem(int16_t item_num);

static void Stats_TraverseFloor(void)
{
    uint32_t secrets = 0;

    for (int i = 0; i < g_RoomCount; i++) {
        ROOM_INFO *r = &g_RoomInfo[i];
        for (int x_floor = 0; x_floor < r->x_size; x_floor++) {
            for (int y_floor = 0; y_floor < r->y_size; y_floor++) {
                Stats_CheckTriggers(r, i, x_floor, y_floor);
            }
        }
    }
}

static void Stats_CheckTriggers(
    ROOM_INFO *r, int room_num, int x_floor, int y_floor)
{
    if (x_floor == 0 || x_floor == r->x_size - 1) {
        if (y_floor == 0 || y_floor == r->y_size - 1) {
            return;
        }
    }

    FLOOR_INFO *floor =
        &m_CachedFloorArray[room_num][x_floor + y_floor * r->x_size];

    if (!floor->index) {
        return;
    }

    int16_t *data = &g_FloorData[floor->index];
    int16_t type;
    int16_t trigger;
    int16_t trig_flags;
    int16_t trig_type;
    do {
        type = *data++;

        switch (type & DATA_TYPE) {
        case FT_TILT:
        case FT_ROOF:
        case FT_DOOR:
            data++;
            break;

        case FT_LAVA:
            break;

        case FT_TRIGGER:
            trig_flags = *data;
            data++;
            trig_type = (type >> 8) & 0x3F;
            do {
                trigger = *data++;
                if (TRIG_BITS(trigger) == TO_SECRET) {
                    int16_t number = trigger & VALUE_BITS;
                    if (!(m_SecretRoom & (1 << number))) {
                        m_SecretRoom |= (1 << number);
                        m_LevelSecrets++;
                    }
                }
                if (TRIG_BITS(trigger) != TO_OBJECT) {
                    if (TRIG_BITS(trigger) == TO_CAMERA) {
                        trigger = *data++;
                    }
                } else {
                    int16_t idx = trigger & VALUE_BITS;

                    if (m_KillableItems[idx]) {
                        continue;
                    }

                    ITEM_INFO *item = &g_Items[idx];

                    // Add Pierre pickup and kills if oneshot
                    if (item->object_number == O_PIERRE
                        && trig_flags & IF_ONESHOT) {
                        Stats_IncludeKillableItem(idx);
                    }

                    // Check for only valid pods
                    if ((item->object_number == O_PODS
                         || item->object_number == O_BIG_POD)
                        && item->data != NULL) {
                        int16_t bug_item_num = *(int16_t *)item->data;
                        const ITEM_INFO *bug_item = &g_Items[bug_item_num];
                        if (g_Objects[bug_item->object_number].loaded) {
                            Stats_IncludeKillableItem(idx);
                        }
                    }

                    // Add killable if object triggered
                    if (Stats_IsObjectKillable(item->object_number)) {
                        Stats_IncludeKillableItem(idx);
                    }
                }
            } while (!(trigger & END_BIT));
            break;
        }
    } while (!(type & END_BIT));
}

static bool Stats_IsObjectKillable(int32_t obj_num)
{
    for (int i = 0; m_KillableObjs[i] != NO_ITEM; i++) {
        if (m_KillableObjs[i] == obj_num) {
            return true;
        }
    }
    return false;
}

static void Stats_IncludeKillableItem(int16_t item_num)
{
    m_KillableItems[item_num] = true;
    m_LevelKillables += 1;
    m_LevelPickups += Carrier_GetItemCount(item_num);
}

static void Stats_ComputeTotal(GAMEFLOW_LEVEL_TYPE level_type)
{
    memset(&m_TotalStats, 0, sizeof(TOTAL_STATS));

    int16_t secret_flags = 0;

    for (int i = 0; i < g_GameFlow.level_count; i++) {
        if (g_GameFlow.levels[i].level_type != level_type) {
            continue;
        }
        const GAME_STATS *stats = &g_GameInfo.current[i].stats;

        m_TotalStats.player_kill_count += stats->kill_count;
        m_TotalStats.player_pickup_count += stats->pickup_count;
        secret_flags = stats->secret_flags;
        for (int j = 0; j < MAX_SECRETS; j++) {
            if (secret_flags & 1) {
                m_TotalStats.player_secret_count++;
            }
            secret_flags >>= 1;
        }

        m_TotalStats.timer += stats->timer;
        m_TotalStats.death_count += stats->death_count;
        m_TotalStats.total_kill_count += stats->max_kill_count;
        m_TotalStats.total_secret_count += stats->max_secret_count;
        m_TotalStats.total_pickup_count += stats->max_pickup_count;
    }
}

void Stats_ObserveRoomsLoad(void)
{
    m_CachedFloorArray =
        GameBuf_Alloc(g_RoomCount * sizeof(FLOOR_INFO *), GBUF_ROOM_FLOOR);
    for (int i = 0; i < g_RoomCount; i++) {
        const ROOM_INFO *current_room_info = &g_RoomInfo[i];
        int count = current_room_info->y_size * current_room_info->x_size;
        m_CachedFloorArray[i] =
            GameBuf_Alloc(count * sizeof(FLOOR_INFO), GBUF_ROOM_FLOOR);
        memcpy(
            m_CachedFloorArray[i], current_room_info->floor,
            count * sizeof(FLOOR_INFO));
    }
}

void Stats_ObserveItemsLoad(void)
{
    m_CachedItemCount = g_LevelItemCount;
}

void Stats_CalculateStats(void)
{
    m_LevelPickups = 0;
    m_LevelKillables = 0;
    m_LevelSecrets = 0;
    m_SecretRoom = 0;
    memset(&m_KillableItems, 0, sizeof(m_KillableItems));

    if (m_CachedItemCount) {
        if (m_CachedItemCount > MAX_ITEMS) {
            LOG_ERROR("Too Many g_Items being Loaded!!");
            return;
        }

        for (int i = 0; i < m_CachedItemCount; i++) {
            ITEM_INFO *item = &g_Items[i];

            if (item->object_number < 0 || item->object_number >= O_NUMBER_OF) {
                LOG_ERROR(
                    "Bad Object number (%d) on Item %d", item->object_number,
                    i);
                continue;
            }

            for (int j = 0; m_PickupObjs[j] != NO_ITEM; j++) {
                if (item->object_number == m_PickupObjs[j]) {
                    m_LevelPickups++;
                }
            }
        }
    }

    // Check triggers for special pickups / killables
    Stats_TraverseFloor();

    m_LevelPickups -= g_GameFlow.levels[g_CurrentLevel].unobtainable.pickups;
    m_LevelKillables -= g_GameFlow.levels[g_CurrentLevel].unobtainable.kills;
}

int32_t Stats_GetPickups(void)
{
    return m_LevelPickups;
}

int32_t Stats_GetKillables(void)
{
    return m_LevelKillables;
}

int32_t Stats_GetSecrets(void)
{
    return m_LevelSecrets;
}

void Stats_Show(int32_t level_num)
{
    if (g_CurrentLevel == g_GameFlow.gym_level_num) {
        Output_FadeToBlack(false);
        while (Output_FadeIsAnimating()) {
            Game_DrawScene(false);
            Output_DumpScreen();
        }
        Output_FadeReset();
        return;
    }

    g_GameInfo.status |= GMS_IN_STATS;

    char buf[100];
    char time_str[100];
    TEXTSTRING *txt;

    const GAME_STATS *stats = &g_GameInfo.current[level_num].stats;

    Text_RemoveAll();

    int y = -50;
    const int row_height = 30;

    // heading
    sprintf(buf, "%s", g_GameFlow.levels[level_num].level_title);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    // kills
    sprintf(
        buf,
        g_GameFlow.strings
            [g_Config.enable_detailed_stats ? GS_STATS_KILLS_DETAIL_FMT
                                            : GS_STATS_KILLS_BASIC_FMT],
        stats->kill_count, stats->max_kill_count);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    // pickups
    sprintf(
        buf,
        g_GameFlow.strings
            [g_Config.enable_detailed_stats ? GS_STATS_PICKUPS_DETAIL_FMT
                                            : GS_STATS_PICKUPS_BASIC_FMT],
        stats->pickup_count, stats->max_pickup_count);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    // secrets
    int secret_count = 0;
    int16_t secret_flags = stats->secret_flags;
    for (int i = 0; i < MAX_SECRETS; i++) {
        if (secret_flags & 1) {
            secret_count++;
        }
        secret_flags >>= 1;
    }
    sprintf(
        buf, g_GameFlow.strings[GS_STATS_SECRETS_FMT], secret_count,
        stats->max_secret_count);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    // deaths
    if (g_Config.enable_deaths_counter && g_GameInfo.death_counter_supported) {
        sprintf(
            buf, g_GameFlow.strings[GS_STATS_DEATHS_FMT], stats->death_count);
        txt = Text_Create(0, y, buf);
        Text_CentreH(txt, 1);
        Text_CentreV(txt, 1);
        y += row_height;
    }

    // time taken
    int seconds = stats->timer / 30;
    int hours = seconds / 3600;
    int minutes = (seconds / 60) % 60;
    seconds %= 60;
    if (hours) {
        sprintf(
            time_str, "%d:%d%d:%d%d", hours, minutes / 10, minutes % 10,
            seconds / 10, seconds % 10);
    } else {
        sprintf(time_str, "%d:%d%d", minutes, seconds / 10, seconds % 10);
    }
    sprintf(buf, g_GameFlow.strings[GS_STATS_TIME_TAKEN_FMT], time_str);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    Output_FadeToSemiBlack(true);
    // wait till a skip key is pressed
    do {
        Game_DrawScene(false);
        Input_Update();
        Text_Draw();
        Output_DumpScreen();
    } while (!g_InputDB.menu_confirm && !g_InputDB.menu_back);

    Output_FadeToBlack(false);
    Text_RemoveAll();

    // finish fading
    while (Output_FadeIsAnimating()) {
        Game_DrawScene(false);
        Output_DumpScreen();
    }

    Output_FadeReset();
    g_GameInfo.status &= ~GMS_IN_STATS;
}

void Stats_ShowTotal(const char *filename, GAMEFLOW_LEVEL_TYPE level_type)
{
    Text_RemoveAll();

    Stats_ComputeTotal(level_type);

    char buf[100];
    char time_str[100];
    TEXTSTRING *txt;

    int top_y = 55;
    int y = 55;
    const int row_width = 220;
    const int row_height = 20;
    int16_t border = 4;

    // reserve space for heading
    y += row_height + border * 2;

    // kills
    sprintf(
        buf,
        g_GameFlow.strings
            [g_Config.enable_detailed_stats ? GS_STATS_KILLS_DETAIL_FMT
                                            : GS_STATS_KILLS_BASIC_FMT],
        m_TotalStats.player_kill_count, m_TotalStats.total_kill_count);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    // pickups
    sprintf(
        buf,
        g_GameFlow.strings
            [g_Config.enable_detailed_stats ? GS_STATS_PICKUPS_DETAIL_FMT
                                            : GS_STATS_PICKUPS_BASIC_FMT],
        m_TotalStats.player_pickup_count, m_TotalStats.total_pickup_count);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    // secrets
    sprintf(
        buf, g_GameFlow.strings[GS_STATS_SECRETS_FMT],
        m_TotalStats.player_secret_count, m_TotalStats.total_secret_count);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    // deaths
    if (g_Config.enable_deaths_counter && g_GameInfo.death_counter_supported) {
        sprintf(
            buf, g_GameFlow.strings[GS_STATS_DEATHS_FMT],
            m_TotalStats.death_count);
        txt = Text_Create(0, y, buf);
        Text_CentreH(txt, 1);
        Text_CentreV(txt, 1);
        y += row_height;
    }

    // time taken
    int seconds = m_TotalStats.timer / 30;
    int hours = seconds / 3600;
    int minutes = (seconds / 60) % 60;
    seconds %= 60;
    if (hours) {
        sprintf(
            time_str, "%d:%d%d:%d%d", hours, minutes / 10, minutes % 10,
            seconds / 10, seconds % 10);
    } else {
        sprintf(time_str, "%d:%d%d", minutes, seconds / 10, seconds % 10);
    }
    sprintf(buf, g_GameFlow.strings[GS_STATS_TIME_TAKEN_FMT], time_str);
    txt = Text_Create(0, y, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    y += row_height;

    // border
    int16_t height = y + border * 2 - top_y;
    txt = Text_Create(0, top_y, " ");
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    Text_AddBackground(txt, row_width, height, 0, 0, TS_BACKGROUND);
    Text_AddOutline(txt, true, TS_BACKGROUND);

    // heading
    sprintf(
        buf, "%s",
        g_GameFlow.strings
            [level_type == GFL_BONUS ? GS_STATS_BONUS_STATISTICS
                                     : GS_STATS_FINAL_STATISTICS]);
    txt = Text_Create(0, top_y + 2, buf);
    Text_CentreH(txt, 1);
    Text_CentreV(txt, 1);
    Text_AddBackground(txt, row_width - 4, 0, 0, 0, TS_HEADING);
    Text_AddOutline(txt, true, TS_HEADING);

    Output_LoadBackdropImage(filename);
    Clock_SyncTicks(1);

    Output_FadeReset();
    Output_FadeResetToBlack();
    Output_FadeToTransparent(true);
    while (Output_FadeIsAnimating()) {
        Output_DrawBackdropImage();
        Output_DumpScreen();
    }

    // wait till a skip key is pressed
    do {
        Output_DrawBackdropImage();
        Input_Update();
        Text_Draw();
        Output_DumpScreen();
    } while (!g_InputDB.menu_confirm && !g_InputDB.menu_back);

    // fade out
    Output_FadeToBlack(true);
    while (Output_FadeIsAnimating()) {
        Output_DrawBackdropImage();
        Output_DumpScreen();
    }

    Output_FadeReset();
    Text_RemoveAll();
    Music_Stop();
}

bool Stats_CheckAllSecretsCollected(GAMEFLOW_LEVEL_TYPE level_type)
{
    Stats_ComputeTotal(level_type);
    return m_TotalStats.player_secret_count >= m_TotalStats.total_secret_count;
}
