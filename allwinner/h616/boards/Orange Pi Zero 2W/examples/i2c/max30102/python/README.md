# max30102
MAX30102 Pulse Oximetry Sensor

```
sudo apt install python-smbus python-numpy
```

## Use as a script

Run `python main.py`, data will be printed in standard output. 

The full usage:

```
$ python main.py -h
usage: main.py [-h] [-r] [-t TIME]

Read and print data from MAX30102

optional arguments:
  -h, --help            show this help message and exit
  -r, --raw             print raw data instead of calculation result
  -t TIME, --time TIME  duration in seconds to read from sensor, default 30
```

## Use as a library
To use the code, instantiate the `HeartRateMonitor` class found in `heartrate_monitor.py`.
The thread is used by running `start_sensor` and `stop_sensor`. While the thread
is running you can read `bpm` to get the active beats per minute. Note that a few
seconds are required to get a reliable BPM value and the sensor is very sensitive
to movement so a steady finger is required!

