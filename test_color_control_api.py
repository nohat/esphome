#!/usr/bin/env python3
"""
Test script for Color Control Cluster web server API
Usage: python test_color_control_api.py <bulb_ip>
"""

import json
import sys
import time

import requests


class ColorControlTester:
    def __init__(self, bulb_ip):
        self.base_url = f"http://{bulb_ip}"
        self.light_id = "athom_light"  # Adjust based on your light ID

    def get_light_state(self):
        """Get current light state"""
        try:
            response = requests.get(f"{self.base_url}/light/{self.light_id}")
            if response.status_code == 200:
                state = response.json()
                print(f"Current state: {json.dumps(state, indent=2)}")
                return state
            else:
                print(f"Failed to get state: {response.status_code}")
                return None
        except Exception as e:
            print(f"Error getting state: {e}")
            return None

    def test_basic_turn_on(self):
        """Test basic turn on with brightness"""
        print("\n=== Testing basic turn on ===")
        try:
            response = requests.post(
                f"{self.base_url}/light/{self.light_id}/turn_on?brightness=128"
            )
            print(f"Turn on response: {response.status_code}")
            time.sleep(1)
            return self.get_light_state()
        except Exception as e:
            print(f"Error in basic turn on: {e}")

    def test_hue_saturation(self):
        """Test direct hue and saturation control"""
        print("\n=== Testing HSV control ===")
        try:
            # Set to red (hue=0) with full saturation
            response = requests.post(
                f"{self.base_url}/light/{self.light_id}/turn_on?hue=0&saturation=100"
            )
            print(f"Red HSV response: {response.status_code}")
            time.sleep(2)
            self.get_light_state()

            # Set to green (hue=120) with 75% saturation
            response = requests.post(
                f"{self.base_url}/light/{self.light_id}/turn_on?hue=120&saturation=75"
            )
            print(f"Green HSV response: {response.status_code}")
            time.sleep(2)
            self.get_light_state()

            # Set to blue (hue=240) with 50% saturation
            response = requests.post(
                f"{self.base_url}/light/{self.light_id}/turn_on?hue=240&saturation=50"
            )
            print(f"Blue HSV response: {response.status_code}")
            time.sleep(2)
            return self.get_light_state()
        except Exception as e:
            print(f"Error in HSV test: {e}")

    def test_continuous_hue_move(self):
        """Test continuous hue transitions"""
        print("\n=== Testing continuous hue move ===")
        try:
            # Start hue rotation upward at 30°/sec
            response = requests.post(
                f"{self.base_url}/light/{self.light_id}/turn_on?hue_move_direction=up&hue_move_speed=30"
            )
            print(f"Hue move start response: {response.status_code}")

            # Let it run for 5 seconds
            for i in range(5):
                time.sleep(1)
                state = self.get_light_state()
                if state and "transition_active" in state:
                    print(f"Transition active: {state['transition_active']}")
                    if "hue" in state:
                        print(f"Current hue: {state['hue']}")

            # Stop the transition
            print("Stopping transition...")
            response = requests.post(f"{self.base_url}/light/{self.light_id}/stop")
            print(f"Stop response: {response.status_code}")
            time.sleep(1)
            return self.get_light_state()
        except Exception as e:
            print(f"Error in continuous hue test: {e}")

    def test_move_endpoint(self):
        """Test the new /move endpoint"""
        print("\n=== Testing /move endpoint ===")
        try:
            # Test brightness move down
            response = requests.post(
                f"{self.base_url}/light/{self.light_id}/move?parameter=brightness&direction=down&speed=0.2"
            )
            print(f"Brightness move response: {response.status_code}")

            # Watch for 3 seconds
            for i in range(3):
                time.sleep(1)
                state = self.get_light_state()
                if state and "brightness" in state:
                    print(f"Brightness: {state['brightness']}")

            # Stop it
            response = requests.post(f"{self.base_url}/light/{self.light_id}/stop")
            print(f"Stop response: {response.status_code}")
            return self.get_light_state()
        except Exception as e:
            print(f"Error in move endpoint test: {e}")

    def test_step_endpoint(self):
        """Test the new /step endpoint"""
        print("\n=== Testing /step endpoint ===")
        try:
            # Step hue by 60 degrees over 1 second
            response = requests.post(
                f"{self.base_url}/light/{self.light_id}/step?parameter=hue&direction=up&step_size=60&transition_time=1000"
            )
            print(f"Hue step response: {response.status_code}")
            time.sleep(2)
            return self.get_light_state()
        except Exception as e:
            print(f"Error in step endpoint test: {e}")

    def test_move_to_level_endpoint(self):
        """Test the new /move_to_level endpoint"""
        print("\n=== Testing /move_to_level endpoint ===")
        try:
            # Move to specific hue over 2 seconds
            response = requests.post(
                f"{self.base_url}/light/{self.light_id}/move_to_level?parameter=hue&target=300&transition_time=2000"
            )
            print(f"Move to level response: {response.status_code}")
            time.sleep(3)
            return self.get_light_state()
        except Exception as e:
            print(f"Error in move_to_level test: {e}")

    def run_all_tests(self):
        """Run all tests"""
        print(f"Testing Color Control API on {self.base_url}")

        # Initial state
        print("\n=== Initial State ===")
        self.get_light_state()

        # Run tests
        self.test_basic_turn_on()
        self.test_hue_saturation()
        self.test_continuous_hue_move()
        self.test_move_endpoint()
        self.test_step_endpoint()
        self.test_move_to_level_endpoint()

        print("\n=== All tests complete! ===")


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python test_color_control_api.py <bulb_ip>")
        print("Example: python test_color_control_api.py 192.168.1.100")
        sys.exit(1)

    bulb_ip = sys.argv[1]
    tester = ColorControlTester(bulb_ip)
    tester.run_all_tests()
