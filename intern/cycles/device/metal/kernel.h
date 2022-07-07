/* SPDX-License-Identifier: Apache-2.0
 * Copyright 2021-2022 Blender Foundation */

#pragma once

#ifdef WITH_METAL

#  include "device/kernel.h"
#  include <Metal/Metal.h>

CCL_NAMESPACE_BEGIN

class MetalDevice;

enum {
  METALRT_FUNC_DEFAULT_TRI,
  METALRT_FUNC_DEFAULT_BOX,
  METALRT_FUNC_SHADOW_TRI,
  METALRT_FUNC_SHADOW_BOX,
  METALRT_FUNC_LOCAL_TRI,
  METALRT_FUNC_LOCAL_BOX,
  METALRT_FUNC_CURVE_RIBBON,
  METALRT_FUNC_CURVE_RIBBON_SHADOW,
  METALRT_FUNC_CURVE_ALL,
  METALRT_FUNC_CURVE_ALL_SHADOW,
  METALRT_FUNC_POINT,
  METALRT_FUNC_POINT_SHADOW,
  METALRT_FUNC_NUM
};

enum { METALRT_TABLE_DEFAULT, METALRT_TABLE_SHADOW, METALRT_TABLE_LOCAL, METALRT_TABLE_NUM };

/* Pipeline State Object types */
enum {
  /* A kernel that can be used with all scenes, supporting all features.
   * It is slow to compile, but only needs to be compiled once and is then
   * cached for future render sessions. This allows a render to get underway
   * on the GPU quickly.
   */
  PSO_GENERIC,

  /* A kernel that is relatively quick to compile, but is specialized for the
   * scene being rendered. It only contains the functionality and even baked in
   * constants for values that means it needs to be recompiled whenever a
   * dependent setting is changed. The render performance of this kernel is
   * significantly faster though, and justifies the extra compile time.
   */
  /* METAL_WIP: This isn't used and will require more changes to enable. */
  PSO_SPECIALISED,

  PSO_NUM
};

const char *kernel_type_as_string(int kernel_type);

struct MetalKernelPipeline {

  void compile();

  id<MTLLibrary> mtlLibrary = nil;
  bool scene_specialized;
  string source_md5;

  bool use_metalrt;
  bool metalrt_hair;
  bool metalrt_hair_thick;
  bool metalrt_pointcloud;

  int threads_per_threadgroup;

  DeviceKernel device_kernel;
  bool loaded = false;
  id<MTLDevice> mtlDevice = nil;
  id<MTLFunction> function = nil;
  id<MTLComputePipelineState> pipeline = nil;
  int num_threads_per_block = 0;

  string error_str;

  API_AVAILABLE(macos(11.0))
  id<MTLIntersectionFunctionTable> intersection_func_table[METALRT_TABLE_NUM] = {nil};
  id<MTLFunction> rt_intersection_function[METALRT_FUNC_NUM] = {nil};
};

/* Cache of Metal kernels for each DeviceKernel. */
namespace MetalDeviceKernels {

bool load(MetalDevice *device, bool scene_specialized);
const MetalKernelPipeline *get_best_pipeline(const MetalDevice *device, DeviceKernel kernel);

} /* namespace MetalDeviceKernels */

CCL_NAMESPACE_END

#endif /* WITH_METAL */