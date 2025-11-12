import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    CONF_CHANNEL,
    DEVICE_CLASS_BATTERY,
    DEVICE_CLASS_HUMIDITY,
    DEVICE_CLASS_TEMPERATURE,
    STATE_CLASS_MEASUREMENT,
    UNIT_CELSIUS,
    UNIT_PERCENT,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

CODEOWNERS = ["@your_username"]
DEPENDENCIES = ["remote_receiver"]
AUTO_LOAD = ["sensor"]

bl999_sensor_ns = cg.esphome_ns.namespace("bl999_sensor")
BL999Sensor = bl999_sensor_ns.class_("BL999Sensor", cg.Component)

CONF_POWER_UUID = "power_uuid"
CONF_RECEIVER_ID = "receiver_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(BL999Sensor),
        cv.Required(CONF_RECEIVER_ID): cv.use_id(cg.global_ns.GlobalRP),
        cv.Optional(CONF_CHANNEL): sensor.sensor_schema(
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_POWER_UUID): sensor.sensor_schema(
            accuracy_decimals=0,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Required("temperature"): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Required("humidity"): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_HUMIDITY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Required("battery"): sensor.sensor_schema(
            unit_of_measurement=UNIT_PERCENT,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_BATTERY,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Получаем ID remote_receiver и передаем его в компонент
    receiver = await cg.get_variable(config[CONF_RECEIVER_ID])
    cg.add(var.set_remote_receiver(receiver))

    if "temperature" in config:
        sens = await sensor.new_sensor(config["temperature"])
        cg.add(var.set_temperature_sensor(sens))
    
    if "humidity" in config:
        sens = await sensor.new_sensor(config["humidity"])
        cg.add(var.set_humidity_sensor(sens))
    
    if "battery" in config:
        sens = await sensor.new_sensor(config["battery"])
        cg.add(var.set_battery_sensor(sens))
    
    if CONF_CHANNEL in config:
        sens = await sensor.new_sensor(config[CONF_CHANNEL])
        cg.add(var.set_channel_sensor(sens))
    
    if CONF_POWER_UUID in config:
        sens = await sensor.new_sensor(config[CONF_POWER_UUID])
        cg.add(var.set_power_uuid_sensor(sens))