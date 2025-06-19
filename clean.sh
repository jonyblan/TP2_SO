# Limpia el proyecto a traves del container de Docker
docker start tpSO3
docker exec -ti tpSO3 make clean -C/root/Toolchain
docker exec -ti tpSO3 make clean -C/root
docker stop tpSO3