#!/usr/bin/env bash

paused=0
pause_key=" "

frames=($@)
frame_i=0
frame_count=$#

interval=${INTERVAL:-0}

do_restore_cursor=1
msg=

usage="
Space Pause
←/→   Prev/Next
↑/↓   Speed up/down
c     Compile & rerun
g     Goto
h     Help
q     Quit
"

run_frame() {
  show_msg
  data="$(./run.sh ${frames[frame_i]} "$data" 2>&1 >/dev/tty)"
  if [[ $? != 0 ]]; then
    clear_screen
    paused=1
		msg=Paused
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

save_cursor() {
  echo -n -e "\e[s"
}
restore_cursor() {
  echo -n -e "\e[u"
}
clear_screen() {
  echo -n -e "\e[2J\e[H"
}

show_msg() {
  save_cursor

  local last_lc=${#msg_lines[@]}
  mapfile -t msg_lines <<< "$data"$'\n\n'"$msg"
  local lc=${#msg_lines[@]}

  if [ $lc -lt $last_lc ]; then
    lc=$last_lc
  fi

  for ((i = 0; i < $lc; i++)); do
    echo -n -e "\e[$((i + 2));195H"
    printf "%-32s" "${msg_lines[i]}"
  done

  if [ $do_restore_cursor == 1 ]; then
    restore_cursor
  fi
}

input_msg() {
  do_restore_cursor=0
  msg=
  show_msg
  echo -n -e "\e[$((${#msg_lines[@]} + 1));195H"
  read -p "$1" $2
  restore_cursor
  do_restore_cursor=1
}

set_msg() {
  msg="$1"
  show_msg
}

append_msg() {
  set_msg "$msg"$'\n'"$1"
}

while true; do
  run_frame
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
        frame_i=$((frame_i - 1))
        if [ $frame_i -lt 1 ]; then
          frame_i=1
          set_msg "No prev"
        else
          set_msg "Prev"
        fi
        run_frame
        ;;
      ($'\e[C')
        frame_i=$((frame_i + 1))
        if [ $frame_i -gt $# ]; then
          frame_i=$#
          set_msg "No next"
        else
          set_msg "Next"
        fi
        run_frame
        ;;
      ($'\e[A')
        interval=$(bc <<< "$interval - 0.05")
        if [ $(bc <<< "$interval < 0") == 1 ]; then
          interval=0
        fi
        set_msg "Interval <- $interval s"
        ;;
      ($'\e[B')
        interval=$(bc <<< "$interval + 0.05")
        set_msg "Interval <- $interval s"
        ;;
      (c)
        set_msg "Compiling..."
        clear_screen
        if ./compile.sh -q; then
          append_msg "Compile OK"
          run_frame
        else
          append_msg "Compile Error"
        fi
        ;;
      (q)
        exit 0
        ;;
      (g)
        input_msg "Goto: " new_i

        if [[ $new_i =~ ^[0-9]+$ ]] && [ "$new_i" -gt 0 ] && [ "$new_i" -le $# ]; then
          frame_i=$((new_i - 1))
          msg="Goto $new_i"
          run_frame
        else
          set_msg "Invalid index"
        fi
        ;;
      (h)
        set_msg "$usage"
        ;;
    esac

    if [ $paused == 0 ]; then
      break
    fi

    get_key
  done

  frame_i=$((frame_i + 1))
done
