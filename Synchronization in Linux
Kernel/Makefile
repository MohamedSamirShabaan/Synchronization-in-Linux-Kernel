obj-m += mod.o	
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	gcc -o leds leds.c
	gcc -o users user.c
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	$(RM) leds
	$(RM) users

