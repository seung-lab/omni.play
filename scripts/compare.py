#!/usr/bin/python

import os, sys, difflib, time, errno, shutil

def mkdir_p(path):
    try:
        os.makedirs(path)
    except OSError as exc:
        if exc.errno == errno.EEXIST and os.path.isdir(path):
            pass
        else:
            raise

inpath = "desktop/src/gui"
outPath = "/home/mjp/htmldiff"
p = os.path.join(outPath, inpath)
if os.path.exists(p):
    shutil.rmtree(p)
mkdir_p(p)

def makeDiff(fnp, ofnp):
    context_lines = 3
    fromdate = time.ctime(os.stat(ofnp).st_mtime)
    todate = time.ctime(os.stat(fnp).st_mtime)
    fromlines = open(ofnp, 'U').readlines()
    tolines = open(fnp, 'U').readlines()

    if fromlines == tolines:
        return False, ""

    diff = difflib.HtmlDiff().make_file(fromlines, tolines, ofnp,
                                        fnp, context=False,
                                        numlines=context_lines)
    hfnp = fnp.replace("omni.staging", "htmldiff")
    hfnp += ".html"
    d = os.path.dirname(hfnp)
    mkdir_p(d)
    with open(hfnp, "w") as f:
        f.write(diff)
    return True, hfnp

def makeIndex(diffs):
    fnp = os.path.join(outPath, "index.html")
    with open(fnp, "w") as f:
        f.write("<html><body><ul>")
        for d in diffs:
            d = d.replace(outPath + "/", "")
            print d
            f.write('<li><a href="{f}">{name}</a></li>'.format(f=d, name=d))
        f.write("</ul></body></html>")

ignorePostFix = ["~", ".moc.cpp", ".qrc", ".rcc.cpp"]

def ignoreFile(fn):
    for pf in ignorePostFix:
        if fn.endswith(pf):
            return True
    return False

def run():
    d = os.path.dirname(os.path.abspath(__file__))
    d = os.path.join(d, "../")
    path = os.path.join("{d}/{f}".format(d=d, f=inpath))

    diffs = []

    for root, dirs, files in os.walk(path):
        for fn in files:
            if ignoreFile(fn):
                continue
            fnp = os.path.abspath(os.path.join(root, fn))
            ofnp = fnp.replace("omni.staging", "omni.dlr")
            if not os.path.exists(ofnp):
                print "missing", ofnp
                continue
            wasDiff, hfnp = makeDiff(fnp, ofnp)
            if wasDiff:
                print "difference in:", fnp, "to", ofnp
                diffs.append(hfnp)
    makeIndex(diffs)

run()
