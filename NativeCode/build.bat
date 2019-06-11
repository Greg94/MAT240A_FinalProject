@echo off
del testString.dll
cl /LD testString.cpp AudioPluginUtil.cpp libtcc.lib
del testString.exp
del testString.lib
del testString.obj
