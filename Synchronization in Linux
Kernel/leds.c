#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// get function to get the led's state
void get(int argc, char* arg)
{
    if (argc > 3)
    {
        printf("erererer > 3\n");
        return;
    }
    char *temp = "cat /sys/kernel/kobject_example/";

    char * str3 = (char *) malloc(1 + strlen(temp)+ strlen(arg) );
    strcpy(str3,  temp);
    strcat(str3, arg);
    system(str3);
}

// to set the state of specific led
void set(int argc, char* arg, char * args)
{
    if (argc > 4)
    {
        printf("error command > 4\n");
        return;
    }
    char* test = NULL;
    if (strncmp(arg, "on",2) == 0)
    {
        test = "echo \"1\" > /sys/kernel/kobject_example/";
    }
    else if (strncmp(arg, "off",3) == 0)
    {
        test = "echo \"0\" > /sys/kernel/kobject_example/";
    }
    else
    {
        printf("error in argv[4]\n");
        return;
    }

    char *str = (char *) malloc(1 + strlen(test)+ strlen(args) );
    strcpy(str,  test);
    strcat(str, args);
    system(str);
}

// read argument from terminal
int main(int argc, char* const argv[])
{
    if(argc > 1)
    {
        if (strcmp(argv[1], "get") == 0)
        {
            get(argc,argv[2]); // get function
        }
        else if (strcmp(argv[1], "set") == 0)
        {
            set(argc,argv[3], argv[2]);  // set function
        }
        else
        {
            printf("error in get or set \n");
            return 0;
        }
    }
    else
    {
        printf("error missing command\n");
    }

    return 0;
}


