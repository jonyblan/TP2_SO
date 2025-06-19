# Compila el proyecto a traves del container de Docker
docker start tpSO3
docker exec -ti tpSO3 make clean -C/root/Toolchain
docker exec -ti tpSO3 make clean -C/root
docker exec -ti tpSO3 make all -C/root/Toolchain
docker exec -ti tpSO3 make all -C/root
docker exec -ti tpSO3 chown -R 1000:1000 /root
docker stop tpSO3