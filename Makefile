NAME= Matt_daemon

HEADERS=	Headers/daemon.hpp\
			Headers/Matt_daemon.hpp\
			Headers/Tintin_reporter.hpp\
		
OBJ_PATH= objects

SRC_PATH= Sources

SRCS_NAME=	main.cpp\
			daemon.cpp\
			Matt_daemon.cpp\
			Tintin_reporter.cpp\


OBJS= $(SRCS_NAME:.cpp=.o)
OBJ= $(addprefix $(OBJ_PATH)/,$(OBJS))
SRC= $(addprefix $(SRC_PATH)/,$(SRCS_NAME))

# FLAGS= -Wall -Wextra -Werror
H_FLAG= -I Headers

COMP= clang++ -std=c++17

all: $(NAME) 

$(NAME) : $(OBJ) $(HEADERS)
	@$(COMP) $(H_FLAG) $(OBJ) -o $@ 
	@echo "Compilation of $(NAME):  \033[1;32mOK\033[m"

$(OBJ_PATH)/%.o:  $(SRC_PATH)/%.cpp
	@mkdir -p $(OBJ_PATH) 
	@$(COMP) $(FLAGS) $(H_FLAG) $ -o $@ -c $<

clean:
	@rm -rf $(OBJ_PATH)
	@echo "\033[1;33m>> all objects files are deleted.\033[0m" 

fclean: clean
	@rm -rf $(NAME)
	@echo "\033[0;31m>> $(NAME) is deleted.\033[0m" 

re : fclean all