import tarfile
import os
import shutil
import urllib

def ensure_dir(f):
    if not dir_exists(f):
        try:
            os.makedirs(f)
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
