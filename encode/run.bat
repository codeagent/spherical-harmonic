@echo off

start .\encode.exe  --o './sh-mc-linear.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='2' ^
    --samples='1024' ^
    --method 'monte-carlo' ^
    --filtering 'linear'

start .\encode.exe  --o './sh-mc-nearest.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='2' ^
    --samples='1024' ^
    --method 'monte-carlo' ^
    --filtering 'nearest'

start .\encode.exe  --o './sh-spherical-linear.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='2' ^
    --samples='1024' ^
    --method 'spherical' ^
    --filtering 'linear'

start .\encode.exe  --o './sh-spherical-nearest.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='2' ^
    --samples='1024' ^
    --method 'spherical' ^
    --filtering 'nearest'


start .\encode.exe  --o './sh-cubemap-linear.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='2' ^
    --method 'cubemap'

start .\encode.exe  --o './sh-cubemap-nearest.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='2' ^
    --method 'cubemap'