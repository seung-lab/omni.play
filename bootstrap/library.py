"""
struct to hold info needed to build library
"""

import yaml
import sys

class Library:
    def __init__(self, key):
        f = open("bootstrap/metadata.yaml", 'r')
        libs = yaml.load(f)

        if not libs.has_key(key):
            print "library metadata not found for \"{key}\"".format(key=key)
            sys.exit(1)

        lib = libs[key]
        self.baseFileName = lib["ver"]
        self.libFolderName = lib["lib_folder"]
        self.uri = lib["uri"]

    @staticmethod
    def boost():
        return Library("boost")

    @staticmethod
    def zlib():
        return Library("zlib")

    @staticmethod
    def thrift():
        return Library("thrift")

    @staticmethod
    def jpeg():
        return Library("libjpeg")

    @staticmethod
    def png():
        return Library("libpng")
