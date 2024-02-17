# Arch (or maybe just endevouros) installs libevdev at the wrong place. This is a bad idea, probably. However, I want my code to run, so here we are.
sudo unlink /usr/include/libevdev
sudo ln -s /usr/include/libevdev-1.0/libevdev /usr/include/libevdev