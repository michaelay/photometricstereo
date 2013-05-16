CC=g++
CFLAGS=-c -Wall
LDFLAGS=-lopencv_core \
				-lopencv_imgproc \
				-lopencv_highgui \
				-lopencv_legacy \
				-lboost_date_time-mt \
				-lboost_thread-mt \
				-lboost_system-mt \
				-lglfw \
				-lIL \
				-lILU \
				-lILUT \
				-lvtkCommon \
				-lvtkRendering \
				-lvtkFiltering \
				-framework GLUT \
				-framework OpenGL \
				-framework Cocoa

SOURCES = $(wildcard **/*.cpp)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=PhotometricStereoCpp

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
		$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
		$(CC) $(CFLAGS) $< -o $@

#%: %.o
#		$(CC) -o $@ $^ $(LDFLAGS)
#
#%.o: %.cpp
#		$(CC) -c $(CFLAGS) $<

.PHONY: clean cleanest

clean:
		rm $(wildcard **/*.o)

cleanest: clean
		rm $(EXECUTABLE)
