#!/usr/bin/python
# -*- coding: utf-8 -*-

"""
Class to read out the AM2320 humidity/temperature sensor via a gpio pin (1-wire)
Copyright (C) 2021 C.M. Punter

To use this class the RPi.gpio module is required:
sudo apt-get install rpi.gpio

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

import RPi.GPIO as gpio
import time

class AM2320_1W:
    def __init__(self, channel):
        self.channel = channel

    def measure(self):
        self.humidity = None
        self.temperature = None

        # use gpio numbering
        gpio.setmode(gpio.BCM)
        gpio.setup(self.channel, gpio.OUT, initial=gpio.HIGH)

        # set low for at least 18ms
        gpio.output(self.channel, gpio.LOW)
        time.sleep(18e-3)

        # set high
        gpio.output(self.channel, gpio.HIGH)
        #time.sleep(20e-6)
        
        # switch to input
        gpio.setup(self.channel, gpio.IN, gpio.PUD_UP)

        # we keep track of how long each high signal lasts
        counts = []
        count = 0

        while count < 1000:
            if gpio.input(self.channel) == gpio.HIGH:
                count += 1
            elif count:
                counts.append(count)
                count = 0
        
        gpio.cleanup()

        # interpret the data
        # make sure we have read at least 41 high signals
        if len(counts) < 41:
            return

        # data is contained in the last 40 high signal
        counts = counts[-40:]

        # determine what is a long- and was is a short signal
        # long signals are binary 1, short signals are binary 0
        threshold = (min(counts) + max(counts)) / 2

        # convert bits to list of bytes
        byte_values = [0, 0, 0, 0, 0]
        for i, count in enumerate(counts):
            index = i // 8
            byte_values[index] <<= 1
            byte_values[index] |= count > threshold
        
        # convert bytes to values
        checksum = byte_values[4]
        calculated_checksum = sum(byte_values[:-1]) & 0xff

        if checksum == calculated_checksum:
            self.humidity = byte_values[0] + byte_values[1] * 0.1 
            self.temperature = (byte_values[2] & 0x7f) + byte_values[3] * 0.1
            
            if byte_values[2] & 0x80:
                self.temperature *= -1

if __name__ == "__main__":
    am2320 = AM2320_1W(17) # assume gpio 17
    am2320.measure()

    if am2320.humidity != None:
        print("humidity : %.1f%%" % am2320.humidity)
        print("temperature : %.1fC" % am2320.temperature)

