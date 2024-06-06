/**
 * @copyright (C) 2017 Melexis N.V.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef _MLX90640_API_H_
#define _MLX90640_API_H_

#include <stdint.h>

#define SCALEALPHA 0.000001

typedef struct
{
    uint8_t ktaScale;
    uint8_t kvScale;
    uint8_t alphaScale;
    uint8_t resolutionEE;
    uint8_t calibrationModeEE;
    int16_t kVdd;
    int16_t vdd25;
    uint16_t vPTAT25;
    int16_t gainEE;
    float KvPTAT;
    float KtPTAT;
    float alphaPTAT;
    float tgc;
    float cpKv;
    float cpKta;
    float KsTa;
    float ksTo[5];
    float cpAlpha[2];
    float ilChessC[3];
    int16_t cpOffset[2];
    uint16_t brokenPixels[5];
    uint16_t outlierPixels[5];
    int16_t ct[5];
    int8_t kta[768];
    int8_t kv[768];
    int16_t offset[768];
    uint16_t alpha[768];
} paramsMLX90640;

extern int MLX90640_DumpEE(void* slave, uint16_t *eeData);
extern int MLX90640_SynchFrame(void* slave);
extern int MLX90640_TriggerMeasurement(void* slave);
extern int MLX90640_GetFrameData(void* slave, uint16_t *frameData);
extern int MLX90640_ExtractParameters(uint16_t *eeData, paramsMLX90640 *mlx90640);
extern float MLX90640_GetVdd(uint16_t *frameData, const paramsMLX90640 *params);
extern float MLX90640_GetTa(uint16_t *frameData, const paramsMLX90640 *params);
extern void MLX90640_GetImage(uint16_t *frameData, const paramsMLX90640 *params, float *result);
extern void MLX90640_CalculateTo(uint16_t *frameData, const paramsMLX90640 *params, float emissivity, float tr, float *result);
extern int MLX90640_SetResolution(void* slave, uint8_t resolution);
extern int MLX90640_GetCurResolution(void* slave);
extern int MLX90640_SetRefreshRate(void* slave, uint8_t refreshRate);
extern int MLX90640_GetRefreshRate(void* slave);
extern int MLX90640_GetSubPageNumber(uint16_t *frameData);
extern int MLX90640_GetCurMode(void* slave);
extern int MLX90640_SetInterleavedMode(void* slave);
extern int MLX90640_SetChessMode(void* slave);
extern void MLX90640_BadPixelsCorrection(uint16_t *pixels, float *to, int mode, paramsMLX90640 *params);

#endif