echo "Configurazione nodo A"
# Login
VBoxManage guestcontrol A run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ A
# Set up address
VBoxManage guestcontrol A run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.128.0.1/11 dev enp0s3
# Set up routing
VBoxManage guestcontrol A run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.128.7.254

echo "Configurazione nodo B"
# Login
VBoxManage guestcontrol B run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ B
# Set up address
VBoxManage guestcontrol B run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.128.0.2/11 dev enp0s3
# Set up routing
VBoxManage guestcontrol B run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.128.7.254

echo "Configurazione nodo C"
# Login
VBoxManage guestcontrol C run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ C
# Set up address
VBoxManage guestcontrol C run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.209.0.1/16 dev enp0s3
# Set up routing
VBoxManage guestcontrol C run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.209.255.254

echo "Configurazione nodo D"
# Login
VBoxManage guestcontrol D run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ D
# Set up address
VBoxManage guestcontrol D run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.1/16 dev enp0s3
# Set up routing
VBoxManage guestcontrol D run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.210.255.254

echo "Configurazione nodo E"
# Login
VBoxManage guestcontrol E run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ E
# Set up address
VBoxManage guestcontrol E run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.160.0.1/11 dev enp0s3
# Set up routing
VBoxManage guestcontrol E run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.160.7.254

echo "Configurazione nodo F"
# Login
VBoxManage guestcontrol F run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ F
# Set up address
VBoxManage guestcontrol F run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.160.0.2/11 dev enp0s3
# Set up routing
VBoxManage guestcontrol F run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.160.7.254

echo "Configurazione nodo G"
# Login
VBoxManage guestcontrol G run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ G
# Set up address
VBoxManage guestcontrol G run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.192.0.1/12 dev enp0s3
# Set up routing
VBoxManage guestcontrol G run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.192.15.254

echo "Configurazione nodo R1"
# Login
VBoxManage guestcontrol R1 run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ R1
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s9 up
# Set up address
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.128.7.254/11 dev enp0s3
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.1/30 dev enp0s8
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.5/30 dev enp0s9
# Set up routing
# Se riceve un pacchetto per LAN04, allora inoltra verso R2 (LAN02)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.209.0.0/16 via 10.210.0.2
# Se riceve un pacchetto per LAN05, allora inoltra verso R2 (LAN02)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.210.0.0/16 via 10.210.0.2
# Se riceve un pacchetto per LAN08, allora inoltra verso R2 (LAN02)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.160.0.0/11 via 10.210.0.2
# Se riceve un pacchetto per LAN08, allora inoltra verso R3 (LAN03)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.160.0.0/11 via 10.210.0.6
# Se riceve un pacchetto per LAN09, allora inoltra verso R3 (LAN02)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.192.0.0/12 via 10.210.0.6
# Set up packet-forwarding
VBoxManage guestcontrol R1 run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv4.ip_forward=1

echo "Configurazione nodo R2"
# Login
VBoxManage guestcontrol R2 run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ R2
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s9 up
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s10 up
# Set up address
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.2/30 dev enp0s3
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.209.255.254/16 dev enp0s8
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.255.254/16 dev enp0s9
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.9/30 dev enp0s10
# Set up routing
# Se riceve un pacchetto per LAN01, allora inoltra verso R1 (LAN02)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.128.0.0/11 via 10.210.0.1
# Se riceve un pacchetto per LAN08, allora inoltra verso R4 (LAN06)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.160.0.0/11 via 10.210.0.10
# Se riceve un pacchetto per LAN09, allora inoltra verso R4 (LAN06)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.192.0.0/12 via 10.210.0.10
# Se riceve un pacchetto per LAN09, allora inoltra verso R1 (LAN02)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.192.0.0/12 via 10.210.0.1
# Set up packet-forwarding
VBoxManage guestcontrol R2 run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv4.ip_forward=1

echo "Configurazione nodo R3"
# Login
VBoxManage guestcontrol R3 run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ R3
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s9 up
# Set up address
# LAN03
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.6/30 dev enp0s3
# LAN09
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.192.15.254/12 dev enp0s8
# LAN07
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.13/30 dev enp0s9
# Set up routing
# Se riceve un pacchetto per LAN01, allora inoltra verso R1 (LAN03)
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.128.0.0/11 via 10.210.0.5
# Se riceve un pacchetto per LAN04, allora inoltra verso R1 (LAN03)
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.209.0.0/16 via 10.210.0.5
# Se riceve un pacchetto per LAN05, allora inoltra verso R1 (LAN03)
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.210.0.0/16 via 10.210.0.5
# Se riceve un pacchetto per LAN04, allora inoltra verso R4 (LAN07)
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.209.0.0/16 via 10.210.0.14
# Se riceve un pacchetto per LAN05, allora inoltra verso R4 (LAN07)
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.210.0.0/16 via 10.210.0.14
# Se riceve un pacchetto per LAN08, allora inoltra verso R4 (LAN07)
VBoxManage guestcontrol R3 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.160.0.0/11 via 10.210.0.14
# Set up packet-forwarding
VBoxManage guestcontrol R3 run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv4.ip_forward=1

echo "Configurazione nodo R4"
# Login
VBoxManage guestcontrol R4 run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ R4
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s9 up
# Set up address
# LAN06
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.10/30 dev enp0s3
# LAN08
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.160.7.254/11 dev enp0s8
# LAN07
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.210.0.14/30 dev enp0s9
# Set up routing
# Se riceve un pacchetto per LAN01, allora inoltra verso R2 (LAN06)
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.128.0.0/11 via 10.210.0.9
# Se riceve un pacchetto per LAN01, allora inoltra verso R3 (LAN07)
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.128.0.0/11 via 10.210.0.13
# Se riceve un pacchetto per LAN09, allora inoltra verso R3 (LAN07)
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.192.0.0/12 via 10.210.0.13
# Se riceve un pacchetto per LAN04, allora inoltra verso R2 (LAN06)
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.209.0.0/16 via 10.210.0.9
# Se riceve un pacchetto per LAN05, allora inoltra verso R2 (LAN06)
VBoxManage guestcontrol R4 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.210.0.0/16 via 10.210.0.9
# Set up packet-forwarding
VBoxManage guestcontrol R4 run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv4.ip_forward=1