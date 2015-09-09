#include "addon.hpp"
#include "base.hpp"
#include "globals.hpp"
#include "audio.hpp"
#include "presets.hpp"
#include "renderer.hpp"

using namespace std;


//-----------------------------------------------------------------------------
void LogProps(VIS_PROPS *props)
{
  cout << "Props = {" << endl
       << "\t x: " << props->x << endl
       << "\t y: " << props->y << endl
       << "\t width: " << props->width << endl
       << "\t height: " << props->height << endl
       << "\t pixelRatio: " << props->pixelRatio << endl
       << "\t name: " << props->name << endl
       << "\t presets: " << props->presets << endl
       << "\t profile: " << props->profile << endl
//       << "\t submodule: " << props->submodule << endl // Causes problems? Is it initialized?
       << "}" << endl;
}

//-----------------------------------------------------------------------------
void LogTrack(VisTrack *track)
{
  cout << "Track = {" << endl
       << "\t title: " << track->title << endl
       << "\t artist: " << track->artist << endl
       << "\t album: " << track->album << endl
       << "\t albumArtist: " << track->albumArtist << endl
       << "\t genre: " << track->genre << endl
       << "\t comment: " << track->comment << endl
       << "\t lyrics: " << track->lyrics << endl
       << "\t trackNumber: " << track->trackNumber << endl
       << "\t discNumber: " << track->discNumber << endl
       << "\t duration: " << track->duration << endl
       << "\t year: " << track->year << endl
       << "\t rating: " << track->rating << endl
       << "}" << endl;
}

//-----------------------------------------------------------------------------
void LogAction(const char *message)
{
  cout << "Action " << message << endl;
}

//-----------------------------------------------------------------------------
void LogActionString(const char *message, const char *param)
{
  cout << "Action " << message << " " << param << endl;
}



//-- Start -------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName)
{
  cout << "Start " << iChannels << " " << iSamplesPerSec << " " << iBitsPerSample << " " << szSongName << std::endl;
  audio::SetSamplesPerSec(iSamplesPerSec);
}


//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
void Render()
{
  glGetError();
  //cout << "Render" << std::endl;
  if (initialized){
#if defined(HAS_GLES)
    if (state->fbwidth && state->fbheight) {
      presets::RenderTo(shadertoy_shader, state->effect_fb);
      presets::RenderTo(state->render_program, 0);
    } else {
      presets::RenderTo(shadertoy_shader, 0);
    }
#else
    presets::RenderTo(shadertoy_shader, 0);
#endif

    if (num_frames++ == 0)
      frame_timestamp = PLATFORM::GetTimeMs();

    if (PLATFORM::GetTimeMs() - frame_timestamp > 1e3)
    {
      //printf("%d fps : %lld > %lld\n", num_frames, frame_timestamp, lastPresetChangeTime + g_shufflePresetsDuration);
      frame_timestamp += 1e3;
      num_frames = 0;
    }

    // change preset if it is about the time
    if (g_shufflePresets && frame_timestamp > lastPresetChangeTime + g_shufflePresetsDuration)
    {
        lastPresetChangeTime = frame_timestamp;

        cout << "Shuffle preset choose preset ";

        // non-uniform sampling - we sample based on the weights
        if (g_shufflePresetsNonunfirom)
        {
            std::discrete_distribution<> rnd(presetSamplingWeight.begin(), presetSamplingWeight.end());
            g_currentPreset = rnd(randomGenerator);
            presetSamplingWeight[g_currentPreset] *= g_shufflePresetsNonuniformReductionFactor;

            if (presetSamplingWeight[g_currentPreset] < 1e-10)
                presetSamplingWeight[g_currentPreset] = 1.0;

        // uniform sampling
        }else
        {
            std::uniform_int_distribution<> rnd(0, g_presets.size());
            g_currentPreset = rnd(randomGenerator);
        }

        cout << g_currentPreset << std::endl;

        presets::launch(g_currentPreset);
    }
  }
}


//-- AudioData ------------------------------------------------------------------
// Called from XBMC to deliver to chunk of audio
//-----------------------------------------------------------------------------
void AudioData(const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength)
{
  audio::NewAudioData(pAudioData, iAudioDataLength, pFreqData, iFreqDataLength);
  needsUpload = true;
}

//-- GetInfo ------------------------------------------------------------------
// Tell XBMC our requirements
//-----------------------------------------------------------------------------
void GetInfo(VIS_INFO *pInfo)
{
  FUNCTION_ENTER;

  pInfo->bWantsFreq = false;
  pInfo->iSyncDelay = 0;
}


//-- GetSubModules ------------------------------------------------------------
// Return any sub modules supported by this vis
//-----------------------------------------------------------------------------
unsigned int GetSubModules(char ***names)
{
  FUNCTION_ENTER;

  return 0; // this vis supports 0 sub modules
}

//-- OnAction -----------------------------------------------------------------
// Handle XBMC actions such as next preset, lock preset, album art changed etc
//-----------------------------------------------------------------------------
bool OnAction(long flags, const void *param)
{
  FUNCTION_ENTER;

  switch (flags)
  {
    case VIS_ACTION_NEXT_PRESET:
      LogAction("VIS_ACTION_NEXT_PRESET");
      g_currentPreset = (g_currentPreset + 1)  % g_presets.size();
      presets::launch(g_currentPreset);
      return true;
    case VIS_ACTION_PREV_PRESET:
      LogAction("VIS_ACTION_PREV_PRESET");
      g_currentPreset = (g_currentPreset - 1)  % g_presets.size();
      presets::launch(g_currentPreset);
      return true;
    case VIS_ACTION_LOAD_PRESET:
      LogAction("VIS_ACTION_LOAD_PRESET"); // TODO param is int *
      if (param)
      {
        g_currentPreset = *(int *)param % g_presets.size();
        presets::launch(g_currentPreset);
        return true;
      }

      break;
    case VIS_ACTION_RANDOM_PRESET:
      LogAction("VIS_ACTION_RANDOM_PRESET");
      g_currentPreset = (int)((std::rand() / (float)RAND_MAX) * g_presets.size());
      presets::launch(g_currentPreset);
      return true;

    case VIS_ACTION_LOCK_PRESET:
      LogAction("VIS_ACTION_LOCK_PRESET");
      break;
    case VIS_ACTION_RATE_PRESET_PLUS:
      LogAction("VIS_ACTION_RATE_PRESET_PLUS");
      break;
    case VIS_ACTION_RATE_PRESET_MINUS:
      LogAction("VIS_ACTION_RATE_PRESET_MINUS");
      break;
    case VIS_ACTION_UPDATE_ALBUMART:
      LogActionString("VIS_ACTION_UPDATE_ALBUMART", (const char *)param);
      break;
    case VIS_ACTION_UPDATE_TRACK:
      LogTrack((VisTrack *)param);
      break;

    default:
      break;
  }

  return false;
}

//-- GetPresets ---------------------------------------------------------------
// Return a list of presets to XBMC for display
//-----------------------------------------------------------------------------
unsigned int GetPresets(char ***presets)
{
  FUNCTION_ENTER;

  cout << "GetPresets " << g_presets.size() << std::endl;

  if (!lpresets) {
    lpresets = new char*[g_presets.size()];
    size_t i=0;
    for (auto p : g_presets)
      lpresets[i++] = const_cast<char*>(p.name.c_str());
  }

  *presets = lpresets;
  return g_presets.size();
}

//-- GetPreset ----------------------------------------------------------------
// Return the index of the current playing preset
//-----------------------------------------------------------------------------
unsigned GetPreset()
{
  return g_currentPreset;
}

//-- IsLocked -----------------------------------------------------------------
// Returns true if this add-on use settings
//-----------------------------------------------------------------------------
bool IsLocked()
{
  FUNCTION_ENTER;

  return false;
}

//-- Create -------------------------------------------------------------------
// Called on load. Addon should fully initalize or return error status
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_Create(void* hdl, void* props)
{
  FUNCTION_ENTER;

  VIS_PROPS *p = (VIS_PROPS *)props;

  LogProps(p);

  g_pathPresets = p->presets;
  win_width = p->width;
  win_height = p->height;

  presetSamplingWeight.clear();
  presetSamplingWeight.resize(g_presets.size(), 1.0);

  audio::Init();

#if !defined(HAS_GLES)
  if (GLEW_OK != glewInit()) {
	  std::cout << "Failed to initialize glew";
  }
#endif
  if (!initialized)
  {
#if defined(HAS_GLES)
    static const GLfloat vertex_data[] = {
        -1.0,1.0,1.0,1.0,
        1.0,1.0,1.0,1.0,
        1.0,-1.0,1.0,1.0,
        -1.0,-1.0,1.0,1.0,
    };
    glGetError();
    // Upload vertex data to a buffer
    glGenBuffers(1, &state->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, state->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
#endif
    initialized = true;
  }

  if (!props)
    return ADDON_STATUS_UNKNOWN;

  return ADDON_STATUS_NEED_SAVEDSETTINGS;
}

//-- Stop ---------------------------------------------------------------------
// This dll must cease all runtime activities
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Stop()
{
  FUNCTION_ENTER;
}

//-- Destroy ------------------------------------------------------------------
// Do everything before unload of this add-on
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Destroy()
{
  FUNCTION_ENTER;

  presets::unloadPreset();
  renderer::unloadTextures();

  if (lpresets)
    delete[] lpresets, lpresets = nullptr;

  audio::Destroy();

#if defined(HAS_GLES)
  glDeleteBuffers(1, &state->vertex_buffer);
#endif

  initialized = false;
}

//-- HasSettings --------------------------------------------------------------
// Returns true if this add-on use settings
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
bool ADDON_HasSettings()
{
  FUNCTION_ENTER;

  return true;
}

//-- GetStatus ---------------------------------------------------------------
// Returns the current Status of this visualisation
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_GetStatus()
{
  FUNCTION_ENTER;

  return ADDON_STATUS_OK;
}

//-- GetSettings --------------------------------------------------------------
// Return the settings for XBMC to display
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet)
{
  FUNCTION_ENTER;
  return 0;
}

//-- FreeSettings --------------------------------------------------------------
// Free the settings struct passed from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------

void ADDON_FreeSettings()
{
  FUNCTION_ENTER;
}

//-- SetSetting ---------------------------------------------------------------
// Set a specific Setting value (called from XBMC)
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value)
{
  FUNCTION_ENTER;

  cout << "ADDON_SetSetting " << strSetting << std::endl;
  if (!strSetting || !value)
    return ADDON_STATUS_UNKNOWN;

  // TODO Someone _needs_ to fix this API in kodi, its terrible.
  // a) Why not use GetSettings instead of hacking SetSettings like this?
  // b) Why does it give index and not settings key?
  // c) Seemingly random ###End which if you never write will while(true) the app
  // d) Writes into const setting and value...
  if (strcmp(strSetting, "###GetSavedSettings") == 0)
  {
    if (strcmp((char*)value, "0") == 0)
    {
      strcpy((char*)strSetting, "lastpresetidx");
      sprintf ((char*)value, "%i", (int)g_currentPreset);

    }else if (strcmp((char*)value, "1") == 0)
    {
        strcpy((char*)strSetting, "lastpresetchangetime");
        sprintf ((char*)value, "%i", (int)lastPresetChangeTime);

    }else if (strcmp((char*)value, "2") == 0)
    {
      strcpy((char*)strSetting, "###End");
    }

    return ADDON_STATUS_OK;
  }

  if (strcmp(strSetting, "lastpresetidx") == 0)
  {
    cout << "lastpresetidx = " << *((int *)value) << endl;
    g_currentPreset = *(int *)value % g_presets.size();
    presets::launch(g_currentPreset);

  } else if (strcmp(strSetting, "lastpresetchangetime") == 0)
  {
    int last = *(int*)value;
    cout << "lastpresetchangetime = " << last << endl;
    if (last < PLATFORM::GetTimeMs() + g_shufflePresetsDuration)
        lastPresetChangeTime = last;
    else
        lastPresetChangeTime = PLATFORM::GetTimeMs();

  } else if (strcmp(strSetting, "shufflePresets") == 0)
  {
    g_shufflePresets = *(bool*)value;
    if (g_shufflePresets) cout << "Enable preset shuffling" << endl;
    else cout << "Disable preset shuffling" << endl;

  }else if (strcmp(strSetting, "shufflePresetsDelay") == 0)
  {
    g_shufflePresetsDuration = *(float*)value * 1000.;
    cout << "Change preset every: " << g_shufflePresetsDuration << " ms" << endl;

  } else if (strcmp(strSetting, "shufflePresetsNonuniform") == 0)
  {
    g_shufflePresetsNonunfirom = *(bool*)value;
    if (g_shufflePresetsNonunfirom) cout << "Use non-uniform preset shuffling" << endl;
    else cout << "Unfirom random sampling shuffling" << endl;

  }else
  {
    cout << "Unknow setting: " << strSetting << endl;
    return ADDON_STATUS_UNKNOWN;
  }

  return ADDON_STATUS_OK;
}

//-- Announce -----------------------------------------------------------------
// Receive announcements from XBMC
// !!! Add-on master function !!!
//-----------------------------------------------------------------------------
void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data)
{
  FUNCTION_ENTER;

  cout << "ADDON_Announce " << flag << " " << sender << " " << message << std::endl;
}
