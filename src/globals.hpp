#ifndef GLOBALS_HPP__
#define GLOBALS_HPP__

#include "base.hpp"

#include <vector>
#include <string>


// -------------------------settings ----------------------
extern bool g_shufflePresets;
extern int g_shufflePresetsDuration;
extern bool g_shufflePresetsNonunfirom;
extern float g_shufflePresetsNonuniformReductionFactor;

extern std::string g_pathPresets;

struct Preset {
  std::string name;
  std::string file;
  int channel[4];
};

extern const std::vector<Preset> g_presets;

extern int g_currentPreset;
extern char** lpresets;

extern std::vector<std::string> g_fileTextures;
extern std::vector<GLint> g_textures;

#if defined(HAS_GLES)
struct State
{
  GLuint vertex_buffer;
  GLuint attr_vertex_e;
  GLuint attr_vertex_r, uTexture;
  GLuint effect_fb;
  GLuint framebuffer_texture;
  GLuint render_program;
  GLuint uScale;
  int fbwidth, fbheight;
};

extern State  state_g;
extern State* state;
#endif


// --------------------------- globals --------------------------
extern bool initialized;
extern bool preset_active;

extern GLuint shadertoy_shader;

extern GLint iResolutionLoc   ;
extern GLint iGlobalTimeLoc   ;
extern GLint iChannelTimeLoc  ;
extern GLint iMouseLoc        ;
extern GLint iDateLoc         ;
extern GLint iSampleRateLoc   ;
extern GLint iChannelResolutionLoc;
extern GLint iChannelLoc[4];
extern GLuint iChannel[4];

extern int win_width;
extern int win_height;
extern int num_frames;
extern int64_t frame_timestamp;

extern int64_t initial_time;
extern int bits_precision;

extern bool needsUpload;


extern std::vector<double> presetSamplingWeight;
extern std::random_device randomDevice;
extern std::mt19937 randomGenerator;
extern int64_t lastPresetChangeTime;


#endif 
