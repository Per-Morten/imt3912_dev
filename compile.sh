#!/bin/bash

if [[ ! "$(pwd)" =~ imt3912_dev$ ]] 
then
	echo "You need to be in the imt3912_dev directory to run this script!";
	exit 1;
fi

CMAKE_ARGUMENTS="";
MAKE_ARGUMENTS="";

#Parses all arguments and put them into variables
for i in "$@"
do
    case $i in
        --cmake-arguments=*)
        CMAKE_ARGUMENTS="${i#*=}"
        shift # past argument=value
        ;;
        --make-arguments=*)
        MAKE_ARGUMENTS="${1#*=}"
        shift # past argument=value
        ;;
    esac
done


if [ ! -d "./build" ]
then
	mkdir build;
fi

pushd build > /dev/null 2>&1;
cmake .. $CMAKE_ARGUMENTS;
make $MAKE_ARGUMENTS;
popd > /dev/null 2>&1;
