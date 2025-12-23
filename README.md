# ZC702 PS Ethernet UDP GPIO Inverter (Vitis 2025.2)

Minimal lwIP bare-metal app for the Zynq-7000 ZC702 that proves the PS → AXI → PL → AXI → PS datapath. A UDP server on GEM0 (port 5005) accepts a 4-byte little-endian word `W`, drives it on AXI GPIO OUT, reads AXI GPIO IN through a NOT gate in PL, and replies with 8 bytes `[W][R]` (little-endian), where `R` should equal `~W`.

## Hardware datapath
- PS writes `W` to AXI GPIO OUT DATA (base 0x41210000).
- AXI GP0 → interconnect → AXI GPIO OUT → `gpio_io_o[31:0]`.
- PL util_vector_logic NOT inverts all 32 bits.
- Inverted bus drives AXI GPIO IN `gpio_io_i[31:0]` (base 0x41200000).
- PS reads `R` from AXI GPIO IN DATA; expected `R == ~W`.
- Ethernet uses PS GEM0 via MIO/RGMII to the onboard PHY/RJ45.

## Repo layout
- `vitis/src/` – app sources (no generated BSP/build outputs).
- `host/` – Python UDP probe client.
- `vivado/` – placeholder for TCL/export notes (no full project).

## Build (Vitis 2025.2)
1) Create/import platform `zc702_platform` (standalone, ps7_cortexa9_0, lwIP enabled, GEM0).
2) Create an empty app and drop in `vitis/src/*` as sources. Use the BSP-provided `platform.c/h`; no custom platform files required.
3) Build with SDT enabled (as in the default standalone domain). The app uses `XPAR_XEMACPS_0_BASEADDR`.

## Runbook (board test)
1) Program PL with the exported bitstream (contains AXI GPIO OUT → NOT → AXI GPIO IN).
2) Launch the ELF on `ps7_cortexa9_0` over JTAG. UART 115200 8N1.
3) Watch UART: banner, IP `192.168.1.10/24`, and “udp_inverter: listening on UDP port 5005”.
4) Host PC: set static IP on same subnet (e.g., `192.168.1.20/24`), connect RJ45 to ZC702.
5) Optional: `ping 192.168.1.10` (if ICMP enabled in lwipopts).
6) UDP test (from `host/`):
   ```bash
   python host_udp_inverter.py 192.168.1.10 0xAAAAAAAA
   python host_udp_inverter.py 192.168.1.10 0x55555555
   python host_udp_inverter.py 192.168.1.10 0x12345678
   ```
   Expected: `read back` equals bitwise NOT of the sent word.

## Troubleshooting
- No UDP reply: check link LEDs, host firewall, IP/subnet, GEM0 init.
- `R == W`: PL inverter not in bitstream or wiring incorrect.
- `R` stuck (all 0/1): GPIO direction wrong or PL not clocked/reset.
- DHCP/ARP issues: this app uses static IP; keep host on same /24.

## License
MIT.
