#pragma once

#include "global/types.h"

int16_t Room_GetTiltType(FLOOR_INFO *floor, int32_t x, int32_t y, int32_t z);
int32_t Room_FindGridShift(int32_t src, int32_t dst);
void Room_GetNewRoom(int32_t x, int32_t y, int32_t z, int16_t room_num);
void Room_GetNearByRooms(
    int32_t x, int32_t y, int32_t z, int32_t r, int32_t h, int16_t room_num);