What is it
----------

In short, Stacksmith is a HyperCard clone that runs on the current version of Mac OS X.


How to build
------------

Make sure you've inited all the submodules, and then just build the Stacksmith Xcode project. If you want to contribute, note that you'll probably have to change the subprojects to point to your own clones of the subprojects. These instructions might be helpful: http://365git.tumblr.com/post/12581972612/set-up-a-submodule-where-only-you-have-write-access


Debugging Hammer code
---------------------

Stacksmith's scripting language (its analog to HyperTalk) is called Hammer. There is a source-level debugger for it in the ForgeDebugger project. To use it, *first* start ForgeDebugger, *then* Stacksmith. Then put a "debug checkpoint" command on the line of your script where you want to start debugging. Stacksmith will connect to the separate ForgeDebugger application and display script text and bytecode in it, as well as local variables and the call stack, and even the stack where temporary variables live.


What is what
------------

Stacksmith consists of several parts:

* Stacksmith - the Hypercard clone proper
* ForgeDebugger - the debugger. Yes, it's currently a separate application. This will change eventually.
* Forge - the part that reads scripts and compiles them into a special bytecode. This is written in C++, but its public API consists of a handful of C functions with a LEO prefix. [Cross platform]
* Leonie - the part that runs bytecode (i.e. the "compiled" scripts generated by Forge). All of the functions in there are prefixed with LEO [Cross platform]
* stackimport - A command-line tool that Stacksmith uses to import HyperCard stacks. [Almost cross platform]
* Sparkle, UKPaintView, UKSyntaxColoredTextDocument, UliKit, UKMelodyQueue, ULINetSocket - Mac-specific helper code used to implement various parts of Stacksmith.


License
-------

Note: Some submodules may be subject to different licenses.

	Copyright 2003-2012 by Uli Kusterer.
	
	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.
	
	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:
	
	   1. The origin of this software must not be misrepresented; you must not
	   claim that you wrote the original software. If you use this software
	   in a product, an acknowledgment in the product documentation would be
	   appreciated but is not required.
	
	   2. Altered source versions must be plainly marked as such, and must not be
	   misrepresented as being the original software.
	
	   3. This notice may not be removed or altered from any source
	   distribution.
