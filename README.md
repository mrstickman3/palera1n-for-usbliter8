# palera1n for usbliter8

A port of palera1n designed for usbliter8.

## Overview

This project aims to bring palera1n functionality to A12–A13 devices through usbliter8.

At this stage, testing has only confirmed that the tool launches successfully. The jailbreak process does not currently work. That is being fixed. Expect a fix within a week or so

## Device Support

| iPhone(s) | iPad(s) | Apple TV(s) |
| --- | --- | --- |
| iPhone XS | iPad Air (3rd generation) | Apple TV 4K (2nd generation) |
| iPhone XS Max | iPad mini (5th generation) | |
| iPhone XR | iPad (8th generation) | |
| iPhone 11 | iPad (9th generation) | |
| iPhone 11 Pro | | |
| iPhone 11 Pro Max | | |
| iPhone SE (2nd generation) | | |

## Usage

Usage: palera1n [-DEhpvVdsSLRnPI] [-e boot arguments] [-k Pongo image] [-o overlay file] [-r ramdisk file] [-K KPF file] [-i checkra1n file]
```
--version				Print version
--force-revert				Remove jailbreak
-d, --demote				Demote
-D, --dfuhelper				Exit after entering DFU
-e, --boot-args <boot arguments>	XNU boot arguments
-E, --enter-recovery			Enter recovery mode
-h, --help				Show this help
-i, --override-checkra1n <file>		Override checkra1n
-k, --override-pongo <file>		Override Pongo image
-K, --override-kpf <file>		Override kernel patchfinder
-L, --jbinit-log-to-file		Make jbinit log to /cores/jbinit.log (can be read from sandbox while jailbroken)
-n, --exit-recovery			Exit recovery mode
-I, --device-info			Print info about the connected device
--simulator <chip>			Simulate having an A12/A13 device
-o, --override-overlay <file>		Override overlay
-p, --pongo-shell			Boot to PongoOS shell
-P, --pongo-full			Boot to PongoOS shell with default images already uploaded
-r, --override-ramdisk <file>		Override ramdisk
-R, --reboot-device			Reboot connected device normally
-s, --safe-mode				Enter safe mode
-S, --no-colors				Disable colors in terminal output
-v, --debug-logging			Enable debug logging
					Repeat this option for additional verbosity.
-V, --verbose-boot			Enable verbose boot


Environmental variables:
TMPDIR Temporary directory path used for extracted checkra1n files
```

## Installation

Clone the repository and build the project:

```bash
git clone https://github.com/mrstickman3/palera1n-for-usbliter8
cd palera1n-for-usbliter8
sudo make
sudo make install
```

After installation, run:
```
palera1n
```
to launch the tool.

Credits
Paradigm Shift — usbliter8

palera1n team — original palera1n jailbreak project
