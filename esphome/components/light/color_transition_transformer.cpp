#include "color_transition_transformer.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace light {

ColorTransitionTransformer::ColorTransitionTransformer(ColorTransitionType type, TransitionDirection direction,
                                                       float speed)
    : type_(type), direction_(direction), speed_(speed) {}

void ColorTransitionTransformer::start() {
  // Store the initial value for the transition type
  this->initial_value_ = this->start_values_.get_transition_value(this->type_);
}

optional<LightColorValues> ColorTransitionTransformer::apply() {
  // Calculate elapsed time since start
  float elapsed_seconds = this->get_elapsed_time_() / 1000.0f;
  float delta = this->speed_ * elapsed_seconds;

  if (this->direction_ == TRANSITION_DIRECTION_DOWN) {
    delta = -delta;
  }

  auto new_values = this->start_values_;

  switch (this->type_) {
    case COLOR_TRANSITION_BRIGHTNESS:
      new_values.update_brightness(delta);
      break;
    case COLOR_TRANSITION_COLOR_TEMPERATURE:
      new_values.update_color_temperature(delta);
      break;
    case COLOR_TRANSITION_HUE:
      new_values.update_hue(delta, this->hue_path_);
      break;
    case COLOR_TRANSITION_SATURATION:
      new_values.update_saturation(delta);
      break;
    case COLOR_TRANSITION_CIE_X:
      new_values.update_cie_x(delta);
      break;
    case COLOR_TRANSITION_CIE_Y:
      new_values.update_cie_y(delta);
      break;
    default:
      return {};
  }

  return new_values;
}

void ColorTransitionTransformer::stop() {
  // Nothing special needed for cleanup
}

uint32_t ColorTransitionTransformer::get_elapsed_time_() const { return millis() - this->start_time_; }

}  // namespace light
}  // namespace esphome
