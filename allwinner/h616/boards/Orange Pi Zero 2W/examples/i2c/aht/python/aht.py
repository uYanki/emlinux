import smbus
import time

class AHTxx:
    def __init__(self, bus=1, address=0x38):
        self.bus = smbus.SMBus(bus)
        self.address = address
        
        time.sleep(0.04)

        # soft reset the sensor
        self.bus.write_byte(self.address, 0x0ba)
        time.sleep(0.02)

        # check if the sensor is calibrated
        if not self._get_status() & 0x08:
            self.calibrate()

    def _get_status(self):
        self.bus.write_byte(self.address, 0x71)
        return self.bus.read_byte(self.address)

    def _calibrate(self):
        self.bus.write_i2c_block_data(self.address, 0xbe, [0x08, 0x00])
        time.sleep(0.01)
        self._wait_while_busy()

    def _wait_while_busy(self):
        while self._get_status() & 0x80:
            time.sleep(0.01)

    def measure(self):
        self.bus.write_i2c_block_data(self.address, 0xac, [0x33, 0x00])
        time.sleep(0.08)
        self._wait_while_busy()

        data = self.bus.read_i2c_block_data(self.address, 0, 7)

        crc = self._crc8(data[:6])

        if crc == data[6]:
            self.humidity = (data[1] << 12)
            self.humidity |= (data[2] << 4)
            self.humidity |= (data[3] >> 4)
            self.humidity /= (1 << 20)
            self.humidity *= 100

            self.temperature = ((data[3] & 0x0f) << 16)
            self.temperature |= (data[4] << 8)
            self.temperature |= data[5]
            self.temperature /= (1 << 20)
            self.temperature *= 200
            self.temperature -= 50
        else:
            self.humidity = None
            self.temperature = None

    def _crc8(self, data):
        crc = 0xff

        for x in data:
            crc ^= x
            for i in range(8):
                if crc & 0x80:
                    crc <<= 1
                    crc ^= 0x31
                else:
                    crc <<= 1
                
        return crc & 0xff

if __name__ == "__main__":
    aht20 = AHTxx(2)
    aht20.measure()

    if aht20.humidity != None:
        print("temperature : %.1f °C" % aht20.temperature)
        print("humidity : %.1f %%" % aht20.humidity)

