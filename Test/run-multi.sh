#!/usr/bin/env bash
for f in $@; do
	./run.sh $f
	if [[ $? != 0 ]]; then
		break
	fi
	sleep ${INTERVAL:-0}
done
