from esphome import automation
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import (
    CONF_BLUE,
    CONF_BRIGHTNESS,
    CONF_BRIGHTNESS_LIMITS,
    CONF_COLD_WHITE,
    CONF_COLOR_BRIGHTNESS,
    CONF_COLOR_MODE,
    CONF_COLOR_TEMPERATURE,
    CONF_DIRECTION,
    CONF_EFFECT,
    CONF_FLASH_LENGTH,
    CONF_GREEN,
    CONF_ID,
    CONF_LIMIT_MODE,
    CONF_MAX_BRIGHTNESS,
    CONF_MIN_BRIGHTNESS,
    CONF_RANGE_FROM,
    CONF_RANGE_TO,
    CONF_RED,
    CONF_SPEED,
    CONF_STATE,
    CONF_TRANSITION_LENGTH,
    CONF_WARM_WHITE,
    CONF_WHITE,
)

from .types import (
    COLOR_MODES,
    HUE_TRANSITION_PATHS,
    LIMIT_MODES,
    TRANSITION_DIRECTIONS,
    AddressableLightState,
    AddressableSet,
    ColorMode,
    DimRelativeAction,
    HueTransitionPath,
    LightControlAction,
    LightIsOffCondition,
    LightIsOnCondition,
    LightState,
    ToggleAction,
    TransitionDirection,
)

# Dynamic control configuration constants
CONF_DYNAMIC_CONTROL = "dynamic_control"
CONF_BRIGHTNESS_ACTION = "brightness_action"
CONF_HUE_ACTION = "hue_action"
CONF_SATURATION_ACTION = "saturation_action"
CONF_COLOR_TEMPERATURE_ACTION = "color_temperature_action"
CONF_STOP_ACTION = "stop_action"

# Color control parameter constants
CONF_HUE_PATH = "hue_path"
CONF_TARGET_HUE = "target_hue"
CONF_TARGET_SATURATION = "target_saturation"
CONF_TARGET_COLOR_TEMPERATURE = "target_color_temperature"
CONF_STEP_SIZE = "step_size"
CONF_ACTION_TYPE = "type"


@automation.register_action(
    "light.toggle",
    ToggleAction,
    automation.maybe_simple_id(
        {
            cv.Required(CONF_ID): cv.use_id(LightState),
            cv.Optional(CONF_TRANSITION_LENGTH): cv.templatable(
                cv.positive_time_period_milliseconds
            ),
        }
    ),
)
async def light_toggle_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    if CONF_TRANSITION_LENGTH in config:
        template_ = await cg.templatable(
            config[CONF_TRANSITION_LENGTH], args, cg.uint32
        )
        cg.add(var.set_transition_length(template_))
    return var


LIGHT_STATE_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_COLOR_MODE): cv.enum(COLOR_MODES, upper=True, space="_"),
        cv.Optional(CONF_STATE): cv.templatable(cv.boolean),
        cv.Optional(CONF_BRIGHTNESS): cv.templatable(cv.percentage),
        cv.Optional(CONF_COLOR_BRIGHTNESS): cv.templatable(cv.percentage),
        cv.Optional(CONF_RED): cv.templatable(cv.percentage),
        cv.Optional(CONF_GREEN): cv.templatable(cv.percentage),
        cv.Optional(CONF_BLUE): cv.templatable(cv.percentage),
        cv.Optional(CONF_WHITE): cv.templatable(cv.percentage),
        cv.Optional(CONF_COLOR_TEMPERATURE): cv.templatable(cv.color_temperature),
        cv.Optional(CONF_COLD_WHITE): cv.templatable(cv.percentage),
        cv.Optional(CONF_WARM_WHITE): cv.templatable(cv.percentage),
    }
)

# Dynamic control sub-schemas
BRIGHTNESS_ACTION_TYPES = {
    "move": "move",
    "step": "step",
    "move_to_level": "move_to_level",
}

BRIGHTNESS_ACTION_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ACTION_TYPE, default="move"): cv.enum(
            BRIGHTNESS_ACTION_TYPES, lower=True
        ),
        cv.Required(CONF_DIRECTION): cv.templatable(
            cv.enum(TRANSITION_DIRECTIONS, upper=True)
        ),
        cv.Optional(CONF_SPEED, default=1.0): cv.templatable(cv.positive_float),
        cv.Optional(CONF_STEP_SIZE): cv.templatable(cv.percentage),
        cv.Optional(CONF_TRANSITION_LENGTH, default="0ms"): cv.templatable(
            cv.positive_time_period_milliseconds
        ),
        cv.Optional("target_level"): cv.templatable(cv.percentage),
    }
).add_extra(cv.has_at_least_one_key(CONF_SPEED, CONF_STEP_SIZE, "target_level"))

HUE_ACTION_TYPES = {
    "move": "move",
    "step": "step",
    "move_to_hue": "move_to_hue",
}

HUE_ACTION_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ACTION_TYPE, default="move"): cv.enum(
            HUE_ACTION_TYPES, lower=True
        ),
        cv.Required(CONF_DIRECTION): cv.templatable(
            cv.enum(TRANSITION_DIRECTIONS, upper=True)
        ),
        cv.Optional(CONF_SPEED, default=30.0): cv.templatable(
            cv.positive_float
        ),  # degrees/sec
        cv.Optional(CONF_STEP_SIZE): cv.templatable(cv.float_range(min=1.0, max=360.0)),
        cv.Optional(CONF_TRANSITION_LENGTH, default="0ms"): cv.templatable(
            cv.positive_time_period_milliseconds
        ),
        cv.Optional(CONF_TARGET_HUE): cv.templatable(
            cv.float_range(min=0.0, max=360.0)
        ),
        cv.Optional(CONF_HUE_PATH, default="SHORTEST"): cv.enum(
            HUE_TRANSITION_PATHS, upper=True
        ),
    }
).add_extra(cv.has_at_least_one_key(CONF_SPEED, CONF_STEP_SIZE, CONF_TARGET_HUE))

SATURATION_ACTION_TYPES = {
    "move": "move",
    "step": "step",
    "move_to_saturation": "move_to_saturation",
}

SATURATION_ACTION_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ACTION_TYPE, default="move"): cv.enum(
            SATURATION_ACTION_TYPES, lower=True
        ),
        cv.Required(CONF_DIRECTION): cv.templatable(
            cv.enum(TRANSITION_DIRECTIONS, upper=True)
        ),
        cv.Optional(CONF_SPEED, default=0.5): cv.templatable(
            cv.positive_float
        ),  # saturation units/sec
        cv.Optional(CONF_STEP_SIZE): cv.templatable(cv.percentage),
        cv.Optional(CONF_TRANSITION_LENGTH, default="0ms"): cv.templatable(
            cv.positive_time_period_milliseconds
        ),
        cv.Optional(CONF_TARGET_SATURATION): cv.templatable(cv.percentage),
    }
).add_extra(cv.has_at_least_one_key(CONF_SPEED, CONF_STEP_SIZE, CONF_TARGET_SATURATION))

COLOR_TEMPERATURE_ACTION_TYPES = {
    "move": "move",
    "step": "step",
    "move_to_color_temperature": "move_to_color_temperature",
}

COLOR_TEMPERATURE_ACTION_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_ACTION_TYPE, default="move"): cv.enum(
            COLOR_TEMPERATURE_ACTION_TYPES, lower=True
        ),
        cv.Required(CONF_DIRECTION): cv.templatable(
            cv.enum(TRANSITION_DIRECTIONS, upper=True)
        ),
        cv.Optional(CONF_SPEED, default=50.0): cv.templatable(
            cv.positive_float
        ),  # mireds/sec
        cv.Optional(CONF_STEP_SIZE): cv.templatable(cv.positive_float),
        cv.Optional(CONF_TRANSITION_LENGTH, default="0ms"): cv.templatable(
            cv.positive_time_period_milliseconds
        ),
        cv.Optional(CONF_TARGET_COLOR_TEMPERATURE): cv.templatable(
            cv.color_temperature
        ),
    }
).add_extra(
    cv.has_at_least_one_key(CONF_SPEED, CONF_STEP_SIZE, CONF_TARGET_COLOR_TEMPERATURE)
)

DYNAMIC_CONTROL_SCHEMA = cv.Schema(
    {
        cv.Exclusive(
            CONF_BRIGHTNESS_ACTION, "dynamic_action"
        ): BRIGHTNESS_ACTION_SCHEMA,
        cv.Exclusive(CONF_HUE_ACTION, "dynamic_action"): HUE_ACTION_SCHEMA,
        cv.Exclusive(
            CONF_SATURATION_ACTION, "dynamic_action"
        ): SATURATION_ACTION_SCHEMA,
        cv.Exclusive(
            CONF_COLOR_TEMPERATURE_ACTION, "dynamic_action"
        ): COLOR_TEMPERATURE_ACTION_SCHEMA,
        cv.Exclusive(CONF_STOP_ACTION, "dynamic_action"): cv.Schema({}),
    }
)

LIGHT_CONTROL_ACTION_SCHEMA = LIGHT_STATE_SCHEMA.extend(
    {
        cv.Required(CONF_ID): cv.use_id(LightState),
        cv.Exclusive(CONF_TRANSITION_LENGTH, "transformer"): cv.templatable(
            cv.positive_time_period_milliseconds
        ),
        cv.Exclusive(CONF_FLASH_LENGTH, "transformer"): cv.templatable(
            cv.positive_time_period_milliseconds
        ),
        cv.Exclusive(CONF_EFFECT, "transformer"): cv.templatable(cv.string),
        cv.Exclusive(CONF_DYNAMIC_CONTROL, "transformer"): DYNAMIC_CONTROL_SCHEMA,
    }
)

LIGHT_TURN_OFF_ACTION_SCHEMA = automation.maybe_simple_id(
    {
        cv.Required(CONF_ID): cv.use_id(LightState),
        cv.Optional(CONF_TRANSITION_LENGTH): cv.templatable(
            cv.positive_time_period_milliseconds
        ),
        cv.Optional(CONF_STATE, default=False): False,
    }
)
LIGHT_TURN_ON_ACTION_SCHEMA = automation.maybe_simple_id(
    LIGHT_CONTROL_ACTION_SCHEMA.extend(
        {
            cv.Optional(CONF_STATE, default=True): True,
        }
    )
)


@automation.register_action(
    "light.turn_off", LightControlAction, LIGHT_TURN_OFF_ACTION_SCHEMA
)
@automation.register_action(
    "light.turn_on", LightControlAction, LIGHT_TURN_ON_ACTION_SCHEMA
)
@automation.register_action(
    "light.control", LightControlAction, LIGHT_CONTROL_ACTION_SCHEMA
)
async def light_control_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    if CONF_COLOR_MODE in config:
        template_ = await cg.templatable(config[CONF_COLOR_MODE], args, ColorMode)
        cg.add(var.set_color_mode(template_))
    if CONF_STATE in config:
        template_ = await cg.templatable(config[CONF_STATE], args, bool)
        cg.add(var.set_state(template_))
    if CONF_TRANSITION_LENGTH in config:
        template_ = await cg.templatable(
            config[CONF_TRANSITION_LENGTH], args, cg.uint32
        )
        cg.add(var.set_transition_length(template_))
    if CONF_FLASH_LENGTH in config:
        template_ = await cg.templatable(config[CONF_FLASH_LENGTH], args, cg.uint32)
        cg.add(var.set_flash_length(template_))
    if CONF_BRIGHTNESS in config:
        template_ = await cg.templatable(config[CONF_BRIGHTNESS], args, float)
        cg.add(var.set_brightness(template_))
    if CONF_COLOR_BRIGHTNESS in config:
        template_ = await cg.templatable(config[CONF_COLOR_BRIGHTNESS], args, float)
        cg.add(var.set_color_brightness(template_))
    if CONF_RED in config:
        template_ = await cg.templatable(config[CONF_RED], args, float)
        cg.add(var.set_red(template_))
    if CONF_GREEN in config:
        template_ = await cg.templatable(config[CONF_GREEN], args, float)
        cg.add(var.set_green(template_))
    if CONF_BLUE in config:
        template_ = await cg.templatable(config[CONF_BLUE], args, float)
        cg.add(var.set_blue(template_))
    if CONF_WHITE in config:
        template_ = await cg.templatable(config[CONF_WHITE], args, float)
        cg.add(var.set_white(template_))
    if CONF_COLOR_TEMPERATURE in config:
        template_ = await cg.templatable(config[CONF_COLOR_TEMPERATURE], args, float)
        cg.add(var.set_color_temperature(template_))
    if CONF_COLD_WHITE in config:
        template_ = await cg.templatable(config[CONF_COLD_WHITE], args, float)
        cg.add(var.set_cold_white(template_))
    if CONF_WARM_WHITE in config:
        template_ = await cg.templatable(config[CONF_WARM_WHITE], args, float)
        cg.add(var.set_warm_white(template_))
    if CONF_EFFECT in config:
        template_ = await cg.templatable(config[CONF_EFFECT], args, cg.std_string)
        cg.add(var.set_effect(template_))
    if CONF_DYNAMIC_CONTROL in config:
        dynamic_control = config[CONF_DYNAMIC_CONTROL]
        if CONF_BRIGHTNESS_ACTION in dynamic_control:
            brightness_config = dynamic_control[CONF_BRIGHTNESS_ACTION]
            action_type = brightness_config.get(CONF_ACTION_TYPE, "move")
            direction_template = await cg.templatable(
                brightness_config[CONF_DIRECTION], args, TransitionDirection
            )

            if action_type == "move":
                speed_template = await cg.templatable(
                    brightness_config[CONF_SPEED], args, float
                )
                cg.add(var.set_brightness_move(direction_template, speed_template))
            elif action_type == "step":
                step_size_template = await cg.templatable(
                    brightness_config[CONF_STEP_SIZE], args, float
                )
                transition_time_template = await cg.templatable(
                    brightness_config[CONF_TRANSITION_LENGTH], args, cg.uint32
                )
                cg.add(
                    var.set_brightness_step(
                        direction_template, step_size_template, transition_time_template
                    )
                )
            elif action_type == "move_to_level":
                target_template = await cg.templatable(
                    brightness_config["target_level"], args, float
                )
                transition_time_template = await cg.templatable(
                    brightness_config[CONF_TRANSITION_LENGTH], args, cg.uint32
                )
                cg.add(
                    var.set_brightness_move_to_level(
                        target_template, transition_time_template
                    )
                )

        elif CONF_HUE_ACTION in dynamic_control:
            hue_config = dynamic_control[CONF_HUE_ACTION]
            action_type = hue_config.get(CONF_ACTION_TYPE, "move")
            direction_template = await cg.templatable(
                hue_config[CONF_DIRECTION], args, TransitionDirection
            )

            if action_type == "move":
                speed_template = await cg.templatable(
                    hue_config[CONF_SPEED], args, float
                )
                cg.add(var.set_hue_move(direction_template, speed_template))
            elif action_type == "step":
                step_size_template = await cg.templatable(
                    hue_config[CONF_STEP_SIZE], args, float
                )
                transition_time_template = await cg.templatable(
                    hue_config[CONF_TRANSITION_LENGTH], args, cg.uint32
                )
                hue_path_template = await cg.templatable(
                    hue_config[CONF_HUE_PATH], args, HueTransitionPath
                )
                cg.add(
                    var.set_hue_step(
                        direction_template,
                        step_size_template,
                        transition_time_template,
                        hue_path_template,
                    )
                )
            elif action_type == "move_to_hue":
                target_template = await cg.templatable(
                    hue_config[CONF_TARGET_HUE], args, float
                )
                transition_time_template = await cg.templatable(
                    hue_config[CONF_TRANSITION_LENGTH], args, cg.uint32
                )
                hue_path_template = await cg.templatable(
                    hue_config[CONF_HUE_PATH], args, HueTransitionPath
                )
                cg.add(
                    var.set_hue_move_to_level(
                        target_template, transition_time_template, hue_path_template
                    )
                )

        elif CONF_SATURATION_ACTION in dynamic_control:
            saturation_config = dynamic_control[CONF_SATURATION_ACTION]
            action_type = saturation_config.get(CONF_ACTION_TYPE, "move")
            direction_template = await cg.templatable(
                saturation_config[CONF_DIRECTION], args, TransitionDirection
            )

            if action_type == "move":
                speed_template = await cg.templatable(
                    saturation_config[CONF_SPEED], args, float
                )
                cg.add(var.set_saturation_move(direction_template, speed_template))
            elif action_type == "step":
                step_size_template = await cg.templatable(
                    saturation_config[CONF_STEP_SIZE], args, float
                )
                transition_time_template = await cg.templatable(
                    saturation_config[CONF_TRANSITION_LENGTH], args, cg.uint32
                )
                cg.add(
                    var.set_saturation_step(
                        direction_template, step_size_template, transition_time_template
                    )
                )
            elif action_type == "move_to_saturation":
                target_template = await cg.templatable(
                    saturation_config[CONF_TARGET_SATURATION], args, float
                )
                transition_time_template = await cg.templatable(
                    saturation_config[CONF_TRANSITION_LENGTH], args, cg.uint32
                )
                cg.add(
                    var.set_saturation_move_to_level(
                        target_template, transition_time_template
                    )
                )

        elif CONF_COLOR_TEMPERATURE_ACTION in dynamic_control:
            color_temp_config = dynamic_control[CONF_COLOR_TEMPERATURE_ACTION]
            action_type = color_temp_config.get(CONF_ACTION_TYPE, "move")
            direction_template = await cg.templatable(
                color_temp_config[CONF_DIRECTION], args, TransitionDirection
            )

            if action_type == "move":
                speed_template = await cg.templatable(
                    color_temp_config[CONF_SPEED], args, float
                )
                cg.add(
                    var.set_color_temperature_move(direction_template, speed_template)
                )
            elif action_type == "step":
                step_size_template = await cg.templatable(
                    color_temp_config[CONF_STEP_SIZE], args, float
                )
                transition_time_template = await cg.templatable(
                    color_temp_config[CONF_TRANSITION_LENGTH], args, cg.uint32
                )
                cg.add(
                    var.set_color_temperature_step(
                        direction_template, step_size_template, transition_time_template
                    )
                )
            elif action_type == "move_to_color_temperature":
                target_template = await cg.templatable(
                    color_temp_config[CONF_TARGET_COLOR_TEMPERATURE], args, float
                )
                transition_time_template = await cg.templatable(
                    color_temp_config[CONF_TRANSITION_LENGTH], args, cg.uint32
                )
                cg.add(
                    var.set_color_temperature_move_to_level(
                        target_template, transition_time_template
                    )
                )

        elif CONF_STOP_ACTION in dynamic_control:
            cg.add(var.set_brightness_stop())
    return var


CONF_RELATIVE_BRIGHTNESS = "relative_brightness"
LIGHT_DIM_RELATIVE_ACTION_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(LightState),
        cv.Required(CONF_RELATIVE_BRIGHTNESS): cv.templatable(
            cv.possibly_negative_percentage
        ),
        cv.Optional(CONF_TRANSITION_LENGTH): cv.templatable(
            cv.positive_time_period_milliseconds
        ),
        cv.Optional(CONF_BRIGHTNESS_LIMITS): cv.Schema(
            {
                cv.Optional(CONF_MIN_BRIGHTNESS, default="0%"): cv.percentage,
                cv.Optional(CONF_MAX_BRIGHTNESS, default="100%"): cv.percentage,
                cv.Optional(CONF_LIMIT_MODE, default="CLAMP"): cv.enum(
                    LIMIT_MODES, upper=True, space="_"
                ),
            }
        ),
    }
)


@automation.register_action(
    "light.dim_relative", DimRelativeAction, LIGHT_DIM_RELATIVE_ACTION_SCHEMA
)
async def light_dim_relative_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    templ = await cg.templatable(config[CONF_RELATIVE_BRIGHTNESS], args, float)
    cg.add(var.set_relative_brightness(templ))
    if CONF_TRANSITION_LENGTH in config:
        templ = await cg.templatable(config[CONF_TRANSITION_LENGTH], args, cg.uint32)
        cg.add(var.set_transition_length(templ))
    if conf := config.get(CONF_BRIGHTNESS_LIMITS):
        cg.add(
            var.set_min_max_brightness(
                conf[CONF_MIN_BRIGHTNESS], conf[CONF_MAX_BRIGHTNESS]
            )
        )
        cg.add(var.set_limit_mode(conf[CONF_LIMIT_MODE]))
    return var


LIGHT_ADDRESSABLE_SET_ACTION_SCHEMA = cv.Schema(
    {
        cv.Required(CONF_ID): cv.use_id(AddressableLightState),
        cv.Optional(CONF_RANGE_FROM): cv.templatable(cv.positive_int),
        cv.Optional(CONF_RANGE_TO): cv.templatable(cv.positive_int),
        cv.Optional(CONF_COLOR_BRIGHTNESS): cv.templatable(cv.percentage),
        cv.Optional(CONF_RED): cv.templatable(cv.percentage),
        cv.Optional(CONF_GREEN): cv.templatable(cv.percentage),
        cv.Optional(CONF_BLUE): cv.templatable(cv.percentage),
        cv.Optional(CONF_WHITE): cv.templatable(cv.percentage),
    }
)


@automation.register_action(
    "light.addressable_set", AddressableSet, LIGHT_ADDRESSABLE_SET_ACTION_SCHEMA
)
async def light_addressable_set_to_code(config, action_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    var = cg.new_Pvariable(action_id, template_arg, paren)
    if CONF_RANGE_FROM in config:
        templ = await cg.templatable(config[CONF_RANGE_FROM], args, cg.int32)
        cg.add(var.set_range_from(templ))
    if CONF_RANGE_TO in config:
        templ = await cg.templatable(config[CONF_RANGE_TO], args, cg.int32)
        cg.add(var.set_range_to(templ))

    if CONF_COLOR_BRIGHTNESS in config:
        templ = await cg.templatable(config[CONF_COLOR_BRIGHTNESS], args, cg.float_)
        cg.add(var.set_color_brightness(templ))
    if CONF_RED in config:
        templ = await cg.templatable(config[CONF_RED], args, cg.float_)
        cg.add(var.set_red(templ))
    if CONF_GREEN in config:
        templ = await cg.templatable(config[CONF_GREEN], args, cg.float_)
        cg.add(var.set_green(templ))
    if CONF_BLUE in config:
        templ = await cg.templatable(config[CONF_BLUE], args, cg.float_)
        cg.add(var.set_blue(templ))
    if CONF_WHITE in config:
        templ = await cg.templatable(config[CONF_WHITE], args, cg.float_)
        cg.add(var.set_white(templ))
    return var


@automation.register_condition(
    "light.is_on",
    LightIsOnCondition,
    automation.maybe_simple_id(
        {
            cv.Required(CONF_ID): cv.use_id(LightState),
        }
    ),
)
@automation.register_condition(
    "light.is_off",
    LightIsOffCondition,
    automation.maybe_simple_id(
        {
            cv.Required(CONF_ID): cv.use_id(LightState),
        }
    ),
)
async def light_is_on_off_to_code(config, condition_id, template_arg, args):
    paren = await cg.get_variable(config[CONF_ID])
    return cg.new_Pvariable(condition_id, template_arg, paren)
