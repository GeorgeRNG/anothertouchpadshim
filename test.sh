#! /usr/bin/bash
# this is used to rapidly test on my device, your touchpad probably has a different file.
# just compile it with the gcc command, put it somewhere you like, and run it for your touchpad
# you can use `evtest` to easily list your devices.
gcc main.c -levdev -o funnytouchpad
sudo ./funnytouchpad /dev/input/event7
rm funnytouchpad
