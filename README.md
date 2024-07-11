# Client-Server-Aggregation-Project
 This project involves the development of a client-server application using Visual C++ and
 MFC in Visual Studio 2015. The primary objective is to establish a reliable connection
 between client and server applications via UDP sockets to transmit and display data from
 an Excel file. The project successfully implements socket programming, data parsing, and
 display functionalities. The data, comprising time-series coordinates, is transmitted at
 regular intervals and displayed in list boxes within the client-server interfaces.

 ### IDE used: 
 - Microsoft Visual Studio 2015

### Language used:
- Visual C++

### Libraries used:
  - Microsoft Foundation Classes (MFC)
  - Windows Sockets (Winsock)

The project
 inherits from the CFormView class to manage both client-side and server-side
 functionalities within the same application window.

  - Two listbox controls were added to the application's user interface.
 One listbox displays the data transmitted from the Excel file, providing a visual
 representation of the data being sent. The other listbox displays the data received
 from the server, allowing the user to see the data echoed back by the server.

 - A button control labeled "Send" was implemented to initiate data
 transmission upon user interaction. Clicking this button triggers the process of
 sending the data from the Excel file to the server.

The client parses timestamps, X, Y, and Z
 coordinates from the CSV file, transmits them using a UDP socket at 100 millisecond
 intervals, and displays the data in list boxes. The server utilizes a separate thread to
 handle incoming data asynchronously, updates its list box with the received information,
 and mirrors the client's functionality for real-time data visualization. 

Here we have considered Points.csv file (sample data containing 50 data rows)
 
## Successful Data Parsing and Loading
  <img src="https://github.com/tanisha11do/Client-Server-Aggregation-Project/assets/114300837/da3e1772-851c-4515-8cbd-6c3055912952" width=600>

## Real-Time Data Display
  <img src="https://github.com/tanisha11do/Client-Server-Aggregation-Project/assets/114300837/a7193242-27a3-43e7-8586-abd4caff0907" width=600>
  <br/><br/>
   This project provided significant learning opportunities, particularly in understanding the
 trade-offs between different data transmission protocols and the intricacies of real-time
 data handling. The challenges encountered underscored the importance of user control
 and dynamic data sources, which are critical for the practical application of such systems.
 The insights gained will guide future developments, ensuring more robust and
 user-friendly solutions.
