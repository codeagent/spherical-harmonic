@echo off

start ../decode.exe  --i './sh-rgb.json' --o './test-write' --prefix='png-' --format='png' --size '64'
start ../decode.exe  --i './sh-rgb.json' --o './test-write' --prefix='bmp-' --format='bmp' --size '64'
start ../decode.exe  --i './sh-rgb.json' --o './test-write' --prefix='tga-' --format='tga' --size '64'
start ../decode.exe  --i './sh-rgb.json' --o './test-write' --prefix='jpg-' --format='jpg' --size '64'
start ../decode.exe  --i './sh-rgb.json' --o './test-write' --prefix='hdr-' --format='hdr' --size '64'
