#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// get state of led using system fuction from file in sys dirctory
void get(char* arg)
{

    char *temp = "cat /sys/kernel/kobject_example/";

    char * str3 = (char *) malloc(1 + strlen(temp)+ strlen(arg) );
    strcpy(str3,  temp);
    strcat(str3, arg);
    system(str3);
}

// set state of led using system fuction in file in sys dirctory
void set(char* arg, char * args)
{
    char* test = NULL;
    if (strncmp(arg, "on",2) == 0)
    {
        test = "echo \"1\" > /sys/kernel/kobject_example/";
    }
    else if (strncmp(arg, "off",3) == 0)
    {
        test = "echo \"0\" > /sys/kernel/kobject_example/";
    }
    char *str = (char *) malloc(1 + strlen(test)+ strlen(args) );
    strcpy(str,  test);
    strcat(str, args);
    system(str);
}


// main funtion to show chooise to the user
int main()
{
    printf("Hello !!!! \n");
    int temp, choose;
    do
    {
        printf("1- num\n");
        printf("2- caps\n");
        printf("3- scroll\n");
        printf("4- exit\n");
        scanf("%d",&temp);
        if(temp == 1)
        {
            printf("1 - num on\n");
            printf("2 - num off\n");
            printf("3 - get num state\n");
            scanf("%d",&choose);
            if (choose == 1) // num
            {
                set("on", "num"); // set num on
            }
            else if (choose == 2)
            {
                set("off", "num"); // set num off
            }
            else if (choose == 3)
            {
                get("num"); // get num state
            }
            else
            {
                printf("error\n");
            }
        }
        else if(temp == 2)
        {
            printf("1 - caps on\n");
            printf("2 - caps off\n");
            printf("3 - get caps state\n");
            scanf("%d",&choose); // caps
            if (choose == 1)
            {
                set("on", "caps"); // set caps on
            }
            else if (choose == 2)
            {
                set("off", "caps"); // set caps off
            }
            else if (choose == 3)
            {
                get("caps"); // get caps state
            }
            else
            {
                printf("error\n");
            }
        }
        else if(temp == 3)
        {
            printf("1 - scroll on\n");
            printf("2 - scroll off\n");
            printf("3 - get scroll state\n");
            scanf("%d",&choose);
            if (choose == 1) // scroll
            {
                set("on", "scroll"); // set scroll on
            }
            else if (choose == 2)
            {
                set("off", "scroll"); // set scroll off
            }
            else if (choose == 3)
            {
                get("scroll"); // get scroll state
             }
            else
            {
                printf("error\n");
            }
        }
        else if(temp == 4)
        {
            printf("Goodbuy !!!! \n");
            return 0;
        }
        else
        {
            printf("Not valid!!\n");
        }
        temp = 0;
        choose = 0;
    }
    while(1);

    return 0;

}

