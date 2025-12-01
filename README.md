# How to Use
You'll need to be on a Linux machine (WSL is great)
1. [Install nix](https://nixos.org/download/)
2. Enable flakes by adding this line in `~/.config/nix/nix.conf`:
    `experimental-features = nix-command flakes`
3. Reload the nix daemon with `systemctl`.

Compile:

``` sh
gcc -lulfius -lmysqlclient ./src/server.c 
./a.out
```
