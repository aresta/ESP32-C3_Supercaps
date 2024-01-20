# ESP32-C3 Supercapacitor - Solar project
## ESP32-C3 weather station powered only with a supercapacitor and a Solar Panel

The ESP32-C3 is nice RISC-V microcontroller with relatively low power consumption.  This project demonstrates how to run it with just a 10F supercapacitor and a small solar panel (8 x 6 cms), without any battery.

The setup and code is very simple. The microcontroller stays in deep sleep most of the time. It wakes up every 15 minutes to take the weather measures from the BME280 sensor ( temperature, humidity, pressure) and the voltage of the supercap. 

The measures are stored in the RTC memory and are only sent via WIFI if the charge of the supercap is at 95% or after a defined number of hours. This way the system can run many hours without any sun.



<img src="img/ESP32-C3_supercaps.jpg" alt="ESP32-C3 supercaps" style="width:400px;"/>


