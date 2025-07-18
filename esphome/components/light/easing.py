import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_TYPE, CONF_POINTS

# Import from parent light module
from .types import light_ns

# Easing curve types
EASING_TYPES = {
    # Standard easing curves
    "LINEAR": "EasingType::LINEAR",
    "EASE_IN_QUAD": "EasingType::EASE_IN_QUAD",
    "EASE_OUT_QUAD": "EasingType::EASE_OUT_QUAD", 
    "EASE_IN_OUT_QUAD": "EasingType::EASE_IN_OUT_QUAD",
    "EASE_IN_CUBIC": "EasingType::EASE_IN_CUBIC",
    "EASE_OUT_CUBIC": "EasingType::EASE_OUT_CUBIC",
    "EASE_IN_OUT_CUBIC": "EasingType::EASE_IN_OUT_CUBIC",
    "EASE_IN_QUART": "EasingType::EASE_IN_QUART",
    "EASE_OUT_QUART": "EasingType::EASE_OUT_QUART",
    "EASE_IN_OUT_QUART": "EasingType::EASE_IN_OUT_QUART",
    "EASE_IN_QUINT": "EasingType::EASE_IN_QUINT",
    "EASE_OUT_QUINT": "EasingType::EASE_OUT_QUINT",
    "EASE_IN_OUT_QUINT": "EasingType::EASE_IN_OUT_QUINT",
    "EASE_IN_SINE": "EasingType::EASE_IN_SINE",
    "EASE_OUT_SINE": "EasingType::EASE_OUT_SINE",
    "EASE_IN_OUT_SINE": "EasingType::EASE_IN_OUT_SINE",
    "EASE_IN_EXPO": "EasingType::EASE_IN_EXPO",
    "EASE_OUT_EXPO": "EasingType::EASE_OUT_EXPO",
    "EASE_IN_OUT_EXPO": "EasingType::EASE_IN_OUT_EXPO",
    "EASE_IN_CIRC": "EasingType::EASE_IN_CIRC",
    "EASE_OUT_CIRC": "EasingType::EASE_OUT_CIRC",
    "EASE_IN_OUT_CIRC": "EasingType::EASE_IN_OUT_CIRC",
    "EASE_IN_BACK": "EasingType::EASE_IN_BACK",
    "EASE_OUT_BACK": "EasingType::EASE_OUT_BACK",
    "EASE_IN_OUT_BACK": "EasingType::EASE_IN_OUT_BACK",
    "EASE_IN_ELASTIC": "EasingType::EASE_IN_ELASTIC",
    "EASE_OUT_ELASTIC": "EasingType::EASE_OUT_ELASTIC",
    "EASE_IN_OUT_ELASTIC": "EasingType::EASE_IN_OUT_ELASTIC",
    "EASE_IN_BOUNCE": "EasingType::EASE_IN_BOUNCE",
    "EASE_OUT_BOUNCE": "EasingType::EASE_OUT_BOUNCE",
    "EASE_IN_OUT_BOUNCE": "EasingType::EASE_IN_OUT_BOUNCE",
    
    # Professional lighting curves
    "SMOOTH": "EasingType::SMOOTH",  # Current ESPHome default
    "THEATER_DIM": "EasingType::THEATER_DIM",
    "CANDLE_FLICKER": "EasingType::CANDLE_FLICKER", 
    "FADE_TO_BLACK": "EasingType::FADE_TO_BLACK",
    "FAST_DIM_SLOW": "EasingType::FAST_DIM_SLOW",
    
    # Custom curve
    "CUSTOM_POINTS": "EasingType::CUSTOM_POINTS",
}

# Constants for configuration
CONF_EASING = "easing"
CONF_X = "x"
CONF_Y = "y"

# Types from the light component
EasingType = light_ns.enum("EasingType")
EasingCurve = light_ns.class_("EasingCurve")
EasingPoint = light_ns.struct("EasingPoint")

def validate_easing_point(value):
    """Validate an easing curve point."""
    if isinstance(value, dict):
        value = cv.Schema({
            cv.Required(CONF_X): cv.float_range(0.0, 1.0),
            cv.Required(CONF_Y): cv.float_range(0.0, 1.0),
        })(value)
        return value
    return cv.Invalid("Easing point must be a dict with x and y values")

def validate_easing_points(value):
    """Validate a list of easing points."""
    if not isinstance(value, list):
        raise cv.Invalid("Easing points must be a list")
    
    if len(value) < 2:
        raise cv.Invalid("At least 2 points are required for custom easing")
    
    points = [validate_easing_point(point) for point in value]
    
    # Check that points are sorted by x value
    for i in range(1, len(points)):
        if points[i][CONF_X] <= points[i-1][CONF_X]:
            raise cv.Invalid("Easing points must be sorted by x value and x values must be unique")
    
    # First point should start at x=0 and last at x=1
    if points[0][CONF_X] != 0.0:
        raise cv.Invalid("First easing point must have x=0.0")
    if points[-1][CONF_X] != 1.0:
        raise cv.Invalid("Last easing point must have x=1.0")
    
    return points

def validate_easing_config(value):
    """Validate easing configuration."""
    if isinstance(value, str):
        # Simple string type
        if value.upper() not in EASING_TYPES:
            raise cv.Invalid(f"Unknown easing type: {value}")
        return {CONF_TYPE: value.upper()}
    
    if isinstance(value, dict):
        # Full configuration
        schema = cv.Schema({
            cv.Required(CONF_TYPE): cv.enum(EASING_TYPES, upper=True),
            cv.Optional(CONF_POINTS): validate_easing_points,
        })
        
        config = schema(value)
        
        # If type is CUSTOM_POINTS, points are required
        if config[CONF_TYPE] == "CUSTOM_POINTS" and CONF_POINTS not in config:
            raise cv.Invalid("Custom easing requires points configuration")
        
        # If type is not CUSTOM_POINTS, points should not be specified
        if config[CONF_TYPE] != "CUSTOM_POINTS" and CONF_POINTS in config:
            raise cv.Invalid(f"Points can only be specified for CUSTOM_POINTS easing type")
        
        return config
    
    raise cv.Invalid("Easing must be a string or dict")

# Schema for easing configuration
EASING_SCHEMA = cv.Any(
    cv.string,  # Simple string type
    cv.Schema({
        cv.Required(CONF_TYPE): cv.enum(EASING_TYPES, upper=True),
        cv.Optional(CONF_POINTS): validate_easing_points,
    })
)

async def easing_to_code(config):
    """Convert easing configuration to C++ code."""
    if isinstance(config, str):
        config = {CONF_TYPE: config.upper()}
    
    easing_type = EASING_TYPES[config[CONF_TYPE]]
    
    # Create the easing curve object
    curve_var = cg.new_Pvariable(EasingCurve, cg.RawExpression(easing_type))
    
    if CONF_POINTS in config:
        # Create a vector of points
        points = []
        for point in config[CONF_POINTS]:
            points.append(cg.RawExpression(f"esphome::light::EasingPoint({point[CONF_X]}f, {point[CONF_Y]}f)"))
        
        # Set the points
        cg.add(curve_var.set_points(cg.RawExpression(f"std::vector<esphome::light::EasingPoint>{{{', '.join(map(str, points))}}}")))
    
    return curve_var