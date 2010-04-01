#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <curl/curl.h>

size_t read_callback(void *ptr, size_t size, size_t nmemb, void *stream){
    size_t retcode;
    retcode = fread(ptr, size, nmemb, (FILE*) stream);
    return retcode;
}

int processRecord(int nameID ){
    char buffer[200];
    printf("\nExec: OpenRTSP capture for 20 secs to temp%d.mp4 .....", nameID);
    fflush(stdout);
    sprintf(buffer, "./openRTSP -4 -d 20 rtsp://128.125.91.194/img/video.sav 1>temp%d.mp4 2>/dev/null", nameID);
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
        curl_easy_setopt(curl, CURLOPT_URL, "http://mslab09.usc.edu:22112/cs599/upload.jsp");
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
    int status;
    /*pid_t pid;
    pid_t pid2;
    pid = fork();
    if(pid == 0){
        processRecord(1011);
        status 0;
    }
    else if(pid<0){
        // Fork Failed!!
        status = -1;
    }else{
        // Parent Process. Wait for the child to complete
        if(waitpid( pid, &status, 0) != pid){
            // We finished recording the openRTSP. Now we should for and call
            // the 
            status = -1;
        }
    }*/
    int nameID = time(NULL);
    processRecord(nameID);
    processSend(nameID);
    printf("completed sending %d\n", nameID);
}
