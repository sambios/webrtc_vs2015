SRC = src
MYARCH:=android-x86
PREFIX:= ../$(MYARCH)
ANDROID_API_LEVEL:=21


ifeq ($(MYARCH), android-arm64)
	ANDROID_CROSS :=$(ANDROID_NDK)/toolchains/aarch64-linux-android-4.9/prebuilt/linux-x86_64/bin/aarch64-linux-android-
	ANDROID_SYSROOT := $(ANDROID_NDK)/platforms/android-$(ANDROID_API_LEVEL)/arch-arm64
	HOST:=aarch64-linux
	ECFLAGS :="-march=armv8-a -D__ARM_ARCH_8__ -D__ARM_ARCH_8A__"
	
else ifeq ($(MYARCH), android-armeabi)
	ANDROID_CROSS := $(ANDROID_NDK)/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-
	ANDROID_SYSROOT := $(ANDROID_NDK)/platforms/android-$(ANDROID_API_LEVEL)/arch-arm	
	HOST:=armv5-linux
	#ECFLAGS :="-march=armv5 -mfloat-abi=softfp -mfpu=neon"
	
else ifeq ($(MYARCH), android-armeabi-v7a)
	ANDROID_CROSS := $(ANDROID_NDK)/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64/bin/arm-linux-androideabi-
	ANDROID_SYSROOT := $(ANDROID_NDK)/platforms/android-$(ANDROID_API_LEVEL)/arch-arm
	HOST:=arm-linux
	ECFLAGS :="-march=armv7-a -mtune=cortex-a8 -mfloat-abi=softfp -mfpu=neon -D__ARM_ARCH_7__ -D__ARM_ARCH_7A__"
	
else ifeq ($(MYARCH), android-mips64)
	ANDROID_CROSS := $(ANDROID_NDK)/toolchains/mip64sel-linux-android-4.9/prebuilt/linux-x86_64/bin/mips64el-linux-android-
	ANDROID_SYSROOT := $(ANDROID_NDK)/platforms/android-$(ANDROID_API_LEVEL)/arch-mips64
	HOST:=mips64-linux
	
else ifeq ($(MYARCH), android-mips)
	ANDROID_CROSS := $(ANDROID_NDK)/toolchains/mipsel-linux-android-4.9/prebuilt/linux-x86_64/bin/mipsel-linux-android-
	ANDROID_SYSROOT := $(ANDROID_NDK)/platforms/android-$(ANDROID_API_LEVEL)/arch-mips	
	HOST:=mips-linux
	
else ifeq ($(MYARCH), android-x86)
	ANDROID_CROSS := $(ANDROID_NDK)/toolchains/x86-4.9/prebuilt/linux-x86_64/bin/i686-linux-android-
	ANDROID_SYSROOT := $(ANDROID_NDK)/platforms/android-$(ANDROID_API_LEVEL)/arch-x86	
	HOST:=i686-linux
	
else ifeq ($(MYARCH), android-x86_64)
	ANDROID_CROSS := $(ANDROID_NDK)/toolchains/x86_64-4.9/prebuilt/linux-x86_64/bin/x86_64-linux-android-
	ANDROID_SYSROOT := $(ANDROID_NDK)/platforms/android-$(ANDROID_API_LEVEL)/arch-x86_64	
	HOST:=x86_64-linux
endif

#CPPFLAGS +="--sysroot=$ANDROID_SYSROOT"
#CFLAGS +="--sysroot=$ANDROID_SYSROOT"
#CXXFLAGS +="--sysroot=$ANDROID_SYSROOT"

all: install
config: clean
	cd $(SRC); ./configure --enable-pic --enable-strip --enable-shared --cross-prefix=$(ANDROID_CROSS) --sysroot=$(ANDROID_SYSROOT) --host=$(HOST) --prefix=$(PREFIX) --extra-cflags=$(ECFLAGS)

build: config
	$(V)make -C $(SRC)

install: build
	$(V)make -C $(SRC) install-lib-shared

clean:
	$(V)make -C $(SRC) clean






