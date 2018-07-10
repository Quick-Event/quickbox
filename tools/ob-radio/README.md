install pyserial python module
```sh
sudo pip3 install pyserial
```
run script
```sh
python3 sipunchrelay.py
```
or install it as systemd service
```sh
sudo cp tools/ob-radio/etc/systemd/system/ob-radio.service /etc/systemd/system/
sudo systemctl daemon-reload
sudo systemctl enable ob-radio
sudo systemctl start ob-radio
sudo journalctl -u ob-radio -f
```
