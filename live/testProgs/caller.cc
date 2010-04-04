/**
 *
 * Ankur's Super Awesome cool utility for world Domination
 * Compile using: 
 *      g++ -g caller.cc $(pkg-config --libs --cflags libcurl) -o caller
 *
 */


#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <curl/curl.h>

size_t ignore_output(void *ptr, size_t size, size_t nmemb, void *stream){
    return size*nmemb;
}

int processRecord(int nameID, int duration){
    char buffer[200];
    printf("\nExec: OpenRTSP capture for 20 secs to temp%d.mp4 .....", nameID);
    fflush(stdout);
    sprintf(buffer, "./openRTSP -4 -d %d rtsp://128.125.91.194/img/video.sav 1>temp%d.mp4 2>/dev/null", duration ,nameID);
    return system(buffer);
}

int processSend(int nameID){
    char buffer[300];
    CURL *curl;
    CURLcode res;
    FILE *hd_src;
    int hd;
    //struct stat file_info;
    struct curl_httppost *post=NULL;
    struct curl_httppost *last=NULL;
    struct curl_slist *headerlist = NULL;
    char file[100];

    char videoID[100];
    char blockID[100];
    
    char url[256];
    //strcpy(url,"http://earth.usc.edu/uploads/upload.php");
    strcpy(url,"http://mslab09.usc.edu:22112/cs599/upload.jsp");
    printf("\nAttempting to send temp%d.mp4 to server .....", nameID);
    fflush(stdout);
    sprintf(file, "temp%d.mp4", nameID);
    
    sprintf(videoID,"%d", nameID);
    sprintf(blockID, "%d", 1);

    /*
    hd = open(file, O_RDONLY);
    fstat(hd, &file_info);
    close(hd);
    */

    hd_src = fopen(file, "rb");

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if(curl){
        // Setup Variables for curl mumbo-jumbo!
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "videoID", CURLFORM_COPYCONTENTS, videoID, CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "blockID", CURLFORM_COPYCONTENTS, blockID, CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "requestType", CURLFORM_COPYCONTENTS, "put", CURLFORM_END);
        curl_formadd(&post, &last, CURLFORM_COPYNAME, "F1", CURLFORM_FILE, file, CURLFORM_END);
        
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
        
        int res = curl_easy_perform(curl); // Do the actual HTTP POST
        curl_easy_cleanup(curl);
        printf("Complete\n");
        fflush(stdout);
    }
    fclose(hd_src);
    curl_global_cleanup();
    return 0;
}

int main(int argc, char **argv){
    opterr = 0;
    int duration=5;
    int c;
    int cyclecount = 10;
    while((c=getopt(argc, argv, "d:n:"))>-1){
        printf("Opt=%c, string=%s\n", c, optarg ); /* Add DEBUG */
        switch(c){
            case 'n':
                if((sscanf(optarg, "%d", &cyclecount)) != 1)
                    printf("No n\n");
                break;
            case 'd': 
                if((sscanf(optarg, "%d", &duration)) != 1)
                    printf("No d\n");
                break;
            case '?':
                fprintf(stdout, 
                "Usage: ./caller [-d duration=5] [-n=10] url\n-d Duration of each recording\n-n Number of recordings\n");
                exit(0);
                break;
            default:
                duration = 5;
                cyclecount=10;
                break;
            }
    }
    // Non option Argument is the url
    do{
        int status;
        int nameID = time(NULL);
        pid_t pid;
        pid_t pid2;
        cyclecount--;
        printf("\nAnkur's Super Awesome cool utility for world Domination\n");
        fflush(stdout);
        pid = fork();
        if(pid == 0){
            return processRecord(nameID, duration);
            status = 0;
        }else if(pid<0){
            // Fork Failed!!
            status = -1;
        }else{
            // Parent Process. Wait for the child to complete
            int w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
            if(w == pid){
                // We finished recording the openRTSP. Now we should for and call
                // send the program
                pid2 = fork();
                if(pid2<0)
                    status = -1;
                else if( pid2 == 0){
                        return processSend(nameID);
                        status = 0;
                    }
                    status = -1;
            }
        }
    }while(cyclecount >= 0);

}
