/**
 * Copyright 2020. Huawei Technologies Co., Ltd. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef C_ARENGINE_HELLOE_AR_UTIL_H
#define C_ARENGINE_HELLOE_AR_UTIL_H

#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <vector>

#include <android/asset_manager.h>
#include <android/log.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <jni.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "huawei_arengine_interface.h"

#ifdef  __ANDROID__

#include <android/log.h>

#ifndef LOG_TAG
#define LOG_TAG "larkar"
#endif

#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, LOG_TAG, __VA_ARGS__)

#define LogD(tag, ...) __android_log_print(ANDROID_LOG_DEBUG, tag, __VA_ARGS__)
#define LogE(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)

#else //  __ANDROID__

// TODO ADD LOG
#define LOGV(msg) (void)0
#define LOGD(msg) (void)0
#define LOGI(msg) (void)0
#define LOGW(msg) (void)0
#define LOGE(msg) (void)0
#define LOGF(msg) (void)0

#endif //

#ifndef CHECK
#define CHECK(condition)                                                   \
    if (!(condition)) {                                                      \
        LOGE("*** CHECK FAILED at %d: %s", __LINE__, #condition); \
        abort();                                                               \
    }
#endif

namespace gWorldAr {
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

    // Utilities for C hello AR project.
    namespace util {
        class Util {
        public:
            explicit Util(const HwArSession *session)
            {
                HwArPose_create(session, nullptr, &pose);
            }

            ~Util()
            {
                HwArPose_destroy(pose);
            }

            HwArPose *GetArPose() const
            {
                return pose;
            }

            // Delete copy constructors.
            Util(const Util &) = delete;

            void operator=(const Util &) = delete;

        private:
            HwArPose *pose = nullptr;
        };

        using FileInfor = struct {
            AAssetManager *mgr;
            std::string fileName;
        };

        using FileData = struct {
            std::vector<char *> perVertInfoList;
            int i;
        };

        using DrawTempData = struct {
            std::vector<GLfloat> tempPositions;
            std::vector<GLfloat> tempNormals;
            std::vector<GLfloat> tempUvs;
            std::vector<GLushort> vertexIndices;
            std::vector<GLushort> normalIndices;
            std::vector<GLushort> uvIndices;
        };

        // ??????GL????????????????????????????????????.
        // @param ????????? GL?????????????????????.
        void CheckGlError(const char *operation);

        // ?????????????????????ID.
        // @param ???????????????????????????.
        // @param ??????????????????????????????.
        // @return GL?????????
        GLuint CreateProgram(const char *vertexSource, const char *fragmentSource);

        void GetTransformMatrixFromAnchor(HwArSession *arSession,
                                          const HwArAnchor *arAnchor,
                                          glm::mat4 *outModelMat);

        glm::vec3 GetPlaneNormal(const HwArSession &arSession, const HwArPose &planePose);

        glm::quat GetArPose(const HwArSession &arSession, const HwArPose &planePose);
        glm::vec3 GetArPosePosition(const HwArSession &arSession, const HwArPose &planePose);

        glm::vec3 GetCPPosition(const HwArSession &arSession,
                                const HwArPose &planePose,
                                const HwArPose &cameraPose);
        // ????????????????????????????????????????????????????????????y??????????????????????????????.
        // ????????????????????????????????????????????????
        float CalculateDistanceToPlane(const HwArSession &arSession,
                                       const HwArPose &planePose,
                                       const HwArPose &cameraPose);

        bool LoadPngFromAssetManager(int target, const std::string &path);

        // ?????????????????????assets???????????????obj??????.
        //
        // @param fileInformation, AAssetManager?????????obj????????????.
        // @param outVertices, ????????????.
        // @param outNormals, ????????????.
        // @param outUv, ????????????UV??????.
        // @param outIndices, ??????????????????.
        // @return ?????????????????????obj?????????true????????????false.
        bool LoadObjFile(FileInfor fileInformation,
                         std::vector<GLfloat> &outVertices,
                         std::vector<GLfloat> &outNormals,
                         std::vector<GLfloat> &outUv,
                         std::vector<GLushort> &outIndices);

    }
}
#endif