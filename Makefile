CC  = g++
CFLAGS	= -O3 -Wall
TARGET	= octree_sample 
.SUFFIXES:	.cpp .o

all:	$(TARGET)
octree_sample: octree_main.o
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(LIBS) $(LDFLAGS) 
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< 
clean:
	rm -f $(TARGET) *.o *~
