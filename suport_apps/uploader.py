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
import argparse
from base64 import b64encode

import requests
from requests.auth import HTTPDigestAuth

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

def main():
    """Main function"""

    # Create parser.
    parser = argparse.ArgumentParser()

    # Add arguments.
    parser.add_argument("--ip", type=str, default="192.168.4.1", help="IP Address of the target.")
    parser.add_argument("--port", type=int, default=80, help="HTTP Port.")
    parser.add_argument("--path", type=str, default=".", help="Path to the target files.")
    parser.add_argument("--user", type=str, default="admin", help="Usre name")
    parser.add_argument("--password", type=str, default="admin", help="Password")

    # Take arguments.
    args = parser.parse_args()

    # Build URL.
    url = "http://{}:{}/api/v1/upload".format(args.ip, args.port)

    base_path = args.path
    for file in os.listdir(base_path):
        try:
            file_name = os.path.join(base_path, file)
            fin = open(file_name, 'rb')
            files = {'file': fin}
            print(file_name)
            r = requests.post(url=url, auth=HTTPDigestAuth(args.user, args.password), files=files)
            print(r)
        finally:
            fin.close()
    print("Done")

if __name__ == "__main__":
    main()
