#include <iostream>
#include <string>
#include <curl/curl.h>

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void addHttpHeaders(CURL *curl, char const *headerAuthorization, char const *headerContentType) {
        struct curl_slist *headerlist = NULL;
        headerlist = curl_slist_append(headerlist, headerAuthorization);
        headerlist = curl_slist_append(headerlist, headerContentType);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
}

CURLcode sendMessage(CURL *curl, std::string postFields) 
{
    std::string url = "https://notify-api.line.me/api/notify";
    const char *completeUrl = url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, completeUrl);

    //set postFields for post method otherwise libcurl is using http get by default
    if (!postFields.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
    }
    return curl_easy_perform(curl);
}

CURLcode sendNotification(CURL *curl, std::string message) 
{
    std::string url = "https://notify-api.line.me/api/notify";
    const char *completeUrl = url.c_str();
    curl_easy_setopt(curl, CURLOPT_URL, completeUrl);

    curl_mime *form = NULL;
    curl_mimepart *field = NULL;

    form = curl_mime_init(curl);

    /* Fill in the message field */
    field = curl_mime_addpart(form);
    curl_mime_name(field, "message");
    curl_mime_data(field, message.c_str(), CURL_ZERO_TERMINATED);

    /* Fill in the image file field */
    field = curl_mime_addpart(form);
    curl_mime_name(field, "imageFile");
    curl_mime_filedata(field, "picture.jpg");

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

    return curl_easy_perform(curl);
}

int main(int argc, char const *argv[])
{
    CURL *curl;
    CURLcode res;
    std::string readBuffer;
    long responsecode;
    
    const char *headerContentType = "Content-Type: multipart/form-data";
    // Generate access token from https://notify-bot.line.me/my/
    const char *headerAuthorization = "Authorization: Bearer YOUR_ACCESS_TOKEN";

    curl = curl_easy_init();
    if (curl) {
        addHttpHeaders(curl, headerAuthorization, headerContentType);
        
        // res = sendMessage(curl, "message=Hello Tom");
        res = sendNotification(curl, "Hi, Super Tom");

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }else{
                curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responsecode);
                if (responsecode == 200) {
                    std::cout << " Success" << std::endl;
                }
                else if (responsecode == 400) {
                    std::cout << " Bad request" << std::endl;
                }
                else if (responsecode == 401) {
                    std::cout << " Invalid access token" << std::endl;
                }
                else if (responsecode == 500) {
                    std::cout << " Failure due to server error" << std::endl;
                }
                else {
                    std::cout << " Processed over time or stopped. Server answer with status code: " << responsecode << std::endl;
                }
        }

        curl_easy_cleanup(curl);

        std::cout << readBuffer << std::endl;
    }

    return 0;
}
