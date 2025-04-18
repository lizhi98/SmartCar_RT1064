#!/usr/bin/env bash

paused=0
pause_key=" "

i=1
frame_count=$#

interval=${INTERVAL:-0}

run_frame() {
	show_msg
	./run.sh $1
	if [[ $? != 0 ]]; then
		exit 1
	fi
  show_msg
}

get_key() {
	local chr
	IFS= read $* -n 1 -s key
	if [ "$key" == $'\e' ]; then
		for k in {1..2}; do
			read -n 1 -s chr
			key="$key$chr"
		done
	fi
}

show_msg() {
	echo -n -e "\e[s\e[40;195H"
	echo -n "+------------------+"

	echo -n -e "\e[41;195H"
	echo -n "| "
	printf "%-16s" "$msg"
	echo -n " |"

	echo -n -e "\e[42;195H"
	echo -n "+------------------+"

	echo -n -e "\e[u"
}

set_msg() {
	msg="$1"
	show_msg
}

while true; do
	run_frame ${!i}
	sleep $interval
	
	get_key -t 0.1

	if [ "$key" == "$pause_key" ]; then
		paused=1
		set_msg "Paused"
		key=
	fi

	while true; do
		case "$key" in
			("$pause_key")
				paused=0
				set_msg
				;;
			($'\e[D')
				i=$((i - 1))
				if [ $i -lt 1 ]; then
					i=1
				fi
				run_frame ${!i}
				;;
			($'\e[C')
				i=$((i + 1))
				if [ $i -gt $# ]; then
					i=$#
				fi
				run_frame ${!i}
				;;
			($'\e[A')
				interval=$(bc <<< "$interval - 0.05")
				if [ $(bc <<< "$interval < 0") == 1 ]; then
					interval=0
				fi
				set_msg "Interval <- $interval"
				;;
			($'\e[B')
				interval=$(bc <<< "$interval + 0.05")
				set_msg "Interval <- $interval"
				;;
			(c)
				if ./compile.sh -q; then
					set_msg "Compile OK"
				else
					set_msg "Compile Error"
				fi
				run_frame ${!i}
				;;
			(q)
				exit 0
				;;
			(g)
				read -p "Goto: " new_i
				if [[ $input =~ ^[0-9]+$ ]] && [ "$new_i" -gt 0 ] && [ "$new_i" -le $# ]; then
					i=$new_i
					msg="Goto $new_i"
					run_frame ${!i}
				else
					set_msg "Invalid index"
				fi
		esac

		if [ $paused == 0 ]; then
			break
		fi

		get_key
	done

	i=$((i + 1))
done
