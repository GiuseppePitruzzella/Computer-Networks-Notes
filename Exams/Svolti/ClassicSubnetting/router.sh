echo "Configurazione nodo ISP"
# Login
VBoxManage guestcontrol ISP run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ ISP
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol ISP run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up

# Set up address (using second-last IP address)
VBoxManage guestcontrol ISP run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.1.1.2/17 dev enp0s3
VBoxManage guestcontrol ISP run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.74/30 dev enp0s8

# Set up routing
# By default, inoltra verso R0 (LAN00)
VBoxManage guestcontrol ISP run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.135.73

# Set up packet-forwarding
VBoxManage guestcontrol ISP run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv4.ip_forward=1

echo "Configurazione nodo R0"
# Login
VBoxManage guestcontrol R0 run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ R0
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up

# Set up address (using second-last IP address)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.73/30 dev enp0s3
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.70/29 dev enp0s8

# Set up routing
# Se riceve un pacchetto per LAN02, allora inoltra verso R1 (LAN02)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.0.128.0/22 via 10.0.135.69
# Se riceve un pacchetto per LAN03, allora inoltra verso R1 (LAN03)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.0.135.0/26 via 10.0.135.69
# Se riceve un pacchetto per LAN04, allora inoltra verso R2 (LAN04)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.0.132.0/23 via 10.0.135.68
# Se riceve un pacchetto per LAN05, allora inoltra verso R2 (LAN05)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.0.134.0/24 via 10.0.135.68
# Altrimenti, inoltra verso ISP (LAN00)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.135.70

# Set up packet-forwarding
VBoxManage guestcontrol R0 run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv4.ip_forward=1

echo "Configurazione nodo R1"
# Login
VBoxManage guestcontrol R1 run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ R1
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s9 up

# Set up address (using second-last IP address)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.69/29 dev enp0s3
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.131.254/22 dev enp0s8
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.62/26 dev enp0s9

# Set up routing
# Se riceve un pacchetto per LAN04, allora inoltra verso R2 (LAN01)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.0.132.0/23 via 10.0.135.68
# Se riceve un pacchetto per LAN05, allora inoltra verso R2 (LAN01)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.0.134.0/24 via 10.0.135.68
# Altrimenti, inoltra verso R0 (LAN01)
VBoxManage guestcontrol R1 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.135.70

# Set up packet-forwarding
VBoxManage guestcontrol R1 run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv4.ip_forward=1

echo "Configurazione nodo R2"
# Login
VBoxManage guestcontrol R2 run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ R2
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s9 up

# Set up address (using second-last IP address)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.68/29 dev enp0s3
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.133.254/23 dev enp0s8
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.134.254/24 dev enp0s9

# Set up routing
# Se riceve un pacchetto per LAN02, allora inoltra verso R1 (LAN01)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.0.128.0/22 via 10.0.135.69
# Se riceve un pacchetto per LAN03, allora inoltra verso R1 (LAN01)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add 10.0.132.0/23 via 10.0.135.69
# Altrimenti, inoltra verso R0 (LAN01)
VBoxManage guestcontrol R2 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.135.70

# Set up packet-forwarding
VBoxManage guestcontrol R2 run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv4.ip_forward=1
