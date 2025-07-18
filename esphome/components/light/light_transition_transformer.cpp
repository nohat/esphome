#include "light_transition_transformer.h"

namespace esphome {
namespace light {

void LightTransitionTransformer::start() {
  // When turning light on from off state, use target state and only increase brightness from zero.
  if (!this->start_values_.is_on() && this->target_values_.is_on()) {
    this->start_values_ = LightColorValues(this->target_values_);
    this->start_values_.set_brightness(0.0f);
  }

  // When turning light off from on state, use source state and only decrease brightness to zero. Use a second
  // variable for transition end state, as overwriting target_values breaks LightState logic.
  if (this->start_values_.is_on() && !this->target_values_.is_on()) {
    this->end_values_ = LightColorValues(this->start_values_);
    this->end_values_.set_brightness(0.0f);
  } else {
    this->end_values_ = LightColorValues(this->target_values_);
  }

  // When changing color mode, go through off state, as color modes are orthogonal and there can't be two active.
  if (this->start_values_.get_color_mode() != this->target_values_.get_color_mode()) {
    this->changing_color_mode_ = true;
    this->intermediate_values_ = this->start_values_;
    this->intermediate_values_.set_state(false);
  }
}

optional<LightColorValues> LightTransitionTransformer::apply() {
  float p = this->get_progress_();

  // Halfway through, when intermediate state (off) is reached, flip it to the target, but remain off.
  if (this->changing_color_mode_ && p > 0.5f &&
      this->intermediate_values_.get_color_mode() != this->target_values_.get_color_mode()) {
    this->intermediate_values_ = this->target_values_;
    this->intermediate_values_.set_state(false);
  }

  LightColorValues &start = this->changing_color_mode_ && p > 0.5f ? this->intermediate_values_ : this->start_values_;
  LightColorValues &end = this->changing_color_mode_ && p < 0.5f ? this->intermediate_values_ : this->end_values_;
  if (this->changing_color_mode_)
    p = p < 0.5f ? p * 2 : (p - 0.5) * 2;

  float v = LightTransitionTransformer::smoothed_progress(p);
  return LightColorValues::lerp_with_hue_path(start, end, v, this->hue_path_);
}

}  // namespace light
}  // namespace esphome
