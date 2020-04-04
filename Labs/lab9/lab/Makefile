ccflags-y = -Wno-unused-function -Wno-unused-label -Wno-unused-variable
obj-m += mod_hrtimer.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean