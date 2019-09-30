@echo off

start ../encode.exe  --o './sh-mc-linear.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='6' ^
    --samples='24000' ^
    --method 'monte-carlo' ^
    --filtering 'linear'

start ../encode.exe  --o './sh-mc-nearest.json' ^
    --px './assets/cubemap/posx.jpg' ^
    --nx './assets/cubemap/negx.jpg' ^
    --py './assets/cubemap/posy.jpg' ^
    --ny './assets/cubemap/negy.jpg' ^
    --pz './assets/cubemap/posz.jpg' ^
    --nz './assets/cubemap/negz.jpg' ^
    --order='6' ^
    --samples='24000' ^
    --method 'monte-carlo' ^
    --filtering 'nearest'