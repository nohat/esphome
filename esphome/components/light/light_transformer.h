#pragma once

#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "light_color_values.h"

namespace esphome {
namespace light {

/// Base class for all light color transformers, such as transitions or flashes.
class LightTransformer {
 public:
  virtual ~LightTransformer() = default;

  /// This will be called before the transition is started.
  virtual void start() {}

  /// This will be called while the transformer is active to apply the transition to the light. Can either write to the
  /// light directly, or return LightColorValues that will be applied.
  virtual optional<LightColorValues> apply() = 0;

  /// Indicates whether this transformation is finished.
  virtual bool is_finished() = 0;

  /// This will be called after transition is finished.
  virtual void stop() {}

  virtual const LightColorValues &get_start_values() const = 0;
  virtual const LightColorValues &get_target_values() const = 0;

  /// Returns true if this is a continuous transformer (for state publishing)
  virtual bool is_continuous() const { return false; }

 protected:
  uint32_t start_time_;
};

/// Base class for finite-duration transformers like transitions and flashes.
class FiniteTransformer : public LightTransformer {
 public:
  void setup(const LightColorValues &start_values, const LightColorValues &target_values, uint32_t length) {
    this->start_time_ = millis();
    this->length_ = length;
    this->start_values_ = start_values;
    this->target_values_ = target_values;
    this->start();
  }

  bool is_finished() override { return this->get_progress_() >= 1.0f; }

  const LightColorValues &get_start_values() const override { return this->start_values_; }
  const LightColorValues &get_target_values() const override { return this->target_values_; }

 protected:
  /// The progress of this transition, on a scale of 0 to 1.
  float get_progress_() {
    uint32_t now = esphome::millis();
    if (now < this->start_time_)
      return 0.0f;
    if (now >= this->start_time_ + this->length_)
      return 1.0f;

    return clamp((now - this->start_time_) / float(this->length_), 0.0f, 1.0f);
  }

  uint32_t length_;
  LightColorValues start_values_;
  LightColorValues target_values_;
};

/// Base class for continuous transformers like dynamic control.
class ContinuousTransformer : public LightTransformer {
 public:
  void setup(const LightColorValues &start_values) {
    this->start_time_ = millis();
    this->start_values_ = start_values;
    this->finished_ = false;
    this->start();
  }

  bool is_finished() override { return this->finished_; }

  const LightColorValues &get_start_values() const override { return this->start_values_; }
  const LightColorValues &get_target_values() const override { return this->start_values_; }

  bool is_continuous() const override { return true; }

 protected:
  /// Get elapsed time in milliseconds since start
  uint32_t get_elapsed_time_() {
    uint32_t now = esphome::millis();
    if (now < this->start_time_)
      return 0;
    return now - this->start_time_;
  }

  /// Set this to true to indicate the transformer should stop
  void set_finished(bool finished) { this->finished_ = finished; }

  LightColorValues start_values_;
  bool finished_{false};
};

}  // namespace light
}  // namespace esphome
