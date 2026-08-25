

sudo openvpn --config Downloads/VPN-AGH.2026.ovpn --daemon --log /dev/null
sleep 10
lftp sftp://matjanis@student.agh.edu.pl