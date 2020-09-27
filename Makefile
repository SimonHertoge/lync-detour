INCS = packages\Detours.4.0.1\lib\native\include\ 
LIBS = packages\Detours.4.0.1\lib\native\libs\x86\

all: loader detour

loader: loader\loader.cpp
    cl /I $(INCS) $** /MT \
    /link /LIBPATH:$(LIBS)

detour: detour\detour.cpp
    cl /I $(INCS) $** advapi32.lib /MT \
    /link /LIBPATH:$(LIBS) /DLL \
    /export:DetourFinishHelperProcess,@1,NONAME \
    /OUT:$@.dll
