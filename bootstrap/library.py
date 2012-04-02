"""
struct to hold info needed to build library
"""

import yaml
import sys

class LibraryMetadata:
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
        return LibraryMetadata("boost")

    @staticmethod
    def zlib():
        return LibraryMetadata("zlib")

    @staticmethod
    def thrift():
        return LibraryMetadata("thrift")

    @staticmethod
    def jpeg():
        return LibraryMetadata("jpeg")

    @staticmethod
    def png():
        return LibraryMetadata("png")
