TARGET=lo
CC=g++ -std=c++11
PATH=./src
SOURCE=$(shell find $(PATH) -name "*.cpp")
HEADER=$(shell find $(PATH) -name "*.h")
OBJ=$(SOURCE:%.cpp=%.o)

$(TARGET):$(OBJ)
	$(CC) -o $(PATH)/$(TARGET) $(OBJ)


%.o:%.cpp $(HEADER)
	$(CC) -c  $< -o $@

clean:
	rm -rf $(OBJ) $(PATH)/lo
