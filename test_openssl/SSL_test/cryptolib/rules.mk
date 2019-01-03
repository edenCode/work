#Rules
%.o:%.c
	$(CC) $(CFLAGS) $(INC_DIR) $(LIB_DIR) $(LIBS) -c $< -o $@
%.o:%.cpp
	$(CC) $(CFLAGS) $(INC_DIR) $(LIB_DIR) $(LIBS) -c $< -o $@
