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
import argparse

from header_builder import HeaderBuilder

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
    parser.add_argument("--path",\
        type=str, default=".\\..\\..\\IoTR\\data",\
            help="Path to the target files.")

    # Take arguments.
    args = parser.parse_args()

    base_path = args.path

    header_builder = HeaderBuilder(base_path)

    for dir_item in os.listdir(base_path):
        try:
            header_builder.build(dir_item)
        finally:
            pass

    print("Done")

if __name__ == "__main__":
    main()
