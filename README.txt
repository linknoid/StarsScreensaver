By Bryce Wagner  
brycew at wawonastaff.org

Multimonitor support thanks to the Matrix screensaver (see file screensave.cpp
for details) written by J Brown, found here:
http://catch22.net/software/matrix.asp

I make no warranties about this code or what it could potentially do to your
computer, but it should be safe.  You can use this code in any manner you
see fit as long as I don't end up in a lawsuit because of it.

I probably won't release many more versions, but if I do, they will be posted
at http://linknoid.net/software/


This screensaver is based off of my friend David Enderson's "Space" Screensaver
for BeOS, which can be found here: http://www.bebits.com/app/303

There are two programs, one is a standalone and requires an SDL.dll (not
included).  But the main point of this is a Windows screensaver.

Installation:

Right click on the stars.scr file and choose "Install".  That's it.  I've
tested it under Windows XP and Windows 2000, but I have no idea if it will
work on Win9X operating systems, and I have no way to test it.

Usage:

The following keys can be used while the screensaver is active:

Up and down arrows:  Change the speed of the stars.
Left and right arrows: Change the direction of the stars.
PageUp and PageDown: Change the number of stars.
Keypad + and -: Change the size of stars.
Delete: Resets all the above parameters to default values.
A key: Turns anti-aliasing on and off.  Turning it on uses more processing power to make it look better, so if it is running jerky try turning off antialiasing.
Tab: Switches between multiple monitors



OTHER INFO (NOT RELEVANT FOR MOST PEOPLE):

The main purpose of the standalone version is for development and debugging 
purposes.  Most people won't ever need to worry about it.  It requires you to
get and compile SDL before you can use it.  It takes up to 3 optional
parameters on the command line.  The first is the number of stars to use.  The
second and third are screen resolution.

Example:
  stars.exe 10000 800 600
creates an 800x600 window with 10,000 stars.

