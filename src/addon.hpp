/*
 *      Copyright (C) 2005-2013 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#ifndef ADDON_HPP__
#define ADDON_HPP__

#include "kodi/xbmc_vis_dll.h"

extern "C" {
    void Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, const char* szSongName);
    void Render();
    void AudioData(const float* pAudioData, int iAudioDataLength, float *pFreqData, int iFreqDataLength);
    void GetInfo(VIS_INFO *pInfo);
    unsigned int GetSubModules(char ***names);
    bool OnAction(long flags, const void *param);
    unsigned int GetPresets(char ***presets);
    unsigned GetPreset();
    bool IsLocked();
    ADDON_STATUS ADDON_Create(void* hdl, void* props);
    void ADDON_Stop();
    void ADDON_Destroy();
    bool ADDON_HasSettings();
    ADDON_STATUS ADDON_GetStatus();
    unsigned int ADDON_GetSettings(ADDON_StructSetting ***sSet);
    void ADDON_FreeSettings();
    ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value);
    ADDON_STATUS ADDON_SetSetting(const char *strSetting, const void* value);
    void ADDON_Announce(const char *flag, const char *sender, const char *message, const void *data);
}

#endif 
