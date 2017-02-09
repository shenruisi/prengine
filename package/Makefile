#makefile@prengine
#
# This source code is licensed under the BSD style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
CC = g++
LIB_NAME = libprengine
STATIC_LIB := $(LIB_NAME).a
DYNAMIC_LIB := $(LIB_NAME).so
TAR_DIR = ./bin
INCLUDE_DIR := $(TAR_DIR)/include
DEPS = prengine_val.h cond_ast.h prengine.h
OBJ = prengine_val.o cond_ast.o prengine.o

prengine: $(OBJ)
		test -d $(TAR_DIR) || mkdir $(TAR_DIR)
		test -d $(INCLUDE_DIR) || mkdir $(INCLUDE_DIR)
		cp $(DEPS) $(INCLUDE_DIR)
		ar rcs $(TAR_DIR)/$(STATIC_LIB) $^
		$(CC) -shared -fPIC $^ -o $(TAR_DIR)/$(DYNAMIC_LIB)
		rm -rf $^

clean:
		rm -rf $(TAR_DIR)
