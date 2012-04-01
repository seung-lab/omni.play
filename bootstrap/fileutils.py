import tarfile
import os
import shutil
import urllib

# http://stackoverflow.com/questions/273192/python-best-way-to-create-directory-if-it-doesnt-exist-for-file-write
def ensure_dir(f):
    d = os.path.dirname(f)
    if not f.endswith("/"):
        d = os.path.dirname(f+"/")
    if not os.path.exists(d):
        try:
            os.makedirs(d)
        except:
            print "could not make folder " + f
            raise

def dir_exists(f):
    d = os.path.dirname(f)
    if not f.endswith("/"):
        d = os.path.dirname(f+"/")
    return os.path.exists(d)

# note: racey, since file could disappear after call...
def file_exists(fnp):
    os.path.isfile(fnp)

def rm_f(f):
    shutil.rmtree(f)

def gunzip(fnp, fp):
    tar = tarfile.open(fnp)
    tar.extractall(fp)
    tar.close()

def wget(uri, fnp):
    urllib.urlretrieve(uri, fnp)
