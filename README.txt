Multimonitor support thanks to the Matrix screensaver (see file screensave.cpp
for details) written by J Brown, found here:
http://catch22.net/software/matrix.asp

I make no warranties about this code or what it could potentially do to your
computer, but it should be safe.  You can use this code in any manner you
see fit as long as I don't end up in a lawsuit because of it.


This screensaver is based off of my friend's "Space" Screensaver
for BeOS.

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
's' key: A slower rendering mode which uses finer grained star positioning than 1 pixel, leading to much smoother motion
Home: Increases frame rate
End: Decreases frame rate
Tab: Switches between multiple monitors



History of versions:

1.0 (September 2005) Initial release, just kind of hacked together in one source file, mixing SDL with OpenGL, standalone app with screensaver.

1.1 (May 2006) Added anti-aliasing.

1.2: (May 2006)  Minor optimizations, added help dialog, fixed centering on smaller than 1280x1024 screens.

1.3: (June 2006) Refactored the whole thing into seperate classes for rendering, star movement, settings, etc.  Added the ability to turn on logging.

1.4: (September 2006) Added multi-monitor support.

1.5: (October 2006) Massive optimization effort from 1.4.
	Went a bit overboard on the default compiler flags, causing rendering problems on some systems. -funsafe-math-optimizations works great on some systems, but others gives weirdness.

1.6 (January 2009) Fixing a bunch of things that have bugged me for a while:
	Reworked the inheritence tree, so it's not a big tall single class.  Rendering engine (OpenGL) is now seperate from the generic rendering class, so other rendering engines could be dropped in.  Same with reading/writing the registry.
	Speed and angle are saved as integers (Windows registry limitation), but represented internally as floats.  Increased the multiplier they are saved with so they don't round so noticably.
	Created a new "smooth" scrolling mode.  This mode uses fractional pixel positions to render (instead of jumping from 2 to 3, it might go 2.1 2.7 3.3 etc), but the old mode is 2.5x faster.
	Fixed a bug that was causing a crash with the configure dialog.

1.7 (January 2009) Major optimization from 1.6 of the subpixel rendering mode.  Most of the improvement came from removing redundant calls to floor() and fixing a couple incorrect boundary conditions.
	Fixed the handling of time in the Windows event loop when it takes longer than one tick to draw the screen.
	Fixed a problem with the random number generator.  It generates a random number between 0 and 32768, and I took the modulus 20000, which lead to a major imbalance in stars towards the bottom right side of the screen.

