# Testing Color Control Web Server on Athom Bulb

## Step 1: Deploy the Code

First, you'll need to compile and upload the code to your Athom bulb with our Color Control extensions.

### Option A: Using ESPHome CLI
```bash
# In the esphome directory
esphome compile athom_bulb_color_control_test.yaml
esphome upload athom_bulb_color_control_test.yaml
```

### Option B: Using ESPHome Dashboard
1. Copy the `athom_bulb_color_control_test.yaml` to your ESPHome config directory
2. Compile and upload through the web interface

## Step 2: Find Your Bulb's IP

Once uploaded and connected to WiFi, find the bulb's IP address:
- Check your router's DHCP client list
- Or use ESPHome logs to see the assigned IP
- Or check the ESPHome dashboard

## Step 3: Manual Testing

### Basic Web Interface Test
Open `http://YOUR_BULB_IP` in a browser to see the updated web interface with new JSON fields.

### API Testing with curl

**Get current state (should show new hue/saturation fields):**
```bash
curl "http://YOUR_BULB_IP/light/athom_light"
```

**Test HSV control:**
```bash
# Red (hue=0, sat=100%)
curl "http://YOUR_BULB_IP/light/athom_light/turn_on?hue=0&saturation=100"

# Green (hue=120, sat=75%)  
curl "http://YOUR_BULB_IP/light/athom_light/turn_on?hue=120&saturation=75"

# Blue (hue=240, sat=50%)
curl "http://YOUR_BULB_IP/light/athom_light/turn_on?hue=240&saturation=50"
```

**Test continuous hue rotation:**
```bash
# Start rotating hue upward at 30°/second
curl "http://YOUR_BULB_IP/light/athom_light/turn_on?hue_move_direction=up&hue_move_speed=30"

# Watch the color change for a few seconds, then stop
curl -X POST "http://YOUR_BULB_IP/light/athom_light/stop"
```

**Test new dynamic endpoints:**
```bash
# Start brightness dimming down at 0.2 units/sec
curl -X POST "http://YOUR_BULB_IP/light/athom_light/move?parameter=brightness&direction=down&speed=0.2"

# Step hue by 60° over 1 second
curl -X POST "http://YOUR_BULB_IP/light/athom_light/step?parameter=hue&direction=up&step_size=60&transition_time=1000"

# Move to purple (hue=300°) over 2 seconds
curl -X POST "http://YOUR_BULB_IP/light/athom_light/move_to_level?parameter=hue&target=300&transition_time=2000"

# Stop any active transitions
curl -X POST "http://YOUR_BULB_IP/light/athom_light/stop"
```

## Step 4: Automated Testing

Run the Python test script:
```bash
python test_color_control_api.py YOUR_BULB_IP
```

## Expected Results

1. **JSON State Response** should include new fields:
   ```json
   {
     "hue": 120.5,
     "saturation": 75,
     "transition_active": false,
     ...
   }
   ```

2. **HSV Control** should show immediate color changes

3. **Continuous Transitions** should show:
   - `transition_active: true` in JSON
   - Smooth color/brightness changes
   - Proper stop functionality

4. **Dynamic Endpoints** should respond with 200 OK and perform the requested actions

## Troubleshooting

- **404 errors**: Check the light ID in the URL (should match your YAML)
- **400 errors**: Verify parameter names and values
- **No color changes**: Check if the light supports RGB mode
- **Compilation errors**: Ensure you're using our modified ESPHome code

## What to Look For

- Smooth hue transitions during continuous moves
- Accurate HSV values in JSON responses  
- Proper transition state reporting
- Immediate response to stop commands
- Compatibility with existing web interface