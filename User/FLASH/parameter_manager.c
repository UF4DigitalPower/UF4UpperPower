#include "parameter_manager.h"

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "bsp_lcd.h"
#include "board_model.h"
#include "w25q64.h"
#include "w25qxx_port.h"

#define PARAM_FLASH_BASE_ADDR        0x00000000UL
#define PARAM_FLASH_SECTOR_SIZE      0x1000U
#define PARAM_FLASH_SECTOR_COUNT     2U
#define PARAM_MAGIC                  0x55463455UL
#define PARAM_VERSION                6U
#define PARAM_SAVE_DEBOUNCE_MS       200U
#define PARAM_PERSISTED_SETTING_COUNT 5U
#define PARAM_HOME_PRESET_BYTES      (sizeof(ui_home_preset_t) * UI_HOME_PRESET_COUNT)

typedef struct __attribute__((packed))
{
  uint32_t magic;
  uint16_t version;
  uint16_t length;
  uint32_t sequence;
  uint8_t settings[PARAM_PERSISTED_SETTING_COUNT];
  uint8_t home_step_index;
  uint8_t home_preset_selected;
  uint8_t reserved[2U];
  uint8_t home_presets[PARAM_HOME_PRESET_BYTES];
  uint16_t crc;
} parameter_blob_t;

_Static_assert(sizeof(parameter_blob_t) <= 256U,
               "F429 parameter blob must fit one W25Qxx page program");

/* V5 had the current home-state layout but did not persist ESP32 start mode.
   V6 adds ESP32 START_MODE after the stable display settings. */
typedef struct __attribute__((packed))
{
  uint32_t magic;
  uint16_t version;
  uint16_t length;
  uint32_t sequence;
  uint8_t settings[4U];
  uint8_t home_step_index;
  uint8_t home_preset_selected;
  uint8_t reserved[2U];
  uint8_t home_presets[PARAM_HOME_PRESET_BYTES];
  uint16_t crc;
} parameter_blob_v5_t;

/* V3 had the current home-state layout but persisted only five display IDs.
   V4 adds the F429-only heartbeat option as the first persisted setting. */
typedef struct __attribute__((packed))
{
  uint32_t magic;
  uint16_t version;
  uint16_t length;
  uint32_t sequence;
  uint8_t settings[5U];
  uint8_t home_step_index;
  uint8_t home_preset_selected;
  uint8_t reserved[2U];
  uint8_t home_presets[PARAM_HOME_PRESET_BYTES];
  uint16_t crc;
} parameter_blob_v3_t;

/* V4 persisted HEARTBEAT, BRIGHTNESS, THEME, SLEEP_TIME, TOUCH_BEEP and
   COLOR_PRESET. V5 removes THEME and TOUCH_BEEP. */
typedef struct __attribute__((packed))
{
  uint32_t magic;
  uint16_t version;
  uint16_t length;
  uint32_t sequence;
  uint8_t settings[6U];
  uint8_t home_step_index;
  uint8_t home_preset_selected;
  uint8_t reserved[2U];
  uint8_t home_presets[PARAM_HOME_PRESET_BYTES];
  uint16_t crc;
} parameter_blob_v4_t;

typedef struct __attribute__((packed))
{
  uint32_t magic;
  uint16_t version;
  uint16_t length;
  uint32_t sequence;
  uint8_t settings[5U];
  uint16_t crc;
} parameter_blob_v2_t;

/* Compatibility with the transient V2 format that accidentally persisted the
   runtime-only G474 stream entry as a sixth setting. */
typedef struct __attribute__((packed))
{
  uint32_t magic;
  uint16_t version;
  uint16_t length;
  uint32_t sequence;
  uint8_t settings[6U];
  uint16_t crc;
} parameter_blob_stream_v2_t;

_Static_assert(sizeof(parameter_blob_t) <= sizeof(parameter_blob_v4_t),
               "flash read buffer must cover the current parameter blob");

typedef enum
{
  PARAM_SAVE_IDLE = 0U,
  PARAM_SAVE_ERASE_WAIT,
  PARAM_SAVE_WRITE
} parameter_save_state_t;

static volatile uint8_t s_dirty;
static volatile uint32_t s_dirty_since_ms;
static uint8_t s_flash_ready;
static uint8_t s_active_sector = 0xFFU;
static uint8_t s_save_target_sector;
static uint8_t s_last_save_ok = 1U;
static uint32_t s_sequence;
static parameter_save_state_t s_save_state = PARAM_SAVE_IDLE;
static parameter_blob_t s_pending_blob;
static uint8_t s_default_settings[UI_SETTING_COUNT];
static ui_home_preset_t s_default_home_presets[UI_HOME_PRESET_COUNT];
static uint8_t s_default_home_step_index;
static uint8_t s_default_home_preset_selected;
static const uint16_t s_persisted_setting_ids[PARAM_PERSISTED_SETTING_COUNT] =
{
  8U, 9U, 11U, 29U, UI_SETTING_ID_ESP32_START_MODE
};

static uint16_t UF4_ParameterManager_Crc(const uint8_t *data, uint32_t length)
{
  uint16_t crc = 0xFFFFU;

  for (uint32_t i = 0U; i < length; i++)
  {
    crc ^= (uint16_t)data[i] << 8U;
    for (uint8_t bit = 0U; bit < 8U; bit++)
    {
      crc = (crc & 0x8000U) ?
          (uint16_t)((crc << 1U) ^ 0x1021U) :
          (uint16_t)(crc << 1U);
    }
  }

  return crc;
}

static uint32_t UF4_ParameterManager_SectorAddress(uint8_t sector)
{
  return PARAM_FLASH_BASE_ADDR + (uint32_t)sector * PARAM_FLASH_SECTOR_SIZE;
}

static ui_setting_t *UF4_ParameterManager_FindSetting(uint16_t id)
{
  return Board_Model_FindSetting(id);
}

static uint8_t UF4_ParameterManager_SettingValue(uint16_t id, uint8_t default_value)
{
  ui_setting_t *setting = UF4_ParameterManager_FindSetting(id);

  if (setting == NULL)
  {
    return default_value;
  }

  return setting->current;
}

static void UF4_ParameterManager_ApplyRuntimeSettings(void)
{
  LCD_SetBacklightLevel(UF4_ParameterManager_SettingValue(9U, 5U));
  ui_apply_color_preset(UF4_ParameterManager_SettingValue(29U, 0U));
}

static void UF4_ParameterManager_CaptureDefaults(void)
{
  for (uint16_t i = 0U; i < UI_SETTING_COUNT; i++)
  {
    s_default_settings[i] = settings[i].current;
  }
  ui_home_export_state(&s_default_home_step_index,
                       s_default_home_presets,
                       &s_default_home_preset_selected);
}

static void UF4_ParameterManager_ApplyDefaults(void)
{
  for (uint16_t i = 0U; i < UI_SETTING_COUNT; i++)
  {
    settings[i].current = s_default_settings[i];
  }
  ui_home_import_state(s_default_home_step_index,
                       s_default_home_presets,
                       s_default_home_preset_selected);
  UF4_ParameterManager_ApplyRuntimeSettings();
}

static void UF4_ParameterManager_MakeBlob(parameter_blob_t *blob)
{
  ui_home_preset_t home_presets[UI_HOME_PRESET_COUNT];

  memset(blob, 0xFF, sizeof(*blob));
  blob->magic = PARAM_MAGIC;
  blob->version = PARAM_VERSION;
  blob->length = (uint16_t)sizeof(*blob);
  blob->sequence = s_sequence + 1U;

  for (uint16_t i = 0U; i < PARAM_PERSISTED_SETTING_COUNT; i++)
  {
    ui_setting_t *setting = UF4_ParameterManager_FindSetting(s_persisted_setting_ids[i]);

    blob->settings[i] = (setting != NULL) ? setting->current : 0U;
  }
  ui_home_export_state(&blob->home_step_index,
                       home_presets,
                       &blob->home_preset_selected);
  memcpy(blob->home_presets, home_presets, sizeof(home_presets));

  blob->crc = 0U;
  blob->crc = UF4_ParameterManager_Crc((const uint8_t *)blob,
                                       (uint32_t)(sizeof(*blob) - sizeof(blob->crc)));
}

static uint8_t UF4_ParameterManager_BlobValid(const parameter_blob_t *blob)
{
  uint16_t crc;
  parameter_blob_t tmp;

  if (blob->magic != PARAM_MAGIC ||
      blob->version != PARAM_VERSION ||
      blob->length != sizeof(*blob))
  {
    return 0U;
  }

  tmp = *blob;
  tmp.crc = 0U;
  crc = UF4_ParameterManager_Crc((const uint8_t *)&tmp,
                                 (uint32_t)(sizeof(tmp) - sizeof(tmp.crc)));
  return (uint8_t)(crc == blob->crc);
}

static uint8_t UF4_ParameterManager_V2BlobValid(const parameter_blob_v2_t *blob)
{
  uint16_t crc;
  parameter_blob_v2_t tmp;

  if (blob->magic != PARAM_MAGIC ||
      blob->version != 2U ||
      blob->length != sizeof(*blob))
  {
    return 0U;
  }

  tmp = *blob;
  tmp.crc = 0U;
  crc = UF4_ParameterManager_Crc((const uint8_t *)&tmp,
                                 (uint32_t)(sizeof(tmp) - sizeof(tmp.crc)));
  return (uint8_t)(crc == blob->crc);
}

static uint8_t UF4_ParameterManager_V5BlobValid(const parameter_blob_v5_t *blob)
{
  uint16_t crc;
  parameter_blob_v5_t tmp;

  if (blob->magic != PARAM_MAGIC || blob->version != 5U ||
      blob->length != sizeof(*blob))
  {
    return 0U;
  }

  tmp = *blob;
  tmp.crc = 0U;
  crc = UF4_ParameterManager_Crc((const uint8_t *)&tmp,
                                 (uint32_t)(sizeof(tmp) - sizeof(tmp.crc)));
  return (uint8_t)(crc == blob->crc);
}

static uint8_t UF4_ParameterManager_V3BlobValid(const parameter_blob_v3_t *blob)
{
  uint16_t crc;
  parameter_blob_v3_t tmp;

  if (blob->magic != PARAM_MAGIC || blob->version != 3U ||
      blob->length != sizeof(*blob))
  {
    return 0U;
  }

  tmp = *blob;
  tmp.crc = 0U;
  crc = UF4_ParameterManager_Crc((const uint8_t *)&tmp,
                                 (uint32_t)(sizeof(tmp) - sizeof(tmp.crc)));
  return (uint8_t)(crc == blob->crc);
}

static uint8_t UF4_ParameterManager_V4BlobValid(const parameter_blob_v4_t *blob)
{
  uint16_t crc;
  parameter_blob_v4_t tmp;

  if (blob->magic != PARAM_MAGIC || blob->version != 4U ||
      blob->length != sizeof(*blob))
  {
    return 0U;
  }

  tmp = *blob;
  tmp.crc = 0U;
  crc = UF4_ParameterManager_Crc((const uint8_t *)&tmp,
                                 (uint32_t)(sizeof(tmp) - sizeof(tmp.crc)));
  return (uint8_t)(crc == blob->crc);
}

static uint8_t UF4_ParameterManager_StreamV2BlobValid(const parameter_blob_stream_v2_t *blob)
{
  uint16_t crc;
  parameter_blob_stream_v2_t tmp;

  if (blob->magic != PARAM_MAGIC || blob->version != 2U ||
      blob->length != sizeof(*blob))
  {
    return 0U;
  }

  tmp = *blob;
  tmp.crc = 0U;
  crc = UF4_ParameterManager_Crc((const uint8_t *)&tmp,
                                 (uint32_t)(sizeof(tmp) - sizeof(tmp.crc)));
  return (uint8_t)(crc == blob->crc);
}

static void UF4_ParameterManager_ApplyBlob(const parameter_blob_t *blob)
{
  ui_home_preset_t home_presets[UI_HOME_PRESET_COUNT];

  for (uint16_t i = 0U; i < PARAM_PERSISTED_SETTING_COUNT; i++)
  {
    ui_setting_t *setting = UF4_ParameterManager_FindSetting(s_persisted_setting_ids[i]);
    uint8_t value = blob->settings[i];

    if (setting == NULL)
    {
      continue;
    }
    if (setting->option_count > 0U && value >= setting->option_count)
    {
      value = (uint8_t)(setting->option_count - 1U);
    }
    setting->current = value;
  }

  memcpy(home_presets, blob->home_presets, sizeof(home_presets));
  ui_home_import_state(blob->home_step_index,
                       home_presets,
                       blob->home_preset_selected);
  UF4_ParameterManager_ApplyRuntimeSettings();
}

static uint8_t UF4_ParameterManager_ReadBlob(uint8_t sector, parameter_blob_t *blob)
{
  uint8_t raw[sizeof(parameter_blob_v4_t)];
  parameter_blob_stream_v2_t raw_stream_v2;
  parameter_blob_v5_t raw_v5;
  parameter_blob_v2_t raw_v2;
  parameter_blob_v3_t raw_v3;
  parameter_blob_v4_t raw_v4;
  parameter_blob_t canonical;

  if (w25qxx_read(&g_w25qxx, UF4_ParameterManager_SectorAddress(sector),
                  raw, sizeof(raw)) != 0U)
  {
    return 0U;
  }

  memcpy(&canonical, raw, sizeof(canonical));
  if (UF4_ParameterManager_BlobValid(&canonical) != 0U)
  {
    *blob = canonical;
    return 1U;
  }

  memcpy(&raw_v5, raw, sizeof(raw_v5));
  if (UF4_ParameterManager_V5BlobValid(&raw_v5) != 0U)
  {
    memset(blob, 0xFF, sizeof(*blob));
    blob->magic = raw_v5.magic;
    blob->version = PARAM_VERSION;
    blob->length = sizeof(*blob);
    blob->sequence = raw_v5.sequence;
    memcpy(blob->settings, raw_v5.settings, sizeof(raw_v5.settings));
    blob->settings[4] = UI_ESP32_START_NONE;
    blob->home_step_index = raw_v5.home_step_index;
    blob->home_preset_selected = raw_v5.home_preset_selected;
    memcpy(blob->home_presets, raw_v5.home_presets, sizeof(blob->home_presets));
    blob->crc = 0U;
    blob->crc = UF4_ParameterManager_Crc((const uint8_t *)blob,
                                         (uint32_t)(sizeof(*blob) - sizeof(blob->crc)));
    return 1U;
  }

  memcpy(&raw_v4, raw, sizeof(raw_v4));
  if (UF4_ParameterManager_V4BlobValid(&raw_v4) != 0U)
  {
    memset(blob, 0xFF, sizeof(*blob));
    blob->magic = raw_v4.magic;
    blob->version = PARAM_VERSION;
    blob->length = sizeof(*blob);
    blob->sequence = raw_v4.sequence;
    blob->settings[0] = raw_v4.settings[0];
    blob->settings[1] = raw_v4.settings[1];
    blob->settings[2] = raw_v4.settings[3];
    blob->settings[3] = raw_v4.settings[5];
    blob->home_step_index = raw_v4.home_step_index;
    blob->home_preset_selected = raw_v4.home_preset_selected;
    memcpy(blob->home_presets, raw_v4.home_presets, sizeof(blob->home_presets));
    blob->crc = 0U;
    blob->crc = UF4_ParameterManager_Crc((const uint8_t *)blob,
                                         (uint32_t)(sizeof(*blob) - sizeof(blob->crc)));
    return 1U;
  }

  memcpy(&raw_v3, raw, sizeof(raw_v3));
  if (UF4_ParameterManager_V3BlobValid(&raw_v3) != 0U)
  {
    memset(blob, 0xFF, sizeof(*blob));
    blob->magic = raw_v3.magic;
    blob->version = PARAM_VERSION;
    blob->length = sizeof(*blob);
    blob->sequence = raw_v3.sequence;
    blob->settings[0] = 0U;
    blob->settings[1] = raw_v3.settings[0];
    blob->settings[2] = raw_v3.settings[2];
    blob->settings[3] = raw_v3.settings[4];
    blob->home_step_index = raw_v3.home_step_index;
    blob->home_preset_selected = raw_v3.home_preset_selected;
    memcpy(blob->home_presets, raw_v3.home_presets, sizeof(blob->home_presets));
    blob->crc = 0U;
    blob->crc = UF4_ParameterManager_Crc((const uint8_t *)blob,
                                         (uint32_t)(sizeof(*blob) - sizeof(blob->crc)));
    return 1U;
  }

  memcpy(&raw_v2, raw, sizeof(raw_v2));
  if (UF4_ParameterManager_V2BlobValid(&raw_v2) != 0U)
  {
    memset(blob, 0xFF, sizeof(*blob));
    blob->magic = raw_v2.magic;
    blob->version = PARAM_VERSION;
    blob->length = sizeof(*blob);
    blob->sequence = raw_v2.sequence;
    blob->settings[0] = 0U;
    blob->settings[1] = raw_v2.settings[0];
    blob->settings[2] = raw_v2.settings[2];
    blob->settings[3] = raw_v2.settings[4];
    ui_home_preset_t home_presets[UI_HOME_PRESET_COUNT];

    ui_home_export_state(&blob->home_step_index,
                         home_presets,
                         &blob->home_preset_selected);
    memcpy(blob->home_presets, home_presets, sizeof(home_presets));
    blob->crc = 0U;
    blob->crc = UF4_ParameterManager_Crc((const uint8_t *)blob,
                                         (uint32_t)(sizeof(*blob) - sizeof(blob->crc)));
    return 1U;
  }

  memcpy(&raw_stream_v2, raw, sizeof(raw_stream_v2));
  if (UF4_ParameterManager_StreamV2BlobValid(&raw_stream_v2) != 0U)
  {
    ui_home_preset_t home_presets[UI_HOME_PRESET_COUNT];

    memset(blob, 0xFF, sizeof(*blob));
    blob->magic = raw_stream_v2.magic;
    blob->version = PARAM_VERSION;
    blob->length = sizeof(*blob);
    blob->sequence = raw_stream_v2.sequence;
    /* The transient layout put G474 STREAM before the stable UI entries. */
    blob->settings[0] = 0U;
    blob->settings[1] = raw_stream_v2.settings[1];
    blob->settings[2] = raw_stream_v2.settings[3];
    blob->settings[3] = raw_stream_v2.settings[5];
    ui_home_export_state(&blob->home_step_index,
                         home_presets,
                         &blob->home_preset_selected);
    memcpy(blob->home_presets, home_presets, sizeof(home_presets));
    blob->crc = 0U;
    blob->crc = UF4_ParameterManager_Crc((const uint8_t *)blob,
                                         (uint32_t)(sizeof(*blob) - sizeof(blob->crc)));
    return 1U;
  }

  return 0U;
}

static void UF4_ParameterManager_Load(void)
{
  parameter_blob_t blob;
  parameter_blob_t best_blob;
  uint8_t found = 0U;

  for (uint8_t sector = 0U; sector < PARAM_FLASH_SECTOR_COUNT; sector++)
  {
    if (UF4_ParameterManager_ReadBlob(sector, &blob) != 0U &&
        (found == 0U || blob.sequence > best_blob.sequence))
    {
      best_blob = blob;
      s_active_sector = sector;
      found = 1U;
    }
  }

  if (found != 0U)
  {
    s_sequence = best_blob.sequence;
    UF4_ParameterManager_ApplyBlob(&best_blob);
    printf("[UF4] flash load ok sector=%u seq=%lu version=%u\r\n",
           (unsigned)s_active_sector,
           (unsigned long)s_sequence,
           (unsigned)best_blob.version);
  }
  else
  {
    s_active_sector = 0xFFU;
    s_sequence = 0U;
    s_dirty = s_flash_ready;
    s_dirty_since_ms = HAL_GetTick();
    UF4_ParameterManager_ApplyRuntimeSettings();
    printf("[UF4] flash load empty ready=%u\r\n", (unsigned)s_flash_ready);
  }
}

static void UF4_ParameterManager_StartSave(void)
{
  if (s_flash_ready == 0U)
  {
    printf("[UF4] flash save skipped: not ready\r\n");
    return;
  }

  UF4_ParameterManager_MakeBlob(&s_pending_blob);
  s_save_target_sector =
      (s_active_sector == 0xFFU) ? 0U : (uint8_t)(s_active_sector ^ 1U);

  if (W25Qxx_SectorErase4KStart(
          UF4_ParameterManager_SectorAddress(s_save_target_sector)) != 0U)
  {
    s_last_save_ok = 0U;
    s_dirty = 1U;
    s_dirty_since_ms = HAL_GetTick();
    s_save_state = PARAM_SAVE_IDLE;
    printf("[UF4] flash erase start failed sector=%u\r\n",
           (unsigned)s_save_target_sector);
    return;
  }

  s_save_state = PARAM_SAVE_ERASE_WAIT;
  printf("[UF4] flash erase start sector=%u seq=%lu\r\n",
         (unsigned)s_save_target_sector,
         (unsigned long)s_pending_blob.sequence);
}

static void UF4_ParameterManager_SaveStep(void)
{
  uint8_t busy = 0U;
  parameter_blob_t verify;

  switch (s_save_state)
  {
    case PARAM_SAVE_ERASE_WAIT:
      if (W25Qxx_IsBusy(&busy) != 0U)
      {
        s_last_save_ok = 0U;
        s_dirty = 1U;
        s_dirty_since_ms = HAL_GetTick();
        s_save_state = PARAM_SAVE_IDLE;
        printf("[UF4] flash erase wait failed sector=%u\r\n",
               (unsigned)s_save_target_sector);
      }
      else if (busy == 0U)
      {
        s_save_state = PARAM_SAVE_WRITE;
        printf("[UF4] flash erase done sector=%u\r\n",
               (unsigned)s_save_target_sector);
      }
      break;

    case PARAM_SAVE_WRITE:
      if (w25qxx_page_program(&g_w25qxx,
                              UF4_ParameterManager_SectorAddress(s_save_target_sector),
                              (uint8_t *)&s_pending_blob,
                              sizeof(s_pending_blob)) == 0U &&
          w25qxx_read(&g_w25qxx,
                      UF4_ParameterManager_SectorAddress(s_save_target_sector),
                      (uint8_t *)&verify,
                      sizeof(verify)) == 0U &&
          memcmp(&s_pending_blob, &verify, sizeof(verify)) == 0)
      {
        s_active_sector = s_save_target_sector;
        s_sequence = s_pending_blob.sequence;
        s_last_save_ok = 1U;
        printf("[UF4] flash save ok sector=%u seq=%lu bytes=%u\r\n",
               (unsigned)s_active_sector,
               (unsigned long)s_sequence,
               (unsigned)sizeof(s_pending_blob));
      }
      else
      {
        s_last_save_ok = 0U;
        s_dirty = 1U;
        s_dirty_since_ms = HAL_GetTick();
        printf("[UF4] flash save verify failed sector=%u\r\n",
               (unsigned)s_save_target_sector);
      }
      s_save_state = PARAM_SAVE_IDLE;
      break;

    case PARAM_SAVE_IDLE:
    default:
      break;
  }
}

void UF4_ParameterManager_Init(void)
{
  s_flash_ready = (g_w25qxx_init_status == 0U) ? 1U : 0U;
  s_dirty = 0U;
  s_dirty_since_ms = 0U;
  s_last_save_ok = 1U;
  s_save_state = PARAM_SAVE_IDLE;
  UF4_ParameterManager_CaptureDefaults();
  UF4_ParameterManager_Load();
}

void UF4_ParameterManager_Process(void)
{
  const uint32_t now_ms = HAL_GetTick();

  if (s_save_state != PARAM_SAVE_IDLE)
  {
    UF4_ParameterManager_SaveStep();
    return;
  }

  if (s_dirty == 0U ||
      (uint32_t)(now_ms - s_dirty_since_ms) < PARAM_SAVE_DEBOUNCE_MS)
  {
    return;
  }

  s_dirty = 0U;
  UF4_ParameterManager_ApplyRuntimeSettings();
  UF4_ParameterManager_StartSave();
}

void UF4_ParameterManager_OnWriteApply(void *user)
{
  (void)user;
  s_dirty = 1U;
  s_dirty_since_ms = HAL_GetTick();
  UF4_ParameterManager_ApplyRuntimeSettings();
}

void UF4_ParameterManager_SaveNow(void)
{
  s_dirty = 1U;
  s_dirty_since_ms = HAL_GetTick() - PARAM_SAVE_DEBOUNCE_MS;
}

void UF4_ParameterManager_LoadDefaults(void)
{
  UF4_ParameterManager_ApplyDefaults();
  s_dirty = 1U;
  s_dirty_since_ms = HAL_GetTick();
}

uint8_t UF4_ParameterManager_IsReady(void)
{
  return s_flash_ready;
}

uint8_t UF4_ParameterManager_LastSaveOk(void)
{
  return s_last_save_ok;
}
