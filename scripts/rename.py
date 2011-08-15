#!/usr/bin/python

import os, re, string

names  = {'8_101_46': 'a',
          '16_101_46': 'b',
          '8_116_46': 'c',
          '16_116_46': 'd',
          '8_131_46': 'e',
          '16_131_46': 'f',
          '8_101_61': 'g',
          '16_101_61': 'h',
          '8_116_61': 'i',
          '16_116_61': 'j',
          '8_131_61': 'k',
          '16_131_61': 'l',
          '8_146_61': 'u',
          '16_46_61': 'v',
          '8_101_76': 'm',
          '16_101_76': 'n',
          '8_116_76': 'o',
          '16_116_76': 'p',
          '8_131_76': 'q',
          '16_131_76': 'r',
          '8_146_61': 'u',
          '16_146_61': 'v',
          '8_146_76': 's',
          '16_146_76': 't'
          }

for f in os.listdir("."):
    for k, v in names.iteritems():
        pattern = "e2198_s" + k + ".*"
        if re.match(pattern, f):
            oldf = f
            newf = "e2198_" + v + "_" + string.replace(f, "e2198_", "")
            print "renaming: " + oldf + " to " + newf
            os.rename(oldf, newf)



