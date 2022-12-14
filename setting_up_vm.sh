#!/bin/bash
# This script is used to set up the virtual machines.
# Author: Giuseppe Pitruzzella

# Number of hosts
printf "Enter the number of hosts: \n" 
read n
# Name of main VM
printf "Enter the name of the main VM: \n"
read main

# Creating snapshot of main VM
VBoxManage snapshot $main take Snapshot
# Creating hosts
for (( i=1; i<=$n; i++ )) 
do
	printf "Creating host $i \n"
	# printf "Crezione dell'host" $i
	printf "Insert the name for the host (e.g. A): \n"
	read name

	VBoxManage clonevm $main --snapshot Snapshot --name $name --options link --register
	
	# Network settings
	# N.B. Set "Internal Network" by default
	printf "Setting up network for host $name \n"

	# Number of interfaces per host
	printf "Insert number of interfaces per host... \n"
	read m

	for (( j=1; j<=$m; j++ ))
	do
		printf "Insert name for LAN (e.g. lan01)... \n"
		read lan
		VBoxManage modifyvm $name --nic$j intnet --intnet$j $lan  
	done

	VBoxManage startvm $name --type headless
done 
