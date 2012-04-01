import tarfile
import os
import shutil
import urllib

def ensure_dir(f):
    if not dir_exists(f):
        try:
            print "making folder: ", f
            os.makedirs(f)
        except:
            print "could not make folder " + f
            raise

def dir_exists(f):
    try:
        d = os.path.dirname(f)
        if not f.endswith("/"):
            d = os.path.dirname(f+"/")
        print "checking folder exists: ", d
        return os.path.exists(d)
    except:
        print "error checking existence of " + f
        raise

# note: racey, since file could disappear after call...
def file_exists(fnp):
    print "checking file exists: ", fnp
    return os.path.isfile(fnp)

def rm_f(f):
    if dir_exists(f):
        print "deleting folder: ", f
        shutil.rmtree(f)

def gunzip(fnp, fp):
    tar = tarfile.open(fnp)
    tar.extractall(fp)
    tar.close()

def wget(uri, fnp):
    urllib.urlretrieve(uri, fnp)
