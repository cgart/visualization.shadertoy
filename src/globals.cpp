#include "globals.hpp"

using namespace std;


// -------------------------settings ----------------------
bool g_shufflePresets = false;
int g_shufflePresetsDuration = 20 * 1000;
bool g_shufflePresetsNonunfirom = true;
float g_shufflePresetsNonuniformReductionFactor = 0.5;

string g_pathPresets;

#if defined(HAS_GLES)
const std::vector<Preset> g_presets =
  {
   {"Input Sound by iq",                        "input.frag.glsl",                  99, -1, -1, -1},
   {"LED spectrum by simesgreen",               "ledspectrum.frag.glsl",            99, -1, -1, -1},
   {"2D LED Spectrum by uNiversal",             "2Dspectrum.frag.glsl",             99, -1, -1, -1},
   {"Audio Eclipse by airtight",                "audioeclipse.frag.glsl",           99, -1, -1, -1},
   //{"Audio Reaktive by choard1895",             "audioreaktive.frag.glsl",          99, -1, -1, -1},
   {"AudioVisual by Passion",                   "audiovisual.frag.glsl",            99, -1, -1, -1},
   {"Beating Circles by Phoenix72",             "beatingcircles.frag.glsl",         99, -1, -1, -1},
   {"BPM by iq",                                "bpm.frag.glsl",                    99, -1, -1, -1},
   {"Dancing Metalights by Danguafare",         "dancingmetalights.frag.glsl",      99, -1, -1, -1},
   {"The Disco Tunnel by poljere",              "discotunnel.frag.glsl",             2, 13, 99, -1},
   {"Gameboy by iq",                            "gameboy.frag.glsl",                99, -1, -1, -1},
   {"Polar Beats by sauj123",                   "polarbeats.frag.glsl",             99, -1, -1, -1},
   {"Simplicity Galaxy by CBS",                 "simplicitygalaxy.frag.glsl",       99, -1, -1, -1},
   {"Sound Flower by iq",                       "soundflower.frag.glsl",            99, -1, -1, -1},
   {"Sound sinus wave by Eitraz",               "soundsinuswave.frag.glsl",         99, -1, -1, -1},
   {"Spectrometer by jaba",                     "spectrometer.frag.glsl",           99, -1, -1, -1},
   {"symmetrical sound visualiser by thelinked","symmetricalsound.frag.glsl",       99, -1, -1, -1},
   {"Twisted Rings by poljere",                 "twistedrings.frag.glsl",           99, -1, -1, -1},
   {"Undulant Spectre by mafik",                "undulantspectre.frag.glsl",        99, -1, -1, -1},
   {"Waves Remix by ADOB",                      "wavesremix.frag.glsl",             99, -1, -1, -1},
   {"Circle Wave by TekF",                      "circlewave.frag.glsl",             99, -1, -1, -1},

   {"Revision 2015 Livecoding Round 1 by mu6k", "revision2015.frag.glsl",           99, -1, -1, -1},
   {"Ribbons by XT95",                          "ribbons.frag.glsl",                99, -1, -1, -1},
   {"Nyancat by mu6k",                          "nyancat.frag.glsl",                99, 13, -1, -1},
   {"Kaleidoscope Visualizer by Qqwy",          "kaleidoscopevisualizer.frag.glsl", 99, 16, -1, -1},
   {"Audio Surf by 4rknova",                    "audiosurf.frag.glsl",              99, -1, -1, -1},
   {"I/O by movAX13h",                          "io.frag.glsl",                     99, -1, -1, -1},
   {"Fractal Tiling by iq",                     "fractal_tiling.frag.glsl",         99, -1, -1, -1},
   {"SH2014 Cellular by vug",                   "sh2014_cellular.frag.glsl",        99, -1, -1, -1},
   {"NV2015 Space Curvature by iq",             "space_curvature.frag.glsl",        99,  9, -1, -1},
   {"Paralax Fractal Galaxy by CBS",            "galaxy.frag.glsl",                 99, -1, -1, -1}};

#else
const std::vector<Preset> g_presets =
  {
   {"2D LED Spectrum by uNiversal",             "2Dspectrum.frag.glsl",             99, -1, -1, -1},
   {"Audio Reaktive by choard1895",             "audioreaktive.frag.glsl",          99, -1, -1, -1},
   {"AudioVisual by Passion",                   "audiovisual.frag.glsl",            99, -1, -1, -1},
   {"Beating Circles by Phoenix72",             "beatingcircles.frag.glsl",         99, -1, -1, -1},
   {"BPM by iq",                                "bpm.frag.glsl",                    99, -1, -1, -1},
   {"Circle Wave by TekF",                      "circlewave.frag.glsl",             99, -1, -1, -1},
   {"Circuits by Kali",                         "circuits.frag.glsl",               99,  7, -1, -1},
   {"Colored Bars by novalis",                  "coloredbars.frag.glsl",            99, -1, -1, -1},
   {"Cubescape by iq",                          "cubescape.frag.glsl",              99,  5, -1, -1},
   {"The Disco Tunnel by poljere",              "discotunnel.frag.glsl",            99,  2, 14, -1},
   {"Fractal Land by Kali",                     "fractalland.frag.glsl",             2, 13, 99, -1},
   {"Gameboy by iq",                            "gameboy.frag.glsl",                99, -1, -1, -1},
   {"I/O by movAX13h",                          "io.frag.glsl",                     99, -1, -1, -1},
   {"Kaleidoscope Visualizer by Qqwy",          "kaleidoscopevisualizer.frag.glsl", 99, 15, -1, -1},
   {"Nyancat by mu6k",                          "nyancat.frag.glsl",                99, 13, -1, -1},
   {"Polar Beats by sauj123",                   "polarbeats.frag.glsl",             99, -1, -1, -1},
   {"Revision 2015 Livecoding Round 1 by mu6k", "revision2015.frag.glsl",           99, -1, -1, -1},
   {"Ribbons by XT95",                          "ribbons.frag.glsl",                99, -1, -1, -1},
   {"Simplicity Galaxy by CBS",                 "simplicitygalaxy.frag.glsl",       99, -1, -1, -1},
   {"Sound Flower by iq",                       "soundflower.frag.glsl",            99, -1, -1, -1},
   {"Sound sinus wave by Eitraz",               "soundsinuswave.frag.glsl",         99, -1, -1, -1},
   {"symmetrical sound visualiser by thelinked","symmetricalsound.frag.glsl",       99, -1, -1, -1},
   {"Twisted Rings by poljere",                 "twistedrings.frag.glsl",           99, -1, -1, -1},
   {"Undulant Spectre by mafik",                "undulantspectre.frag.glsl",        99, -1, -1, -1},
   {"Demo - Volumetric Lines by iq",            "volumetriclines.frag.glsl",        99, -1, -1, -1},
   {"Galaxy Of Universes by Dave_Hoskins",      "galaxy_spinning.frag.glsl",        99, -1, -1, -1},
   {"Waves Remix by ADOB",                      "wavesremix.frag.glsl",             99, -1, -1, -1}};
#endif

int g_currentPreset = 0;
char** lpresets = nullptr;

std::vector<std::string> g_fileTextures = {
  "tex00.png",
  "tex01.png",
  "tex02.png",
  "tex03.png",
  "tex04.png",
  "tex05.png",
  "tex06.png",
  "tex07.png",
  "tex08.png",
  "tex09.png",
  "tex10.png",
  "tex11.png",
  "tex12.png",
  "tex13.png",
  "tex14.png",
  "tex15.png",
  "tex16.png"
};

std::vector<GLint> g_textures(g_fileTextures.size(), 0);

#if defined(HAS_GLES)
State  state_g;
State* state = &state_g;
#endif


bool initialized = false;
bool preset_active = false;

GLuint shadertoy_shader = 0;

GLint iResolutionLoc        = 0;
GLint iGlobalTimeLoc        = 0;
GLint iChannelTimeLoc       = 0;
GLint iMouseLoc             = 0;
GLint iDateLoc              = 0;
GLint iSampleRateLoc        = 0;
GLint iChannelResolutionLoc = 0;
GLint iChannelLoc[4];
GLuint iChannel[4];

int win_width = 0;
int win_height = 0;
int num_frames = 0;
int64_t frame_timestamp = 0;

int64_t initial_time;
int bits_precision = 0;

bool needsUpload = true;


std::vector<double> presetSamplingWeight(g_presets.size(), 1.0);
std::random_device randomDevice;
std::mt19937 randomGenerator(randomDevice());
int64_t lastPresetChangeTime = PLATFORM::GetTimeMs();
