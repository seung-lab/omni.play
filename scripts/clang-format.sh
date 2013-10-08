#!/bin/bash

DIRS="common/src common/test/src server/src server/test/src desktop/src desktop/test/src urtm/src valid/src"

find $DIRS -regex ".*\.[cChH]\(pp\)?" | parallel clang-format -i --style=Google
