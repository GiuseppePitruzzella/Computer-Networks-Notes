echo "Configurazione nodo A"
# Login
VBoxManage guestcontrol A run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ A

# Set up address
VBoxManage guestcontrol A run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.65/29 dev enp0s3

# Set up routing
# By default, send packets to R0 (LAN01)
VBoxManage guestcontrol A run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.135.70

echo "Configurazione nodo B"
# Login
VBoxManage guestcontrol B run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ B

# Set up address
VBoxManage guestcontrol B run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.128.1/22 dev enp0s3

# Set up routing
# By default, send packets to R1 (LAN02)
VBoxManage guestcontrol B run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.131.254

echo "Configurazione nodo C"
# Login
VBoxManage guestcontrol C run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ C

# Set up address
VBoxManage guestcontrol C run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.128.2/22 dev enp0s3

# Set up routing
# By default, send packets to R1 (LAN02)
VBoxManage guestcontrol C run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.131.254

echo "Configurazione nodo D"
# Login
VBoxManage guestcontrol D run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ D

# Set up address
VBoxManage guestcontrol D run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.1/26 dev enp0s3

# Set up routing
# By default, send packets to R1 (LAN03)
VBoxManage guestcontrol D run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.135.62

echo "Configurazione nodo E"
# Login
VBoxManage guestcontrol E run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ E

# Set up address
VBoxManage guestcontrol E run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.135.2/26 dev enp0s3

# Set up routing
# By default, send packets to R1 (LAN03)
VBoxManage guestcontrol E run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.135.62

echo "Configurazione nodo F"
# Login
VBoxManage guestcontrol F run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ F

# Set up address
VBoxManage guestcontrol F run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.132.1/23 dev enp0s3

# Set up routing
# By default, send packets to R2 (LAN04)
VBoxManage guestcontrol F run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.133.254

echo "Configurazione nodo G"
# Login
VBoxManage guestcontrol G run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ G

# Set up address
VBoxManage guestcontrol G run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.132.2/23 dev enp0s3

# Set up routing
# By default, send packets to R2 (LAN04)
VBoxManage guestcontrol G run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.133.254

echo "Configurazione nodo H"
# Login
VBoxManage guestcontrol H run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ H

# Set up address
VBoxManage guestcontrol H run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.134.1/24 dev enp0s3

# Set up routing
# By default, send packets to R2 (LAN05)
VBoxManage guestcontrol H run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.134.254

echo "Configurazione nodo J"
# Login
VBoxManage guestcontrol J run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ J

# Set up address
VBoxManage guestcontrol J run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.0.134.2/24 dev enp0s3

# Set up routing
# By default, send packets to R2 (LAN05)
VBoxManage guestcontrol J run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.0.134.254

echo "Configurazione nodo X"
# Login
VBoxManage guestcontrol X run --exe /usr/bin/hostname --username root --password root --wait-stdout -- hostname/ X

# Set up address
VBoxManage guestcontrol X run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ addr add 10.1.1.1/17 dev enp0s3

# Set up routing
# By default, send packets to ISP (LANISP)
VBoxManage guestcontrol X run --exe /sbin/ip --username root --password root --wait-stdout -- ip/ route add default via 10.1.1.2
