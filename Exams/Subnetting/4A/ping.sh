echo "Ping A -> D"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.210.0.1

echo "Ping C -> G"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.192.0.1

echo "Ping D -> C"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.209.0.1

echo "Ping F -> B"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.128.0.2