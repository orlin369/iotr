#!/usr/bin/env python3
# -*- coding: utf8 -*-

"""

IoTR - Robot Monitoring Device System

Copyright (C) [2020] [Orlin Dmitrov]

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

"""

import os
import json
from time import gmtime, strftime
from datetime import datetime
import logging
import signal
import argparse
import traceback
import configparser

import paho.mqtt.client as mqtt

#region File Attributes

__author__ = "Orlin Dimitrov"
"""Author of the file."""

__copyright__ = "Orlin Dimitrov"
"""Copyrighter"""

__credits__ = ["Milen Cholakov"]
"""Credits"""

__license__ = "GPLv3"
"""License
@see http://www.gnu.org/licenses/"""

__version__ = "1.0.0"
"""Version of the file."""

__maintainer__ = "Orlin Dimitrov"
"""Name of the maintainer."""

__email__ = "orlin369@gmail.com"
"""E-mail of the author.
@see orlin369@gmail.com"""

__status__ = "Debug"
"""File status."""

#endregion

class Device:
    """Device monitor class."""

#region Attributes

    __settings_file = ""
    """Settings file path."""

    __config = None
    """Configuration"""

    __mqtt_client = None
    """MQTT Client"""

    __logger = None
    """Logger"""

    __organization = "roboleague"
    """Organization segment"""

    __product_id = "iotr"
    """Product ID"""

#endregion

#region Constructor

    def __init__(self, settings_file):

        # Create settings.
        self.__settings_file = settings_file
        self.__config = configparser.ConfigParser()

        # Create settings file if necessary.
        if not os.path.exists(self.__settings_file):
            self.__create_default_settings()

        # Load if it is there.
        else:
            self.__load_settings()

        # Get logger.
        # TODO: Take it from the settings.
        self.__create_logger()

        # Create MQTT client
        self.__create_mqtt_client()

        # Create log file.
        self.__crate_log_file()

        self.__logger.info("Starting")

#endregion

#region Properties

    @property
    def id(self):
        """Returns the ID of the device.

        Returns
        -------
        str
            Device ID.
        """
        return self.__config["DEVICE"]["id"]

#endregion

#region Private Methods

    def __create_topic(self, name):

        return "{}/{}/{}{}".format(
            self.__organization,
            self.__product_id,
            self.__config["DEVICE"]["id"],
            name)

    def __create_default_settings(self):

        # Device
        self.__config["DEVICE"] = \
            {\
                "id": "KerelV1"\
            }

        # MQTT
        self.__config["MQTT"] = \
            {\
                "host": "broker.mqtt-dashboard.com",\
                "port": 1883,\
                "alive": 60,\
                "auth": False,\
                "user": "admin",\
                "pass": "admin"\
            }

        self.__config["LOG"] = \
            {\
                "path": ".\\log\\",\
                "level": 10,\
            }

        # Write to file.
        with open(self.__settings_file, "w") as config_file:
            self.__config.write(config_file)

    def __load_settings(self):

        self.__config.read(self.__settings_file)


    def __create_mqtt_client(self):

        # Initiate MQTT Client
        self.__mqtt_client = mqtt.Client(client_id="", clean_session=True)

        # Assign event callbacks
        self.__mqtt_client.on_message = self.__on_message
        self.__mqtt_client.on_connect = self.__on_connect
        self.__mqtt_client.on_subscribe = self.__on_subscribe
        self.__mqtt_client.on_disconnect = self.__on_disconnect

        # Set credentials.
        if self.__config["MQTT"]["auth"]:
            self.__mqtt_client.username_pw_set(\
                self.__config["MQTT"]["user"],\
                self.__config["MQTT"]["pass"])

    def __on_connect(self, client, userdata, flags, rc):

        self.__logger.info("Connected with RC: {}".format(str(rc)))

        self.__mqtt_client.subscribe(self.__create_topic("/serial/out"), 0)
        self.__mqtt_client.subscribe(self.__create_topic("/serial/in"), 0)
        self.__mqtt_client.subscribe(self.__create_topic("/status"), 0)
        self.__mqtt_client.subscribe(self.__create_topic("/ir"), 0)

        # Test
        # self.__mqtt_client.subscribe("$SYS/broker/clients/connected", 0)
        # self.__mqtt_client.subscribe("$SYS/broker/clients/disconnected", 0)
        # self.__mqtt_client.subscribe("$SYS/#", 0)

    def __on_disconnect(self, client, userdata, rc):
        self.__logger.info("Disconnected with RC: {}".format(str(rc)))

    def __on_subscribe(self, client, userdata, mid, granted_qos):
        self.__logger.info("Subscribed to Topic: {} with QoS: {}".format(mid, str(granted_qos)))

    def __on_message(self, client, userdata, msg):

        message = msg.payload.decode("utf-8")
        content = {"topic": str(msg.topic), "payload": message, "qos": msg.qos}
        self.__logger.info(str(content))

        if msg.topic == self.__create_topic("/status"):
            jmsg = json.loads(message)
            dt_object = datetime.fromtimestamp(jmsg["ts"])
            print(dt_object)

        elif msg.topic == self.__create_topic("/serial/in"):
            print(message)


    def __crate_log_file(self, logs_dir_name="logs/"):
        """This method create a new instance of the LOG direcotry.

        Parameters
        ----------
        logs_dir_name : str
            Path to the log direcotory.
        """

        # Crete log directory.
        if not os.path.exists(logs_dir_name):
            os.makedirs(logs_dir_name)

        # File name.
        log_file = ""
        log_file += logs_dir_name
        log_file += strftime("%Y%m%d", gmtime())
        log_file += ".log"

        # create message format.
        log_format = "%(asctime)s\t%(levelname)s\t%(name)s\t%(message)s"

        logging.basicConfig( \
            filename=log_file, \
            level=logging.INFO, \
            format=log_format)

    def __create_logger(self):
        """Generate a device circuit for major and minor index.

        Parameters
        ----------
        name : str
            Logger name.

        Returns
        -------
        logger
            Logger object.
        """

        self.__logger = logging.getLogger(self.__config["DEVICE"]["id"])

        # create console handler and set level to debug
        ch = logging.StreamHandler()

        # Set level
        ch.setLevel(logging.INFO)

        # add ch to logger
        self.__logger.addHandler(ch)

#endregion

#region Public Methods

    def connect(self):

        port = self.__config["MQTT"]["port"]
        port = int(port)

        keepalive = self.__config["MQTT"]["alive"]
        keepalive = int(keepalive)

        host = self.__config["MQTT"]["host"]

        # Connect with MQTT Broker
        self.__mqtt_client.connect(host=host, port=port, keepalive=keepalive)

    def update(self):

        # Continue monitoring the incoming messages for subscribed topic
        self.__mqtt_client.loop()

    def disconnect(self):
        self.__mqtt_client.disconnect()

#endregion


#region Variables

__time_to_stop = False
"""Time to stop flag."""

__devices = {}
"""Devices"""

#endregion

def interupt_handler(signum, frame):
    """Interupt handler."""

    global __time_to_stop, __devices

    if signum == 2:
        print("Stopped by interupt.")

    elif signum == 15:
        print("Stopped by termination.")

    else:
        print("Signal handler called. Signal: {}; Frame: {}".format(signum, frame))

    __time_to_stop = True

    for device in __devices:
        dev = __devices[device]
        dev.disconnect()
        print("Disconnect device: {}".format(dev.id))

def main():
    """Main"""

    global __time_to_stop, __devices

    __time_to_stop = False

    # Add signal handler.
    signal.signal(signal.SIGINT, interupt_handler)
    signal.signal(signal.SIGTERM, interupt_handler)

    # Create parser.
    parser = argparse.ArgumentParser()

    # Add path.
    parser.add_argument("--path", type=str, default=".", help="Home of the settings files.")

    # Take arguments.
    args = parser.parse_args()

    base_path = args.path
    base_path = os.path.join(base_path, "devices")
    for dir_item in os.listdir(base_path):
        settings_path = os.path.join(base_path, dir_item)
        if settings_path.endswith(".ini"):
            print("Create device: {}".format(settings_path))

            # Create device
            __devices.update({dir_item: Device(settings_path)})

    for device in __devices:
        dev = __devices[device]
        dev.connect()
        print("Connect device: {}".format(dev.id))

    while not __time_to_stop:
        for device in __devices:
            __devices[device].update()

if __name__ == "__main__":
    try:
        main()
    except Exception:
        print(traceback.format_exc())
    finally:
        pass
