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
import gzip

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

    def __init__(self):

        pass

def get_content_type(filename):

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

def generate_name(base_path, dir_item):

    file_path = os.path.join(base_path, dir_item)
    file_name = os.path.basename(file_path)
    name = os.path.splitext(file_name)[0]
    out_file_name = "bzf_{}.h".format(name)

    return out_file_name

def format_byte(data):
    return "0x{:02X}".format(data)

def generate_zip_array(bytes_content):

    byte_array = ""
    line_counter = 0
    global_counter = 0

    size = len(bytes_content)
    for single_byte in bytes_content:

        byte_array += format_byte(single_byte)

        if global_counter < size - 1:
            byte_array += ", "

        global_counter += 1

        if line_counter == 16:
            line_counter = 0
            byte_array += "\n"

        else:
            line_counter += 1

    return byte_array

def write_comment(target_file, comment):
    target_file.write("// {}\r".format(comment))

def write_header(target_file, file_name):
    target_file.write("//\r")
    target_file.write("// {}\r".format(file_name))
    target_file.write("//\r\r")

def write_define(target_file, name, value=""):
    value_name = name.replace(".", "_")
    target_file.write("#define {} {}\r".format(value_name, value))

def write_ifdef(target_file, value):
    target_file.write("#ifdef {}\r".format(value))

def write_ifndef(target_file, value):
    target_file.write("#ifndef {}\r".format(value))

def write_endif(target_file, value=""):
    target_file.write("#endif // {}\r".format(value))

def write_array_name(target_file, name):
    target_file.write("\r")
    value_name = name.replace(".", "_")
    array_name = "const uint8_t {}[] PROGMEM = \r{}\r".format(value_name, "{")
    target_file.write(array_name)

def write_array(target_file, name, content):

    # Write byte array name.
    write_array_name(target_file, name)

    # Write byte array.
    byte_array = generate_zip_array(content)
    target_file.write(byte_array)

    # Write byte array end.
    line = "\r{};\r".format("}")
    target_file.write(line)

def generate_include_line(file_name):
    return "#include \"src\\{}\"".format(file_name)

def build_file(base_path, dir_item):
    # 1. Generate name.
    src_file_path = os.path.join(base_path, dir_item)

    # 2. Open source file.
    with open(src_file_path, 'rb') as src_file:
        src_data = src_file.read()
        src_file.close()

    # 3. Generate compressed data.
    compressed_dst_data = gzip.compress(src_data)

    # 4. Generate output file name.
    out_file_name = generate_name(base_path, dir_item)
    dst_file_path = os.path.join(base_path, "..\\src\\", out_file_name)

    # 5. Write headers
    with open(dst_file_path, 'w') as dst_file:
        # Header name
        write_header(dst_file, out_file_name)

        # Write automation comment.
        write_comment(dst_file, "THIS FILE IS AUTOMATIC GENERATED")

        # Write security strings
        out_file_name_hed = out_file_name.replace(".", "_")
        out_file_name_hed = out_file_name_hed.upper()
        out_file_name_hed = "_" + out_file_name_hed
        write_ifndef(dst_file,  out_file_name_hed)
        write_define(dst_file, out_file_name_hed)

        # Beautify
        dst_file.write("\r")

    	# TODO: Add it.
        """        
        #if defined(ARDUINO) && ARDUINO >= 100
            #include "Arduino.h"
        #else
            #include "WProgram.h"
        #endif
        """

        # Define mime type.
        dir_item_def_name = out_file_name.replace(".h", "")
        mime_type = get_content_type(dir_item)
        dir_item_def_name = "{}{}".format(dir_item_def_name.upper(), "_MT")
        write_define(dst_file, dir_item_def_name, "\"{}\"".format(mime_type))

        # Define file name.
        dir_item_def_name = out_file_name.replace(".h", "")
        dir_item_def_name = "{}{}".format(dir_item_def_name.upper(), "_PATH")
        write_define(dst_file, dir_item_def_name, "\"/{}\"".format(dir_item))

        # Define array size.
        out_file_name_no_h = out_file_name.replace(".h", "")
        def_name = "{}{}".format(out_file_name_no_h.upper(), "_SIZE")
        def_value = len(compressed_dst_data)
        write_define(dst_file, def_name, def_value)

        # Write array.
        write_array(dst_file, out_file_name_no_h, compressed_dst_data)

        dst_file.write("\r")

        # End security.
        write_endif(dst_file, out_file_name_hed)

        # Close the file.
        dst_file.close()

        # 6. Generate Include lines
        include_line = generate_include_line(out_file_name)
        print(include_line)

def main():
    """Main function"""

    # Create parser.
    parser = argparse.ArgumentParser()

    # Add arguments.
    parser.add_argument("--path", type=str, default=".\\..\\IoTR\\IoTR\\data", help="Path to the target files.")

    # Take arguments.
    args = parser.parse_args()

    base_path = args.path
    for dir_item in os.listdir(base_path):
        try:
            build_file(base_path, dir_item)
        finally:
            pass

    print("Done")

if __name__ == "__main__":
    main()
