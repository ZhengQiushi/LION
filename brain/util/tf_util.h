
#pragma once

#include <tensorflow/c/c_api.h>

/**
 * Simple utility functions associated with Tensorflow
 */

namespace lion {
namespace brain {
class TFUtil {
 public:
  static const char *GetTFVersion() { return TF_Version(); }
};
}  // namespace brain
}  // namespace lion
