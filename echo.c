#include<stdio.h>
#include<string.h>

//----------------------------------------------------------
//! Command "echo" display a line of text to standard output
//!
//!@param <...text...> list of lines to display
//!
//!@note the key "-n" do not output the trailing newline
//!      number of keys "-n" isn't limited
//----------------------------------------------------------
int main(int argc, char* argv[])
{
    int n_counter = 0;
    if(argc > 1)
    {
        switch(strcmp(argv[1], "-n"))
        {
            case 0: // echo -n ... -n <...text...>
                n_counter = 1;
                for(n_counter = 2; n_counter < argc; ++n_counter)
                    if(strcmp(argv[n_counter], "-n") == 0)
                        continue;
                for(int i = n_counter; i < argc; ++i)
                    if(i == argc - 1)
                        printf("%s", argv[i]);
                    else
                        printf("%s ", argv[i]);
                break;

            default: // echo <...text...>
                for(int i = 1; i < argc; ++i)
                    if(i == argc - 1)
                        printf("%s", argv[i]);
                    else
                        printf("%s ", argv[i]);
                printf("\n");
                break;
        }
    }
    else // echo
        printf("\n");
    return 0;
}
