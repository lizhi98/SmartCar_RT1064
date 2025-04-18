#!/usr/bin/env bash

dir="$(dirname $0)"

if ! ./image_test.out "$dir/Data/Array/$1.dat"; then
	echo "[ERROR]"
	exit 1
fi
echo "[$1]"
