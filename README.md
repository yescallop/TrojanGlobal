# TrojanGlobal
A VC++ program that starts [trojan](https://github.com/trojan-gfw/trojan) client and sets it as global proxy through Privoxy and WinINet API.

## Usage
Put `TrojanGlobal.exe`, `trojan.exe`, `tj_privoxy.exe`, `config.txt` in the same directory. Make sure that `local_port` in `config.json` matches with the port of `forward-socks5` in `config.txt`. And then run `TrojanGlobal.exe`.

**Do not modify the filename of executables or `config.txt`, otherwise the program might not be able to run.**
