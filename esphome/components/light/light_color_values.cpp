#include "light_color_values.h"
#include "esphome/core/log.h"

namespace esphome {
namespace light {

static const char *const TAG = "light.color_values";

void LightColorValues::update_brightness(float delta) {
  float current = this->get_brightness();
  float new_value = clamp_value(current + delta, 0.0f, 1.0f);
  this->set_brightness(new_value);

  // Turn on/off light based on brightness
  if (new_value > 0.0f && !this->is_on()) {
    this->set_state(true);
  } else if (new_value == 0.0f && this->is_on()) {
    this->set_state(false);
  }
}

void LightColorValues::update_color_temperature(float delta) {
  if (this->get_color_mode() & ColorCapability::COLOR_TEMPERATURE) {
    float current = this->get_color_temperature();
    float new_value = clamp_value(current + delta, 153.0f, 500.0f);  // Mireds range (2000K to 6536K)
    this->set_color_temperature(new_value);
  }
}

void LightColorValues::update_hue(float delta, HueTransitionPath path) {
  if (this->get_color_mode() & ColorCapability::RGB) {
    float current = this->get_hue();
    float new_value = wrap_hue(current + delta);
    this->set_hue(new_value);
  }
}

void LightColorValues::update_saturation(float delta) {
  if (this->get_color_mode() & ColorCapability::RGB) {
    float current = this->get_saturation();
    float new_value = clamp_value(current + delta, 0.0f, 1.0f);
    this->set_saturation(new_value);
  }
}

void LightColorValues::update_cie_x(float delta) {
  if (this->get_color_mode() & ColorCapability::RGB) {
    // Convert current RGB to CIE XY
    float r, g, b;
    this->as_rgb(&r, &g, &b);

    // Simple sRGB to XYZ conversion (for demonstration - could use more precise conversion)
    float X = 0.4124f * r + 0.3576f * g + 0.1805f * b;
    float Y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
    float Z = 0.0193f * r + 0.1192f * g + 0.9505f * b;

    float sum = X + Y + Z;
    if (sum > 0.0f) {
      float current_x = X / sum;
      float new_x = clamp_value(current_x + delta, 0.0f, 1.0f);

      // Convert back to RGB (simplified)
      float y = (sum > 0.0f) ? Y / sum : 0.0f;

      // This is a simplified conversion - in practice you'd want more sophisticated CIE->RGB conversion
      float ratio = new_x / (current_x > 0.0f ? current_x : 0.001f);
      this->set_rgb(r * ratio, g * ratio, b * ratio);
    }
  }
}

void LightColorValues::update_cie_y(float delta) {
  if (this->get_color_mode() & ColorCapability::RGB) {
    // Convert current RGB to CIE XY
    float r, g, b;
    this->as_rgb(&r, &g, &b);

    // Simple sRGB to XYZ conversion
    float X = 0.4124f * r + 0.3576f * g + 0.1805f * b;
    float Y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
    float Z = 0.0193f * r + 0.1192f * g + 0.9505f * b;

    float sum = X + Y + Z;
    if (sum > 0.0f) {
      float current_y = Y / sum;
      float new_y = clamp_value(current_y + delta, 0.0f, 1.0f);

      // Convert back to RGB (simplified)
      float x = (sum > 0.0f) ? X / sum : 0.0f;

      // This is a simplified conversion - in practice you'd want more sophisticated CIE->RGB conversion
      float ratio = new_y / (current_y > 0.0f ? current_y : 0.001f);
      this->set_rgb(r * ratio, g * ratio, b * ratio);
    }
  }
}

float LightColorValues::get_transition_value(ColorTransitionType type) const {
  switch (type) {
    case COLOR_TRANSITION_BRIGHTNESS:
      return this->get_brightness();
    case COLOR_TRANSITION_COLOR_TEMPERATURE:
      return this->get_color_temperature();
    case COLOR_TRANSITION_HUE:
      return this->get_hue();
    case COLOR_TRANSITION_SATURATION:
      return this->get_saturation();
    case COLOR_TRANSITION_CIE_X: {
      float r, g, b;
      this->as_rgb(&r, &g, &b);
      float X = 0.4124f * r + 0.3576f * g + 0.1805f * b;
      float Y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
      float Z = 0.0193f * r + 0.1192f * g + 0.9505f * b;
      float sum = X + Y + Z;
      return (sum > 0.0f) ? (X / sum) : 0.0f;
    }
    case COLOR_TRANSITION_CIE_Y: {
      float r, g, b;
      this->as_rgb(&r, &g, &b);
      float X = 0.4124f * r + 0.3576f * g + 0.1805f * b;
      float Y = 0.2126f * r + 0.7152f * g + 0.0722f * b;
      float Z = 0.0193f * r + 0.1192f * g + 0.9505f * b;
      float sum = X + Y + Z;
      return (sum > 0.0f) ? (Y / sum) : 0.0f;
    }
    default:
      return 0.0f;
  }
}

float LightColorValues::calculate_hue_distance(float from_hue, float to_hue, HueTransitionPath path) {
  float diff = to_hue - from_hue;

  switch (path) {
    case HUE_PATH_SHORTEST:
      // Choose the shortest path around the color wheel
      if (diff > 180.0f) {
        diff -= 360.0f;
      } else if (diff < -180.0f) {
        diff += 360.0f;
      }
      break;

    case HUE_PATH_LONGEST:
      // Choose the longest path around the color wheel
      if (diff > 0.0f && diff < 180.0f) {
        diff -= 360.0f;
      } else if (diff < 0.0f && diff > -180.0f) {
        diff += 360.0f;
      }
      break;

    case HUE_PATH_CLOCKWISE:
      // Force clockwise direction (positive)
      while (diff <= 0.0f)
        diff += 360.0f;
      break;

    case HUE_PATH_COUNTER_CLOCKWISE:
      // Force counter-clockwise direction (negative)
      while (diff >= 0.0f)
        diff -= 360.0f;
      break;
  }

  return diff;
}

LightColorValues LightColorValues::lerp_with_hue_path(const LightColorValues &start, const LightColorValues &end,
                                                      float completion, HueTransitionPath hue_path) {
  // Use the standard lerp for most values
  LightColorValues result = LightColorValues::lerp(start, end, completion);

  // Special handling for hue transitions if both values have RGB color mode
  if ((start.get_color_mode() & ColorCapability::RGB) && (end.get_color_mode() & ColorCapability::RGB)) {
    float start_hue = start.get_hue();
    float end_hue = end.get_hue();

    // Calculate the hue distance according to the specified path
    float hue_distance = calculate_hue_distance(start_hue, end_hue, hue_path);
    float interpolated_hue = wrap_hue(start_hue + hue_distance * completion);

    // Set the interpolated hue while keeping other HSV values as they were interpolated
    result.set_hue(interpolated_hue);
  }

  return result;
}

// ========== HSV METHODS IMPLEMENTATION ==========

float LightColorValues::get_hue() const {
  // Convert RGB to HSV and return hue
  float r = this->get_red();
  float g = this->get_green();
  float b = this->get_blue();

  float max_val = std::max({r, g, b});
  float min_val = std::min({r, g, b});
  float delta = max_val - min_val;

  if (delta == 0)
    return 0.0f;  // No hue when saturation is 0

  float hue;
  if (max_val == r) {
    hue = 60.0f * fmod((g - b) / delta + 6.0f, 6.0f);
  } else if (max_val == g) {
    hue = 60.0f * ((b - r) / delta + 2.0f);
  } else {
    hue = 60.0f * ((r - g) / delta + 4.0f);
  }

  return hue;
}

void LightColorValues::set_hue(float hue) {
  // Get current saturation and value
  float saturation = this->get_saturation();
  float value = std::max({this->get_red(), this->get_green(), this->get_blue()});

  // Convert HSV to RGB
  hue = wrap_hue(hue);  // Ensure hue is in 0-360 range

  float c = value * saturation;
  float x = c * (1.0f - std::abs(fmod(hue / 60.0f, 2.0f) - 1.0f));
  float m = value - c;

  float r, g, b;
  if (hue >= 0 && hue < 60) {
    r = c;
    g = x;
    b = 0;
  } else if (hue >= 60 && hue < 120) {
    r = x;
    g = c;
    b = 0;
  } else if (hue >= 120 && hue < 180) {
    r = 0;
    g = c;
    b = x;
  } else if (hue >= 180 && hue < 240) {
    r = 0;
    g = x;
    b = c;
  } else if (hue >= 240 && hue < 300) {
    r = x;
    g = 0;
    b = c;
  } else {
    r = c;
    g = 0;
    b = x;
  }

  this->set_red(r + m);
  this->set_green(g + m);
  this->set_blue(b + m);
}

float LightColorValues::get_saturation() const {
  // Convert RGB to HSV and return saturation
  float r = this->get_red();
  float g = this->get_green();
  float b = this->get_blue();

  float max_val = std::max({r, g, b});
  float min_val = std::min({r, g, b});

  if (max_val == 0)
    return 0.0f;  // No saturation when value is 0

  return (max_val - min_val) / max_val;
}

void LightColorValues::set_saturation(float saturation) {
  // Get current hue and value
  float hue = this->get_hue();
  float value = std::max({this->get_red(), this->get_green(), this->get_blue()});

  // Convert HSV to RGB
  saturation = clamp(saturation, 0.0f, 1.0f);

  float c = value * saturation;
  float x = c * (1.0f - std::abs(fmod(hue / 60.0f, 2.0f) - 1.0f));
  float m = value - c;

  float r, g, b;
  if (hue >= 0 && hue < 60) {
    r = c;
    g = x;
    b = 0;
  } else if (hue >= 60 && hue < 120) {
    r = x;
    g = c;
    b = 0;
  } else if (hue >= 120 && hue < 180) {
    r = 0;
    g = c;
    b = x;
  } else if (hue >= 180 && hue < 240) {
    r = 0;
    g = x;
    b = c;
  } else if (hue >= 240 && hue < 300) {
    r = x;
    g = 0;
    b = c;
  } else {
    r = c;
    g = 0;
    b = x;
  }

  this->set_red(r + m);
  this->set_green(g + m);
  this->set_blue(b + m);
}

void LightColorValues::as_cie_xy(float *x, float *y) const {
  // Simple RGB to CIE XY conversion (basic implementation)
  float r = this->get_red();
  float g = this->get_green();
  float b = this->get_blue();

  // Convert to XYZ color space (simplified sRGB conversion)
  float X = r * 0.4124564f + g * 0.3575761f + b * 0.1804375f;
  float Y = r * 0.2126729f + g * 0.7151522f + b * 0.0721750f;
  float Z = r * 0.0193339f + g * 0.1191920f + b * 0.9503041f;

  float sum = X + Y + Z;
  if (sum == 0) {
    *x = 0.3127f;  // Default to white point
    *y = 0.3290f;
  } else {
    *x = X / sum;
    *y = Y / sum;
  }
}

void LightColorValues::set_cie_xy(float x, float y) {
  // Simple CIE XY to RGB conversion (basic implementation)
  // Use Y = 1 for maximum brightness
  float Y = 1.0f;
  float X = (Y / y) * x;
  float Z = (Y / y) * (1.0f - x - y);

  // Convert XYZ to RGB (simplified sRGB conversion)
  float r = X * 3.2404542f + Y * -1.5371385f + Z * -0.4985314f;
  float g = X * -0.9692660f + Y * 1.8760108f + Z * 0.0415560f;
  float b = X * 0.0556434f + Y * -0.2040259f + Z * 1.0572252f;

  // Clamp and set RGB values
  this->set_red(clamp(r, 0.0f, 1.0f));
  this->set_green(clamp(g, 0.0f, 1.0f));
  this->set_blue(clamp(b, 0.0f, 1.0f));
}

}  // namespace light
}  // namespace esphome
