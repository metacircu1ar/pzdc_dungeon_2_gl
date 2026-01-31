#define _GNU_SOURCE
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <yaml.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define NAME_MAX_LEN 20

typedef struct {
  uint32_t codepoint;
  float u0, v0, u1, v1;
} Glyph;

typedef struct {
  char *text;
  uint32_t *cells;
  size_t len_cells;
} Line;

typedef struct {
  Line *lines;
  size_t line_count;
  size_t max_cols;
} View;

typedef struct {
  int line_idx;
  char placeholder;
  char modifier;
  char **methods;
  size_t method_count;
} InsertOption;

typedef struct {
  int y0, y1, x0, x1;
} ArtSlot;

typedef struct {
  char *name;
  int y0, y1, x0, x1;
} PartialSlot;

typedef struct {
  View view;
  InsertOption *inserts;
  size_t insert_count;
  ArtSlot *arts;
  size_t art_count;
  PartialSlot *partials;
  size_t partial_count;
} Menu;

typedef struct {
  char *name;
  View view;
} Art;

typedef struct {
  Art *arts;
  size_t art_count;
} ArtFile;

typedef struct {
  char *key;
  char *value;
} KV;

typedef struct {
  KV *items;
  size_t count;
} ValueMap;

typedef struct {
  char *name;
  char *path;
} ArtArg;

typedef struct {
  GLuint tex;
  Glyph *glyphs;
  size_t glyph_count;
  uint32_t *glyph_list;
  size_t glyph_list_count;
  int grid_w;
  int grid_h;
} RenderState;

typedef struct {
  char **lines;
  size_t count;
  size_t cap;
} LogBuffer;

typedef enum {
  NODE_SCALAR,
  NODE_SEQ,
  NODE_MAP
} NodeType;

typedef struct Node Node;

struct Node {
  NodeType type;
  char *scalar;
  Node **seq;
  size_t seq_len;
  struct {
    char **keys;
    Node **values;
    size_t len;
  } map;
};

typedef struct {
  char code[32];
  char name[64];
  int min_dmg;
  int max_dmg;
  int accuracy;
  int block_chance;
  int armor_penetration;
  int price;
  int enhance_min_dmg;
  int enhance_max_dmg;
  int enhance_accuracy;
  int enhance_block_chance;
  int enhance_armor_penetration;
  bool enhanced;
  char enhance_name[64];
} WeaponItem;

typedef struct {
  char code[32];
  char name[64];
  int armor;
  int accuracy;
  int price;
  int enhance_armor;
  int enhance_accuracy;
  bool enhanced;
  char enhance_name[64];
} ArmorItem;

typedef struct {
  char code[32];
  char name[64];
  int armor;
  int accuracy;
  int block_chance;
  int min_dmg;
  int max_dmg;
  int price;
  int enhance_armor;
  int enhance_accuracy;
  int enhance_block_chance;
  int enhance_min_dmg;
  int enhance_max_dmg;
  bool enhanced;
  char enhance_name[64];
} ShieldItem;

typedef enum {
  SKILL_ACTIVE,
  SKILL_PASSIVE,
  SKILL_CAMP
} SkillType;

typedef struct {
  SkillType type;
  char code[32];
  char name[64];
  int lvl;
  int mp_cost;
  int hp_cost;
} Skill;

typedef struct {
  char code[32];
  char name[64];
  int hp;
  int mp;
  int min_dmg;
  int max_dmg;
  int armor_penetration;
  int accuracy;
  int armor;
  int skill_points;
  char **weapon_options;
  size_t weapon_count;
  char **body_armor_options;
  size_t body_armor_count;
  char **head_armor_options;
  size_t head_armor_count;
  char **arms_armor_options;
  size_t arms_armor_count;
  char **shield_options;
  size_t shield_count;
} HeroTemplate;

typedef struct {
  char code[32];
  char code_name[32];
  char name[64];
  int hp;
  int min_dmg;
  int max_dmg;
  int armor_penetration;
  int accuracy;
  int armor;
  int regen_hp;
  int exp_gived;
  int coins_gived;
  char **weapon_options;
  size_t weapon_count;
  char **body_armor_options;
  size_t body_armor_count;
  char **head_armor_options;
  size_t head_armor_count;
  char **arms_armor_options;
  size_t arms_armor_count;
  char **shield_options;
  size_t shield_count;
  char **ingredient_options;
  size_t ingredient_count;
  bool is_boss;
} EnemyTemplate;

typedef struct {
  char name[16];
  EnemyTemplate *enemies;
  size_t enemy_count;
} DungeonData;

typedef struct {
  char code[32];
  char name[64];
  int hp;
  int hp_max;
  int regen_hp_base;
  int mp;
  int mp_max;
  int regen_mp_base;
  int min_dmg_base;
  int max_dmg_base;
  int armor_penetration_base;
  int accuracy_base;
  int armor_base;
  int block_chance_base;
  int exp;
  int lvl;
  int stat_points;
  int skill_points;
  int pzdc_monolith_points;
  int coins;
  int exp_gived;
  int coins_gived;
  char background[32];
  char dungeon_name[16];
  int dungeon_part_number;
  int leveling;
  char ingredient[32];
  WeaponItem weapon;
  ArmorItem body_armor;
  ArmorItem head_armor;
  ArmorItem arms_armor;
  ShieldItem shield;
  Skill active_skill;
  Skill passive_skill;
  Skill camp_skill;
  ValueMap ingredients;
} Character;

typedef struct {
  char weapon[3][32];
  char body_armor[3][32];
  char head_armor[3][32];
  char arms_armor[3][32];
  char shield[3][32];
} ShopData;

typedef struct {
  int coins;
  char weapon[32];
  char body_armor[32];
  char head_armor[32];
  char arms_armor[32];
  char shield[32];
} WarehouseData;

typedef struct {
  int points;
  int hp;
  int mp;
  int accuracy;
  int damage;
  int stat_points;
  int skill_points;
  int armor;
  int regen_hp;
  int regen_mp;
  int armor_penetration;
  int block_chance;
} MonolithData;

typedef struct {
  char name[32];
  int count;
} RecipeIngredient;

typedef struct {
  int accuracy;
  int min_dmg;
  int max_dmg;
  int block_chance;
  int armor;
  int armor_penetration;
} RecipeEffect;

typedef struct {
  char code[64];
  int view_code;
  char name[64];
  int price;
  RecipeIngredient *ingredients;
  size_t ingredient_count;
  RecipeEffect weapon;
  RecipeEffect head_armor;
  RecipeEffect body_armor;
  RecipeEffect arms_armor;
  RecipeEffect shield;
  bool purchased;
} OccultRecipe;

typedef struct {
  OccultRecipe *recipes;
  size_t recipe_count;
} OccultLibraryData;

typedef struct {
  int bandits[6];
  int undeads[6];
  int swamp[6];
  int pzdc[3];
} StatisticsTotal;

typedef struct {
  char type[16];
  char code[32];
} LootEntry;

typedef enum {
  EVENT_EFFECT_NONE,
  EVENT_EFFECT_COINS,
  EVENT_EFFECT_HP,
  EVENT_EFFECT_MP,
  EVENT_EFFECT_INGREDIENT,
  EVENT_EFFECT_GAMBLE
} EventEffectType;

typedef enum {
  EVENT_INPUT_NONE,
  EVENT_INPUT_DIGIT,
  EVENT_INPUT_TEXT
} EventInputMode;

typedef enum {
  EVENT_PENDING_NONE,
  EVENT_PENDING_GRAVE_DIG,
  EVENT_PENDING_GRAVE_REWARD,
  EVENT_PENDING_PIG_SALLET
} EventPendingAction;

typedef struct {
  char code[32];
  char name[48];
  char desc[5][48];
  char art_path[64];
  EventEffectType effect;
  int value;
  char ingredient[32];
} EventDef;

typedef enum {
  STATE_START,
  STATE_LOAD_MENU,
  STATE_LOAD_NO_HERO,
  STATE_LOAD_CONFIRM,
  STATE_CHOOSE_DUNGEON,
  STATE_NAME_INPUT,
  STATE_HERO_SELECT,
  STATE_SKILL_ACTIVE,
  STATE_SKILL_PASSIVE,
  STATE_SKILL_CAMP,
  STATE_ENEMY_SELECT,
  STATE_BATTLE,
  STATE_CAMPFIRE,
  STATE_CAMP,
  STATE_MONOLITH,
  STATE_OCCULT_LIBRARY,
  STATE_OL_RECIPE,
  STATE_OL_ENHANCE_LIST,
  STATE_OL_ENHANCE,
  STATE_STATS_CHOOSE,
  STATE_STATS_SHOW,
  STATE_LOOT,
  STATE_LOOT_MESSAGE,
  STATE_EVENT_SELECT,
  STATE_EVENT_RESULT,
  STATE_OPTIONS,
  STATE_OPTIONS_ANIM,
  STATE_OPTIONS_REPLACE,
  STATE_CREDITS,
  STATE_SHOP,
  STATE_AMMO_SHOW,
  STATE_HERO_INFO,
  STATE_SPEND_STAT,
  STATE_SPEND_SKILL,
  STATE_MESSAGE
} GameState;

typedef struct {
  GameState state;
  GameState next_state;
  char message_title[128];
  char message_art_name[32];
  char message_art_path[64];
  LogBuffer log;
  HeroTemplate *heroes;
  size_t hero_count;
  DungeonData dungeons[3];
  EnemyTemplate *event_enemies;
  size_t event_enemy_count;
  WeaponItem *weapons;
  size_t weapon_count;
  ArmorItem *body_armors;
  size_t body_armor_count;
  ArmorItem *head_armors;
  size_t head_armor_count;
  ArmorItem *arms_armors;
  size_t arms_armor_count;
  ShieldItem *shields;
  size_t shield_count;
  int dungeon_index;
  Character hero;
  Character enemy;
  Character enemy_choices[3];
  int enemy_choice_count;
  int enemy_choice_is_boss[3];
  int enemy_is_boss;
  char enemy_choose_message[128];
  int hero_selected;
  char name_input[32];
  size_t name_len;
  char name_error[128];
  int stat_dice1;
  int stat_dice2;
  int stat_roll;
  int skill_dice1;
  int skill_dice2;
  int skill_choice_count;
  SkillType skill_choices[3];
  ShopData shop;
  WarehouseData warehouse;
  MonolithData monolith;
  OccultLibraryData occult;
  StatisticsTotal stats_total;
  int stats_dungeon_index;
  int current_recipe_index;
  LootEntry loot_items[5];
  int loot_count;
  int loot_index;
  int loot_show_coins;
  int loot_show_ingredient;
  int loot_message_mode;
  int loot_coins;
  char loot_ingredient[32];
  char loot_message[256];
  int pending_levelup;
  EventDef event_choices[3];
  int event_choice_count;
  EventDef current_event;
  char event_message[192];
  char event_art_path[64];
  char event_art_name[32];
  char event_choose_message[128];
  char event_code[32];
  int event_step;
  int event_data[4];
  EventInputMode event_input_mode;
  char event_text[64];
  size_t event_text_len;
  EventPendingAction event_pending_action;
  int wg_taken;
  char wg_enemy[32];
  int wg_count;
  int wg_level;
  int anim_speed_index;
  int screen_replace_type;
  char battle_art_name[32];
  char battle_art_dungeon[16];
  char battle_anim_seq[4][32];
  int battle_anim_active;
  int battle_anim_step;
  int battle_anim_count;
  uint32_t battle_anim_deadline;
  int battle_exit_pending;
  GameState battle_exit_state;
  int force_instant_redraw;
  GameState loot_return_state;
  int loot_return_pending;
  int loot_last_taken;
  GameState return_state;
  char ammo_show_type[16];
  char ammo_show_code[32];
} Game;

static int rand_range(int min, int max);
static void event_after_loot(Game *g);
static void event_begin(Game *g, const EventDef *ev);
static void event_handle_digit(Game *g, int digit);
static void event_handle_text(Game *g, const char *text);
static int stats_total_get(const StatisticsTotal *s, const char *dungeon, const char *enemy_code);
static const EnemyTemplate *event_enemy_by_code(const Game *g, const char *code);

static bool file_exists(const char *path) {
  struct stat st;
  return stat(path, &st) == 0;
}

static char *strdup_safe(const char *s) {
  size_t n = strlen(s);
  char *out = (char *)malloc(n + 1);
  if (!out) return NULL;
  memcpy(out, s, n + 1);
  return out;
}

static void rtrim_inplace(char *s) {
  size_t n = strlen(s);
  while (n > 0 && isspace((unsigned char)s[n - 1])) {
    s[n - 1] = '\0';
    n--;
  }
}

static const char *ltrim(const char *s) {
  while (*s && isspace((unsigned char)*s)) s++;
  return s;
}

static void trim_both_inplace(char *s) {
  if (!s) return;
  const char *start = ltrim(s);
  if (start != s) memmove(s, start, strlen(start) + 1);
  rtrim_inplace(s);
}

static bool str_has_letter(const char *s) {
  if (!s) return false;
  while (*s) {
    unsigned char c = (unsigned char)*s;
    if (isalpha(c) || c >= 128) return true;
    s++;
  }
  return false;
}

static void append_text(char *buf, size_t *len, size_t max_len, const char *text) {
  if (!buf || !len || !text) return;
  while (*text) {
    unsigned char c = (unsigned char)*text++;
    if (*len + 1 >= max_len) break;
    if (isprint(c) || c >= 128) {
      buf[*len] = (char)c;
      *len += 1;
      buf[*len] = '\0';
    }
  }
}

static void backspace_text(char *buf, size_t *len) {
  if (!buf || !len) return;
  if (*len == 0) return;
  *len -= 1;
  buf[*len] = '\0';
}

static void yaml_write_escaped(FILE *f, const char *s) {
  fputc('"', f);
  if (s) {
    for (const char *p = s; *p; ++p) {
      if (*p == '"' || *p == '\\') fputc('\\', f);
      fputc(*p, f);
    }
  }
  fputc('"', f);
}

static const char *find_existing_path(const char **candidates, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    if (candidates[i] && file_exists(candidates[i])) return candidates[i];
  }
  return NULL;
}

static bool dir_exists(const char *path) {
  struct stat st;
  if (stat(path, &st) != 0) return false;
  return S_ISDIR(st.st_mode);
}

static char *resolve_saves_dir(void) {
  const char *candidates[] = {
    "saves",
    "demo/pzdc_dungeon_2_gl/saves",
    "../saves",
    "../../saves",
    "../../../saves"
  };
  for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
    if (dir_exists(candidates[i])) return strdup_safe(candidates[i]);
  }
  return NULL;
}

static char *resolve_data_path(const char *path) {
  if (!path) return NULL;
  if (file_exists(path)) return strdup_safe(path);
  const char *candidates[] = { path, NULL, NULL, NULL, NULL, NULL, NULL };
  char buf1[512];
  char buf2[512];
  char buf3[512];
  char buf4[512];
  char buf5[512];
  char buf6[512];
  snprintf(buf1, sizeof(buf1), "../%s", path);
  snprintf(buf2, sizeof(buf2), "../../%s", path);
  snprintf(buf3, sizeof(buf3), "../../../%s", path);
  snprintf(buf4, sizeof(buf4), "demo/pzdc_dungeon_2_gl/%s", path);
  snprintf(buf5, sizeof(buf5), "../demo/pzdc_dungeon_2_gl/%s", path);
  snprintf(buf6, sizeof(buf6), "../../demo/pzdc_dungeon_2_gl/%s", path);
  candidates[1] = buf1;
  candidates[2] = buf2;
  candidates[3] = buf3;
  candidates[4] = buf4;
  candidates[5] = buf5;
  candidates[6] = buf6;
  const char *found = find_existing_path(candidates, 7);
  return found ? strdup_safe(found) : strdup_safe(path);
}

static char *resolve_menu_path(const char *path) {
  if (!path) return NULL;
  if (strstr(path, ".yml")) {
    if (file_exists(path)) return strdup_safe(path);
    const char *candidates[] = { path, NULL, NULL };
    char buf1[512];
    char buf2[512];
    snprintf(buf1, sizeof(buf1), "../%s", path);
    snprintf(buf2, sizeof(buf2), "../../%s", path);
    candidates[1] = buf1;
    candidates[2] = buf2;
    const char *found = find_existing_path(candidates, 3);
    return found ? strdup_safe(found) : strdup_safe(path);
  }

  char buf[512];
  snprintf(buf, sizeof(buf), "views/menues/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "demo/pzdc_dungeon_2_gl/views/menues/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "../views/menues/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "../demo/pzdc_dungeon_2_gl/views/menues/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "../../views/menues/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "../../demo/pzdc_dungeon_2_gl/views/menues/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "views/menues/%s.yml", path);
  return strdup_safe(buf);
}

static char *resolve_art_path(const char *path) {
  if (!path) return NULL;
  if (strstr(path, ".yml")) {
    if (file_exists(path)) return strdup_safe(path);
    const char *candidates[] = { path, NULL, NULL };
    char buf1[512];
    char buf2[512];
    snprintf(buf1, sizeof(buf1), "../%s", path);
    snprintf(buf2, sizeof(buf2), "../../%s", path);
    candidates[1] = buf1;
    candidates[2] = buf2;
    const char *found = find_existing_path(candidates, 3);
    return found ? strdup_safe(found) : strdup_safe(path);
  }

  char buf[512];
  snprintf(buf, sizeof(buf), "views/arts/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "demo/pzdc_dungeon_2_gl/views/arts/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "../views/arts/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "../demo/pzdc_dungeon_2_gl/views/arts/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "../../views/arts/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "../../demo/pzdc_dungeon_2_gl/views/arts/%s.yml", path);
  if (file_exists(buf)) return strdup_safe(buf);
  snprintf(buf, sizeof(buf), "views/arts/%s.yml", path);
  return strdup_safe(buf);
}

static size_t utf8_decode(const char *s, size_t len, size_t i, uint32_t *out) {
  unsigned char c = (unsigned char)s[i];
  if (c < 0x80) {
    *out = c;
    return 1;
  } else if ((c >> 5) == 0x6 && i + 1 < len) {
    *out = ((uint32_t)(c & 0x1F) << 6) | (uint32_t)(s[i + 1] & 0x3F);
    return 2;
  } else if ((c >> 4) == 0xE && i + 2 < len) {
    *out = ((uint32_t)(c & 0x0F) << 12) | ((uint32_t)(s[i + 1] & 0x3F) << 6) | (uint32_t)(s[i + 2] & 0x3F);
    return 3;
  } else if ((c >> 3) == 0x1E && i + 3 < len) {
    *out = ((uint32_t)(c & 0x07) << 18) | ((uint32_t)(s[i + 1] & 0x3F) << 12) | ((uint32_t)(s[i + 2] & 0x3F) << 6) | (uint32_t)(s[i + 3] & 0x3F);
    return 4;
  }
  *out = 0xFFFD;
  return 1;
}

static size_t utf8_encode(uint32_t cp, char out[5]) {
  if (cp < 0x80) {
    out[0] = (char)cp;
    out[1] = '\0';
    return 1;
  } else if (cp < 0x800) {
    out[0] = (char)(0xC0 | (cp >> 6));
    out[1] = (char)(0x80 | (cp & 0x3F));
    out[2] = '\0';
    return 2;
  } else if (cp < 0x10000) {
    out[0] = (char)(0xE0 | (cp >> 12));
    out[1] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[2] = (char)(0x80 | (cp & 0x3F));
    out[3] = '\0';
    return 3;
  } else {
    out[0] = (char)(0xF0 | (cp >> 18));
    out[1] = (char)(0x80 | ((cp >> 12) & 0x3F));
    out[2] = (char)(0x80 | ((cp >> 6) & 0x3F));
    out[3] = (char)(0x80 | (cp & 0x3F));
    out[4] = '\0';
    return 4;
  }
}

static void free_view(View *view) {
  if (!view) return;
  for (size_t i = 0; i < view->line_count; ++i) {
    free(view->lines[i].text);
    free(view->lines[i].cells);
  }
  free(view->lines);
  view->lines = NULL;
  view->line_count = 0;
  view->max_cols = 0;
}

static void view_build_cells(View *view) {
  if (!view) return;
  size_t max_cols = 0;
  for (size_t i = 0; i < view->line_count; ++i) {
    Line *line = &view->lines[i];
    size_t len = strlen(line->text);
    size_t count = 0;
    for (size_t j = 0; j < len;) {
      uint32_t cp = 0;
      size_t adv = utf8_decode(line->text, len, j, &cp);
      (void)cp;
      count++;
      j += adv;
    }
    if (count > max_cols) max_cols = count;
  }
  view->max_cols = max_cols;

  for (size_t i = 0; i < view->line_count; ++i) {
    Line *line = &view->lines[i];
    free(line->cells);
    line->cells = (uint32_t *)malloc(max_cols * sizeof(uint32_t));
    line->len_cells = max_cols;
    for (size_t c = 0; c < max_cols; ++c) line->cells[c] = (uint32_t)' ';

    size_t len = strlen(line->text);
    size_t col = 0;
    for (size_t j = 0; j < len && col < max_cols;) {
      uint32_t cp = 0;
      size_t adv = utf8_decode(line->text, len, j, &cp);
      line->cells[col++] = cp;
      j += adv;
    }
  }
}

static void value_map_clear(ValueMap *map) {
  if (!map) return;
  for (size_t i = 0; i < map->count; ++i) {
    free(map->items[i].key);
    free(map->items[i].value);
  }
  free(map->items);
  map->items = NULL;
  map->count = 0;
}

static void value_map_set(ValueMap *map, const char *key, const char *value) {
  if (!map || !key || !value) return;
  for (size_t i = 0; i < map->count; ++i) {
    if (strcmp(map->items[i].key, key) == 0) {
      free(map->items[i].value);
      map->items[i].value = strdup_safe(value);
      return;
    }
  }
  KV *items = (KV *)realloc(map->items, (map->count + 1) * sizeof(KV));
  if (!items) return;
  map->items = items;
  map->items[map->count].key = strdup_safe(key);
  map->items[map->count].value = strdup_safe(value);
  map->count++;
}

static void value_map_set_int(ValueMap *map, const char *key, int value) {
  char buf[32];
  snprintf(buf, sizeof(buf), "%d", value);
  value_map_set(map, key, buf);
}

static const char *value_map_get(const ValueMap *map, const char *key) {
  if (!map || !key) return NULL;
  for (size_t i = 0; i < map->count; ++i) {
    if (strcmp(map->items[i].key, key) == 0) return map->items[i].value;
  }
  return NULL;
}

static int value_map_get_int(const ValueMap *map, const char *key, int fallback) {
  const char *s = value_map_get(map, key);
  return s ? atoi(s) : fallback;
}

static void value_map_set_if_missing(ValueMap *map, const char *key, const char *value) {
  if (!value_map_get(map, key)) value_map_set(map, key, value);
}

static void insert_option_free(InsertOption *opt) {
  if (!opt) return;
  for (size_t i = 0; i < opt->method_count; ++i) free(opt->methods[i]);
  free(opt->methods);
  opt->methods = NULL;
  opt->method_count = 0;
}

static void free_menu(Menu *menu) {
  if (!menu) return;
  free_view(&menu->view);
  for (size_t i = 0; i < menu->insert_count; ++i) insert_option_free(&menu->inserts[i]);
  free(menu->inserts);
  for (size_t i = 0; i < menu->partial_count; ++i) free(menu->partials[i].name);
  free(menu->partials);
  free(menu->arts);
  menu->inserts = NULL;
  menu->insert_count = 0;
  menu->partials = NULL;
  menu->partial_count = 0;
  menu->arts = NULL;
  menu->art_count = 0;
}

static void free_art_file(ArtFile *file) {
  if (!file) return;
  for (size_t i = 0; i < file->art_count; ++i) {
    free(file->arts[i].name);
    free_view(&file->arts[i].view);
  }
  free(file->arts);
  file->arts = NULL;
  file->art_count = 0;
}

static Node *node_new(NodeType type) {
  Node *n = (Node *)calloc(1, sizeof(Node));
  if (!n) return NULL;
  n->type = type;
  return n;
}

static void node_free(Node *node) {
  if (!node) return;
  if (node->type == NODE_SCALAR) {
    free(node->scalar);
  } else if (node->type == NODE_SEQ) {
    for (size_t i = 0; i < node->seq_len; ++i) node_free(node->seq[i]);
    free(node->seq);
  } else if (node->type == NODE_MAP) {
    for (size_t i = 0; i < node->map.len; ++i) {
      free(node->map.keys[i]);
      node_free(node->map.values[i]);
    }
    free(node->map.keys);
    free(node->map.values);
  }
  free(node);
}

static void node_seq_push(Node *seq, Node *item) {
  if (!seq || seq->type != NODE_SEQ) return;
  Node **arr = (Node **)realloc(seq->seq, (seq->seq_len + 1) * sizeof(Node *));
  if (!arr) return;
  seq->seq = arr;
  seq->seq[seq->seq_len++] = item;
}

static void node_map_set(Node *map, char *key, Node *value) {
  if (!map || map->type != NODE_MAP || !key) return;
  char **keys = (char **)realloc(map->map.keys, (map->map.len + 1) * sizeof(char *));
  Node **values = (Node **)realloc(map->map.values, (map->map.len + 1) * sizeof(Node *));
  if (!keys || !values) return;
  map->map.keys = keys;
  map->map.values = values;
  map->map.keys[map->map.len] = key;
  map->map.values[map->map.len] = value;
  map->map.len++;
}

static Node *node_map_get(Node *map, const char *key) {
  if (!map || map->type != NODE_MAP || !key) return NULL;
  for (size_t i = 0; i < map->map.len; ++i) {
    if (strcmp(map->map.keys[i], key) == 0) return map->map.values[i];
  }
  return NULL;
}

static const char *node_scalar(Node *node) {
  if (!node || node->type != NODE_SCALAR) return NULL;
  return node->scalar ? node->scalar : "";
}

static int node_int(Node *node, int fallback) {
  const char *s = node_scalar(node);
  if (!s) return fallback;
  return atoi(s);
}

static int node_map_int(Node *map, const char *key, int fallback) {
  if (!map || map->type != NODE_MAP) return fallback;
  return node_int(node_map_get(map, key), fallback);
}

static const char *node_map_str(Node *map, const char *key, const char *fallback) {
  if (!map || map->type != NODE_MAP) return fallback;
  Node *n = node_map_get(map, key);
  const char *s = node_scalar(n);
  return s ? s : fallback;
}

static char **node_string_list(Node *node, size_t *out_count) {
  if (out_count) *out_count = 0;
  if (!node) return NULL;
  char **list = NULL;
  size_t count = 0;

  if (node->type == NODE_SCALAR) {
    list = (char **)malloc(sizeof(char *));
    if (!list) return NULL;
    list[0] = strdup_safe(node->scalar ? node->scalar : "");
    count = 1;
  } else if (node->type == NODE_SEQ) {
    for (size_t i = 0; i < node->seq_len; ++i) {
      const char *s = node_scalar(node->seq[i]);
      if (!s) continue;
      char **arr = (char **)realloc(list, (count + 1) * sizeof(char *));
      if (!arr) continue;
      list = arr;
      list[count++] = strdup_safe(s);
    }
  }

  if (out_count) *out_count = count;
  return list;
}

static void free_string_list(char **list, size_t count) {
  if (!list) return;
  for (size_t i = 0; i < count; ++i) free(list[i]);
  free(list);
}

static Node *yaml_load_file(const char *path) {
  if (!path) return NULL;
  FILE *f = fopen(path, "r");
  if (!f) return NULL;

  yaml_parser_t parser;
  yaml_event_t event;
  if (!yaml_parser_initialize(&parser)) {
    fclose(f);
    return NULL;
  }
  yaml_parser_set_input_file(&parser, f);

  Node *root = NULL;
  Node *stack[128];
  size_t stack_len = 0;
  char *map_key[128];
  bool map_expect_key[128];

  while (yaml_parser_parse(&parser, &event)) {
    bool done = false;
    switch (event.type) {
      case YAML_STREAM_END_EVENT:
        done = true;
        break;
      case YAML_MAPPING_START_EVENT: {
        Node *map = node_new(NODE_MAP);
        if (!map) break;
        if (stack_len == 0) {
          if (!root) root = map;
        } else {
          Node *parent = stack[stack_len - 1];
          if (parent->type == NODE_SEQ) {
            node_seq_push(parent, map);
          } else if (parent->type == NODE_MAP) {
            if (!map_expect_key[stack_len - 1] && map_key[stack_len - 1]) {
              node_map_set(parent, map_key[stack_len - 1], map);
              map_key[stack_len - 1] = NULL;
              map_expect_key[stack_len - 1] = true;
            }
          }
        }
        stack[stack_len] = map;
        map_key[stack_len] = NULL;
        map_expect_key[stack_len] = true;
        stack_len++;
        break;
      }
      case YAML_SEQUENCE_START_EVENT: {
        Node *seq = node_new(NODE_SEQ);
        if (!seq) break;
        if (stack_len == 0) {
          if (!root) root = seq;
        } else {
          Node *parent = stack[stack_len - 1];
          if (parent->type == NODE_SEQ) {
            node_seq_push(parent, seq);
          } else if (parent->type == NODE_MAP) {
            if (!map_expect_key[stack_len - 1] && map_key[stack_len - 1]) {
              node_map_set(parent, map_key[stack_len - 1], seq);
              map_key[stack_len - 1] = NULL;
              map_expect_key[stack_len - 1] = true;
            }
          }
        }
        stack[stack_len] = seq;
        map_key[stack_len] = NULL;
        map_expect_key[stack_len] = false;
        stack_len++;
        break;
      }
      case YAML_SCALAR_EVENT: {
        Node *scalar = node_new(NODE_SCALAR);
        if (!scalar) break;
        scalar->scalar = strdup_safe((const char *)event.data.scalar.value);
        if (stack_len == 0) {
          if (!root) root = scalar;
          else node_free(scalar);
        } else {
          Node *parent = stack[stack_len - 1];
          if (parent->type == NODE_SEQ) {
            node_seq_push(parent, scalar);
          } else if (parent->type == NODE_MAP) {
            if (map_expect_key[stack_len - 1]) {
              map_key[stack_len - 1] = scalar->scalar;
              scalar->scalar = NULL;
              node_free(scalar);
              map_expect_key[stack_len - 1] = false;
            } else if (map_key[stack_len - 1]) {
              node_map_set(parent, map_key[stack_len - 1], scalar);
              map_key[stack_len - 1] = NULL;
              map_expect_key[stack_len - 1] = true;
            }
          }
        }
        break;
      }
      case YAML_MAPPING_END_EVENT:
      case YAML_SEQUENCE_END_EVENT:
        if (stack_len > 0) stack_len--;
        break;
      default:
        break;
    }
    yaml_event_delete(&event);
    if (done) break;
  }

  yaml_parser_delete(&parser);
  fclose(f);
  return root;
}

static bool menu_load(const char *path, Menu *menu) {
  if (!path || !menu) return false;
  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }

  memset(menu, 0, sizeof(*menu));

  Node *view = node_map_get(root, "view");
  if (view && view->type == NODE_SEQ) {
    for (size_t i = 0; i < view->seq_len; ++i) {
      const char *line = node_scalar(view->seq[i]);
      if (!line) continue;
      Line *lines = (Line *)realloc(menu->view.lines, (menu->view.line_count + 1) * sizeof(Line));
      if (!lines) continue;
      menu->view.lines = lines;
      menu->view.lines[menu->view.line_count].text = strdup_safe(line);
      menu->view.lines[menu->view.line_count].cells = NULL;
      menu->view.lines[menu->view.line_count].len_cells = 0;
      menu->view.line_count++;
    }
  }

  Node *insert = node_map_get(root, "insert_options");
  if (insert && insert->type == NODE_MAP) {
    for (size_t i = 0; i < insert->map.len; ++i) {
      const char *line_key = insert->map.keys[i];
      Node *line_map = insert->map.values[i];
      if (!line_key || !line_map || line_map->type != NODE_MAP) continue;
      int line_idx = atoi(line_key);

      for (size_t j = 0; j < line_map->map.len; ++j) {
        const char *ph_key = line_map->map.keys[j];
        Node *ph_map = line_map->map.values[j];
        if (!ph_key || !ph_map || ph_map->type != NODE_MAP) continue;
        char placeholder = ph_key[0];
        char modifier = 's';
        char **methods = NULL;
        size_t method_count = 0;

        Node *methods_node = node_map_get(ph_map, "methods");
        if (methods_node && methods_node->type == NODE_SEQ) {
          for (size_t k = 0; k < methods_node->seq_len; ++k) {
            const char *m = node_scalar(methods_node->seq[k]);
            if (!m) continue;
            char **arr = (char **)realloc(methods, (method_count + 1) * sizeof(char *));
            if (!arr) continue;
            methods = arr;
            methods[method_count++] = strdup_safe(m);
          }
        }

        Node *mod_node = node_map_get(ph_map, "modifier");
        if (mod_node && mod_node->type == NODE_SCALAR && mod_node->scalar && mod_node->scalar[0]) {
          modifier = mod_node->scalar[0];
        }

        InsertOption *ins = (InsertOption *)realloc(menu->inserts, (menu->insert_count + 1) * sizeof(InsertOption));
        if (!ins) {
          for (size_t k = 0; k < method_count; ++k) free(methods[k]);
          free(methods);
          continue;
        }
        menu->inserts = ins;
        InsertOption *opt = &menu->inserts[menu->insert_count++];
        opt->line_idx = line_idx;
        opt->placeholder = placeholder;
        opt->modifier = modifier;
        opt->methods = methods;
        opt->method_count = method_count;
      }
    }
  }

  Node *arts = node_map_get(root, "arts");
  if (arts && arts->type == NODE_SEQ) {
    for (size_t i = 0; i < arts->seq_len; ++i) {
      Node *art_map = arts->seq[i];
      if (!art_map || art_map->type != NODE_MAP) continue;
      ArtSlot slot = {0};

      Node *y = node_map_get(art_map, "y");
      Node *x = node_map_get(art_map, "x");
      if (y && y->type == NODE_SEQ && y->seq_len >= 2) {
        slot.y0 = node_int(y->seq[0], 0);
        slot.y1 = node_int(y->seq[1], 0);
      }
      if (x && x->type == NODE_SEQ && x->seq_len >= 2) {
        slot.x0 = node_int(x->seq[0], 0);
        slot.x1 = node_int(x->seq[1], 0);
      }

      ArtSlot *arr = (ArtSlot *)realloc(menu->arts, (menu->art_count + 1) * sizeof(ArtSlot));
      if (!arr) continue;
      menu->arts = arr;
      menu->arts[menu->art_count++] = slot;
    }
  }

  Node *partials = node_map_get(root, "partials");
  if (partials && partials->type == NODE_SEQ) {
    for (size_t i = 0; i < partials->seq_len; ++i) {
      Node *partial_map = partials->seq[i];
      if (!partial_map || partial_map->type != NODE_MAP) continue;
      PartialSlot slot = {0};

      Node *pn = node_map_get(partial_map, "partial_name");
      if (pn && pn->type == NODE_SCALAR) slot.name = strdup_safe(pn->scalar);

      Node *y = node_map_get(partial_map, "y");
      Node *x = node_map_get(partial_map, "x");
      if (y && y->type == NODE_SEQ && y->seq_len >= 2) {
        slot.y0 = node_int(y->seq[0], 0);
        slot.y1 = node_int(y->seq[1], 0);
      }
      if (x && x->type == NODE_SEQ && x->seq_len >= 2) {
        slot.x0 = node_int(x->seq[0], 0);
        slot.x1 = node_int(x->seq[1], 0);
      }

      PartialSlot *arr = (PartialSlot *)realloc(menu->partials, (menu->partial_count + 1) * sizeof(PartialSlot));
      if (!arr) {
        free(slot.name);
        continue;
      }
      menu->partials = arr;
      menu->partials[menu->partial_count++] = slot;
    }
  }

  node_free(root);
  return menu->view.line_count > 0;
}

static bool artfile_load(const char *path, ArtFile *file) {
  if (!path || !file) return false;
  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }
  memset(file, 0, sizeof(*file));
  for (size_t i = 0; i < root->map.len; ++i) {
    const char *name = root->map.keys[i];
    Node *seq = root->map.values[i];
    if (!name || !seq || seq->type != NODE_SEQ) continue;
    Art *arts = (Art *)realloc(file->arts, (file->art_count + 1) * sizeof(Art));
    if (!arts) continue;
    file->arts = arts;
    Art *art = &file->arts[file->art_count++];
    memset(art, 0, sizeof(*art));
    art->name = strdup_safe(name);
    for (size_t j = 0; j < seq->seq_len; ++j) {
      const char *line = node_scalar(seq->seq[j]);
      if (!line) continue;
      Line *lines = (Line *)realloc(art->view.lines, (art->view.line_count + 1) * sizeof(Line));
      if (!lines) continue;
      art->view.lines = lines;
      art->view.lines[art->view.line_count].text = strdup_safe(line);
      art->view.lines[art->view.line_count].cells = NULL;
      art->view.lines[art->view.line_count].len_cells = 0;
      art->view.line_count++;
    }
  }
  node_free(root);
  return file->art_count > 0;
}

static Art *artfile_find(ArtFile *file, const char *name) {
  if (!file || !name) return NULL;
  for (size_t i = 0; i < file->art_count; ++i) {
    if (strcmp(file->arts[i].name, name) == 0) return &file->arts[i];
  }
  return NULL;
}

static char *read_version(const char *path) {
  if (!path) return NULL;
  FILE *f = fopen(path, "r");
  if (!f) return NULL;
  char *line = NULL;
  size_t cap = 0;
  char *version = NULL;
  while (getline(&line, &cap, f) != -1) {
    char *p = strstr(line, "VERSION");
    if (p) {
      char *q = strchr(p, '\'');
      if (q) {
        char *r = strchr(q + 1, '\'');
        if (r && r > q + 1) {
          size_t n = (size_t)(r - q - 1);
          version = (char *)malloc(n + 4);
          if (version) {
            memcpy(version, "v ", 2);
            memcpy(version + 2, q + 1, n);
            version[2 + n] = '\0';
          }
          break;
        }
      }
    }
  }
  free(line);
  fclose(f);
  return version;
}

static void apply_insert(View *view, const InsertOption *opt, const char *value) {
  if (!view || !opt || !value) return;
  if (opt->line_idx < 0 || (size_t)opt->line_idx >= view->line_count) return;

  char *line = view->lines[opt->line_idx].text;
  char *p = line;
  char ch = opt->placeholder;

  while (*p) {
    if (*p == ch) {
      char *run_start = p;
      while (*p == ch) p++;
      size_t run_len = (size_t)(p - run_start);
      if (run_len >= 3) {
        size_t val_len = strlen(value);
        size_t out_len = run_len;
        char *insert = (char *)malloc(out_len + 1);
        if (!insert) return;
        if (val_len >= out_len) {
          memcpy(insert, value, out_len);
        } else {
          size_t pad = out_len - val_len;
          if (opt->modifier == 'm') {
            size_t left = pad / 2;
            size_t right = pad - left;
            memset(insert, ' ', left);
            memcpy(insert + left, value, val_len);
            memset(insert + left + val_len, ' ', right);
          } else if (opt->modifier == 'e') {
            memset(insert, ' ', pad);
            memcpy(insert + pad, value, val_len);
          } else {
            memcpy(insert, value, val_len);
            memset(insert + val_len, ' ', pad);
          }
        }
        insert[out_len] = '\0';

        size_t prefix_len = (size_t)(run_start - line);
        size_t suffix_len = strlen(p);
        size_t new_len = prefix_len + out_len + suffix_len;
        char *new_line = (char *)malloc(new_len + 1);
        if (!new_line) {
          free(insert);
          return;
        }
        memcpy(new_line, line, prefix_len);
        memcpy(new_line + prefix_len, insert, out_len);
        memcpy(new_line + prefix_len + out_len, p, suffix_len + 1);
        free(insert);
        free(view->lines[opt->line_idx].text);
        view->lines[opt->line_idx].text = new_line;
        return;
      }
    } else {
      p++;
    }
  }
}

static char *apply_method_chain(ValueMap *map, const InsertOption *opt) {
  if (!opt || opt->method_count == 0) return strdup_safe("");
  char key[128];
  snprintf(key, sizeof(key), "%s", opt->methods[0]);
  const char *base = value_map_get(map, key);
  if (!base) base = "";
  char *current = strdup_safe(base);

  for (size_t i = 1; i < opt->method_count; ++i) {
    if (strcmp(opt->methods[i], "round") == 0) {
      double v = atof(current);
      char buf[64];
      snprintf(buf, sizeof(buf), "%.0f", round(v));
      free(current);
      current = strdup_safe(buf);
    } else {
      if (strlen(key) + strlen(opt->methods[i]) + 2 < sizeof(key)) {
        strcat(key, ".");
        strcat(key, opt->methods[i]);
        const char *val = value_map_get(map, key);
        if (val) {
          free(current);
          current = strdup_safe(val);
        }
      }
    }
  }
  return current;
}

static void apply_inserts(Menu *menu, ValueMap *map) {
  for (size_t i = 0; i < menu->insert_count; ++i) {
    InsertOption *opt = &menu->inserts[i];
    char *value = apply_method_chain(map, opt);
    if (value) {
      apply_insert(&menu->view, opt, value);
      free(value);
    }
  }
}

static void insert_view(View *dst, const View *src, int y0, int x0) {
  if (!dst || !src) return;
  for (int y = 0; y < (int)src->line_count; ++y) {
    int dy = y0 + y;
    if (dy < 0 || dy >= (int)dst->line_count) continue;
    Line *dline = &dst->lines[dy];
    Line *sline = (Line *)&src->lines[y];
    for (int x = 0; x < (int)src->max_cols; ++x) {
      int dx = x0 + x;
      if (dx < 0 || dx >= (int)dst->max_cols) continue;
      dline->cells[dx] = sline->cells[x];
    }
  }
}

static void align_art_to_field(const ArtSlot *slot, int art_w, int art_h, int *out_x, int *out_y) {
  int field_y_center = (slot->y0 + slot->y1) / 2;
  int field_x_center = (slot->x0 + slot->x1) / 2;
  int y_half_1 = art_h / 2 - ((art_h % 2 == 1) ? 0 : 1);
  int y_min = field_y_center - y_half_1;
  int x_half_1 = art_w / 2 - ((art_w % 2 == 1) ? 0 : 1);
  int x_min = field_x_center - x_half_1;
  *out_x = x_min;
  *out_y = y_min;
}

static void compose_menu(Menu *menu, ValueMap *main_map, ValueMap **partial_maps, size_t partial_map_count, ArtArg *art_args, size_t art_arg_count) {
  apply_inserts(menu, main_map);
  view_build_cells(&menu->view);

  for (size_t i = 0; i < menu->partial_count; ++i) {
    PartialSlot *slot = &menu->partials[i];
    if (!slot->name) continue;
    char *partial_path = resolve_menu_path(slot->name);
    if (!partial_path) continue;

    Menu partial = {0};
    if (menu_load(partial_path, &partial)) {
      ValueMap *map = main_map;
      if (partial_maps && i < partial_map_count && partial_maps[i]) map = partial_maps[i];
      apply_inserts(&partial, map);
      view_build_cells(&partial.view);
      insert_view(&menu->view, &partial.view, slot->y0, slot->x0);
    }
    free_menu(&partial);
    free(partial_path);
  }

  size_t count = menu->art_count < art_arg_count ? menu->art_count : art_arg_count;
  for (size_t i = 0; i < count; ++i) {
    ArtArg *arg = &art_args[i];
    if (!arg->name || !arg->path) continue;
    char *art_path = resolve_art_path(arg->path);
    if (!art_path) continue;

    ArtFile file = {0};
    if (artfile_load(art_path, &file)) {
      Art *art = artfile_find(&file, arg->name);
      if (!art && strcmp(arg->name, "normal") != 0) {
        art = artfile_find(&file, "normal");
      }
      if (art) {
        view_build_cells(&art->view);
        int x = 0, y = 0;
        align_art_to_field(&menu->arts[i], (int)art->view.max_cols, (int)art->view.line_count, &x, &y);
        insert_view(&menu->view, &art->view, y, x);
      }
    }
    free_art_file(&file);
    free(art_path);
  }
}

static void render_state_free(RenderState *rs) {
  if (!rs) return;
  if (rs->tex) glDeleteTextures(1, &rs->tex);
  rs->tex = 0;
  free(rs->glyphs);
  free(rs->glyph_list);
  rs->glyphs = NULL;
  rs->glyph_list = NULL;
  rs->glyph_count = 0;
  rs->glyph_list_count = 0;
  rs->grid_w = 0;
  rs->grid_h = 0;
}

static bool has_codepoint(const uint32_t *arr, size_t count, uint32_t cp) {
  for (size_t i = 0; i < count; ++i) {
    if (arr[i] == cp) return true;
  }
  return false;
}

static Glyph *find_glyph(Glyph *glyphs, size_t count, uint32_t cp) {
  for (size_t i = 0; i < count; ++i) {
    if (glyphs[i].codepoint == cp) return &glyphs[i];
  }
  return NULL;
}

static bool build_atlas(Menu *menu, TTF_Font *font, int cell_w, int cell_h, RenderState *rs) {
  if (!menu || !font || !rs) return false;
  render_state_free(rs);

  size_t glyph_cap = 128;
  uint32_t *glyph_list = (uint32_t *)malloc(glyph_cap * sizeof(uint32_t));
  size_t glyph_count = 0;

  for (size_t i = 0; i < menu->view.line_count; ++i) {
    Line *line = &menu->view.lines[i];
    for (size_t j = 0; j < line->len_cells; ++j) {
      uint32_t cp = line->cells[j];
      if (!has_codepoint(glyph_list, glyph_count, cp)) {
        if (glyph_count + 1 > glyph_cap) {
          glyph_cap *= 2;
          glyph_list = (uint32_t *)realloc(glyph_list, glyph_cap * sizeof(uint32_t));
        }
        glyph_list[glyph_count++] = cp;
      }
    }
  }

  if (glyph_count == 0) {
    free(glyph_list);
    return false;
  }

  size_t atlas_cols = (size_t)ceil(sqrt((double)glyph_count));
  size_t atlas_rows = (size_t)ceil((double)glyph_count / (double)atlas_cols);
  int atlas_w = (int)(atlas_cols * cell_w);
  int atlas_h = (int)(atlas_rows * cell_h);

  SDL_Surface *atlas = SDL_CreateRGBSurfaceWithFormat(0, atlas_w, atlas_h, 32, SDL_PIXELFORMAT_RGBA32);
  SDL_FillRect(atlas, NULL, SDL_MapRGBA(atlas->format, 0, 0, 0, 0));

  Glyph *glyphs = (Glyph *)calloc(glyph_count, sizeof(Glyph));
  SDL_Color white = {255, 255, 255, 255};

  for (size_t i = 0; i < glyph_count; ++i) {
    uint32_t cp = glyph_list[i];
    char utf8[5];
    utf8_encode(cp, utf8);

    SDL_Surface *g = TTF_RenderUTF8_Blended(font, utf8, white);
    if (!g) continue;

    int gx = (int)((i % atlas_cols) * cell_w);
    int gy = (int)((i / atlas_cols) * cell_h);

    SDL_Rect dst;
    dst.w = g->w;
    dst.h = g->h;
    dst.x = gx + (cell_w - g->w) / 2;
    dst.y = gy + (cell_h - g->h) / 2;

    SDL_BlitSurface(g, NULL, atlas, &dst);
    SDL_FreeSurface(g);

    glyphs[i].codepoint = cp;
    glyphs[i].u0 = (float)gx / (float)atlas_w;
    glyphs[i].v0 = (float)gy / (float)atlas_h;
    glyphs[i].u1 = (float)(gx + cell_w) / (float)atlas_w;
    glyphs[i].v1 = (float)(gy + cell_h) / (float)atlas_h;
  }

  GLuint tex = 0;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, atlas->w, atlas->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, atlas->pixels);
  SDL_FreeSurface(atlas);

  rs->tex = tex;
  rs->glyphs = glyphs;
  rs->glyph_count = glyph_count;
  rs->glyph_list = glyph_list;
  rs->glyph_list_count = glyph_count;
  rs->grid_w = (int)menu->view.max_cols;
  rs->grid_h = (int)menu->view.line_count;
  return true;
}

static float shade_intensity(uint32_t cp) {
  switch (cp) {
    case 0x2591: return 0.25f; // ░
    case 0x2592: return 0.5f;  // ▒
    case 0x2593: return 0.75f; // ▓
    default: return 1.0f;
  }
}

static void draw_menu(Menu *menu, RenderState *rs, int win_w, int win_h, int cell_w, int cell_h, float alpha, int max_chars) {
  if (!menu || !rs || rs->glyph_count == 0) return;

  float sx = (float)win_w / (float)(rs->grid_w * cell_w);
  float sy = (float)win_h / (float)(rs->grid_h * cell_h);
  float draw_w = cell_w * sx;
  float draw_h = cell_h * sy;

  if (alpha < 0.f) alpha = 0.f;
  if (alpha > 1.f) alpha = 1.f;
  glClearColor(0.f, 0.f, 0.f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT);
  glBindTexture(GL_TEXTURE_2D, rs->tex);

  glBegin(GL_QUADS);
  int total_cells = rs->grid_w * rs->grid_h;
  int limit = max_chars < 0 ? total_cells : max_chars;
  int idx = 0;
  for (int y = 0; y < rs->grid_h; ++y) {
    Line *line = &menu->view.lines[y];
    for (int x = 0; x < rs->grid_w; ++x) {
      if (idx >= limit) {
        y = rs->grid_h;
        break;
      }
      idx++;
      uint32_t cp = (x < (int)line->len_cells) ? line->cells[x] : (uint32_t)' ';
      if (cp == (uint32_t)' ') continue;
      Glyph *g = find_glyph(rs->glyphs, rs->glyph_count, cp);
      if (!g) continue;

      float intensity = shade_intensity(cp);
      glColor4f(intensity, intensity, intensity, alpha);

      float px = x * draw_w;
      float py = y * draw_h;

      glTexCoord2f(g->u0, g->v0); glVertex2f(px, py);
      glTexCoord2f(g->u1, g->v0); glVertex2f(px + draw_w, py);
      glTexCoord2f(g->u1, g->v1); glVertex2f(px + draw_w, py + draw_h);
      glTexCoord2f(g->u0, g->v1); glVertex2f(px, py + draw_h);
    }
  }
  glEnd();
}

static void logbuffer_init(LogBuffer *lb) {
  lb->lines = NULL;
  lb->count = 0;
  lb->cap = 0;
}

static void logbuffer_clear(LogBuffer *lb) {
  if (!lb) return;
  for (size_t i = 0; i < lb->count; ++i) free(lb->lines[i]);
  lb->count = 0;
}

static void logbuffer_free(LogBuffer *lb) {
  if (!lb) return;
  logbuffer_clear(lb);
  free(lb->lines);
  lb->lines = NULL;
  lb->cap = 0;
}

static void logbuffer_push(LogBuffer *lb, const char *text) {
  if (!lb || !text) return;
  if (lb->count + 1 > lb->cap) {
    size_t new_cap = lb->cap == 0 ? 8 : lb->cap * 2;
    char **arr = (char **)realloc(lb->lines, new_cap * sizeof(char *));
    if (!arr) return;
    lb->lines = arr;
    lb->cap = new_cap;
  }
  lb->lines[lb->count++] = strdup_safe(text);
}

static void logbuffer_apply_full(ValueMap *map, const LogBuffer *lb, size_t max_lines) {
  char key[32];
  for (size_t i = 0; i < max_lines; ++i) {
    snprintf(key, sizeof(key), "log_%zu", i);
    if (lb && i < lb->count) value_map_set(map, key, lb->lines[i]);
    else value_map_set(map, key, "");
  }
}

static void logbuffer_apply_last(ValueMap *map, const LogBuffer *lb, size_t last_count) {
  char key[32];
  for (size_t i = 0; i < last_count; ++i) {
    snprintf(key, sizeof(key), "log_last%zu", i + 1);
    if (lb && lb->count > i) {
      value_map_set(map, key, lb->lines[lb->count - 1 - i]);
    } else {
      value_map_set(map, key, "");
    }
  }
}

static const char *default_font_path(void) {
  static const char *candidates[] = {
    "/usr/share/fonts/truetype/dejavu/DejaVuSansMono.ttf",
    "/usr/share/fonts/truetype/liberation/LiberationMono-Regular.ttf",
    "/Library/Fonts/Menlo.ttc",
    "C:/Windows/Fonts/consola.ttf",
  };
  for (size_t i = 0; i < sizeof(candidates) / sizeof(candidates[0]); ++i) {
    if (file_exists(candidates[i])) return candidates[i];
  }
  return NULL;
}

static int character_min_dmg(const Character *c) {
  int weapon_min = c->weapon.min_dmg + c->weapon.enhance_min_dmg;
  int shield_min = c->shield.min_dmg + c->shield.enhance_min_dmg;
  if (weapon_min < 0) weapon_min = 0;
  if (shield_min < 0) shield_min = 0;
  return c->min_dmg_base + weapon_min + shield_min;
}

static int character_max_dmg(const Character *c) {
  int weapon_max = c->weapon.max_dmg + c->weapon.enhance_max_dmg;
  int shield_max = c->shield.max_dmg + c->shield.enhance_max_dmg;
  if (weapon_max < 0) weapon_max = 0;
  if (shield_max < 0) shield_max = 0;
  return c->max_dmg_base + weapon_max + shield_max;
}

static int character_accuracy(const Character *c) {
  return c->accuracy_base + c->weapon.accuracy + c->weapon.enhance_accuracy +
         c->body_armor.accuracy + c->body_armor.enhance_accuracy +
         c->head_armor.accuracy + c->head_armor.enhance_accuracy +
         c->arms_armor.accuracy + c->arms_armor.enhance_accuracy +
         c->shield.accuracy + c->shield.enhance_accuracy;
}

static int character_armor(const Character *c) {
  int body = c->body_armor.armor + c->body_armor.enhance_armor;
  int head = c->head_armor.armor + c->head_armor.enhance_armor;
  int arms = c->arms_armor.armor + c->arms_armor.enhance_armor;
  int shield = c->shield.armor + c->shield.enhance_armor;
  if (body < 0) body = 0;
  if (head < 0) head = 0;
  if (arms < 0) arms = 0;
  if (shield < 0) shield = 0;
  return c->armor_base + body + head + arms + shield;
}

static int character_armor_penetration(const Character *c) {
  int pen = c->weapon.armor_penetration + c->weapon.enhance_armor_penetration;
  if (pen < 0) pen = 0;
  return c->armor_penetration_base + pen;
}

static int character_block_chance(const Character *c) {
  int res = c->block_chance_base + c->weapon.block_chance + c->weapon.enhance_block_chance +
            c->shield.block_chance + c->shield.enhance_block_chance;
  if (strcmp(c->passive_skill.code, "shield_master") == 0 && strcmp(c->shield.code, "without") != 0) {
    res += 10 + 2 * c->passive_skill.lvl;
  }
  return res;
}

static int block_power_in_percents(const Character *c) {
  if (!c) return 0;
  double coeff = 1.0 + (double)c->hp / 200.0;
  if (coeff <= 0.0) return 0;
  int res = 100 - (int)(100.0 / coeff);
  return res < 0 ? 0 : res;
}

static int character_recovery_hp(const Character *c) {
  return (int)round(c->hp_max * 0.1);
}

static int character_recovery_mp(const Character *c) {
  return (int)round(c->mp_max * 0.1);
}

static void skill_init_empty(Skill *s, SkillType type) {
  if (!s) return;
  memset(s, 0, sizeof(*s));
  s->type = type;
  snprintf(s->code, sizeof(s->code), "none");
  snprintf(s->name, sizeof(s->name), "---");
}

static void skill_assign(Skill *s, SkillType type, const char *code) {
  if (!s) return;
  memset(s, 0, sizeof(*s));
  s->type = type;
  snprintf(s->code, sizeof(s->code), "%s", code ? code : "none");
  s->lvl = 0;

  if (strcmp(s->code, "ascetic_strike") == 0) { snprintf(s->name, sizeof(s->name), "Ascetic strike"); s->mp_cost = 2; }
  else if (strcmp(s->code, "precise_strike") == 0) { snprintf(s->name, sizeof(s->name), "Precise strike"); s->mp_cost = 8; }
  else if (strcmp(s->code, "strong_strike") == 0) { snprintf(s->name, sizeof(s->name), "Strong strike"); s->mp_cost = 12; }
  else if (strcmp(s->code, "traumatic_strike") == 0) { snprintf(s->name, sizeof(s->name), "Traumatic strike"); s->mp_cost = 6; }
  else if (strcmp(s->code, "berserk") == 0) { snprintf(s->name, sizeof(s->name), "Berserk"); }
  else if (strcmp(s->code, "concentration") == 0) { snprintf(s->name, sizeof(s->name), "Concentration"); }
  else if (strcmp(s->code, "dazed") == 0) { snprintf(s->name, sizeof(s->name), "Dazed"); }
  else if (strcmp(s->code, "shield_master") == 0) { snprintf(s->name, sizeof(s->name), "Shield master"); }
  else if (strcmp(s->code, "bloody_ritual") == 0) { snprintf(s->name, sizeof(s->name), "Bloody ritual"); s->hp_cost = 10; }
  else if (strcmp(s->code, "first_aid") == 0) { snprintf(s->name, sizeof(s->name), "First aid"); s->mp_cost = 10; }
  else if (strcmp(s->code, "treasure_hunter") == 0) { snprintf(s->name, sizeof(s->name), "Treasure hunter"); }
  else { snprintf(s->name, sizeof(s->name), "---"); }
}

static WeaponItem weapon_from_code(const Game *g, const char *code);
static ArmorItem armor_from_code(const ArmorItem *items, size_t count, const char *code);
static ShieldItem shield_from_code(const Game *g, const char *code);
static const char *pick_random_option(char **list, size_t count);
static const HeroTemplate *hero_template_by_code(const Game *g, const char *code);
static bool save_warehouse_data(const WarehouseData *wh);
static const char *ammo_name(Game *g, const char *type, const char *code);
static void hero_rest(Character *hero, LogBuffer *log);
static bool save_shop_data(const ShopData *shop);
static bool save_monolith_data(const MonolithData *m);
static void titleize_token(const char *in, char *out, size_t out_sz);
static const EnemyTemplate *enemy_template_boss(const DungeonData *d);
static const EnemyTemplate *enemy_template_random_standard(const DungeonData *d, int leveling);
static int enemy_choices_count_for(const Character *hero);

static const HeroTemplate *hero_template_by_code(const Game *g, const char *code) {
  if (!g || !code) return NULL;
  for (size_t i = 0; i < g->hero_count; ++i) {
    if (strcmp(g->heroes[i].code, code) == 0) return &g->heroes[i];
  }
  return NULL;
}

static Character character_from_hero(Game *g, const HeroTemplate *t, const char *name) {
  Character c;
  memset(&c, 0, sizeof(c));
  snprintf(c.code, sizeof(c.code), "%s", t->code);
  snprintf(c.name, sizeof(c.name), "%s", name ? name : t->name);
  snprintf(c.background, sizeof(c.background), "%s", t->code);
  c.hp = t->hp;
  c.hp_max = t->hp;
  c.mp = t->mp;
  c.mp_max = t->mp;
  c.regen_hp_base = 0;
  c.regen_mp_base = 0;
  c.min_dmg_base = t->min_dmg;
  c.max_dmg_base = t->max_dmg;
  c.armor_penetration_base = t->armor_penetration;
  c.accuracy_base = t->accuracy;
  c.armor_base = t->armor;
  c.block_chance_base = 0;
  c.exp = 0;
  c.lvl = 0;
  c.stat_points = 5;
  c.skill_points = t->skill_points;
  c.pzdc_monolith_points = 0;
  c.coins = 0;
  c.exp_gived = 0;
  c.coins_gived = 0;
  c.dungeon_part_number = 1;
  c.leveling = 0;
  c.ingredients.items = NULL;
  c.ingredients.count = 0;
  snprintf(c.ingredient, sizeof(c.ingredient), "without");

  const char *weapon_code = pick_random_option(t->weapon_options, t->weapon_count);
  const char *body_code = pick_random_option(t->body_armor_options, t->body_armor_count);
  const char *head_code = pick_random_option(t->head_armor_options, t->head_armor_count);
  const char *arms_code = pick_random_option(t->arms_armor_options, t->arms_armor_count);
  const char *shield_code = pick_random_option(t->shield_options, t->shield_count);

  c.weapon = weapon_from_code(g, weapon_code);
  c.body_armor = armor_from_code(g->body_armors, g->body_armor_count, body_code);
  c.head_armor = armor_from_code(g->head_armors, g->head_armor_count, head_code);
  c.arms_armor = armor_from_code(g->arms_armors, g->arms_armor_count, arms_code);
  c.shield = shield_from_code(g, shield_code);

  skill_init_empty(&c.active_skill, SKILL_ACTIVE);
  skill_init_empty(&c.passive_skill, SKILL_PASSIVE);
  skill_init_empty(&c.camp_skill, SKILL_CAMP);

  return c;
}

static Character character_from_enemy(Game *g, const EnemyTemplate *t) {
  Character c;
  memset(&c, 0, sizeof(c));
  snprintf(c.code, sizeof(c.code), "%s", t->code_name);
  snprintf(c.name, sizeof(c.name), "%s", t->name);
  c.hp = t->hp;
  c.hp_max = t->hp;
  c.mp = 0;
  c.mp_max = 0;
  c.regen_hp_base = t->regen_hp;
  c.regen_mp_base = 0;
  c.min_dmg_base = t->min_dmg;
  c.max_dmg_base = t->max_dmg;
  c.armor_penetration_base = t->armor_penetration;
  c.accuracy_base = t->accuracy;
  c.armor_base = t->armor;
  c.block_chance_base = 0;
  c.exp = 0;
  c.lvl = 0;
  c.stat_points = 0;
  c.skill_points = 0;
  c.exp_gived = t->exp_gived;
  c.coins_gived = t->coins_gived;
  c.ingredients.items = NULL;
  c.ingredients.count = 0;
  snprintf(c.ingredient, sizeof(c.ingredient), "without");

  const char *weapon_code = pick_random_option(t->weapon_options, t->weapon_count);
  const char *body_code = pick_random_option(t->body_armor_options, t->body_armor_count);
  const char *head_code = pick_random_option(t->head_armor_options, t->head_armor_count);
  const char *arms_code = pick_random_option(t->arms_armor_options, t->arms_armor_count);
  const char *shield_code = pick_random_option(t->shield_options, t->shield_count);
  const char *ingredient_code = pick_random_option(t->ingredient_options, t->ingredient_count);

  c.weapon = weapon_from_code(g, weapon_code);
  c.body_armor = armor_from_code(g->body_armors, g->body_armor_count, body_code);
  c.head_armor = armor_from_code(g->head_armors, g->head_armor_count, head_code);
  c.arms_armor = armor_from_code(g->arms_armors, g->arms_armor_count, arms_code);
  c.shield = shield_from_code(g, shield_code);
  snprintf(c.ingredient, sizeof(c.ingredient), "%s", ingredient_code ? ingredient_code : "without");

  skill_init_empty(&c.active_skill, SKILL_ACTIVE);
  skill_init_empty(&c.passive_skill, SKILL_PASSIVE);
  skill_init_empty(&c.camp_skill, SKILL_CAMP);

  return c;
}

static void hero_add_dmg_base(Character *h, int n) {
  if (!h || n <= 0) return;
  for (int i = 0; i < n; ++i) {
    if (h->min_dmg_base < h->max_dmg_base && rand_range(0, 1) == 0) {
      h->min_dmg_base += 1;
    } else {
      h->max_dmg_base += 1;
    }
  }
}

static void hero_reduce_dmg_base(Character *h, int n) {
  if (!h || n <= 0) return;
  for (int i = 0; i < n; ++i) {
    if (h->max_dmg_base > h->min_dmg_base && rand_range(0, 1) == 0) {
      if (h->max_dmg_base > 0) h->max_dmg_base -= 1;
    } else {
      if (h->min_dmg_base > 0) h->min_dmg_base -= 1;
    }
    if (h->max_dmg_base < h->min_dmg_base) h->max_dmg_base = h->min_dmg_base;
  }
}

static void hero_add_hp(Character *h, int amount) {
  if (!h || amount <= 0) return;
  if (h->hp + amount > h->hp_max) amount = h->hp_max - h->hp;
  if (amount < 0) amount = 0;
  h->hp += amount;
}

static void hero_add_mp(Character *h, int amount) {
  if (!h || amount <= 0) return;
  if (h->mp + amount > h->mp_max) amount = h->mp_max - h->mp;
  if (amount < 0) amount = 0;
  h->mp += amount;
}

static void hero_reduce_mp(Character *h, int amount) {
  if (!h || amount <= 0) return;
  if (amount > h->mp) amount = h->mp;
  h->mp -= amount;
}

static void hero_reduce_coins(Character *h, int amount) {
  if (!h || amount <= 0) return;
  if (amount > h->coins) amount = h->coins;
  h->coins -= amount;
}

static void apply_monolith_bonuses(const MonolithData *m, Character *h) {
  if (!m || !h) return;
  h->hp_max += m->hp;
  h->hp += m->hp;
  h->mp_max += m->mp;
  h->mp += m->mp;
  h->accuracy_base += m->accuracy;
  hero_add_dmg_base(h, m->damage);
  h->stat_points += m->stat_points;
  h->skill_points += m->skill_points;
  h->armor_base += m->armor;
  h->armor_penetration_base += m->armor_penetration;
  h->regen_hp_base += m->regen_hp;
  h->regen_mp_base += m->regen_mp;
  h->block_chance_base += m->block_chance;
}

static void apply_statistics_bonuses(const StatisticsTotal *s, Game *g, Character *h) {
  if (!s || !g || !h) return;
  if (s->bandits[0] >= 30) h->weapon = weapon_from_code(g, "stick");
  if (s->bandits[1] >= 30) { h->hp_max += 2; h->hp += 2; }
  if (s->bandits[2] >= 30) h->accuracy_base += 1;
  if (s->bandits[3] >= 30) { h->hp_max += 5; h->hp += 5; }
  if (s->bandits[4] >= 30) h->stat_points += 1;
  if (s->bandits[5] >= 5) h->skill_points += 1;

  if (s->undeads[0] >= 30) h->arms_armor = armor_from_code(g->arms_armors, g->arms_armor_count, "worn_gloves");
  if (s->undeads[1] >= 30) { h->mp_max += 3; h->mp += 3; }
  if (s->undeads[2] >= 30) h->accuracy_base += 1;
  if (s->undeads[3] >= 30) { h->hp_max += 7; h->hp += 7; }
  if (s->undeads[4] >= 30) h->block_chance_base += 3;
  if (s->undeads[5] >= 5) h->regen_mp_base += 1;

  if (s->swamp[0] >= 30) { h->mp_max += 3; h->mp += 3; }
  if (s->swamp[1] >= 30) h->shield = shield_from_code(g, "holey_wicker_buckler");
  if (s->swamp[2] >= 30) { h->hp_max += 3; h->hp += 3; }
  if (s->swamp[3] >= 30) h->accuracy_base += 1;
  if (s->swamp[4] >= 30) h->max_dmg_base += 1;
  if (s->swamp[5] >= 5) h->armor_base += 1;
}

static void apply_warehouse_bonuses(Game *g, Character *h) {
  if (!g || !h) return;
  bool changed = false;
  if (strcmp(g->warehouse.weapon, "without") != 0) {
    h->weapon = weapon_from_code(g, g->warehouse.weapon);
    snprintf(g->warehouse.weapon, sizeof(g->warehouse.weapon), "without");
    changed = true;
  }
  if (strcmp(g->warehouse.body_armor, "without") != 0) {
    h->body_armor = armor_from_code(g->body_armors, g->body_armor_count, g->warehouse.body_armor);
    snprintf(g->warehouse.body_armor, sizeof(g->warehouse.body_armor), "without");
    changed = true;
  }
  if (strcmp(g->warehouse.head_armor, "without") != 0) {
    h->head_armor = armor_from_code(g->head_armors, g->head_armor_count, g->warehouse.head_armor);
    snprintf(g->warehouse.head_armor, sizeof(g->warehouse.head_armor), "without");
    changed = true;
  }
  if (strcmp(g->warehouse.arms_armor, "without") != 0) {
    h->arms_armor = armor_from_code(g->arms_armors, g->arms_armor_count, g->warehouse.arms_armor);
    snprintf(g->warehouse.arms_armor, sizeof(g->warehouse.arms_armor), "without");
    changed = true;
  }
  if (strcmp(g->warehouse.shield, "without") != 0) {
    h->shield = shield_from_code(g, g->warehouse.shield);
    snprintf(g->warehouse.shield, sizeof(g->warehouse.shield), "without");
    changed = true;
  }
  if (changed) save_warehouse_data(&g->warehouse);
}

static void shop_add_from_hero(Game *g, const Character *h) {
  if (!g || !h) return;
  const int sell_chance = 3;
  for (int t = 0; t < 5; ++t) {
    const char *code = "without";
    if (t == 0) code = h->weapon.code;
    else if (t == 1) code = h->body_armor.code;
    else if (t == 2) code = h->head_armor.code;
    else if (t == 3) code = h->arms_armor.code;
    else code = h->shield.code;
    if (strcmp(code, "without") == 0) continue;
    if (rand_range(0, sell_chance - 1) != 0) continue;
    char (*arr)[32] = NULL;
    if (t == 0) arr = g->shop.weapon;
    else if (t == 1) arr = g->shop.body_armor;
    else if (t == 2) arr = g->shop.head_armor;
    else if (t == 3) arr = g->shop.arms_armor;
    else arr = g->shop.shield;
    int slot = -1;
    for (int i = 0; i < 3; ++i) {
      if (strcmp(arr[i], "without") == 0) { slot = i; break; }
    }
    if (slot < 0) slot = rand_range(0, 2);
    snprintf(arr[slot], sizeof(arr[slot]), "%s", code);
  }
  save_shop_data(&g->shop);
}

static void delete_hero_in_run_file(void) {
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return;
  char path[512];
  snprintf(path, sizeof(path), "%s/hero_in_run.yml", saves_dir);
  free(saves_dir);
  if (file_exists(path)) remove(path);
}

static void end_run_transfer(Game *g, bool hero_alive) {
  if (!g) return;
  if (strcmp(g->hero.name, "Cheater") != 0) {
    g->monolith.points += g->hero.pzdc_monolith_points;
    g->hero.pzdc_monolith_points = 0;
    save_monolith_data(&g->monolith);
    if (hero_alive) {
      shop_add_from_hero(g, &g->hero);
      g->warehouse.coins += g->hero.coins;
      g->hero.coins = 0;
      save_warehouse_data(&g->warehouse);
    }
  }
  delete_hero_in_run_file();
}

static int treasure_hunter_coeff(const Skill *camp_skill) {
  if (!camp_skill || strcmp(camp_skill->code, "treasure_hunter") != 0) return 0;
  return 50 + 10 * camp_skill->lvl;
}

static bool loot_should_drop(Game *g) {
  if (!g) return false;
  if (strcmp(g->hero.camp_skill.code, "treasure_hunter") == 0) {
    int coeff = treasure_hunter_coeff(&g->hero.camp_skill);
    return rand_range(0, 1) == 1 || rand_range(0, 150) < coeff;
  }
  return rand_range(0, 1) == 1;
}

static void loot_reset(Game *g) {
  if (!g) return;
  g->loot_count = 0;
  g->loot_index = 0;
  g->loot_show_coins = 0;
  g->loot_show_ingredient = 0;
  g->loot_message_mode = 0;
  g->loot_coins = 0;
  g->loot_ingredient[0] = '\0';
  g->loot_message[0] = '\0';
  g->pending_levelup = 0;
  g->loot_return_pending = 0;
  g->loot_return_state = STATE_CAMPFIRE;
  g->loot_last_taken = -1;
}

static void loot_add(Game *g, const char *type, const char *code) {
  if (!g || !type || !code) return;
  if (g->loot_count >= (int)(sizeof(g->loot_items) / sizeof(g->loot_items[0]))) return;
  snprintf(g->loot_items[g->loot_count].type, sizeof(g->loot_items[g->loot_count].type), "%s", type);
  snprintf(g->loot_items[g->loot_count].code, sizeof(g->loot_items[g->loot_count].code), "%s", code);
  g->loot_count += 1;
}

static void loot_setup(Game *g) {
  if (!g) return;
  loot_reset(g);
  if (loot_should_drop(g) && strcmp(g->enemy.weapon.code, "without") != 0) loot_add(g, "weapon", g->enemy.weapon.code);
  if (loot_should_drop(g) && strcmp(g->enemy.body_armor.code, "without") != 0) loot_add(g, "body_armor", g->enemy.body_armor.code);
  if (loot_should_drop(g) && strcmp(g->enemy.head_armor.code, "without") != 0) loot_add(g, "head_armor", g->enemy.head_armor.code);
  if (loot_should_drop(g) && strcmp(g->enemy.arms_armor.code, "without") != 0) loot_add(g, "arms_armor", g->enemy.arms_armor.code);
  if (loot_should_drop(g) && strcmp(g->enemy.shield.code, "without") != 0) loot_add(g, "shield", g->enemy.shield.code);
  if (g->enemy.coins_gived > 0) {
    g->loot_show_coins = 1;
    g->loot_coins = g->enemy.coins_gived;
  }
  if (strcmp(g->enemy.ingredient, "without") != 0) {
    g->loot_show_ingredient = 1;
    snprintf(g->loot_ingredient, sizeof(g->loot_ingredient), "%s", g->enemy.ingredient);
  }
}

static void loot_advance(Game *g) {
  if (!g) return;
  if (g->loot_index < g->loot_count) {
    const LootEntry *le = &g->loot_items[g->loot_index];
    char item_name[128];
    const char *name = ammo_name(g, le->type, le->code);
    snprintf(item_name, sizeof(item_name), "%s", name);
    snprintf(g->loot_message, sizeof(g->loot_message),
             "After searching the %s's body you found %s", g->enemy.name, item_name);
    g->state = STATE_LOOT;
    return;
  }
  if (g->loot_show_coins) {
    g->loot_message_mode = 1;
    g->state = STATE_LOOT_MESSAGE;
    return;
  }
  if (g->loot_show_ingredient) {
    g->loot_message_mode = 2;
    g->state = STATE_LOOT_MESSAGE;
    return;
  }
  if (g->loot_return_pending) {
    g->loot_return_pending = 0;
    g->state = g->loot_return_state;
    if (g->state == STATE_EVENT_RESULT) {
      event_after_loot(g);
    }
    return;
  }
  if (!g->enemy_is_boss) {
    g->hero.leveling += 1;
  }
  g->hero.dungeon_part_number += 1;
  logbuffer_clear(&g->log);
  hero_rest(&g->hero, &g->log);
  g->state = STATE_CAMPFIRE;
}

static const EventDef kEvents[] = {
  {
    "loot_field",
    "Some scrub",
    {
      "In this pile of scrub...",
      "...you might find some",
      "",
      "",
      ""
    },
    "events/_loot_field",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "loot_secret",
    "Secret something",
    {
      "There might be something...",
      "...unusual here",
      "",
      ""
    },
    "events/_loot_secret",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "gambler",
    "Gambler",
    {
      "Little man...",
      "...he juggling dice...",
      "...easy way to get rich",
      ""
    },
    "events/_gambler",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "altar_of_blood",
    "Altar of Blood",
    {
      "Old Altar...",
      "...its take your blood...",
      "...and give you some",
      "",
      ""
    },
    "events/_altar_of_blood",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "boatman_eugene",
    "Who are you and who am i...",
    {
      "Boatman will take you...",
      "...will ask for something...",
      "...path will be shorter?",
      "",
      ""
    },
    "events/_boatman_eugene",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "briedge_keeper",
    "Bridge of death",
    {
      "Bridge keeper...",
      "...will ask questions...",
      "...answer correctly...",
      "...and otherwise",
      ""
    },
    "events/_briedge_keeper",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "wariors_grave",
    "Warior's Grave",
    {
      "Old grave...",
      "...warrior is buried here...",
      "...maybe with ammunition?",
      "",
      ""
    },
    "events/_warriors_grave",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "pig_with_saucepan",
    "Pig with saucepan",
    {
      "Pigman is eating something...",
      "...smelly in his saucepan...",
      "...the saucepan looks like...",
      "...on a shiny helmet",
      ""
    },
    "events/_pig_with_saucepan",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "black_mage",
    "Black mage",
    {
      "Casts spells...",
      "...for your coins",
      "",
      "",
      ""
    },
    "events/_black_mage",
    EVENT_EFFECT_NONE,
    0,
    ""
  },
  {
    "exit_run",
    "Exit from dugeon",
    {
      "Looks like an exit...",
      "...you can save life...",
      "...you can save coins...",
      "...but be careful...",
      "...you might fall"
    },
    "events/_exit_run",
    EVENT_EFFECT_NONE,
    0,
    ""
  }
};

static void event_to_map(const EventDef *ev, ValueMap *map) {
  if (!ev || !map) return;
  value_map_clear(map);
  value_map_set(map, "name", ev->name);
  value_map_set(map, "description1", ev->desc[0]);
  value_map_set(map, "description2", ev->desc[1]);
  value_map_set(map, "description3", ev->desc[2]);
  value_map_set(map, "description4", ev->desc[3]);
  value_map_set(map, "description5", ev->desc[4]);
}

static void pick_random_events(Game *g) {
  if (!g) return;
  int random = rand_range(1, 200);
  int th = treasure_hunter_coeff(&g->hero.camp_skill);
  int res = random + th;
  g->event_choice_count = res > 150 ? 3 : res > 80 ? 2 : 1;
  int total = (int)(sizeof(kEvents) / sizeof(kEvents[0]));
  int used[16] = {0};
  for (int i = 0; i < g->event_choice_count; ++i) {
    int idx = rand_range(0, total - 1);
    int guard = 0;
    while (used[idx] && guard < 20) {
      idx = rand_range(0, total - 1);
      guard++;
    }
    used[idx] = 1;
    g->event_choices[i] = kEvents[idx];
  }
  if (th > 0) {
    snprintf(g->event_choose_message, sizeof(g->event_choose_message),
             "Random is %d + treasure hunter %d = you find %d ways. Which way will you go?",
             random, th, g->event_choice_count);
  } else {
    snprintf(g->event_choose_message, sizeof(g->event_choose_message),
             "Random is %d = you find %d ways. Which way will you go?",
             random, g->event_choice_count);
  }
}

static void event_clear_input(Game *g) {
  if (!g) return;
  g->event_text_len = 0;
  g->event_text[0] = '\0';
}

static void event_set_input(Game *g, EventInputMode mode) {
  if (!g) return;
  g->event_input_mode = mode;
  if (mode != EVENT_INPUT_TEXT) event_clear_input(g);
}

static void event_set_main(Game *g, const char *text) {
  if (!g || !text) return;
  snprintf(g->event_message, sizeof(g->event_message), "%s", text);
}

static void event_set_art(Game *g, const char *name) {
  if (!g || !name) return;
  snprintf(g->event_art_name, sizeof(g->event_art_name), "%s", name);
}

static void event_finish(Game *g) {
  if (!g) return;
  g->event_pending_action = EVENT_PENDING_NONE;
  g->hero.dungeon_part_number += 1;
  logbuffer_clear(&g->log);
  hero_rest(&g->hero, &g->log);
  g->state = STATE_CAMPFIRE;
}

static void event_end_run(Game *g, const char *title, const char *log_line, bool alive) {
  if (!g) return;
  logbuffer_clear(&g->log);
  if (log_line && log_line[0]) logbuffer_push(&g->log, log_line);
  end_run_transfer(g, alive);
  snprintf(g->message_title, sizeof(g->message_title), "%s", title ? title : "Run ended");
  if (!alive) {
    snprintf(g->message_art_name, sizeof(g->message_art_name), "game_over");
    snprintf(g->message_art_path, sizeof(g->message_art_path), "_game_over");
  } else {
    g->message_art_name[0] = '\0';
    g->message_art_path[0] = '\0';
  }
  g->next_state = STATE_START;
  g->state = STATE_MESSAGE;
}

static void event_offer_loot(Game *g, const char *type, const char *code, const char *message, EventPendingAction pending) {
  if (!g || !type || !code) return;
  loot_reset(g);
  g->event_pending_action = pending;
  g->loot_last_taken = -1;
  g->loot_return_pending = 1;
  g->loot_return_state = STATE_EVENT_RESULT;
  snprintf(g->loot_message, sizeof(g->loot_message), "%s", message ? message : "Loot found");
  loot_add(g, type, code);
  g->state = STATE_LOOT;
}

static void event_after_loot(Game *g) {
  if (!g) return;
  if (g->event_pending_action == EVENT_PENDING_GRAVE_DIG) {
    int taken = g->loot_last_taken == 1;
    int mp = taken ? rand_range(20, 100) : rand_range(5, 20);
    hero_reduce_mp(&g->hero, mp);
    logbuffer_clear(&g->log);
    if (taken) {
      char msg[128];
      snprintf(msg, sizeof(msg), "The warrior's spirit is furious, he took %d MP from you", mp);
      logbuffer_push(&g->log, msg);
    } else {
      char msg[128];
      snprintf(msg, sizeof(msg), "The warrior spirit is not happy, he took %d MP from you", mp);
      logbuffer_push(&g->log, msg);
    }
    event_set_main(g, "Leave [Enter 0]");
    event_set_art(g, "diged");
    event_set_input(g, EVENT_INPUT_DIGIT);
    g->event_step = 31;
  } else if (g->event_pending_action == EVENT_PENDING_GRAVE_REWARD) {
    logbuffer_clear(&g->log);
    logbuffer_push(&g->log, "\"Good luck, brother. With people like you, we will cleanse these lands.\"");
    event_set_main(g, "Leave [Enter 0]");
    event_set_art(g, "clean");
    event_set_input(g, EVENT_INPUT_DIGIT);
    g->event_step = 41;
  } else if (g->event_pending_action == EVENT_PENDING_PIG_SALLET) {
    g->event_pending_action = EVENT_PENDING_NONE;
    event_finish(g);
    return;
  }
  g->event_pending_action = EVENT_PENDING_NONE;
}

static void event_enter_step(Game *g) {
  if (!g) return;
  logbuffer_clear(&g->log);
  event_set_art(g, "normal");

  if (strcmp(g->event_code, "loot_field") == 0) {
    int base = rand_range(1, 200);
    int th = treasure_hunter_coeff(&g->hero.camp_skill);
    int chance = base + th;
    g->event_data[0] = base;
    g->event_data[1] = chance;
    event_set_main(g, "To continue press Enter");
    logbuffer_push(&g->log, "Search everything around...");
    if (th > 0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "Random luck is %d + treasure hunter %d = %d...", base, th, chance);
      logbuffer_push(&g->log, msg);
    } else {
      char msg[128];
      snprintf(msg, sizeof(msg), "Random luck is %d...", chance);
      logbuffer_push(&g->log, msg);
    }
    if (chance > 130) {
      logbuffer_push(&g->log, "...more then 130");
      hero_add_hp(&g->hero, 20);
      char msg[128];
      snprintf(msg, sizeof(msg), "Found a potion that restores 20 HP, now you have %d/%d HP", g->hero.hp, g->hero.hp_max);
      logbuffer_push(&g->log, msg);
    } else if (chance > 70) {
      logbuffer_push(&g->log, "...lower then 130");
      logbuffer_push(&g->log, "There is nothing valuable");
    } else {
      logbuffer_push(&g->log, "...lower then 70");
      g->hero.hp -= 5;
      event_set_main(g, "You died from a rat bite. A miserable death. To continue press Enter");
      char msg[160];
      snprintf(msg, sizeof(msg), "While you were rummaging around the corners, you were bitten by a rat (-5 HP), now you have %d/%d HP", g->hero.hp, g->hero.hp_max);
      logbuffer_push(&g->log, msg);
    }
    event_set_input(g, EVENT_INPUT_NONE);
  } else if (strcmp(g->event_code, "loot_secret") == 0) {
    int base = rand_range(1, 200);
    int th = treasure_hunter_coeff(&g->hero.camp_skill);
    int chance = base + th;
    event_set_main(g, "To continue press Enter");
    logbuffer_push(&g->log, "Looking around, you noticed the magician's hiding place, and in it... ");
    if (th > 0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "Random luck is %d + treasure hunter %d = %d...", base, th, chance);
      logbuffer_push(&g->log, msg);
    } else {
      char msg[128];
      snprintf(msg, sizeof(msg), "Random luck is %d...", chance);
      logbuffer_push(&g->log, msg);
    }
    if (chance >= 130) {
      logbuffer_push(&g->log, "...more then 130");
      int stash = rand_range(1, 32);
      if (stash <= 10) {
        int bonus = rand_range(1, 3);
        char msg[128];
        snprintf(msg, sizeof(msg), "Elixir of Health. Your HP %d/%d increase by %d", g->hero.hp, g->hero.hp_max, bonus);
        logbuffer_push(&g->log, msg);
        g->hero.hp_max += bonus;
        g->hero.hp += bonus;
        snprintf(msg, sizeof(msg), "Now you have %d/%d HP", g->hero.hp, g->hero.hp_max);
        logbuffer_push(&g->log, msg);
      } else if (stash <= 20) {
        int bonus = rand_range(1, 3);
        char msg[128];
        snprintf(msg, sizeof(msg), "Elixir of Endurance. Your MP %d/%d increase by %d", g->hero.mp, g->hero.mp_max, bonus);
        logbuffer_push(&g->log, msg);
        g->hero.mp_max += bonus;
        g->hero.mp += bonus;
        snprintf(msg, sizeof(msg), "Now you have %d/%d MP", g->hero.mp, g->hero.mp_max);
        logbuffer_push(&g->log, msg);
      } else if (stash <= 25) {
        int bonus = rand_range(1, 2);
        char msg[128];
        snprintf(msg, sizeof(msg), "Elixir of Precision. Your accuracy %d increase by %d", g->hero.accuracy_base, bonus);
        logbuffer_push(&g->log, msg);
        g->hero.accuracy_base += bonus;
        snprintf(msg, sizeof(msg), "Now you have %d accuracy", g->hero.accuracy_base);
        logbuffer_push(&g->log, msg);
      } else if (stash <= 27) {
        g->hero.stat_points += 1;
        logbuffer_push(&g->log, "Book of Knowledge. Your stat points increase by 1");
      } else if (stash <= 29) {
        g->hero.skill_points += 1;
        logbuffer_push(&g->log, "Book of Skills. Your skill points increase by 1");
      } else if (stash == 30) {
        g->hero.armor_base += 1;
        logbuffer_push(&g->log, "Elixir of Stone. Your armor increase by 1");
        char msg[64];
        snprintf(msg, sizeof(msg), "Now you have %d armor", g->hero.armor_base);
        logbuffer_push(&g->log, msg);
      } else if (stash == 31) {
        g->hero.regen_hp_base += 1;
        logbuffer_push(&g->log, "Elixir of the Troll. HP regeneration increase by 1");
        char msg[64];
        snprintf(msg, sizeof(msg), "Now you have %d HP regeneration", g->hero.regen_hp_base);
        logbuffer_push(&g->log, msg);
      } else {
        g->hero.regen_mp_base += 1;
        logbuffer_push(&g->log, "Unicorn Elixir. MP regeneration increase by 1");
        char msg[64];
        snprintf(msg, sizeof(msg), "Now you have %d MP regeneration", g->hero.regen_mp_base);
        logbuffer_push(&g->log, msg);
      }
    } else {
      logbuffer_push(&g->log, "...lower then 130");
      logbuffer_push(&g->log, "There is nothing valuable");
    }
    event_set_input(g, EVENT_INPUT_NONE);
  } else if (strcmp(g->event_code, "gambler") == 0) {
    event_set_art(g, "normal");
    if (g->event_step == 0) {
      if (g->hero.coins > 0) event_set_main(g, "Play [Enter 1]    Catch and rob [Enter 2]    Leave [Enter 0]");
      else event_set_main(g, "You cant play without coins    Catch and rob [Enter 2]    Leave [Enter 0]");
      logbuffer_push(&g->log, "You see a little man juggling dice");
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else if (g->event_step == 1) {
      if (g->hero.coins == 0) {
        event_set_main(g, "Catch and rob [Enter 2]       Leave [Enter 0]");
        logbuffer_push(&g->log, "You have 0 coin, and cant play more");
      } else {
        char main[128];
        snprintf(main, sizeof(main), "Your coins: %d   Roll the dice [Enter 1]    Catch and rob [Enter 2]    Leave [Enter 0]", g->hero.coins);
        event_set_main(g, main);
        logbuffer_push(&g->log, "Lets play?!");
      }
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else if (g->event_step == 2) {
      event_set_input(g, EVENT_INPUT_NONE);
    }
  } else if (strcmp(g->event_code, "altar_of_blood") == 0) {
    int adept = strcmp(g->hero.camp_skill.code, "bloody_ritual") == 0;
    int hp_taken = adept ? (g->hero.camp_skill.lvl > 5 ? 10 : 20) : 30;
    if (g->hero.hp <= hp_taken) {
      char main[128];
      snprintf(main, sizeof(main), "You have only %d HP, but need %d HP. Press Enter to exit", g->hero.hp, hp_taken);
      event_set_main(g, main);
      logbuffer_push(&g->log, "The altar doesn't speak to you, maybe you don't have enough blood");
      event_set_input(g, EVENT_INPUT_NONE);
    } else if (adept) {
      event_set_art(g, "adept");
      if (g->hero.camp_skill.lvl > 5) {
        event_set_main(g, "+5 max-HP [Enter 1]    +5 max-MP [Enter 2]    +1 Accuracy [Enter 3]    +1 Damage [Enter 4]    Exit [Enter 0]");
        logbuffer_push(&g->log, "I see you are my prophet. Shed blood (-10 HP) and receive great gifts");
      } else {
        event_set_main(g, "+5 max-HP [Enter 1]     +5 max-MP [Enter 2]     +1 Accuracy [Enter 3]     Exit [Enter 0]");
        logbuffer_push(&g->log, "I see you are my disciple. Spill blood (-20 HP) and receive gifts");
      }
      logbuffer_push(&g->log, "This is the altar of your bloody god, he recognized his own and began to vibrate");
      logbuffer_push(&g->log, "An inscription in blood appeared on the altar:");
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else {
      event_set_main(g, "Random Gift [Enter 1]                   Exit [Enter 0]");
      logbuffer_push(&g->log, "This is the altar of bloody god");
      logbuffer_push(&g->log, "An inscription in blood appeared on the altar:");
      logbuffer_push(&g->log, "Spill blood (-30 HP) and receive gifts");
      event_set_input(g, EVENT_INPUT_DIGIT);
    }
  } else if (strcmp(g->event_code, "boatman_eugene") == 0) {
    if (g->event_step == 0) {
      event_set_main(g, "Agree to teach Eugene [Enter 1]            Go away [Enter 0]");
      logbuffer_push(&g->log, "You are greeted by the boatman Evgeny. \"I'll take you the short way\" - he offers");
      logbuffer_push(&g->log, "\"You are such a skilled and strong warrior, help me become like that too\" - Evgeniy asks you");
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else {
      event_set_input(g, EVENT_INPUT_NONE);
    }
  } else if (strcmp(g->event_code, "briedge_keeper") == 0) {
    if (g->event_step == 0) {
      event_set_main(g, "You see a stern old man, this is the keeper of the bridge, he asks questions");
      logbuffer_push(&g->log, "First question: How old are you?");
      event_set_input(g, EVENT_INPUT_TEXT);
    } else if (g->event_step == 1) {
      event_set_main(g, "Answer the question and press Enter");
      logbuffer_push(&g->log, "Second question: Who is the greatest champion Gachimuchi?");
      event_set_input(g, EVENT_INPUT_TEXT);
    } else if (g->event_step == 2) {
      event_set_main(g, "Press Enter to cross the bridge");
      event_set_input(g, EVENT_INPUT_NONE);
    } else if (g->event_step == 3) {
      event_set_main(g, "To continue press Enter");
      event_set_input(g, EVENT_INPUT_NONE);
    } else if (g->event_step == 4) {
      event_set_main(g, "Press Enter to end the game");
      event_set_input(g, EVENT_INPUT_NONE);
    } else if (g->event_step == 5) {
      event_set_main(g, "The bridge keeper shows your prize");
      event_set_art(g, "action");
      event_set_input(g, EVENT_INPUT_NONE);
      logbuffer_push(&g->log, "What you saw blinded you a little, but made you harder. Accuracy -1. Armor penetration +1");
    }
  } else if (strcmp(g->event_code, "exit_run") == 0) {
    if (g->event_step == 0) {
      event_set_main(g, "Climb the stairs [Enter 1]                Leave [Enter 0]");
      logbuffer_push(&g->log, "You see an old staircase leading up, it looks like it's the exit from the dungeon...");
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else {
      event_set_input(g, EVENT_INPUT_NONE);
    }
  } else if (strcmp(g->event_code, "black_mage") == 0) {
    int adept = strcmp(g->hero.camp_skill.code, "bloody_ritual") == 0;
    int price = adept ? (g->hero.camp_skill.lvl > 5 ? 0 : 1) : 2;
    if (g->event_step == 0) {
      char main[128];
      snprintf(main, sizeof(main), "You have %d coins.    Buy spell [Enter 1]    Attack mage [Enter 2]    Leave [Enter 0]", g->hero.coins);
      event_set_main(g, main);
      if (adept) {
        logbuffer_push(&g->log, "Hello brother, I see you also hear our Bloody God");
        logbuffer_push(&g->log, "I give my brothers a discount and reduce the risks of negative effects");
      }
      char msg[128];
      snprintf(msg, sizeof(msg), "Black mage offers to cast an experimental spell on you for %d coins", price);
      logbuffer_push(&g->log, msg);
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else if (g->event_step == 1) {
      char main[128];
      snprintf(main, sizeof(main), "You have %d coins.      Attack mage [Enter 1]      Leave [Enter 0]", g->hero.coins);
      event_set_main(g, main);
      event_set_art(g, "action");
      event_set_input(g, EVENT_INPUT_DIGIT);
    }
  } else if (strcmp(g->event_code, "pig_with_saucepan") == 0) {
    if (g->event_step == 0) {
      event_set_main(g, "Offer the pigman an acorn for his helmet [Enter 1]    Rob a pigman [Enter 2]    Leave [Enter 0]");
      logbuffer_push(&g->log, "Looking closely you noticed that it was a new and shiny Sallet helmet, it would be nice to get it");
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else if (g->event_step == 1) {
      event_set_art(g, "buy");
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else if (g->event_step == 3) {
      event_set_input(g, EVENT_INPUT_NONE);
    }
  } else if (strcmp(g->event_code, "wariors_grave") == 0) {
    if (g->event_step == 0) {
      if (g->wg_taken) {
        hero_add_hp(&g->hero, 5);
        hero_add_mp(&g->hero, 5);
        logbuffer_push(&g->log, "Warrior's spirit restored you 5 HP and 5 MP");
        char enemy_name[64];
        titleize_token(g->wg_enemy, enemy_name, sizeof(enemy_name));
        int stats_count = stats_total_get(&g->stats_total, g->hero.dungeon_name, g->wg_enemy);
        if (stats_count >= g->wg_count) {
          g->wg_taken = 0;
          char msg[160];
          snprintf(msg, sizeof(msg), "\"You did a great job %d %ss is killed, here is your reward\"", g->wg_count, enemy_name);
          const char *reward_code = "sword";
          if (g->wg_level == 1) {
            reward_code = (rand_range(0, 4) < 4) ? "sword" : "hatchet";
          } else {
            const char *pool[] = {"falchion", "pernach", "axe", "flail"};
            reward_code = pool[rand_range(0, 3)];
          }
          event_offer_loot(g, "weapon", reward_code, msg, EVENT_PENDING_GRAVE_REWARD);
          return;
        } else {
          int count_left = g->wg_count - stats_count;
          char msg[160];
          snprintf(msg, sizeof(msg), "\"Keep up the good work you still have to kill %d %ss\"", count_left, enemy_name);
          logbuffer_push(&g->log, msg);
        }
        event_set_main(g, "Leave [Enter 0]");
        event_set_art(g, "clean");
        event_set_input(g, EVENT_INPUT_DIGIT);
      } else {
        event_set_main(g, "Dig up the grave [Enter 1]    Clean the grave from dirt [Enter 2]    Leave [Enter 0]");
        logbuffer_push(&g->log, "You see an old grave, judging by the inscription a warrior is buried there.");
        event_set_input(g, EVENT_INPUT_DIGIT);
      }
    } else if (g->event_step == 1) {
      event_set_input(g, EVENT_INPUT_DIGIT);
    } else if (g->event_step == 2) {
      event_set_art(g, "clean");
      event_set_input(g, EVENT_INPUT_DIGIT);
    }
  }
}

static void event_begin(Game *g, const EventDef *ev) {
  if (!g || !ev) return;
  snprintf(g->event_code, sizeof(g->event_code), "%s", ev->code);
  snprintf(g->event_art_path, sizeof(g->event_art_path), "%s", ev->art_path);
  snprintf(g->event_art_name, sizeof(g->event_art_name), "normal");
  g->event_step = 0;
  g->event_pending_action = EVENT_PENDING_NONE;
  event_clear_input(g);
  event_enter_step(g);
  g->state = STATE_EVENT_RESULT;
}

static void event_handle_text(Game *g, const char *text) {
  if (!g || !text) return;
  if (strcmp(g->event_code, "briedge_keeper") == 0) {
    if (g->event_step == 0) {
      int age = atoi(text);
      if (age >= 18) {
        char msg[128];
        snprintf(msg, sizeof(msg), "First question: How old are you? Your answer %d is correct", age);
        logbuffer_clear(&g->log);
        logbuffer_push(&g->log, msg);
        g->event_step = 1;
        event_clear_input(g);
        event_enter_step(g);
      } else {
        logbuffer_clear(&g->log);
        logbuffer_push(&g->log, "First question: How old are you?");
        logbuffer_push(&g->log, "Get out of here, you're not old enough yet.");
        g->event_step = 3;
        event_set_main(g, "To continue press Enter");
        event_set_input(g, EVENT_INPUT_NONE);
      }
    } else if (g->event_step == 1) {
      char lower[128];
      snprintf(lower, sizeof(lower), "%s", text);
      for (size_t i = 0; lower[i]; ++i) lower[i] = (char)tolower((unsigned char)lower[i]);
      bool ok = (strstr(lower, "herrington") != NULL) && (strstr(lower, "billy") != NULL || strstr(lower, "william") != NULL);
      if (ok) {
        char msg1[160];
        snprintf(msg1, sizeof(msg1), "Second question: Who is the greatest champion Gachimuchi?");
        char msg2[160];
        snprintf(msg2, sizeof(msg2), "Your answer: %s is correct", text);
        logbuffer_clear(&g->log);
        logbuffer_push(&g->log, msg1);
        logbuffer_push(&g->log, msg2);
        char msg3[160];
        snprintf(msg3, sizeof(msg3), "Сome with me across the bridge %s i'll show you something", g->hero.name);
        logbuffer_push(&g->log, msg3);
        g->event_step = 2;
        event_set_main(g, "Press Enter to cross the bridge");
        event_set_input(g, EVENT_INPUT_NONE);
      } else {
        logbuffer_clear(&g->log);
        char msg1[160];
        snprintf(msg1, sizeof(msg1), "Your answer: %s is incorrect. You shall not pass!!", text);
        logbuffer_push(&g->log, msg1);
        logbuffer_push(&g->log, "The bridge keeper uses magic to throw you into the gorge.");
        char msg2[160];
        snprintf(msg2, sizeof(msg2), "%s say AAAAAAAAAAAAAAAAAAAAAAAA!!!", g->hero.name);
        logbuffer_push(&g->log, msg2);
        int loss = (int)round(g->hero.hp_max * 0.2);
        g->hero.hp -= loss;
        char msg3[160];
        snprintf(msg3, sizeof(msg3), "%s fell and lost %d HP. %d/%d HP left", g->hero.name, loss, g->hero.hp, g->hero.hp_max);
        logbuffer_push(&g->log, msg3);
        if (g->hero.hp <= 0) {
          logbuffer_push(&g->log, "You died");
          g->event_step = 4;
          event_set_main(g, "Press Enter to end the game");
        } else {
          g->event_step = 3;
          event_set_main(g, "To continue press Enter");
        }
        event_set_input(g, EVENT_INPUT_NONE);
      }
    }
  }
}

static void event_handle_digit(Game *g, int digit) {
  if (!g) return;
  if (digit < 0) digit = 0;
  if (strcmp(g->event_code, "loot_field") == 0) {
    if (g->hero.hp <= 0) {
      event_end_run(g, "You are dead", "Your run has ended. Camp loot saved.", false);
    } else {
      event_finish(g);
    }
  } else if (strcmp(g->event_code, "loot_secret") == 0) {
    event_finish(g);
  } else if (strcmp(g->event_code, "gambler") == 0) {
    if (g->event_step == 0) {
      if (digit == 1 && g->hero.coins > 0) {
        g->event_step = 1;
        event_enter_step(g);
      } else if (digit == 2) {
        int random = rand_range(1, 100);
        int acc = character_accuracy(&g->hero);
        int chance = random + acc;
        logbuffer_clear(&g->log);
        char msg[128];
        snprintf(msg, sizeof(msg), "Accuracy check: Random %d + Accuracy %d = %d", random, acc, chance);
        logbuffer_push(&g->log, msg);
        if (chance >= 140) {
          int coins = rand_range(1, 10);
          g->hero.coins += coins;
          logbuffer_push(&g->log, "140 or more. You caught the little one");
          snprintf(msg, sizeof(msg), "He had %d coins in his pocket. What was yours became mine!!!", coins);
          logbuffer_push(&g->log, msg);
          event_set_art(g, "rob_success");
        } else if (chance < 100 && strcmp(g->hero.weapon.code, "without") != 0) {
          char old_name[64];
          snprintf(old_name, sizeof(old_name), "%s", g->hero.weapon.name);
          g->hero.weapon = weapon_from_code(g, "without");
          logbuffer_push(&g->log, "You didn't catch the little one");
          snprintf(msg, sizeof(msg), "The little guy not only ran away, but also stole %s", old_name);
          logbuffer_push(&g->log, msg);
          logbuffer_push(&g->log, "What a disgrace and now there is nothing to kill myself with");
          event_set_art(g, "rob_fail");
        } else if (chance < 120 && g->hero.coins > 0) {
          int coins = rand_range(1, g->hero.coins);
          g->hero.coins -= coins;
          logbuffer_push(&g->log, "You didn't catch the little one");
          snprintf(msg, sizeof(msg), "The little guy not only ran away, but also stole %d coins", coins);
          logbuffer_push(&g->log, msg);
          event_set_art(g, "rob_fail");
        } else {
          logbuffer_push(&g->log, "You didn't catch the little one");
          event_set_art(g, "rob_fail");
        }
        event_set_main(g, "Press Enter to leave");
        event_set_input(g, EVENT_INPUT_NONE);
        g->event_step = 2;
      } else if (digit == 0) {
        event_finish(g);
      }
    } else if (g->event_step == 1) {
      if (digit == 0) {
        event_finish(g);
      } else if (digit == 2) {
        g->event_step = 0;
        event_handle_digit(g, 2);
      } else if (digit == 1 && g->hero.coins > 0) {
        int y1 = rand_range(1, 6);
        int y2 = rand_range(1, 6);
        int e1 = rand_range(1, 6);
        int e2 = rand_range(1, 7);
        logbuffer_clear(&g->log);
        char msg[160];
        snprintf(msg, sizeof(msg), "Your result is %d + %d = %d, the little one's result is %d + %d = %d",
                 y1, y2, y1 + y2, e1, e2, e1 + e2);
        logbuffer_push(&g->log, msg);
        if (y1 + y2 > e1 + e2) {
          g->hero.coins += 1;
          logbuffer_push(&g->log, "You won 1 coin");
          event_set_art(g, "win");
        } else if (y1 + y2 < e1 + e2) {
          hero_reduce_coins(&g->hero, 1);
          logbuffer_push(&g->log, "You lose 1 coin");
          event_set_art(g, "loose");
        } else {
          logbuffer_push(&g->log, "Draw");
          event_set_art(g, "draw");
        }
        if (e2 == 7) {
          logbuffer_push(&g->log, "7 on the dice? The little bastard is cheating!!!");
        }
        if (g->hero.coins == 0) {
          event_set_main(g, "Catch and rob [Enter 2]       Leave [Enter 0]");
        } else {
          char main[128];
          snprintf(main, sizeof(main), "Your coins: %d   Roll the dice [Enter 1]    Catch and rob [Enter 2]    Leave [Enter 0]", g->hero.coins);
          event_set_main(g, main);
        }
        event_set_input(g, EVENT_INPUT_DIGIT);
      }
    } else if (g->event_step == 2) {
      event_finish(g);
    }
  } else if (strcmp(g->event_code, "altar_of_blood") == 0) {
    int adept = strcmp(g->hero.camp_skill.code, "bloody_ritual") == 0;
    int hp_taken = adept ? (g->hero.camp_skill.lvl > 5 ? 10 : 20) : 30;
    if (g->event_step == 3) {
      event_finish(g);
      return;
    }
    if (g->hero.hp <= hp_taken) {
      event_finish(g);
    } else if (adept) {
      if (digit == 0) {
        event_finish(g);
      } else if (digit >= 1 && digit <= 4) {
        g->hero.hp -= hp_taken;
        logbuffer_clear(&g->log);
        char gift[32];
        if (digit == 1) { snprintf(gift, sizeof(gift), "5 max-HP"); g->hero.hp_max += 5; g->hero.hp += 5; }
        else if (digit == 2) { snprintf(gift, sizeof(gift), "5 max-MP"); g->hero.mp_max += 5; g->hero.mp += 5; }
        else if (digit == 3) { snprintf(gift, sizeof(gift), "1 Accuracy"); g->hero.accuracy_base += 1; }
        else { snprintf(gift, sizeof(gift), "1 Damage"); hero_add_dmg_base(&g->hero, 1); }
        char msg[128];
        snprintf(msg, sizeof(msg), "Bloody god for your blood gives you: %s", gift);
        logbuffer_push(&g->log, msg);
        event_set_main(g, "Press Enter to exit");
        event_set_art(g, "adept_sacrifice");
        event_set_input(g, EVENT_INPUT_NONE);
        g->event_step = 3;
      }
    } else {
      if (digit == 0) {
        event_finish(g);
      } else if (digit == 1) {
        int choices[] = {1,1,2,2,3};
        int pick = choices[rand_range(0, 4)];
        g->hero.hp -= hp_taken;
        logbuffer_clear(&g->log);
        const char *gift = "nothing";
        if (pick == 1) { gift = "5 max-HP"; g->hero.hp_max += 5; g->hero.hp += 5; }
        else if (pick == 2) { gift = "5 max-MP"; g->hero.mp_max += 5; g->hero.mp += 5; }
        else { gift = "1 Accuracy"; g->hero.accuracy_base += 1; }
        char msg[128];
        snprintf(msg, sizeof(msg), "Bloody god for your blood gives you: %s", gift);
        logbuffer_push(&g->log, msg);
        event_set_main(g, "Press Enter to exit");
        event_set_art(g, "common_sacrifice");
        event_set_input(g, EVENT_INPUT_NONE);
        g->event_step = 3;
      }
    }
  } else if (strcmp(g->event_code, "boatman_eugene") == 0) {
    if (g->event_step == 0) {
      if (digit == 1) {
        int random = rand_range(1, 150);
        int acc = character_accuracy(&g->hero);
        bool success = random < acc;
        logbuffer_clear(&g->log);
        logbuffer_push(&g->log, "You offer to teach Evgeniy the art of war while you are sailing");
        logbuffer_push(&g->log, "But Eugene doesn't even try to learn, he just counts cockroaches");
        char msg[128];
        snprintf(msg, sizeof(msg), "Test for attentiveness, random %d %s your accuracy %d", random, success ? "<" : ">=", acc);
        logbuffer_push(&g->log, msg);
        if (success) {
          hero_reduce_mp(&g->hero, 20);
          g->hero.accuracy_base += 1;
          logbuffer_push(&g->log, "You quickly noticed this and stopped wasting time. You lost 20 MP, but gained 1 accuracy");
        } else {
          hero_reduce_mp(&g->hero, 40);
          logbuffer_push(&g->log, "But you didn't notice it right away and kept trying to teach him. You lost 40 MP");
        }
        logbuffer_push(&g->log, "You sailed to the same place. \"What's wrong with you?\" - you asked. \"Who are you and who am I\" - Evgeniy answered");
        event_set_main(g, "To continue press Enter");
        event_set_art(g, "studyng");
        event_set_input(g, EVENT_INPUT_NONE);
        g->event_step = 1;
      } else if (digit == 0) {
        event_finish(g);
      }
    } else {
      event_finish(g);
    }
  } else if (strcmp(g->event_code, "briedge_keeper") == 0) {
    if (g->event_step == 2) {
      g->event_step = 5;
      event_enter_step(g);
    } else if (g->event_step == 3) {
      event_finish(g);
    } else if (g->event_step == 4) {
      event_end_run(g, "You are dead", "Your run has ended. Camp loot saved.", false);
    } else if (g->event_step == 5) {
      g->hero.accuracy_base -= 1;
      g->hero.armor_penetration_base += 1;
      event_finish(g);
    }
  } else if (strcmp(g->event_code, "exit_run") == 0) {
    if (g->event_step == 0) {
      if (digit == 1) {
        int base = rand_range(1, 200);
        int th = treasure_hunter_coeff(&g->hero.camp_skill);
        int chance = base + th;
        logbuffer_clear(&g->log);
        if (th > 0) {
          char msg[128];
          snprintf(msg, sizeof(msg), "Random luck is %d + treasure hunter(%d) = %d...", base, th, chance);
          logbuffer_push(&g->log, msg);
        } else {
          char msg[128];
          snprintf(msg, sizeof(msg), "Random luck is %d...", chance);
          logbuffer_push(&g->log, msg);
        }
        if (chance > 140) {
          logbuffer_push(&g->log, "...more then 140");
          logbuffer_push(&g->log, "...managed to climb the old stairs, hurray, exit");
          event_set_main(g, "You survived. To continue press Enter");
          g->event_step = 1;
          event_set_input(g, EVENT_INPUT_NONE);
        } else if (chance > 70) {
          logbuffer_push(&g->log, "...lower then 140");
          logbuffer_push(&g->log, "...unfortunately it is impossible to reach the stairs");
          event_set_main(g, "To continue press Enter");
          g->event_step = 2;
          event_set_input(g, EVENT_INPUT_NONE);
        } else {
          logbuffer_push(&g->log, "...lower then 70");
          int loss = (int)round(g->hero.hp_max * 0.1);
          g->hero.hp -= loss;
          char msg[160];
          snprintf(msg, sizeof(msg), "...climbed the old ladder, the exit was already close, but the ladder broke...");
          logbuffer_push(&g->log, msg);
          snprintf(msg, sizeof(msg), "...fell and lost %d HP. %d/%d HP left", loss, g->hero.hp, g->hero.hp_max);
          logbuffer_push(&g->log, msg);
          event_set_main(g, "You died and the exit was so close. To continue press Enter");
          g->event_step = 3;
          event_set_input(g, EVENT_INPUT_NONE);
        }
      } else if (digit == 0) {
        event_finish(g);
      }
    } else if (g->event_step == 1) {
      event_end_run(g, "You survived", "Your run has ended. Camp loot saved.", true);
    } else if (g->event_step == 2) {
      event_finish(g);
    } else if (g->event_step == 3) {
      if (g->hero.hp <= 0) {
        event_end_run(g, "You are dead", "Your run has ended. Camp loot saved.", false);
      } else {
        event_finish(g);
      }
    }
  } else if (strcmp(g->event_code, "black_mage") == 0) {
    int adept = strcmp(g->hero.camp_skill.code, "bloody_ritual") == 0;
    int price = adept ? (g->hero.camp_skill.lvl > 5 ? 0 : 1) : 2;
    int b = adept ? 4 : 5;
    int bp = adept ? 3 : 1;
    if (g->event_step == 0) {
      if (digit == 1) {
        if (g->hero.coins >= price) {
          g->hero.coins -= price;
          logbuffer_clear(&g->log);
          logbuffer_push(&g->log, "Black magician pronounces the magic words: 'Klaatu Verata Nikto'");
          int bonus_give = rand_range(1, b);
          int bonus_take = rand_range(1, b);
          while (bonus_give + 1 == bonus_take) bonus_take = rand_range(1, b);
          int bonus_give_power = rand_range(bp, 5);
          int bonus_take_power = rand_range(1, 5);
          if (adept && bonus_give_power < bonus_take_power) bonus_give_power = bonus_take_power;
          if (bonus_give == 1) {
            g->hero.hp_max += bonus_give_power;
            g->hero.hp += bonus_give_power;
            char msg[128];
            snprintf(msg, sizeof(msg), "You got %d Max HP, now you have %d/%d HP", bonus_give_power, g->hero.hp, g->hero.hp_max);
            logbuffer_push(&g->log, msg);
          } else if (bonus_give == 2) {
            g->hero.mp_max += bonus_give_power;
            g->hero.mp += bonus_give_power;
            char msg[128];
            snprintf(msg, sizeof(msg), "You got %d Max MP, now you have %d/%d MP", bonus_give_power, g->hero.mp, g->hero.mp_max);
            logbuffer_push(&g->log, msg);
          } else if (bonus_give == 3) {
            g->hero.accuracy_base += 1;
            char msg[128];
            snprintf(msg, sizeof(msg), "You got 1 accuracy, now you have %d accuracy", g->hero.accuracy_base);
            logbuffer_push(&g->log, msg);
          } else if (bonus_give == 4) {
            hero_add_dmg_base(&g->hero, 1);
            char msg[128];
            snprintf(msg, sizeof(msg), "You got 1 damage, now you have %d-%d damage", g->hero.min_dmg_base, g->hero.max_dmg_base);
            logbuffer_push(&g->log, msg);
          } else {
            logbuffer_push(&g->log, "You got nothing");
          }
          if (bonus_take == 1) {
            logbuffer_push(&g->log, "...and you lose nothing");
          } else if (bonus_take == 2) {
            g->hero.hp_max -= bonus_take_power;
            if (g->hero.hp > g->hero.hp_max) g->hero.hp = g->hero.hp_max;
            char msg[128];
            snprintf(msg, sizeof(msg), "...but you lose %d Max HP, now you have %d/%d HP", bonus_take_power, g->hero.hp, g->hero.hp_max);
            logbuffer_push(&g->log, msg);
          } else if (bonus_take == 3) {
            g->hero.mp_max -= bonus_take_power;
            if (g->hero.mp > g->hero.mp_max) g->hero.mp = g->hero.mp_max;
            char msg[128];
            snprintf(msg, sizeof(msg), "...but you lose %d Max MP, now you have %d/%d MP", bonus_take_power, g->hero.mp, g->hero.mp_max);
            logbuffer_push(&g->log, msg);
          } else if (bonus_take == 4) {
            g->hero.accuracy_base -= 1;
            char msg[128];
            snprintf(msg, sizeof(msg), "...but you lose 1 accuracy, now you have %d accuracy", g->hero.accuracy_base);
            logbuffer_push(&g->log, msg);
          } else {
            hero_reduce_dmg_base(&g->hero, 1);
            char msg[128];
            snprintf(msg, sizeof(msg), "...but you lose 1 damage, now you have %d-%d damage", g->hero.min_dmg_base, g->hero.max_dmg_base);
            logbuffer_push(&g->log, msg);
          }
          g->event_step = 1;
          {
            char main[128];
            snprintf(main, sizeof(main), "You have %d coins.      Attack mage [Enter 1]      Leave [Enter 0]", g->hero.coins);
            event_set_main(g, main);
            event_set_art(g, "action");
            event_set_input(g, EVENT_INPUT_DIGIT);
          }
        } else {
          logbuffer_clear(&g->log);
          char msg[128];
          snprintf(msg, sizeof(msg), "Not enough coins to buy a spell for %d", price);
          logbuffer_push(&g->log, msg);
          g->event_step = 1;
          {
            char main[128];
            snprintf(main, sizeof(main), "You have %d coins.      Attack mage [Enter 1]      Leave [Enter 0]", g->hero.coins);
            event_set_main(g, main);
            event_set_art(g, "action");
            event_set_input(g, EVENT_INPUT_DIGIT);
          }
        }
      } else if (digit == 2) {
        const EnemyTemplate *tmpl = event_enemy_by_code(g, "black_mage");
        if (tmpl) {
          g->enemy = character_from_enemy(g, tmpl);
          snprintf(g->battle_art_dungeon, sizeof(g->battle_art_dungeon), "events");
          snprintf(g->battle_art_name, sizeof(g->battle_art_name), "normal");
          g->battle_anim_active = 0;
          g->battle_anim_step = 0;
          g->battle_anim_count = 0;
          g->battle_anim_deadline = 0;
          g->battle_exit_pending = 0;
          g->enemy_is_boss = 0;
          logbuffer_clear(&g->log);
          g->state = STATE_BATTLE;
        } else {
          logbuffer_push(&g->log, "No enemy found");
        }
      } else if (digit == 0) {
        event_finish(g);
      }
    } else if (g->event_step == 1) {
      if (digit == 1) {
        const EnemyTemplate *tmpl = event_enemy_by_code(g, "black_mage");
        if (tmpl) {
          g->enemy = character_from_enemy(g, tmpl);
          snprintf(g->battle_art_dungeon, sizeof(g->battle_art_dungeon), "events");
          snprintf(g->battle_art_name, sizeof(g->battle_art_name), "normal");
          g->battle_anim_active = 0;
          g->battle_anim_step = 0;
          g->battle_anim_count = 0;
          g->battle_anim_deadline = 0;
          g->battle_exit_pending = 0;
          g->enemy_is_boss = 0;
          logbuffer_clear(&g->log);
          g->state = STATE_BATTLE;
        }
      } else if (digit == 0) {
        event_finish(g);
      }
    }
  } else if (strcmp(g->event_code, "pig_with_saucepan") == 0) {
    if (g->event_step == 0) {
      if (digit == 1) {
        int price = 15;
        if (strcmp(g->hero.camp_skill.code, "treasure_hunter") == 0) {
          int th = treasure_hunter_coeff(&g->hero.camp_skill);
          double discount = th * 0.5;
          if (discount > 90) discount = 90;
          price = (int)round(price * (100.0 - discount) * 0.01);
          logbuffer_clear(&g->log);
          char msg[128];
          snprintf(msg, sizeof(msg), "Treasure Hunter skill check %d => you get a %.0f%% discount", th, discount);
          logbuffer_push(&g->log, msg);
        } else {
          logbuffer_clear(&g->log);
        }
        logbuffer_push(&g->log, "Acorn? Do you think pigmen are idiots? You can eat from this saucepan and then shit in it");
        char msg[128];
        snprintf(msg, sizeof(msg), "it’s healthy. I know it costs %d coins, pay up or get lost", price);
        logbuffer_push(&g->log, msg);
        if (g->hero.coins < price) {
          snprintf(msg, sizeof(msg), "You have no %d coins, and cant buy Sallet", price);
          logbuffer_push(&g->log, msg);
          char main[128];
          snprintf(main, sizeof(main), "Your coins: %d    Catch and rob [Enter 1]       Leave [Enter 0]", g->hero.coins);
          event_set_main(g, main);
        } else {
          char main[128];
          snprintf(main, sizeof(main), "Your coins: %d   Buy for %d coins [Enter 1]   Catch and rob [Enter 2]   Leave [Enter 0]", g->hero.coins, price);
          event_set_main(g, main);
        }
        event_set_art(g, "buy");
        event_set_input(g, EVENT_INPUT_DIGIT);
        g->event_data[0] = price;
        g->event_step = 1;
      } else if (digit == 2) {
        g->event_step = 3;
        g->event_data[1] = 0;
        event_handle_digit(g, 2);
      } else if (digit == 0) {
        event_finish(g);
      }
    } else if (g->event_step == 1) {
      int price = g->event_data[0];
      if (g->hero.coins < price) {
        if (digit == 1) {
          g->event_step = 3;
          event_handle_digit(g, 2);
        } else if (digit == 0) {
          event_finish(g);
        }
      } else {
        if (digit == 1) {
          hero_reduce_coins(&g->hero, price);
          event_offer_loot(g, "head_armor", "sallet", "Sallet is yours, you want to equip it?", EVENT_PENDING_PIG_SALLET);
          return;
        } else if (digit == 2) {
          g->event_step = 3;
          g->event_data[1] = 0;
          event_handle_digit(g, 2);
        } else if (digit == 0) {
          event_finish(g);
        }
      }
    } else if (g->event_step == 3) {
      if (digit == 2 && g->event_data[1] == 0) {
        int random = rand_range(1, 100);
        int acc = character_accuracy(&g->hero);
        int chance = random + acc;
        logbuffer_clear(&g->log);
        char msg[128];
        snprintf(msg, sizeof(msg), "Accuracy check: Random %d + Accuracy %d = %d", random, acc, chance);
        logbuffer_push(&g->log, msg);
        if (chance >= 170) {
          logbuffer_push(&g->log, "You caught the pigman");
          logbuffer_push(&g->log, "Now Sallet is yours, and the pigman can be used for meat");
          event_set_art(g, "catch");
          event_set_main(g, "Press Enter to view Sallet");
          g->event_pending_action = EVENT_PENDING_PIG_SALLET;
        } else if (chance < 130 && g->hero.coins > 0) {
          int coins = rand_range(1, g->hero.coins);
          g->hero.coins -= coins;
          logbuffer_push(&g->log, "You didn't catch the pigman");
          char msg2[128];
          snprintf(msg2, sizeof(msg2), "The pigman not only run away, but also stole %d coins", coins);
          logbuffer_push(&g->log, msg2);
          event_set_art(g, "run");
          event_set_main(g, "Press Enter to leave");
        } else {
          logbuffer_push(&g->log, "You didn't catch the pigman");
          event_set_art(g, "run");
          event_set_main(g, "Press Enter to leave");
        }
        event_set_input(g, EVENT_INPUT_NONE);
        g->event_data[1] = 1;
      } else {
        if (g->event_data[1] == 1 && g->event_pending_action == EVENT_PENDING_PIG_SALLET) {
          event_offer_loot(g, "head_armor", "sallet", "Sallet is yours, you want to equip it?", EVENT_PENDING_PIG_SALLET);
        } else if (g->event_data[1] == 1) {
          event_finish(g);
        }
      }
    }
  } else if (strcmp(g->event_code, "wariors_grave") == 0) {
    if (!g->wg_taken && g->event_step == 2) {
      if (digit == 1) {
        int stats_count = stats_total_get(&g->stats_total, g->hero.dungeon_name, g->wg_enemy);
        g->wg_taken = 1;
        g->wg_count = stats_count + 3;
        logbuffer_clear(&g->log);
        event_set_main(g, "Leave [Enter 0]");
        logbuffer_push(&g->log, "\"I immediately realized that you are one of us, let's cleanse these lands\"");
        event_set_art(g, "clean");
        event_set_input(g, EVENT_INPUT_DIGIT);
        g->event_step = 21;
      } else if (digit == 0) {
        event_finish(g);
      }
      return;
    }
    if (!g->wg_taken && (g->event_step == 31 || g->event_step == 41)) {
      if (digit == 0) event_finish(g);
      return;
    }
    if (!g->wg_taken) {
      if (digit == 0) {
        event_finish(g);
      } else if (digit == 1) {
        int base = rand_range(0, 200);
        int th = treasure_hunter_coeff(&g->hero.camp_skill);
        int chance = base + th;
        if (chance > 220) {
          char msg[192];
          if (th > 0) {
            snprintf(msg, sizeof(msg), "Random luck is %d + treasure hunter %d = %d > 220. You dug up Rusty falchion, take it or bury it back?",
                     base, th, chance);
          } else {
            snprintf(msg, sizeof(msg), "Random luck is %d > 220. You dug up a grave and Rusty falchion there, take it or bury it back?",
                     chance);
          }
          event_offer_loot(g, "weapon", "rusty_falchion", msg, EVENT_PENDING_GRAVE_DIG);
          return;
        } else if (chance > 150) {
          char msg[192];
          if (th > 0) {
            snprintf(msg, sizeof(msg), "Random luck is %d + treasure hunter %d = %d > 150. You dug up Rusty sword, take it or bury it back?",
                     base, th, chance);
          } else {
            snprintf(msg, sizeof(msg), "Random luck is %d > 150. You dug up a grave and Rusty sword there, take it or bury it back?",
                     chance);
          }
          event_offer_loot(g, "weapon", "rusty_sword", msg, EVENT_PENDING_GRAVE_DIG);
          return;
        } else if (chance > 80) {
          char msg[192];
          if (th > 0) {
            snprintf(msg, sizeof(msg), "Random luck is %d + treasure hunter %d = %d > 80. You dug up Rusty hatchet, take it or bury it back?",
                     base, th, chance);
          } else {
            snprintf(msg, sizeof(msg), "Random luck is %d > 80. You dug up a grave and Rusty hatchet there, take it or bury it back?",
                     chance);
          }
          event_offer_loot(g, "weapon", "rusty_hatchet", msg, EVENT_PENDING_GRAVE_DIG);
          return;
        } else {
          logbuffer_clear(&g->log);
          char msg[160];
          if (th > 0) {
            snprintf(msg, sizeof(msg), "Random luck is %d + treasure hunter %d = %d <= 80. You dug up a grave and nothing there", base, th, chance);
          } else {
            snprintf(msg, sizeof(msg), "Random luck is %d <= 80. You dug up a grave and nothing there", chance);
          }
          logbuffer_push(&g->log, msg);
          int mp = rand_range(20, 100);
          hero_reduce_mp(&g->hero, mp);
          snprintf(msg, sizeof(msg), "The warrior's spirit is furious, he took %d MP from you", mp);
          logbuffer_push(&g->log, msg);
          event_set_main(g, "Leave [Enter 0]");
          event_set_art(g, "diged");
          event_set_input(g, EVENT_INPUT_DIGIT);
          g->event_step = 31;
        }
      } else if (digit == 2) {
        hero_add_hp(&g->hero, 5);
        hero_add_mp(&g->hero, 5);
        const char *enemy = "poacher";
        int level = g->hero.lvl < 5 ? 1 : 2;
        if (strcmp(g->hero.dungeon_name, "bandits") == 0) enemy = level == 1 ? "poacher" : "deserter";
        else if (strcmp(g->hero.dungeon_name, "undeads") == 0) enemy = level == 1 ? "skeleton" : "skeleton_soldier";
        else if (strcmp(g->hero.dungeon_name, "swamp") == 0) enemy = level == 1 ? "goblin" : "orc";
        char enemy_name[64];
        titleize_token(enemy, enemy_name, sizeof(enemy_name));
        logbuffer_clear(&g->log);
        logbuffer_push(&g->log, "After cleaning the grave you felt better, the warrior's spirit restored you 5 HP and 5 MP");
        logbuffer_push(&g->log, "\"I see that you are also a warrior and could continue my work and cleanse these lands\"");
        char msg[160];
        snprintf(msg, sizeof(msg), "\"If you kill 3 %ss and go to any warrior's grave, you will receive a reward\"", enemy_name);
        logbuffer_push(&g->log, msg);
        event_set_main(g, "Take quest [Enter 1]                 Leave [Enter 0]");
        event_set_art(g, "clean");
        event_set_input(g, EVENT_INPUT_DIGIT);
        snprintf(g->wg_enemy, sizeof(g->wg_enemy), "%s", enemy);
        g->wg_level = level;
        g->event_step = 2;
      }
    } else {
      if (digit == 0) event_finish(g);
    }
  }
}

static void skill_show_cost(const Skill *s, char *out, size_t out_sz) {
  if (!s || !out || out_sz == 0) return;
  if (s->type == SKILL_ACTIVE) {
    snprintf(out, out_sz, "%d MP", s->mp_cost);
  } else if (strcmp(s->code, "bloody_ritual") == 0) {
    snprintf(out, out_sz, "%d HP", s->hp_cost);
  } else if (strcmp(s->code, "first_aid") == 0) {
    snprintf(out, out_sz, "%d MP", s->mp_cost);
  } else {
    snprintf(out, out_sz, "passive");
  }
}

static double skill_active_damage_mod(const Skill *s, const Character *hero) {
  if (!s) return 1.0;
  if (strcmp(s->code, "ascetic_strike") == 0) return 1.0 + 0.007 * s->lvl * hero->stat_points;
  if (strcmp(s->code, "precise_strike") == 0) return 1.1 + 0.05 * s->lvl;
  if (strcmp(s->code, "strong_strike") == 0) return 1.5 + 0.15 * s->lvl;
  if (strcmp(s->code, "traumatic_strike") == 0) return 1.0;
  return 1.0;
}

static double skill_active_accuracy_mod(const Skill *s, const Character *hero) {
  (void)hero;
  if (!s) return 1.0;
  if (strcmp(s->code, "ascetic_strike") == 0) return 1.0 + 0.007 * s->lvl * hero->stat_points;
  if (strcmp(s->code, "precise_strike") == 0) return 1.3 + 0.1 * s->lvl;
  if (strcmp(s->code, "strong_strike") == 0) return 1.0;
  if (strcmp(s->code, "traumatic_strike") == 0) return 1.0;
  return 1.0;
}

static double skill_traumatic_effect_coef(const Skill *s) {
  if (!s) return 1.0;
  if (strcmp(s->code, "traumatic_strike") != 0) return 1.0;
  double effect = 30 + 3.0 * s->lvl;
  double coef = (100.0 - effect) / 100.0;
  return coef < 0 ? 0 : coef;
}

static double skill_berserk_coef(const Skill *s, const Character *hero) {
  if (!s || strcmp(s->code, "berserk") != 0) return 1.0;
  double hp_part = (hero->hp_max > 0) ? (double)hero->hp / hero->hp_max : 0.0;
  double mod = 0.5 + 0.05 * s->lvl;
  return 1.0 + (1.0 - hp_part) * mod;
}

static double skill_concentration_bonus(const Skill *s, const Character *hero) {
  if (!s || strcmp(s->code, "concentration") != 0) return 0.0;
  double coef = hero->mp_max * (0.1 + 0.005 * s->lvl) - 10.0;
  if (coef <= 0) return 0.0;
  return (double)rand_range(0, (int)coef);
}

static double skill_dazed_hp_part_coef(const Skill *s) {
  if (!s || strcmp(s->code, "dazed") != 0) return 0.0;
  return 1.2 + 0.15 * s->lvl;
}

static double skill_dazed_accuracy_reduce_coef(const Skill *s) {
  if (!s || strcmp(s->code, "dazed") != 0) return 1.0;
  int min_reduce = 10 + 3 * s->lvl;
  if (min_reduce > 90) min_reduce = 90;
  int reduce = rand_range(min_reduce, 90);
  return 0.01 * (100 - reduce);
}

static void skill_description(const Skill *s, const Character *hero, char *out, size_t out_sz) {
  if (!s || !out || out_sz == 0) return;
  if (strcmp(s->code, "ascetic_strike") == 0) {
    int dmg = (int)round((skill_active_damage_mod(s, hero) - 1) * 100);
    int acc = (int)round((skill_active_accuracy_mod(s, hero) - 1) * 100);
    snprintf(out, out_sz, "Free stat points %d. Additional damage +%d%%. Additional accuracy +%d%%", hero->stat_points, dmg, acc);
  } else if (strcmp(s->code, "precise_strike") == 0) {
    int dmg = (int)round((skill_active_damage_mod(s, hero) - 1) * 100);
    int acc = (int)round((skill_active_accuracy_mod(s, hero) - 1) * 100);
    snprintf(out, out_sz, "Additional damage +%d%%. Additional accuracy +%d%%", dmg, acc);
  } else if (strcmp(s->code, "strong_strike") == 0) {
    int dmg = (int)round((skill_active_damage_mod(s, hero) - 1) * 100);
    snprintf(out, out_sz, "Additional damage +%d%%", dmg);
  } else if (strcmp(s->code, "traumatic_strike") == 0) {
    int effect = 30 + 3 * s->lvl;
    snprintf(out, out_sz, "Attack reduces enemy damage by %d%%", effect);
  } else if (strcmp(s->code, "berserk") == 0) {
    int dmg = (int)round((skill_berserk_coef(s, hero) - 1) * 100);
    int hp_part = hero->hp_max > 0 ? (int)round((double)hero->hp / hero->hp_max * 100) : 0;
    snprintf(out, out_sz, "The less HP - the more damage. HP is %d%% from the maximum. Additional damage +%d%%", hp_part, dmg);
  } else if (strcmp(s->code, "concentration") == 0) {
    double coef = hero->mp_max * (0.1 + 0.005 * s->lvl) - 10.0;
    snprintf(out, out_sz, "If max MP is more than 100(%d) random additional damage up to %.1f", hero->mp_max, coef);
  } else if (strcmp(s->code, "dazed") == 0) {
    double hp_part = skill_dazed_hp_part_coef(s);
    int min_reduce = 10 + 3 * s->lvl;
    if (min_reduce > 90) min_reduce = 90;
    int percent = (int)round(100 / (2 * hp_part));
    snprintf(out, out_sz, "If damage is greater %d%% remaining enemy HP then he loses %d-90%% accuracy", percent, min_reduce);
  } else if (strcmp(s->code, "shield_master") == 0) {
    int bonus = 10 + 2 * s->lvl;
    snprintf(out, out_sz, "Shield block chance increased by %d%%", bonus);
  } else if (strcmp(s->code, "bloody_ritual") == 0) {
    int mp_dif = hero->mp_max - hero->mp;
    int min_effect = mp_dif < 10 ? mp_dif : 10;
    double effect = mp_dif * 0.3 * (1 + 0.1 * s->lvl);
    int restore = (int)round(effect);
    if (restore < min_effect) restore = min_effect;
    if (restore > mp_dif) restore = mp_dif;
    snprintf(out, out_sz, "Restores %d MP, the more MP lost the greater the effect", restore);
  } else if (strcmp(s->code, "first_aid") == 0) {
    int hp_dif = hero->hp_max - hero->hp;
    int min_effect = hp_dif < 5 ? hp_dif : 5;
    double effect = hp_dif * 0.2 * (1 + 0.1 * s->lvl);
    int restore = (int)round(effect);
    if (restore < min_effect) restore = min_effect;
    if (restore > hp_dif) restore = hp_dif;
    snprintf(out, out_sz, "Restores %d HP, the more HP lost the greater the effect", restore);
  } else if (strcmp(s->code, "treasure_hunter") == 0) {
    int bonus = 50 + 10 * s->lvl;
    snprintf(out, out_sz, "Positively affects random actions. Luck bonus %d", bonus);
  } else {
    if (out_sz > 0) out[0] = '\0';
  }
}

static void skill_description_short(const Skill *s, const Character *hero, char *out, size_t out_sz) {
  (void)hero;
  if (!s || !out || out_sz == 0) return;
  if (strcmp(s->code, "ascetic_strike") == 0) {
    snprintf(out, out_sz, "Cost %d MP. More free stat points = more damage/accuracy", s->mp_cost);
  } else if (strcmp(s->code, "precise_strike") == 0) {
    snprintf(out, out_sz, "Cost %d MP. Attack much more accurately", s->mp_cost);
  } else if (strcmp(s->code, "strong_strike") == 0) {
    snprintf(out, out_sz, "Cost %d MP. Attack much stronger", s->mp_cost);
  } else if (strcmp(s->code, "traumatic_strike") == 0) {
    snprintf(out, out_sz, "Cost %d MP. Reduces enemy damage", s->mp_cost);
  } else if (strcmp(s->code, "berserk") == 0) {
    snprintf(out, out_sz, "The less HP are left - the more damage you do");
  } else if (strcmp(s->code, "concentration") == 0) {
    snprintf(out, out_sz, "More max MP -> more random additional damage");
  } else if (strcmp(s->code, "dazed") == 0) {
    snprintf(out, out_sz, "Big hit can drastically reduce enemy accuracy");
  } else if (strcmp(s->code, "shield_master") == 0) {
    snprintf(out, out_sz, "Shield block chance increased");
  } else if (strcmp(s->code, "bloody_ritual") == 0) {
    snprintf(out, out_sz, "Cost %d HP. Restores MP based on missing MP", s->hp_cost);
  } else if (strcmp(s->code, "first_aid") == 0) {
    snprintf(out, out_sz, "Cost %d MP. Restores HP based on missing HP", s->mp_cost);
  } else if (strcmp(s->code, "treasure_hunter") == 0) {
    snprintf(out, out_sz, "Positively affects random actions in game");
  } else {
    if (out_sz > 0) out[0] = '\0';
  }
}

static int hero_next_lvl_exp(const Character *c) {
  static const int EXP_LVL[] = {0, 2, 5, 9, 14, 20, 27, 35, 44, 54, 65, 77, 90, 104, 129, 145, 162, 180, 200};
  int idx = c->lvl + 1;
  if (idx < 0) idx = 0;
  if (idx >= (int)(sizeof(EXP_LVL) / sizeof(EXP_LVL[0]))) idx = (int)(sizeof(EXP_LVL) / sizeof(EXP_LVL[0])) - 1;
  return EXP_LVL[idx];
}

static void hero_add_exp(Character *hero, int added_exp, LogBuffer *log) {
  static const int EXP_LVL[] = {0, 2, 5, 9, 14, 20, 27, 35, 44, 54, 65, 77, 90, 104, 129, 145, 162, 180, 200};
  if (!hero) return;
  hero->exp += added_exp;
  if (log) {
    char msg[128];
    snprintf(msg, sizeof(msg), "You have gained %d exp, now you have %d exp", added_exp, hero->exp);
    logbuffer_push(log, msg);
  }
  int new_levels = 0;
  int max_idx = (int)(sizeof(EXP_LVL) / sizeof(EXP_LVL[0])) - 1;
  for (int i = 0; i <= max_idx; ++i) {
    if (hero->exp >= EXP_LVL[i] && hero->lvl < i) {
      new_levels = i - hero->lvl;
      hero->lvl = i;
    }
  }
  if (new_levels > 0) {
    hero->stat_points += new_levels;
    hero->skill_points += new_levels;
    if (log) {
      char msg[128];
      snprintf(msg, sizeof(msg), "You have gained %d new level(s). Now lvl %d", new_levels, hero->lvl);
      logbuffer_push(log, msg);
      snprintf(msg, sizeof(msg), "You gained %d stat and %d skill points", new_levels, new_levels);
      logbuffer_push(log, msg);
    }
  }
}

static void hero_rest(Character *hero, LogBuffer *log) {
  if (!hero) return;
  if (hero->hp < hero->hp_max) {
    int rec = character_recovery_hp(hero);
    int gain = rec;
    if (hero->hp + gain > hero->hp_max) gain = hero->hp_max - hero->hp;
    hero->hp += gain;
    if (log && gain > 0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "After resting, you restore %d HP (%d/%d)", gain, hero->hp, hero->hp_max);
      logbuffer_push(log, msg);
    }
  }
  if (hero->mp < hero->mp_max) {
    int rec = character_recovery_mp(hero);
    int gain = rec;
    if (hero->mp + gain > hero->mp_max) gain = hero->mp_max - hero->mp;
    hero->mp += gain;
    if (log && gain > 0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "After resting, you restore %d MP (%d/%d)", gain, hero->mp, hero->mp_max);
      logbuffer_push(log, msg);
    }
  }
}

static int camp_skill_restore_hp(const Character *hero) {
  if (!hero) return 0;
  int hp_dif = hero->hp_max - hero->hp;
  if (hp_dif <= 0) return 0;
  int min_effect = hp_dif < 5 ? hp_dif : 5;
  double effect = hp_dif * 0.2 * (1 + 0.1 * hero->camp_skill.lvl);
  int restore = (int)round(effect);
  if (restore < min_effect) restore = min_effect;
  if (restore > hp_dif) restore = hp_dif;
  return restore;
}

static int camp_skill_restore_mp(const Character *hero) {
  if (!hero) return 0;
  int mp_dif = hero->mp_max - hero->mp;
  if (mp_dif <= 0) return 0;
  int min_effect = mp_dif < 10 ? mp_dif : 10;
  double effect = mp_dif * 0.3 * (1 + 0.1 * hero->camp_skill.lvl);
  int restore = (int)round(effect);
  if (restore < min_effect) restore = min_effect;
  if (restore > mp_dif) restore = mp_dif;
  return restore;
}

static void game_use_camp_skill(Game *g) {
  if (!g) return;
  Character *hero = &g->hero;
  logbuffer_clear(&g->log);
  if (strcmp(hero->camp_skill.code, "first_aid") == 0) {
    if (hero->hp_max <= hero->hp) {
      char msg[128];
      snprintf(msg, sizeof(msg), "You dont need use %s", hero->camp_skill.name);
      logbuffer_push(&g->log, msg);
      return;
    }
    if (hero->mp < hero->camp_skill.mp_cost) {
      char msg[128];
      snprintf(msg, sizeof(msg), "Not enough MP for \"%s\"", hero->camp_skill.name);
      logbuffer_push(&g->log, msg);
      return;
    }
    int restore = camp_skill_restore_hp(hero);
    hero->hp += restore;
    if (hero->hp > hero->hp_max) hero->hp = hero->hp_max;
    hero->mp -= hero->camp_skill.mp_cost;
    if (hero->mp < 0) hero->mp = 0;
    char msg[160];
    snprintf(msg, sizeof(msg), "You restored %d HP for %d MP, now you have %d/%d HP and %d/%d MP",
             restore, hero->camp_skill.mp_cost, hero->hp, hero->hp_max, hero->mp, hero->mp_max);
    logbuffer_push(&g->log, msg);
    return;
  }
  if (strcmp(hero->camp_skill.code, "bloody_ritual") == 0) {
    if (hero->mp_max <= hero->mp) {
      char msg[128];
      snprintf(msg, sizeof(msg), "You dont need use %s", hero->camp_skill.name);
      logbuffer_push(&g->log, msg);
      return;
    }
    if (hero->hp <= hero->camp_skill.hp_cost) {
      char msg[128];
      snprintf(msg, sizeof(msg), "Not enough HP for \"%s\"", hero->camp_skill.name);
      logbuffer_push(&g->log, msg);
      return;
    }
    int restore = camp_skill_restore_mp(hero);
    hero->mp += restore;
    if (hero->mp > hero->mp_max) hero->mp = hero->mp_max;
    hero->hp -= hero->camp_skill.hp_cost;
    if (hero->hp < 0) hero->hp = 0;
    char msg[160];
    snprintf(msg, sizeof(msg), "You restored %d MP for %d HP, now you have %d/%d MP and %d/%d HP",
             restore, hero->camp_skill.hp_cost, hero->mp, hero->mp_max, hero->hp, hero->hp_max);
    logbuffer_push(&g->log, msg);
    return;
  }
  char msg[128];
  snprintf(msg, sizeof(msg), "You dont need use %s", hero->camp_skill.name);
  logbuffer_push(&g->log, msg);
}

static void ammo_display_name(const char *name, bool enhanced, char *out, size_t out_sz) {
  if (!out || out_sz == 0) return;
  if (!name) name = "---";
  if (enhanced) snprintf(out, out_sz, "(E+) %s", name);
  else snprintf(out, out_sz, "%s", name);
}

static void character_to_map(const Character *c, ValueMap *map) {
  char buf[128];
  value_map_set(map, "name", c->name);
  snprintf(buf, sizeof(buf), "%d", c->hp); value_map_set(map, "hp", buf);
  snprintf(buf, sizeof(buf), "%d", c->hp_max); value_map_set(map, "hp_max", buf);
  snprintf(buf, sizeof(buf), "%d", c->regen_hp_base); value_map_set(map, "regen_hp_base", buf);
  snprintf(buf, sizeof(buf), "%d", c->mp); value_map_set(map, "mp", buf);
  snprintf(buf, sizeof(buf), "%d", c->mp_max); value_map_set(map, "mp_max", buf);
  snprintf(buf, sizeof(buf), "%d", c->regen_mp_base); value_map_set(map, "regen_mp_base", buf);
  snprintf(buf, sizeof(buf), "%d", character_min_dmg(c)); value_map_set(map, "min_dmg", buf);
  snprintf(buf, sizeof(buf), "%d", character_max_dmg(c)); value_map_set(map, "max_dmg", buf);
  snprintf(buf, sizeof(buf), "%d", character_armor_penetration(c)); value_map_set(map, "armor_penetration", buf);
  snprintf(buf, sizeof(buf), "%d", character_accuracy(c)); value_map_set(map, "accuracy", buf);
  snprintf(buf, sizeof(buf), "%d", character_armor(c)); value_map_set(map, "armor", buf);
  snprintf(buf, sizeof(buf), "%d", character_block_chance(c)); value_map_set(map, "block_chance", buf);
  snprintf(buf, sizeof(buf), "%d", block_power_in_percents(c)); value_map_set(map, "block_power_in_percents", buf);

  snprintf(buf, sizeof(buf), "%d", c->min_dmg_base); value_map_set(map, "min_dmg_base", buf);
  snprintf(buf, sizeof(buf), "%d", c->max_dmg_base); value_map_set(map, "max_dmg_base", buf);
  snprintf(buf, sizeof(buf), "%d", c->accuracy_base); value_map_set(map, "accuracy_base", buf);
  snprintf(buf, sizeof(buf), "%d", c->armor_base); value_map_set(map, "armor_base", buf);
  snprintf(buf, sizeof(buf), "%d", c->block_chance_base); value_map_set(map, "block_chance_base", buf);
  snprintf(buf, sizeof(buf), "%d", c->armor_penetration_base); value_map_set(map, "armor_penetration_base", buf);

  snprintf(buf, sizeof(buf), "%d", c->stat_points); value_map_set(map, "stat_points", buf);
  snprintf(buf, sizeof(buf), "%d", c->skill_points); value_map_set(map, "skill_points", buf);
  snprintf(buf, sizeof(buf), "%d", c->exp); value_map_set(map, "exp", buf);
  snprintf(buf, sizeof(buf), "%d", c->lvl); value_map_set(map, "lvl", buf);
  snprintf(buf, sizeof(buf), "%d", hero_next_lvl_exp(c)); value_map_set(map, "next_lvl_exp", buf);
  snprintf(buf, sizeof(buf), "%d", c->pzdc_monolith_points); value_map_set(map, "pzdc_monolith_points", buf);
  snprintf(buf, sizeof(buf), "%d", c->coins); value_map_set(map, "coins", buf);

  snprintf(buf, sizeof(buf), "%d", character_recovery_hp(c)); value_map_set(map, "recovery_hp", buf);
  snprintf(buf, sizeof(buf), "%d", character_recovery_mp(c)); value_map_set(map, "recovery_mp", buf);

  ammo_display_name(c->weapon.name, c->weapon.enhanced, buf, sizeof(buf));
  value_map_set(map, "weapon.name", buf);
  ammo_display_name(c->head_armor.name, c->head_armor.enhanced, buf, sizeof(buf));
  value_map_set(map, "head_armor.name", buf);
  ammo_display_name(c->body_armor.name, c->body_armor.enhanced, buf, sizeof(buf));
  value_map_set(map, "body_armor.name", buf);
  ammo_display_name(c->arms_armor.name, c->arms_armor.enhanced, buf, sizeof(buf));
  value_map_set(map, "arms_armor.name", buf);
  ammo_display_name(c->shield.name, c->shield.enhanced, buf, sizeof(buf));
  value_map_set(map, "shield.name", buf);

  value_map_set(map, "active_skill.name", c->active_skill.name);
  snprintf(buf, sizeof(buf), "%d", c->active_skill.lvl); value_map_set(map, "active_skill.lvl", buf);
  char cost[64];
  skill_show_cost(&c->active_skill, cost, sizeof(cost));
  value_map_set(map, "active_skill.show_cost", cost);
  char desc[256];
  skill_description(&c->active_skill, c, desc, sizeof(desc));
  value_map_set(map, "active_skill.description", desc);

  value_map_set(map, "passive_skill.name", c->passive_skill.name);
  snprintf(buf, sizeof(buf), "%d", c->passive_skill.lvl); value_map_set(map, "passive_skill.lvl", buf);
  skill_show_cost(&c->passive_skill, cost, sizeof(cost));
  value_map_set(map, "passive_skill.show_cost", cost);
  skill_description(&c->passive_skill, c, desc, sizeof(desc));
  value_map_set(map, "passive_skill.description", desc);

  value_map_set(map, "camp_skill.name", c->camp_skill.name);
  snprintf(buf, sizeof(buf), "%d", c->camp_skill.lvl); value_map_set(map, "camp_skill.lvl", buf);
  skill_show_cost(&c->camp_skill, cost, sizeof(cost));
  value_map_set(map, "camp_skill.show_cost", cost);
  skill_description(&c->camp_skill, c, desc, sizeof(desc));
  value_map_set(map, "camp_skill.description", desc);
}

static bool load_heroes(const char *path, HeroTemplate **out, size_t *out_count) {
  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }

  HeroTemplate *heroes = NULL;
  size_t count = 0;

  for (size_t i = 0; i < root->map.len; ++i) {
    const char *code = root->map.keys[i];
    Node *h = root->map.values[i];
    if (!code || !h || h->type != NODE_MAP) continue;

    HeroTemplate ht = {0};
    snprintf(ht.code, sizeof(ht.code), "%s", code);
    Node *name = node_map_get(h, "name");
    Node *hp = node_map_get(h, "hp");
    Node *mp = node_map_get(h, "mp");
    Node *min_dmg = node_map_get(h, "min_dmg");
    Node *max_dmg = node_map_get(h, "max_dmg");
    Node *armor_pen = node_map_get(h, "armor_penetration");
    Node *acc = node_map_get(h, "accurasy");
    Node *armor = node_map_get(h, "armor");
    Node *skill_points = node_map_get(h, "skill_points");
    Node *weapon = node_map_get(h, "weapon");
    Node *body_armor = node_map_get(h, "body_armor");
    Node *head_armor = node_map_get(h, "head_armor");
    Node *arms_armor = node_map_get(h, "arms_armor");
    Node *shield = node_map_get(h, "shield");

    snprintf(ht.name, sizeof(ht.name), "%s", name ? node_scalar(name) : code);
    ht.hp = node_int(hp, 0);
    ht.mp = node_int(mp, 0);
    ht.min_dmg = node_int(min_dmg, 0);
    ht.max_dmg = node_int(max_dmg, 0);
    ht.armor_penetration = node_int(armor_pen, 0);
    ht.accuracy = node_int(acc, 0);
    ht.armor = node_int(armor, 0);
    ht.skill_points = node_int(skill_points, 0);

    ht.weapon_options = node_string_list(weapon, &ht.weapon_count);
    ht.body_armor_options = node_string_list(body_armor, &ht.body_armor_count);
    ht.head_armor_options = node_string_list(head_armor, &ht.head_armor_count);
    ht.arms_armor_options = node_string_list(arms_armor, &ht.arms_armor_count);
    ht.shield_options = node_string_list(shield, &ht.shield_count);

    if (ht.weapon_count == 0) {
      ht.weapon_options = (char **)malloc(sizeof(char *));
      ht.weapon_options[0] = strdup_safe("without");
      ht.weapon_count = 1;
    }
    if (ht.body_armor_count == 0) {
      ht.body_armor_options = (char **)malloc(sizeof(char *));
      ht.body_armor_options[0] = strdup_safe("without");
      ht.body_armor_count = 1;
    }
    if (ht.head_armor_count == 0) {
      ht.head_armor_options = (char **)malloc(sizeof(char *));
      ht.head_armor_options[0] = strdup_safe("without");
      ht.head_armor_count = 1;
    }
    if (ht.arms_armor_count == 0) {
      ht.arms_armor_options = (char **)malloc(sizeof(char *));
      ht.arms_armor_options[0] = strdup_safe("without");
      ht.arms_armor_count = 1;
    }
    if (ht.shield_count == 0) {
      ht.shield_options = (char **)malloc(sizeof(char *));
      ht.shield_options[0] = strdup_safe("without");
      ht.shield_count = 1;
    }

    HeroTemplate *arr = (HeroTemplate *)realloc(heroes, (count + 1) * sizeof(HeroTemplate));
    if (!arr) continue;
    heroes = arr;
    heroes[count++] = ht;
  }

  node_free(root);
  *out = heroes;
  *out_count = count;
  return count > 0;
}

static bool load_enemies(const char *path, EnemyTemplate **out, size_t *out_count) {
  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }

  EnemyTemplate *enemies = NULL;
  size_t count = 0;

  for (size_t i = 0; i < root->map.len; ++i) {
    const char *code = root->map.keys[i];
    Node *e = root->map.values[i];
    if (!code || !e || e->type != NODE_MAP) continue;

    EnemyTemplate et = {0};
    snprintf(et.code, sizeof(et.code), "%s", code);
    et.is_boss = (strcmp(code, "boss") == 0);

    Node *code_name = node_map_get(e, "code_name");
    Node *name = node_map_get(e, "name");
    Node *hp = node_map_get(e, "hp");
    Node *min_dmg = node_map_get(e, "min_dmg");
    Node *max_dmg = node_map_get(e, "max_dmg");
    Node *armor_pen = node_map_get(e, "armor_penetration");
    Node *acc = node_map_get(e, "accurasy");
    Node *armor = node_map_get(e, "armor");
    Node *regen_hp = node_map_get(e, "regen_hp_base");
    Node *exp_gived = node_map_get(e, "exp_gived");
    Node *coins_gived = node_map_get(e, "coins_gived");
    Node *weapon = node_map_get(e, "weapon");
    Node *body_armor = node_map_get(e, "body_armor");
    Node *head_armor = node_map_get(e, "head_armor");
    Node *arms_armor = node_map_get(e, "arms_armor");
    Node *shield = node_map_get(e, "shield");
    Node *ingredients = node_map_get(e, "ingredients");

    snprintf(et.code_name, sizeof(et.code_name), "%s", code_name ? node_scalar(code_name) : code);
    snprintf(et.name, sizeof(et.name), "%s", name ? node_scalar(name) : et.code_name);
    et.hp = node_int(hp, 0);
    et.min_dmg = node_int(min_dmg, 0);
    et.max_dmg = node_int(max_dmg, 0);
    et.armor_penetration = node_int(armor_pen, 0);
    et.accuracy = node_int(acc, 0);
    et.armor = node_int(armor, 0);
    et.regen_hp = node_int(regen_hp, 0);
    et.exp_gived = node_int(exp_gived, 0);
    et.coins_gived = node_int(coins_gived, 0);

    et.weapon_options = node_string_list(weapon, &et.weapon_count);
    et.body_armor_options = node_string_list(body_armor, &et.body_armor_count);
    et.head_armor_options = node_string_list(head_armor, &et.head_armor_count);
    et.arms_armor_options = node_string_list(arms_armor, &et.arms_armor_count);
    et.shield_options = node_string_list(shield, &et.shield_count);
    et.ingredient_options = node_string_list(ingredients, &et.ingredient_count);

    if (et.weapon_count == 0) {
      et.weapon_options = (char **)malloc(sizeof(char *));
      et.weapon_options[0] = strdup_safe("without");
      et.weapon_count = 1;
    }
    if (et.body_armor_count == 0) {
      et.body_armor_options = (char **)malloc(sizeof(char *));
      et.body_armor_options[0] = strdup_safe("without");
      et.body_armor_count = 1;
    }
    if (et.head_armor_count == 0) {
      et.head_armor_options = (char **)malloc(sizeof(char *));
      et.head_armor_options[0] = strdup_safe("without");
      et.head_armor_count = 1;
    }
    if (et.arms_armor_count == 0) {
      et.arms_armor_options = (char **)malloc(sizeof(char *));
      et.arms_armor_options[0] = strdup_safe("without");
      et.arms_armor_count = 1;
    }
    if (et.shield_count == 0) {
      et.shield_options = (char **)malloc(sizeof(char *));
      et.shield_options[0] = strdup_safe("without");
      et.shield_count = 1;
    }
    if (et.ingredient_count == 0) {
      et.ingredient_options = (char **)malloc(sizeof(char *));
      et.ingredient_options[0] = strdup_safe("without");
      et.ingredient_count = 1;
    }

    EnemyTemplate *arr = (EnemyTemplate *)realloc(enemies, (count + 1) * sizeof(EnemyTemplate));
    if (!arr) continue;
    enemies = arr;
    enemies[count++] = et;
  }

  node_free(root);
  *out = enemies;
  *out_count = count;
  return count > 0;
}

static bool load_weapons(const char *path, WeaponItem **out, size_t *out_count) {
  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }
  WeaponItem *items = NULL;
  size_t count = 0;
  for (size_t i = 0; i < root->map.len; ++i) {
    const char *code = root->map.keys[i];
    Node *w = root->map.values[i];
    if (!code || !w || w->type != NODE_MAP) continue;
    WeaponItem it = {0};
    snprintf(it.code, sizeof(it.code), "%s", code);
    Node *name = node_map_get(w, "name");
    Node *min_dmg = node_map_get(w, "min_dmg");
    Node *max_dmg = node_map_get(w, "max_dmg");
    Node *acc = node_map_get(w, "accuracy");
    Node *block = node_map_get(w, "block_chance");
    Node *armor_pen = node_map_get(w, "armor_penetration");
    Node *price = node_map_get(w, "price");
    snprintf(it.name, sizeof(it.name), "%s", name ? node_scalar(name) : code);
    it.min_dmg = node_int(min_dmg, 0);
    it.max_dmg = node_int(max_dmg, 0);
    it.accuracy = node_int(acc, 0);
    it.block_chance = node_int(block, 0);
    it.armor_penetration = node_int(armor_pen, 0);
    it.price = node_int(price, 0);
    WeaponItem *arr = (WeaponItem *)realloc(items, (count + 1) * sizeof(WeaponItem));
    if (!arr) continue;
    items = arr;
    items[count++] = it;
  }
  node_free(root);
  *out = items;
  *out_count = count;
  return count > 0;
}

static bool load_armors(const char *path, ArmorItem **out, size_t *out_count) {
  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }
  ArmorItem *items = NULL;
  size_t count = 0;
  for (size_t i = 0; i < root->map.len; ++i) {
    const char *code = root->map.keys[i];
    Node *a = root->map.values[i];
    if (!code || !a || a->type != NODE_MAP) continue;
    ArmorItem it = {0};
    snprintf(it.code, sizeof(it.code), "%s", code);
    Node *name = node_map_get(a, "name");
    Node *armor = node_map_get(a, "armor");
    Node *acc = node_map_get(a, "accuracy");
    Node *price = node_map_get(a, "price");
    snprintf(it.name, sizeof(it.name), "%s", name ? node_scalar(name) : code);
    it.armor = node_int(armor, 0);
    it.accuracy = node_int(acc, 0);
    it.price = node_int(price, 0);
    ArmorItem *arr = (ArmorItem *)realloc(items, (count + 1) * sizeof(ArmorItem));
    if (!arr) continue;
    items = arr;
    items[count++] = it;
  }
  node_free(root);
  *out = items;
  *out_count = count;
  return count > 0;
}

static bool load_shields(const char *path, ShieldItem **out, size_t *out_count) {
  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }
  ShieldItem *items = NULL;
  size_t count = 0;
  for (size_t i = 0; i < root->map.len; ++i) {
    const char *code = root->map.keys[i];
    Node *s = root->map.values[i];
    if (!code || !s || s->type != NODE_MAP) continue;
    ShieldItem it = {0};
    snprintf(it.code, sizeof(it.code), "%s", code);
    Node *name = node_map_get(s, "name");
    Node *armor = node_map_get(s, "armor");
    Node *acc = node_map_get(s, "accuracy");
    Node *block = node_map_get(s, "block_chance");
    Node *min_dmg = node_map_get(s, "min_dmg");
    Node *max_dmg = node_map_get(s, "max_dmg");
    Node *price = node_map_get(s, "price");
    snprintf(it.name, sizeof(it.name), "%s", name ? node_scalar(name) : code);
    it.armor = node_int(armor, 0);
    it.accuracy = node_int(acc, 0);
    it.block_chance = node_int(block, 0);
    it.min_dmg = node_int(min_dmg, 0);
    it.max_dmg = node_int(max_dmg, 0);
    it.price = node_int(price, 0);
    ShieldItem *arr = (ShieldItem *)realloc(items, (count + 1) * sizeof(ShieldItem));
    if (!arr) continue;
    items = arr;
    items[count++] = it;
  }
  node_free(root);
  *out = items;
  *out_count = count;
  return count > 0;
}

static WeaponItem weapon_from_code(const Game *g, const char *code) {
  WeaponItem empty = {0};
  snprintf(empty.code, sizeof(empty.code), "%s", code ? code : "without");
  snprintf(empty.name, sizeof(empty.name), "%s", code ? code : "---");
  if (!g || !code) return empty;
  for (size_t i = 0; i < g->weapon_count; ++i) {
    if (strcmp(g->weapons[i].code, code) == 0) return g->weapons[i];
  }
  return empty;
}

static ArmorItem armor_from_code(const ArmorItem *items, size_t count, const char *code) {
  ArmorItem empty = {0};
  snprintf(empty.code, sizeof(empty.code), "%s", code ? code : "without");
  snprintf(empty.name, sizeof(empty.name), "%s", code ? code : "---");
  if (!items || !code) return empty;
  for (size_t i = 0; i < count; ++i) {
    if (strcmp(items[i].code, code) == 0) return items[i];
  }
  return empty;
}

static ShieldItem shield_from_code(const Game *g, const char *code) {
  ShieldItem empty = {0};
  snprintf(empty.code, sizeof(empty.code), "%s", code ? code : "without");
  snprintf(empty.name, sizeof(empty.name), "%s", code ? code : "---");
  if (!g || !code) return empty;
  for (size_t i = 0; i < g->shield_count; ++i) {
    if (strcmp(g->shields[i].code, code) == 0) return g->shields[i];
  }
  return empty;
}

static const char *pick_random_option(char **list, size_t count) {
  if (!list || count == 0) return "without";
  size_t idx = (size_t)rand_range(0, (int)count - 1);
  return list[idx] ? list[idx] : "without";
}

static void shop_init_default(ShopData *shop) {
  if (!shop) return;
  for (int i = 0; i < 3; ++i) {
    snprintf(shop->weapon[i], sizeof(shop->weapon[i]), "without");
    snprintf(shop->body_armor[i], sizeof(shop->body_armor[i]), "without");
    snprintf(shop->head_armor[i], sizeof(shop->head_armor[i]), "without");
    snprintf(shop->arms_armor[i], sizeof(shop->arms_armor[i]), "without");
    snprintf(shop->shield[i], sizeof(shop->shield[i]), "without");
  }
}

static void warehouse_init_default(WarehouseData *wh) {
  if (!wh) return;
  wh->coins = 0;
  snprintf(wh->weapon, sizeof(wh->weapon), "without");
  snprintf(wh->body_armor, sizeof(wh->body_armor), "without");
  snprintf(wh->head_armor, sizeof(wh->head_armor), "without");
  snprintf(wh->arms_armor, sizeof(wh->arms_armor), "without");
  snprintf(wh->shield, sizeof(wh->shield), "without");
}

static bool load_shop_data(ShopData *shop) {
  if (!shop) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/shop.yml", saves_dir);
  free(saves_dir);

  if (!file_exists(path)) {
    shop_init_default(shop);
    FILE *f = fopen(path, "w");
    if (f) {
      fprintf(f, "ammunition:\n");
      fprintf(f, "  weapon: [without, without, without]\n");
      fprintf(f, "  body_armor: [without, without, without]\n");
      fprintf(f, "  head_armor: [without, without, without]\n");
      fprintf(f, "  arms_armor: [without, without, without]\n");
      fprintf(f, "  shield: [without, without, without]\n");
      fclose(f);
    }
    return true;
  }

  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }
  Node *ammo = node_map_get(root, "ammunition");
  shop_init_default(shop);
  if (ammo && ammo->type == NODE_MAP) {
    const char *types[] = {"weapon", "body_armor", "head_armor", "arms_armor", "shield"};
    for (int t = 0; t < 5; ++t) {
      Node *seq = node_map_get(ammo, types[t]);
      if (!seq || seq->type != NODE_SEQ) continue;
      for (int i = 0; i < 3 && i < (int)seq->seq_len; ++i) {
        const char *code = node_scalar(seq->seq[i]);
        if (!code) code = "without";
        if (t == 0) snprintf(shop->weapon[i], sizeof(shop->weapon[i]), "%s", code);
        else if (t == 1) snprintf(shop->body_armor[i], sizeof(shop->body_armor[i]), "%s", code);
        else if (t == 2) snprintf(shop->head_armor[i], sizeof(shop->head_armor[i]), "%s", code);
        else if (t == 3) snprintf(shop->arms_armor[i], sizeof(shop->arms_armor[i]), "%s", code);
        else if (t == 4) snprintf(shop->shield[i], sizeof(shop->shield[i]), "%s", code);
      }
    }
  }
  node_free(root);
  return true;
}

static bool save_shop_data(const ShopData *shop) {
  if (!shop) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/shop.yml", saves_dir);
  free(saves_dir);
  FILE *f = fopen(path, "w");
  if (!f) return false;
  fprintf(f, "ammunition:\n");
  fprintf(f, "  weapon: [%s, %s, %s]\n", shop->weapon[0], shop->weapon[1], shop->weapon[2]);
  fprintf(f, "  body_armor: [%s, %s, %s]\n", shop->body_armor[0], shop->body_armor[1], shop->body_armor[2]);
  fprintf(f, "  head_armor: [%s, %s, %s]\n", shop->head_armor[0], shop->head_armor[1], shop->head_armor[2]);
  fprintf(f, "  arms_armor: [%s, %s, %s]\n", shop->arms_armor[0], shop->arms_armor[1], shop->arms_armor[2]);
  fprintf(f, "  shield: [%s, %s, %s]\n", shop->shield[0], shop->shield[1], shop->shield[2]);
  fclose(f);
  return true;
}

static bool load_warehouse_data(WarehouseData *wh) {
  if (!wh) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/warehouse.yml", saves_dir);
  free(saves_dir);

  if (!file_exists(path)) {
    warehouse_init_default(wh);
    FILE *f = fopen(path, "w");
    if (f) {
      fprintf(f, "coins: 0\n");
      fprintf(f, "weapon: without\n");
      fprintf(f, "body_armor: without\n");
      fprintf(f, "head_armor: without\n");
      fprintf(f, "arms_armor: without\n");
      fprintf(f, "shield: without\n");
      fclose(f);
    }
    return true;
  }

  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }
  warehouse_init_default(wh);
  wh->coins = node_map_int(root, "coins", 0);
  snprintf(wh->weapon, sizeof(wh->weapon), "%s", node_map_str(root, "weapon", "without"));
  snprintf(wh->body_armor, sizeof(wh->body_armor), "%s", node_map_str(root, "body_armor", "without"));
  snprintf(wh->head_armor, sizeof(wh->head_armor), "%s", node_map_str(root, "head_armor", "without"));
  snprintf(wh->arms_armor, sizeof(wh->arms_armor), "%s", node_map_str(root, "arms_armor", "without"));
  snprintf(wh->shield, sizeof(wh->shield), "%s", node_map_str(root, "shield", "without"));
  node_free(root);
  return true;
}

static bool save_warehouse_data(const WarehouseData *wh) {
  if (!wh) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/warehouse.yml", saves_dir);
  free(saves_dir);
  FILE *f = fopen(path, "w");
  if (!f) return false;
  fprintf(f, "coins: %d\n", wh->coins);
  fprintf(f, "weapon: %s\n", wh->weapon);
  fprintf(f, "body_armor: %s\n", wh->body_armor);
  fprintf(f, "head_armor: %s\n", wh->head_armor);
  fprintf(f, "arms_armor: %s\n", wh->arms_armor);
  fprintf(f, "shield: %s\n", wh->shield);
  fclose(f);
  return true;
}

static const char *shop_items_for_fill(const char *type, int idx) {
  static const char *weapon_items[] = {"stick", "knife", "club"};
  static const char *body_items[] = {"leather_jacket", "rusty_gambeson"};
  static const char *head_items[] = {"rusty_quilted_helmet", "leather_helmet"};
  static const char *arms_items[] = {"worn_gloves", "leather_gloves"};
  static const char *shield_items[] = {"holey_wicker_buckler", "braided_buckler", "wooden_buckler"};
  (void)idx;
  if (strcmp(type, "weapon") == 0) return weapon_items[rand_range(0, 2)];
  if (strcmp(type, "body_armor") == 0) return body_items[rand_range(0, 1)];
  if (strcmp(type, "head_armor") == 0) return head_items[rand_range(0, 1)];
  if (strcmp(type, "arms_armor") == 0) return arms_items[rand_range(0, 1)];
  if (strcmp(type, "shield") == 0) return shield_items[rand_range(0, 2)];
  return "without";
}

static void shop_fill(ShopData *shop) {
  if (!shop) return;
  const char *types[] = {"weapon", "body_armor", "head_armor", "arms_armor", "shield"};
  for (int t = 0; t < 5; ++t) {
    char (*arr)[32] = NULL;
    if (t == 0) arr = shop->weapon;
    else if (t == 1) arr = shop->body_armor;
    else if (t == 2) arr = shop->head_armor;
    else if (t == 3) arr = shop->arms_armor;
    else arr = shop->shield;
    int without_count = 0;
    for (int i = 0; i < 3; ++i) if (strcmp(arr[i], "without") == 0) without_count++;
    int n = without_count == 3 ? 2 : without_count == 2 ? 1 : 0;
    for (int k = 0; k < n; ++k) {
      int idx = -1;
      for (int i = 0; i < 3; ++i) {
        if (strcmp(arr[i], "without") == 0) { idx = i; break; }
      }
      if (idx >= 0) snprintf(arr[idx], sizeof(arr[idx]), "%s", shop_items_for_fill(types[t], idx));
    }
  }
}

static void monolith_init_default(MonolithData *m) {
  if (!m) return;
  memset(m, 0, sizeof(*m));
}

static bool load_monolith_data(MonolithData *m) {
  if (!m) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/pzdc_monolith.yml", saves_dir);
  free(saves_dir);

  if (!file_exists(path)) {
    monolith_init_default(m);
    FILE *f = fopen(path, "w");
    if (f) {
      fprintf(f, "points: 0\n");
      fprintf(f, "hp: 0\n");
      fprintf(f, "mp: 0\n");
      fprintf(f, "accuracy: 0\n");
      fprintf(f, "damage: 0\n");
      fprintf(f, "stat_points: 0\n");
      fprintf(f, "skill_points: 0\n");
      fprintf(f, "armor: 0\n");
      fprintf(f, "regen_hp: 0\n");
      fprintf(f, "regen_mp: 0\n");
      fprintf(f, "armor_penetration: 0\n");
      fprintf(f, "block_chance: 0\n");
      fclose(f);
    }
    return true;
  }

  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }
  monolith_init_default(m);
  m->points = node_map_int(root, "points", 0);
  m->hp = node_map_int(root, "hp", 0);
  m->mp = node_map_int(root, "mp", 0);
  m->accuracy = node_map_int(root, "accuracy", 0);
  m->damage = node_map_int(root, "damage", 0);
  m->stat_points = node_map_int(root, "stat_points", 0);
  m->skill_points = node_map_int(root, "skill_points", 0);
  m->armor = node_map_int(root, "armor", 0);
  m->regen_hp = node_map_int(root, "regen_hp", 0);
  m->regen_mp = node_map_int(root, "regen_mp", 0);
  m->armor_penetration = node_map_int(root, "armor_penetration", 0);
  m->block_chance = node_map_int(root, "block_chance", 0);
  node_free(root);
  return true;
}

static bool save_monolith_data(const MonolithData *m) {
  if (!m) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/pzdc_monolith.yml", saves_dir);
  free(saves_dir);
  FILE *f = fopen(path, "w");
  if (!f) return false;
  fprintf(f, "points: %d\n", m->points);
  fprintf(f, "hp: %d\n", m->hp);
  fprintf(f, "mp: %d\n", m->mp);
  fprintf(f, "accuracy: %d\n", m->accuracy);
  fprintf(f, "damage: %d\n", m->damage);
  fprintf(f, "stat_points: %d\n", m->stat_points);
  fprintf(f, "skill_points: %d\n", m->skill_points);
  fprintf(f, "armor: %d\n", m->armor);
  fprintf(f, "regen_hp: %d\n", m->regen_hp);
  fprintf(f, "regen_mp: %d\n", m->regen_mp);
  fprintf(f, "armor_penetration: %d\n", m->armor_penetration);
  fprintf(f, "block_chance: %d\n", m->block_chance);
  fclose(f);
  return true;
}

static int monolith_get_stat(const MonolithData *m, const char *key) {
  if (!m || !key) return 0;
  if (strcmp(key, "hp") == 0) return m->hp;
  if (strcmp(key, "mp") == 0) return m->mp;
  if (strcmp(key, "accuracy") == 0) return m->accuracy;
  if (strcmp(key, "damage") == 0) return m->damage;
  if (strcmp(key, "stat_points") == 0) return m->stat_points;
  if (strcmp(key, "skill_points") == 0) return m->skill_points;
  if (strcmp(key, "armor") == 0) return m->armor;
  if (strcmp(key, "regen_hp") == 0) return m->regen_hp;
  if (strcmp(key, "regen_mp") == 0) return m->regen_mp;
  if (strcmp(key, "armor_penetration") == 0) return m->armor_penetration;
  if (strcmp(key, "block_chance") == 0) return m->block_chance;
  return 0;
}

static int monolith_price_for(const MonolithData *m, const char *key) {
  if (!m || !key) return 0;
  const char *stats[] = {"hp","mp","accuracy","damage","stat_points","skill_points","armor","regen_hp","regen_mp","armor_penetration","block_chance"};
  const int base[] = {1,1,5,10,7,15,40,70,60,30,5};
  const double mult[] = {1.04,1.04,1.4,1.4,1.3,1.3,1.7,2.0,2.0,1.4,1.5};
  for (size_t i = 0; i < sizeof(stats) / sizeof(stats[0]); ++i) {
    if (strcmp(stats[i], key) == 0) {
      int cur = monolith_get_stat(m, key);
      double price = base[i] * pow(mult[i], cur);
      return (int)floor(price);
    }
  }
  return 0;
}

static bool monolith_buy(MonolithData *m, const char *key) {
  if (!m || !key) return false;
  int price = monolith_price_for(m, key);
  if (m->points < price || price <= 0) return false;
  m->points -= price;
  if (strcmp(key, "hp") == 0) m->hp += 1;
  else if (strcmp(key, "mp") == 0) m->mp += 1;
  else if (strcmp(key, "accuracy") == 0) m->accuracy += 1;
  else if (strcmp(key, "damage") == 0) m->damage += 1;
  else if (strcmp(key, "stat_points") == 0) m->stat_points += 1;
  else if (strcmp(key, "skill_points") == 0) m->skill_points += 1;
  else if (strcmp(key, "armor") == 0) m->armor += 1;
  else if (strcmp(key, "regen_hp") == 0) m->regen_hp += 1;
  else if (strcmp(key, "regen_mp") == 0) m->regen_mp += 1;
  else if (strcmp(key, "armor_penetration") == 0) m->armor_penetration += 1;
  else if (strcmp(key, "block_chance") == 0) m->block_chance += 1;
  return true;
}

static void statistics_total_init_default(StatisticsTotal *s) {
  if (!s) return;
  memset(s, 0, sizeof(*s));
}

static bool load_statistics_total(StatisticsTotal *s) {
  if (!s) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/statistics_total.yml", saves_dir);
  free(saves_dir);

  if (!file_exists(path)) {
    statistics_total_init_default(s);
    FILE *f = fopen(path, "w");
    if (f) {
      fprintf(f, "bandits:\n");
      fprintf(f, "  rabble: 0\n  rabid_dog: 0\n  poacher: 0\n  thug: 0\n  deserter: 0\n  bandit_leader: 0\n");
      fprintf(f, "undeads:\n");
      fprintf(f, "  zombie: 0\n  skeleton: 0\n  ghost: 0\n  fat_ghoul: 0\n  skeleton_soldier: 0\n  zombie_knight: 0\n");
      fprintf(f, "swamp:\n");
      fprintf(f, "  leech: 0\n  goblin: 0\n  sworm: 0\n  spider: 0\n  orc: 0\n  ancient_snail: 0\n");
      fprintf(f, "pzdc:\n");
      fprintf(f, "  stage_1_mimic: 0\n  stage_2_thing: 0\n  stage_3_dog: 0\n");
      fclose(f);
    }
    return true;
  }

  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }
  statistics_total_init_default(s);
  const char *bandits[] = {"rabble","rabid_dog","poacher","thug","deserter","bandit_leader"};
  const char *undeads[] = {"zombie","skeleton","ghost","fat_ghoul","skeleton_soldier","zombie_knight"};
  const char *swamp[] = {"leech","goblin","sworm","spider","orc","ancient_snail"};
  const char *pzdc[] = {"stage_1_mimic","stage_2_thing","stage_3_dog"};
  Node *b = node_map_get(root, "bandits");
  Node *u = node_map_get(root, "undeads");
  Node *w = node_map_get(root, "swamp");
  Node *p = node_map_get(root, "pzdc");
  for (int i = 0; i < 6; ++i) {
    s->bandits[i] = b ? node_map_int(b, bandits[i], 0) : 0;
    s->undeads[i] = u ? node_map_int(u, undeads[i], 0) : 0;
    s->swamp[i] = w ? node_map_int(w, swamp[i], 0) : 0;
  }
  for (int i = 0; i < 3; ++i) {
    s->pzdc[i] = p ? node_map_int(p, pzdc[i], 0) : 0;
  }
  node_free(root);
  return true;
}

static bool save_statistics_total(const StatisticsTotal *s) {
  if (!s) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/statistics_total.yml", saves_dir);
  free(saves_dir);
  FILE *f = fopen(path, "w");
  if (!f) return false;
  fprintf(f, "bandits:\n");
  fprintf(f, "  rabble: %d\n  rabid_dog: %d\n  poacher: %d\n  thug: %d\n  deserter: %d\n  bandit_leader: %d\n",
          s->bandits[0], s->bandits[1], s->bandits[2], s->bandits[3], s->bandits[4], s->bandits[5]);
  fprintf(f, "undeads:\n");
  fprintf(f, "  zombie: %d\n  skeleton: %d\n  ghost: %d\n  fat_ghoul: %d\n  skeleton_soldier: %d\n  zombie_knight: %d\n",
          s->undeads[0], s->undeads[1], s->undeads[2], s->undeads[3], s->undeads[4], s->undeads[5]);
  fprintf(f, "swamp:\n");
  fprintf(f, "  leech: %d\n  goblin: %d\n  sworm: %d\n  spider: %d\n  orc: %d\n  ancient_snail: %d\n",
          s->swamp[0], s->swamp[1], s->swamp[2], s->swamp[3], s->swamp[4], s->swamp[5]);
  fprintf(f, "pzdc:\n");
  fprintf(f, "  stage_1_mimic: %d\n  stage_2_thing: %d\n  stage_3_dog: %d\n", s->pzdc[0], s->pzdc[1], s->pzdc[2]);
  fclose(f);
  return true;
}

static int stats_total_get(const StatisticsTotal *s, const char *dungeon, const char *enemy_code) {
  if (!s || !dungeon || !enemy_code) return 0;
  const char *bandits[] = {"rabble","rabid_dog","poacher","thug","deserter","bandit_leader"};
  const char *undeads[] = {"zombie","skeleton","ghost","fat_ghoul","skeleton_soldier","zombie_knight"};
  const char *swamp[] = {"leech","goblin","sworm","spider","orc","ancient_snail"};
  const char *pzdc[] = {"stage_1_mimic","stage_2_thing","stage_3_dog"};
  if (strcmp(dungeon, "bandits") == 0) {
    for (int i = 0; i < 6; ++i) if (strcmp(bandits[i], enemy_code) == 0) return s->bandits[i];
  } else if (strcmp(dungeon, "undeads") == 0) {
    for (int i = 0; i < 6; ++i) if (strcmp(undeads[i], enemy_code) == 0) return s->undeads[i];
  } else if (strcmp(dungeon, "swamp") == 0) {
    for (int i = 0; i < 6; ++i) if (strcmp(swamp[i], enemy_code) == 0) return s->swamp[i];
  } else if (strcmp(dungeon, "pzdc") == 0) {
    for (int i = 0; i < 3; ++i) if (strcmp(pzdc[i], enemy_code) == 0) return s->pzdc[i];
  }
  return 0;
}

static void stats_total_increment(StatisticsTotal *s, const char *dungeon, const char *enemy_code) {
  if (!s || !dungeon || !enemy_code) return;
  const char *bandits[] = {"rabble","rabid_dog","poacher","thug","deserter","bandit_leader"};
  const char *undeads[] = {"zombie","skeleton","ghost","fat_ghoul","skeleton_soldier","zombie_knight"};
  const char *swamp[] = {"leech","goblin","sworm","spider","orc","ancient_snail"};
  const char *pzdc[] = {"stage_1_mimic","stage_2_thing","stage_3_dog"};
  if (strcmp(dungeon, "bandits") == 0) {
    for (int i = 0; i < 6; ++i) if (strcmp(bandits[i], enemy_code) == 0) { s->bandits[i] += 1; return; }
  } else if (strcmp(dungeon, "undeads") == 0) {
    for (int i = 0; i < 6; ++i) if (strcmp(undeads[i], enemy_code) == 0) { s->undeads[i] += 1; return; }
  } else if (strcmp(dungeon, "swamp") == 0) {
    for (int i = 0; i < 6; ++i) if (strcmp(swamp[i], enemy_code) == 0) { s->swamp[i] += 1; return; }
  } else if (strcmp(dungeon, "pzdc") == 0) {
    for (int i = 0; i < 3; ++i) if (strcmp(pzdc[i], enemy_code) == 0) { s->pzdc[i] += 1; return; }
  }
}

static void occult_library_free(OccultLibraryData *ol) {
  if (!ol) return;
  for (size_t i = 0; i < ol->recipe_count; ++i) {
    free(ol->recipes[i].ingredients);
  }
  free(ol->recipes);
  ol->recipes = NULL;
  ol->recipe_count = 0;
}

static void parse_recipe_effect(Node *node, RecipeEffect *out) {
  if (!out) return;
  memset(out, 0, sizeof(*out));
  if (!node || node->type != NODE_MAP) return;
  out->accuracy = node_map_int(node, "accuracy", 0);
  out->min_dmg = node_map_int(node, "min_dmg", 0);
  out->max_dmg = node_map_int(node, "max_dmg", 0);
  out->block_chance = node_map_int(node, "block_chance", 0);
  out->armor = node_map_int(node, "armor", 0);
  out->armor_penetration = node_map_int(node, "armor_penetration", 0);
}

static bool load_occult_library_data(OccultLibraryData *ol) {
  if (!ol) return false;
  occult_library_free(ol);
  char *data_path = resolve_data_path("data/camp/occult_library.yml");
  if (!data_path) return false;
  Node *root = yaml_load_file(data_path);
  free(data_path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }

  OccultRecipe *recipes = NULL;
  size_t count = 0;
  for (size_t i = 0; i < root->map.len; ++i) {
    const char *code = root->map.keys[i];
    Node *r = root->map.values[i];
    if (!code || !r || r->type != NODE_MAP) continue;
    OccultRecipe rec;
    memset(&rec, 0, sizeof(rec));
    snprintf(rec.code, sizeof(rec.code), "%s", code);
    rec.view_code = node_map_int(r, "view_code", 0);
    snprintf(rec.name, sizeof(rec.name), "%s", node_map_str(r, "name", code));
    rec.price = node_map_int(r, "price", 0);

    Node *recipe_map = node_map_get(r, "recipe");
    if (recipe_map && recipe_map->type == NODE_MAP) {
      rec.ingredients = (RecipeIngredient *)calloc(recipe_map->map.len, sizeof(RecipeIngredient));
      if (rec.ingredients) {
        rec.ingredient_count = recipe_map->map.len;
        for (size_t j = 0; j < recipe_map->map.len; ++j) {
          const char *k = recipe_map->map.keys[j];
          int v = node_int(recipe_map->map.values[j], 0);
          if (k) snprintf(rec.ingredients[j].name, sizeof(rec.ingredients[j].name), "%s", k);
          rec.ingredients[j].count = v;
        }
      }
    }

    Node *effect = node_map_get(r, "effect");
    if (effect && effect->type == NODE_MAP) {
      parse_recipe_effect(node_map_get(effect, "weapon"), &rec.weapon);
      parse_recipe_effect(node_map_get(effect, "head_armor"), &rec.head_armor);
      parse_recipe_effect(node_map_get(effect, "body_armor"), &rec.body_armor);
      parse_recipe_effect(node_map_get(effect, "arms_armor"), &rec.arms_armor);
      parse_recipe_effect(node_map_get(effect, "shield"), &rec.shield);
    }

    OccultRecipe *arr = (OccultRecipe *)realloc(recipes, (count + 1) * sizeof(OccultRecipe));
    if (!arr) {
      free(rec.ingredients);
      continue;
    }
    recipes = arr;
    recipes[count++] = rec;
  }
  node_free(root);
  ol->recipes = recipes;
  ol->recipe_count = count;

  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return true;
  char path[512];
  snprintf(path, sizeof(path), "%s/occult_library.yml", saves_dir);
  free(saves_dir);
  if (!file_exists(path)) {
    FILE *f = fopen(path, "w");
    if (f) {
      for (size_t i = 0; i < ol->recipe_count; ++i) {
        fprintf(f, "%s: false\n", ol->recipes[i].code);
      }
      fclose(f);
    }
    return true;
  }
  Node *saved = yaml_load_file(path);
  if (!saved || saved->type != NODE_MAP) {
    node_free(saved);
    return true;
  }
  for (size_t i = 0; i < ol->recipe_count; ++i) {
    const char *val = node_map_str(saved, ol->recipes[i].code, "false");
    ol->recipes[i].purchased = (strcmp(val, "true") == 0 || strcmp(val, "1") == 0);
  }
  node_free(saved);
  return true;
}

static bool save_occult_library_data(const OccultLibraryData *ol) {
  if (!ol) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/occult_library.yml", saves_dir);
  free(saves_dir);
  FILE *f = fopen(path, "w");
  if (!f) return false;
  for (size_t i = 0; i < ol->recipe_count; ++i) {
    fprintf(f, "%s: %s\n", ol->recipes[i].code, ol->recipes[i].purchased ? "true" : "false");
  }
  fclose(f);
  return true;
}

static OccultRecipe *occult_recipe_by_view_code(OccultLibraryData *ol, int view_code) {
  if (!ol || view_code <= 0) return NULL;
  for (size_t i = 0; i < ol->recipe_count; ++i) {
    if (ol->recipes[i].view_code == view_code) return &ol->recipes[i];
  }
  return NULL;
}

static OccultRecipe *occult_recipe_by_code(OccultLibraryData *ol, const char *code) {
  if (!ol || !code) return NULL;
  for (size_t i = 0; i < ol->recipe_count; ++i) {
    if (strcmp(ol->recipes[i].code, code) == 0) return &ol->recipes[i];
  }
  return NULL;
}

static void titleize_token(const char *in, char *out, size_t out_sz) {
  if (!out || out_sz == 0) return;
  if (!in) { snprintf(out, out_sz, "---"); return; }
  size_t n = strlen(in);
  if (n + 1 > out_sz) n = out_sz - 1;
  for (size_t i = 0; i < n; ++i) {
    char ch = in[i];
    if (ch == '_') ch = ' ';
    out[i] = (i == 0) ? (char)toupper((unsigned char)ch) : ch;
  }
  out[n] = '\0';
}

static void append_kv(char *out, size_t out_sz, const char *name, int val, bool *first) {
  if (!out || out_sz == 0 || !name) return;
  char buf[128];
  snprintf(buf, sizeof(buf), "%s: %d", name, val);
  if (*first) {
    snprintf(out, out_sz, "%s", buf);
    *first = false;
  } else {
    size_t len = strlen(out);
    snprintf(out + len, out_sz - len, ";   %s", buf);
  }
}

static void format_recipe_ingredients(const OccultRecipe *r, char *out, size_t out_sz) {
  if (!out || out_sz == 0) return;
  out[0] = '\0';
  if (!r || r->ingredient_count == 0) return;
  bool first = true;
  for (size_t i = 0; i < r->ingredient_count; ++i) {
    char name_buf[64];
    titleize_token(r->ingredients[i].name, name_buf, sizeof(name_buf));
    append_kv(out, out_sz, name_buf, r->ingredients[i].count, &first);
  }
}

static void format_effect(const RecipeEffect *e, char *out, size_t out_sz) {
  if (!out || out_sz == 0) return;
  out[0] = '\0';
  if (!e) return;
  bool first = true;
  if (e->accuracy) append_kv(out, out_sz, "Accuracy", e->accuracy, &first);
  if (e->min_dmg) append_kv(out, out_sz, "Min dmg", e->min_dmg, &first);
  if (e->max_dmg) append_kv(out, out_sz, "Max dmg", e->max_dmg, &first);
  if (e->block_chance) append_kv(out, out_sz, "Block chance", e->block_chance, &first);
  if (e->armor) append_kv(out, out_sz, "Armor", e->armor, &first);
  if (e->armor_penetration) append_kv(out, out_sz, "Armor penetration", e->armor_penetration, &first);
}

static bool recipe_hero_has_ingredients(const OccultRecipe *r, const Character *hero) {
  if (!r || !hero) return false;
  for (size_t i = 0; i < r->ingredient_count; ++i) {
    int have = value_map_get_int(&hero->ingredients, r->ingredients[i].name, 0);
    if (have < r->ingredients[i].count) return false;
  }
  return true;
}

static void recipe_consume_ingredients(const OccultRecipe *r, Character *hero) {
  if (!r || !hero) return;
  for (size_t i = 0; i < r->ingredient_count; ++i) {
    int have = value_map_get_int(&hero->ingredients, r->ingredients[i].name, 0);
    int left = have - r->ingredients[i].count;
    if (left < 0) left = 0;
    value_map_set_int(&hero->ingredients, r->ingredients[i].name, left);
  }
}

static void recipe_apply_weapon(const OccultRecipe *r, WeaponItem *w) {
  if (!r || !w || strcmp(w->code, "without") == 0) return;
  w->enhanced = true;
  snprintf(w->enhance_name, sizeof(w->enhance_name), "%s", r->code);
  w->enhance_accuracy += r->weapon.accuracy;
  w->enhance_min_dmg += r->weapon.min_dmg;
  w->enhance_max_dmg += r->weapon.max_dmg;
  w->enhance_block_chance += r->weapon.block_chance;
  w->enhance_armor_penetration += r->weapon.armor_penetration;
}

static void recipe_apply_armor(const OccultRecipe *r, ArmorItem *a, const RecipeEffect *e) {
  if (!r || !a || !e || strcmp(a->code, "without") == 0) return;
  a->enhanced = true;
  snprintf(a->enhance_name, sizeof(a->enhance_name), "%s", r->code);
  a->enhance_accuracy += e->accuracy;
  a->enhance_armor += e->armor;
}

static void recipe_apply_shield(const OccultRecipe *r, ShieldItem *s) {
  if (!r || !s || strcmp(s->code, "without") == 0) return;
  s->enhanced = true;
  snprintf(s->enhance_name, sizeof(s->enhance_name), "%s", r->code);
  s->enhance_accuracy += r->shield.accuracy;
  s->enhance_armor += r->shield.armor;
  s->enhance_block_chance += r->shield.block_chance;
  s->enhance_min_dmg += r->shield.min_dmg;
  s->enhance_max_dmg += r->shield.max_dmg;
}

static int compare_recipe_code(const void *a, const void *b, void *ctx) {
  const OccultLibraryData *ol = (const OccultLibraryData *)ctx;
  size_t ia = *(const size_t *)a;
  size_t ib = *(const size_t *)b;
  if (!ol) return 0;
  return strcmp(ol->recipes[ia].code, ol->recipes[ib].code);
}

static size_t *occult_accessible_indices(const OccultLibraryData *ol, size_t *out_count) {
  if (out_count) *out_count = 0;
  if (!ol) return NULL;
  size_t *indices = (size_t *)calloc(ol->recipe_count, sizeof(size_t));
  if (!indices) return NULL;
  size_t count = 0;
  for (size_t i = 0; i < ol->recipe_count; ++i) {
    if (ol->recipes[i].purchased) indices[count++] = i;
  }
  if (count > 1) {
    qsort_r(indices, count, sizeof(size_t), compare_recipe_code, (void *)ol);
  }
  if (out_count) *out_count = count;
  return indices;
}

static bool save_hero_in_run(const Game *g) {
  if (!g) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/hero_in_run.yml", saves_dir);
  free(saves_dir);

  FILE *f = fopen(path, "w");
  if (!f) return false;

  const Character *h = &g->hero;
  fprintf(f, "hero_create:\n");
  fprintf(f, "  name: ");
  yaml_write_escaped(f, h->name);
  fprintf(f, "\n");
  fprintf(f, "  background: %s\n", h->background[0] ? h->background : h->code);

  fprintf(f, "hero_stats:\n");
  fprintf(f, "  hp: %d\n", h->hp);
  fprintf(f, "  hp_max: %d\n", h->hp_max);
  fprintf(f, "  regen_hp_base: %d\n", h->regen_hp_base);
  fprintf(f, "  mp: %d\n", h->mp);
  fprintf(f, "  mp_max: %d\n", h->mp_max);
  fprintf(f, "  regen_mp_base: %d\n", h->regen_mp_base);
  fprintf(f, "  min_dmg_base: %d\n", h->min_dmg_base);
  fprintf(f, "  max_dmg_base: %d\n", h->max_dmg_base);
  fprintf(f, "  accuracy_base: %d\n", h->accuracy_base);
  fprintf(f, "  armor_base: %d\n", h->armor_base);
  fprintf(f, "  block_chance_base: %d\n", h->block_chance_base);
  fprintf(f, "  armor_penetration_base: %d\n", h->armor_penetration_base);
  fprintf(f, "  exp: %d\n", h->exp);
  fprintf(f, "  lvl: %d\n", h->lvl);
  fprintf(f, "  stat_points: %d\n", h->stat_points);
  fprintf(f, "  skill_points: %d\n", h->skill_points);

  fprintf(f, "hero_skills:\n");
  fprintf(f, "  active_skill:\n");
  fprintf(f, "    code: %s\n", h->active_skill.code);
  fprintf(f, "    lvl: %d\n", h->active_skill.lvl);
  fprintf(f, "  passive_skill:\n");
  fprintf(f, "    code: %s\n", h->passive_skill.code);
  fprintf(f, "    lvl: %d\n", h->passive_skill.lvl);
  fprintf(f, "  camp_skill:\n");
  fprintf(f, "    code: %s\n", h->camp_skill.code);
  fprintf(f, "    lvl: %d\n", h->camp_skill.lvl);

  fprintf(f, "hero_ammunition:\n");
  fprintf(f, "  weapon:\n");
  fprintf(f, "    code: %s\n", h->weapon.code);
  fprintf(f, "    enhance_code: ");
  yaml_write_escaped(f, h->weapon.enhanced ? h->weapon.enhance_name : "");
  fprintf(f, "\n");
  fprintf(f, "  body_armor:\n");
  fprintf(f, "    code: %s\n", h->body_armor.code);
  fprintf(f, "    enhance_code: ");
  yaml_write_escaped(f, h->body_armor.enhanced ? h->body_armor.enhance_name : "");
  fprintf(f, "\n");
  fprintf(f, "  head_armor:\n");
  fprintf(f, "    code: %s\n", h->head_armor.code);
  fprintf(f, "    enhance_code: ");
  yaml_write_escaped(f, h->head_armor.enhanced ? h->head_armor.enhance_name : "");
  fprintf(f, "\n");
  fprintf(f, "  arms_armor:\n");
  fprintf(f, "    code: %s\n", h->arms_armor.code);
  fprintf(f, "    enhance_code: ");
  yaml_write_escaped(f, h->arms_armor.enhanced ? h->arms_armor.enhance_name : "");
  fprintf(f, "\n");
  fprintf(f, "  shield:\n");
  fprintf(f, "    code: %s\n", h->shield.code);
  fprintf(f, "    enhance_code: ");
  yaml_write_escaped(f, h->shield.enhanced ? h->shield.enhance_name : "");
  fprintf(f, "\n");

  fprintf(f, "dungeon_name: %s\n", h->dungeon_name[0] ? h->dungeon_name : g->dungeons[g->dungeon_index].name);
  fprintf(f, "dungeon_part_number: %d\n", h->dungeon_part_number);
  fprintf(f, "leveling: %d\n", h->leveling);

  fprintf(f, "camp_loot:\n");
  fprintf(f, "  pzdc_monolith_points: %d\n", h->pzdc_monolith_points);
  fprintf(f, "  coins: %d\n", h->coins);

  fprintf(f, "ingredients:\n");
  if (h->ingredients.count == 0) {
    fprintf(f, "  {}\n");
  } else {
    for (size_t i = 0; i < h->ingredients.count; ++i) {
      fprintf(f, "  %s: %s\n", h->ingredients.items[i].key, h->ingredients.items[i].value);
    }
  }
  if (g->wg_taken) {
    fprintf(f, "events_data:\n");
    fprintf(f, "  wariors_grave:\n");
    fprintf(f, "    taken: 1\n");
    fprintf(f, "    enemy: %s\n", g->wg_enemy[0] ? g->wg_enemy : "poacher");
    fprintf(f, "    count: %d\n", g->wg_count);
    fprintf(f, "    level: %d\n", g->wg_level);
  } else {
    fprintf(f, "events_data: {}\n");
  }

  fclose(f);
  return true;
}

static bool load_hero_in_run(Game *g) {
  if (!g) return false;
  char *saves_dir = resolve_saves_dir();
  if (!saves_dir) return false;
  char path[512];
  snprintf(path, sizeof(path), "%s/hero_in_run.yml", saves_dir);
  free(saves_dir);

  Node *root = yaml_load_file(path);
  if (!root || root->type != NODE_MAP) {
    node_free(root);
    return false;
  }

  Node *hero_create = node_map_get(root, "hero_create");
  const char *name = node_map_str(hero_create, "name", "Hero");
  const char *background = node_map_str(hero_create, "background", "passerby");
  const HeroTemplate *tmpl = hero_template_by_code(g, background);
  if (!tmpl) tmpl = (g->hero_count > 0) ? &g->heroes[0] : NULL;
  if (!tmpl) {
    node_free(root);
    return false;
  }

  g->hero = character_from_hero(g, tmpl, name);
  snprintf(g->hero.background, sizeof(g->hero.background), "%s", background);

  Node *hero_stats = node_map_get(root, "hero_stats");
  g->hero.hp = node_map_int(hero_stats, "hp", g->hero.hp);
  g->hero.hp_max = node_map_int(hero_stats, "hp_max", g->hero.hp_max);
  g->hero.regen_hp_base = node_map_int(hero_stats, "regen_hp_base", g->hero.regen_hp_base);
  g->hero.mp = node_map_int(hero_stats, "mp", g->hero.mp);
  g->hero.mp_max = node_map_int(hero_stats, "mp_max", g->hero.mp_max);
  g->hero.regen_mp_base = node_map_int(hero_stats, "regen_mp_base", g->hero.regen_mp_base);
  g->hero.min_dmg_base = node_map_int(hero_stats, "min_dmg_base", g->hero.min_dmg_base);
  g->hero.max_dmg_base = node_map_int(hero_stats, "max_dmg_base", g->hero.max_dmg_base);
  g->hero.accuracy_base = node_map_int(hero_stats, "accuracy_base", g->hero.accuracy_base);
  g->hero.armor_base = node_map_int(hero_stats, "armor_base", g->hero.armor_base);
  g->hero.block_chance_base = node_map_int(hero_stats, "block_chance_base", g->hero.block_chance_base);
  g->hero.armor_penetration_base = node_map_int(hero_stats, "armor_penetration_base", g->hero.armor_penetration_base);
  g->hero.exp = node_map_int(hero_stats, "exp", g->hero.exp);
  g->hero.lvl = node_map_int(hero_stats, "lvl", g->hero.lvl);
  g->hero.stat_points = node_map_int(hero_stats, "stat_points", g->hero.stat_points);
  g->hero.skill_points = node_map_int(hero_stats, "skill_points", g->hero.skill_points);

  Node *hero_skills = node_map_get(root, "hero_skills");
  Node *active = node_map_get(hero_skills, "active_skill");
  Node *passive = node_map_get(hero_skills, "passive_skill");
  Node *camp = node_map_get(hero_skills, "camp_skill");
  skill_assign(&g->hero.active_skill, SKILL_ACTIVE, node_map_str(active, "code", "none"));
  g->hero.active_skill.lvl = node_map_int(active, "lvl", 0);
  skill_assign(&g->hero.passive_skill, SKILL_PASSIVE, node_map_str(passive, "code", "none"));
  g->hero.passive_skill.lvl = node_map_int(passive, "lvl", 0);
  skill_assign(&g->hero.camp_skill, SKILL_CAMP, node_map_str(camp, "code", "none"));
  g->hero.camp_skill.lvl = node_map_int(camp, "lvl", 0);

  Node *hero_ammo = node_map_get(root, "hero_ammunition");
  Node *w = node_map_get(hero_ammo, "weapon");
  Node *b = node_map_get(hero_ammo, "body_armor");
  Node *h = node_map_get(hero_ammo, "head_armor");
  Node *a = node_map_get(hero_ammo, "arms_armor");
  Node *s = node_map_get(hero_ammo, "shield");
  g->hero.weapon = weapon_from_code(g, node_map_str(w, "code", "without"));
  g->hero.body_armor = armor_from_code(g->body_armors, g->body_armor_count, node_map_str(b, "code", "without"));
  g->hero.head_armor = armor_from_code(g->head_armors, g->head_armor_count, node_map_str(h, "code", "without"));
  g->hero.arms_armor = armor_from_code(g->arms_armors, g->arms_armor_count, node_map_str(a, "code", "without"));
  g->hero.shield = shield_from_code(g, node_map_str(s, "code", "without"));
  const char *w_enh = node_map_str(w, "enhance_code", "");
  const char *b_enh = node_map_str(b, "enhance_code", "");
  const char *h_enh = node_map_str(h, "enhance_code", "");
  const char *a_enh = node_map_str(a, "enhance_code", "");
  const char *s_enh = node_map_str(s, "enhance_code", "");
  if (w_enh && w_enh[0]) {
    OccultRecipe *r = occult_recipe_by_code(&g->occult, w_enh);
    if (r) recipe_apply_weapon(r, &g->hero.weapon);
  }
  if (b_enh && b_enh[0]) {
    OccultRecipe *r = occult_recipe_by_code(&g->occult, b_enh);
    if (r) recipe_apply_armor(r, &g->hero.body_armor, &r->body_armor);
  }
  if (h_enh && h_enh[0]) {
    OccultRecipe *r = occult_recipe_by_code(&g->occult, h_enh);
    if (r) recipe_apply_armor(r, &g->hero.head_armor, &r->head_armor);
  }
  if (a_enh && a_enh[0]) {
    OccultRecipe *r = occult_recipe_by_code(&g->occult, a_enh);
    if (r) recipe_apply_armor(r, &g->hero.arms_armor, &r->arms_armor);
  }
  if (s_enh && s_enh[0]) {
    OccultRecipe *r = occult_recipe_by_code(&g->occult, s_enh);
    if (r) recipe_apply_shield(r, &g->hero.shield);
  }

  const char *dungeon_name = node_map_str(root, "dungeon_name", g->dungeons[g->dungeon_index].name);
  snprintf(g->hero.dungeon_name, sizeof(g->hero.dungeon_name), "%s", dungeon_name);
  g->hero.dungeon_part_number = node_map_int(root, "dungeon_part_number", 0);
  g->hero.leveling = node_map_int(root, "leveling", 0);

  Node *camp_loot = node_map_get(root, "camp_loot");
  g->hero.pzdc_monolith_points = node_map_int(camp_loot, "pzdc_monolith_points", 0);
  g->hero.coins = node_map_int(camp_loot, "coins", 0);

  value_map_clear(&g->hero.ingredients);
  Node *ingredients = node_map_get(root, "ingredients");
  if (ingredients && ingredients->type == NODE_MAP) {
    for (size_t i = 0; i < ingredients->map.len; ++i) {
      const char *k = ingredients->map.keys[i];
      const char *v = node_scalar(ingredients->map.values[i]);
      if (k && v) value_map_set(&g->hero.ingredients, k, v);
    }
  }

  g->wg_taken = 0;
  g->wg_enemy[0] = '\0';
  g->wg_count = 0;
  g->wg_level = 0;
  Node *events_data = node_map_get(root, "events_data");
  if (events_data && events_data->type == NODE_MAP) {
    Node *wg = node_map_get(events_data, "wariors_grave");
    if (wg && wg->type == NODE_MAP) {
      g->wg_taken = node_map_int(wg, "taken", 0);
      const char *enemy = node_map_str(wg, "enemy", "");
      if (enemy) snprintf(g->wg_enemy, sizeof(g->wg_enemy), "%s", enemy);
      g->wg_count = node_map_int(wg, "count", 0);
      g->wg_level = node_map_int(wg, "level", 0);
    }
  }

  node_free(root);
  return true;
}

static int rand_range(int min, int max) {
  if (max < min) return min;
  return min + rand() % (max - min + 1);
}

static const char *weapon_name_from_code(const Game *g, const char *code) {
  if (!g || !code) return "---";
  for (size_t i = 0; i < g->weapon_count; ++i) {
    if (strcmp(g->weapons[i].code, code) == 0) return g->weapons[i].name;
  }
  if (strcmp(code, "without") == 0) return "without";
  return code;
}

static const char *armor_name_from_code(const ArmorItem *items, size_t count, const char *code) {
  if (!items || !code) return "---";
  for (size_t i = 0; i < count; ++i) {
    if (strcmp(items[i].code, code) == 0) return items[i].name;
  }
  if (strcmp(code, "without") == 0) return "without";
  return code;
}

static const char *shield_name_from_code(const Game *g, const char *code) {
  if (!g || !code) return "---";
  for (size_t i = 0; i < g->shield_count; ++i) {
    if (strcmp(g->shields[i].code, code) == 0) return g->shields[i].name;
  }
  if (strcmp(code, "without") == 0) return "without";
  return code;
}

static const char *ammo_name(Game *g, const char *type, const char *code) {
  if (!g || !type || !code) return "---";
  if (strcmp(type, "weapon") == 0) return weapon_name_from_code(g, code);
  if (strcmp(type, "body_armor") == 0) return armor_name_from_code(g->body_armors, g->body_armor_count, code);
  if (strcmp(type, "head_armor") == 0) return armor_name_from_code(g->head_armors, g->head_armor_count, code);
  if (strcmp(type, "arms_armor") == 0) return armor_name_from_code(g->arms_armors, g->arms_armor_count, code);
  if (strcmp(type, "shield") == 0) return shield_name_from_code(g, code);
  return "---";
}

static int ammo_price(Game *g, const char *type, const char *code) {
  if (!g || !type || !code) return 0;
  if (strcmp(code, "without") == 0) return 0;
  if (strcmp(type, "weapon") == 0) return weapon_from_code(g, code).price;
  if (strcmp(type, "body_armor") == 0) return armor_from_code(g->body_armors, g->body_armor_count, code).price;
  if (strcmp(type, "head_armor") == 0) return armor_from_code(g->head_armors, g->head_armor_count, code).price;
  if (strcmp(type, "arms_armor") == 0) return armor_from_code(g->arms_armors, g->arms_armor_count, code).price;
  if (strcmp(type, "shield") == 0) return shield_from_code(g, code).price;
  return 0;
}

static void ammo_to_map(Game *g, const char *type, const char *code, ValueMap *map) {
  value_map_clear(map);
  if (!g || !type || !code) return;
  if (strcmp(type, "weapon") == 0) {
    WeaponItem it = weapon_from_code(g, code);
    char buf[32];
    char name_buf[96];
    ammo_display_name(it.name, it.enhanced, name_buf, sizeof(name_buf));
    value_map_set(map, "name", name_buf);
    snprintf(buf, sizeof(buf), "%d", it.min_dmg); value_map_set(map, "min_dmg", buf);
    snprintf(buf, sizeof(buf), "%d", it.max_dmg); value_map_set(map, "max_dmg", buf);
    snprintf(buf, sizeof(buf), "%d", it.accuracy); value_map_set(map, "accuracy", buf);
    snprintf(buf, sizeof(buf), "%d", it.block_chance); value_map_set(map, "block_chance", buf);
    snprintf(buf, sizeof(buf), "%d", it.armor_penetration); value_map_set(map, "armor_penetration", buf);
    snprintf(buf, sizeof(buf), "%d", it.price); value_map_set(map, "price", buf);
  } else if (strcmp(type, "shield") == 0) {
    ShieldItem it = shield_from_code(g, code);
    char buf[32];
    char name_buf[96];
    ammo_display_name(it.name, it.enhanced, name_buf, sizeof(name_buf));
    value_map_set(map, "name", name_buf);
    snprintf(buf, sizeof(buf), "%d", it.armor); value_map_set(map, "armor", buf);
    snprintf(buf, sizeof(buf), "%d", it.accuracy); value_map_set(map, "accuracy", buf);
    snprintf(buf, sizeof(buf), "%d", it.block_chance); value_map_set(map, "block_chance", buf);
    snprintf(buf, sizeof(buf), "%d", it.min_dmg); value_map_set(map, "min_dmg", buf);
    snprintf(buf, sizeof(buf), "%d", it.max_dmg); value_map_set(map, "max_dmg", buf);
    snprintf(buf, sizeof(buf), "%d", it.price); value_map_set(map, "price", buf);
  } else {
    ArmorItem it;
    if (strcmp(type, "body_armor") == 0) it = armor_from_code(g->body_armors, g->body_armor_count, code);
    else if (strcmp(type, "head_armor") == 0) it = armor_from_code(g->head_armors, g->head_armor_count, code);
    else it = armor_from_code(g->arms_armors, g->arms_armor_count, code);
    char buf[32];
    char name_buf[96];
    ammo_display_name(it.name, it.enhanced, name_buf, sizeof(name_buf));
    value_map_set(map, "name", name_buf);
    snprintf(buf, sizeof(buf), "%d", it.armor); value_map_set(map, "armor", buf);
    snprintf(buf, sizeof(buf), "%d", it.accuracy); value_map_set(map, "accuracy", buf);
    snprintf(buf, sizeof(buf), "%d", it.price); value_map_set(map, "price", buf);
  }
}

static int anim_speed_ms_for(const Game *g) {
  const int speeds[] = {100, 400, 700, 1000, 1500};
  int idx = g ? g->anim_speed_index : 1;
  if (idx < 0) idx = 0;
  if (idx > 4) idx = 4;
  return speeds[idx];
}

static const char *enemy_attack_art_from_type(int attack_type) {
  if (attack_type == 2) return "attack_head";
  if (attack_type == 3) return "attack_legs";
  return "attack";
}

static void battle_anim_queue(Game *g, const char **seq, int count) {
  if (!g || !seq || count <= 0) return;
  if (count > 4) count = 4;
  g->battle_anim_active = 1;
  g->battle_anim_step = 0;
  g->battle_anim_count = count;
  for (int i = 0; i < count; ++i) {
    snprintf(g->battle_anim_seq[i], sizeof(g->battle_anim_seq[i]), "%s", seq[i]);
  }
  snprintf(g->battle_art_name, sizeof(g->battle_art_name), "%s", seq[0]);
  g->battle_anim_deadline = SDL_GetTicks() + (uint32_t)anim_speed_ms_for(g);
  g->force_instant_redraw = 1;
}

static bool battle_anim_tick(Game *g, uint32_t now) {
  if (!g || !g->battle_anim_active) return false;
  if (now < g->battle_anim_deadline) return false;
  g->battle_anim_step += 1;
  if (g->battle_anim_step < g->battle_anim_count) {
    snprintf(g->battle_art_name, sizeof(g->battle_art_name), "%s", g->battle_anim_seq[g->battle_anim_step]);
    g->battle_anim_deadline = now + (uint32_t)anim_speed_ms_for(g);
    g->force_instant_redraw = 1;
    return true;
  }
  g->battle_anim_active = 0;
  if (g->battle_exit_pending) {
    g->battle_exit_pending = 0;
    g->state = g->battle_exit_state;
    g->force_instant_redraw = 1;
    return true;
  }
  if (strcmp(g->battle_art_name, "normal") != 0) {
    snprintf(g->battle_art_name, sizeof(g->battle_art_name), "normal");
    g->force_instant_redraw = 1;
    return true;
  }
  return false;
}

static void battle_round(Game *g, int attack_type, int *out_enemy_attack_type) {
  Character *h = &g->hero;
  Character *e = &g->enemy;
  if (out_enemy_attack_type) *out_enemy_attack_type = 0;

  double h_damage = rand_range(character_min_dmg(h), character_max_dmg(h));
  double h_acc = character_accuracy(h);
  const char *attack_label = "body";
  bool used_active = false;
  double enemy_damage_mod = 1.0;

  if (attack_type == 2) {
    h_damage *= 1.5;
    h_acc *= 0.7;
    attack_label = "head";
  } else if (attack_type == 3) {
    h_damage *= 0.7;
    h_acc *= 1.5;
    attack_label = "legs";
  } else if (attack_type == 4) {
    if (strcmp(h->active_skill.code, "none") == 0) {
      logbuffer_push(&g->log, "You have no active skill");
      return;
    }
    if (h->mp < h->active_skill.mp_cost) {
      logbuffer_push(&g->log, "Not enough MP");
      return;
    }
    h->mp -= h->active_skill.mp_cost;
    h_damage *= skill_active_damage_mod(&h->active_skill, h);
    h_acc *= skill_active_accuracy_mod(&h->active_skill, h);
    attack_label = h->active_skill.name;
    used_active = true;
  }

  h_damage *= skill_berserk_coef(&h->passive_skill, h);

  bool enemy_block = rand_range(1, 100) <= character_block_chance(e);
  bool h_hit = rand_range(1, 100) <= (int)round(h_acc);
  if (h_hit) {
    if (enemy_block) {
      double coeff = 1.0 + (double)e->hp / 200.0;
      h_damage /= coeff;
    }
    int armor_block = character_armor(e) - character_armor_penetration(h);
    if (armor_block < 0) armor_block = 0;
    h_damage -= armor_block;
    if (h_damage < 0) h_damage = 0;
    e->hp -= (int)round(h_damage);
    if (e->hp < 0) e->hp = 0;

    char msg[160];
    snprintf(msg, sizeof(msg), "You hit %s for %d (%s)", e->name, (int)round(h_damage), attack_label);
    if (enemy_block) {
      char block_msg[64];
      snprintf(block_msg, sizeof(block_msg), " (blocked %d%%)", block_power_in_percents(e));
      strncat(msg, block_msg, sizeof(msg) - strlen(msg) - 1);
    }
    logbuffer_push(&g->log, msg);

    double bonus = skill_concentration_bonus(&h->passive_skill, h);
    if (bonus > 0) {
      e->hp -= (int)round(bonus);
      if (e->hp < 0) e->hp = 0;
      char msg2[128];
      snprintf(msg2, sizeof(msg2), "Concentration adds %d damage", (int)round(bonus));
      logbuffer_push(&g->log, msg2);
    }

    if (strcmp(h->passive_skill.code, "dazed") == 0) {
      double hp_part_coef = skill_dazed_hp_part_coef(&h->passive_skill);
      if (h_damage * hp_part_coef > e->hp / 2.0) {
        enemy_damage_mod = skill_dazed_accuracy_reduce_coef(&h->passive_skill);
        char msg3[128];
        snprintf(msg3, sizeof(msg3), "%s is dazed, accuracy reduced", e->name);
        logbuffer_push(&g->log, msg3);
      }
    }

    if (used_active && strcmp(h->active_skill.code, "traumatic_strike") == 0) {
      enemy_damage_mod = skill_traumatic_effect_coef(&h->active_skill);
      char msg4[128];
      snprintf(msg4, sizeof(msg4), "%s injured, damage reduced", e->name);
      logbuffer_push(&g->log, msg4);
    }
  } else {
    char msg[128];
    snprintf(msg, sizeof(msg), "You miss (%s)", attack_label);
    logbuffer_push(&g->log, msg);
  }

  if (e->hp <= 0) return;

  int e_attack_type = rand_range(1, 3);
  if (out_enemy_attack_type) *out_enemy_attack_type = e_attack_type;
  double e_damage = rand_range(character_min_dmg(e), character_max_dmg(e));
  double e_acc = character_accuracy(e) * enemy_damage_mod;
  const char *e_label = "body";
  if (e_attack_type == 2) {
    e_damage *= 1.5;
    e_acc *= 0.7;
    e_label = "head";
  } else if (e_attack_type == 3) {
    e_damage *= 0.7;
    e_acc *= 1.5;
    e_label = "legs";
  }

  bool hero_block = rand_range(1, 100) <= character_block_chance(h);
  bool e_hit = rand_range(1, 100) <= (int)round(e_acc);
  if (e_hit) {
    if (hero_block) {
      double coeff = 1.0 + (double)h->hp / 200.0;
      e_damage /= coeff;
    }
    int armor_block = character_armor(h) - character_armor_penetration(e);
    if (armor_block < 0) armor_block = 0;
    e_damage -= armor_block;
    if (e_damage < 0) e_damage = 0;
    h->hp -= (int)round(e_damage);
    if (h->hp < 0) h->hp = 0;
    char msg[160];
    snprintf(msg, sizeof(msg), "%s hits you for %d (%s)", e->name, (int)round(e_damage), e_label);
    if (hero_block) {
      char block_msg[64];
      snprintf(block_msg, sizeof(block_msg), " (blocked %d%%)", block_power_in_percents(h));
      strncat(msg, block_msg, sizeof(msg) - strlen(msg) - 1);
    }
    logbuffer_push(&g->log, msg);
  } else {
    char msg[128];
    snprintf(msg, sizeof(msg), "%s misses (%s)", e->name, e_label);
    logbuffer_push(&g->log, msg);
  }

  if (h->regen_hp_base > 0 && h->hp < h->hp_max) {
    int gain = h->regen_hp_base;
    if (h->hp + gain > h->hp_max) gain = h->hp_max - h->hp;
    h->hp += gain;
    if (gain > 0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "You regenerate %d HP", gain);
      logbuffer_push(&g->log, msg);
    }
  }
  if (h->regen_mp_base > 0 && h->mp < h->mp_max) {
    int gain = h->regen_mp_base;
    if (h->mp + gain > h->mp_max) gain = h->mp_max - h->mp;
    h->mp += gain;
    if (gain > 0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "You regenerate %d MP", gain);
      logbuffer_push(&g->log, msg);
    }
  }
  if (e->regen_hp_base > 0 && e->hp < e->hp_max) {
    int gain = e->regen_hp_base;
    if (e->hp + gain > e->hp_max) gain = e->hp_max - e->hp;
    e->hp += gain;
    if (gain > 0) {
      char msg[128];
      snprintf(msg, sizeof(msg), "%s regenerates %d HP", e->name, gain);
      logbuffer_push(&g->log, msg);
    }
  }
}

static int monolith_points_from_enemy(const Character *hero, const Character *enemy) {
  if (!hero || !enemy) return 0;
  double stats_sum = 0.0;
  const double hero_stats[] = {
      (double)hero->hp_max, (double)hero->mp_max,
      (double)character_min_dmg(hero), (double)character_max_dmg(hero),
      (double)hero->regen_hp_base, (double)hero->regen_mp_base,
      (double)character_armor(hero), (double)character_accuracy(hero)
  };
  const double enemy_stats[] = {
      (double)enemy->hp_max, (double)enemy->mp_max,
      (double)character_min_dmg(enemy), (double)character_max_dmg(enemy),
      (double)enemy->regen_hp_base, (double)enemy->regen_mp_base,
      (double)character_armor(enemy), (double)character_accuracy(enemy)
  };
  int count = (int)(sizeof(hero_stats) / sizeof(hero_stats[0]));
  for (int i = 0; i < count; ++i) {
    if (hero_stats[i] <= 1.0) stats_sum += enemy_stats[i];
    else stats_sum += enemy_stats[i] / hero_stats[i];
  }
  double probability = stats_sum / count;
  int points = (int)floor(probability);
  double frac = probability - points;
  if (((double)rand() / (double)RAND_MAX) < frac) points += 1;
  if (points < 0) points = 0;
  return points;
}

static void game_init(Game *g) {
  memset(g, 0, sizeof(*g));
  g->state = STATE_START;
  g->next_state = STATE_START;
  logbuffer_init(&g->log);
  g->message_art_name[0] = '\0';
  g->message_art_path[0] = '\0';
  g->hero_selected = 0;
  g->name_len = 0;
  g->name_input[0] = '\0';
  g->name_error[0] = '\0';
  g->stat_dice1 = 0;
  g->stat_dice2 = 0;
  g->stat_roll = 0;
  g->skill_dice1 = 0;
  g->skill_dice2 = 0;
  g->skill_choice_count = 0;
  g->return_state = STATE_START;
  g->ammo_show_type[0] = '\0';
  g->ammo_show_code[0] = '\0';
  g->stats_dungeon_index = 0;
  g->current_recipe_index = -1;
  g->loot_count = 0;
  g->loot_index = 0;
  g->loot_show_coins = 0;
  g->loot_show_ingredient = 0;
  g->loot_message_mode = 0;
  g->loot_coins = 0;
  g->loot_ingredient[0] = '\0';
  g->loot_message[0] = '\0';
  g->event_choice_count = 0;
  g->event_message[0] = '\0';
  g->event_art_path[0] = '\0';
  g->event_art_name[0] = '\0';
  g->event_choose_message[0] = '\0';
  g->event_code[0] = '\0';
  g->event_step = 0;
  for (int i = 0; i < 4; ++i) g->event_data[i] = 0;
  g->event_input_mode = EVENT_INPUT_NONE;
  g->event_text[0] = '\0';
  g->event_text_len = 0;
  g->event_pending_action = EVENT_PENDING_NONE;
  g->wg_taken = 0;
  g->wg_enemy[0] = '\0';
  g->wg_count = 0;
  g->wg_level = 0;
  g->anim_speed_index = 1;
  g->screen_replace_type = 1;
  snprintf(g->battle_art_name, sizeof(g->battle_art_name), "normal");
  g->battle_art_dungeon[0] = '\0';
  g->battle_anim_active = 0;
  g->battle_anim_step = 0;
  g->battle_anim_count = 0;
  g->battle_anim_deadline = 0;
  g->battle_exit_pending = 0;
  g->battle_exit_state = STATE_BATTLE;
  g->force_instant_redraw = 0;
  g->loot_return_state = STATE_CAMPFIRE;
  g->loot_return_pending = 0;
  g->loot_last_taken = -1;
  for (int i = 0; i < 3; ++i) g->enemy_choice_is_boss[i] = 0;
  g->enemy_is_boss = 0;
  g->enemy_choose_message[0] = '\0';
  for (int i = 0; i < 3; ++i) g->enemy_choice_is_boss[i] = 0;
  shop_init_default(&g->shop);
  warehouse_init_default(&g->warehouse);
  monolith_init_default(&g->monolith);
  statistics_total_init_default(&g->stats_total);
  g->occult.recipes = NULL;
  g->occult.recipe_count = 0;
  snprintf(g->dungeons[0].name, sizeof(g->dungeons[0].name), "bandits");
  snprintf(g->dungeons[1].name, sizeof(g->dungeons[1].name), "undeads");
  snprintf(g->dungeons[2].name, sizeof(g->dungeons[2].name), "swamp");
}

static void game_free(Game *g) {
  if (!g) return;
  if (g->heroes) {
    for (size_t i = 0; i < g->hero_count; ++i) {
      free_string_list(g->heroes[i].weapon_options, g->heroes[i].weapon_count);
      free_string_list(g->heroes[i].body_armor_options, g->heroes[i].body_armor_count);
      free_string_list(g->heroes[i].head_armor_options, g->heroes[i].head_armor_count);
      free_string_list(g->heroes[i].arms_armor_options, g->heroes[i].arms_armor_count);
      free_string_list(g->heroes[i].shield_options, g->heroes[i].shield_count);
    }
    free(g->heroes);
  }
  for (int i = 0; i < 3; ++i) {
    if (g->dungeons[i].enemies) {
      for (size_t j = 0; j < g->dungeons[i].enemy_count; ++j) {
        free_string_list(g->dungeons[i].enemies[j].weapon_options, g->dungeons[i].enemies[j].weapon_count);
        free_string_list(g->dungeons[i].enemies[j].body_armor_options, g->dungeons[i].enemies[j].body_armor_count);
        free_string_list(g->dungeons[i].enemies[j].head_armor_options, g->dungeons[i].enemies[j].head_armor_count);
        free_string_list(g->dungeons[i].enemies[j].arms_armor_options, g->dungeons[i].enemies[j].arms_armor_count);
        free_string_list(g->dungeons[i].enemies[j].shield_options, g->dungeons[i].enemies[j].shield_count);
        free_string_list(g->dungeons[i].enemies[j].ingredient_options, g->dungeons[i].enemies[j].ingredient_count);
      }
      free(g->dungeons[i].enemies);
    }
  }
  if (g->event_enemies) {
    for (size_t j = 0; j < g->event_enemy_count; ++j) {
      free_string_list(g->event_enemies[j].weapon_options, g->event_enemies[j].weapon_count);
      free_string_list(g->event_enemies[j].body_armor_options, g->event_enemies[j].body_armor_count);
      free_string_list(g->event_enemies[j].head_armor_options, g->event_enemies[j].head_armor_count);
      free_string_list(g->event_enemies[j].arms_armor_options, g->event_enemies[j].arms_armor_count);
      free_string_list(g->event_enemies[j].shield_options, g->event_enemies[j].shield_count);
      free_string_list(g->event_enemies[j].ingredient_options, g->event_enemies[j].ingredient_count);
    }
    free(g->event_enemies);
  }
  free(g->weapons);
  free(g->body_armors);
  free(g->head_armors);
  free(g->arms_armors);
  free(g->shields);
  occult_library_free(&g->occult);
  value_map_clear(&g->hero.ingredients);
  logbuffer_free(&g->log);
}

static void game_prepare_hero_select(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  value_map_set(main_map, "main", "Select a background");

  logbuffer_clear(&g->log);
  logbuffer_push(&g->log, "Background:   HP:         MP:         Min dmg:    Max dmg:    Accuracy:   Armor:");
  for (size_t i = 0; i < g->hero_count; ++i) {
    char line[256];
    const HeroTemplate *h = &g->heroes[i];
    snprintf(line, sizeof(line), "[Enter %2zu]  %-10s  %-10d %-10d %-10d %-10d %-10d %-10d", i + 1, h->name, h->hp, h->mp, h->min_dmg, h->max_dmg, h->accuracy, h->armor);
    logbuffer_push(&g->log, line);
  }
  logbuffer_apply_full(main_map, &g->log, 60);
}

static void game_prepare_name_input(Game *g, ValueMap *main_map, const char *error) {
  value_map_clear(main_map);
  char title[128];
  snprintf(title, sizeof(title), "Enter character name: %s", g->name_input);
  value_map_set(main_map, "main", title);
  logbuffer_clear(&g->log);
  const char *err = error;
  if (!err || !err[0]) err = g->name_error;
  if (err && err[0]) {
    logbuffer_push(&g->log, err);
  }
  logbuffer_push(&g->log, "The character name must contain at least 1 letter and be no more than 20 characters");
  logbuffer_apply_full(main_map, &g->log, 20);
}

static void game_prepare_skill_select(Game *g, ValueMap *main_map, SkillType type) {
  value_map_clear(main_map);
  if (type == SKILL_ACTIVE) value_map_set(main_map, "main", "Select an active skill");
  else if (type == SKILL_PASSIVE) value_map_set(main_map, "main", "Select a passive skill");
  else value_map_set(main_map, "main", "Select a camp skill");

  logbuffer_clear(&g->log);

  const char *skills_active[] = {"ascetic_strike", "precise_strike", "strong_strike", "traumatic_strike"};
  const char *skills_passive[] = {"berserk", "concentration", "dazed", "shield_master"};
  const char *skills_camp[] = {"bloody_ritual", "first_aid", "treasure_hunter"};
  const char **list = skills_active;
  size_t count = sizeof(skills_active) / sizeof(skills_active[0]);
  if (type == SKILL_PASSIVE) { list = skills_passive; count = sizeof(skills_passive) / sizeof(skills_passive[0]); }
  if (type == SKILL_CAMP) { list = skills_camp; count = sizeof(skills_camp) / sizeof(skills_camp[0]); }

  for (size_t i = 0; i < count; ++i) {
    Skill s;
    skill_assign(&s, type, list[i]);
    char desc[160];
    skill_description_short(&s, &g->hero, desc, sizeof(desc));
    char line[240];
    snprintf(line, sizeof(line), "   [Enter %zu]   %-20s %s", i + 1, s.name, desc);
    logbuffer_push(&g->log, line);
  }
  logbuffer_apply_full(main_map, &g->log, 60);
}

static void game_prepare_start(Game *g, ValueMap *main_map, const char *version) {
  value_map_clear(main_map);
  value_map_set(main_map, "main", version ? version : "v");
  logbuffer_clear(&g->log);
}

static void game_prepare_load_menu(Game *g, ValueMap *main_map) {
  (void)g;
  value_map_clear(main_map);
  value_map_set(main_map, "main", "");
  logbuffer_clear(&g->log);
}

static void game_prepare_load_confirm(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  value_map_set(main_map, "main", "Load game [Enter 1]            Back to menu [Enter 0]");
  char log0[64];
  snprintf(log0, sizeof(log0), "%s", g->hero.dungeon_name);
  if (log0[0]) log0[0] = (char)toupper((unsigned char)log0[0]);
  value_map_set(main_map, "log_0", log0);
  char log1[32];
  snprintf(log1, sizeof(log1), "%d", g->hero.leveling + 1);
  value_map_set(main_map, "log_1", log1);
}

static void game_prepare_choose_dungeon(Game *g, ValueMap *main_map) {
  (void)g;
  value_map_clear(main_map);
  value_map_set(main_map, "main", "");
  logbuffer_clear(&g->log);
}

static void game_prepare_enemy_select(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  if (g->enemy_choose_message[0]) value_map_set(main_map, "main", g->enemy_choose_message);
  else value_map_set(main_map, "main", "Choose your enemy");
  logbuffer_clear(&g->log);
}

static void game_prepare_battle(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  value_map_set(main_map, "main", "Battle");
  char actions[160];
  if (strcmp(g->hero.active_skill.code, "none") != 0) {
    snprintf(actions, sizeof(actions), "Hit body [1]  Head [2]  Legs [3]  %s [4]", g->hero.active_skill.name);
  } else {
    snprintf(actions, sizeof(actions), "Hit body [1]  Head [2]  Legs [3]");
  }
  value_map_set(main_map, "actions", actions);
  logbuffer_apply_last(main_map, &g->log, 4);
}

static void game_prepare_campfire(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  char buf[32];
  snprintf(buf, sizeof(buf), "%d", g->hero.stat_points);
  value_map_set(main_map, "additional_1", buf);
  snprintf(buf, sizeof(buf), "%d", g->hero.skill_points);
  value_map_set(main_map, "additional_2", buf);
  logbuffer_apply_full(main_map, &g->log, 3);
}

static void game_prepare_camp(Game *g, ValueMap *main_map) {
  (void)g;
  value_map_clear(main_map);
  value_map_set(main_map, "main", "");
  logbuffer_clear(&g->log);
}

static void game_prepare_shop(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  char buf[64];
  snprintf(buf, sizeof(buf), "%d", g->warehouse.coins);
  value_map_set(main_map, "coins", buf);

  const char *types[] = {"weapon", "body_armor", "head_armor", "arms_armor", "shield"};
  for (int t = 0; t < 5; ++t) {
    char (*arr)[32] = NULL;
    if (t == 0) arr = g->shop.weapon;
    else if (t == 1) arr = g->shop.body_armor;
    else if (t == 2) arr = g->shop.head_armor;
    else if (t == 3) arr = g->shop.arms_armor;
    else arr = g->shop.shield;
    for (int i = 0; i < 3; ++i) {
      char key[32];
      snprintf(key, sizeof(key), "%s__%d", types[t], i);
      value_map_set(main_map, key, ammo_name(g, types[t], arr[i]));
      snprintf(key, sizeof(key), "%s__%d__price", types[t], i);
      snprintf(buf, sizeof(buf), "%d", ammo_price(g, types[t], arr[i]));
      value_map_set(main_map, key, buf);
    }
  }

  value_map_set(main_map, "weapon", ammo_name(g, "weapon", g->warehouse.weapon));
  value_map_set(main_map, "body_armor", ammo_name(g, "body_armor", g->warehouse.body_armor));
  value_map_set(main_map, "head_armor", ammo_name(g, "head_armor", g->warehouse.head_armor));
  value_map_set(main_map, "arms_armor", ammo_name(g, "arms_armor", g->warehouse.arms_armor));
  value_map_set(main_map, "shield", ammo_name(g, "shield", g->warehouse.shield));
}

static void game_prepare_monolith(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  char buf[64];
  snprintf(buf, sizeof(buf), "%d", g->monolith.points);
  value_map_set(main_map, "points", buf);

  const char *stats[] = {"hp","mp","accuracy","damage","stat_points","skill_points","armor","regen_hp","regen_mp","armor_penetration","block_chance"};
  for (size_t i = 0; i < sizeof(stats) / sizeof(stats[0]); ++i) {
    if (!stats[i]) continue;
    snprintf(buf, sizeof(buf), "%d", monolith_get_stat(&g->monolith, stats[i]));
    value_map_set(main_map, stats[i], buf);
    char key[64];
    snprintf(key, sizeof(key), "%s__p", stats[i]);
    snprintf(buf, sizeof(buf), "%d", monolith_price_for(&g->monolith, stats[i]));
    value_map_set(main_map, key, buf);
  }
}

static void format_hero_ingredients(const Character *h, char *out, size_t out_sz) {
  if (!out || out_sz == 0) return;
  out[0] = '\0';
  if (!h || h->ingredients.count == 0) {
    snprintf(out, out_sz, "Your ingredients:     ---");
    return;
  }
  char list[512] = {0};
  bool first = true;
  for (size_t i = 0; i < h->ingredients.count; ++i) {
    char name_buf[64];
    titleize_token(h->ingredients.items[i].key, name_buf, sizeof(name_buf));
    int val = atoi(h->ingredients.items[i].value ? h->ingredients.items[i].value : "0");
    append_kv(list, sizeof(list), name_buf, val, &first);
  }
  snprintf(out, out_sz, "Your ingredients:     %s", list[0] ? list : "---");
}

static void game_prepare_occult_library(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  char buf[128];
  snprintf(buf, sizeof(buf), "%d", g->warehouse.coins);
  value_map_set(main_map, "coins", buf);

  for (int i = 1; i <= 24; ++i) {
    OccultRecipe *r = occult_recipe_by_view_code(&g->occult, i);
    char key[32];
    if (r) {
      snprintf(key, sizeof(key), "show__%d", i);
      snprintf(buf, sizeof(buf), "[Enter %c]", (char)('A' + (i - 1)));
      value_map_set(main_map, key, buf);
      snprintf(key, sizeof(key), "name__%d", i);
      value_map_set(main_map, key, r->name);
      snprintf(key, sizeof(key), "price__%d", i);
      if (r->purchased) value_map_set(main_map, key, "SOLD");
      else { snprintf(buf, sizeof(buf), "%d", r->price); value_map_set(main_map, key, buf); }
      snprintf(key, sizeof(key), "status__%d", i);
      if (r->purchased) value_map_set(main_map, key, "IN YOUR WAREHOUSE");
      else { snprintf(buf, sizeof(buf), "[Enter %d]", i); value_map_set(main_map, key, buf); }
    } else {
      snprintf(key, sizeof(key), "show__%d", i); value_map_set(main_map, key, "");
      snprintf(key, sizeof(key), "name__%d", i); value_map_set(main_map, key, "");
      snprintf(key, sizeof(key), "price__%d", i); value_map_set(main_map, key, "");
      snprintf(key, sizeof(key), "status__%d", i); value_map_set(main_map, key, "");
    }
  }
}

static void game_prepare_recipe_view(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  OccultRecipe *r = (g->current_recipe_index >= 0 && (size_t)g->current_recipe_index < g->occult.recipe_count)
                      ? &g->occult.recipes[g->current_recipe_index] : NULL;
  if (!r) return;
  value_map_set(main_map, "name", r->name);
  char buf[512];
  format_recipe_ingredients(r, buf, sizeof(buf));
  value_map_set(main_map, "recipe", buf);
  format_hero_ingredients(&g->hero, buf, sizeof(buf));
  value_map_set(main_map, "ingredients", buf);

  format_effect(&r->weapon, buf, sizeof(buf)); value_map_set(main_map, "weapon", buf);
  format_effect(&r->head_armor, buf, sizeof(buf)); value_map_set(main_map, "head_armor", buf);
  format_effect(&r->body_armor, buf, sizeof(buf)); value_map_set(main_map, "body_armor", buf);
  format_effect(&r->arms_armor, buf, sizeof(buf)); value_map_set(main_map, "arms_armor", buf);
  format_effect(&r->shield, buf, sizeof(buf)); value_map_set(main_map, "shield", buf);
}

static void game_prepare_recipe_enhance(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  OccultRecipe *r = (g->current_recipe_index >= 0 && (size_t)g->current_recipe_index < g->occult.recipe_count)
                      ? &g->occult.recipes[g->current_recipe_index] : NULL;
  if (!r) return;
  value_map_set(main_map, "name", r->name);
  char buf[512];
  format_recipe_ingredients(r, buf, sizeof(buf));
  value_map_set(main_map, "recipe", buf);
  format_hero_ingredients(&g->hero, buf, sizeof(buf));
  value_map_set(main_map, "ingredients", buf);

  ammo_display_name(g->hero.weapon.name, g->hero.weapon.enhanced, buf, sizeof(buf));
  value_map_set(main_map, "hero__weapon__name", buf);
  ammo_display_name(g->hero.head_armor.name, g->hero.head_armor.enhanced, buf, sizeof(buf));
  value_map_set(main_map, "hero__head_armor__name", buf);
  ammo_display_name(g->hero.body_armor.name, g->hero.body_armor.enhanced, buf, sizeof(buf));
  value_map_set(main_map, "hero__body_armor__name", buf);
  ammo_display_name(g->hero.arms_armor.name, g->hero.arms_armor.enhanced, buf, sizeof(buf));
  value_map_set(main_map, "hero__arms_armor__name", buf);
  ammo_display_name(g->hero.shield.name, g->hero.shield.enhanced, buf, sizeof(buf));
  value_map_set(main_map, "hero__shield__name", buf);

  format_effect(&r->weapon, buf, sizeof(buf)); value_map_set(main_map, "weapon", buf);
  format_effect(&r->head_armor, buf, sizeof(buf)); value_map_set(main_map, "head_armor", buf);
  format_effect(&r->body_armor, buf, sizeof(buf)); value_map_set(main_map, "body_armor", buf);
  format_effect(&r->arms_armor, buf, sizeof(buf)); value_map_set(main_map, "arms_armor", buf);
  format_effect(&r->shield, buf, sizeof(buf)); value_map_set(main_map, "shield", buf);
}

static void game_prepare_enhance_list(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  size_t count = 0;
  size_t *indices = occult_accessible_indices(&g->occult, &count);
  for (int i = 1; i <= 24; ++i) {
    char key[32];
    char buf[128];
    if ((size_t)(i - 1) < count) {
      OccultRecipe *r = &g->occult.recipes[indices[i - 1]];
      snprintf(key, sizeof(key), "show__%d", i);
      snprintf(buf, sizeof(buf), "[Enter %c]", (char)('A' + (i - 1)));
      value_map_set(main_map, key, buf);
      snprintf(key, sizeof(key), "name__%d", i);
      value_map_set(main_map, key, r->name);
      snprintf(key, sizeof(key), "has_ingredients__%d", i);
      value_map_set(main_map, key, recipe_hero_has_ingredients(r, &g->hero) ? "YES" : "NO");
    } else {
      snprintf(key, sizeof(key), "show__%d", i); value_map_set(main_map, key, "");
      snprintf(key, sizeof(key), "name__%d", i); value_map_set(main_map, key, "");
      snprintf(key, sizeof(key), "has_ingredients__%d", i); value_map_set(main_map, key, "");
    }
  }
  free(indices);
}

static void game_prepare_stats_choose(Game *g, ValueMap *main_map) {
  (void)g;
  value_map_clear(main_map);
  value_map_set(main_map, "main", "Select dungeon statistics");
}

static void game_prepare_stats_show(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  const char *dungeon = g->stats_dungeon_index == 0 ? "bandits" :
                        g->stats_dungeon_index == 1 ? "undeads" : "swamp";
  char name_buf[64];
  titleize_token(dungeon, name_buf, sizeof(name_buf));
  value_map_set(main_map, "name", name_buf);

  const char *bandits[] = {"rabble","rabid_dog","poacher","thug","deserter","bandit_leader"};
  const char *undeads[] = {"zombie","skeleton","ghost","fat_ghoul","skeleton_soldier","zombie_knight"};
  const char *swamp[] = {"leech","goblin","sworm","spider","orc","ancient_snail"};
  const char *rewards[] = {
    "Permanent weapon \"Stick\"",
    "+2 HP",
    "+1 accuracy",
    "+5 HP",
    "+1 stat point",
    "+1 skill point",
    "Permanent \"Worn gloves\"",
    "+3 MP",
    "+1 accuracy",
    "+7 HP",
    "+3 block chance",
    "+1 MP-regen",
    "+3 MP",
    "Permanent \"Holey wicker buckler\"",
    "+3 HP",
    "+1 accuracy",
    "+1 max damage",
    "+1 armor"
  };
  const char **list = g->stats_dungeon_index == 0 ? bandits :
                      g->stats_dungeon_index == 1 ? undeads : swamp;
  int counts[6];
  for (int i = 0; i < 6; ++i) {
    counts[i] = stats_total_get(&g->stats_total, dungeon, list[i]);
  }
  for (int i = 0; i < 6; ++i) {
    char key[32];
    char buf[128];
    char enemy_name[64];
    titleize_token(list[i], enemy_name, sizeof(enemy_name));
    snprintf(key, sizeof(key), "enemy_name__%d", i);
    value_map_set(main_map, key, enemy_name);
    snprintf(key, sizeof(key), "enemy_count__%d", i);
    snprintf(buf, sizeof(buf), "%d", counts[i]);
    value_map_set(main_map, key, buf);
    snprintf(key, sizeof(key), "enemy_done__%d", i);
    int needed = (g->stats_dungeon_index == 0 && i == 5) || (g->stats_dungeon_index == 1 && i == 5) || (g->stats_dungeon_index == 2 && i == 5) ? 5 : 30;
    value_map_set(main_map, key, counts[i] >= needed ? "DONE" : "");
    snprintf(key, sizeof(key), "enemy_kill__%d", i);
    snprintf(buf, sizeof(buf), "%d", needed);
    value_map_set(main_map, key, buf);
    snprintf(key, sizeof(key), "enemy_get__%d", i);
    int reward_idx = g->stats_dungeon_index * 6 + i;
    value_map_set(main_map, key, rewards[reward_idx]);
  }
}

static void game_prepare_event_result(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  const char *title = g->event_message[0] ? g->event_message : "Event";
  value_map_set(main_map, "main", title);
  logbuffer_apply_full(main_map, &g->log, 7);
  if (g->event_input_mode == EVENT_INPUT_TEXT) {
    char buf[128];
    snprintf(buf, sizeof(buf), "> %s", g->event_text);
    value_map_set(main_map, "log_6", buf);
  }
}

static void game_prepare_options(Game *g, ValueMap *main_map) {
  (void)g;
  value_map_clear(main_map);
  value_map_set(main_map, "main", "");
}

static void game_prepare_options_anim(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  for (int i = 0; i < 5; ++i) {
    char key[64];
    snprintf(key, sizeof(key), "enemy_actions_animation_speed__%d", i);
    if (g->anim_speed_index == i) value_map_set(main_map, key, "SELECTED");
    else {
      char buf[32];
      snprintf(buf, sizeof(buf), "[Enter %d]", i + 1);
      value_map_set(main_map, key, buf);
    }
  }
}

static void game_prepare_options_replace(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  for (int i = 0; i < 3; ++i) {
    char key[64];
    snprintf(key, sizeof(key), "screen_replacement_type__%d", i);
    if (g->screen_replace_type == i) value_map_set(main_map, key, "SELECTED");
    else {
      char buf[32];
      snprintf(buf, sizeof(buf), "[Enter %d]", i + 1);
      value_map_set(main_map, key, buf);
    }
  }
}

static void game_prepare_loot(Game *g, ValueMap *main_map, ValueMap *hero_item_map, ValueMap *enemy_item_map,
                              ArtArg **out_arts, size_t *out_art_count) {
  value_map_clear(main_map);
  value_map_set(main_map, "main", g->loot_message[0] ? g->loot_message : "Loot found");
  value_map_clear(hero_item_map);
  value_map_clear(enemy_item_map);

  if (g->loot_index >= g->loot_count) return;
  const LootEntry *le = &g->loot_items[g->loot_index];
  const char *type = le->type;
  const char *enemy_code = le->code;
  const char *hero_code = "without";
  if (strcmp(type, "weapon") == 0) hero_code = g->hero.weapon.code;
  else if (strcmp(type, "body_armor") == 0) hero_code = g->hero.body_armor.code;
  else if (strcmp(type, "head_armor") == 0) hero_code = g->hero.head_armor.code;
  else if (strcmp(type, "arms_armor") == 0) hero_code = g->hero.arms_armor.code;
  else if (strcmp(type, "shield") == 0) hero_code = g->hero.shield.code;

  ammo_to_map(g, type, hero_code, hero_item_map);
  ammo_to_map(g, type, enemy_code, enemy_item_map);

  *out_art_count = 2;
  *out_arts = (ArtArg *)calloc(*out_art_count, sizeof(ArtArg));
  if (*out_arts) {
    char path0[256];
    char path1[256];
    snprintf(path0, sizeof(path0), "ammunition/%s/_%s", type, hero_code);
    snprintf(path1, sizeof(path1), "ammunition/%s/_%s", type, enemy_code);
    (*out_arts)[0].name = strdup_safe("normal");
    (*out_arts)[0].path = strdup_safe(path0);
    (*out_arts)[1].name = strdup_safe("normal");
    (*out_arts)[1].path = strdup_safe(path1);
  }
}

static void game_prepare_loot_message(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  logbuffer_clear(&g->log);
  if (g->loot_message_mode == 1) {
    g->hero.coins += g->loot_coins;
    char msg[192];
    snprintf(msg, sizeof(msg),
             "After searching the %s's body you found %d coins. Now you have %d coins",
             g->enemy.name, g->loot_coins, g->hero.coins);
    value_map_set(main_map, "main", "My precious... Press Enter to continue");
    logbuffer_push(&g->log, msg);
    g->loot_show_coins = 0;
  } else if (g->loot_message_mode == 2) {
    int have = value_map_get_int(&g->hero.ingredients, g->loot_ingredient, 0);
    value_map_set_int(&g->hero.ingredients, g->loot_ingredient, have + 1);
    char ing[64];
    titleize_token(g->loot_ingredient, ing, sizeof(ing));
    char msg[192];
    snprintf(msg, sizeof(msg), "After searching the %s's body you found %s", g->enemy.name, ing);
    value_map_set(main_map, "main", "Press Enter to continue");
    logbuffer_push(&g->log, msg);
    g->loot_show_ingredient = 0;
  } else {
    value_map_set(main_map, "main", "Press Enter to continue");
  }
  logbuffer_apply_full(main_map, &g->log, 6);
}

static void game_prepare_hero_info(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  char log0[64];
  const char *dn = g->hero.dungeon_name[0] ? g->hero.dungeon_name : g->dungeons[g->dungeon_index].name;
  snprintf(log0, sizeof(log0), "%s", dn);
  log0[0] = (char)toupper((unsigned char)log0[0]);
  value_map_set(main_map, "log_0", log0);
  char log1[32];
  snprintf(log1, sizeof(log1), "%d", g->hero.lvl + 1);
  value_map_set(main_map, "log_1", log1);
  value_map_set(main_map, "main", "BACK TO CAMP FIRE OPTIONS  [Enter 0]");
}

static void game_prepare_spend_stat(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  char title[128];
  snprintf(title, sizeof(title), "Distribute stat points. You have %d points left", g->hero.stat_points);
  value_map_set(main_map, "main", title);

  if (g->stat_roll == 0) {
    g->stat_dice1 = rand_range(1, 6);
    g->stat_dice2 = rand_range(1, 6);
    g->stat_roll = g->stat_dice1 + g->stat_dice2;
  }

  logbuffer_clear(&g->log);
  char line[128];
  snprintf(line, sizeof(line), "The dice showed: %d (%d + %d)", g->stat_roll, g->stat_dice1, g->stat_dice2);
  logbuffer_push(&g->log, line);
  logbuffer_push(&g->log, "");
  logbuffer_push(&g->log, "+5 HP                     [1]");
  logbuffer_push(&g->log, "+5 MP                     [2]");
  if (g->stat_roll >= 8) logbuffer_push(&g->log, "+1 accuracy               [3]");
  if (g->stat_roll >= 11) logbuffer_push(&g->log, "+1 min/max(random) damage [4]");
  logbuffer_apply_full(main_map, &g->log, 7);
}

static void game_prepare_spend_skill(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  char title[128];
  snprintf(title, sizeof(title), "Distribute skill points. You have %d points left", g->hero.skill_points);
  value_map_set(main_map, "main", title);

  if (g->skill_choice_count == 0) {
    g->skill_dice1 = rand_range(1, 6);
    g->skill_dice2 = rand_range(1, 6);
    int roll = g->skill_dice1 + g->skill_dice2;
    g->skill_choice_count = roll >= 10 ? 3 : roll >= 6 ? 2 : 1;

    SkillType pool[3] = {SKILL_ACTIVE, SKILL_PASSIVE, SKILL_CAMP};
    for (int i = 0; i < 3; ++i) {
      int j = rand_range(i, 2);
      SkillType tmp = pool[i];
      pool[i] = pool[j];
      pool[j] = tmp;
    }
    for (int i = 0; i < g->skill_choice_count; ++i) {
      g->skill_choices[i] = pool[i];
    }
    // sort by type
    for (int i = 0; i < g->skill_choice_count - 1; ++i) {
      for (int j = i + 1; j < g->skill_choice_count; ++j) {
        if (g->skill_choices[j] < g->skill_choices[i]) {
          SkillType tmp = g->skill_choices[i];
          g->skill_choices[i] = g->skill_choices[j];
          g->skill_choices[j] = tmp;
        }
      }
    }
  }

  logbuffer_clear(&g->log);
  char line[128];
  snprintf(line, sizeof(line), "The dice showed: %d (%d + %d)", g->skill_dice1 + g->skill_dice2, g->skill_dice1, g->skill_dice2);
  logbuffer_push(&g->log, line);
  logbuffer_push(&g->log, "");
  for (int i = 0; i < g->skill_choice_count; ++i) {
    const Skill *s = g->skill_choices[i] == SKILL_ACTIVE ? &g->hero.active_skill :
                     g->skill_choices[i] == SKILL_PASSIVE ? &g->hero.passive_skill :
                     &g->hero.camp_skill;
    char line2[128];
    snprintf(line2, sizeof(line2), "%-20s [%d]", s->name, i + 1);
    logbuffer_push(&g->log, line2);
  }
  logbuffer_apply_full(main_map, &g->log, 7);
}

static void game_prepare_message(Game *g, ValueMap *main_map) {
  value_map_clear(main_map);
  value_map_set(main_map, "main", g->message_title);
  logbuffer_apply_full(main_map, &g->log, 20);
}

static int key_to_digit(SDL_Keycode key) {
  if (key >= SDLK_0 && key <= SDLK_9) return (int)(key - SDLK_0);
  if (key >= SDLK_KP_0 && key <= SDLK_KP_9) return (int)(key - SDLK_KP_0);
  return -1;
}

static char key_to_letter(SDL_Keycode key) {
  if (key >= SDLK_a && key <= SDLK_z) return (char)('a' + (key - SDLK_a));
  return '\0';
}

static void pick_random_enemies(Game *g) {
  DungeonData *d = &g->dungeons[g->dungeon_index];
  g->enemy_choose_message[0] = '\0';
  if (g->hero.leveling >= 25) {
    const EnemyTemplate *boss = enemy_template_boss(d);
    if (boss) {
      g->enemy_choice_count = 1;
      g->enemy_choices[0] = character_from_enemy(g, boss);
      g->enemy_choice_is_boss[0] = 1;
      snprintf(g->enemy_choose_message, sizeof(g->enemy_choose_message),
               "You've reached the end of the dungeon, this is a boss fight!");
      return;
    }
  }
  int count = enemy_choices_count_for(&g->hero);
  if (count < 1) count = 1;
  if (count > 3) count = 3;
  g->enemy_choice_count = count;
  for (int i = 0; i < g->enemy_choice_count; ++i) {
    const EnemyTemplate *tmpl = enemy_template_random_standard(d, g->hero.leveling);
    if (!tmpl) tmpl = &d->enemies[0];
    g->enemy_choices[i] = character_from_enemy(g, tmpl);
    g->enemy_choice_is_boss[i] = tmpl->is_boss ? 1 : 0;
  }
  if (g->enemy_choice_count > 0) {
    int random = rand_range(1, 200);
    int th = treasure_hunter_coeff(&g->hero.camp_skill);
    if (th > 0) {
      snprintf(g->enemy_choose_message, sizeof(g->enemy_choose_message),
               "Random is %d + treasure hunter %d = you find %d ways. Which way will you go?",
               random, th, g->enemy_choice_count);
    } else {
      snprintf(g->enemy_choose_message, sizeof(g->enemy_choose_message),
               "Random is %d = you find %d ways. Which way will you go?",
               random, g->enemy_choice_count);
    }
  }
}

static int dungeon_index_by_name(Game *g, const char *name) {
  if (!g || !name) return 0;
  for (int i = 0; i < 3; ++i) {
    if (strcmp(g->dungeons[i].name, name) == 0) return i;
  }
  return 0;
}

static const EnemyTemplate *enemy_template_by_code(const DungeonData *d, const char *code) {
  if (!d || !code) return NULL;
  for (size_t i = 0; i < d->enemy_count; ++i) {
    if (strcmp(d->enemies[i].code, code) == 0) return &d->enemies[i];
  }
  return NULL;
}

static const EnemyTemplate *event_enemy_by_code(const Game *g, const char *code) {
  if (!g || !code) return NULL;
  for (size_t i = 0; i < g->event_enemy_count; ++i) {
    if (strcmp(g->event_enemies[i].code, code) == 0) return &g->event_enemies[i];
  }
  return NULL;
}

static const char *enemy_art_dungeon(const Game *g) {
  if (!g) return "";
  if (g->battle_art_dungeon[0]) return g->battle_art_dungeon;
  return g->dungeons[g->dungeon_index].name;
}

static const EnemyTemplate *enemy_template_boss(const DungeonData *d) {
  if (!d) return NULL;
  for (size_t i = 0; i < d->enemy_count; ++i) {
    if (d->enemies[i].is_boss) return &d->enemies[i];
  }
  return NULL;
}

static const EnemyTemplate *enemy_template_random_standard(const DungeonData *d, int leveling) {
  if (!d || d->enemy_count == 0) return NULL;
  int standard_count = 0;
  for (size_t i = 0; i < d->enemy_count; ++i) {
    if (d->enemies[i].code[0] == 'e') standard_count++;
  }
  if (standard_count == 0) return &d->enemies[0];
  int chance = rand_range(1, 9) + rand_range(0, leveling);
  int target = standard_count;
  for (int n = 1; n <= standard_count; ++n) {
    if (chance <= n * 4) { target = n; break; }
  }
  char code[16];
  snprintf(code, sizeof(code), "e%d", target);
  const EnemyTemplate *tmpl = enemy_template_by_code(d, code);
  if (tmpl) return tmpl;
  return &d->enemies[0];
}

static int enemy_choices_count_for(const Character *hero) {
  int random = rand_range(1, 200);
  int th = treasure_hunter_coeff(&hero->camp_skill);
  int res = random + th;
  int n = res > 120 ? 3 : res > 50 ? 2 : 1;
  return n;
}

static bool game_build_screen(Game *g, const char *version, Menu *menu, ValueMap *main_map,
                              ValueMap *hero_map, ValueMap *enemy_maps[3],
                              ArtArg **out_arts, size_t *out_art_count,
                              char **out_menu_path) {
  (void)menu;
  char *menu_path = NULL;
  ArtArg *arts = NULL;
  size_t art_count = 0;

  if (g->state == STATE_START) {
    menu_path = resolve_menu_path("start_game_screen");
    game_prepare_start(g, main_map, version);
  } else if (g->state == STATE_LOAD_MENU) {
    menu_path = resolve_menu_path("load_new_run_screen");
    game_prepare_load_menu(g, main_map);
    art_count = 1;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    arts[0].name = strdup_safe("dungeon_cave");
    arts[0].path = strdup_safe("_dungeon_enter");
  } else if (g->state == STATE_LOAD_NO_HERO) {
    menu_path = resolve_menu_path("load_no_hero_screen");
    value_map_clear(main_map);
  } else if (g->state == STATE_CHOOSE_DUNGEON) {
    menu_path = resolve_menu_path("choose_dungeon_screen");
    game_prepare_choose_dungeon(g, main_map);
    art_count = 3;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    arts[0].name = strdup_safe("normal"); arts[0].path = strdup_safe("dungeons/_bandits");
    arts[1].name = strdup_safe("normal"); arts[1].path = strdup_safe("dungeons/_undeads");
    arts[2].name = strdup_safe("normal"); arts[2].path = strdup_safe("dungeons/_swamp");
  } else if (g->state == STATE_NAME_INPUT) {
    menu_path = resolve_menu_path("messages_screen");
    game_prepare_name_input(g, main_map, NULL);
    art_count = 1;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    arts[0].name = strdup_safe("scroll");
    arts[0].path = strdup_safe("_choose_name");
  } else if (g->state == STATE_HERO_SELECT) {
    menu_path = resolve_menu_path("messages_full_screen");
    game_prepare_hero_select(g, main_map);
  } else if (g->state == STATE_LOAD_CONFIRM) {
    menu_path = resolve_menu_path("hero_sl_screen");
    game_prepare_load_confirm(g, main_map);
    value_map_clear(hero_map);
    character_to_map(&g->hero, hero_map);
    art_count = 1;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    char path[256];
    snprintf(path, sizeof(path), "dungeons/_%s", g->hero.dungeon_name[0] ? g->hero.dungeon_name : g->dungeons[g->dungeon_index].name);
    arts[0].name = strdup_safe("normal");
    arts[0].path = strdup_safe(path);
  } else if (g->state == STATE_SKILL_ACTIVE) {
    menu_path = resolve_menu_path("messages_full_screen");
    game_prepare_skill_select(g, main_map, SKILL_ACTIVE);
  } else if (g->state == STATE_SKILL_PASSIVE) {
    menu_path = resolve_menu_path("messages_full_screen");
    game_prepare_skill_select(g, main_map, SKILL_PASSIVE);
  } else if (g->state == STATE_SKILL_CAMP) {
    menu_path = resolve_menu_path("messages_full_screen");
    game_prepare_skill_select(g, main_map, SKILL_CAMP);
  } else if (g->state == STATE_ENEMY_SELECT) {
    if (g->enemy_choice_count >= 3) menu_path = resolve_menu_path("enemy_3_choose_screen");
    else if (g->enemy_choice_count == 2) menu_path = resolve_menu_path("enemy_2_choose_screen");
    else menu_path = resolve_menu_path("enemy_1_choose_screen");
    game_prepare_enemy_select(g, main_map);

    for (int i = 0; i < g->enemy_choice_count; ++i) {
      value_map_clear(enemy_maps[i]);
      character_to_map(&g->enemy_choices[i], enemy_maps[i]);
    }
    art_count = (size_t)g->enemy_choice_count;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    for (size_t i = 0; i < art_count; ++i) {
      char path[256];
      snprintf(path, sizeof(path), "enemyes/%s/_%s", g->dungeons[g->dungeon_index].name, g->enemy_choices[i].code);
      arts[i].name = strdup_safe("normal");
      arts[i].path = strdup_safe(path);
    }
  } else if (g->state == STATE_BATTLE) {
    menu_path = resolve_menu_path("battle_screen");
    game_prepare_battle(g, main_map);
    value_map_clear(hero_map);
    character_to_map(&g->hero, hero_map);
    value_map_clear(enemy_maps[0]);
    character_to_map(&g->enemy, enemy_maps[0]);
    art_count = 1;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    char path[256];
    snprintf(path, sizeof(path), "enemyes/%s/_%s", enemy_art_dungeon(g), g->enemy.code);
    const char *battle_art = g->battle_art_name[0] ? g->battle_art_name : "normal";
    arts[0].name = strdup_safe(battle_art);
    arts[0].path = strdup_safe(path);
  } else if (g->state == STATE_CAMPFIRE) {
    menu_path = resolve_menu_path("rest_menu_screen");
    game_prepare_campfire(g, main_map);
    art_count = 1;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    arts[0].name = strdup_safe("camp_fire_big");
    arts[0].path = strdup_safe("_rest");
  } else if (g->state == STATE_CAMP) {
    menu_path = resolve_menu_path("camp_screen");
    game_prepare_camp(g, main_map);
  } else if (g->state == STATE_MONOLITH) {
    menu_path = resolve_menu_path("camp_monolith_screen");
    game_prepare_monolith(g, main_map);
  } else if (g->state == STATE_OCCULT_LIBRARY) {
    menu_path = resolve_menu_path("camp_occult_library_screen");
    game_prepare_occult_library(g, main_map);
  } else if (g->state == STATE_OL_RECIPE) {
    menu_path = resolve_menu_path("camp_ol_recipe_screen");
    game_prepare_recipe_view(g, main_map);
  } else if (g->state == STATE_OL_ENHANCE_LIST) {
    menu_path = resolve_menu_path("enhance_by_recipe_screen");
    game_prepare_enhance_list(g, main_map);
  } else if (g->state == STATE_OL_ENHANCE) {
    menu_path = resolve_menu_path("camp_ol_enhance_screen");
    game_prepare_recipe_enhance(g, main_map);
  } else if (g->state == STATE_STATS_CHOOSE) {
    menu_path = resolve_menu_path("statistics_choose_screen");
    game_prepare_stats_choose(g, main_map);
  } else if (g->state == STATE_STATS_SHOW) {
    menu_path = resolve_menu_path("statistics_enemyes_camp_screen");
    game_prepare_stats_show(g, main_map);
  } else if (g->state == STATE_EVENT_SELECT) {
    if (g->event_choice_count >= 3) menu_path = resolve_menu_path("event_3_choose_screen");
    else if (g->event_choice_count == 2) menu_path = resolve_menu_path("event_2_choose_screen");
    else menu_path = resolve_menu_path("event_1_choose_screen");
    value_map_clear(main_map);
    value_map_set(main_map, "main", g->event_choose_message[0] ? g->event_choose_message : "Choose an event");
    for (int i = 0; i < g->event_choice_count; ++i) {
      value_map_clear(enemy_maps[i]);
      event_to_map(&g->event_choices[i], enemy_maps[i]);
    }
    art_count = (size_t)g->event_choice_count;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    for (size_t i = 0; i < art_count; ++i) {
      arts[i].name = strdup_safe("mini");
      arts[i].path = strdup_safe(g->event_choices[i].art_path);
    }
  } else if (g->state == STATE_EVENT_RESULT) {
    menu_path = resolve_menu_path("messages_screen");
    game_prepare_event_result(g, main_map);
    if (g->event_art_path[0]) {
      art_count = 1;
      arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
      const char *art_name = g->event_art_name[0] ? g->event_art_name : "normal";
      arts[0].name = strdup_safe(art_name);
      arts[0].path = strdup_safe(g->event_art_path);
    }
  } else if (g->state == STATE_OPTIONS) {
    menu_path = resolve_menu_path("options_choose_screen");
    game_prepare_options(g, main_map);
  } else if (g->state == STATE_OPTIONS_ANIM) {
    menu_path = resolve_menu_path("options_animation_speed_screen");
    game_prepare_options_anim(g, main_map);
  } else if (g->state == STATE_OPTIONS_REPLACE) {
    menu_path = resolve_menu_path("options_screen_replacement_type_screen");
    game_prepare_options_replace(g, main_map);
  } else if (g->state == STATE_CREDITS) {
    menu_path = resolve_menu_path("credits_screen");
  } else if (g->state == STATE_LOOT) {
    const char *type = (g->loot_index < g->loot_count) ? g->loot_items[g->loot_index].type : "weapon";
    if (strcmp(type, "weapon") == 0) menu_path = resolve_menu_path("loot_enemy_weapon");
    else if (strcmp(type, "body_armor") == 0) menu_path = resolve_menu_path("loot_enemy_body_armor");
    else if (strcmp(type, "head_armor") == 0) menu_path = resolve_menu_path("loot_enemy_head_armor");
    else if (strcmp(type, "arms_armor") == 0) menu_path = resolve_menu_path("loot_enemy_arms_armor");
    else menu_path = resolve_menu_path("loot_enemy_shield");
    game_prepare_loot(g, main_map, hero_map, enemy_maps[0], &arts, &art_count);
  } else if (g->state == STATE_LOOT_MESSAGE) {
    menu_path = resolve_menu_path("messages_screen");
    game_prepare_loot_message(g, main_map);
    if (g->loot_message_mode == 1) {
      art_count = 1;
      arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
      arts[0].name = strdup_safe("loot_coins");
      arts[0].path = strdup_safe("_loot_coins");
    } else if (g->loot_message_mode == 2) {
      art_count = 1;
      arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
      char path[256];
      snprintf(path, sizeof(path), "enemyes/%s/_%s", enemy_art_dungeon(g), g->enemy.code);
      arts[0].name = strdup_safe("normal");
      arts[0].path = strdup_safe(path);
    }
  } else if (g->state == STATE_SHOP) {
    menu_path = resolve_menu_path("camp_shop_screen");
    game_prepare_shop(g, main_map);
  } else if (g->state == STATE_AMMO_SHOW) {
    if (strcmp(g->ammo_show_type, "weapon") == 0) menu_path = resolve_menu_path("ammunition_weapon_screen");
    else if (strcmp(g->ammo_show_type, "body_armor") == 0) menu_path = resolve_menu_path("ammunition_body_armor_screen");
    else if (strcmp(g->ammo_show_type, "head_armor") == 0) menu_path = resolve_menu_path("ammunition_head_armor_screen");
    else if (strcmp(g->ammo_show_type, "arms_armor") == 0) menu_path = resolve_menu_path("ammunition_arms_armor_screen");
    else menu_path = resolve_menu_path("ammunition_shield_screen");
    ammo_to_map(g, g->ammo_show_type, g->ammo_show_code, main_map);
    art_count = 1;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    char path[256];
    snprintf(path, sizeof(path), "ammunition/%s/_%s", g->ammo_show_type, g->ammo_show_code);
    arts[0].name = strdup_safe("normal");
    arts[0].path = strdup_safe(path);
  } else if (g->state == STATE_HERO_INFO) {
    menu_path = resolve_menu_path("hero_sl_screen");
    game_prepare_hero_info(g, main_map);
    value_map_clear(hero_map);
    character_to_map(&g->hero, hero_map);
    art_count = 1;
    arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
    char path[256];
    snprintf(path, sizeof(path), "dungeons/_%s", g->dungeons[g->dungeon_index].name);
    arts[0].name = strdup_safe("normal");
    arts[0].path = strdup_safe(path);
  } else if (g->state == STATE_SPEND_STAT || g->state == STATE_SPEND_SKILL) {
    menu_path = resolve_menu_path("hero_update_screen");
    if (g->state == STATE_SPEND_STAT) {
      game_prepare_spend_stat(g, main_map);
    } else {
      game_prepare_spend_skill(g, main_map);
    }
    value_map_clear(hero_map);
    character_to_map(&g->hero, hero_map);
  } else if (g->state == STATE_MESSAGE) {
    menu_path = resolve_menu_path("messages_screen");
    game_prepare_message(g, main_map);
    if (g->message_art_path[0]) {
      art_count = 1;
      arts = (ArtArg *)calloc(art_count, sizeof(ArtArg));
      arts[0].name = strdup_safe(g->message_art_name[0] ? g->message_art_name : "normal");
      arts[0].path = strdup_safe(g->message_art_path);
    }
  }

  if (!menu_path) return false;
  *out_menu_path = menu_path;
  *out_arts = arts;
  *out_art_count = art_count;
  return true;
}

static void free_art_args(ArtArg *arts, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    free(arts[i].name);
    free(arts[i].path);
  }
  free(arts);
}

int main(int argc, char **argv) {
  bool static_mode = false;
  const char *static_menu_path_arg = NULL;
  const char *font_path = NULL;
  ValueMap static_map = {0};
  ArtArg *static_arts = NULL;
  size_t static_art_count = 0;

  fprintf(stderr, "[pzdc_dungeon_2_gl] starting...\n");

  for (int i = 1; i < argc; ++i) {
    if (strcmp(argv[i], "--static") == 0) {
      static_mode = true;
      continue;
    }
    if (strcmp(argv[i], "--set") == 0 && i + 1 < argc) {
      char *pair = argv[++i];
      char *eq = strchr(pair, '=');
      if (eq) {
        *eq = '\0';
        value_map_set(&static_map, pair, eq + 1);
        *eq = '=';
      }
      continue;
    }
    if (strcmp(argv[i], "--art") == 0 && i + 1 < argc) {
      char *pair = argv[++i];
      char *eq = strchr(pair, '=');
      if (eq) {
        *eq = '\0';
        ArtArg *arr = (ArtArg *)realloc(static_arts, (static_art_count + 1) * sizeof(ArtArg));
        if (arr) {
          static_arts = arr;
          static_arts[static_art_count].name = strdup_safe(pair);
          static_arts[static_art_count].path = strdup_safe(eq + 1);
          static_art_count++;
        }
        *eq = '=';
      }
      continue;
    }
    if (strcmp(argv[i], "--font") == 0 && i + 1 < argc) {
      font_path = argv[++i];
      continue;
    }
    if (!static_menu_path_arg && argv[i][0] != '-') {
      static_menu_path_arg = argv[i];
      continue;
    }
  }

  srand((unsigned int)time(NULL));

  fprintf(stderr, "[pzdc_dungeon_2_gl] argv parsed (static_mode=%d)\n", static_mode ? 1 : 0);

  const char *version_path = NULL;
  {
    const char *candidates[] = {
        "version.rb",
        "../version.rb",
        "../../version.rb",
    };
    version_path = find_existing_path(candidates, sizeof(candidates) / sizeof(candidates[0]));
  }
  char *version = read_version(version_path ? version_path : "version.rb");
  if (!version) version = strdup_safe("v 0.9.1");

  if (!font_path) font_path = default_font_path();
  if (!font_path) {
    fprintf(stderr, "No font found. Pass a monospace TTF path via --font.\n");
    return 1;
  }

  fprintf(stderr, "[pzdc_dungeon_2_gl] font: %s\n", font_path);

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
    return 1;
  }
  fprintf(stderr, "[pzdc_dungeon_2_gl] SDL_Init OK\n");
  if (TTF_Init() != 0) {
    fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
    SDL_Quit();
    return 1;
  }
  fprintf(stderr, "[pzdc_dungeon_2_gl] TTF_Init OK\n");

  TTF_Font *font = TTF_OpenFont(font_path, 20);
  if (!font) {
    fprintf(stderr, "Failed to load font: %s\n", TTF_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
  }
  fprintf(stderr, "[pzdc_dungeon_2_gl] font loaded\n");

  int cell_w = 0, cell_h = 0;
  TTF_SizeUTF8(font, "M", &cell_w, &cell_h);
  cell_h = TTF_FontHeight(font);
  if (cell_w <= 0 || cell_h <= 0) {
    cell_w = 12;
    cell_h = 20;
  }

  Menu menu = {0};
  RenderState rs = {0};

  if (static_mode) {
    fprintf(stderr, "[pzdc_dungeon_2_gl] static mode\n");
    const char *menu_path = static_menu_path_arg;
    if (!menu_path) {
      const char *candidates[] = {
          "views/menues/start_game_screen.yml",
          "../views/menues/start_game_screen.yml",
          "../../views/menues/start_game_screen.yml",
      };
      menu_path = find_existing_path(candidates, sizeof(candidates) / sizeof(candidates[0]));
      if (!menu_path) menu_path = "views/menues/start_game_screen.yml";
    }
    char *resolved_menu = resolve_menu_path(menu_path);
    if (!resolved_menu) resolved_menu = strdup_safe(menu_path);

    if (!menu_load(resolved_menu, &menu)) {
      fprintf(stderr, "Failed to load menu from %s\n", resolved_menu);
      free(resolved_menu);
      free(version);
      value_map_clear(&static_map);
      free_art_args(static_arts, static_art_count);
      TTF_CloseFont(font);
      TTF_Quit();
      SDL_Quit();
      return 1;
    }
    fprintf(stderr, "[pzdc_dungeon_2_gl] menu loaded: %s\n", resolved_menu);
    value_map_set_if_missing(&static_map, "main", version);
    compose_menu(&menu, &static_map, NULL, 0, static_arts, static_art_count);
    free(resolved_menu);
  }

  Game game;
  ValueMap main_map = {0};
  ValueMap hero_map = {0};
  ValueMap enemy_map1 = {0};
  ValueMap enemy_map2 = {0};
  ValueMap enemy_map3 = {0};
  ValueMap *enemy_maps[3] = {&enemy_map1, &enemy_map2, &enemy_map3};

  if (!static_mode) {
    fprintf(stderr, "[pzdc_dungeon_2_gl] interactive mode\n");
    game_init(&game);
    fprintf(stderr, "[pzdc_dungeon_2_gl] game_init OK\n");
    {
      char cwd_buf[512];
      if (getcwd(cwd_buf, sizeof(cwd_buf))) {
        fprintf(stderr, "[pzdc_dungeon_2_gl] cwd: %s\n", cwd_buf);
      }
    }
    char *heroes_path = resolve_data_path("data/characters/heroes.yml");
    char *bandits_path = resolve_data_path("data/characters/enemyes/bandits.yml");
    char *undeads_path = resolve_data_path("data/characters/enemyes/undeads.yml");
    char *swamp_path = resolve_data_path("data/characters/enemyes/swamp.yml");
    char *events_path = resolve_data_path("data/characters/enemyes/events.yml");
    char *weapons_path = resolve_data_path("data/ammunition/weapon.yml");
    char *body_path = resolve_data_path("data/ammunition/body_armor.yml");
    char *head_path = resolve_data_path("data/ammunition/head_armor.yml");
    char *arms_path = resolve_data_path("data/ammunition/arms_armor.yml");
    char *shield_path = resolve_data_path("data/ammunition/shield.yml");

    fprintf(stderr, "[pzdc_dungeon_2_gl] load heroes: %s\n", heroes_path);
    load_heroes(heroes_path, &game.heroes, &game.hero_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] heroes loaded: %zu\n", game.hero_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load bandits: %s\n", bandits_path);
    load_enemies(bandits_path, &game.dungeons[0].enemies, &game.dungeons[0].enemy_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] bandits loaded: %zu\n", game.dungeons[0].enemy_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load undeads: %s\n", undeads_path);
    load_enemies(undeads_path, &game.dungeons[1].enemies, &game.dungeons[1].enemy_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] undeads loaded: %zu\n", game.dungeons[1].enemy_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load swamp: %s\n", swamp_path);
    load_enemies(swamp_path, &game.dungeons[2].enemies, &game.dungeons[2].enemy_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] swamp loaded: %zu\n", game.dungeons[2].enemy_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load events enemyes: %s\n", events_path);
    load_enemies(events_path, &game.event_enemies, &game.event_enemy_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] events enemyes loaded: %zu\n", game.event_enemy_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load weapons: %s\n", weapons_path);
    load_weapons(weapons_path, &game.weapons, &game.weapon_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] weapons loaded: %zu\n", game.weapon_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load body armor: %s\n", body_path);
    load_armors(body_path, &game.body_armors, &game.body_armor_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] body armor loaded: %zu\n", game.body_armor_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load head armor: %s\n", head_path);
    load_armors(head_path, &game.head_armors, &game.head_armor_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] head armor loaded: %zu\n", game.head_armor_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load arms armor: %s\n", arms_path);
    load_armors(arms_path, &game.arms_armors, &game.arms_armor_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] arms armor loaded: %zu\n", game.arms_armor_count);

    fprintf(stderr, "[pzdc_dungeon_2_gl] load shields: %s\n", shield_path);
    load_shields(shield_path, &game.shields, &game.shield_count);
    fprintf(stderr, "[pzdc_dungeon_2_gl] shields loaded: %zu\n", game.shield_count);

    load_shop_data(&game.shop);
    load_warehouse_data(&game.warehouse);
    shop_fill(&game.shop);
    save_shop_data(&game.shop);
    save_warehouse_data(&game.warehouse);
    load_monolith_data(&game.monolith);
    load_statistics_total(&game.stats_total);
    load_occult_library_data(&game.occult);

    if (game.hero_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load heroes from %s\n", heroes_path);
    if (game.dungeons[0].enemy_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load bandits from %s\n", bandits_path);
    if (game.dungeons[1].enemy_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load undeads from %s\n", undeads_path);
    if (game.dungeons[2].enemy_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load swamp from %s\n", swamp_path);
    if (game.event_enemy_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load events enemyes from %s\n", events_path);
    if (game.weapon_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load weapons from %s\n", weapons_path);
    if (game.body_armor_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load body armor from %s\n", body_path);
    if (game.head_armor_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load head armor from %s\n", head_path);
    if (game.arms_armor_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load arms armor from %s\n", arms_path);
    if (game.shield_count == 0) fprintf(stderr, "[pzdc_dungeon_2_gl] WARN: failed to load shields from %s\n", shield_path);

    free(heroes_path);
    free(bandits_path);
    free(undeads_path);
    free(swamp_path);
    free(events_path);
    free(weapons_path);
    free(body_path);
    free(head_path);
    free(arms_path);
    free(shield_path);
    fprintf(stderr, "[pzdc_dungeon_2_gl] data loaded (heroes=%zu, enemies=%zu/%zu/%zu)\n",
            game.hero_count, game.dungeons[0].enemy_count, game.dungeons[1].enemy_count, game.dungeons[2].enemy_count);

    ArtArg *arts = NULL;
    size_t art_count = 0;
    char *menu_path = NULL;
    if (!game_build_screen(&game, version, &menu, &main_map, &hero_map, enemy_maps, &arts, &art_count, &menu_path)) {
      fprintf(stderr, "Failed to build initial screen.\n");
      game_free(&game);
      free(version);
      TTF_CloseFont(font);
      TTF_Quit();
      SDL_Quit();
      return 1;
    }

    if (!menu_load(menu_path, &menu)) {
      fprintf(stderr, "Failed to load menu from %s\n", menu_path);
      free(menu_path);
      free_art_args(arts, art_count);
      game_free(&game);
      free(version);
      TTF_CloseFont(font);
      TTF_Quit();
      SDL_Quit();
      return 1;
    }
    fprintf(stderr, "[pzdc_dungeon_2_gl] menu loaded: %s\n", menu_path);
    compose_menu(&menu, &main_map, NULL, 0, arts, art_count);
    free(menu_path);
    free_art_args(arts, art_count);
  }

  int win_w = (int)menu.view.max_cols * cell_w;
  int win_h = (int)menu.view.line_count * cell_h;

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);

  SDL_Window *window = SDL_CreateWindow(
      "PZDC OpenGL", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
      win_w, win_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!window) {
    fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
    free_menu(&menu);
    render_state_free(&rs);
    value_map_clear(&static_map);
    value_map_clear(&main_map);
    value_map_clear(&hero_map);
    value_map_clear(&enemy_map1);
    value_map_clear(&enemy_map2);
    value_map_clear(&enemy_map3);
    if (!static_mode) game_free(&game);
    free(version);
    free_art_args(static_arts, static_art_count);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }
  fprintf(stderr, "[pzdc_dungeon_2_gl] window created (%dx%d)\n", win_w, win_h);

  SDL_GLContext gl_ctx = SDL_GL_CreateContext(window);
  if (!gl_ctx) {
    fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    free_menu(&menu);
    render_state_free(&rs);
    value_map_clear(&static_map);
    value_map_clear(&main_map);
    value_map_clear(&hero_map);
    value_map_clear(&enemy_map1);
    value_map_clear(&enemy_map2);
    value_map_clear(&enemy_map3);
    if (!static_mode) game_free(&game);
    free(version);
    free_art_args(static_arts, static_art_count);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
    return 1;
  }
  fprintf(stderr, "[pzdc_dungeon_2_gl] GL context created\n");

  glViewport(0, 0, win_w, win_h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, win_w, win_h, 0, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  build_atlas(&menu, font, cell_w, cell_h, &rs);

  bool running = true;
  bool dirty = false;
  bool text_input_active = false;
  float transition_alpha = 1.0f;
  bool transition_active = false;
  bool typewriter_active = false;
  int typewriter_pos = -1;
  int fade_duration_ms = 200;
  int typewriter_duration_ms = 700;
  uint32_t last_tick = SDL_GetTicks();

  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) running = false;
      if (e.type == SDL_TEXTINPUT) {
        if (!static_mode && game.state == STATE_NAME_INPUT) {
          append_text(game.name_input, &game.name_len, NAME_MAX_LEN + 1, e.text.text);
          game.name_error[0] = '\0';
          dirty = true;
        } else if (!static_mode && game.state == STATE_EVENT_RESULT && game.event_input_mode == EVENT_INPUT_TEXT) {
          append_text(game.event_text, &game.event_text_len, sizeof(game.event_text), e.text.text);
          dirty = true;
        }
      }
      if (e.type == SDL_KEYDOWN) {
        SDL_Keycode key = e.key.keysym.sym;
        if (key == SDLK_ESCAPE) running = false;
        if (static_mode) continue;

        int digit = key_to_digit(key);
        if (game.state == STATE_START) {
          if (digit == 1) {
            game.state = STATE_LOAD_MENU;
            dirty = true;
          } else if (digit == 0) {
            running = false;
          } else if (digit == 2) {
            game.state = STATE_CAMP;
            dirty = true;
          } else if (digit == 3) {
            game.state = STATE_OPTIONS;
            dirty = true;
          } else if (digit == 4) {
            game.state = STATE_CREDITS;
            dirty = true;
          }
        } else if (game.state == STATE_LOAD_MENU) {
          if (digit == 1) {
            if (load_hero_in_run(&game)) {
              game.dungeon_index = dungeon_index_by_name(&game, game.hero.dungeon_name);
              game.state = STATE_LOAD_CONFIRM;
            } else {
              game.state = STATE_LOAD_NO_HERO;
            }
            dirty = true;
          } else if (digit == 2) {
            game.state = STATE_CHOOSE_DUNGEON;
            dirty = true;
          } else if (digit == 0) {
            game.state = STATE_START;
            dirty = true;
          }
        } else if (game.state == STATE_LOAD_NO_HERO) {
          if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            game.state = STATE_LOAD_MENU;
            dirty = true;
          }
        } else if (game.state == STATE_CHOOSE_DUNGEON) {
          if (digit >= 1 && digit <= 3) {
            game.dungeon_index = digit - 1;
            game.name_len = 0;
            game.name_input[0] = '\0';
            game.name_error[0] = '\0';
            game.state = STATE_NAME_INPUT;
            dirty = true;
          } else if (digit == 0) {
            game.state = STATE_LOAD_MENU;
            dirty = true;
          }
        } else if (game.state == STATE_NAME_INPUT) {
          if (key == SDLK_BACKSPACE) {
            backspace_text(game.name_input, &game.name_len);
            game.name_error[0] = '\0';
            dirty = true;
          } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            char tmp[32];
            snprintf(tmp, sizeof(tmp), "%s", game.name_input);
            trim_both_inplace(tmp);
            if (tmp[0] == '\0') {
              snprintf(game.name_error, sizeof(game.name_error), "The name must contain at least one letter");
              dirty = true;
            } else if (strlen(tmp) > NAME_MAX_LEN) {
              snprintf(game.name_error, sizeof(game.name_error), "%s is an incorrect name. The name must be no more than 20 characters", tmp);
              dirty = true;
            } else if (!str_has_letter(tmp)) {
              snprintf(game.name_error, sizeof(game.name_error), "%s is an incorrect name. The name must contain at least one letter", tmp);
              dirty = true;
            } else {
              snprintf(game.name_input, sizeof(game.name_input), "%s", tmp);
              game.name_len = strlen(game.name_input);
              game.name_error[0] = '\0';
              game.state = STATE_HERO_SELECT;
              dirty = true;
            }
          } else if (digit == 0) {
            game.state = STATE_CHOOSE_DUNGEON;
            dirty = true;
          }
        } else if (game.state == STATE_HERO_SELECT) {
          if (digit >= 1 && (size_t)digit <= game.hero_count) {
            const char *hero_name = game.name_input[0] ? game.name_input : "Hero";
            game.hero = character_from_hero(&game, &game.heroes[digit - 1], hero_name);
            snprintf(game.hero.dungeon_name, sizeof(game.hero.dungeon_name), "%s", game.dungeons[game.dungeon_index].name);
            game.hero.dungeon_part_number = 1;
            game.hero.leveling = 0;
            apply_monolith_bonuses(&game.monolith, &game.hero);
            apply_statistics_bonuses(&game.stats_total, &game, &game.hero);
            apply_warehouse_bonuses(&game, &game.hero);
            if (strcmp(game.name_input, "BAMBUGA") == 0) {
              game.hero.weapon = weapon_from_code(&game, "bambuga");
              snprintf(game.hero.name, sizeof(game.hero.name), "Cheater");
            }
            game.wg_taken = 0;
            game.wg_enemy[0] = '\0';
            game.wg_count = 0;
            game.wg_level = 0;
            game.hero_selected = 1;
            game.state = STATE_SKILL_ACTIVE;
            dirty = true;
          } else if (digit == 0) {
            game.state = STATE_NAME_INPUT;
            dirty = true;
          }
        } else if (game.state == STATE_LOAD_CONFIRM) {
          if (digit == 1) {
            game.hero_selected = 1;
            pick_random_enemies(&game);
            game.state = STATE_ENEMY_SELECT;
            dirty = true;
          } else if (digit == 0) {
            game.state = STATE_LOAD_MENU;
            dirty = true;
          }
        } else if (game.state == STATE_CAMP) {
          if (digit == 1) {
            game.state = STATE_MONOLITH;
            dirty = true;
          } else if (digit == 2) {
            game.state = STATE_SHOP;
            dirty = true;
          } else if (digit == 3) {
            game.state = STATE_OCCULT_LIBRARY;
            dirty = true;
          } else if (digit == 4) {
            game.state = STATE_STATS_CHOOSE;
            dirty = true;
          } else if (digit == 0) {
            game.state = STATE_START;
            dirty = true;
          }
        } else if (game.state == STATE_MONOLITH) {
          if (digit == 0) {
            game.state = STATE_CAMP;
            dirty = true;
          } else if (digit >= 1 && digit <= 11) {
            const char *stats[] = {"hp","mp","accuracy","damage","stat_points","skill_points","armor","regen_hp","regen_mp","armor_penetration","block_chance"};
            const char *key = stats[digit - 1];
            if (monolith_buy(&game.monolith, key)) {
              save_monolith_data(&game.monolith);
            } else {
              snprintf(game.message_title, sizeof(game.message_title), "PZDC Monolith");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "Not enough points");
              game.next_state = STATE_MONOLITH;
              game.state = STATE_MESSAGE;
            }
            dirty = true;
          }
        } else if (game.state == STATE_OCCULT_LIBRARY) {
          char letter = key_to_letter(key);
          if (digit == 0) {
            game.state = STATE_CAMP;
            dirty = true;
          } else if (digit >= 1 && digit <= 24) {
            OccultRecipe *r = occult_recipe_by_view_code(&game.occult, digit);
            if (!r) {
              snprintf(game.message_title, sizeof(game.message_title), "Occult Library");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "No recipe on this line");
              game.next_state = STATE_OCCULT_LIBRARY;
              game.state = STATE_MESSAGE;
              dirty = true;
            } else if (r->purchased) {
              snprintf(game.message_title, sizeof(game.message_title), "Occult Library");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "Already purchased");
              game.next_state = STATE_OCCULT_LIBRARY;
              game.state = STATE_MESSAGE;
              dirty = true;
            } else if (game.warehouse.coins < r->price) {
              snprintf(game.message_title, sizeof(game.message_title), "Occult Library");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "Not enough coins");
              game.next_state = STATE_OCCULT_LIBRARY;
              game.state = STATE_MESSAGE;
              dirty = true;
            } else {
              game.warehouse.coins -= r->price;
              r->purchased = true;
              save_occult_library_data(&game.occult);
              save_warehouse_data(&game.warehouse);
              snprintf(game.message_title, sizeof(game.message_title), "Occult Library");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "Recipe purchased");
              game.next_state = STATE_OCCULT_LIBRARY;
              game.state = STATE_MESSAGE;
              dirty = true;
            }
          } else if (letter) {
            int idx = (int)(letter - 'a') + 1;
            OccultRecipe *r = occult_recipe_by_view_code(&game.occult, idx);
            if (r) {
              game.current_recipe_index = (int)(r - game.occult.recipes);
              game.return_state = STATE_OCCULT_LIBRARY;
              game.state = STATE_OL_RECIPE;
              dirty = true;
            }
          }
        } else if (game.state == STATE_OL_RECIPE) {
          if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            GameState back = (game.return_state == STATE_OL_ENHANCE_LIST || game.return_state == STATE_OCCULT_LIBRARY)
                               ? game.return_state : STATE_OCCULT_LIBRARY;
            game.state = back;
            dirty = true;
          }
        } else if (game.state == STATE_OL_ENHANCE_LIST) {
          char letter = key_to_letter(key);
          if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            game.state = STATE_CAMPFIRE;
            dirty = true;
          } else if (letter) {
            size_t count = 0;
            size_t *indices = occult_accessible_indices(&game.occult, &count);
            int idx = (int)(letter - 'a');
            if ((size_t)idx < count) {
              game.current_recipe_index = (int)indices[idx];
              OccultRecipe *r = &game.occult.recipes[indices[idx]];
              if (recipe_hero_has_ingredients(r, &game.hero)) {
                game.state = STATE_OL_ENHANCE;
              } else {
                game.return_state = STATE_OL_ENHANCE_LIST;
                game.state = STATE_OL_RECIPE;
              }
              dirty = true;
            }
            free(indices);
          }
        } else if (game.state == STATE_OL_ENHANCE) {
          char letter = key_to_letter(key);
          if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            game.state = STATE_OL_ENHANCE_LIST;
            dirty = true;
          } else if (letter) {
            const char *type = NULL;
            if (letter == 'a') type = "weapon";
            else if (letter == 'b') type = "head_armor";
            else if (letter == 'c') type = "body_armor";
            else if (letter == 'd') type = "arms_armor";
            else if (letter == 'e') type = "shield";
            if (type) {
              const char *code = "without";
              if (strcmp(type, "weapon") == 0) code = game.hero.weapon.code;
              else if (strcmp(type, "head_armor") == 0) code = game.hero.head_armor.code;
              else if (strcmp(type, "body_armor") == 0) code = game.hero.body_armor.code;
              else if (strcmp(type, "arms_armor") == 0) code = game.hero.arms_armor.code;
              else if (strcmp(type, "shield") == 0) code = game.hero.shield.code;
              if (strcmp(code, "without") != 0) {
                snprintf(game.ammo_show_type, sizeof(game.ammo_show_type), "%s", type);
                snprintf(game.ammo_show_code, sizeof(game.ammo_show_code), "%s", code);
                game.return_state = STATE_OL_ENHANCE;
                game.state = STATE_AMMO_SHOW;
                dirty = true;
              }
            }
          } else if (digit >= 1 && digit <= 5) {
            OccultRecipe *r = (game.current_recipe_index >= 0 && (size_t)game.current_recipe_index < game.occult.recipe_count)
                                ? &game.occult.recipes[game.current_recipe_index] : NULL;
            if (!r) {
              game.state = STATE_OL_ENHANCE_LIST;
              dirty = true;
            } else if (!recipe_hero_has_ingredients(r, &game.hero)) {
              snprintf(game.message_title, sizeof(game.message_title), "Occult Library");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "Not enough ingredients");
              game.next_state = STATE_OL_ENHANCE;
              game.state = STATE_MESSAGE;
              dirty = true;
            } else {
              if (digit == 1) recipe_apply_weapon(r, &game.hero.weapon);
              else if (digit == 2) recipe_apply_armor(r, &game.hero.head_armor, &r->head_armor);
              else if (digit == 3) recipe_apply_armor(r, &game.hero.body_armor, &r->body_armor);
              else if (digit == 4) recipe_apply_armor(r, &game.hero.arms_armor, &r->arms_armor);
              else if (digit == 5) recipe_apply_shield(r, &game.hero.shield);
              recipe_consume_ingredients(r, &game.hero);
              snprintf(game.message_title, sizeof(game.message_title), "Occult Library");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "Ammunition enhanced");
              game.next_state = STATE_OL_ENHANCE;
              game.state = STATE_MESSAGE;
              dirty = true;
            }
          }
        } else if (game.state == STATE_STATS_CHOOSE) {
          if (digit == 0) {
            game.state = STATE_CAMP;
            dirty = true;
          } else if (digit >= 1 && digit <= 3) {
            game.stats_dungeon_index = digit - 1;
            game.state = STATE_STATS_SHOW;
            dirty = true;
          }
        } else if (game.state == STATE_STATS_SHOW) {
          if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            game.state = STATE_STATS_CHOOSE;
            dirty = true;
          }
        } else if (game.state == STATE_EVENT_SELECT) {
          if (digit == 0) {
            game.hero.dungeon_part_number += 1;
            logbuffer_clear(&game.log);
            hero_rest(&game.hero, &game.log);
            game.state = STATE_CAMPFIRE;
            dirty = true;
          } else if (digit >= 1 && digit <= game.event_choice_count) {
            game.current_event = game.event_choices[digit - 1];
            event_begin(&game, &game.current_event);
            dirty = true;
          }
        } else if (game.state == STATE_EVENT_RESULT) {
          if (game.event_input_mode == EVENT_INPUT_TEXT) {
            if (key == SDLK_BACKSPACE) {
              backspace_text(game.event_text, &game.event_text_len);
              dirty = true;
            } else if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
              event_handle_text(&game, game.event_text);
              dirty = true;
            }
          } else if (game.event_input_mode == EVENT_INPUT_DIGIT) {
            if (digit >= 0) {
              event_handle_digit(&game, digit);
              dirty = true;
            }
          } else {
            if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
              event_handle_digit(&game, digit);
              dirty = true;
            }
          }
        } else if (game.state == STATE_OPTIONS) {
          if (digit == 1) {
            game.state = STATE_OPTIONS_ANIM;
            dirty = true;
          } else if (digit == 2) {
            game.state = STATE_OPTIONS_REPLACE;
            dirty = true;
          } else if (digit == 0) {
            game.state = STATE_START;
            dirty = true;
          }
        } else if (game.state == STATE_OPTIONS_ANIM) {
          if (digit >= 1 && digit <= 5) {
            game.anim_speed_index = digit - 1;
            dirty = true;
          } else if (digit == 0) {
            game.state = STATE_OPTIONS;
            dirty = true;
          }
        } else if (game.state == STATE_OPTIONS_REPLACE) {
          if (digit >= 1 && digit <= 3) {
            game.screen_replace_type = digit - 1;
            dirty = true;
          } else if (digit == 0) {
            game.state = STATE_OPTIONS;
            dirty = true;
          }
        } else if (game.state == STATE_CREDITS) {
          if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            game.state = STATE_START;
            dirty = true;
          }
        } else if (game.state == STATE_LOOT) {
          char letter = key_to_letter(key);
          if (letter == 'y' || letter == 'n') {
            game.loot_last_taken = (letter == 'y') ? 1 : 0;
            const LootEntry *le = (game.loot_index < game.loot_count) ? &game.loot_items[game.loot_index] : NULL;
            if (le && letter == 'y') {
              if (strcmp(le->type, "weapon") == 0) game.hero.weapon = weapon_from_code(&game, le->code);
              else if (strcmp(le->type, "body_armor") == 0) game.hero.body_armor = armor_from_code(game.body_armors, game.body_armor_count, le->code);
              else if (strcmp(le->type, "head_armor") == 0) game.hero.head_armor = armor_from_code(game.head_armors, game.head_armor_count, le->code);
              else if (strcmp(le->type, "arms_armor") == 0) game.hero.arms_armor = armor_from_code(game.arms_armors, game.arms_armor_count, le->code);
              else if (strcmp(le->type, "shield") == 0) game.hero.shield = shield_from_code(&game, le->code);
            }
            game.loot_index += 1;
            loot_advance(&game);
            dirty = true;
          }
        } else if (game.state == STATE_LOOT_MESSAGE) {
          if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            game.loot_message_mode = 0;
            loot_advance(&game);
            dirty = true;
          }
        } else if (game.state == STATE_SHOP) {
          char letter = key_to_letter(key);
          if (digit == 0) {
            game.state = STATE_CAMP;
            dirty = true;
          } else if (digit >= 1 && digit <= 15) {
            const char *type = NULL;
            int idx = 0;
            if (digit <= 3) { type = "weapon"; idx = digit - 1; }
            else if (digit <= 6) { type = "body_armor"; idx = digit - 4; }
            else if (digit <= 9) { type = "head_armor"; idx = digit - 7; }
            else if (digit <= 12) { type = "arms_armor"; idx = digit - 10; }
            else { type = "shield"; idx = digit - 13; }
            char *arr = NULL;
            if (strcmp(type, "weapon") == 0) arr = game.shop.weapon[idx];
            else if (strcmp(type, "body_armor") == 0) arr = game.shop.body_armor[idx];
            else if (strcmp(type, "head_armor") == 0) arr = game.shop.head_armor[idx];
            else if (strcmp(type, "arms_armor") == 0) arr = game.shop.arms_armor[idx];
            else arr = game.shop.shield[idx];
            if (strcmp(arr, "without") == 0) {
              snprintf(game.message_title, sizeof(game.message_title), "Shop");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "Empty slot");
              game.next_state = STATE_SHOP;
              game.state = STATE_MESSAGE;
              dirty = true;
            } else {
              int price = ammo_price(&game, type, arr);
              if (game.warehouse.coins < price) {
                snprintf(game.message_title, sizeof(game.message_title), "Shop");
                logbuffer_clear(&game.log);
                logbuffer_push(&game.log, "Not enough coins");
                game.next_state = STATE_SHOP;
                game.state = STATE_MESSAGE;
                dirty = true;
              } else {
                game.warehouse.coins -= price;
                if (strcmp(type, "weapon") == 0) snprintf(game.warehouse.weapon, sizeof(game.warehouse.weapon), "%s", arr);
                else if (strcmp(type, "body_armor") == 0) snprintf(game.warehouse.body_armor, sizeof(game.warehouse.body_armor), "%s", arr);
                else if (strcmp(type, "head_armor") == 0) snprintf(game.warehouse.head_armor, sizeof(game.warehouse.head_armor), "%s", arr);
                else if (strcmp(type, "arms_armor") == 0) snprintf(game.warehouse.arms_armor, sizeof(game.warehouse.arms_armor), "%s", arr);
                else snprintf(game.warehouse.shield, sizeof(game.warehouse.shield), "%s", arr);
                snprintf(arr, 32, "without");
                save_shop_data(&game.shop);
                save_warehouse_data(&game.warehouse);
                snprintf(game.message_title, sizeof(game.message_title), "Shop");
                logbuffer_clear(&game.log);
                logbuffer_push(&game.log, "Item purchased");
                game.next_state = STATE_SHOP;
                game.state = STATE_MESSAGE;
                dirty = true;
              }
            }
          } else if (letter) {
            const char *type = NULL;
            const char *code = NULL;
            if (letter >= 'a' && letter <= 'o') {
              int idx = letter - 'a';
              if (idx <= 2) { type = "weapon"; code = game.shop.weapon[idx]; }
              else if (idx <= 5) { type = "body_armor"; code = game.shop.body_armor[idx - 3]; }
              else if (idx <= 8) { type = "head_armor"; code = game.shop.head_armor[idx - 6]; }
              else if (idx <= 11) { type = "arms_armor"; code = game.shop.arms_armor[idx - 9]; }
              else { type = "shield"; code = game.shop.shield[idx - 12]; }
            } else if (letter == 'v') { type = "weapon"; code = game.warehouse.weapon; }
            else if (letter == 'w') { type = "body_armor"; code = game.warehouse.body_armor; }
            else if (letter == 'x') { type = "head_armor"; code = game.warehouse.head_armor; }
            else if (letter == 'y') { type = "arms_armor"; code = game.warehouse.arms_armor; }
            else if (letter == 'z') { type = "shield"; code = game.warehouse.shield; }
            if (type && code && strcmp(code, "without") != 0) {
              snprintf(game.ammo_show_type, sizeof(game.ammo_show_type), "%s", type);
              snprintf(game.ammo_show_code, sizeof(game.ammo_show_code), "%s", code);
              game.return_state = STATE_SHOP;
              game.state = STATE_AMMO_SHOW;
              dirty = true;
            } else if (type && code && strcmp(code, "without") == 0) {
              snprintf(game.message_title, sizeof(game.message_title), "Shop");
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "Nothing to show");
              game.next_state = STATE_SHOP;
              game.state = STATE_MESSAGE;
              dirty = true;
            }
          }
        } else if (game.state == STATE_AMMO_SHOW) {
          if (digit == 0 || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
            game.state = game.return_state;
            dirty = true;
          }
        } else if (game.state == STATE_SKILL_ACTIVE) {
          const char *skills[] = {"ascetic_strike", "precise_strike", "strong_strike", "traumatic_strike"};
          if (digit >= 1 && digit <= 4) {
            skill_assign(&game.hero.active_skill, SKILL_ACTIVE, skills[digit - 1]);
            game.state = STATE_SKILL_PASSIVE;
            dirty = true;
          }
        } else if (game.state == STATE_SKILL_PASSIVE) {
          const char *skills[] = {"berserk", "concentration", "dazed", "shield_master"};
          if (digit >= 1 && digit <= 4) {
            skill_assign(&game.hero.passive_skill, SKILL_PASSIVE, skills[digit - 1]);
            game.state = STATE_SKILL_CAMP;
            dirty = true;
          }
        } else if (game.state == STATE_SKILL_CAMP) {
          const char *skills[] = {"bloody_ritual", "first_aid", "treasure_hunter"};
          if (digit >= 1 && digit <= 3) {
            skill_assign(&game.hero.camp_skill, SKILL_CAMP, skills[digit - 1]);
            pick_random_enemies(&game);
            game.state = STATE_ENEMY_SELECT;
            dirty = true;
          }
        } else if (game.state == STATE_ENEMY_SELECT) {
          if (digit >= 1 && digit <= game.enemy_choice_count) {
            game.enemy = game.enemy_choices[digit - 1];
            game.enemy_is_boss = game.enemy_choice_is_boss[digit - 1];
            logbuffer_clear(&game.log);
            snprintf(game.battle_art_name, sizeof(game.battle_art_name), "normal");
            game.battle_art_dungeon[0] = '\0';
            game.battle_anim_active = 0;
            game.battle_anim_step = 0;
            game.battle_anim_count = 0;
            game.battle_anim_deadline = 0;
            game.battle_exit_pending = 0;
            game.state = STATE_BATTLE;
            dirty = true;
          } else if (digit == 0) {
            logbuffer_clear(&game.log);
            hero_rest(&game.hero, &game.log);
            game.state = STATE_CAMPFIRE;
            dirty = true;
          }
        } else if (game.state == STATE_BATTLE) {
          if (game.battle_anim_active || game.battle_exit_pending) {
            // ignore input while battle animation plays
          } else if (digit >= 1 && digit <= 4) {
            int enemy_attack_type = 0;
            battle_round(&game, digit, &enemy_attack_type);
            const bool enemy_dead = (game.enemy.hp <= 0);
            const bool hero_dead = (game.hero.hp <= 0);

            if (enemy_dead) {
              snprintf(game.message_title, sizeof(game.message_title), "Enemy defeated");
              logbuffer_clear(&game.log);
              hero_add_exp(&game.hero, game.enemy.exp_gived, &game.log);
              stats_total_increment(&game.stats_total, game.dungeons[game.dungeon_index].name, game.enemy.code);
              save_statistics_total(&game.stats_total);
              int points = monolith_points_from_enemy(&game.hero, &game.enemy);
              if (points > 0) {
                game.hero.pzdc_monolith_points += points;
                char msg[128];
                snprintf(msg, sizeof(msg), "PZDC Monolith gained %d point(s)", points);
                logbuffer_push(&game.log, msg);
              }
              if (game.enemy_is_boss) {
                end_run_transfer(&game, true);
                snprintf(game.message_title, sizeof(game.message_title), "Dungeon completed");
                snprintf(game.message_art_name, sizeof(game.message_art_name), "dungeon_completed");
                snprintf(game.message_art_path, sizeof(game.message_art_path), "_game_over");
                game.next_state = STATE_START;
                game.battle_exit_state = STATE_MESSAGE;
              } else {
                loot_setup(&game);
                if (game.loot_count > 0 || game.loot_show_coins || game.loot_show_ingredient) {
                  loot_advance(&game);
                  game.battle_exit_state = game.state;
                  game.state = STATE_BATTLE;
                } else {
                  game.pending_levelup = 1;
                  game.next_state = STATE_CAMPFIRE;
                  game.battle_exit_state = STATE_MESSAGE;
                }
              }
              game.battle_exit_pending = 1;
            } else if (hero_dead) {
              snprintf(game.message_title, sizeof(game.message_title), "You are dead");
              logbuffer_clear(&game.log);
              end_run_transfer(&game, false);
              logbuffer_push(&game.log, "Your run has ended. Camp loot saved.");
              snprintf(game.message_art_name, sizeof(game.message_art_name), "game_over");
              snprintf(game.message_art_path, sizeof(game.message_art_path), "_game_over");
              game.next_state = STATE_START;
              game.battle_exit_state = STATE_MESSAGE;
              game.battle_exit_pending = 1;
            }

            const char *seq[3];
            int seq_count = 0;
            if (enemy_dead) {
              seq[0] = "damaged";
              seq[1] = "dead";
              seq_count = 2;
            } else {
              seq[0] = "damaged";
              seq[1] = "normal";
              seq[2] = enemy_attack_art_from_type(enemy_attack_type);
              seq_count = 3;
            }
            battle_anim_queue(&game, seq, seq_count);
            dirty = true;
          }
        } else if (game.state == STATE_CAMPFIRE) {
          if (digit == 1) {
            game.state = STATE_HERO_INFO;
            dirty = true;
          } else if (digit == 2) {
            if (game.hero.stat_points > 0) {
              game.stat_roll = 0;
              game.state = STATE_SPEND_STAT;
              dirty = true;
            } else {
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "No stat points to spend");
              dirty = true;
            }
          } else if (digit == 3) {
            if (game.hero.skill_points > 0) {
              game.skill_choice_count = 0;
              game.state = STATE_SPEND_SKILL;
              dirty = true;
            } else {
              logbuffer_clear(&game.log);
              logbuffer_push(&game.log, "No skill points to spend");
              dirty = true;
            }
          } else if (digit == 4) {
            game_use_camp_skill(&game);
            dirty = true;
          } else if (digit == 5) {
            game.state = STATE_OL_ENHANCE_LIST;
            dirty = true;
          } else if (digit == 6) {
            save_hero_in_run(&game);
            snprintf(game.message_title, sizeof(game.message_title), "Game saved");
            logbuffer_clear(&game.log);
            logbuffer_push(&game.log, "You can resume from the main menu");
            game.next_state = STATE_START;
            game.state = STATE_MESSAGE;
            dirty = true;
          } else if (digit == 7) {
            end_run_transfer(&game, game.hero.hp > 0);
            snprintf(game.message_title, sizeof(game.message_title), "Run ended");
            logbuffer_clear(&game.log);
            logbuffer_push(&game.log, "Camp loot and monolith points transferred");
            game.next_state = STATE_START;
            game.state = STATE_MESSAGE;
            dirty = true;
          } else if (digit == 0) {
            if (game.hero.dungeon_part_number % 2 == 0) {
              pick_random_events(&game);
              game.state = STATE_EVENT_SELECT;
            } else {
              pick_random_enemies(&game);
              game.state = STATE_ENEMY_SELECT;
            }
            dirty = true;
          }
        } else if (game.state == STATE_HERO_INFO) {
          if (digit == 0) {
            game.state = STATE_CAMPFIRE;
            dirty = true;
          }
        } else if (game.state == STATE_SPEND_STAT) {
          if (digit == 0) {
            game.state = STATE_CAMPFIRE;
            dirty = true;
          } else if (digit == 1) {
            game.hero.hp_max += 5;
            game.hero.hp += 5;
            game.hero.stat_points -= 1;
            game.stat_roll = 0;
            dirty = true;
          } else if (digit == 2) {
            game.hero.mp_max += 5;
            game.hero.mp += 5;
            game.hero.stat_points -= 1;
            game.stat_roll = 0;
            dirty = true;
          } else if (digit == 3 && game.stat_roll >= 8) {
            game.hero.accuracy_base += 1;
            game.hero.stat_points -= 1;
            game.stat_roll = 0;
            dirty = true;
          } else if (digit == 4 && game.stat_roll >= 11) {
            if (game.hero.min_dmg_base < game.hero.max_dmg_base && rand_range(0, 1) == 0) {
              game.hero.min_dmg_base += 1;
            } else {
              game.hero.max_dmg_base += 1;
            }
            game.hero.stat_points -= 1;
            game.stat_roll = 0;
            dirty = true;
          }
          if (game.state == STATE_SPEND_STAT && game.hero.stat_points <= 0) {
            game.state = STATE_CAMPFIRE;
            dirty = true;
          }
        } else if (game.state == STATE_SPEND_SKILL) {
          if (digit == 0) {
            game.state = STATE_CAMPFIRE;
            dirty = true;
          } else if (digit >= 1 && digit <= game.skill_choice_count) {
            SkillType chosen = game.skill_choices[digit - 1];
            if (chosen == SKILL_ACTIVE) game.hero.active_skill.lvl += 1;
            else if (chosen == SKILL_PASSIVE) game.hero.passive_skill.lvl += 1;
            else if (chosen == SKILL_CAMP) game.hero.camp_skill.lvl += 1;
            game.hero.skill_points -= 1;
            game.skill_choice_count = 0;
            dirty = true;
            if (game.hero.skill_points <= 0) {
              game.state = STATE_CAMPFIRE;
            }
          }
        } else if (game.state == STATE_MESSAGE) {
          game.state = game.next_state;
          if (game.state == STATE_ENEMY_SELECT) pick_random_enemies(&game);
          if (game.state == STATE_CAMPFIRE) {
            if (game.pending_levelup) {
              game.hero.leveling += 1;
              game.hero.dungeon_part_number += 1;
              game.pending_levelup = 0;
            }
            logbuffer_clear(&game.log);
            hero_rest(&game.hero, &game.log);
          }
          game.message_art_name[0] = '\0';
          game.message_art_path[0] = '\0';
          dirty = true;
        }
      }
      if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
        win_w = e.window.data1;
        win_h = e.window.data2;
        glViewport(0, 0, win_w, win_h);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, win_w, win_h, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
      }
    }

    if (!static_mode) {
      bool want_text = (game.state == STATE_NAME_INPUT) ||
                       (game.state == STATE_EVENT_RESULT && game.event_input_mode == EVENT_INPUT_TEXT);
      if (want_text && !text_input_active) {
        SDL_StartTextInput();
        text_input_active = true;
      } else if (!want_text && text_input_active) {
        SDL_StopTextInput();
        text_input_active = false;
      }
    }

    if (!static_mode) {
      uint32_t now_anim = SDL_GetTicks();
      if (battle_anim_tick(&game, now_anim)) {
        dirty = true;
      }
    }

    if (!static_mode && dirty) {
      ArtArg *arts = NULL;
      size_t art_count = 0;
      char *menu_path = NULL;
      if (game_build_screen(&game, version, &menu, &main_map, &hero_map, enemy_maps, &arts, &art_count, &menu_path)) {
        free_menu(&menu);
        menu_load(menu_path, &menu);
        ValueMap *partial_maps[3] = {0};
        size_t partial_count = 0;

        if (game.state == STATE_BATTLE) {
          partial_maps[0] = &hero_map;
          partial_maps[1] = &enemy_map1;
          partial_count = 2;
        } else if (game.state == STATE_ENEMY_SELECT) {
          for (int i = 0; i < game.enemy_choice_count; ++i) partial_maps[i] = enemy_maps[i];
          partial_count = (size_t)game.enemy_choice_count;
        } else if (game.state == STATE_EVENT_SELECT) {
          for (int i = 0; i < game.event_choice_count; ++i) partial_maps[i] = enemy_maps[i];
          partial_count = (size_t)game.event_choice_count;
        } else if (game.state == STATE_LOOT) {
          partial_maps[0] = &hero_map;
          partial_maps[1] = &enemy_map1;
          partial_count = 2;
        } else if (game.state == STATE_HERO_INFO || game.state == STATE_SPEND_STAT || game.state == STATE_SPEND_SKILL || game.state == STATE_LOAD_CONFIRM) {
          partial_maps[0] = &hero_map;
          partial_maps[1] = &hero_map;
          partial_count = 2;
        }

        compose_menu(&menu, &main_map, partial_maps, partial_count, arts, art_count);
        build_atlas(&menu, font, cell_w, cell_h, &rs);
        {
          const int speeds[] = {100, 400, 700, 1000, 1500};
          int idx = game.anim_speed_index;
          if (idx < 0) idx = 0;
          if (idx > 4) idx = 4;
          fade_duration_ms = speeds[idx];
          typewriter_duration_ms = speeds[idx];
        }
        int replace_type = game.screen_replace_type;
        if (game.force_instant_redraw) replace_type = 0;
        if (replace_type == 0) {
          transition_alpha = 1.0f;
          transition_active = false;
          typewriter_active = false;
          typewriter_pos = -1;
        } else if (replace_type == 1) {
          transition_alpha = 0.0f;
          transition_active = true;
          typewriter_active = false;
          typewriter_pos = -1;
        } else {
          transition_alpha = 1.0f;
          transition_active = false;
          typewriter_active = true;
          typewriter_pos = 0;
        }
        game.force_instant_redraw = 0;
      }
      free(menu_path);
      free_art_args(arts, art_count);
      dirty = false;
    }

    uint32_t now = SDL_GetTicks();
    uint32_t dt = now - last_tick;
    last_tick = now;
    if (transition_active) {
      transition_alpha += (float)dt / (float)(fade_duration_ms > 0 ? fade_duration_ms : 200);
      if (transition_alpha >= 1.0f) {
        transition_alpha = 1.0f;
        transition_active = false;
      }
    }
    if (typewriter_active) {
      int total = rs.grid_w * rs.grid_h;
      if (typewriter_pos < total) {
        float step = (float)total * ((float)dt / (float)(typewriter_duration_ms > 0 ? typewriter_duration_ms : 700));
        int add = (int)ceilf(step);
        if (add < 1) add = 1;
        typewriter_pos += add;
        if (typewriter_pos >= total) {
          typewriter_pos = total;
          typewriter_active = false;
        }
      } else {
        typewriter_active = false;
      }
    }
    int max_chars = typewriter_active ? typewriter_pos : -1;
    draw_menu(&menu, &rs, win_w, win_h, cell_w, cell_h, transition_alpha, max_chars);
    SDL_GL_SwapWindow(window);
    SDL_Delay(16);
  }

  render_state_free(&rs);
  free_menu(&menu);
  value_map_clear(&static_map);
  value_map_clear(&main_map);
  value_map_clear(&hero_map);
  value_map_clear(&enemy_map1);
  value_map_clear(&enemy_map2);
  value_map_clear(&enemy_map3);
  if (!static_mode) game_free(&game);
  free(version);
  free_art_args(static_arts, static_art_count);

  SDL_GL_DeleteContext(gl_ctx);
  SDL_DestroyWindow(window);
  TTF_CloseFont(font);
  TTF_Quit();
  SDL_Quit();
  return 0;
}
