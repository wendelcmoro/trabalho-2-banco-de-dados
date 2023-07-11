NAME	=	escalona
CXX		=	g++
DEBUG		+=	-Wall -g
CXXFLAGS	+=	-std=c++17 -O2 $(DEBUG)

EXE			=	$(NAME)
CFILES		=	$(wildcard *.c)
CXXFILES	=	$(wildcard *.cpp)
OBJECTS		=	$(CFILES:.c=.o) $(CXXFILES:.cpp=.o)

all: $(NAME)
	@echo "Building..."

$(NAME): $(NAME).cpp
	$(CXX) $^ $(CXXFLAGS) -o $@

# $(EXE) : $(OBJECTS)
# 	$(LD) $^ $(CFLAGS) $(LDFLAGS) -o $@

# %.o : %.c
# 	$(CC) $(CFLAGS) -c $< -o $@

# %.o : %.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :	
	rm -rf $(NAME)
