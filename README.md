# Voip++

This is a __VERY__ dumb UDP voip application developed for educational purposes.
It makes so many assumptions you are better off yelling at the top of your lungs and 
expecting the other person to hear you.

It assumes that the data layout expected by the audio device on the other end 
is the same as the one your mic is outputting, makes no attempt to timestamp 
audio data sent through the network, sends raw mic data to the recipient and has the 
target ip hardcoded to "127.0.0.1". In a nutshell, if you want to talk to yourself 
through the same device this is the application for you.

Uses SDL for audio and Boost for networking.