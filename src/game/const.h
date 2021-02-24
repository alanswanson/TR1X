#ifndef T1M_GAME_CONST_H
#define T1M_GAME_CONST_H

#define PHD_ONE 0x10000
#define PHD_DEGREE (PHD_ONE / 360) // = 182
#define NUM_PU 3
#define NUM_SLOTS 8
#define MAX_ROOMS 1024
#define MAX_FRAMES 10
#define MAX_TEXTURES 2048
#define NUMBER_ITEMS 256
#define MAX_SECRETS 16
#define MAX_SAVEGAME_BUFFER (10 * 1024)
#define GRAVITY 6
#define FASTFALL_SPEED 128
#define LARA_HITPOINTS 1000
#define LARA_AIR 1800
#define LARA_TURN_UNDO (2 * PHD_DEGREE) // = 364
#define LARA_TURN_RATE ((PHD_DEGREE / 4) + LARA_TURN_UNDO) // = 409
#define LARA_SLOW_TURN ((PHD_DEGREE * 2) + LARA_TURN_UNDO) // = 728
#define LARA_JUMP_TURN ((PHD_DEGREE * 1) + LARA_TURN_UNDO) // = 546
#define LARA_MED_TURN ((PHD_DEGREE * 4) + LARA_TURN_UNDO) // = 1092
#define LARA_FAST_TURN ((PHD_DEGREE * 6) + LARA_TURN_UNDO) // = 1456
#define LARA_LEAN_UNDO PHD_DEGREE
#define LARA_DEF_ADD_EDGE (5 * PHD_DEGREE) // = 910
#define LARA_LEAN_RATE 273
#define LARA_LEAN_MAX ((10 * PHD_DEGREE) + LARA_LEAN_UNDO) // = 2002
#define LARA_LEAN_MAX_UW (LARA_LEAN_MAX * 2)
#define LARA_FASTFALL_SPEED (FASTFALL_SPEED + 3) // = 131
#define LARA_RAD 100 // global radius of Lara
#define LARA_HITE 762 // global height of Lara - less than 3/4 block
#define UW_MAXSPEED 200
#define UW_RADIUS 300
#define UW_HITE 400
#define UW_WALLDEFLECT (2 * PHD_DEGREE) // = 364
#define SURF_MAXSPEED 60
#define SURF_RADIUS 100
#define SURF_HITE 700
#define WATER_FRICTION 6
#define DAMAGE_START 140
#define DAMAGE_LENGTH 14
#define NO_ITEM (-1)
#define NO_CAMERA (-1)
#define PELLET_SCATTER (20 * PHD_DEGREE)
#define NUM_SG_SHELLS 2
#define GUN_AMMO_CLIP 16
#define MAGNUM_AMMO_CLIP 25
#define UZI_AMMO_CLIP 50
#define SHOTGUN_AMMO_CLIP 6
#define GUN_AMMO_QTY (GUN_AMMO_CLIP * 2)
#define MAGNUM_AMMO_QTY (MAGNUM_AMMO_CLIP * 2)
#define UZI_AMMO_QTY (UZI_AMMO_CLIP * 2)
#define SHOTGUN_AMMO_QTY (SHOTGUN_AMMO_CLIP * NUM_SG_SHELLS)
#define SFX_ALWAYS 2
#define NUM_EFFECTS 100
#define DEATH_WAIT (10 * 30)
#define DEATH_WAIT_MIN (2 * 30)
#define MAX_HEAD_ROTATION (50 * PHD_DEGREE) // = 9100
#define MAX_HEAD_TILT (22 * PHD_DEGREE) // = 4004
#define HEAD_TURN (4 * PHD_DEGREE) // = 728
#define HEAD_TURN_SURF (3 * PHD_DEGREE) // = 546
#define MAX_HEAD_ROTATION_SURF (50 * PHD_DEGREE) // = 9100
#define MAX_HEAD_TILT_SURF (40 * PHD_DEGREE) // = 7280
#define MIN_HEAD_TILT_SURF (-40 * PHD_DEGREE) // = -7280
#define MIN_HEAD_TILT (-42 * PHD_DEGREE) // = -7644
#define DIVE_COUNT 10
#define WALL_L 1024
#define WALL_SHIFT 10
#define STEP_L 256
#define STEPUP_HEIGHT ((STEP_L * 3) / 2) // = 384
#define FRONT_ARC 16384
#define MAX_HEAD_CHANGE (PHD_DEGREE * 5) // = 910
#define MAX_TILT (PHD_DEGREE * 3) // = 546
#define CAM_A_HANG 0
#define CAM_E_HANG (-60 * PHD_DEGREE) // = -10920
#define W2V_SHIFT 14
#define W2V_SCALE (1 << W2V_SHIFT)
#define FOLLOW_CENTRE 1
#define NO_CHUNKY 2
#define CHASE_OBJECT 3
#define CHASE_SPEED 12
#define COMBAT_DISTANCE (WALL_L * 5 / 2) // = 2560
#define MAX_ELEVATION (85 * PHD_DEGREE) // = 15470
#define DEFAULT_RADIUS 10
#define DONT_TARGET (-16384)
#define UNIT_SHADOW 256
#define NO_HEIGHT (-32512)
#define NO_BAD_POS (-NO_HEIGHT)
#define NO_BAD_NEG NO_HEIGHT
#define BAD_JUMP_CEILING ((STEP_L * 3) / 4) // = 192
#define WIBBLE_SIZE 32
#define MAX_WIBBLE 2
#define MAX_SHADE 0x300
#define MAX_EXPANSION 5
#define NO_BOX (-1)
#define BOX_NUMBER 0x7FFF
#define EXPAND_LEFT 0x1
#define EXPAND_RIGHT 0x2
#define EXPAND_TOP 0x4
#define EXPAND_BOTTOM 0x8
#define BLOCKABLE 0x8000
#define BLOCKED 0x4000
#define OVERLAP_INDEX 0x3FFF
#define SEARCH_NUMBER 0x7FFF
#define BLOCKED_SEARCH 0x8000
#define CLIP_LEFT 1
#define CLIP_RIGHT 2
#define CLIP_TOP 4
#define CLIP_BOTTOM 8
#define ALL_CLIP (CLIP_LEFT | CLIP_RIGHT | CLIP_TOP | CLIP_BOTTOM)
#define SECONDARY_CLIP 16
#define STALK_DIST (WALL_L * 3) // = 3072
#define ESCAPE_DIST (WALL_L * 5) // = 5120
#define ATTACK_RANGE SQUARE(WALL_L * 3) // = 9437184
#define ESCAPE_CHANCE 2048
#define RECOVER_CHANCE 256

#define BIFF (WALL_L >> 1)

#ifdef T1M_FEAT_EXTENDED_MEMORY
    #define MAX_STRING_SIZE 100
    #define MAX_TEXT_STRINGS 64
#else
    #define MAX_STRING_SIZE 64
    #define MAX_TEXT_STRINGS 32
#endif

#define TEXT_HEIGHT 11
#define FRAME_BOUND_MIN_X 0
#define FRAME_BOUND_MAX_X 1
#define FRAME_BOUND_MIN_Y 2
#define FRAME_BOUND_MAX_Y 3
#define FRAME_BOUND_MIN_Z 4
#define FRAME_BOUND_MAX_Z 5
#define FRAME_POS_X 6
#define FRAME_POS_Y 7
#define FRAME_POS_Z 8
#define SLOPE_DIF 60
#define VAULT_ANGLE (30 * PHD_DEGREE)
#define HANG_ANGLE (35 * PHD_DEGREE)
#define FRAME_ROT 10
#define END_BIT 0x8000
#define DATA_TYPE 0x00FF
#define VALUE_BITS 0x03FF
#define TRIG_BITS(T) ((T & 0x3FFF) >> 10)

#define MIN_SQUARE SQUARE(WALL_L / 4) // = 65536
#define GROUND_SHIFT (STEP_L)

#define DEFAULT_RADIUS 10
#define BEAR_HITPOINTS 20
#define BEAR_RADIUS (WALL_L / 3) // = 341
#define BEAR_SMARTNESS 0x4000
#define WOLF_HITPOINTS 6
#define WOLF_RADIUS (WALL_L / 3) // = 341
#define WOLF_SMARTNESS 0x2000
#define BAT_HITPOINTS 1
#define BAT_RADIUS (WALL_L / 10) // = 102
#define BAT_SMARTNESS 0x400
#define DINOSAUR_HITPOINTS 100
#define DINOSAUR_RADIUS (WALL_L / 3) // = 341
#define DINOSAUR_SMARTNESS 0x7fff
#define RAPTOR_HITPOINTS 20
#define RAPTOR_RADIUS (WALL_L / 3) // = 341
#define RAPTOR_SMARTNESS 0x4000
#define CROCODILE_HITPOINTS 20
#define CROCODILE_RADIUS (WALL_L / 3) // = 341
#define CROCODILE_SMARTNESS 0x2000
#define ALLIGATOR_HITPOINTS 20
#define ALLIGATOR_RADIUS (WALL_L / 3) // = 341
#define ALLIGATOR_SMARTNESS 0x400
#define LARSON_HITPOINTS 50
#define LARSON_RADIUS (WALL_L / 10) // = 102
#define LARSON_SMARTNESS 0x7fff
#define PIERRE_HITPOINTS 70
#define PIERRE_RADIUS (WALL_L / 10) // = 102
#define PIERRE_SMARTNESS 0x7fff
#define RAT_HITPOINTS 5
#define RAT_RADIUS (WALL_L / 5) // = 204
#define RAT_SMARTNESS 0x2000
#define APE_HITPOINTS 22
#define APE_RADIUS (WALL_L / 3) // = 341
#define APE_SMARTNESS 0x7fff
#define LION_HITPOINTS 30
#define LION_RADIUS (WALL_L / 3) // = 341
#define LION_SMARTNESS 0x7fff
#define LIONESS_HITPOINTS 25
#define LIONESS_RADIUS (WALL_L / 3) // = 341
#define LIONESS_SMARTNESS 0x2000
#define PUMA_HITPOINTS 45
#define PUMA_RADIUS (WALL_L / 3) // = 341
#define PUMA_SMARTNESS 0x2000
#define FLYER_HITPOINTS 50
#define FLYER_RADIUS (WALL_L / 3) // = 341
#define FLYER_SMARTNESS 0x7fff
#define WARRIOR2_SMARTNESS 0x2000
#define CENTAUR_HITPOINTS 120
#define CENTAUR_RADIUS (WALL_L / 3) // = 341
#define CENTAUR_SMARTNESS 0x7fff
#define MUMMY_HITPOINTS 18
#define SKATEKID_HITPOINTS 125
#define SKATEKID_RADIUS (WALL_L / 5) // = 204
#define SKATEKID_SMARTNESS 0x7fff
#define COWBOY_HITPOINTS 150
#define COWBOY_RADIUS (WALL_L / 10) // = 102
#define COWBOY_SMARTNESS 0x7fff
#define BALDY_HITPOINTS 200
#define BALDY_RADIUS (WALL_L / 10) // = 102
#define BALDY_SMARTNESS 0x7fff
#define ABORTION_HITPOINTS 500
#define ABORTION_RADIUS (WALL_L / 3) // = 341
#define ABORTION_SMARTNESS 0x7fff
#define NATLA_HITPOINTS 400
#define NATLA_RADIUS (WALL_L / 5) // = 204
#define NATLA_SMARTNESS 0x7fff

#endif
