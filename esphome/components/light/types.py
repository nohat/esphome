from esphome import automation
import esphome.codegen as cg

# Base
light_ns = cg.esphome_ns.namespace("light")
LightState = light_ns.class_("LightState", cg.EntityBase, cg.Component)
AddressableLightState = light_ns.class_("AddressableLightState", LightState)
LightOutput = light_ns.class_("LightOutput")
AddressableLight = light_ns.class_("AddressableLight", LightOutput, cg.Component)
AddressableLightRef = AddressableLight.operator("ref")

Color = cg.esphome_ns.class_("Color")
LightColorValues = light_ns.class_("LightColorValues")

LightStateRTCState = light_ns.struct("LightStateRTCState")

# Color modes
ColorMode = light_ns.enum("ColorMode", is_class=True)
COLOR_MODES = {
    "ON_OFF": ColorMode.ON_OFF,
    "BRIGHTNESS": ColorMode.BRIGHTNESS,
    "WHITE": ColorMode.WHITE,
    "COLOR_TEMPERATURE": ColorMode.COLOR_TEMPERATURE,
    "COLD_WARM_WHITE": ColorMode.COLD_WARM_WHITE,
    "RGB": ColorMode.RGB,
    "RGB_WHITE": ColorMode.RGB_WHITE,
    "RGB_COLOR_TEMPERATURE": ColorMode.RGB_COLOR_TEMPERATURE,
    "RGB_COLD_WARM_WHITE": ColorMode.RGB_COLD_WARM_WHITE,
}

# Limit modes
LimitMode = light_ns.enum("LimitMode", is_class=True)
LIMIT_MODES = {
    "CLAMP": LimitMode.CLAMP,
    "DO_NOTHING": LimitMode.DO_NOTHING,
}

# Actions
ToggleAction = light_ns.class_("ToggleAction", automation.Action)
LightControlAction = light_ns.class_("LightControlAction", automation.Action)
DimRelativeAction = light_ns.class_("DimRelativeAction", automation.Action)
AddressableSet = light_ns.class_("AddressableSet", automation.Action)

# Matter Level Control Cluster Actions
MoveToLevelAction = light_ns.class_("MoveToLevelAction", automation.Action)
MoveAction = light_ns.class_("MoveAction", automation.Action)
StepAction = light_ns.class_("StepAction", automation.Action)
StopLevelAction = light_ns.class_("StopLevelAction", automation.Action)

# Matter Color Control Cluster Actions
MoveToHueAction = light_ns.class_("MoveToHueAction", automation.Action)
MoveHueAction = light_ns.class_("MoveHueAction", automation.Action)
StepHueAction = light_ns.class_("StepHueAction", automation.Action)
MoveToSaturationAction = light_ns.class_("MoveToSaturationAction", automation.Action)
MoveSaturationAction = light_ns.class_("MoveSaturationAction", automation.Action)
StepSaturationAction = light_ns.class_("StepSaturationAction", automation.Action)
MoveToHueAndSaturationAction = light_ns.class_("MoveToHueAndSaturationAction", automation.Action)
ColorLoopSetAction = light_ns.class_("ColorLoopSetAction", automation.Action)
StopMoveStepAction = light_ns.class_("StopMoveStepAction", automation.Action)

LightIsOnCondition = light_ns.class_("LightIsOnCondition", automation.Condition)
LightIsOffCondition = light_ns.class_("LightIsOffCondition", automation.Condition)

# Triggers
LightTurnOnTrigger = light_ns.class_(
    "LightTurnOnTrigger", automation.Trigger.template()
)
LightTurnOffTrigger = light_ns.class_(
    "LightTurnOffTrigger", automation.Trigger.template()
)
LightStateTrigger = light_ns.class_("LightStateTrigger", automation.Trigger.template())

# Effects
LightEffect = light_ns.class_("LightEffect")
PulseLightEffect = light_ns.class_("PulseLightEffect", LightEffect)
RandomLightEffect = light_ns.class_("RandomLightEffect", LightEffect)
LambdaLightEffect = light_ns.class_("LambdaLightEffect", LightEffect)
AutomationLightEffect = light_ns.class_("AutomationLightEffect", LightEffect)
StrobeLightEffect = light_ns.class_("StrobeLightEffect", LightEffect)
StrobeLightEffectColor = light_ns.class_("StrobeLightEffectColor", LightEffect)
FlickerLightEffect = light_ns.class_("FlickerLightEffect", LightEffect)
AddressableLightEffect = light_ns.class_("AddressableLightEffect", LightEffect)
AddressableLambdaLightEffect = light_ns.class_(
    "AddressableLambdaLightEffect", AddressableLightEffect
)
AddressableRainbowLightEffect = light_ns.class_(
    "AddressableRainbowLightEffect", AddressableLightEffect
)
AddressableColorWipeEffect = light_ns.class_(
    "AddressableColorWipeEffect", AddressableLightEffect
)
AddressableColorWipeEffectColor = light_ns.struct("AddressableColorWipeEffectColor")
AddressableScanEffect = light_ns.class_("AddressableScanEffect", AddressableLightEffect)
AddressableTwinkleEffect = light_ns.class_(
    "AddressableTwinkleEffect", AddressableLightEffect
)
AddressableRandomTwinkleEffect = light_ns.class_(
    "AddressableRandomTwinkleEffect", AddressableLightEffect
)
AddressableFireworksEffect = light_ns.class_(
    "AddressableFireworksEffect", AddressableLightEffect
)
AddressableFlickerEffect = light_ns.class_(
    "AddressableFlickerEffect", AddressableLightEffect
)
