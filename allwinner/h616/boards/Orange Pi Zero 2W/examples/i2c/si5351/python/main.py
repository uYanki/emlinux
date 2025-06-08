from Si5351 import Si5351
from i2cbus import I2CBUS

si = Si5351(busnum = 2)

"Output #0:"
# vco = 25 MHz * (24 + 2 / 3) = 616.67 MHz
si.setupPLL(si.PLL_A, 24, 2, 3)
# out = 616.67 MHz / 45 = 13.703704 MHz 
si.setupMultisynth(0, si.PLL_A, 45)
# out = 13.703704 Mhz / 64 = 214.120375 kHz
si.setupRdiv(0, si.R_DIV_64)

"Output #1:"
# vco = 25 MHz * (20 + 5 / 3) = 541.666666 MHz
si.setupPLL(si.PLL_B, 20, 5, 3)
# out = 541.666666 MHz / 10 = 54.166666 MHz
si.setupMultisynth(1, si.PLL_B, 10)
# out = 54.166666 MHz / 128 = 423.177078 kHz
si.setupRdiv(1, si.R_DIV_128)

si.enableOutputs(True)