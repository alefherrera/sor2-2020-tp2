mkdir output
ENABLEUDP=false
MBDATARATE=5

if [ ! -z "$1" ]
then
      ENABLEUDP=$1
fi
if [ ! -z "$2" ]
then
      MBDATARATE=$2
fi
../../waf --run "tp-redes --enableUdpApplication=$ENABLEUDP --megabytesDataRate=$MBDATARATE" --cwd=examples/sor2-2020-tp2/output/