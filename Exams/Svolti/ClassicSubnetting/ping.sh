echo "ping H -> A"
VBoxManage guestcontrol H run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.0.135.65

echo "ping H -> C"
VBoxManage guestcontrol H run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.0.128.2

echo "ping F -> E"
VBoxManage guestcontrol F run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.0.135.2

echo "ping G -> B"
VBoxManage guestcontrol G run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.0.128.1

echo "ping B -> H"
VBoxManage guestcontrol B run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.0.134.1

echo "ping D -> H"
VBoxManage guestcontrol D run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.0.134.1

echo "ping A -> X"
VBoxManage guestcontrol A run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.1.1.1

echo "ping E -> X"
VBoxManage guestcontrol E run --exe /bin/ping --username root --password root  --wait-stdout -- ping/ -c 4 10.1.1.1
