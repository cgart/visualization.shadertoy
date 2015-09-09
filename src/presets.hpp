#ifndef PRESETS__HPP__
#define PRESETS__HPP__

namespace presets
{

void unloadPreset();
void loadPreset(int preset, std::string vsSource, std::string fsSource);
void RenderTo(GLuint shader, GLuint effect_fb);
int determine_bits_precision();
double measure_performance(int preset, int size);
void launch(int preset);

}

#endif 
