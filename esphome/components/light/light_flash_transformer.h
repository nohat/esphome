#pragma once

#include "light_transformer.h"
#include "light_color_values.h"

namespace esphome {
namespace light {

// Forward declaration
class LightState;

/// Transformer for flash effects (brief target state, then restore original)
class LightFlashTransformer : public FiniteTransformer {
 public:
  LightFlashTransformer(LightState &state) : state_(state) {}

  void start() override;
  optional<LightColorValues> apply() override;
  void stop() override;
  bool is_finished() override;

 protected:
  LightState &state_;
  uint32_t transition_length_;
  std::unique_ptr<LightTransformer> transformer_{nullptr};
  bool begun_lightstate_restore_;
};

}  // namespace light
}  // namespace esphome
