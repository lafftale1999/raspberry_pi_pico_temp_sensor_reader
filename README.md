# Documentation Links for components

[BME280](https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf)

[Raspberry Pi Pico W](https://datasheets.raspberrypi.com/picow/pico-w-datasheet.pdf)

![Pin out diagram of Pico W](/resources/picow-pinout.svg)

![Circuit diagram for the system](/resources/pico-w-bme280-circuit.png)


### Generating corrects certs

1. Need ca.crt, client.crt and client.key
2. Run the following command in Linux environment:
    
    `chmod +x convert_certs.sh`
    
    `./convert_certs.sh`
3. Compile