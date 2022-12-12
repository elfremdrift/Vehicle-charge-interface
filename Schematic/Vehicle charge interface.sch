EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L Connector:Conn_01x07_Male J2
U 1 1 637E8234
P 2150 3600
F 0 "J2" H 2250 4050 50  0000 C CNN
F 1 "Type 2 in" V 2050 3600 50  0000 C CNN
F 2 "" H 2150 3600 50  0001 C CNN
F 3 "~" H 2150 3600 50  0001 C CNN
	1    2150 3600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 3300 2600 3300
Wire Wire Line
	2350 3400 2600 3400
Wire Wire Line
	2350 3500 2600 3500
Wire Wire Line
	2350 3600 2600 3600
Wire Wire Line
	2350 3700 2600 3700
Wire Wire Line
	2350 3800 2600 3800
Wire Wire Line
	2350 3900 2600 3900
Text Label 2400 3500 0    50   ~ 0
PE
Text Label 2400 3300 0    50   ~ 0
CP
Text Label 2400 3400 0    50   ~ 0
PP
Text Label 2400 3600 0    50   ~ 0
N
Text Label 2400 3700 0    50   ~ 0
L1
Text Label 2400 3800 0    50   ~ 0
L2
Text Label 2400 3900 0    50   ~ 0
L3
$Comp
L Connector:Conn_01x05_Female J3
U 1 1 637EB0C7
P 2150 4450
F 0 "J3" H 2050 4800 50  0000 C CNN
F 1 "Charger Out" V 2250 4450 50  0000 C CNN
F 2 "" H 2150 4450 50  0001 C CNN
F 3 "~" H 2150 4450 50  0001 C CNN
	1    2150 4450
	-1   0    0    -1  
$EndComp
Wire Wire Line
	2350 4250 2600 4250
Wire Wire Line
	2350 4350 2600 4350
Wire Wire Line
	2350 4450 2600 4450
Wire Wire Line
	2350 4550 2600 4550
Wire Wire Line
	2350 4650 2600 4650
Entry Wire Line
	2600 3900 2700 4000
Entry Wire Line
	2600 3800 2700 3900
Entry Wire Line
	2600 3700 2700 3800
Entry Wire Line
	2600 3600 2700 3700
Entry Wire Line
	2600 3500 2700 3600
Entry Wire Line
	2600 3400 2700 3500
Entry Wire Line
	2600 3300 2700 3400
Entry Wire Line
	2600 4250 2700 4150
Entry Wire Line
	2600 4350 2700 4250
Entry Wire Line
	2600 4450 2700 4350
Entry Wire Line
	2600 4550 2700 4450
Entry Wire Line
	2600 4650 2700 4550
$Comp
L Connector:Conn_01x04_Male J1
U 1 1 637EED59
P 2150 2750
F 0 "J1" H 2258 3031 50  0000 C CNN
F 1 "Type 2 lock" V 2050 2700 50  0000 C CNN
F 2 "" H 2150 2750 50  0001 C CNN
F 3 "~" H 2150 2750 50  0001 C CNN
	1    2150 2750
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 2650 2600 2650
Wire Wire Line
	2350 2750 2600 2750
Wire Wire Line
	2350 2850 2600 2850
Wire Wire Line
	2350 2950 2600 2950
Text Label 2400 2650 0    50   ~ 0
M1
Text Label 2400 2750 0    50   ~ 0
M2
Text Label 2400 2850 0    50   ~ 0
S1
Text Label 2400 2950 0    50   ~ 0
S2
Entry Wire Line
	2600 2650 2700 2750
Entry Wire Line
	2600 2750 2700 2850
Entry Wire Line
	2600 2850 2700 2950
Entry Wire Line
	2600 2950 2700 3050
Connection ~ 2700 3150
Text Label 2400 4250 0    50   ~ 0
PE
Text Label 2400 4350 0    50   ~ 0
N
Text Label 2400 4450 0    50   ~ 0
L1
Text Label 2400 4550 0    50   ~ 0
L2
Text Label 2400 4650 0    50   ~ 0
L3
$Comp
L power:+12V #PWR015
U 1 1 637F63EA
P 7350 3550
F 0 "#PWR015" H 7350 3400 50  0001 C CNN
F 1 "+12V" V 7350 3700 50  0000 L CNN
F 2 "" H 7350 3550 50  0001 C CNN
F 3 "" H 7350 3550 50  0001 C CNN
	1    7350 3550
	0    1    1    0   
$EndComp
$Comp
L power:-12V #PWR014
U 1 1 637F73EC
P 7350 3450
F 0 "#PWR014" H 7350 3550 50  0001 C CNN
F 1 "-12V" V 7350 3600 50  0000 L CNN
F 2 "" H 7350 3450 50  0001 C CNN
F 3 "" H 7350 3450 50  0001 C CNN
	1    7350 3450
	0    1    1    0   
$EndComp
Wire Wire Line
	5900 4050 4050 4050
Text Label 3900 4050 0    50   ~ 0
M1
Entry Wire Line
	3750 3950 3850 4050
$Comp
L power:GND #PWR03
U 1 1 637FABE1
P 4050 5700
F 0 "#PWR03" H 4050 5450 50  0001 C CNN
F 1 "GND" H 4055 5527 50  0000 C CNN
F 2 "" H 4050 5700 50  0001 C CNN
F 3 "" H 4050 5700 50  0001 C CNN
	1    4050 5700
	-1   0    0    -1  
$EndComp
Entry Wire Line
	3750 5400 3850 5500
Entry Wire Line
	3750 5300 3850 5400
Entry Wire Line
	3750 5200 3850 5300
Wire Wire Line
	4050 5700 4050 5500
Wire Wire Line
	3850 5500 4050 5500
Connection ~ 4050 5500
Wire Wire Line
	4050 5500 4050 5400
$Comp
L power:+12V #PWR04
U 1 1 637FD203
P 4550 2350
F 0 "#PWR04" H 4550 2200 50  0001 C CNN
F 1 "+12V" H 4565 2523 50  0000 C CNN
F 2 "" H 4550 2350 50  0001 C CNN
F 3 "" H 4550 2350 50  0001 C CNN
	1    4550 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 5300 4050 5400
Connection ~ 4050 5400
Wire Wire Line
	4050 4050 3850 4050
Wire Wire Line
	4700 2500 4700 2650
Wire Wire Line
	5650 3950 5900 3950
$Comp
L power:+12V #PWR06
U 1 1 6380FC8A
P 5400 3800
F 0 "#PWR06" H 5400 3650 50  0001 C CNN
F 1 "+12V" H 5300 4000 50  0000 L CNN
F 2 "" H 5400 3800 50  0001 C CNN
F 3 "" H 5400 3800 50  0001 C CNN
	1    5400 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 4150 5900 4150
$Comp
L Device:R R7
U 1 1 63819FDC
P 5400 4600
F 0 "R7" H 5550 4550 50  0000 R CNN
F 1 "1k3" H 5650 4650 50  0000 R CNN
F 2 "" V 5330 4600 50  0001 C CNN
F 3 "~" H 5400 4600 50  0001 C CNN
	1    5400 4600
	1    0    0    1   
$EndComp
Wire Bus Line
	2700 3150 3750 3150
Entry Wire Line
	3750 4850 3850 4950
Text Label 3900 4950 0    50   ~ 0
CP
$Comp
L Device:R R8
U 1 1 6381A739
P 5400 5300
F 0 "R8" H 5470 5346 50  0000 L CNN
F 1 "2k7" H 5470 5255 50  0000 L CNN
F 2 "" V 5330 5300 50  0001 C CNN
F 3 "~" H 5400 5300 50  0001 C CNN
	1    5400 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 4950 5400 5150
Entry Wire Line
	3750 3300 3850 3200
Text Label 3900 3200 0    50   ~ 0
PP
Wire Wire Line
	3850 4950 4850 4950
$Comp
L Device:D D4
U 1 1 63814435
P 5100 4950
F 0 "D4" H 5100 4733 50  0000 C CNN
F 1 "1n4007" H 5100 4824 50  0000 C CNN
F 2 "" H 5100 4950 50  0001 C CNN
F 3 "~" H 5100 4950 50  0001 C CNN
	1    5100 4950
	-1   0    0    1   
$EndComp
Wire Wire Line
	5550 5550 5550 5700
$Comp
L power:GND #PWR08
U 1 1 6381DA2E
P 5550 5700
F 0 "#PWR08" H 5550 5450 50  0001 C CNN
F 1 "GND" H 5555 5527 50  0000 C CNN
F 2 "" H 5550 5700 50  0001 C CNN
F 3 "" H 5550 5700 50  0001 C CNN
	1    5550 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 3950 5650 3800
$Comp
L power:-12V #PWR09
U 1 1 6380EDA8
P 5650 3800
F 0 "#PWR09" H 5650 3900 50  0001 C CNN
F 1 "-12V" H 5650 4000 50  0000 C CNN
F 2 "" H 5650 3800 50  0001 C CNN
F 3 "" H 5650 3800 50  0001 C CNN
	1    5650 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 4150 5400 3800
Wire Wire Line
	5400 4450 5400 4250
Wire Wire Line
	5400 4250 5900 4250
Wire Wire Line
	5400 4950 5400 4750
Connection ~ 5400 4950
Wire Wire Line
	5900 4350 5700 4350
Wire Wire Line
	5700 4350 5700 5550
Wire Wire Line
	5400 5450 5400 5550
Wire Wire Line
	5400 5550 5550 5550
Connection ~ 5550 5550
Wire Wire Line
	5550 5550 5700 5550
$Comp
L Converter_DCDC:MGJ2D241709SC U1
U 1 1 6386DCD1
P 7300 5500
F 0 "U1" H 7300 5967 50  0000 C CNN
F 1 "DC/DC" H 7300 5876 50  0000 C CNN
F 2 "Converter_DCDC:Converter_DCDC_Murata_MGJ2DxxxxxxSC_THT" H 7300 5100 50  0001 C CNN
F 3 "https://power.murata.com/pub/data/power/ncl/kdc_mgj2.pdf" H 7275 5500 50  0001 C CNN
	1    7300 5500
	-1   0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x02_Male J7
U 1 1 63871AC6
P 8650 5300
F 0 "J7" H 8750 5450 50  0000 R CNN
F 1 "24V in" V 8550 5400 50  0000 R CNN
F 2 "" H 8650 5300 50  0001 C CNN
F 3 "~" H 8650 5300 50  0001 C CNN
	1    8650 5300
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7800 5300 8450 5300
Wire Wire Line
	8450 5400 8000 5400
Wire Wire Line
	8000 5400 8000 5700
Wire Wire Line
	8000 5700 7800 5700
$Comp
L power:+12V #PWR010
U 1 1 63874EF1
P 6650 5300
F 0 "#PWR010" H 6650 5150 50  0001 C CNN
F 1 "+12V" V 6665 5428 50  0000 L CNN
F 2 "" H 6650 5300 50  0001 C CNN
F 3 "" H 6650 5300 50  0001 C CNN
	1    6650 5300
	0    -1   -1   0   
$EndComp
$Comp
L power:-12V #PWR012
U 1 1 638756B4
P 6650 5700
F 0 "#PWR012" H 6650 5800 50  0001 C CNN
F 1 "-12V" V 6665 5828 50  0000 L CNN
F 2 "" H 6650 5700 50  0001 C CNN
F 3 "" H 6650 5700 50  0001 C CNN
	1    6650 5700
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6650 5300 6800 5300
Wire Wire Line
	6800 5700 6650 5700
$Comp
L power:GND #PWR011
U 1 1 6387CC05
P 6650 5500
F 0 "#PWR011" H 6650 5250 50  0001 C CNN
F 1 "GND" V 6655 5372 50  0000 R CNN
F 2 "" H 6650 5500 50  0001 C CNN
F 3 "" H 6650 5500 50  0001 C CNN
	1    6650 5500
	0    1    1    0   
$EndComp
Wire Wire Line
	6650 5500 6800 5500
$Comp
L Connector:Conn_01x02_Female J5
U 1 1 63885086
P 8650 2600
F 0 "J5" H 8550 2750 50  0000 L CNN
F 1 "Traction enable" V 8750 2250 50  0000 L CNN
F 2 "" H 8650 2600 50  0001 C CNN
F 3 "~" H 8650 2600 50  0001 C CNN
	1    8650 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 3100 7050 3100
Wire Wire Line
	6950 3200 7050 3200
$Comp
L Connector:Conn_01x04_Female J6
U 1 1 638902B0
P 8650 4350
F 0 "J6" H 8550 4600 50  0000 L CNN
F 1 "Status lamps" V 8750 4050 50  0000 L CNN
F 2 "" H 8650 4350 50  0001 C CNN
F 3 "~" H 8650 4350 50  0001 C CNN
	1    8650 4350
	1    0    0    -1  
$EndComp
Wire Wire Line
	6950 4350 8450 4350
Wire Wire Line
	6950 4450 8450 4450
Wire Wire Line
	7400 4650 7400 4550
Wire Wire Line
	7400 4550 8450 4550
Wire Wire Line
	6950 4050 7400 4050
Wire Wire Line
	7400 4050 7400 4250
Wire Wire Line
	7400 4250 8450 4250
Text Label 8050 4250 0    50   ~ 0
RED
Text Label 8050 4350 0    50   ~ 0
YELLOW
Text Label 8050 4450 0    50   ~ 0
GREEN
$Comp
L Relay:RAYEX-L90B RE1
U 1 1 638A337C
P 7550 2600
F 0 "RE1" H 7880 2646 50  0000 L CNN
F 1 "SPST-NC" H 7880 2555 50  0000 L CNN
F 2 "Relay_THT:Relay_SPST_RAYEX-L90B" H 7900 2550 50  0001 L CNN
F 3 "https://a3.sofastcdn.com/attachment/7jioKBjnRiiSrjrjknRiwS77gwbf3zmp/L90-SERIES.pdf" H 8050 2450 50  0001 L CNN
	1    7550 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	7650 2200 7650 2100
Wire Wire Line
	7650 2100 8300 2100
Wire Wire Line
	8300 2100 8300 2600
Wire Wire Line
	8300 2600 8450 2600
Wire Wire Line
	7350 3000 7350 3100
$Comp
L Device:D_TVS D3
U 1 1 6386B4AE
P 4850 5300
F 0 "D3" V 4800 5100 50  0000 L CNN
F 1 "ICTE12C-E3/54" V 4900 4600 50  0000 L CNN
F 2 "" H 4850 5300 50  0001 C CNN
F 3 "~" H 4850 5300 50  0001 C CNN
	1    4850 5300
	0    1    1    0   
$EndComp
Wire Wire Line
	4850 4950 4850 5150
Connection ~ 4850 4950
Wire Wire Line
	4850 4950 4950 4950
$Comp
L power:GND #PWR05
U 1 1 6386DC79
P 4850 5700
F 0 "#PWR05" H 4850 5450 50  0001 C CNN
F 1 "GND" H 4855 5527 50  0000 C CNN
F 2 "" H 4850 5700 50  0001 C CNN
F 3 "" H 4850 5700 50  0001 C CNN
	1    4850 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	4850 5450 4850 5700
$Comp
L Device:R R2
U 1 1 63885035
P 4700 2800
F 0 "R2" H 4770 2846 50  0000 L CNN
F 1 "1k" H 4770 2755 50  0000 L CNN
F 2 "" V 4630 2800 50  0001 C CNN
F 3 "~" H 4700 2800 50  0001 C CNN
	1    4700 2800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R1
U 1 1 63888349
P 4400 2800
F 0 "R1" H 4470 2846 50  0000 L CNN
F 1 "3k3" H 4470 2755 50  0000 L CNN
F 2 "" V 4330 2800 50  0001 C CNN
F 3 "~" H 4400 2800 50  0001 C CNN
	1    4400 2800
	1    0    0    -1  
$EndComp
Entry Wire Line
	3750 3400 3850 3300
Wire Wire Line
	3850 3300 4400 3300
Text Label 3900 3300 0    50   ~ 0
S1
Wire Wire Line
	4400 2950 4400 3300
Wire Wire Line
	4550 2350 4550 2500
Wire Wire Line
	4400 2650 4400 2500
Wire Wire Line
	4400 2500 4550 2500
Wire Wire Line
	4550 2500 4700 2500
Connection ~ 4550 2500
Wire Wire Line
	5250 4950 5400 4950
Wire Wire Line
	3850 3200 4050 3200
Wire Wire Line
	4700 2950 4700 3200
$Comp
L power:GND #PWR02
U 1 1 638695AF
P 4050 4550
F 0 "#PWR02" H 4050 4300 50  0001 C CNN
F 1 "GND" H 4055 4377 50  0000 C CNN
F 2 "" H 4050 4550 50  0001 C CNN
F 3 "" H 4050 4550 50  0001 C CNN
	1    4050 4550
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 4450 4050 4550
Wire Wire Line
	4050 4050 4050 4150
$Comp
L Device:D_TVS D2
U 1 1 6386660D
P 4050 4300
F 0 "D2" V 4004 4380 50  0000 L CNN
F 1 "ICTE12C-E3/54" V 4095 4380 50  0000 L CNN
F 2 "" H 4050 4300 50  0001 C CNN
F 3 "~" H 4050 4300 50  0001 C CNN
	1    4050 4300
	0    1    1    0   
$EndComp
Wire Wire Line
	4850 4950 4850 3100
$Comp
L Device:D_TVS D1
U 1 1 638C219E
P 4050 3550
F 0 "D1" V 4004 3630 50  0000 L CNN
F 1 "ICTE12C-E3/54" V 4095 3630 50  0000 L CNN
F 2 "" H 4050 3550 50  0001 C CNN
F 3 "~" H 4050 3550 50  0001 C CNN
	1    4050 3550
	0    1    1    0   
$EndComp
Wire Wire Line
	4050 3200 4050 3400
Wire Wire Line
	4050 3200 4700 3200
$Comp
L power:GND #PWR01
U 1 1 638C4B05
P 4050 3750
F 0 "#PWR01" H 4050 3500 50  0001 C CNN
F 1 "GND" H 4055 3577 50  0000 C CNN
F 2 "" H 4050 3750 50  0001 C CNN
F 3 "" H 4050 3750 50  0001 C CNN
	1    4050 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	4050 3700 4050 3750
Wire Wire Line
	7750 3000 7750 3100
Wire Wire Line
	7750 3100 8300 3100
Wire Wire Line
	8300 3100 8300 2700
Wire Wire Line
	8300 2700 8450 2700
$Comp
L power:+12V #PWR013
U 1 1 638CF191
P 7350 1400
F 0 "#PWR013" H 7350 1250 50  0001 C CNN
F 1 "+12V" H 7365 1573 50  0000 C CNN
F 2 "" H 7350 1400 50  0001 C CNN
F 3 "" H 7350 1400 50  0001 C CNN
	1    7350 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7350 1400 7350 1550
$Comp
L power:-12V #PWR016
U 1 1 638D18B4
P 7400 4650
F 0 "#PWR016" H 7400 4750 50  0001 C CNN
F 1 "-12V" H 7415 4823 50  0000 C CNN
F 2 "" H 7400 4650 50  0001 C CNN
F 3 "" H 7400 4650 50  0001 C CNN
	1    7400 4650
	-1   0    0    1   
$EndComp
Wire Wire Line
	7050 3450 7050 3200
$Comp
L Device:D D5
U 1 1 638DFE8E
P 7050 2600
F 0 "D5" V 7000 2800 50  0000 R CNN
F 1 "1n4007" V 7100 3000 50  0000 R CNN
F 2 "" H 7050 2600 50  0001 C CNN
F 3 "~" H 7050 2600 50  0001 C CNN
	1    7050 2600
	0    -1   1    0   
$EndComp
Wire Wire Line
	7050 3100 7050 2750
Connection ~ 7050 3100
Wire Wire Line
	7050 3100 7350 3100
Wire Wire Line
	7050 2450 7050 2150
Wire Wire Line
	7050 2150 7350 2150
Connection ~ 7350 2150
Wire Wire Line
	7350 2150 7350 2200
Wire Wire Line
	6950 3450 7050 3450
Wire Wire Line
	7350 3550 6950 3550
Connection ~ 7050 3450
Wire Wire Line
	7050 3450 7350 3450
Connection ~ 4050 3200
Connection ~ 4050 4050
$Comp
L Device:R R3
U 1 1 6394030E
P 5400 3100
F 0 "R3" V 5350 2900 50  0000 C CNN
F 1 "1k" V 5350 3300 50  0000 C CNN
F 2 "" V 5330 3100 50  0001 C CNN
F 3 "~" H 5400 3100 50  0001 C CNN
	1    5400 3100
	0    1    1    0   
$EndComp
Wire Wire Line
	4850 3100 5250 3100
Wire Wire Line
	5900 3100 5550 3100
$Comp
L Device:R R4
U 1 1 639483C2
P 5400 3200
F 0 "R4" V 5350 3000 50  0000 C CNN
F 1 "1k" V 5350 3400 50  0000 C CNN
F 2 "" V 5330 3200 50  0001 C CNN
F 3 "~" H 5400 3200 50  0001 C CNN
	1    5400 3200
	0    1    1    0   
$EndComp
Wire Wire Line
	5900 3200 5550 3200
Wire Wire Line
	4700 3200 5250 3200
Connection ~ 4700 3200
$Comp
L Device:R R5
U 1 1 6394E5D8
P 5400 3300
F 0 "R5" V 5350 3100 50  0000 C CNN
F 1 "1k" V 5350 3500 50  0000 C CNN
F 2 "" V 5330 3300 50  0001 C CNN
F 3 "~" H 5400 3300 50  0001 C CNN
	1    5400 3300
	0    1    1    0   
$EndComp
Wire Wire Line
	4400 3300 5250 3300
Connection ~ 4400 3300
Wire Wire Line
	5550 3300 5900 3300
Wire Wire Line
	4050 5400 3850 5400
Wire Wire Line
	4050 5300 3850 5300
Text Label 3900 5300 0    50   ~ 0
PE
Text Label 3900 5400 0    50   ~ 0
M2
Text Label 3900 5500 0    50   ~ 0
S2
$Comp
L Controllino:CONTROLLINO_MINI_PURE M1
U 1 1 63979FCE
P 6450 3750
F 0 "M1" V 5535 3708 50  0000 C CNN
F 1 "CONTROLLINO_MINI_PURE" V 5626 3708 50  0000 C CNN
F 2 "" H 6200 3000 50  0001 C CNN
F 3 "" H 6200 3000 50  0001 C CNN
	1    6450 3750
	0    1    1    0   
$EndComp
$Comp
L Connector:Conn_01x02_Female J4
U 1 1 63869FAB
P 8650 1550
F 0 "J4" H 8550 1700 50  0000 L CNN
F 1 "Stop charge switch" V 8750 1150 50  0000 L CNN
F 2 "" H 8650 1550 50  0001 C CNN
F 3 "~" H 8650 1550 50  0001 C CNN
	1    8650 1550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 1550 7350 1550
Connection ~ 7350 1550
Wire Wire Line
	7350 1550 7350 2150
Wire Wire Line
	8450 1650 5500 1650
Wire Wire Line
	4950 1650 4950 3400
$Comp
L Device:R R6
U 1 1 63886DE2
P 5400 3400
F 0 "R6" V 5350 3200 50  0000 C CNN
F 1 "1k" V 5350 3600 50  0000 C CNN
F 2 "" V 5330 3400 50  0001 C CNN
F 3 "~" H 5400 3400 50  0001 C CNN
	1    5400 3400
	0    1    1    0   
$EndComp
Wire Wire Line
	4950 3400 5250 3400
Wire Wire Line
	5550 3400 5900 3400
$Comp
L Device:R R9
U 1 1 638913CF
P 5500 2000
F 0 "R9" H 5570 2046 50  0000 L CNN
F 1 "1k" H 5570 1955 50  0000 L CNN
F 2 "" V 5430 2000 50  0001 C CNN
F 3 "~" H 5500 2000 50  0001 C CNN
	1    5500 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 1850 5500 1650
Connection ~ 5500 1650
Wire Wire Line
	5500 1650 4950 1650
$Comp
L power:GND #PWR07
U 1 1 63894FD4
P 5500 2300
F 0 "#PWR07" H 5500 2050 50  0001 C CNN
F 1 "GND" H 5505 2127 50  0000 C CNN
F 2 "" H 5500 2300 50  0001 C CNN
F 3 "" H 5500 2300 50  0001 C CNN
	1    5500 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	5500 2300 5500 2150
Wire Bus Line
	2700 2750 2700 3150
Wire Bus Line
	3750 3150 3750 5400
Wire Bus Line
	2700 3150 2700 4550
$EndSCHEMATC
