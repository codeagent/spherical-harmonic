@echo off

start ../encode.exe  --o './sh-cubemap.json' ^
    --px './assets/cubemap-128x128/posx.jpg' ^
    --nx './assets/cubemap-128x128/negx.jpg' ^
    --py './assets/cubemap-128x128/posy.jpg' ^
    --ny './assets/cubemap-128x128/negy.jpg' ^
    --pz './assets/cubemap-128x128/posz.jpg' ^
    --nz './assets/cubemap-128x128/negz.jpg' ^
    --order='6' ^
    --method 'cubemap'