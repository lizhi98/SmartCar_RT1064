#!/usr/bin/env bash

dir="$(dirname $0)"

if ! ./image_test.out "$dir/Data/Array/$1.dat" "$2" "$3"; then
	echo "[$1 ERROR]" >&2
	exit 1
fi
echo "[$1]" >&2
