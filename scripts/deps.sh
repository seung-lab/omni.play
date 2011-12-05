#!/bin/bash

sudo apt-get update
sudo apt-get -y install apache2 php5 memcached libevent-2.0-5 git make g++ qt4-qmake libssl-dev nasm


if [[ -z $(git config --get user.name) ]]
then
    git config --global user.name "Scripted Deployment"
    git config --global user.email "balkamm@gmail.com"

# Add ssh key for Github if it's not there
    if [[ -z $(grep "MIIEpQIBAAKCAQEA7AnDs8RHramTUJxYouDVt5I8oWQkLmgVki4sdQnHHv" ~/.ssh/id_rsa) ]]
    then

        echo \
            "-----BEGIN RSA PRIVATE KEY-----
MIIEpQIBAAKCAQEA7AnDs8RHramTUJxYouDVt5I8oWQkLmgVki4sdQnHHv+YZ1U6
Qi32Plndqn03AQeLmbbIHruaz//i+D7qdXiFL4+6ImJcEvnImgVGbL6bzcq6GDoe
3tdPa7n0MCPHkjNSlKR6PQIB28FL+9VdAHTEaKB92GOPeUpV73BR/+UoolIlECam
pgAmwAn/II4/6hbSlQFThjPgLg2b9IDr0YtIgcswmHLWYxEj5R0IZ6JV87CbnENO
ZH+Lw4PdJgBJE4UHxxzhkgb6eJenFQBVki3LxyhY0CXbYRjpygaFWaziJN5SbOFV
FeBY2Wk/GUCgdyOZXJ/I265lJw4XWEn/OC6Y/wIDAQABAoIBAQC2162IwMBBr32V
UTd8HKyRQi/DWX6JkEk9bpUzqHpd8eLOEQBLKa15iUWj/tEv8/dZAHUvEWaQP4lD
ggE/NDf/qtemMQlgDawuW229aT3qAWN8xvEI8ELukSKuc9jx0N3klTC3t6lW4PqK
PzKG2u0Wm1RA3obSjxnYm+p5TrvWWEy3yNIlt8h+xSUAE1XEdcabpNmRA1pVwJeI
dv2Mt37zpyL/kAuf+0HkpctkRuyiUf0y6vYjPFxcwbaS+KOiXL0xCOZOw55DnFyQ
/6dnWLevGvgK+pIE1cwsJ1Pg5dBNvAkNophsRc58n0hPkQZI/twBvXDDqr1GA6XS
tHDnR5vBAoGBAPvc7K3oHxRdYBXWMntqv9hxLx+0gWCPRv1tdFmAy/VB2KRUe+ob
+C1EfW98AbH355ZDln1eaSQctsnlJMI4iF/nUOcb7Vg2m6ag6YmcGCaRmtQbEIJD
n+hAHTjQ4SCveQGx4n47bkYtWbvWyZfvKcRaKSZC7Azgu6dk9mKGx7XhAoGBAO/q
TAeaztTcwhEUs/ltEpLCLt68OeBtq8K7wyU/39tipobYndGGvxLloOnMjuQRvN7b
OSsN3vAUavBIwNP1HGqlxUZj4Fqw/XK3z4gyvKR6mTTcxBNnTtrAPP8jgX8maofQ
HF0ZasCSFQEns7O/wcXg8AoSPnmSkTV/2UOr6GrfAoGARXqRc/wHrtd/bSa0sGUn
XS8D/+wMkLWVsM2j/hjQquyqNdL0iAWFNf+h1sJPuWeY5FPqzNThjku5hQKIXGe5
/zLRl5+VnvMZhyL2qbZ81w7ttjttF3JOR4SASy8cG6csa3JF9+qvJf7WtUGwRRB9
5iO1HE/zzC2mtqvcaBukNiECgYEApjpQM3Svmgza6brOeCN6mvSwRONltnpGZ8Zw
j36SPA8+EnBD6MjT2uP/ef6hgqRHmKGMGqihrvZ2ocXG1RUfe0UgSoUpDa1T7ggt
YtDEo7HONTJBKLCLkkVf/ec78j1L0o3/GQJjVMnig8peWEykldYketXpM5K5yber
cZfKkUECgYEAszHdgFbcmDxvciKokoL5YnRKAsiU406Xe/Cje7mBDJ05dnwzdW9C
R5/NFkKGOWOl5DC5Pf+lfFuv8RsYN+EXqvgio2GlYgu4A/29vp89Por2xp3c4C9B
XX5nYiF3Zm4Y6RGoYymvNsf3C1ikp9CoYgMNQJn+0aF5WXMCv5PML2A=
-----END RSA PRIVATE KEY-----" >> ~/.ssh/id_rsa

        chmod 700 ~/.ssh/id_rsa

    fi
fi

mkdir omni
cd omni
git clone git@github.com:seung-lab/omni.server.git
git clone git@github.com:seung-lab/omni.common.git
git clone git@github.com:seung-lab/omni-web.git

cd omni.common
./bootstrap.pl 3
qmake
make -j16 && make install
cp bin/libomni.common.a lib/bin

cd ../omni.server
./bootstrap.pl 1
qmake -set COMMON_PATH ../omni.common
qmake
make -j16