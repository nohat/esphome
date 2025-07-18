#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/helpers.h"
#include "light_state.h"
#include "addressable_light.h"

namespace esphome {
namespace light {

enum class LimitMode { CLAMP, DO_NOTHING };

template<typename... Ts> class ToggleAction : public Action<Ts...> {
 public:
  explicit ToggleAction(LightState *state) : state_(state) {}

  TEMPLATABLE_VALUE(uint32_t, transition_length)

  void play(Ts... x) override {
    auto call = this->state_->toggle();
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }

 protected:
  LightState *state_;
};

template<typename... Ts> class LightControlAction : public Action<Ts...> {
 public:
  explicit LightControlAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(ColorMode, color_mode)
  TEMPLATABLE_VALUE(bool, state)
  TEMPLATABLE_VALUE(uint32_t, transition_length)
  TEMPLATABLE_VALUE(uint32_t, flash_length)
  TEMPLATABLE_VALUE(float, brightness)
  TEMPLATABLE_VALUE(float, color_brightness)
  TEMPLATABLE_VALUE(float, red)
  TEMPLATABLE_VALUE(float, green)
  TEMPLATABLE_VALUE(float, blue)
  TEMPLATABLE_VALUE(float, white)
  TEMPLATABLE_VALUE(float, color_temperature)
  TEMPLATABLE_VALUE(float, cold_white)
  TEMPLATABLE_VALUE(float, warm_white)
  TEMPLATABLE_VALUE(std::string, effect)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    call.set_color_mode(this->color_mode_.optional_value(x...));
    call.set_state(this->state_.optional_value(x...));
    call.set_brightness(this->brightness_.optional_value(x...));
    call.set_color_brightness(this->color_brightness_.optional_value(x...));
    call.set_red(this->red_.optional_value(x...));
    call.set_green(this->green_.optional_value(x...));
    call.set_blue(this->blue_.optional_value(x...));
    call.set_white(this->white_.optional_value(x...));
    call.set_color_temperature(this->color_temperature_.optional_value(x...));
    call.set_cold_white(this->cold_white_.optional_value(x...));
    call.set_warm_white(this->warm_white_.optional_value(x...));
    call.set_effect(this->effect_.optional_value(x...));
    call.set_flash_length(this->flash_length_.optional_value(x...));
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }

 protected:
  LightState *parent_;
};

template<typename... Ts> class DimRelativeAction : public Action<Ts...> {
 public:
  explicit DimRelativeAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(float, relative_brightness)
  TEMPLATABLE_VALUE(uint32_t, transition_length)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    float rel = this->relative_brightness_.value(x...);
    float cur;
    this->parent_->remote_values.as_brightness(&cur);
    if ((limit_mode_ == LimitMode::DO_NOTHING) && ((cur < min_brightness_) || (cur > max_brightness_))) {
      return;
    }
    float new_brightness = clamp(cur + rel, min_brightness_, max_brightness_);
    call.set_state(new_brightness != 0.0f);
    call.set_brightness(new_brightness);

    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }

  void set_min_max_brightness(float min, float max) {
    this->min_brightness_ = min;
    this->max_brightness_ = max;
  }

  void set_limit_mode(LimitMode limit_mode) { this->limit_mode_ = limit_mode; }

 protected:
  LightState *parent_;
  float min_brightness_{0.0};
  float max_brightness_{1.0};
  LimitMode limit_mode_{LimitMode::CLAMP};
};

template<typename... Ts> class LightIsOnCondition : public Condition<Ts...> {
 public:
  explicit LightIsOnCondition(LightState *state) : state_(state) {}
  bool check(Ts... x) override { return this->state_->current_values.is_on(); }

 protected:
  LightState *state_;
};
template<typename... Ts> class LightIsOffCondition : public Condition<Ts...> {
 public:
  explicit LightIsOffCondition(LightState *state) : state_(state) {}
  bool check(Ts... x) override { return !this->state_->current_values.is_on(); }

 protected:
  LightState *state_;
};

class LightTurnOnTrigger : public Trigger<> {
 public:
  LightTurnOnTrigger(LightState *a_light) {
    a_light->add_new_remote_values_callback([this, a_light]() {
      // using the remote value because of transitions we need to trigger as early as possible
      auto is_on = a_light->remote_values.is_on();
      // only trigger when going from off to on
      auto should_trigger = is_on && !this->last_on_;
      // Set new state immediately so that trigger() doesn't devolve
      // into infinite loop
      this->last_on_ = is_on;
      if (should_trigger) {
        this->trigger();
      }
    });
    this->last_on_ = a_light->current_values.is_on();
  }

 protected:
  bool last_on_;
};

class LightTurnOffTrigger : public Trigger<> {
 public:
  LightTurnOffTrigger(LightState *a_light) {
    a_light->add_new_target_state_reached_callback([this, a_light]() {
      auto is_on = a_light->current_values.is_on();
      // only trigger when going from on to off
      if (!is_on) {
        this->trigger();
      }
    });
  }
};

class LightStateTrigger : public Trigger<> {
 public:
  LightStateTrigger(LightState *a_light) {
    a_light->add_new_remote_values_callback([this]() { this->trigger(); });
  }
};

// This is slightly ugly, but we can't log in headers, and can't make this a static method on AddressableSet
// due to the template. It's just a temporary warning anyway.
void addressableset_warn_about_scale(const char *field);

template<typename... Ts> class AddressableSet : public Action<Ts...> {
 public:
  explicit AddressableSet(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(int32_t, range_from)
  TEMPLATABLE_VALUE(int32_t, range_to)
  TEMPLATABLE_VALUE(float, color_brightness)
  TEMPLATABLE_VALUE(float, red)
  TEMPLATABLE_VALUE(float, green)
  TEMPLATABLE_VALUE(float, blue)
  TEMPLATABLE_VALUE(float, white)

  void play(Ts... x) override {
    auto *out = (AddressableLight *) this->parent_->get_output();
    int32_t range_from = interpret_index(this->range_from_.value_or(x..., 0), out->size());
    if (range_from < 0 || range_from >= out->size())
      range_from = 0;

    int32_t range_to = interpret_index(this->range_to_.value_or(x..., out->size() - 1) + 1, out->size());
    if (range_to < 0 || range_to >= out->size())
      range_to = out->size();

    uint8_t color_brightness =
        to_uint8_scale(this->color_brightness_.value_or(x..., this->parent_->remote_values.get_color_brightness()));
    auto range = out->range(range_from, range_to);
    if (this->red_.has_value())
      range.set_red(esp_scale8(to_uint8_compat(this->red_.value(x...), "red"), color_brightness));
    if (this->green_.has_value())
      range.set_green(esp_scale8(to_uint8_compat(this->green_.value(x...), "green"), color_brightness));
    if (this->blue_.has_value())
      range.set_blue(esp_scale8(to_uint8_compat(this->blue_.value(x...), "blue"), color_brightness));
    if (this->white_.has_value())
      range.set_white(to_uint8_compat(this->white_.value(x...), "white"));
    out->schedule_show();
  }

 protected:
  LightState *parent_;

  // Historically, this action required uint8_t (0-255) for RGBW values from lambdas. Keep compatibility.
  static inline uint8_t to_uint8_compat(float value, const char *field) {
    if (value > 1.0f) {
      addressableset_warn_about_scale(field);
      return static_cast<uint8_t>(value);
    }
    return to_uint8_scale(value);
  }
};

// Matter Level Control Cluster Actions
template<typename... Ts> class MoveToLevelAction : public Action<Ts...> {
 public:
  explicit MoveToLevelAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(float, level)
  TEMPLATABLE_VALUE(uint32_t, transition_length)
  TEMPLATABLE_VALUE(bool, with_on_off)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    float target_level = this->level_.value(x...);
    bool with_on_off = this->with_on_off_.value_or(x..., true);
    
    // Clamp level to valid range [0.0, 1.0]
    target_level = clamp(target_level, 0.0f, 1.0f);
    
    if (with_on_off) {
      call.set_state(target_level > 0.0f);
    }
    call.set_brightness(target_level);
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }

 protected:
  LightState *parent_;
};

template<typename... Ts> class MoveAction : public Action<Ts...> {
 public:
  explicit MoveAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint8_t, move_mode)  // 0 = up, 1 = down
  TEMPLATABLE_VALUE(float, rate)         // rate per second

  void play(Ts... x) override {
    uint8_t mode = this->move_mode_.value(x...);
    float rate = this->rate_.value(x...);
    
    // Store movement parameters for continuous operation
    this->parent_->set_move_rate(mode == 0 ? rate : -rate);
  }

 protected:
  LightState *parent_;
};

template<typename... Ts> class StepAction : public Action<Ts...> {
 public:
  explicit StepAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint8_t, step_mode)  // 0 = up, 1 = down
  TEMPLATABLE_VALUE(float, step_size)    // step amount
  TEMPLATABLE_VALUE(uint32_t, transition_length)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    uint8_t mode = this->step_mode_.value(x...);
    float step = this->step_size_.value(x...);
    
    float current_brightness;
    this->parent_->remote_values.as_brightness(&current_brightness);
    
    float new_brightness = current_brightness + (mode == 0 ? step : -step);
    new_brightness = clamp(new_brightness, 0.0f, 1.0f);
    
    call.set_brightness(new_brightness);
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }

 protected:
  LightState *parent_;
};

template<typename... Ts> class StopLevelAction : public Action<Ts...> {
 public:
  explicit StopLevelAction(LightState *parent) : parent_(parent) {}

  void play(Ts... x) override {
    // Stop any ongoing level movement
    this->parent_->stop_move();
  }

 protected:
  LightState *parent_;
};

// Matter Color Control Cluster Actions
template<typename... Ts> class MoveToHueAction : public Action<Ts...> {
 public:
  explicit MoveToHueAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(float, hue)          // target hue [0.0, 360.0]
  TEMPLATABLE_VALUE(uint8_t, direction)  // 0=shortest, 1=longest, 2=up, 3=down
  TEMPLATABLE_VALUE(uint32_t, transition_length)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    float target_hue = this->hue_.value(x...);
    uint8_t dir = this->direction_.value_or(x..., 0);
    
    // Convert hue to RGB using current saturation and brightness
    float current_h, current_s, current_v;
    this->parent_->remote_values.as_hsv(&current_h, &current_s, &current_v);
    
    // Handle hue direction logic
    if (dir == 1) {  // longest path
      if (abs(target_hue - current_h) < 180) {
        target_hue = target_hue > current_h ? target_hue - 360 : target_hue + 360;
      }
    } else if (dir == 2) {  // up
      if (target_hue < current_h) target_hue += 360;
    } else if (dir == 3) {  // down  
      if (target_hue > current_h) target_hue -= 360;
    }
    // dir == 0 (shortest) is handled automatically
    
    float r, g, b;
    int hue_int = static_cast<int>(target_hue);
    hsv_to_rgb(hue_int, current_s, current_v, r, g, b);
    call.set_rgb(r, g, b);
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }
};

template<typename... Ts> class MoveHueAction : public Action<Ts...> {
 public:
  explicit MoveHueAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint8_t, move_mode)  // 0=stop, 1=up, 3=down  
  TEMPLATABLE_VALUE(float, rate)         // degrees per second

  void play(Ts... x) override {
    uint8_t mode = this->move_mode_.value(x...);
    float rate = this->rate_.value(x...);
    
    if (mode == 0) {
      this->parent_->stop_hue_move();
    } else {
      float move_rate = (mode == 1) ? rate : -rate;
      this->parent_->set_hue_move_rate(move_rate);
    }
  }

 protected:
  LightState *parent_;
};

template<typename... Ts> class StepHueAction : public Action<Ts...> {
 public:
  explicit StepHueAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint8_t, step_mode)  // 1=up, 3=down
  TEMPLATABLE_VALUE(float, step_size)    // degrees
  TEMPLATABLE_VALUE(uint32_t, transition_length)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    uint8_t mode = this->step_mode_.value(x...);
    float step = this->step_size_.value(x...);
    
    float current_h, current_s, current_v;
    this->parent_->remote_values.as_hsv(&current_h, &current_s, &current_v);
    
    float new_hue = current_h + (mode == 1 ? step : -step);
    new_hue = fmod(new_hue, 360.0f);
    if (new_hue < 0) new_hue += 360.0f;
    
    float r, g, b;
    int hue_int = static_cast<int>(new_hue);
    hsv_to_rgb(hue_int, current_s, current_v, r, g, b);
    call.set_rgb(r, g, b);
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }
};

template<typename... Ts> class MoveToSaturationAction : public Action<Ts...> {
 public:
  explicit MoveToSaturationAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(float, saturation)  // target saturation [0.0, 1.0]
  TEMPLATABLE_VALUE(uint32_t, transition_length)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    float target_sat = clamp(this->saturation_.value(x...), 0.0f, 1.0f);
    
    float current_h, current_s, current_v;
    this->parent_->remote_values.as_hsv(&current_h, &current_s, &current_v);
    
    float r, g, b;
    int hue_int = static_cast<int>(current_h);
    hsv_to_rgb(hue_int, target_sat, current_v, r, g, b);
    call.set_rgb(r, g, b);
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }
};

template<typename... Ts> class MoveSaturationAction : public Action<Ts...> {
 public:
  explicit MoveSaturationAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint8_t, move_mode)  // 0=stop, 1=up, 3=down
  TEMPLATABLE_VALUE(float, rate)         // rate per second

  void play(Ts... x) override {
    uint8_t mode = this->move_mode_.value(x...);
    float rate = this->rate_.value(x...);
    
    if (mode == 0) {
      this->parent_->stop_saturation_move();
    } else {
      float move_rate = (mode == 1) ? rate : -rate;
      this->parent_->set_saturation_move_rate(move_rate);
    }
  }

 protected:
  LightState *parent_;
};

template<typename... Ts> class StepSaturationAction : public Action<Ts...> {
 public:
  explicit StepSaturationAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint8_t, step_mode)  // 1=up, 3=down
  TEMPLATABLE_VALUE(float, step_size)    // step amount [0.0, 1.0]
  TEMPLATABLE_VALUE(uint32_t, transition_length)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    uint8_t mode = this->step_mode_.value(x...);
    float step = this->step_size_.value(x...);
    
    float current_h, current_s, current_v;
    this->parent_->remote_values.as_hsv(&current_h, &current_s, &current_v);
    
    float new_sat = current_s + (mode == 1 ? step : -step);
    new_sat = clamp(new_sat, 0.0f, 1.0f);
    
    float r, g, b;
    int hue_int = static_cast<int>(current_h);
    hsv_to_rgb(hue_int, new_sat, current_v, r, g, b);
    call.set_rgb(r, g, b);
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }
};

template<typename... Ts> class MoveToHueAndSaturationAction : public Action<Ts...> {
 public:
  explicit MoveToHueAndSaturationAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(float, hue)         // target hue [0.0, 360.0]
  TEMPLATABLE_VALUE(float, saturation)  // target saturation [0.0, 1.0]
  TEMPLATABLE_VALUE(uint32_t, transition_length)

  void play(Ts... x) override {
    auto call = this->parent_->make_call();
    float target_hue = this->hue_.value(x...);
    float target_sat = clamp(this->saturation_.value(x...), 0.0f, 1.0f);
    
    float current_h, current_s, current_v;
    this->parent_->remote_values.as_hsv(&current_h, &current_s, &current_v);
    
    float r, g, b;
    int hue_int = static_cast<int>(target_hue);
    hsv_to_rgb(hue_int, target_sat, current_v, r, g, b);
    call.set_rgb(r, g, b);
    call.set_transition_length(this->transition_length_.optional_value(x...));
    call.perform();
  }
};

template<typename... Ts> class ColorLoopSetAction : public Action<Ts...> {
 public:
  explicit ColorLoopSetAction(LightState *parent) : parent_(parent) {}

  TEMPLATABLE_VALUE(uint8_t, action)    // 0=deactivate, 1=activate, 2=activate_from_hue
  TEMPLATABLE_VALUE(uint8_t, direction) // 0=decrement, 1=increment  
  TEMPLATABLE_VALUE(uint16_t, time)     // time for one loop in seconds
  TEMPLATABLE_VALUE(float, start_hue)   // starting hue for activate_from_hue

  void play(Ts... x) override {
    uint8_t act = this->action_.value(x...);
    uint8_t dir = this->direction_.value_or(x..., 1);
    uint16_t loop_time = this->time_.value_or(x..., 25);
    float start_hue = this->start_hue_.value_or(x..., 0.0f);
    
    if (act == 0) {
      this->parent_->stop_color_loop();
    } else {
      float hue = (act == 2) ? start_hue : 0.0f;
      this->parent_->start_color_loop(hue, dir == 1, loop_time);
    }
  }

 protected:
  LightState *parent_;
};

template<typename... Ts> class StopMoveStepAction : public Action<Ts...> {
 public:
  explicit StopMoveStepAction(LightState *parent) : parent_(parent) {}

  void play(Ts... x) override {
    // Stop all ongoing color movements
    this->parent_->stop_hue_move();
    this->parent_->stop_saturation_move();
    this->parent_->stop_color_loop();
  }

 protected:
  LightState *parent_;
};

}  // namespace light
}  // namespace esphome
