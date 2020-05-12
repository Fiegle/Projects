obj-m += my_timer.o

all:
	make -C /lib/modules/4.19.98/build M=/usr/src/my_timer modules

clean:
	make -C /lib/modules/4.19.98/build M=/usr/src/my_timer clean
