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
import sys
import time
import json
from time import gmtime, strftime
from datetime import datetime
import logging
import signal
import argparse
import traceback

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

#region Variables

__time_to_stop = False
"""Time to stop flag."""

__logger_name = "MQTT"
"""Data logger name."""

__logger = None
"""Data logger object."""

__mqttc = None
"""MQTT Client"""

#endregion

#region Logger

def crate_log_file(logs_dir_name="logs/"):
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

def get_logger(name):
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

    logger = logging.getLogger(name)

    # create console handler and set level to debug
    ch = logging.StreamHandler()

    # Set level
    ch.setLevel(logging.INFO)

    # add ch to logger
    logger.addHandler(ch)

    return logger

#endregion

#region MQTT

def on_connect(mosq, obj, flags, rc):
    global __logger, __mqttc

    __logger.info("Connected with RC: {}".format(str(rc)))
    __mqttc.publish("test/device/update", 1)

def on_error(mosq, obj):
    global __logger, __mqttc

    __logger.info("What is going on????")

#endregion

#region Runtime

def interupt_handler(signum, frame):
    """Interupt handler."""

    global __logger, __time_to_stop, __mqttc

    __time_to_stop = True

    if signum == 2:
        __logger.warning("Stopped by interupt.")

    elif signum == 15:
        __logger.warning("Stopped by termination.")

    else:
        __logger.warning("Signal handler called. Signal: {}; Frame: {}".format(signum, frame))

    __mqttc.disconnect()
    __time_to_stop = True

def main():
    """Main"""

    global __logger, __logger_name, __mqttc, __time_to_stop

    # Add signal handler.
    signal.signal(signal.SIGINT, interupt_handler)
    signal.signal(signal.SIGTERM, interupt_handler)

    # Create LOG file.
    crate_log_file()

    # Get loger.
    __logger = get_logger(__logger_name)
    __logger.info("Starting")

    # Create parser.
    parser = argparse.ArgumentParser()

    # Add arguments.
    no_credentials= True
    if no_credentials:
        parser.add_argument("--host", type=str, default="broker.mqtt-dashboard.com", help="Host of the MQTT service.")
        parser.add_argument("--port", type=int, default="1883", help="IP port of the MQTT service")
        parser.add_argument("--alive", type=int, default="60", help="Keep alive.")
    else:
        parser.add_argument("--host", type=str, default="broker.mqtt-dashboard.com", help="Host of the MQTT service.")
        parser.add_argument("--port", type=int, default="1883", help="IP port of the MQTT service")
        parser.add_argument("--alive", type=int, default="60", help="Keep alive.")
        parser.add_argument("--user", type=str, default="admin", help="Username of the MQTT service.")
        parser.add_argument("--password", type=str, default="admin", help="Password of the MQTT service.")

    # Take arguments.
    args = parser.parse_args()

    # Initiate MQTT Client
    # TODO: Make instance to be unique.
    __mqttc = mqtt.Client(client_id="test-id")

    # Assign event callbacks
    __mqttc.on_connect = on_connect
    __mqttc.on_disconnect = on_error

    # Set credentials.
    if not no_credentials:
        __mqttc.username_pw_set(args.user, args.password)

    # Connect with MQTT Broker
    __mqttc.connect(host=args.host, port=args.port, keepalive=args.alive)
    print(args.host)

    # Continue monitoring the incoming messages for subscribed topic
    while not __time_to_stop:
        __mqttc.loop()

#endregion

if __name__ == "__main__":
    try:
        main()
    except Exception:
        __logger.error(traceback.format_exc())