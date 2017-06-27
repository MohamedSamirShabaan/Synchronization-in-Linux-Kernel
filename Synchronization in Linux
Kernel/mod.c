#include <linux/kobject.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/semaphore.h>

// file names
static int scroll;
static int num;
static int caps;

// semaphore
static struct semaphore semaCaps;
static struct semaphore semaScroll;
static struct semaphore semaNum;
static struct semaphore sema;

static int COUNTER_VALUE_CAPS = 1;
static int COUNTER_VALUE_NUM = 1;
static int COUNTER_VALUE_SCROLL = 1;
static int COUNTER_VALUE_SEMA = 1;

// Functions prototype and varriable

static unsigned char led_status_word = 00000000;
void set_led_state(int led,int state);
int get_led_state(int led);
unsigned char kbd_read_status(void);
unsigned char kbd_read_data(void);
void kbd_write_data(unsigned char data);
int update_leds(unsigned char led_status_word);


// to change the global varriable led_status_led
void set_led_state(int led,int state)
{
    printk("PID -->> %d in set led state Function \n",current->pid);
    if(led == 0 || led == 1 || led == 2) // check if led 0 || 1 || 2 otherwise  >> error to change
    {
        if(state == 0)
        {
            printk("PID -->> %d trun the led = %d off\n",current->pid,led);
            led_status_word &= ~(1 << led);
            //turn off led
        }
        else
        {
            printk("PID -->> %d trun the led = %d on\n",current->pid,led);
            led_status_word |= 1 << led;
            //turn on led
        }
        // update the state
        int ret = update_leds(led_status_word);
        if (ret == -1)
        {
            printk("PID -->> %d unsuccess to update_leds\n",current->pid);
        }
    }

}

// get led state on or off using global varriable led_status_word
int get_led_state(int led)
{
    printk("PID -->> %d in get_led_state Function get led = %d state\n",current->pid, (led_status_word >> led) & 1);
    return (led_status_word >> led) & 1;
}


// read using inb function
unsigned char kbd_read_status(void)
{
    printk("PID -->> %d in kbd_read_status Function\n",current->pid);
    return inb(0x64);
}

// check the output buffer of full read from it other wise wait to be full
unsigned char kbd_read_data(void)
{
    printk("PID -->> %d in kbd_read_data Function \n",current->pid);
    unsigned char status = kbd_read_status();
    while( ( (status >> 0) & 1 ) == 0)
    {
        printk("PID -->> %d output buffer is still clear \n",current->pid);
        status = kbd_read_status();
    }
    printk("PID -->> %d output bffer if full (PS/2) sent data \n",current->pid);
    printk("PID -->> %d Read data using inb(0x60) \n",current->pid);
    return inb(0x60);
}

// check the input buffer if clear write data using outb() other wise wait to be clear
void kbd_write_data(unsigned char data)
{
    printk("PID -->> %d in kbd_write_data Function \n",current->pid);
    unsigned char status = kbd_read_status();
    while( ( (status >> 1) & 1 ) == 1)
    {
        printk("PID -->> %d input buffer is full \n",current->pid);
        status = kbd_read_status();
    }
    printk("PID -->> %d input buffer is clear and ready to recieve data \n",current->pid);
    printk("PID -->> %d write date using outb(data , 0x60) \n",current->pid);

    /**
     change wrtie outb(data , 0X60)
    if (current->pid % 2 == 1){
     printk("PID -->> %d sleep\n",current->pid);
    msleep(0.5);
    printk("PID -->> %d wake up\n",current->pid);
    }
    */

    outb(data, 0x60);
}

// to update leds
int update_leds(unsigned char led_status_word)
{

    printk("PID -->> %d in update_leds Function \n",current->pid);
    down(&sema);
    disable_irq(1);
    printk("PID -->> %d disable_irq(1) \n",current->pid);
// send ’Set LEDs’ command
    printk("PID -->> %d send ’Set LEDs’ command \n",current->pid);
    kbd_write_data(0xED);

// wait for ACK

    /**
    Race condition send 0xED.....

    printk("PID -->> %d sleep\n",current->pid);
    msleep(0.5);
    printk("PID -->> %d wake up\n",current->pid);
    */
    printk("PID -->> %d wait for first ACK from kbd \n",current->pid);
    if (kbd_read_data() != 0xFA)
    {
        printk("PID -->> %d first ACK is (failuer) \n",current->pid);
        up(&sema);
        return -1;
    }
    printk("PID -->> %d first ACK is (success) \n",current->pid);

    /**
    change 0xED data on >> off , off >> on
    printk("PID -->> %d sleep\n",current->pid);
    msleep(0.5);
    printk("PID -->> %d wake up\n",current->pid);
    */

// now send LED states
    printk("PID -->> %d now send LED states\n",current->pid);
    kbd_write_data(led_status_word);

    /**
     enable irq(1) || send led states
    printk("PID -->> %d sleep\n",current->pid);
    msleep(0.5);
    printk("PID -->> %d wake up\n",current->pid);
    */


// wait for ACK
    printk("PID -->> %d wait for another ACK from kbd \n",current->pid);

    if (kbd_read_data() != 0xFA)
    {
        printk("PID -->> %d another ACK is (failuer) \n",current->pid);
        up(&sema);
        return -1;
    }
    printk("PID -->> %d another ACK is (success) \n",current->pid);
    enable_irq(1);
    printk("PID -->> %d enable_irq(1) \n",current->pid);
// success
    up(&sema);
    printk("PID -->> %d update is successed is (success) \n",current->pid);
    return 0;
}


// read scroll state from scroll's file store in /sys/ direction
static ssize_t scroll_show(struct kobject *kobj, struct kobj_attribute *attr,
                           char *buf)
{

    printk("PID -->> %d read the state from scroll\n",current->pid);
    down(&semaScroll);
    ssize_t ret = sprintf(buf, "%s\n", get_led_state(0) == 1 ? "on" : "off");
    up(&semaScroll);
    return ret;
}

// store scroll state in scroll's file store in /sys/ direction
static ssize_t scroll_store(struct kobject *kobj, struct kobj_attribute *attr,
                            const char *buf, size_t count)
{
    down(&semaScroll);
    int ret;
    ret = kstrtoint(buf, 10, &scroll);
    if (ret < 0)
    {
        up(&semaScroll);
        return ret;
    }
    printk("PID -->> %d store the new state = %d in scroll file \n",current->pid,scroll);
    set_led_state(0, scroll);
    up(&semaScroll);
    return count;
}

static struct kobj_attribute scroll_attribute =
    __ATTR(scroll, 0664, scroll_show, scroll_store);


// read num state from num's file store in /sys/ direction
static ssize_t num_show(struct kobject *kobj, struct kobj_attribute *attr,
                        char *buf)
{
    printk("PID -->> %d read the state from num\n",current->pid);
    down(&semaNum);
    ssize_t ret = sprintf(buf, "%s\n", get_led_state(1) == 1 ? "on" : "off");
    up(&semaNum);
    return ret;
}

// store num state in num's file store in /sys/ direction

static ssize_t num_store(struct kobject *kobj, struct kobj_attribute *attr,
                         const char *buf, size_t count)
{
    down(&semaNum);
    int ret;
    ret = kstrtoint(buf, 10, &num);
    if (ret < 0)
    {
        up(&semaNum);
        return ret;
    }
    printk("PID -->> %d store the new state = %d in num file \n",current->pid,num);
    set_led_state(1, num);
    up(&semaNum);
    return count;
}

static struct kobj_attribute num_attribute =
    __ATTR(num, 0664, num_show, num_store);

// read caps state from caps's file store in /sys/ direction

static ssize_t caps_show(struct kobject *kobj, struct kobj_attribute *attr,
                         char *buf)
{
    printk("PID -->> %d read the state from caps\n",current->pid);
    down(&semaCaps);
    ssize_t ret = sprintf(buf, "%s\n", get_led_state(2) == 1 ? "on" : "off");
    up(&semaCaps);
    return ret;
}
// store caps state in caps's file store in /sys/ direction

static ssize_t caps_store(struct kobject *kobj, struct kobj_attribute *attr,
                          const char *buf, size_t count)
{
    down(&semaCaps);
    int ret;
    ret = kstrtoint(buf, 10, &caps);
    if (ret < 0)
    {
        up(&semaCaps);
        return ret;
    }
    printk("PID -->> %d store the new state = %d in caps file \n",current->pid,caps);
    set_led_state(2, caps);
    up(&semaCaps);
    return count;
}

static struct kobj_attribute caps_attribute =
    __ATTR(caps, 0664, caps_show, caps_store);

static struct attribute *attrs[] =
{
    &scroll_attribute.attr,
    &num_attribute.attr,
    &caps_attribute.attr,
    NULL,	/* need to NULL terminate the list of attributes */
};

static struct attribute_group attr_group =
{
    .attrs = attrs,
};


static struct kobject *example_kobj;

static int __init example_init(void)
{
    int retval;
    sema_init(&semaCaps, COUNTER_VALUE_CAPS);
    sema_init(&semaNum, COUNTER_VALUE_NUM);
    sema_init(&semaScroll, COUNTER_VALUE_SCROLL);
    sema_init(&sema, COUNTER_VALUE_SEMA);
    example_kobj = kobject_create_and_add("kobject_example", kernel_kobj);
    if (!example_kobj)
        return -ENOMEM;

    retval = sysfs_create_group(example_kobj, &attr_group);
    if (retval)
        kobject_put(example_kobj);

    return retval;
}

static void __exit example_exit(void)
{
    kobject_put(example_kobj);
    //printk( "led Status Word ==> %2X\n", led_status_word );
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Greg Kroah-Hartman <greg@kroah.com>");

