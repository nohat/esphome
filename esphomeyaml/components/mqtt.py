import voluptuous as vol

import esphomeyaml.config_validation as cv
from esphomeyaml.const import CONF_BIRTH_MESSAGE, CONF_BROKER, CONF_DISCOVERY, \
    CONF_DISCOVERY_PREFIX, CONF_DISCOVERY_RETAIN, CONF_ID, CONF_MQTT, CONF_PASSWORD, \
    CONF_PAYLOAD, CONF_PORT, CONF_QOS, CONF_RETAIN, CONF_TOPIC, CONF_TOPIC_PREFIX, CONF_USERNAME, \
    CONF_WILL_MESSAGE, CONF_CLIENT_ID
from esphomeyaml.helpers import App, Pvariable, StructInitializer, add, exp_empty_optional

MQTT_WILL_BIRTH_SCHEMA = vol.Any(None, vol.Schema({
    vol.Required(CONF_TOPIC): cv.publish_topic,
    vol.Required(CONF_PAYLOAD): cv.mqtt_payload,
    vol.Optional(CONF_QOS, default=0): vol.All(vol.Coerce(int), vol.In([0, 1, 2])),
    vol.Optional(CONF_RETAIN, default=True): cv.boolean,
}))


def validate_broker(value):
    value = cv.string_strict(value)
    if value.endswith(u'.local'):
        raise vol.Invalid(u"MQTT server addresses ending with '.local' are currently unsupported."
                          u" Please specify the static IP instead.")
    if u':' in value:
        raise vol.Invalid(u"Please specify the port using the port: option")
    return value


CONFIG_SCHEMA = cv.ID_SCHEMA.extend({
    cv.GenerateID(CONF_MQTT): cv.register_variable_id,
    vol.Required(CONF_BROKER): validate_broker,
    vol.Optional(CONF_PORT, default=1883): cv.port,
    vol.Optional(CONF_USERNAME, default=''): cv.string,
    vol.Optional(CONF_PASSWORD, default=''): cv.string,
    vol.Optional(CONF_CLIENT_ID): vol.All(cv.string, vol.Length(max=23)),
    vol.Optional(CONF_DISCOVERY): cv.boolean,
    vol.Optional(CONF_DISCOVERY_RETAIN): cv.boolean,
    vol.Optional(CONF_DISCOVERY_PREFIX): cv.publish_topic,
    vol.Optional(CONF_BIRTH_MESSAGE): MQTT_WILL_BIRTH_SCHEMA,
    vol.Optional(CONF_WILL_MESSAGE): MQTT_WILL_BIRTH_SCHEMA,
    vol.Optional(CONF_TOPIC_PREFIX): cv.publish_topic,
})


def exp_mqtt_message(config):
    if config is None:
        return exp_empty_optional('mqtt::MQTTMessage')
    exp = StructInitializer(
        'mqtt::MQTTMessage',
        ('topic', config[CONF_TOPIC]),
        ('payload', config[CONF_PAYLOAD]),
        ('qos', config[CONF_QOS]),
        ('retain', config[CONF_RETAIN])
    )
    return exp


def to_code(config):
    rhs = App.init_mqtt(config[CONF_BROKER], config[CONF_PORT],
                        config[CONF_USERNAME], config[CONF_PASSWORD])
    mqtt = Pvariable('mqtt::MQTTClientComponent', config[CONF_ID], rhs)
    if not config.get(CONF_DISCOVERY, True):
        add(mqtt.disable_discovery())
    if CONF_DISCOVERY_RETAIN in config or CONF_DISCOVERY_PREFIX in config:
        discovery_retain = config.get(CONF_DISCOVERY_RETAIN, True)
        discovery_prefix = config.get(CONF_DISCOVERY_PREFIX, 'homeassistant')
        add(mqtt.set_discovery_info(discovery_prefix, discovery_retain))
    if CONF_BIRTH_MESSAGE in config:
        add(mqtt.set_birth_message(config[CONF_BIRTH_MESSAGE]))
    if CONF_WILL_MESSAGE in config:
        add(mqtt.set_last_will(config[CONF_WILL_MESSAGE]))
    if CONF_TOPIC_PREFIX in config:
        add(mqtt.set_topic_prefix(config[CONF_TOPIC_PREFIX]))
    if CONF_CLIENT_ID in config:
        add(mqtt.set_client_id(config[CONF_CLIENT_ID]))