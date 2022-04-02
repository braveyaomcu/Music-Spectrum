SET C51INC=D:\Keil\C51\Inc;D:\Keil\C51\Inc\STC
SET C51LIB=D:\Keil\C51\Lib
SET CPU_TYPE=STC12C5A60S2
SET CPU_VENDOR=STC
SET UV2_TARGET=Target 1
SET CPU_XTAL=0x016E3600
"D:\Keil\C51\BIN\A51.EXE" @STARTUP._ia
"D:\Keil\C51\BIN\C51.EXE" @main.__i
"D:\Keil\C51\BIN\C51.EXE" @display.__i
"D:\Keil\C51\BIN\C51.EXE" @misc.__i
"D:\Keil\C51\BIN\C51.EXE" @system.__i
"D:\Keil\C51\BIN\C51.EXE" @fft.__i
"D:\Keil\C51\BIN\BL51.EXE" @project.lnp
"D:\Keil\C51\BIN\OH51.EXE" "project" 
