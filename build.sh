#!/bin/bash

if [[ ! "$(pwd)" =~ imt3912_dev$ ]] 
then
	echo "You need to be in the imt3912_dev directory to run this script!";
	exit 1;
fi

if [ ! -d "./build" ]
 then
	mkdir build;
fi

pushd build > /dev/null 2>&1
cmake ..
make -j4
popd > /dev/null 2>&1
