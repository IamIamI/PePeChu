# PePeChu
The animatronic pepe-pal

This project was meant to be used to run a fabric puppet with a metal/plastic
animatronic skeleton. It contains;
  - An ESP32
  - A non-descript SD card reader
  - A max98357A sound amp
  - A non descript 2W speaker
  - A TP4056 USB-C battery manager / charger
  - 750Amp Lipo
  - For the speaker it needs 5V so added a MT3608 2A
  - For the rest it needs 3V so i put a HT7333-A 3.3V 0.25A in between the 
      TP4056 and the ESP32
  - Two cheap 9G servo's with 180° movement radius. (one for eyes and one for mouth)
  
This is one of the first projects that is this size, so i think i'm not providing enough Amp 
To power everything properly... but i can't test till the code works.


# Code v0.0.1
So far the code produces a PePe webinterface. The eyes, mouth and nose can be clicken on to trigger
the code to run something. 
Independantly i have managed to run audio from the SD card, and i have managed to independantly run 
the servos. However when trying to integrate them i have yet to be able to run everything properly.
I'm not sure yet what the problem is, first issue is that the audio won't start or cuts off short.

