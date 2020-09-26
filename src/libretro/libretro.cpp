#include "libretro.h"
#include "libretro_core_options.h"

#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <memory>
#include <sstream>

#include <virt86/virt86.hpp>

#include "xbox.h"
#include "util.h"

Xbox* xbox = nullptr;

static struct retro_log_callback logging;
static retro_log_printf_t log_cb = nullptr;
char retro_system_dir[4096];
char retro_save_dir[4096];
char retro_game_path[4096];

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);  
    vfprintf(stderr, fmt, va);
    va_end(va);
}

static retro_environment_t environ_cb;

void Log(LogLevel level, const char *fmt, ...)
{
    enum retro_log_level log_level;
    switch (level) {
        case LogLevel::Info: default:
            log_level = RETRO_LOG_INFO;
            break;
        case LogLevel::Warning:
            log_level = RETRO_LOG_WARN;
            break;
        case LogLevel::Error:
            log_level = RETRO_LOG_ERROR;
            break;
    }
    
    char buffer[4096];
    va_list va;
    va_start(va, fmt);
    vsnprintf(buffer, 4096, fmt, va);
    va_end(va);

    log_cb(log_level, buffer);
}

void retro_init(void)
{
    const char *dir = NULL;
    if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir) {
        snprintf(retro_system_dir, sizeof(retro_system_dir), "%s", dir);
    }

    if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &dir) && dir) {
        snprintf(retro_save_dir, sizeof(retro_save_dir), "%s", dir);
    }
}

void retro_deinit(void)
{
    xbox->Shutdown();
    delete xbox;
}

unsigned retro_api_version(void)
{
    return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned port, unsigned device)
{
    log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
}

void retro_get_system_info(struct retro_system_info *info)
{
    memset(info, 0, sizeof(*info));
    info->library_name = "libretro-xbox";
    info->library_version = "alpha";
    info->need_fullpath = true;
    info->valid_extensions = "iso";
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_get_system_av_info(struct retro_system_av_info *info)
{
    info->geometry.base_width = 640;
    info->geometry.base_height = 480;
    info->geometry.max_width = 1920;
    info->geometry.max_height = 1080;
    info->geometry.aspect_ratio = 0.0f;

    info->timing.fps = 60;
}

void retro_set_environment(retro_environment_t cb)
{
    environ_cb = cb;

    libretro_set_core_options(cb);

    if (!log_cb) {
        if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging)) {
            log_cb = logging.log;
        }
        else {
            log_cb = fallback_log;
        }
    }

    bool no_game = true;
    cb(RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME, &no_game);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
    audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
    audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
    input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
    input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
    video_cb = cb;
}

void retro_reset()
{
    xbox->Reset();
}

void retro_run()
{
    xbox->RunFrame();

    // TODO: Handle display mode changes

    static auto& nv2a = xbox->GetNv2a();
    auto framebuffer = xbox->GetRamPtr() + (nv2a.Pcrtc.GetFramebufferAddr() & 0x7FFFFFF);
    auto width = nv2a.Prmcio.GetDisplayWidth();
    auto height = nv2a.Prmcio.GetDisplayHeight();
    auto pitch = width * nv2a.Prmcio.GetDisplayBpp();
    video_cb(framebuffer, width, height, pitch);
}   

bool retro_load_game(const struct retro_game_info *info)
{
    bool foundPlatform = false;
    size_t platformIndex = 0;
    for (size_t i = 0; i < array_size(virt86::PlatformFactories); i++) {
        virt86::Platform& platform = virt86::PlatformFactories[i]();
        if (platform.GetInitStatus() == virt86::PlatformInitStatus::OK) {
            log_cb(RETRO_LOG_INFO, "%s loaded successfully\n", platform.GetName().c_str());
            foundPlatform = true;
            platformIndex = i;
            break;
        }
    }

    if (!foundPlatform) {
        log_cb(RETRO_LOG_INFO, "none found\n");
        return false;
    }

    virt86::Platform& platform = virt86::PlatformFactories[platformIndex]();

    xbox = new Xbox(platform);
    xbox->Init();

    std::stringstream mcpx_rom_str;
    std::stringstream bios_rom_str;

    mcpx_rom_str << retro_system_dir << "/mcpx_1.0.bin";
    bios_rom_str << retro_system_dir << "/cromwell_1024.bin";
    //bios_rom_str << retro_system_dir << "/xbox-3944.bin";

    // For now, we use hard-coded paths to supported boot-roms
    // These must be located in the retroarch System directory
    if (!xbox->LoadBootRoms(bios_rom_str.str(), mcpx_rom_str.str())) {
        return false;
    }

    // TODO: Load EEPROM    
    // TODO: Mount HDD
    // TODO: Mount given DVD-ROM image 

    enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
    if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt)) {
        log_cb(RETRO_LOG_INFO, "XRGB8888 is not supported.\n");
        return false;
    }

    return true;
}

bool retro_load_game_special(unsigned type, const struct retro_game_info *info, size_t num)
{
    return false;
}

void retro_unload_game()
{

}

unsigned retro_get_region()
{
    // TODO: Get region flag from Xbox eeprom (if possible?)
    return RETRO_REGION_NTSC;
}

size_t retro_serialize_size()
{
    return false;
}

bool retro_serialize(void *data_, size_t size)
{
    return false;
}

bool retro_unserialize(const void *data_, size_t size)
{
    return false;
}

void *retro_get_memory_data(unsigned id)
{
    return NULL;
}

size_t retro_get_memory_size(unsigned id)
{
    return 0;
}

void retro_cheat_reset(void)
{

}

void retro_cheat_set(unsigned index, bool enabled, const char *code)
{

}
