make clean ; make -j4 ; echo $?
md5sum cs-hud > cs-hud.md5
echo "CSO=$(git rev-parse HEAD)" > cs-hud.version
