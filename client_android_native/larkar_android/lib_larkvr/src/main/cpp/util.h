/*
 * Copyright 2019 Google LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef HELLO_CARDBOARD_ANDROID_SRC_MAIN_JNI_UTIL_H_
#define HELLO_CARDBOARD_ANDROID_SRC_MAIN_JNI_UTIL_H_

#include <android/asset_manager.h>
#include <jni.h>

#include <string>
#include <array>
#include <vector>

#include <GLES2/gl2.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <android/log.h>

#define LOG_TAG "HelloCardboardApp"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#define LogD(tag, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#define LogE(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)
#define HELLOCARDBOARD_CHECK(condition)                                    \
  if (!(condition)) {                                                      \
    LOGE("*** CHECK FAILED at %s:%d: %s", __FILE__, __LINE__, #condition); \
    abort();                                                               \
  }

namespace ndk_hello_cardboard {

static inline uint64_t GetTimestampUs() {
  timeval tv;
  gettimeofday(&tv, nullptr);

  uint64_t Current = (uint64_t)tv.tv_sec * 1000 * 1000 + tv.tv_usec;
  return Current;
}

static inline double GetTimeInSeconds()
{
  struct timespec now;
  clock_gettime( CLOCK_MONOTONIC, &now );
  return ( now.tv_sec * 1e9 + now.tv_nsec ) * 0.000000001;
}

inline std::string GetTimestampMillStr() {
  uint64_t ms = GetTimestampUs() * 1000;
  return std::to_string(ms);
}

class Matrix4x4 {
 public:
  float m[4][4];

  // Multiplies two matrices.
  Matrix4x4 operator*(const Matrix4x4& right);

  // Multiplies a matrix with a vector.
  std::array<float, 4> operator*(const std::array<float, 4>& vec);

  // Converts a matrix to an array of floats suitable for passing to OpenGL.
  std::array<float, 16> ToGlArray();
};

struct Quatf {
  float x;
  float y;
  float z;
  float w;

  Quatf(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}

  Quatf() : x(0), y(0), z(0), w(1) {}

  static Quatf FromXYZW(float q[4]) { return Quatf(q[0], q[1], q[2], q[3]); }

  Matrix4x4 ToMatrix();
};


inline glm::mat4 toGlm(const Matrix4x4& om) {
    return glm::transpose(glm::make_mat4(&om.m[0][0]));
}

inline glm::quat toGlm(const Quatf& oq) {
    return glm::make_quat(&oq.x);
}

inline Matrix4x4 fromGlm(const glm::mat4 & m) {
    Matrix4x4 result = {};
    glm::mat4 transposed(glm::transpose(m));
    memcpy(result.m, &(transposed[0][0]), sizeof(float) * 16);
    return result;
}

inline Quatf fromGlm(const glm::quat & q) {
    Quatf result;
    result.x = q.x;
    result.y = q.y;
    result.z = q.z;
    result.w = q.w;
    return result;
}

/**
 * Converts an array of floats to a matrix.
 *
 * @param vec GL array
 * @return Obtained matrix
 */
Matrix4x4 GetMatrixFromGlArray(float* vec);

/**
 * Construct a translation matrix.
 *
 * @param translation Translation array
 * @return Obtained matrix
 */
Matrix4x4 GetTranslationMatrix(const std::array<float, 3>& translation);

/**
 * Computes the angle between two vectors.
 *
 * @param vec1 First vector
 * @param vec2 Second vector
 * @return Angle between the vectors
 */
float AngleBetweenVectors(const std::array<float, 4>& vec1,
                          const std::array<float, 4>& vec2);

/**
 * Gets system boot time in nanoseconds.
 *
 * @return System boot time in nanoseconds
 */
int64_t GetBootTimeNano();

/**
 * Generates a random floating point number between |min| and |max|.
 *
 * @param min Minimum range
 * @param max Maximum range
 * @return Random float number
 */
float RandomUniformFloat(float min, float max);

/**
 * Generates a random integer in the range [0, max_val).
 *
 * @param max_val Maximum range
 * @return Random int number
 */
int RandomUniformInt(int max_val);

/**
 * Checks for OpenGL errors, and crashes if one has occurred.  Note that this
 * can be an expensive call, so real applications should call this rarely.
 *
 * @param file File name
 * @param line Line number
 * @param label Error label
 */
void CheckGlError(const char* file, int line, const char* label);

#define CHECKGLERROR(label) CheckGlError(__FILE__, __LINE__, label)

/**
 * Converts a string into an OpenGL ES shader.
 *
 * @param type The type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER).
 * @param shader_source The source code of the shader.
 * @return The shader object handler, or 0 if there's an error.
 */
GLuint LoadGLShader(GLenum type, const char* shader_source);

class TexturedMesh {
 public:
  TexturedMesh() = default;

  // Initializes the mesh from a .obj file.
  //
  // @return True if initialization was successful.
  bool Initialize(GLuint position_attrib, GLuint uv_attrib,
                  const std::string& obj_file_path, AAssetManager* asset_mgr);

  // Draws the mesh. The u_MVP uniform should be set before calling this using
  // glUniformMatrix4fv(), and a texture should be bound to GL_TEXTURE0.
  void Draw() const;

 private:
  std::vector<GLfloat> vertices_;
  std::vector<GLfloat> uv_;
  std::vector<GLushort> indices_;
  GLuint position_attrib_{0};
  GLuint uv_attrib_{0};
};

class Texture {
 public:
  Texture() = default;

  ~Texture();

  // Initializes the texture.
  //
  // After this is called the texture will be bound, replacing any previously
  // bound texture.
  //
  // @return True if initialization was successful.
  // TODO(b/138789810): Share some parts of the code between Android and iOS samples.
  bool Initialize(JNIEnv* env, jobject java_asset_mgr,
                  const std::string& texture_path);

  // Binds the texture, replacing any previously bound texture.
  void Bind() const;

 private:
  GLuint texture_id_{0};
};

class EnvWrapper {
public:
    EnvWrapper(JavaVM * vm, JNIEnv * env, bool needAttach)
            :vm_(vm), env_(env), need_attach_(needAttach) {
        if (need_attach_) {
            vm_->AttachCurrentThread(&env_, nullptr);
        }
    }

    ~EnvWrapper() {
        if (need_attach_ && env_ != nullptr) {
            vm_->DetachCurrentThread();
        }
    };

    inline JNIEnv * get() {
        return env_;
    }
private:
    JavaVM* vm_;
    JNIEnv* env_;
    bool need_attach_;
};
}  // namespace ndk_hello_cardboard

#endif  // HELLO_CARDBOARD_ANDROID_SRC_MAIN_JNI_UTIL_H_
