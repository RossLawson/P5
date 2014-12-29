/*
 * recover.c
 * Computer Science 50
 * Problem Set 5
 * Recovers JPEGs from a forensic image.
 */
 
//Include libs

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

//Declare uints

uint8_t block[4];
uint8_t buffer[512];
uint8_t jpgident_a[4] = {0xff, 0xd8, 0xff, 0xe0};
uint8_t jpgident_b[4] = {0xff, 0xd8, 0xff, 0xe1};

//Declare primitive variables

int counter = 0;
int open = 0;

/*main function*/

int main(int argc, char* argv[])
{   
    
    //Open file: memory card
    FILE* memcard = fopen("card.raw","r");
    
    //Error Check: ensure file opened
    if(memcard == NULL)
    {
        fclose(memcard);
        printf("ERROR: Unable to open file!");
        return 1;
    }
    
    //Continue if file opens correctly...
 
    //Creat output file
    FILE* out;

    //Read buffer
    fread(buffer, 512, 1, memcard);
    
    //Repeat until end of card (while)
    while (fread(buffer, 512, 1, memcard) > 0)
    {
        //Add 4 bytes to check...
        for(int i = 0; i < 4; i++)
        block[i] = buffer[i];
        
        //Check if those 4 bytes contain jpg ident
        if((memcmp(jpgident_a, block, 4) == 0) || (memcmp(jpgident_b, block, sizeof(block)) == 0))   
        {
            //If it does, create a new jpg file...
            char outfilename[8];
            sprintf(outfilename, "%03d.jpg",counter);
            
            if (open == 0)
            {
                out = fopen (outfilename, "w");
                fwrite(buffer, sizeof(buffer),1,out);
                open = 1;
            }
            if (open == 1)
            {
                counter++;
                
                fclose(out);
                out = fopen (outfilename, "w");
                fwrite(buffer, sizeof(buffer),1,out);
                
            }
        }
        else
        {
            if (open == 1)
            fwrite(buffer, sizeof(buffer), 1, out);
        }
    }
    
    if(out)
    fclose(out);
    
    //Close mem card
    fclose(memcard);
    printf("%i photos recovered", counter);
    return 0;
}
