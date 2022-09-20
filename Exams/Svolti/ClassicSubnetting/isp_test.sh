echo "Configurazione nodo A"
# Login
VBoxManage guestcontrol A run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ A

# Set up address
VBoxManage guestcontrol A run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 2::FFF0:0059:0011/124 dev enp0s3

# Set up routing
# By default, send packets to R0 (LAN01)
VBoxManage guestcontrol A run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 2::FFF0:0059:0019

echo "Configurazione nodo X"
# Login
VBoxManage guestcontrol X run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ X

# Set up address
VBoxManage guestcontrol X run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 4::AA:BBCC/64 dev enp0s3

# Set up routing
# By default, send packets to R0 (LAN01)
VBoxManage guestcontrol X run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 4::AA:BBCD

echo "Configurazione nodo ISP"
# Login
VBoxManage guestcontrol ISP run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ ISP
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol ISP run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up

# Set up address (using second-last IP address)
VBoxManage guestcontrol ISP run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 4::AA:BBCD/64 dev enp0s3 # LANISP
VBoxManage guestcontrol ISP run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 2::FFF0:0059:0008/124 dev enp0s8 # LAN00

# Set up routing
# Altrimenti, inoltra verso R0 (LAN00)
VBoxManage guestcontrol ISP run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 2::FFF0:0059:0009

# Set up packet-forwarding
VBoxManage guestcontrol ISP run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv6.conf.all.forwarding=1

echo "Configurazione nodo R0"
# Login
VBoxManage guestcontrol R0 run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ R0
# Set up interfaces (> enp0s3)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ link set enp0s8 up

# Set up address (using second-last IP address)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 2::FFF0:0059:0009/124 dev enp0s3
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 2::FFF0:0059:0019/124 dev enp0s8

# Set up routing
# Altrimenti, inoltra verso ISP (LAN00)
VBoxManage guestcontrol R0 run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 2::FFF0:0059:0008

# Set up packet-forwarding
VBoxManage guestcontrol R0 run --exe /usr/sbin/sysctl --username root --password root --wait-stdout -- sysctl/ -w net.ipv6.conf.all.forwarding=1

echo "ping A -> X"
VBoxManage guestcontrol X run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 2::FFF0:0059:0011