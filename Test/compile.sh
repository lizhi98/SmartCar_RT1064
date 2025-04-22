#!/usr/bin/env bash

dir="$(dirname $0)"

compile () {
  local project_root="$dir/../Main/project"
  gcc -I $project_root/user/inc -D IMAGE_DEBUG image_test.c $project_root/user/src/mt_image.c -l m -o ./image_test.out
}

if [ -f ./image_test.out ]; then
  rm -f ./image_test.out
fi

if [ "$1" = -q ]; then
  compile
  exit $?
fi

if ! compile; then
  echo "[COMPILE ERROR]"
  exit 1
else
  echo "[COMPILE OK]"
fi
