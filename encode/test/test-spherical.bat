@echo off

start ../encode.exe  --o './sh-spherical-linear.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='6' ^
    --samples='24000' ^
    --method 'spherical' ^
    --filtering 'linear'

start ../encode.exe  --o './sh-spherical-nearest.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='6' ^
    --samples='24000' ^
    --method 'spherical' ^
    --filtering 'nearest'
