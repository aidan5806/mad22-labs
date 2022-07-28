# MAD 2022
Microarchitectural Attacks and Defenses labs for ISCA 2022

# Install Kernel Module
```
cd spectre-chals/module-src
make uninstall
make clean
make
make install
```

# Set up Isolated Core
Open the file `/etc/default/grub` and update the following line:
`GRUB_CMDLINE_LINUX="isolcpus=1"`
Then run `sudo update-grub` to apply the changes. Reboot.

# Run a Challenge on Isolated Core
```
For each part update part<x> to be whichever challenge you want to run.
```
cd spectre-chals/
make clean
make
taskset 0x2 sudo ./part1
```