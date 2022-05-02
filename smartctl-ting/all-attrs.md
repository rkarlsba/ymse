# List of all S.M.A.R.T. Attributes Including Vendor Specific.
Grabbed from https://www.data-medics.com/forum/threads/list-of-all-s-m-a-r-t-attributes-including-vendor-specific.1476/
Markdownified by Roy Sigurd Karlsbakk <roy@karlsbakk.net> 2022-05-02

List of all S.M.A.R.T. Attributes Including Vendor Specific.

So here's something I just gleaned from a T13 email series that I've been privy to. It's a list of all vendor specific
S.M.A.R.T. attributes by vendor as well as the basic generic ones. Not sure why it'd be useful, but you never know.

Here's the list:

| id | **Generic HDD codes** |
| -- | --------------------- |
| 01 | Read Error Rate |
| 02 | Throughput Performance |
| 03 | Spin-Up Time |
| 04 | Start/Stop Count |
| 05 | Reallocated Sectors Count |
| 06 | Read Channel Margin |
| 07 | Seek Error Rate |
| 08 | Seek Time Performance |
| 09 | Power-On Hours |
| 0A | Spin Retry Count |
| 0B | Recalibration Retries |
| 0C | Power Cycle Count |
| 0D | Soft Read Error Rate stab |
| B8 | End-to-End Error |
| BB | Reported Uncorrectable Errors |
| BC | Command Timeout |
| BD | High Fly Writes |
| BE | Airflow Temperature |
| BF | G-Sense Error Rate |
| C0 | Power-off Retract Count |
| C1 | Load/Unload Cycle Count |
| C2 | Temperature |
| C3 | Hardware ECC recovered |
| C4 | Reallocation Event Count |
| C5 | Current Pending Sector Count |
| C6 | Uncorrectable Sector Count |
| C7 | UltraDMA CRC Error Count |
| C8 | Write Error Rate |
| C9 | Soft Read Error Rate |
| CA | Data Address Mark Error |
| CB | Run Out Cancel |
| CC | Soft ECC Correction |
| CD | Thermal Asperity Rate |
| CE | Flying Height |
| CF | Spin High Current |
| D0 | Spin Buzz |
| D1 | Offline Seek Performance |
| D3 | Vibration During Write |
| D4 | Shock During Write |
| DC | Disk Shift |
| DD | G-Sense Error Rate |
| DE | Loaded Hours |
| DF | Load/Unload Retry Count |
| E0 | Load Friction |
| E1 | Load/Unload Cycle Count |
| E2 | Load 'In'-time |
| E3 | Torque Amplification Count |
| E4 | Power-Off Retract Cycle |
| E6 | GMR Head Amplitude |
| E7 | Temperature |
| F0 | Head Flying Hours |
| FA | Read Error Retry Rate |
| FE | Free Fall Protection |
| F1 | Total Host Writes |
| F2 | Total Host Reads |

| id | **SmartSsd** – generic attributes for SSDs |
| -- | --------------------- |
| 01 | Read Error Rate |
| 02 | Throughput Performance |
| 03 | Spin-Up Time |
| 04 | Start/Stop Count |
| 05 | Reallocated Sectors Count |
| 06 | Read Channel Margin |
| 07 | Seek Error Rate |
| 08 | Seek Time Performance |
| 09 | Power-On Hours |
| 0A | Spin Retry Count |
| 0B | Recalibration Retries |
| 0C | Power Cycle Count |
| 0D | Soft Read Error Rate stab |
| C0 | Unsafe Shutdown Count |
| C2 | Temperature |
| C4 | Reallocation Event Count |
| C5 | Current Pending Sector Count |
| C6 | Uncorrectable Sector Count |
| F1 | Total Host Writes |
| F2 | Total Host Reads |

## Vendor-specific attributes

| id | **SmartMtron** |
| BB | Total Erase Count |

| id | **SmartIndlinx** |
| 01 | Read Error Rate |
| 09 | Power-On Hours |
| 0C | Power Cycle Count |
| B8 | Initial Bad Block Count |
| C2 | Temperature |
| C3 | Program Failure Block Count |
| C4 | Erase Failure Block Count |
| C5 | Read Failure Block Count |
| C6 | Total Count of Read Sectors |
| C7 | Total Count of Write Sectors |
| C8 | Total Count of Read Commands |
| C9 | Total Count of Write Commands |
| CA | Total Count of Error bits from flash |
| CB | Total Count of Read Sectors with Correctable Bit Errors |
| CC | Bad Block Full Flag |
| CD | Maximum PE Count Specification |
| CE | Minimum Erase Count |
| CF | Maximum Erase Count |
| D0 | Average Erase Count |
| D1 | Remaining Drive Life |

| id | **SmartJMicron60x** |
| 09 | Power-On Hours |
| 0C | Power Cycle Count |
| C2 | Temperature |
| E5 | Halt System ID, Flash ID |
| E8 | Firmware Version Information |
| E9 | ECC Fail Record |
| EA | Average Erase Count, Max Erase Count |
| EB | Good Block Count, System Block Count |

| id | **SmartIntel** |
| 03 | Spin Up Time |
| 04 | Start/Stop Count |
| 05 | Re-Allocated Sector Count |
| 09 | Power-On Hours Count |
| 0C | Power Cycle Count |
| C0 | Unsafe Shutdown Count |
| E1 | Host Writes |
| E8 | Available Reserved Space |
| E9 | Media Wearout Indicator |
| B8 | End to End Error Detection Count |
| AA | Available Reserved Space |
| AB | Program Fail Count |
| AC | Erase Fail Count |
| BB | Uncorrectable Error Count |
| AE | Unexpected Power Loss |
| E2 | Timed Workload Media Wear |
| E3 | Timed Workload Host Read/Write Ratio |
| E4 | Timed Workload Timer |
| F1 | Total LBAs Written |
| F2 | Total LBAs Read |
| F9 | Total NAND Writes |
| B7 | SATA Downshift Count |
| BE | Temperature |
| C7 | CRC Error Count |
| AF | Power Loss Protection Failure |
| C2 | Temperature |
| C5 | Pending Sector Count |
| EA | Thermal Throttle Status |

| id | **SmartSamsung** |
| 05 | Reallocated Sector Count |
| 09 | Power-on Hours |
| 0C | Power-on Count |
| AF | Program Fail Count (Chip) |
| B0 | Erase Fail Count (Chip) |
| B1 | Wear Leveling Count |
| B2 | Used Reserved Block Count (Chip) |
| B3 | Used Reserved Block Count (Total) |
| B4 | Unused Reserved Block Count (Total) |
| B5 | Program Fail Count (Total) |
| B6 | Erase Fail Count (Total) |
| B7 | Runtime Bad Block (Total) |
| BB | Uncorrectable Error Count |
| BE | Airflow Temperature |
| C2 | Temperature |
| C3 | ECC Error Rate |
| C6 | Off-Line Uncorrectable Error Count |
| C7 | CRC Error Count |
| C9 | Super cap Status |
| CA | SSD Mode Status |
| EB | POR Recovery Count |
| F1 | Total LBAs Written |
| F2 | Total LBAs Read |
| AA | Unused Reserved Block Count (Chip) |
| AB | Program Fail Count (Chip) |
| AC | Erase Fail Count (Chip) |
| AD | Wear Leveling Count |
| AE | Unexpected Power Loss Count |
| B8 | Error Detection |
| E9 | Normalized Media Wear-out |

| id | **SmartSandForce** |
| 01 | Raw Read Error Rate |
| 05 | Retired Block Count |
| 09 | Power-on Hours |
| 0C | Power Cycle Count |
| 0D | Soft Read Error Rate |
| 64 | Gigabytes Erased |
| AA | Reserve Block Count |
| AB | Program Fail Count |
| AC | Erase Fail Count |
| AE | Unexpected Power Loss Count |
| B1 | Wear Range Delta |
| B5 | Program Fail Count |
| B6 | Erase Fail Count |
| B8 | Reported I/O Error Detection Code Errors |
| BB | Reported Uncorrectable Errors |
| C2 | Temperature |
| C3 | On-the-Fly ECC Uncorrectable Error Count |
| C4 | Reallocation Event Count |
| C6 | Uncorrectable Sector Count |
| C7 | SATA R-Errors (CRC) Error Count |
| C9 | Uncorrectable Soft Read Error Rate |
| CC | Soft ECC Correction Rate |
| E6 | Life Curve Status |
| E7 | SSD Life Left |
| E8 | Available Reserved Space |
| EB | SuperCap health |
| F1 | Lifetime Writes from Host |
| F2 | Lifetime Reads from Host |

| id | **SmartJMicron61x** |
| 01 | Read Error Rate |
| 02 | Throughput Performance |
| 03 | Spin-Up Time |
| 05 | Reallocated Sectors Count |
| 07 | Seek Error Rate |
| 08 | Seek Time Performance |
| 09 | Power-On Hours |
| 0A | Spin Retry Count |
| 0C | Power Cycle Count |
| A8 | SATA PHY Error Count |
| AF | Bad Cluster Table Count (ECC Fail Count) |
| C0 | Unexpected Power Loss Count |
| C2 | Temperature |
| C5 | Current Pending Sector Count |
| AA | Bad Block Count |
| AD | Erase Count |

| id | **SmartMicron** |
| 01 | Raw Read Error Rate |
| 05 | Reallocated NAND Blocks |
| 09 | Power On Hours |
| 0C | Power Cycle Count |
| 0D | Soft Error Rate |
| 0E | Device Capacity (NAND) |
| 0F | User Capacity |
| 10 | Spare Blocks Available |
| 11 | Remaining Spare Blocks |
| 64 | Total Erase Count |
| AA | Reserved Block Count |
| AB | Program Fail Count |
| AC | Erase Fail Count |
| AD | Average Block-Erase Count |
| AE | Unexpected Power Loss Count |
| B4 | Unused Reserve NAND Blocks |
| B5 | Unaligned Access Count |
| B7 | SATA Interface Downshift |
| B8 | Error Correction Count |
| BB | Reported Uncorrectable Errors |
| BC | Command Timeout Count |
| BD | Factory Bad Block Count |
| C2 | Temperature |
| C3 | Cumulative ECC Bit Correction Count |
| C4 | Reallocation Event Count |
| C5 | Current Pending Sector Count |
| C6 | Smart Off-line Scan Uncorrectable Error Count |
| C7 | Ultra DMA CRC Error Rate |
| CA | Percent Lifetime Used |
| CE | Write Error Rate |
| D2 | Successful RAIN Recovery Count |
| EA | Total Bytes Read |
| F2 | Write Protect Progress |
| F3 | ECC Bits Corrected |
| F4 | ECC Cumulative Threshold Events |
| F5 | Cumulative Program NAND Pages |
| F6 | Total Host Sector Writes |
| F7 | Host Program Page Count |
| F8 | Background Program Page Count |

| id | **SmartMicronMU02** |
| 01 | Raw Read Error Rate |
| 05 | Reallocated Sectors Count |
| 09 | Power-On Hours |
| 0C | Power Cycle Count |
| 0D | Soft Error Rate |
| 0E | Device Capacity (NAND) |
| 0F | User Capacity |
| 10 | Spare Blocks Available |
| 11 | Remaining Spare Blocks |
| 64 | Total Erase Count |
| AA | Reserved Block Count |
| AB | Program Fail Count |
| AC | Erase Fail Count |
| AD | Average Block-Erase Count |
| AE | Unexpected Power Loss Count |
| B4 | Unused Reserve NAND Blocks |
| B7 | SATA Interface Downshift |
| B8 | Error Correction Count |
| BB | Reported Uncorrectable Errors |
| BC | Command Timeout Count |
| BD | Factory Bad Block Count |
| C2 | Temperature |
| C3 | Cumulative ECC Bit Correction Count |
| C4 | Reallocation Event Count |
| C5 | Current Pending Sector Count |
| C6 | Smart Off-line Scan Uncorrectable Error Count |
| C7 | Ultra DMA CRC Error Rate |
| CA | Percent Lifetime Used |
| CE | Write Error Rate |
| D2 | Successful RAIN Recovery Count |
| EA | Total Bytes Read |
| F3 | ECC Bits Corrected |
| F4 | ECC Cumulative Threshold Events |
| F5 | Cumulative Program NAND Pages |
| F6 | Total Host Sector Writes |
| F7 | Host Program Page Count |
| F8 | Background Program Page Count |
| A0 | Uncorrectable Sector Count |
| A1 | Valid Spare Blocks |
| A3 | Initial Invalid Blocks |
| 94 | Total SLC Erase Count |
| 95 | Maximum SLC Erase Count |
| 96 | Minimum SLC Erase Count |
| 97 | Average SLC Erase Count |
| A4 | Total TLC Erase Count |
| A5 | Maximum TLC Erase Count |
| A6 | Minimum TLC Erase Count |
| A7 | Average TLC Erase Count |
| A9 | Percentage Lifetime Remaining |
| B5 | Program Fail Count |
| B6 | Erase Fail Count |
| C0 | Power-off Retract Count |
| E8 | Available Reserved Space |
| F1 | Total LBA Write |
| F2 | Total LBA Read |
| F5 | Total TLC Write Count |
| F6 | Total SLC Write Count |
| F7 | RAID Recovery Count |

| id | **SmartOcz** |
| 01 | Raw Read Error Rate |
| 03 | Spin Up Time |
| 04 | Start Stop Count |
| 05 | Reallocated Sectors Count |
| 09 | Power-On Hours |
| 0C | Power Cycle Count |
| C2 | Temperature |
| E8 | Total Count of Write Sectors |
| E9 | Remaining Life |
| 64 | Total Blocks Erased |
| A7 | SSD Protect Mode |
| A8 | SATA PHY Error Count |
| A9 | Bad Block Count |
| AD | Erase Count |
| B8 | Factory Bad Block Count |
| C0 | Unexpected Power Loss Count |
| CA | Total Number of Corrected Bits |
| CD | Max Rated PE Counts |
| CE | Minimum Erase Counts |
| CF | Maximum Erase Counts |
| D3 | SATA Uncorrectable Error Count |
| D4 | NAND Page Reads During Retry |
| D5 | Simple Read Retry Attempts |
| D6 | Adaptive Read Retry Attempts |
| DD | Internal Data Path Uncorrectable Errors |
| DE | RAID Recovery Count |
| E6 | Power Loss Protection |
| FB | NAND Read Count |
| F1 | Total Host Writes |
| F2 | Total Host Reads |

| id | **SmartPlextor** |
| 01 | Read Error Rate |
| 05 | Reallocated Sectors Count |
| 09 | Power-On Hours |
| 0C | Power Cycle Count |
| C0 | Unsafe Shutdown Count |
| C2 | Temperature |
| C4 | Reallocation Event Count |
| C6 | Uncorrectable Sector Count |
| F1 | Total LBA written |
| F2 | Total LBA read |
| B1 | Wear Leveling Count |
| B2 | Used Reserved Block Count (Worst Case) |
| B5 | Program Fail Count (Total) |
| B6 | Erase Fail Count (Total) |
| BB | Uncorrectable Error Count |
| C7 | CRC Error Count |
| E8 | Available Reserved Space |
| AA | Grown Bad Blocks |
| AB | Program Fail Count (Total) |
| AC | Erase Fail Count (Total) |
| AD | Average Program/Erase Count (Total) |
| AE | Unexpected Power Loss Count |
| AF | Program Fail Count (Worst Case) |
| B0 | Erase Fail Count (Worst Case) |
| B3 | Used Reserved Block Count (Total) |
| B4 | UnUsed Reserved Block Count (Total) |
| B7 | SATA Interface Down Shift |
| B8 | End-to-End Data Errors Corrected |
| BC | Command Time out |
| C3 | ECC rate |
| E9 | NAND GB written |

| id | **SmartSanDisk** |
| 05 | Retired Block Count |
| 09 | Power On Hours |
| 0C | Power Cycle Count |
| AA | Reserve Block Count |
| AB | Program Fail Count |
| AC | Erase Fail Count |
| AE | Unexpected Power Loss Count |
| BB | Reported Uncorrectable Errors |
| C2 | Temperature |
| E8 | Remaining Life |
| F1 | Total Host Writes |
| F2 | Total Host Reads |

| id | **SmartOczVector** |
| 05 | Accumulated Runtime Bad Blocks |
| 09 | Power-On Hours Count |
| 0C | Power Cycle Count |
| AB | Available Over-Provisioned Block Count |
| AE | Power Cycle Count (Unplanned) |
| BB | Total Uncorrectable NAND Reads |
| C2 | Temperature |
| C3 | Total Programming Failures |
| C4 | Total Erase Failures |
| C5 | Total Read Failures (Uncorrectable) |
| C6 | Host Reads |
| C7 | Host Writes |
| D0 | Average Erase Count |
| D2 | SATA CRC Error Count |
| E9 | Remaining Life |
| F9 | Total NAND Programming Count |
| F1 | Total Host Writes |
| F2 | Total Host Reads |
| 64 | Total Blocks Erased |
| B8 | Factory Bad Block Count Total |
| CA | Total Number Of Read Bits Corrected |
| CD | Max Rated PE Count |
| CE | Min Erase Count |
| CF | Max Erase Count |
| D4 | Total Count NAND Pages Reads Requiring Read Retry |
| D5 | Total Count of Simple Read Retry Attempts |
| D6 | Total Count of Adaptive Read Retry Attempts |
| DD | Internal Data Path Protection Uncorrectable Errors |
| DE | RAID Recovery Count |
| E0 | In Warranty |
| E1 | DAS Polarity |
| E2 | Partial Pfail |
| E6 | Write Throttling Activation Flag |
| F9 | Total NAND Programming Count (pages) |
| FB | Total NAND Read Count |

| id | **SmartCorsair** |
| 01 | Raw Read Error Rate |
| 05 | Retired Block Count |
| 09 | Power On Hours |
| 0C | Power Cycle Count |
| AA | Reserve Block Count |
| AC | Erase Fail Count |
| AE | Unexpected Power Loss Count |
| B5 | Program Fail Count |
| BB | Reported Uncorrectable Errors |
| C2 | Temperature |
| C4 | Reallocation Event Count |
| E7 | SSD Life Left |
| F1 | Lifetime GB Written |
| F2 | Lifetime GB Read |

| id | **SmartToshiba** |
| 01 | Read Error Rate |
| 02 | Throughput Performance |
| 03 | Spin-Up Time |
| 04 | Start/Stop Count |
| 05 | Reallocated Sectors Count |
| 07 | Seek Error Rate |
| 08 | Seek Time Performance |
| 09 | Power-On Hours |
| 0A | Spin Retry Count |
| 0B | Recalibration Retries |
| 0C | Power Cycle Count |
| A7 | SSD Protect Mode |
| A8 | SATA PHY Error Count |
| A9 | Total Bad Block Count |
| AD | Erase Count |
| C0 | Unexpected Power Loss Count |
| C2 | Temperature |
| C4 | Reallocation Event Count |
| C5 | Current Pending Sector Count |
| C6 | Uncorrectable Sector Count |
| F1 | Total Host Writes |
| F2 | Total Host Reads |

| id | **SmartSanDiskGb** |
| 05 | Reallocated Block Count |
| 09 | Power On Hours |
| 0C | Power Cycle Count |
| A6 | Min W/E Cycle |
| A7 | Max Bad Block/Die |
| A8 | Maximum Erase Cycle |
| A9 | Total Bad Block |
| AB | Program Fail Count |
| AC | Erase Fail Count |
| AD | Average Erase Cycle |
| AE | Unexpected Power Loss Count |
| BB | Reported Uncorrectable Errors |
| C2 | Temperature |
| D4 | SATA PHY Error |
| E6 | Percentage Total P/E Count |
| E8 | Spare Block Remaining |
| E9 | Total GB Written To NAND |
| F1 | Total GB Written |
| F2 | Total GB Read |

| id | **SmartKingston** |
| 01 | Read Error Rate |
| 02 | Throughput Performance |
| 03 | Spin Up Time |
| 05 | Reallocated Sectors Count |
| 07 | Seek Error Rate |
| 08 | Seek Time Performance |
| 09 | Power-On Hours |
| 0A | Spin Retry Count |
| 0C | Power Cycle Count |
| A8 | SATA PHY Error Count |
| AA | Bad Block Count |
| AD | Erase count |
| AF | Bad Cluster Table Count |
| BB | Uncorrectable Errors |
| C0 | Unsafe Shutdown Count |
| C2 | Temperature |
| C4 | Later Bad Block Count |
| C5 | Current Pending Sector Count |
| C7 | CRC Error Count |
| DA | CRC Error Count |
| E7 | SSD Life Left |
| E9 | Lifetime Writes to Flash |
| F0 | Write Head |
| F1 | Host Writes |
| F2 | Host Reads |
| F4 | Average Erase Count |
| F5 | Max Erase Count |
| F6 | Total Erase Count |
