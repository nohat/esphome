#pragma once

// Include all transformer implementations
#include "light_transition_transformer.h"
#include "light_flash_transformer.h"
#include "color_transition_transformer.h"
#include "light_color_values.h"

namespace esphome {
namespace light {

// Re-export all transformer classes for backwards compatibility
using LightTransitionTransformer = LightTransitionTransformer;
using LightFlashTransformer = LightFlashTransformer;
using ColorTransitionTransformer = ColorTransitionTransformer;

// Legacy aliases for backwards compatibility (will be deprecated)
using DynamicControlTransformer = ColorTransitionTransformer;
using LightContinuousTransformer = ColorTransitionTransformer;

// Legacy enum aliases
constexpr auto DYNAMIC_CONTROL_NONE = CONTINUOUS_NONE;
constexpr auto DYNAMIC_CONTROL_BRIGHTNESS = CONTINUOUS_BRIGHTNESS;
constexpr auto DYNAMIC_CONTROL_COLOR_TEMPERATURE = CONTINUOUS_COLOR_TEMPERATURE;
constexpr auto DYNAMIC_CONTROL_HUE = CONTINUOUS_HUE;
constexpr auto DYNAMIC_CONTROL_SATURATION = CONTINUOUS_SATURATION;
constexpr auto DYNAMIC_CONTROL_CIE_X = CONTINUOUS_CIE_X;
constexpr auto DYNAMIC_CONTROL_CIE_Y = CONTINUOUS_CIE_Y;

constexpr auto DYNAMIC_CONTROL_DIRECTION_UP = TRANSITION_DIRECTION_UP;
constexpr auto DYNAMIC_CONTROL_DIRECTION_DOWN = TRANSITION_DIRECTION_DOWN;

}  // namespace light
}  // namespace esphome
