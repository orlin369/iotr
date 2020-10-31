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
import gzip

from cpp_writer import CPPWriter

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

class HeaderBuilder:
    """Header builder"""

#region Attributes

    __base_path = ""
    """Base path directory name."""

    __output_file = None
    """Output file."""

#endregion

#region Constructor

    def __init__(self, base_path):
        """Constructor"""

        self.__base_path = base_path
        self.__output_file = CPPWriter()

#endregion

#region Private Methods

    def __generate_include_line(self, file_name):

        return "#include \"src\\{}\"".format(file_name)

    def __get_content_type(self, filename):

        mime_type = "text/plain"

        if filename.endswith(".htm"):
            mime_type = "text/html"

        elif filename.endswith(".html"):
            mime_type = "text/html"

        elif filename.endswith(".css"):
            mime_type = "text/css"

        elif filename.endswith(".js"):
            mime_type = "application/javascript"

        elif filename.endswith(".json"):
            mime_type = "application/json"

        elif filename.endswith(".png"):
            mime_type = "image/png"

        elif filename.endswith(".gif"):
            mime_type = "image/gif"

        elif filename.endswith(".jpg"):
            mime_type = "image/jpeg"

        elif filename.endswith(".ico"):
            mime_type = "image/x-icon"

        elif filename.endswith(".xml"):
            mime_type = "text/xml"

        elif filename.endswith(".pdf"):
            mime_type = "application/x-pdf"

        elif filename.endswith(".zip"):
            mime_type = "application/x-zip"

        elif filename.endswith(".gz"):
            mime_type = "application/x-gzip"

        return mime_type

    def __generate_name(self, base_path, dir_item):

        file_path = os.path.join(base_path, dir_item)
        file_name = os.path.basename(file_path)
        name = os.path.splitext(file_name)[0]
        out_file_name = "bzf_{}.h".format(name)

        return out_file_name

    def __get_zip_content(self, file_path):

        # Open source file.
        with open(file_path, 'rb') as src_file:
            src_data = src_file.read()
            src_file.close()

        return gzip.compress(src_data)

    def __escape_extention(self, text):
        """Escape extention."""

        return text.replace(".h", "")


#endregion

    def build(self, dir_item):
        """Build the file."""

        # Generate name.
        src_file_path = os.path.join(self.__base_path, dir_item)

        # Generate compressed data.
        compressed_dst_data = self.__get_zip_content(src_file_path)

        # Generate output file name.
        out_file_name = self.__generate_name(self.__base_path, dir_item)
        dst_file_path = os.path.join(self.__base_path, "..\\src\\", out_file_name)

        # Header name
        self.__output_file.write_header(out_file_name)

        # Write automation comment.
        self.__output_file.write_comment("THIS FILE IS AUTOMATIC GENERATED")

        # Write security strings
        out_file_name_hed = out_file_name.replace(".", "_")
        out_file_name_hed = out_file_name_hed.upper()
        out_file_name_hed = "_" + out_file_name_hed
        self.__output_file.write_ifndef(out_file_name_hed)
        self.__output_file.write_define(out_file_name_hed)

        # Beautify
        self.__output_file.write_new_line()

        # Define mime type.
        dir_item_def_name = self.__escape_extention(out_file_name)
        mime_type = self.__get_content_type(dir_item)
        dir_item_def_name = "{}{}".format(dir_item_def_name.upper(), "_MT")
        self.__output_file.write_define(dir_item_def_name, "\"{}\"".format(mime_type))

        # Define file name.
        dir_item_def_name = self.__escape_extention(out_file_name)
        dir_item_def_name = "{}{}".format(dir_item_def_name.upper(), "_PATH")
        self.__output_file.write_define(dir_item_def_name, "\"/{}\"".format(dir_item))

        # Define array size.
        out_file_name_no_h = self.__escape_extention(out_file_name)
        def_name = "{}{}".format(out_file_name_no_h.upper(), "_SIZE")
        def_value = len(compressed_dst_data)
        self.__output_file.write_define(def_name, def_value)

        # Write array.
        self.__output_file.write_array("const uint8_t", out_file_name_no_h, "PROGMEM", compressed_dst_data)

        # Beautify
        self.__output_file.write_new_line()

        # End security.
        self.__output_file.write_endif(out_file_name_hed)

        # From the standart.
        # self.__output_file.write_new_line()

        # Put it to file.
        self.__output_file.to_file(dst_file_path)
        self.__output_file.clear()

        # Generate Include lines
        print(self.__generate_include_line(out_file_name))
