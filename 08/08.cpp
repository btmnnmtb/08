// 08.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define WIN32_LEAN_AND_MEAN //Этот макрос уменьшает размер включаемых заголовочных файлов Windows,
//ускоряя компиляцию и снижая вероятность конфликтов имен.

#include <iostream>
#include <Windows.h> //Включает основные функции
#include <WinSock2.h> //Подключает функции и структуры для работы с сокетами в Windows.
#include <WS2tcpip.h> //Включает дополнительные функции для работы с сокетами
using namespace std;

int main()
{
    WSADATA wsaData; //Структура для хранения информации о реализации Winsock.
    ADDRINFO hints; //Структура для указания критериев, используемых функцией getaddrinfo при выполнении поиска адресов.
    ADDRINFO* addrResult; //Указатель на структуру, содержащую результат вызова getaddrinfo.
    SOCKET ConnectSocket = INVALID_SOCKET; //Переменная для хранения сокета. Инициализируется как INVALID_SOCKET.
    char recvBuffer[512]; //Буфер для получения данных от сервера.

    const char* sendBuffer1 = "pomogite";
    const char* sendBuffer2 = "Bog ODIN on jdet nas vsex Valhalle "; //Строки, которые будут отправлены серверу.

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        cout << "WSAStartup failed with result: " << result << endl;
        return 1;
    } //Инициализирует использование Winsock DLL. Параметр MAKEWORD(2, 2) указывает версию Winsock 2.2. 
    //В случае ошибки возвращается ненулевое значение.

    ZeroMemory(&hints, sizeof(hints)); //Обнуляет структуру hints.
    hints.ai_family = AF_INET; //Указывает, что используется IPv4.
    hints.ai_socktype = SOCK_STREAM; //Указывает, что используется потоковый сокет (TCP).
    hints.ai_protocol = IPPROTO_TCP; //Указывает, что используется протокол TCP.

    result = getaddrinfo("localhost", "666", &hints, &addrResult);
    if (result != 0) {
        cout << "getaddrinfo failed with error: " << result << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    } //Получает адресную информацию для подключения к серверу.Параметры: 
    //"localhost" : имя хоста(в данном случае это локальный компьютер). 
    //"666" : порт сервера. 
    // & hints : указатель на структуру с критериями поиска. 
    //& addrResult : указатель на структуру для хранения результата.


    ConnectSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
    if (ConnectSocket == INVALID_SOCKET) {
        cout << "Socket creation failed" << endl;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    } //Создает сокет с параметрами, полученными из addrResult.
      //В случае ошибки выводится сообщение, освобождаются ресурсы и выполняется очистка Winsock.

    result = connect(ConnectSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
    if (result == SOCKET_ERROR) {
        cout << "Unable to connect to server" << endl;
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    } //Подключается к серверу с использованием адресной информации из addrResult.
      //В случае ошибки выводится сообщение, закрывается сокет, освобождаются ресурсы и выполняется очистка Winsock.

    result = send(ConnectSocket, sendBuffer1, (int)strlen(sendBuffer1), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;

    result = send(ConnectSocket, sendBuffer2, (int)strlen(sendBuffer2), 0);
    if (result == SOCKET_ERROR) {
        cout << "Send failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    }
    cout << "Sent: " << result << " bytes" << endl;
    //Отправляют данные на сервер.
    //ConnectSocket: сокет, через который идет отправка.
    //sendBuffer1 / sendBuffer2 : данные для отправки.
    //(int)strlen(sendBuffer1) / (int)strlen(sendBuffer2) : длина данных.
    //0 : флаги(по умолчанию).
    //В случае ошибки выводится сообщение, закрывается сокет, освобождаются ресурсы и выполняется очистка Winsock.

    result = shutdown(ConnectSocket, SD_SEND);
    if (result == SOCKET_ERROR) {
        cout << "Shutdown failed, error: " << result << endl;
        closesocket(ConnectSocket);
        freeaddrinfo(addrResult);
        WSACleanup();
        return 1;
    } //Завершает отправку данных. Параметр SD_SEND указывает, что отправка данных завершена, но прием данных продолжается.
      //В случае ошибки выводится сообщение, закрывается сокет, освобождаются ресурсы и выполняется очистка Winsock.

    do {
        ZeroMemory(recvBuffer, 512);
        result = recv(ConnectSocket, recvBuffer, 512, 0);
        if (result > 0) {
            cout << "Received " << result << " bytes" << endl;
            cout << "Received data: " << recvBuffer << endl;
        }
        else if (result == 0) {
            cout << "Connection closed" << endl;
        }
        else {
            cout << "Recv failed, error: " << WSAGetLastError() << endl;
        }
    } while (result > 0); //Получает данные от сервера. Параметры:
    //ConnectSocket: сокет для приема данных.
    //recvBuffer : буфер для хранения полученных данных.
    //512 : размер буфера.
    //0 : флаги(по умолчанию).
    //ZeroMemory(recvBuffer, 512) : обнуляет буфер перед получением данных.
    //Если данные получены(result > 0), они выводятся на экран.
    //Если соединение закрыто(result == 0), выводится сообщение.
    //Если произошла ошибка(result < 0), выводится сообщение об ошибке с кодом, полученным с помощью WSAGetLastError().

    closesocket(ConnectSocket); //Закрывает сокет.
    freeaddrinfo(addrResult); //Освобождает память, выделенную для структуры addrResult.
    WSACleanup(); //Завершает использование библиотеки Winsock, освобождая все ресурсы, связанные с Winsock.
    return 0; //Завершает программу с кодом 0, указывая на успешное выполнение.
}

