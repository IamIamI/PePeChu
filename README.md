# PePeChu - WIP (Feb-2023)
The animatronic pepe-pal

This project was meant to be used to run a fabric puppet with a metal/plastic
animatronic skeleton. 

![alt text](https://github.com/IamIamI/PePeChu/blob/main/images/IMG_20221101_183807.jpg?raw=true)

It was build as followed;
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

<img src="https://github.com/IamIamI/PePeChu/blob/main/Robo_pepev2_bb.jpg" width="50%"/>

# Code v0.0.1
So far the code produces a PePe html5 based webinterface. The eyes, mouth and nose can be clicken on to trigger
the code to run something (the red transparant boxes were just added for visual illustration on where the clickable areas are. 
The idea is that the browser updates the individual images for each face element, and corresponds
the press with a movement of the servos. The text box is intended for a bare-bones Text-2-speech function build into the library.

![alt text](https://github.com/IamIamI/PePeChu/blob/main/images/PePeChu_interface_def.jpg?raw=true)![alt text](https://github.com/IamIamI/PePeChu/blob/main/images/PePeChu_interface_interact.jpg?raw=true)

Independantly i have managed to run audio from the SD card, and i have managed to independantly run 
the servos. The T2S works independantly too, however not as smooth as i hoped.
The integration of all elements together however causes issues.
I'm not sure yet what the problem is, first issue is that the audio won't start or cuts off short.

At this point the project is abandoned, but will be resumed with a new puppet as i think the setup is just too "jank" atm and i have very little space to properly build something. 

