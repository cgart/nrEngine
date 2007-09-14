CC=g++

INCLUDES = -I$(top_srcdir)/Plugins/PLUGIN_DIR/ -I$(top_srcdir)/nrEngine/include
CFLAGS= -O2 -Wall -Werror

OBJECTS= Plugin.o


NAME= Plugin.so


	
all: $(NAME)

$(OBJECTS): %.o: %.cpp
	$(CC) $(CFLAGS) -o $@ -c $< $(INCLUDES)

Plugin.so: $(OBJECTS)
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
