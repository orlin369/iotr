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

class CPPWriter:
    """CPP file creator."""

#region Attributes

    __file_content = ""
    """File content."""

#endregion

#region Constructor

    def __init__(self):

        pass

#endregion

#region Private Methods

    def __format_byte(self, data):
        """Format string as hex const byte."""

        return "0x{:02X}".format(data)

    def __generate_array(self, bytes_content):

        byte_array = ""
        line_counter = 0
        global_counter = 0

        size = len(bytes_content)
        for single_byte in bytes_content:

            byte_array += self.__format_byte(single_byte)

            if global_counter < size - 1:
                byte_array += ", "

            global_counter += 1

            if line_counter == 16:
                line_counter = 0
                byte_array += "\n"

            else:
                line_counter += 1

        self.__file_content += byte_array

    def __write_array_name(self, type_name, name, attribute):

        self.__file_content += "\r"
        value_name = name.replace(".", "_")
        array_name = "{} {}[] {} = \r{}\r".format(type_name, value_name, attribute, "{")
        self.__file_content += array_name

#endregion

#region Public Methods

    def write_new_line(self):
        """Write new line."""

        self.__file_content += "\r"

    def write_comment(self, comment=""):
        """Write comment line."""

        self.__file_content += "// {}\r".format(comment)

    def write_define(self, name, value=""):
        """Write definition line."""

        value_name = name.replace(".", "_")
        self.__file_content += "#define {} {}\r".format(value_name, value)

    def write_ifdef(self, value):
        """Write ifdef open block."""

        self.__file_content += "#ifdef {}\r".format(value)

    def write_ifndef(self, value):
        """Write nifdef open block."""

        self.__file_content += "#ifndef {}\r".format(value)

    def write_endif(self, value=""):
        """Write endif close block."""

        self.__file_content += "#endif // {}\r".format(value)

    def write_array(self, type_name, name, attribute, content):
        """Write array."""

        # Write byte array name.
        self.__write_array_name(type_name, name, attribute)

        # Write byte array.
        self.__generate_array(content)

        # Write byte array end.
        self.__file_content += "\r{};\r".format("}")

    def write_header(self, file_name):
        """Write header text."""

        self.write_comment()
        self.write_comment(file_name)
        self.write_comment()
        self.write_new_line()

    def clear(self):
        """Clear the file content."""

        self.__file_content = ""

    def get_content(self):
        """Get file content."""

        return self.__file_content

    def to_file(self, file_path):
        """Export to file."""

        with open(file_path, 'w') as output_file:
            output_file.write(self.get_content())
            output_file.close()

#endregion
