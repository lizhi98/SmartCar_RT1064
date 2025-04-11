#!/usr/bin/env bash

dir="$(dirname $0)"

try-clear () {
	if [ -z "$NOCLEAR" ]; then
		clear
	fi
}

compile () {
	local project_root="$dir/../Main/project"
	gcc -I $project_root/user/inc -l m -D IMAGE_DEBUG image_test.c $project_root/user/src/mt_image.c -o ./image_test.out
}

if [ "$1" = -R ]; then
	shift
	rm -f ./image_test.out
fi

if [ ! -x ./image_test.out ]; then
	if ! compile; then
		echo "[COMPILE ERROR]"
		exit 1
	fi
fi

try-clear
if ! ./image_test.out "$dir/Data/Array/$1.dat"; then
	echo "[$1 ERROR]"
	exit 1
fi
echo "[$1]"
