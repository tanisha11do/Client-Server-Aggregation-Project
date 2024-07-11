# Client-Server-Aggregation-Project
 This project involves the development of a client-server application using Visual C++ and
 MFC in Visual Studio 2015. The primary objective is to establish a reliable connection
 between client and server applications via UDP sockets to transmit and display data from
 an Excel file. The project successfully implements socket programming, data parsing, and
 display functionalities. The data, comprising time-series coordinates, is transmitted at
 regular intervals and displayed in list boxes within the client-server interfaces.

The client parses timestamps, X, Y, and Z
 coordinates from the CSV file, transmits them using a UDP socket at 100 millisecond
 intervals, and displays the data in list boxes. The server utilizes a separate thread to
 handle incoming data asynchronously, updates its list box with the received information,
 and mirrors the client's functionality for real-time data visualization. 


## Real-Time Data Display
  <img src="https://github.com/tanisha11do/Client-Server-Aggregation-Project/assets/114300837/a7193242-27a3-43e7-8586-abd4caff0907" width=600>
