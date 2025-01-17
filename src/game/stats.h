#pragma once

#include "global/types.h"

#include <stdbool.h>
#include <stdint.h>

void Stats_ObserveRoomsLoad(void);
void Stats_ObserveItemsLoad(void);
void Stats_CalculateStats(void);
int32_t Stats_GetPickups(void);
int32_t Stats_GetKillables(void);
int32_t Stats_GetSecrets(void);
void Stats_Show(int32_t level_num);
void Stats_ShowTotal(const char *filename, GAMEFLOW_LEVEL_TYPE level_type);
bool Stats_CheckAllSecretsCollected(GAMEFLOW_LEVEL_TYPE level_type);
