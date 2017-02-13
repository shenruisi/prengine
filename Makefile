#makefile@prengine
#
# This source code is licensed under the BSD style license found in the
# LICENSE file in the root directory of this source tree. An additional grant
# of patent rights can be found in the PATENTS file in the same directory.
CC = g++
LIB_NAME = libprengine
TAR_DIR = ./bin
PACKAGE_DIR = ./package
_DEPS = prengine_val.h cond_ast.h prengine.h
DEPS = $(patsubst %,$(PACKAGE_DIR)/%,$(_DEPS))
_OBJ = prengine_val.o cond_ast.o prengine.o
OBJ = $(patsubst %,$(PACKAGE_DIR)/%,$(_OBJ))
VERSION = 0.1.0-beta.1
IOS_BUILD_DIR = ./build
IOS_SIMULATOR_DIR := $(IOS_BUILD_DIR)/Release-iphonesimulator
IOS_DEVICE_DIR := $(IOS_BUILD_DIR)/Release-iphoneos
export platform = x86

prengine: $(OBJ)
	rm -rf $(TAR_DIR)
	test -d $(TAR_DIR) || mkdir $(TAR_DIR)
	cp $(DEPS) $(TAR_DIR)
ifeq ($(platform), x86)
	ar rcs $(TAR_DIR)/$(LIB_NAME)-$(VERSION).a $^
endif

ifeq ($(platform), ios)
	xcodebuild -project ios-build.xcodeproj ARCHS='i386 x86_64' -sdk iphonesimulator
	xcodebuild -project ios-build.xcodeproj ARCHS='armv7 armv7s arm64' -sdk iphoneos
	lipo -create $(IOS_SIMULATOR_DIR)/libios-build.a $(IOS_DEVICE_DIR)/libios-build.a -output $(TAR_DIR)/$(LIB_NAME)-$(VERSION).a
	rm -rf $(IOS_BUILD_DIR)
endif

	#$(CC) -shared -fPIC $^ -o $(TAR_DIR)/$(DYNAMIC_LIB)
	rm -rf $^

clean:
	rm -rf $(TAR_DIR)
