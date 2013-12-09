#!/usr/bin/python

import os
import re

script_pn = os.path.dirname(os.path.abspath(__file__))
#desktop_pn = os.path.join(script_pn, "../desktop/src")
#desktop_pn = os.path.abspath(desktop_pn)
desktop_pn = "/home/mjp/omni.play/desktop/src"

cpps = []
headers = []

for dirpath, dnames, fnames in os.walk(desktop_pn):
    if "desktop/src/segment" in dirpath:
        pass #continue
    for f in fnames:
        fnp = [os.path.join(dirpath,f), f]
        if f.endswith(".cpp"):
            cpps.append(fnp)
        elif f.endswith(".h") or f.endswith(".hpp"):
            headers.append(fnp)

klasses = {}

for header in headers:
    fnp = header[0]
    mod = os.path.dirname(fnp).replace(desktop_pn, "").split("/")[1]
    found = 0
    with open(fnp) as f:
        lines = f.readlines()
        for line in lines:
            match = re.search(r"typedef .*(\b(?=\w)Om.*?\b(?!\w));", line)
            if not match:
                if ";" in line:
                    continue
                match = re.search(r"class (\b(?=\w).*?\b(?!\w))", line)
                if not match:
                    match = re.search(r"struct (\b(?=\w).*?\b(?!\w))", line)
                    if not match:
                        match = re.search(r"typedef .*(\b(?=\w)Om.*?\b(?!\w))<.*", line)
                        if not match:
                            continue
            klass = match.group(1)
            if not klass.startswith("Om"):
                continue
            found += 1
            k = klass.replace("Om", "")
            newk = "om::" + mod + "::" + k
            klasses[klass] = [mod, newk, k]
            #print k, "to", newk

for header in headers:
    fnp = header[0]
    if not os.path.basename(fnp).startswith("om"):
        continue
    bak = fnp # + ".bak"
    mod = os.path.dirname(fnp).replace(desktop_pn, "").split("/")[1]
    found = 0
    with open(fnp) as f:
        lines = f.readlines()
    with open(bak, "w") as b:
        innamespace = False
        for line in lines:
            if not innamespace and not "class" in line and not "struct" in line:
                b.write(line)
                continue
            match = re.search(r"class (\b(?=\w)Om.*?\b(?!\w));", line)
            if match:
                klass = match.group(1)
                if klass in klasses:
                    line = line.replace(klass, klasses[klass][2])
                    m = klasses[klass][0]
                    b.write("namespace om { ")
                    b.write("namespace " + m + " { ")
                    b.write(line.rstrip())
                    b.write(" }}\n")
                    continue

            match = re.search(r"class (\b(?=\w)Om.*?\b(?!\w))", line)
            if match:
                if not innamespace:
                    b.write("namespace om {\n")
                    b.write("namespace " + mod + " {\n\n")
                    innamespace = True
            else:
                match = re.search(r"struct (\b(?=\w)Om.*?\b(?!\w))", line)
                if match:
                    if not innamespace:
                        b.write("namespace om {\n")
                        b.write("namespace " + mod + " {\n\n")
                        innamespace = True
            for match in re.finditer(r"(\b(?=\w)Om.*?\b(?!\w))", line):
                klass = match.group(1)
                if klass in klasses:
                    if mod == klasses[klass][0]:
                        line = line.replace(klass, klasses[klass][2])
                    else:
                        line = line.replace(klass, klasses[klass][1])
            b.write(line)
        b.write("}} // om::" + mod + "::\n")

for cpp in cpps:
    fnp = cpp[0]
    if not os.path.basename(fnp).startswith("om"):
        continue
    bak = fnp # + ".bak"
    if '/home/big/5.new/home/mjp/omni.play/desktop/src/main.cpp' == cpp[0]:
        continue
    mod = os.path.dirname(fnp).replace(desktop_pn, "").split("/")[1]
    with open(fnp) as f:
        lines = f.readlines()
    with open(bak, "w") as b:
        innamespace = False
        for line in lines:
            if not innamespace and not "Om" in line:
                b.write(line)
                continue
            if not innamespace:
                b.write("namespace om {\n")
                b.write("namespace " + mod + " {\n\n")
                innamespace = True
            for match in re.finditer(r"(\b(?=\w)Om.*?\b(?!\w))", line):
                klass = match.group(1)
                if klass in klasses:
                    if mod == klasses[klass][0]:
                        line = line.replace(klass, klasses[klass][2])
                    else:
                        line = line.replace(klass, klasses[klass][1])
            b.write(line)
        b.write("\n}} // om::" + mod + "::\n")
