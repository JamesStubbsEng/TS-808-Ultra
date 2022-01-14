# TS-808-Ultra

Model of the classic Ibanez Tube Screamer 808 using Nodal Analysis and WDF with a few new features added!
![TS-808-Ultra(Screenshot)](https://user-images.githubusercontent.com/64380573/149594162-f6165671-9757-4dc0-9076-938c92f93ed2.PNG)

## Basis of circuits simulated 
The circuit schematics of the TS-808-Ultra are based off Electrosmash's analysis here:
https://www.electrosmash.com/tube-screamer-analysis

### Clipping Circuit
Jatin Chowdury's WDF library was used to simulate the clipping stage of the TS808.
The library is found here: https://github.com/jatinchowdhury18/WaveDigitalFilters

The clipping stage of the TS808 can be broken down into 3 WDF stages based on ideal opamp circuit analysis.
This is done in the following paper and is shown below:
![WDF_implementation_of_TS808](https://user-images.githubusercontent.com/64380573/149594133-6f77cf1b-f5f6-4c23-8f75-2c05b2dd94b3.PNG)

https://www.researchgate.net/publication/299514713_An_Improved_and_Generalized_Diode_Clipper_Model_for_Wave_Digital_Filters

### Tone Circuit
The tone circuit analog transfer function can be found by nodal analysis, and is shown in section 4.3 on page 6 of the paper below:
http://dafx.de/paper-archive/2007/Papers/p189.pdf

## Other features
To make the iconic tube screamer circuit more usable in applications other than in front of a guitar amp the following is added to the plugin: input gain, mix, direct LFP, direct compression, and output gain! These extra features are implemented in the digital domain.

## Pots
### Drive Pot
The drive pot is modelled with the following function, which emulates a classic audio taper pot:
![audiotaper](https://user-images.githubusercontent.com/64380573/149594797-da323c4f-72a4-48d6-91e9-3cb2d49bbc05.PNG)

### Tone Pot
According to Electrosmash's analysis, the physical Tube Screamer uses a G taper pot. However, during development, it was found an inverse log taper allows for more control of the upper settings, so the inverse log taper pot emulation below is used in the plugin.
![inverselogtaper](https://user-images.githubusercontent.com/64380573/149595145-69b3351c-b309-4108-aeef-f4625535a0fb.PNG)
