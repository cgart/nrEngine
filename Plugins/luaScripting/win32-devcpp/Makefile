CC=g++

INCLUDES = -I../ -I../../.. -I../../../../boost-original/boost_1_33_1 -I../../../nrEngine/include -I../lua
CFLAGS= -shared -O2 -Wall -Werror
LIBS = ../lua/lua51.dll ../../../win32-devcpp/libnrEngine.a
OBJECTS= ../Plugin.o ../Script.o ../ScriptLoader.o ../ScriptConnector.o


NAME= luaScripting.dll


	
all: $(NAME)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

luaScripting.dll: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJECTS) $(LIBS) $(LDFLAGS) $(PLUGIN_FLAGS)

	
install-exec-am:
install:
	cp $(NAME) $(DESTDIR)$(pkglibdir)

uninstall-am:
uninstall:
	rm -f $(DESTDIR)$(pkglibdir)/$(NAME)
	
clean: 
	rm -f core.* core $(OBJECTS)
	rm -f *~
	rm -f DEADJOE
	rm -f *.bb *.bbg *.da *.gcov *.out
	rm -f $(NAME)
