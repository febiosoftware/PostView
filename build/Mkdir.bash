set -v

if [ $# == 0 ]; then
	echo "Usage: Mkdir.bash platform"
	exit
fi

mkdir $1
cd $1
mkdir PostViewLib
mkdir PostView2
