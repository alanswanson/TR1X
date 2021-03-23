#include "specific/input.h"

#include "config.h"
#include "game/inv.h"
#include "game/lara.h"
#include "global/vars.h"
#include "specific/shed.h"
#include "util.h"

#include <dinput.h>

int32_t OldInputDB = 0;

int16_t Layout[2][KEY_NUMBER_OF] = {
    // built-in controls
    {
        DIK_UP, // KEY_UP
        DIK_DOWN, // KEY_DOWN
        DIK_LEFT, // KEY_LEFT
        DIK_RIGHT, // KEY_RIGHT
        DIK_DELETE, // KEY_STEP_L
        DIK_NEXT, // KEY_STEP_R
        DIK_RSHIFT, // KEY_SLOW
        DIK_RMENU, // KEY_JUMP
        DIK_RCONTROL, // KEY_ACTION
        DIK_SPACE, // KEY_DRAW
        DIK_NUMPAD0, // KEY_LOOK
        DIK_END, // KEY_ROLL
        DIK_ESCAPE, // KEY_OPTION
        DIK_O, // KEY_FLY_CHEAT,
        DIK_I, // KEY_ITEM_CHEAT,
        DIK_X, // KEY_LEVEL_SKIP_CHEAT,
        DIK_P, // KEY_PAUSE,
    },

    // default user controls
    {
        DIK_NUMPAD8, // KEY_UP
        DIK_NUMPAD2, // KEY_DOWN
        DIK_NUMPAD4, // KEY_LEFT
        DIK_NUMPAD6, // KEY_RIGHT
        DIK_NUMPAD7, // KEY_STEP_L
        DIK_NUMPAD9, // KEY_STEP_R
        DIK_NUMPAD1, // KEY_SLOW
        DIK_ADD, // KEY_JUMP
        DIK_NUMPADENTER, // KEY_ACTION
        DIK_NUMPAD3, // KEY_DRAW
        DIK_NUMPAD0, // KEY_LOOK
        DIK_NUMPAD5, // KEY_ROLL
        DIK_DECIMAL, // KEY_OPTION
        DIK_O, // KEY_FLY_CHEAT,
        DIK_I, // KEY_ITEM_CHEAT,
        DIK_X, // KEY_LEVEL_SKIP_CHEAT,
        DIK_P, // KEY_PAUSE,
    }
};

int32_t Conflict[KEY_NUMBER_OF] = { 0 };

static int32_t MedipackCoolDown = 0;

int32_t Key_(int32_t number)
{
    int16_t key = Layout[1][number];

    if (key >= 256) {
        if (key == 256) {
            if (JoyHat) {
                return 1;
            }
        } else if (key == 272) {
            if (JoyThrottle) {
                return 1;
            }
        } else if ((1 << (key & 0xff)) & JoyFire) {
            return 1;
        }
    } else {
        if (KeyData->keymap[key]) {
            return 1;
        }
        switch (key) {
        case DIK_RCONTROL:
            return KeyData->keymap[DIK_LCONTROL];
        case DIK_LCONTROL:
            return KeyData->oldkeymap[DIK_LCONTROL];
        case DIK_RSHIFT:
            return KeyData->keymap[DIK_LSHIFT];
        case DIK_LSHIFT:
            return KeyData->keymap[DIK_RSHIFT];
        case DIK_RMENU:
            return KeyData->keymap[DIK_LMENU];
        case DIK_LMENU:
            return KeyData->oldkeymap[DIK_LMENU];
        }
    }

    if (Conflict[number]) {
        return 0;
    }

    key = Layout[0][number];
    if (KeyData->keymap[key]) {
        return 1;
    }
    switch (key) {
    case DIK_RCONTROL:
        return KeyData->keymap[DIK_LCONTROL];
    case DIK_LCONTROL:
        return KeyData->oldkeymap[DIK_LCONTROL];
    case DIK_RSHIFT:
        return KeyData->keymap[DIK_LSHIFT];
    case DIK_LSHIFT:
        return KeyData->keymap[DIK_RSHIFT];
    case DIK_RMENU:
        return KeyData->keymap[DIK_LMENU];
    case DIK_LMENU:
        return KeyData->oldkeymap[DIK_LMENU];
    }

    return 0;
}

void OnKeyPress(int high, int key, int held)
{
    TRACE("%d %d %d", high, key, held);
    if (KeyData) {
        if (high) {
            key += 128;
        }
        if (KeyData->keymap[key] != held) {
            KeyData->keymap[key] = held;
            KeyData->keys_held++;
        }
    }
}

int16_t KeyGet()
{
    if (KeyData) {
        for (int16_t key = 0; key < 256; key++) {
            if (KeyData->keymap[key]) {
                return key;
            }
        }
    }
    return -1;
}

void KeyClearBuffer()
{
    if (KeyData) {
        KeyData->bufchars = 0;
        KeyData->bufout = 0;
        KeyData->bufin = 0;
    }
}

void S_UpdateInput()
{
    int32_t linput = 0;

    WinInReadJoystick();
    if (JoyYPos >= -8) {
        if (JoyYPos > 8) {
            linput = IN_RIGHT;
        }
    } else {
        linput = IN_LEFT;
    }
    if (JoyXPos <= 8) {
        if (JoyXPos < -8) {
            linput |= IN_FORWARD;
        }
    } else {
        linput |= IN_BACK;
    }

    if (Key_(KEY_UP)) {
        linput |= IN_FORWARD;
    }
    if (Key_(KEY_DOWN)) {
        linput |= IN_BACK;
    }
    if (Key_(KEY_LEFT)) {
        linput |= IN_LEFT;
    }
    if (Key_(KEY_RIGHT)) {
        linput |= IN_RIGHT;
    }
    if (Key_(KEY_STEP_L)) {
        linput |= IN_STEPL;
    }
    if (Key_(KEY_STEP_R)) {
        linput |= IN_STEPR;
    }
    if (Key_(KEY_SLOW)) {
        linput |= IN_SLOW;
    }
    if (Key_(KEY_JUMP)) {
        linput |= IN_JUMP;
    }
    if (Key_(KEY_ACTION)) {
        linput |= IN_ACTION;
    }
    if (Key_(KEY_DRAW)) {
        linput |= IN_DRAW;
    }
    if (Key_(KEY_LOOK)) {
        linput |= IN_LOOK;
    }
    if (Key_(KEY_ROLL)) {
        linput |= IN_ROLL;
    }
    if (Key_(KEY_OPTION) && Camera.type != CAM_CINEMATIC) {
        linput |= IN_OPTION;
    }
    if (Key_(KEY_PAUSE)) {
        linput |= IN_PAUSE;
    }
    if ((linput & IN_FORWARD) && (linput & IN_BACK)) {
        linput |= IN_ROLL;
    }

    if (T1MConfig.enable_cheats) {
        static int8_t is_stuff_cheat_key_pressed = 0;
        if (Key_(KEY_ITEM_CHEAT)) {
            if (!is_stuff_cheat_key_pressed) {
                is_stuff_cheat_key_pressed = 1;
                linput |= IN_ITEM_CHEAT;
            }
        } else {
            is_stuff_cheat_key_pressed = 0;
        }

        if (Key_(KEY_FLY_CHEAT)) {
            linput |= IN_FLY_CHEAT;
        }
        if (Key_(KEY_LEVEL_SKIP_CHEAT)) {
            LevelComplete = 1;
        }
        if (KeyData->keymap[DIK_F11] && LaraItem) {
            LaraItem->hit_points += linput & IN_SLOW ? -20 : 20;
            if (LaraItem->hit_points < 0) {
                LaraItem->hit_points = 0;
            }
            if (LaraItem->hit_points > LARA_HITPOINTS) {
                LaraItem->hit_points = LARA_HITPOINTS;
            }
        }
    }

    if (T1MConfig.enable_numeric_keys) {
        if (KeyData->keymap[DIK_1] && Inv_RequestItem(O_GUN_ITEM)) {
            Lara.request_gun_type = LGT_PISTOLS;
        } else if (KeyData->keymap[DIK_2] && Inv_RequestItem(O_SHOTGUN_ITEM)) {
            Lara.request_gun_type = LGT_SHOTGUN;
        } else if (KeyData->keymap[DIK_3] && Inv_RequestItem(O_MAGNUM_ITEM)) {
            Lara.request_gun_type = LGT_MAGNUMS;
        } else if (KeyData->keymap[DIK_4] && Inv_RequestItem(O_UZI_ITEM)) {
            Lara.request_gun_type = LGT_UZIS;
        }

        if (MedipackCoolDown) {
            MedipackCoolDown--;
        } else {
            if (KeyData->keymap[DIK_8] && Inv_RequestItem(O_MEDI_OPTION)) {
                UseItem(O_MEDI_OPTION);
                MedipackCoolDown = HALF_SECOND;
            } else if (
                KeyData->keymap[DIK_9] && Inv_RequestItem(O_BIGMEDI_OPTION)) {
                UseItem(O_BIGMEDI_OPTION);
                MedipackCoolDown = HALF_SECOND;
            }
        }
    }

    if (KeyData->keymap[DIK_RETURN] || (linput & IN_ACTION)) {
        linput |= IN_SELECT;
    }
    if (KeyData->keymap[DIK_ESCAPE]) {
        linput |= IN_DESELECT;
    }

    if ((linput & (IN_RIGHT | IN_LEFT)) == (IN_RIGHT | IN_LEFT)) {
        linput &= ~(IN_RIGHT | IN_LEFT);
    }

    if (!ModeLock && Camera.type != CAM_CINEMATIC) {
        if (KeyData->keymap[DIK_F5]) {
            linput |= IN_SAVE;
        } else if (KeyData->keymap[DIK_F6]) {
            linput |= IN_LOAD;
        }
    }

    if (IsHardwareRenderer) {
        if (KeyData->keymap[DIK_F3]) {
            AppSettings ^= 2u;
            do
                WinVidSpinMessageLoop();
            while (KeyData->keymap[DIK_F3]);
        }

        if (KeyData->keymap[DIK_F4]) {
            AppSettings ^= 1u;
            do {
                WinVidSpinMessageLoop();
            } while (KeyData->keymap[DIK_F4]);
        }

        if (KeyData->keymap[DIK_F2]) {
            AppSettings ^= 4u;
            do {
                WinVidSpinMessageLoop();
            } while (KeyData->keymap[DIK_F2]);
        }
    }

    Input = linput;
    return;
}

int32_t GetDebouncedInput(int32_t input)
{
    int32_t result = input & ~OldInputDB;
    OldInputDB = input;
    return result;
}

void T1MInjectSpecificInput()
{
    INJECT(0x0041E3E0, Key_);
    INJECT(0x0041E550, S_UpdateInput);
    INJECT(0x00437B70, OnKeyPress);
    INJECT(0x00437BC0, KeyGet);
    INJECT(0x00437BD0, KeyClearBuffer);
}
