cp -rf /home/rsi/release/dhcpd.conf /etc/
cp -rf /home/rsi/release/dhcpd.conf /etc/dhcp/
/sbin/ifconfig $1 192.168.2.1
sleep 1
/sbin/service dhcpd restart
