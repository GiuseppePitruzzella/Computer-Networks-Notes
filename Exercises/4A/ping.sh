echo "Ping A -> D"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.209.255.100

echo "Ping C -> G"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.207.255.100

echo "Ping D -> C"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.208.255.100

echo "Ping F -> B"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.159.255.100