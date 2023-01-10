#!/bin/bash

URL="https://s21.filetransfer.io/storage/download/LJQ4Udzl4LSR"
FILENAME="LJQ4Udzl4LSR"

if ! command -v wget &> /dev/null; then
    echo "Please, install wget! `sudo apt install wget`"
    exit
fi

if ! command -v unzip &> /dev/null; then
    echo "Please, install unzip! `sudo apt install unzip`"
    exit
fi

wget $URL
unzip $FILENAME

if [[ $? == 0 ]] ; then
    echo "Success"
else
    echo "Please, try download again."
fi ;

rm -rf $FILENAME
