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

        e = os.path.exists(d)
        print "checking folder exists: {d}...{e}".format( d=d, e=e)
        return e
    except:
        print "error checking existence of " + f
        raise

# note: racey, since file could (dis)appear after call...
def file_exists(fnp):
    e = os.path.isfile(fnp)
    print "checking file exists: {fnp}...{e}".format(fnp=fnp, e=e)
    return e

def rm_f(f):
    if dir_exists(f):
        print "deleting folder: ", f
        shutil.rmtree(f)

def gunzip(fnp, fp):
    print "unzipping: ", fnp
    tar = tarfile.open(fnp)
    tar.extractall(fp)
    tar.close()

def wget(uri, fnp):
    urllib.urlretrieve(uri, fnp)
