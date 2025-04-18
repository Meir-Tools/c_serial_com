#include <windows.h>
#include <stdio.h>
#include <conio.h>

int main(int argc, char *argv[]) {
    char dataToSend[256] = "";
    char *endline = "\n";
    char *com = "COM8";// char *com = argv[1];
    int baudRate = 9600 ; //int baudRate = atoi(argv[2]); // char *baud = argv[2];

    printf("optional m31r_com -m \"message\" -c COM1 -b baud (default:9600)\n");

    // בדיקה אם יש דגל -c
    for (int i = 0; i < argc; i++) {
       printf("%s\n",argv[i]);
    }
    // check -c , com port
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-c") == 0) {
            com = argv[i + 1];
        }
    }
    // check -b , baudRate
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-b") == 0) {
            baudRate = atoi(argv[i + 1]);
        }
    }
    // check -m , message
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-m") == 0) {
            printf("found -m message : %s\n",argv[i]);
            strcpy(dataToSend, argv[i + 1]);  // עדכון ההודעה במקרה של דגל -m
            strcat(dataToSend, endline);  // הוספת סיום שורה
        }
    }
    /*if (argc != 3) {
        printf("Usage: %s com baud\n", argv[0]);

        getch(); // wait key
        return 1;
    }*/

    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    // פתח את הפורט (למשל COM3)
    char comPath[20];
    sprintf(comPath, "\\\\.\\%s", com); // הפורט יהיה בנתיב כמו "\\.\COM8"
    printf("Using COM port: %s\n", comPath); // הצגת הפורט שבו אנחנו משתמשים
    hSerial = CreateFile(
        comPath,                // ex . "\\\\.\\COM8"
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);

    if (hSerial == INVALID_HANDLE_VALUE) {
        printf("Error opening serial port\n");
        return 1;
    }

    // הגדרת פרמטרים של הפורט
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hSerial, &dcbSerialParams)) {
        printf("Error getting port state\n");
        CloseHandle(hSerial);
        return 1;
    }

    dcbSerialParams.BaudRate = baudRate;     // קצב תקשורת
    dcbSerialParams.ByteSize = 8;            // גודל בתים
    dcbSerialParams.StopBits = ONESTOPBIT;   // סיביות עצירה
    dcbSerialParams.Parity   = NOPARITY;     // פריטי

    if (!SetCommState(hSerial, &dcbSerialParams)) {
        printf("Error setting port state\n");
        CloseHandle(hSerial);
        return 1;
    }

    // הגדרות timeout
    timeouts.ReadIntervalTimeout         = MAXDWORD;
    timeouts.ReadTotalTimeoutConstant    = 0;
    timeouts.ReadTotalTimeoutMultiplier  = 0;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    // כתיבה לפורט

    DWORD bytesWritten;

    if (!WriteFile(hSerial, dataToSend, strlen(dataToSend), &bytesWritten, NULL)) {
        printf("Error writing to port\n");
        CloseHandle(hSerial);
        return 1;
    }

    printf("Wrote %ld bytes\n", bytesWritten);

    while (1) {
        // קריאה מהפורט
        char buffer[100];
        DWORD bytesRead;

        if (ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
            if (bytesRead > 0) {
            buffer[bytesRead] = '\0';
            printf("%s", buffer); // פשוט מציג את מה שהגיע
            }
        } else {
            fprintf(stderr, "Error reading from port\n");
            break;
        }

    Sleep(10); // פחות עומס
    }

    //buffer[bytesRead] = '\0';  // סיום מחרוזת
    //printf("Read %ld bytes: %s\n", bytesRead, buffer);
    // סגור את הפורט
    CloseHandle(hSerial);

    getch(); // wait key
    return 0;
}
