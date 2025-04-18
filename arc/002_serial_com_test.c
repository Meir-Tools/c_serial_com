#include <windows.h>
#include <stdio.h>
#include <conio.h>



int main(int argc, char *argv[]) {

    /*if (argc != 3) {
        printf("Usage: %s com baud\n", argv[0]);

        getch(); // ממתין להקשה על מקש
        return 1;
    }*/

    char *com = "COM8";// char *com = argv[1];

    int baudRate = 9600 ; //int baudRate = atoi(argv[2]); // char *baud = argv[2];


    HANDLE hSerial;
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};

    // פתח את הפורט (למשל COM3)
    hSerial = CreateFile(
        "\\\\.\\COM8",                // שם הפורט
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
    timeouts.ReadIntervalTimeout         = 50;
    timeouts.ReadTotalTimeoutConstant    = 50;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.WriteTotalTimeoutConstant   = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    SetCommTimeouts(hSerial, &timeouts);

    // כתיבה לפורט
    const char *dataToSend = "Hello Serial\n";
    DWORD bytesWritten;

    if (!WriteFile(hSerial, dataToSend, strlen(dataToSend), &bytesWritten, NULL)) {
        printf("Error writing to port\n");
        CloseHandle(hSerial);
        return 1;
    }

    printf("Wrote %ld bytes\n", bytesWritten);

    // קריאה מהפורט
    char buffer[100];
    DWORD bytesRead;

    if (!ReadFile(hSerial, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        printf("Error reading from port\n");
        CloseHandle(hSerial);
        return 1;
    }

    buffer[bytesRead] = '\0';  // סיום מחרוזת
    printf("Read %ld bytes: %s\n", bytesRead, buffer);

    // סגור את הפורט
    CloseHandle(hSerial);
    return 0;
}
