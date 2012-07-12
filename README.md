ClipGrab
========

A very simple Windows Clipboard Grabber. Pure C/Win32

Overview
--------

This is a simple Windows Clipboard Grabber. It attaches itself onto the chain of Clipboard listeners
and places the raw contents of the clipboard into a textarea. It only listens for text and not for
other types of clipboard items like files or images.

It is written in C however I have always compiled in C++. It uses direct Win32 APIs. There is basic file
save functionality. The textarea cannot be editing. This is intended to be more of a visual logging utility.