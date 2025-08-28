#include "light_flash_transformer.h"
#include "light_state.h"
#include "light_output.h"
#include "light_transformer.h"

namespace esphome {
namespace light {

void LightFlashTransformer::start() {
  this->transition_length_ = this->state_.get_flash_transition_length();
  if (this->transition_length_ * 2 > this->length_)
    this->transition_length_ = this->length_ / 2;

  this->begun_lightstate_restore_ = false;

  // first transition to original target
  this->transformer_ = this->state_.get_output()->create_default_transition();
  auto *finite_transformer = static_cast<FiniteTransformer *>(this->transformer_.get());
  finite_transformer->setup(this->state_.current_values, this->target_values_, this->transition_length_);
}

optional<LightColorValues> LightFlashTransformer::apply() {
  optional<LightColorValues> result = {};

  if (this->transformer_ == nullptr && millis() > this->start_time_ + this->length_ - this->transition_length_) {
    // second transition back to start value
    this->transformer_ = this->state_.get_output()->create_default_transition();
    auto *finite_transformer = static_cast<FiniteTransformer *>(this->transformer_.get());
    finite_transformer->setup(this->state_.current_values, this->get_start_values(), this->transition_length_);
    this->begun_lightstate_restore_ = true;
  }

  if (this->transformer_ != nullptr) {
    result = this->transformer_->apply();

    if (this->transformer_->is_finished()) {
      this->transformer_->stop();
      this->transformer_ = nullptr;
    }
  }

  return result;
}

void LightFlashTransformer::stop() {
  // Restore the original values after the flash.
  if (this->transformer_ != nullptr) {
    this->transformer_->stop();
    this->transformer_ = nullptr;
  }
  this->state_.current_values = this->get_start_values();
  this->state_.remote_values = this->get_start_values();
  this->state_.publish_state();
}

bool LightFlashTransformer::is_finished() {
  return this->begun_lightstate_restore_ && FiniteTransformer::is_finished();
}

}  // namespace light
}  // namespace esphome
