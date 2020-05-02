#!/usr/bin/env bash

CONFIG_DIR=~/.config/line-tool/
from_file=${CONFIG_DIR}"from"
to_file=${CONFIG_DIR}"to"
save_file=${CONFIG_DIR}"save"

if [ ! -d $CONFIG_DIR ]; then
    mkdir -p $CONFIG_DIR
fi

if [ "$1" == "from" ]; then
    echo capturing from to $from_file
    get_cursor > $from_file
    highlight-point $(cat $from_file) red 100
elif [ "$1" == "to" ]; then
    echo capturing to to $to_file
    get_cursor > $to_file
    highlight-point $(cat $to_file) green 100
elif [ "$1" == "save" ]; then
    echo saving position $save_file
    get_cursor > $save_file
    highlight-point $(cat $save_file) violet 100
elif [ "$1" == "restore" ]; then
    echo restoring position $save_file
    xdotool mousemove $(cat $save_file)
    highlight-point $(cat $save_file) yellow 100
elif [ "$1" == "show" ]; then
    draw_line $(cat $from_file) $(cat $to_file) black 500
fi

