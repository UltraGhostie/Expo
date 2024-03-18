This repo contains a base for running code on a chipkit that I then made the pong game upon. The code requires a chipKit with an IO shield to work properly. All of the code should already be compiled and only needs to be installed, if you want to compile yourself then: IIRC, the compilation software was either custom made or had custom changes made to it. If you want to try anyways or have access to the compiler the compilation instructions are below:

Compilation is through the mcb32tools environment, 
just be in the directory of the code with the terminal
then run . /opt/mcb32tools/environment
then make
and then make install
The code should then be written to the chipKit and run
