# Time relay

## Description

This is a time relay that turns on when the "Start" button is pressed, remains on for a predetermined time.

## Time setup

The time is given by the position of the potentiometer. Minimum duration is 5 seconds, maximum duration is 0ne hour.

## Follow Up

An alternative would be to use a rotary encoder and a display for setting the time. In this case, at power-up, a preset time will be displayed (let's say 15 minutes), the operator having the possibility to change it by rotating the encoder. Since the EEPROM memory allows an limited number of writes, the time will not be memorized to survive after a reset or power break.
Another issue is the increase of the required budget.
