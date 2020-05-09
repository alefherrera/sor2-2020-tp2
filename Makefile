# Makefile wrapper for waf

all:
	../../waf --run "tp-redes" --cwd=examples/sor2-2020-tp2/output/
udp:
	../../waf --run "tp-redes --enableUdpApplication=true" --cwd=examples/sor2-2020-tp2/output/
tcp:
	../../waf --run "tp-redes --enableTcpApplication=true" --cwd=examples/sor2-2020-tp2/output/
hybla:
	../../waf --run "tp-redes --tcpVariant=TcpHybla" --cwd=examples/sor2-2020-tp2/output/
