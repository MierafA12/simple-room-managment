#include <iostream>
#include <string>
using namespace std;

// Define the Room structure

struct Room {
    int roomNumber;
    string guestName;
    bool isReserved;
};

// Function to display all rooms

void displayRooms(Room rooms[], int numRooms) {
    cout << "Room\tName\tReservation" << endl;
    for (int i = 0; i < numRooms; i++) {
        cout << rooms[i].roomNumber << "\t" << rooms[i].guestName << "\t" << (rooms[i].isReserved ? "Reserved" : "Available") << endl;
    }
}

// Function to reserve a room

void reserveRoom(Room rooms[], int numRooms, int roomNumber) {
    for (int i = 0; i < numRooms; i++) {
        if (rooms[i].roomNumber == roomNumber) {
            if (!rooms[i].isReserved) {
                cout << "Enter guest name: ";
                cin >> rooms[i].guestName;
                rooms[i].isReserved = true;
                cout << "Room " << roomNumber << " reserved for " << rooms[i].guestName << endl;
            } else {
                cout << "Room " << roomNumber << " is already reserved" << endl;
            }
            return;
        }
    }
    cout << "Room " << roomNumber << " not found" << endl;
}

int main() {
    const int numRooms = 10;
    Room rooms[numRooms];

    // Initialize room information
    for (int i = 0; i < numRooms; i++) {
        rooms[i].roomNumber = i + 1;
        rooms[i].guestName = "";
        rooms[i].isReserved = false;
    }



    int choice;
    while (true) {
        cout << "\nHotel Management System" << endl;
        cout << "1. Display all rooms" << endl;
        cout << "2. Reserve a room" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            displayRooms(rooms, numRooms);
            break;
        case 2: {
            int roomNumber;
            cout << "Enter room number to reserve: ";
            cin >> roomNumber;
            reserveRoom(rooms, numRooms, roomNumber);
            break;
        }
        case 3:
            exit(0);
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    }

    return 0;
}
                                          



