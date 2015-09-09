#include "globals.hpp"
#include "presets.hpp"
#include "renderer.hpp"
#include "audio.hpp"

namespace presets
{

using namespace std;


#if defined(HAS_GLES)

std::string vsSource = TO_STRING(
         precision mediump float;
         precision mediump int;
         attribute vec4 vertex;
         varying vec2 vTextureCoord;
         uniform vec2 uScale;
         void main(void)
         {
            gl_Position = vertex;
            vTextureCoord = vertex.xy*0.5+0.5;
            vTextureCoord.x = vTextureCoord.x * uScale.x;
            vTextureCoord.y = vTextureCoord.y * uScale.y;
         }
  );

std::string render_vsSource = TO_STRING(
         precision mediump float;
         precision mediump int;
         attribute vec4 vertex;
         varying vec2 vTextureCoord;
         void main(void)
         {
            gl_Position = vertex;
            vTextureCoord = vertex.xy*0.5+0.5;
         }
  );

std::string render_fsSource = TO_STRING(
         precision mediump float;
         precision mediump int;
         varying vec2 vTextureCoord;
         uniform sampler2D uTexture;
         void main(void)
         {
            gl_FragColor = texture2D(uTexture, vTextureCoord);
         }
  );
#else
std::string vsSource = "void main() { gl_Position = ftransform(); }";
#endif




void unloadPreset()
{
  FUNCTION_ENTER;

  if (shadertoy_shader) {
    glDeleteProgram(shadertoy_shader);
    shadertoy_shader = 0;
  }
#if defined(HAS_GLES)
  if (state->framebuffer_texture)
  {
    glDeleteTextures(1, &state->framebuffer_texture);
    state->framebuffer_texture = 0;
  }
  if (state->effect_fb)
  {
    glDeleteFramebuffers(1, &state->effect_fb);
    state->effect_fb = 0;
  }
  if (state->render_program) {
    glDeleteProgram(state->render_program);
    state->render_program = 0;
  }
#endif

  preset_active = false;
}


void loadPreset(int preset, std::string vertSource, std::string fragSource)
{
  FUNCTION_ENTER;

  unloadPreset();
  shadertoy_shader = renderer::compileAndLinkProgram(vertSource.c_str(), fragSource.c_str());

  iResolutionLoc        = glGetUniformLocation(shadertoy_shader, "iResolution");
  iGlobalTimeLoc        = glGetUniformLocation(shadertoy_shader, "iGlobalTime");
  iChannelTimeLoc       = glGetUniformLocation(shadertoy_shader, "iChannelTime");
  iMouseLoc             = glGetUniformLocation(shadertoy_shader, "iMouse");
  iDateLoc              = glGetUniformLocation(shadertoy_shader, "iDate");
  iSampleRateLoc        = glGetUniformLocation(shadertoy_shader, "iSampleRate");
  iChannelResolutionLoc = glGetUniformLocation(shadertoy_shader, "iChannelResolution");
  iChannelLoc[0]        = glGetUniformLocation(shadertoy_shader, "iChannel0");
  iChannelLoc[1]        = glGetUniformLocation(shadertoy_shader, "iChannel1");
  iChannelLoc[2]        = glGetUniformLocation(shadertoy_shader, "iChannel2");
  iChannelLoc[3]        = glGetUniformLocation(shadertoy_shader, "iChannel3");

#if defined(HAS_GLES)
  state->uScale         = glGetUniformLocation(shadertoy_shader, "uScale");
  state->attr_vertex_e  = glGetAttribLocation(shadertoy_shader,  "vertex");
#endif

  state->render_program = renderer::compileAndLinkProgram(render_vsSource.c_str(), render_fsSource.c_str());
  state->uTexture       = glGetUniformLocation(state->render_program, "uTexture");
  state->attr_vertex_r  = glGetAttribLocation(state->render_program,  "vertex");
  // Prepare a texture to render to
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &state->framebuffer_texture);
  glBindTexture(GL_TEXTURE_2D, state->framebuffer_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, state->fbwidth, state->fbheight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  // Prepare a framebuffer for rendering
  glGenFramebuffers(1, &state->effect_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, state->effect_fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state->framebuffer_texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  initial_time = PLATFORM::GetTimeMs();
  preset_active = true;
}

void RenderTo(GLuint shader, GLuint effect_fb)
{
  glUseProgram(shader);

#if !defined(HAS_GLES)
  glDisable(GL_BLEND);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(-1, 1, -1, 1, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix();
#endif

  // rebuild shader if changed
  if (shader == shadertoy_shader)
  {
    GLuint w = win_width, h = win_height;
#if defined(HAS_GLES)
    if (state->fbwidth && state->fbheight)
      w = state->fbwidth, h = state->fbheight;
#endif
    int64_t intt = PLATFORM::GetTimeMs() - initial_time;
    if (bits_precision)
      intt &= (1<<bits_precision)-1;

    if (needsUpload) {
      for (int i=0; i<4; i++) {
        if (g_presets[g_currentPreset].channel[i] == 99) {
          glActiveTexture(GL_TEXTURE0 + i);
          glBindTexture(GL_TEXTURE_2D, iChannel[i]);
          glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, NUM_BANDS, 2, GL_LUMINANCE, GL_UNSIGNED_BYTE, audio::AudioData());
        }
      }
      needsUpload = false;
    }

    float t = intt / 1000.0f;
    GLfloat tv[] = { t, t, t, t };

    glUniform3f(iResolutionLoc, w, h, 0.0f);
    glUniform1f(iGlobalTimeLoc, t);
    glUniform1f(iSampleRateLoc, audio::GetSamplesPerSec());
    glUniform1fv(iChannelTimeLoc, 4, tv);
#if defined(HAS_GLES)
    glUniform2f(state->uScale, (GLfloat)win_width/state->fbwidth, (GLfloat)win_height/state->fbheight);
#endif
    time_t now = time(NULL);
    tm *ltm = localtime(&now);

    float year = 1900 + ltm->tm_year;
    float month = ltm->tm_mon;
    float day = ltm->tm_mday;
    float sec = (ltm->tm_hour * 60 * 60) + (ltm->tm_min * 60) + ltm->tm_sec;

    glUniform4f(iDateLoc, year, month, day, sec);

    for (int i=0; i<4; i++) {
      glActiveTexture(GL_TEXTURE0 + i);
#if !defined(HAS_GLES)
      glEnable(GL_TEXTURE_2D);
#endif
      glUniform1i(iChannelLoc[i], i);
      glBindTexture(GL_TEXTURE_2D, iChannel[i]);
    }
  } else {
#if defined(HAS_GLES)
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, state->framebuffer_texture);
    glUniform1i(state->uTexture, 0); // first currently bound texture "GL_TEXTURE0"
#endif
  }

#if defined(HAS_GLES)
  // Draw the effect to a texture or direct to framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, effect_fb);

  GLuint attr_vertex = shader == shadertoy_shader ? state->attr_vertex_e : state->attr_vertex_r;
  glBindBuffer(GL_ARRAY_BUFFER, state->vertex_buffer);
  glVertexAttribPointer(attr_vertex, 4, GL_FLOAT, 0, 16, 0);
  glEnableVertexAttribArray(attr_vertex);
  glDrawArrays( GL_TRIANGLE_FAN, 0, 4 );
  glDisableVertexAttribArray(attr_vertex);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
#else
  glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 0.0f);
    glVertex3f( 1.0f, 1.0f, 0.0f);
    glVertex3f( 1.0f,-1.0f, 0.0f);
    glVertex3f(-1.0f,-1.0f, 0.0f);
  glEnd();
#endif

  for (int i=0; i<4; i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }
#if !defined(HAS_GLES)
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
#endif
  glUseProgram(0);
}


int determine_bits_precision()
{
  FUNCTION_ENTER;

  std::string vsPrecisionSource = TO_STRING(
	void mainImage( out vec4 fragColor, in vec2 fragCoord )
	{
	    float y = ( fragCoord.y / iResolution.y ) * 26.0;
	    float x = 1.0 - ( fragCoord.x / iResolution.x );
	    float b = fract( pow( 2.0, floor(y) ) + x );
	    if (fract(y) >= 0.9)
		b = 0.0;
	    fragColor = vec4(b, b, b, 1.0 );
	}
  );
  std::string fsPrecisionSource = renderer::fsHeader + "\n" + vsPrecisionSource + "\n" + renderer::fsFooter;

  state->fbwidth = 32, state->fbheight = 26*10;
  loadPreset(0, vsSource, fsPrecisionSource);
  RenderTo(shadertoy_shader, state->effect_fb);
  glFinish();

  unsigned char *buffer = new unsigned char[state->fbwidth * state->fbheight * 4];
  if (buffer)
    glReadPixels(0, 0, state->fbwidth, state->fbheight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
  #if 0
  for (int j=0; j<state->fbheight; j++) {
    for (int i=0; i<state->fbwidth; i++) {
      printf("%02x ", buffer[4*(j*state->fbwidth+i)]);
    }
    printf("\n");
  }
  #endif
  int bits = 0;
  unsigned char b = 0;
  for (int j=0; j<state->fbheight; j++) {
    unsigned char c = buffer[4*(j*state->fbwidth+(state->fbwidth>>1))];
    if (c && !b)
      bits++;
    b = c;
  }
  delete buffer;
  unloadPreset();
  return bits;
}

double measure_performance(int preset, int size)
{
  FUNCTION_ENTER;

  int iterations = -1;
  std::string fsSource = renderer::createShader(g_presets[preset].file);

  state->fbwidth = state->fbheight = size;
  loadPreset(preset, vsSource, fsSource);

  int64_t end, start;
  do {
#if defined(HAS_GLES)
    RenderTo(shadertoy_shader, state->effect_fb);
    RenderTo(state->render_program, state->effect_fb);
#else
    RenderTo(shadertoy_shader, 0);
#endif
    glFinish();
    if (++iterations == 0)
      start = PLATFORM::GetTimeMs();
    end = PLATFORM::GetTimeMs();
  } while (end - start < 50);
  double t = (double)(end - start)/iterations;
  //printf("%s %dx%d %.1fms = %.2f fps\n", __func__, size, size, t, 1000.0/t);
  unloadPreset();
  return t;
}

void launch(int preset)
{
  FUNCTION_ENTER;

  bits_precision = determine_bits_precision();
  // mali-400 has only 10 bits which means millisecond timer wraps after ~1 second.
  // we'll fudge that up a bit as having a larger range is more important than ms accuracy
  bits_precision = max(bits_precision, 13);
  printf("bits=%d\n", bits_precision);

  renderer::unloadTextures();
  for (int i=0; i<4; i++) {
    if (g_presets[preset].channel[i] >= 0)
      iChannel[i] = renderer::loadTexture(g_presets[preset].channel[i]);
  }

  const int size1 = 256, size2=512;
  double t1 = measure_performance(preset, size1);
  double t2 = measure_performance(preset, size2);

  double expected_fps = 50.0;
  // time per pixel for rendering fragment shader
  double B = (t2-t1)/(size2*size2-size1*size1);
  // time to render to screen
  double A = t2 - size2*size2 * B;
  // how many pixels get the desired fps
  double pixels = (1000.0/expected_fps - A) / B;
  state->fbwidth = sqrtf(pixels * win_width / win_height);
  if (state->fbwidth * 4 >= win_width * 3)
    state->fbwidth = win_width * 3 / 4;

  if (state->fbwidth < 320)
    state->fbwidth = 320;
  state->fbheight = state->fbwidth * win_height / win_width;

  printf("expected fps=%f, pixels=%f %dx%d (A:%f B:%f t1:%.1f t2:%.1f)\n", expected_fps, pixels, state->fbwidth, state->fbheight, A, B, t1, t2);

  std::string fsSource = renderer::createShader(g_presets[preset].file);
  loadPreset(preset, vsSource, fsSource);
}

}
