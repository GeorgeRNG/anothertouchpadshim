Because I don't know C, this is the complex solution I've made to getting to build on ubuntu, but should work on anything, this should only require docker:
```bash
docker run --interactive name=touchpadshim archlinux
pacman --noconfirm -Sy git base-devel libevdev
ln -s /usr/include/libevdev-1.0/libevdev /usr/include/libevdev
cd
git clone https://github.com/GeorgeRNG/anothertouchpadshim # or whatever fork you are using, if that ever happens.
cd anothertouchpadshim
gcc main.c -levdev -o touchpadshim
exit
docker cp touchpadshim:/root/anothertouchpadshim/touchpadshim .
docker rm touchpadshim
```
