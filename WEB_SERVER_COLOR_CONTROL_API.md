# Web Server Color Control Cluster API Extensions

This document describes the lightweight web server API extensions that expose ESPHome's Color Control Cluster functionality.

## Overview

The web server API has been extended with Color Control Cluster parameters and endpoints to provide full Zigbee-equivalent control through HTTP requests, while maintaining minimal memory overhead.

## Extended Parameters for `/light/<id>/turn_on`

### Direct HSV Control
- `hue` (0-360) - Set hue in degrees
- `saturation` (0-100) - Set saturation as percentage

### Continuous Transitions (Move)
- `brightness_move_direction` + `brightness_move_speed` - Start continuous brightness transition
- `hue_move_direction` + `hue_move_speed` - Start continuous hue transition
- `saturation_move_direction` + `saturation_move_speed` - Start continuous saturation transition
- `color_temp_move_direction` + `color_temp_move_speed` - Start continuous color temperature transition

Direction values: `up`/`UP` or `down`/`DOWN`

### Examples

```bash
# Set hue to 120° (green) and saturation to 80%
curl "http://esp-device/light/my_light/turn_on?hue=120&saturation=80"

# Start continuous hue rotation upward at 45°/sec
curl "http://esp-device/light/my_light/turn_on?hue_move_direction=up&hue_move_speed=45"

# Start dimming down at 0.5 brightness units/sec  
curl "http://esp-device/light/my_light/turn_on?brightness_move_direction=down&brightness_move_speed=0.5"
```

## New Dynamic Control Endpoints

### POST `/light/<id>/move`
Start continuous transitions for any parameter.

**Parameters:**
- `parameter` - Target parameter (`brightness`, `hue`, `saturation`, `color_temp`)
- `direction` - Direction (`up` or `down`)
- `speed` - Transition speed (units per second)

**Example:**
```bash
curl -X POST "http://esp-device/light/my_light/move?parameter=hue&direction=up&speed=60"
```

### POST `/light/<id>/step`
Perform step transitions for any parameter.

**Parameters:**
- `parameter` - Target parameter (`brightness`, `hue`, `saturation`, `color_temp`)
- `direction` - Direction (`up` or `down`)
- `step_size` - Size of the step
- `transition_time` (optional) - Transition duration in milliseconds

**Example:**
```bash
curl -X POST "http://esp-device/light/my_light/step?parameter=brightness&direction=up&step_size=0.1&transition_time=1000"
```

### POST `/light/<id>/move_to_level`
Move to specific target levels.

**Parameters:**
- `parameter` - Target parameter (`brightness`, `hue`, `saturation`, `color_temp`)
- `target` - Target value
- `transition_time` (optional) - Transition duration in milliseconds

**Example:**
```bash
curl -X POST "http://esp-device/light/my_light/move_to_level?parameter=hue&target=240&transition_time=2000"
```

### POST `/light/<id>/stop`
Stop any active continuous transitions.

**Example:**
```bash
curl -X POST "http://esp-device/light/my_light/stop"
```

## Extended JSON Response

The light state JSON now includes additional Color Control information:

### New Fields
- `hue` - Current hue value (0-360°)
- `saturation` - Current saturation (0-100%)
- `transition_active` - Boolean indicating if continuous transition is active

### Example Response
```json
{
  "id": "my_light",
  "state": "ON",
  "brightness": 200,
  "hue": 120.5,
  "saturation": 75,
  "transition_active": true,
  "color": {
    "r": 51,
    "g": 200,
    "b": 51
  },
  "color_temp": 300
}
```

## Parameter Conversion

- **Hue**: 0-360° (same as Zigbee)
- **Saturation**: 0-100% in API, converted to 0-1.0 internally
- **Brightness**: 0-100% for move_to_level, 0-255 for legacy brightness parameter
- **Color Temperature**: Mireds (same as Zigbee)

## Compatibility

- Fully backward compatible with existing web server API
- All existing parameters and endpoints continue to work unchanged
- New functionality requires Color Control Cluster implementation in ESPHome

## Error Responses

- **400 Bad Request**: Invalid parameters or missing required parameters
- **404 Not Found**: Light not found or invalid endpoint
- **200 OK**: Success

## Implementation Notes

- Uses existing parameter parsing patterns for consistency
- Leverages new LightCall Color Control methods
- Maintains minimal memory footprint
- No changes to core web server architecture
- Thread-safe scheduling via existing ESPHome scheduler