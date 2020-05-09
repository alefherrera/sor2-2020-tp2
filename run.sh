mkdir output
ENABLEUDP=false
ENABLETCP=false
MBDATARATE=5
TCP_VARIANT="TcpNewReno"

if [ ! -z "$1" ]
then
      ENABLEUDP=$1
fi
if [ ! -z "$2" ]
then
      ENABLETCP=$2
fi
if [ ! -z "$3" ]
then
      MBDATARATE=$3
fi
if [ ! -z "$4" ]
then
      TCP_VARIANT=$4
fi
../../waf --run "tp-redes --enableUdpApplication=$ENABLEUDP --enableTcpApplication=$ENABLETCP --megabytesDataRate=$MBDATARATE --tcpVariant=$TCP_VARIANT" --cwd=examples/sor2-2020-tp2/output/