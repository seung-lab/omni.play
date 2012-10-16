import tarfile
import os
import sys
import shutil
import urllib

def ensure_dir(f):
    if not dir_exists(f):
        try:
            #print "making folder: ", f
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
        #print "checking folder exists: {d}...{e}".format( d=d, e=e)
        return e
    except:
        print "error checking existence of " + f
        raise

# note: racey, since file could (dis)appear after call...
def file_exists(fnp):
    e = os.path.isfile(fnp)
    #print "checking file exists: {fnp}...{e}".format(fnp=fnp, e=e)
    return e

def folder_exists(fnp):
    e = os.path.isdir(fnp)
    return e

def rm_f(f):
    if dir_exists(f):
        print "deleting folder: ", f,
        shutil.rmtree(f)

def gunzip(fnp, fp):
    print "unzipping: ", fnp
    tar = tarfile.open(fnp)
    tar.extractall(fp)
    tar.close()

def wget(uri, fnp):
    urllib.urlretrieve(uri, fnp)

def svn_co(uri, fnp):
    cmd = "svn co {remote} {local}".format(remote = uri, local = fnp)
    os.system(cmd)

def svn_up(fnp):
    cmd = "svn up {local}".format(local = fnp)
    os.system(cmd)

def uri_size_bytes(uri):
    # stackoverflow.com/questions/5909/get-size-of-a-file-before-downloading-in-python
    if uri.startswith("ftp"):
        return uri_size_bytes_ftp(uri)
    return uri_size_bytes_http(uri)

def uri_size_bytes_ftp(uri):
    # http://stackoverflow.com/questions/3231910/python-ftplib-cant-get-size-of-file-before-download
    raise Exception("http uri preferred")
    ftp.sendcmd("TYPE i")    # Switch to Binary mode
    return ftp.size(uri)   # Get size of file

def uri_size_bytes_http(uri):
    site = urllib.urlopen(uri)
    try:
        headers = site.info().getheaders("Content-Length")
        if not headers:
            raise Exception("no headers for " + uri)
        return int(headers[0])
    except:
        print headers
        raise

def check_expected_wget_file_size(uri, fnp):
    if not os.path.exists(fnp):
        return False

    fs = os.path.getsize(fnp)
    web = uri_size_bytes(uri)
    same_size = fs == web
    if not same_size:
        print "size mismatch({0} {1})".format(fs, web)
    return same_size

def query_yes_no(question, default="yes"):
    """
    from stackoverflow.com/questions/3041986/python-command-line-yes-no-input

    Ask a yes/no question via raw_input() and return their answer.

    "question" is a string that is presented to the user.
    "default" is the presumed answer if the user just hits <Enter>.
        It must be "yes" (the default), "no" or None (meaning
        an answer is required of the user).

    The "answer" return value is one of "yes" or "no".
    """
    valid = {"yes":True,   "y":True,  "ye":True,
             "no":False,     "n":False}
    if default == None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while True:
        sys.stdout.write(question + prompt)
        choice = raw_input().lower()
        if default is not None and choice == '':
            return valid[default]
        elif choice in valid:
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' "\
                             "(or 'y' or 'n').\n")

def file_size_human_readable(num):
    # stackoverflow.com/questions/1094841/reusable-library-to-get-human-readable-version-of-file-size
    for x in ['bytes','KB','MB','GB']:
        if num < 1024.0:
            return "%3.1f%s" % (num, x)
        num /= 1024.0
    return "%3.1f%s" % (num, 'TB')
