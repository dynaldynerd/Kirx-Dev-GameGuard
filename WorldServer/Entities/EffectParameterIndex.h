#pragma once

// Verified effect slot names only.
// Add new entries after the slot behavior is confirmed from writer and reader paths.

enum EFFECT_STATE_INDEX : unsigned int
{
  EFF_STATE_INVISIBLE = 26,
};

enum EFFECT_HAVE_INDEX : unsigned int
{
  EFF_HAVE_HP_RESTORE_FROM_MAX_HP_RATE = 12,
  EFF_HAVE_FP_RESTORE_FROM_MAX_FP_RATE = 13,
  EFF_HAVE_SP_RESTORE_FROM_MAX_SP_RATE = 14,
  EFF_HAVE_JADE_RETURN_FULL_DAMAGE = 42,
  EFF_HAVE_JADE_INTERACTION_RESTRICT = 50,
  EFF_HAVE_PARTY_JOIN_LEVEL_BONUS = 53,
  EFF_HAVE_JADE_RETURN_DAMAGE_RATE = 54,
};
