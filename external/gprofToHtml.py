"""
from http://www.java2s.com/Code/Python/Utility/Transformgprof1outputintousefulHTML.htm
PYTHON SOFTWARE FOUNDATION LICENSE VERSION 2
--------------------------------------------

1. This LICENSE AGREEMENT is between the Python Software Foundation
("PSF"), and the Individual or Organization ("Licensee") accessing and
otherwise using this software ("Python") in source or binary form and
its associated documentation.

2. Subject to the terms and conditions of this License Agreement, PSF
hereby grants Licensee a nonexclusive, royalty-free, world-wide
license to reproduce, analyze, test, perform and/or display publicly,
prepare derivative works, distribute, and otherwise use Python
alone or in any derivative version, provided, however, that PSF's
License Agreement and PSF's notice of copyright, i.e., "Copyright (c)
2001, 2002, 2003, 2004 Python Software Foundation; All Rights Reserved"
are retained in Python alone or in any derivative version prepared
by Licensee.

3. In the event Licensee prepares a derivative work that is based on
or incorporates Python or any part thereof, and wants to make
the derivative work available to others as provided herein, then
Licensee hereby agrees to include in any such work a brief summary of
the changes made to Python.

4. PSF is making Python available to Licensee on an "AS IS"
basis.  PSF MAKES NO REPRESENTATIONS OR WARRANTIES, EXPRESS OR
IMPLIED.  BY WAY OF EXAMPLE, BUT NOT LIMITATION, PSF MAKES NO AND
DISCLAIMS ANY REPRESENTATION OR WARRANTY OF MERCHANTABILITY OR FITNESS
FOR ANY PARTICULAR PURPOSE OR THAT THE USE OF PYTHON WILL NOT
INFRINGE ANY THIRD PARTY RIGHTS.

5. PSF SHALL NOT BE LIABLE TO LICENSEE OR ANY OTHER USERS OF PYTHON
FOR ANY INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES OR LOSS AS
A RESULT OF MODIFYING, DISTRIBUTING, OR OTHERWISE USING PYTHON,
OR ANY DERIVATIVE THEREOF, EVEN IF ADVISED OF THE POSSIBILITY THEREOF.

6. This License Agreement will automatically terminate upon a material
breach of its terms and conditions.

7. Nothing in this License Agreement shall be deemed to create any
relationship of agency, partnership, or joint venture between PSF and
Licensee.  This License Agreement does not grant permission to use PSF
trademarks or trade name in a trademark sense to endorse or promote
products or services of Licensee, or any third party.

8. By copying, installing or otherwise using Python, Licensee
agrees to be bound by the terms and conditions of this License
Agreement.
"""

#! /usr/bin/env python2.3

"""Transform gprof(1) output into useful HTML."""

import re, os, sys, cgi, webbrowser

header = """\
<html>
<head>
  <title>gprof output (%s)</title>
</head>
<body>
<pre>
"""

trailer = """\
</pre>
</body>
</html>
"""

def add_escapes(input):
    for line in input:
        yield cgi.escape(line)

def main():
    filename = "gprof.out"
    if sys.argv[1:]:
        filename = sys.argv[1]
    outputfilename = filename + ".html"
    input = add_escapes(file(filename))
    output = file(outputfilename, "w")
    output.write(header % filename)
    for line in input:
        output.write(line)
        if line.startswith(" time"):
            break
    labels = {}
    for line in input:
        m = re.match(r"(.*  )(\w+)\n", line)
        if not m:
            output.write(line)
            break
        stuff, fname = m.group(1, 2)
        labels[fname] = fname
        output.write('%s<a name="flat:%s" href="#call:%s">%s</a>\n' %
                     (stuff, fname, fname, fname))
    for line in input:
        output.write(line)
        if line.startswith("index % time"):
            break
    for line in input:
        m = re.match(r"(.*  )(\w+)(( &lt;cycle.*&gt;)? \[\d+\])\n", line)
        if not m:
            output.write(line)
            if line.startswith("Index by function name"):
                break
            continue
        prefix, fname, suffix = m.group(1, 2, 3)
        if fname not in labels:
            output.write(line)
            continue
        if line.startswith("["):
            output.write('%s<a name="call:%s" href="#flat:%s">%s</a>%s\n' %
                         (prefix, fname, fname, fname, suffix))
        else:
            output.write('%s<a href="#call:%s">%s</a>%s\n' %
                         (prefix, fname, fname, suffix))
    for line in input:
        for part in re.findall(r"(\w+(?:\.c)?|\W+)", line):
            if part in labels:
                part = '<a href="#call:%s">%s</a>' % (part, part)
            output.write(part)
    output.write(trailer)
    output.close()
    webbrowser.open("file:" + os.path.abspath(outputfilename))

if __name__ == '__main__':
    main()
