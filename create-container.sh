# Crea el container de Docker y setea la carpeta actual como compartida con el mismo
docker run -d -v ${PWD}:/root --security-opt seccomp:unconfined -ti --name tpSO3 agodio/itba-so:1.0