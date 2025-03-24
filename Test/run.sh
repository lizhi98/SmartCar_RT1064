#!/usr/bin/env bash

try-clear () {
	if [ -z "$NOCLEAR" ]; then
		clear
	fi
}
cat image.c | sed "s/#include.*FAKE DATA/#include \".\\/Data\\/C\\/$1.c\"/" > image.tmp.c
dir=$(dirname $PWD)
gcc_option="-I $dir/Main/project/user/inc -l m -D IMAGE_DEBUG"
if [[ -z "$DEBUG" ]]; then
	if [[ -z "$REPEAT" ]]; then
		gcc $gcc_option image.tmp.c ../Main/project/user/src/mt_image.c -o image.tmp.out && try-clear && ./image.tmp.out
	else
		gcc $gcc_option image.tmp.c ../Main/project/user/src/mt_image.c -o image.tmp.out && try-clear && {
			START_TIME=$(date +%s%N)
			for i in $(seq 1 $REPEAT); do
				if [[ $i = 1 ]]; then
					./image.tmp.out
				else
					./image.tmp.out > /dev/null
				fi
			done
			END_TIME=$(date +%s%N)
			AVG_TIME=$(echo "scale=2; ($END_TIME - $START_TIME) / 1000000 / $REPEAT" | bc)
			echo "[avg time] ${AVG_TIME} ms"
		}
	fi
else
	gcc $gcc_option image.tmp.c ../Main/project/user/src/mt_image.c -g -o image.tmp.out && try-clear && gdb ./image.tmp.out
fi
if [[ $? != 0 ]]; then
	echo "[$1 ERROR]"
	exit 1
fi
echo "[$1]"
