sudo nmcli con modify rpi_hotspot_test 802-11-wireless-security.key-mgmt wpa-psk && \
sudo nmcli con modify rpi_hotspot_test 802-11-wireless-security.proto rsn && \
sudo nmcli con modify rpi_hotspot_test 802-11-wireless-security.group ccmp && \
sudo nmcli con modify rpi_hotspot_test 802-11-wireless-security.pairwise ccmp && \
sudo nmcli con modify rpi_hotspot_test 802-11-wireless-security.psk Al4bi&83 && \
sudo nmcli con modify rpi_hotspot_test ipv4.method shared && \
sudo nmcli con up rpi_hotspot_test
