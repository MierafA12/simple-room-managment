#include <iostream>
#include <string>
#include <cstring> // For strncpy
#include "sqlite3.h" // Include SQLite3 header
using namespace std;

struct Event {
    char E_Name[30];
    int E_ID;
    string description;
    string date;
    string time;
    string organizer;
    Event* next;
    Event* prev;
};

struct Category {
    char C_Name[30];
    int C_ID;
    Category* next;
    Category* prev;
    Event* events;
};

Category* start = NULL;
sqlite3* db; // SQLite database pointer

void add_category();
void add_event();
void display();
void searchevent();
void update();
void Delete();
void sort();
void Statistics();
void initialize_db();
void close_db();
void execute_sql(const string& sql);

void initialize_db() {
    int exit = sqlite3_open("event_management.db", &db);
    if (exit) {
        cerr << "Error open DB: " << sqlite3_errmsg(db) << endl;
    } else {
        cout << "Opened Database Successfully!" << endl;
    }


    const char* create_category_table = "CREATE TABLE IF NOT EXISTS Category (C_ID INTEGER PRIMARY KEY, C_Name TEXT);";
    const char* create_event_table = "CREATE TABLE IF NOT EXISTS Event (E_ID INTEGER PRIMARY KEY, E_Name TEXT, Description TEXT, Date TEXT, Time TEXT, Organizer TEXT, C_ID INTEGER, FOREIGN KEY(C_ID) REFERENCES Category(C_ID));";
    execute_sql(create_category_table);
    execute_sql(create_event_table);
    const char* load_categories_sql = "SELECT C_ID, C_Name FROM Category;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, load_categories_sql, -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int c_id = sqlite3_column_int(stmt, 0);
            const char* c_name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            // Create a new category node
            Category* category_temp = new Category;category_temp->C_ID = c_id;strncpy(category_temp->C_Name, c_name, 30);


            category_temp->next = NULL;
            category_temp->prev = NULL;
            category_temp->events = NULL;

            // Add the category to the linked list
            if (start == NULL) {start = category_temp;
                } else {
                Category* curr = start;
                while (curr->next != NULL) {
                    curr = curr->next;
                }
                curr->next = category_temp;
                category_temp->prev = curr;
            }
            const char* load_events_sql = "SELECT E_ID, E_Name, Description, Date, Time, Organizer FROM Event WHERE C_ID = ?;";
            sqlite3_stmt* event_stmt;
            if (sqlite3_prepare_v2(db, load_events_sql, -1, &event_stmt, NULL) == SQLITE_OK) {
                sqlite3_bind_int(event_stmt, 1, c_id); // Bind the category ID to the query
                while (sqlite3_step(event_stmt) == SQLITE_ROW) {
                    int e_id = sqlite3_column_int(event_stmt, 0);
                    const char* e_name = reinterpret_cast<const char*>(sqlite3_column_text(event_stmt, 1));
                    const char* description = reinterpret_cast<const char*>(sqlite3_column_text(event_stmt, 2));
                    const char* date = reinterpret_cast<const char*>(sqlite3_column_text(event_stmt, 3));
                    const char* time = reinterpret_cast<const char*>(sqlite3_column_text(event_stmt, 4));
                    const char* organizer = reinterpret_cast<const char*>(sqlite3_column_text(event_stmt, 5));

                    // Create a new event node
                    Event* event_temp = new Event;
                    event_temp->E_ID = e_id;
                    strncpy(event_temp->E_Name, e_name, 30);
                    event_temp->description = description;
                    event_temp->date = date;
                    event_temp->time = time;
                    event_temp->organizer = organizer;
                    event_temp->next = NULL;
                    event_temp->prev = NULL;

                    // Add event to the category's event list
                    if (category_temp->events == NULL) {
                        category_temp->events = event_temp;
                    } else {
                        Event* event_curr = category_temp->events;
                        while (event_curr->next != NULL) {
                            event_curr = event_curr->next;
                        }
                        event_curr->next = event_temp;
                        event_temp->prev = event_curr;
                    }
                }
                sqlite3_finalize(event_stmt);
            } else {
                cerr << "Error loading events from database: " << sqlite3_errmsg(db) << endl;
            }
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error loading categories from database: " << sqlite3_errmsg(db) << endl;
    }
}
void close_db() {
    sqlite3_close(db);
}

void execute_sql(const string& sql) {
    char* messageError;
    int exit = sqlite3_exec(db, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        cerr << "Error executing SQL: " << messageError << endl;
        sqlite3_free(messageError);
    }
}
void add_category() {
    int category_id;
    cout << "Enter category ID: ";
    cin >> category_id;

    // Check if the category ID already exists in the database
    string check_sql = "SELECT C_ID FROM Category WHERE C_ID = " + to_string(category_id) + ";";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Error: Category ID " << category_id << " already exists in the database. Please enter a unique category ID.\n";
            sqlite3_finalize(stmt);
            return;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }
// If no duplicate ID, proceed with adding the category
    Category* category_temp = new Category;
    cout << "Enter category name: ";
    cin >> category_temp->C_Name;
    category_temp->C_ID = category_id;

    category_temp->next = NULL;
    category_temp->prev = NULL;
    category_temp->events = NULL;

    // Insert into SQLite database
    string sql = "INSERT INTO Category (C_ID, C_Name) VALUES (" + to_string(category_id) + ", '" + string(category_temp->C_Name) + "');";
    execute_sql(sql);
    if (start == NULL) {
        start = category_temp;
        cout << category_temp->C_Name << " category added successfully.\n";
    } else if (start->next == NULL) {
        start->next = category_temp;
        category_temp->prev = start;
        cout << category_temp->C_Name << " category added successfully.\n";
    } else {
        Category* curr = start;
        while (curr->next != NULL) {
            curr = curr->next;
        }
        curr->next = category_temp;
        category_temp->prev = curr;
        cout << category_temp->C_Name << " category added successfully.\n";
    }
}
void add_event() {
    if (start == NULL) {
        cout << "No categories available. Please add a category first.\n";
        return;
    }

    string category_name;
    cout << "Enter category name to add event: ";
    cin >> category_name;
    cin.ignore();
    Category* curr = start;
    while (curr != NULL && curr->C_Name != category_name) {
        curr = curr->next;
    }

    if (curr == NULL) {
        cout << "Category not found.\n";
        return;
    }

    // Check if the event ID already exists in this category
    int event_id;
    cout << "Enter event ID: ";
    cin >> event_id;

    // Check for duplicate event ID in the database
    string check_sql = "SELECT E_ID FROM Event WHERE E_ID = " + to_string(event_id) + ";";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            cout << "Error: Event ID " << event_id << " already exists in the database. Please enter a unique event ID.\n";
            sqlite3_finalize(stmt);
            return;
        }
        sqlite3_finalize(stmt);
    } else {
        cerr << "Error preparing SQL statement: " << sqlite3_errmsg(db) << endl;
        return;
    }

    // If the ID is unique, proceed with adding the event
    Event* event_temp = new Event;
    cout << "Enter event name: ";
    cin >> event_temp->E_Name;
    string input_date;
    cout << "Enter event date (YYYY-MM-DD): ";
    cin >> input_date;
    if (input_date.size() != 10 || input_date[4] != '-' || input_date[7] != '-') {
        cout << "Invalid date format! Please use YYYY-MM-DD.\n";
        delete event_temp;
        return;
    }

    event_temp->date = input_date;

    cout << "Enter event time: ";
    cin >> event_temp->time;
    cout << "Enter event description: ";
    cin >> event_temp->description;
    cout << "Enter event organizer: ";
    cin >> event_temp->organizer;
    event_temp->E_ID = event_id;

    // Insert event into SQLite database
    string sql = "INSERT INTO Event (E_ID, E_Name, Description, Date, Time, Organizer, C_ID) VALUES (" +
                 to_string(event_id) + ", '" + string(event_temp->E_Name) + "', '" + event_temp->description + "', '" +
                 event_temp->date + "', '" + event_temp->time + "', '" + event_temp->organizer + "', " +
                 to_string(curr->C_ID) + ");";
    execute_sql(sql);

    // Insert event at the beginning of the list for this category
    event_temp->next = curr->events;
    event_temp->prev = NULL;
    if (curr->events != NULL) {
        curr->events->prev = event_temp;
    }
    curr->events = event_temp;
    cout << "Event " << event_temp->E_Name << " added successfully under category " << category_name << ".\n";
}

void display() {
    if (start == NULL) {
        cout << "There are no categories and events.\n";
        return;
    }

    Category* category_temp = start;
    while (category_temp != NULL) {
        cout << "Category Name: " << category_temp->C_Name << endl;
        cout << "Category ID: " << category_temp->C_ID << endl;


        Event* event_temp = category_temp->events;
        if (event_temp == NULL) {
            cout << "\tNo events under this category.\n";
        } else {
            while (event_temp != NULL) {
                cout << "\tEvent ID: " << event_temp->E_ID << endl;
                cout << "\tEvent Name: " << event_temp->E_Name << endl;
                cout << "\tEvent Date: " << event_temp->date << endl;
                cout << "\tEvent Time: " << event_temp->time << endl;
                cout << "\tDescription: " << event_temp->description << endl;
                cout << "\tOrganizer: " << event_temp->organizer << endl;
                cout << "\n";
                event_temp = event_temp->next;
            }
        }
        category_temp = category_temp->next;
    }
}

void searchevent() {
    if (start == NULL) {
        cout << "No categories available. Please add categories and events first.\n";
        return;
    }

    int option;
    cout << "Search Event:\n";
    cout << "1. Search by Name\n";
    cout << "2. Search by Date\n";
    cout << "Enter your choice: ";
    cin >> option;

    if (option == 1) {
        string searchName;
        cout << "Enter the event name to search: ";
        cin.ignore();
        getline(cin, searchName);

        bool found = false;
        Category* category_temp = start;
        while (category_temp != NULL) {
            Event* event_temp = category_temp->events;
            while (event_temp != NULL) {
                if (searchName == event_temp->E_Name) {
                    cout << "\nEvent Found:\n";
                    cout << "Event Name: " << event_temp->E_Name << endl;
                    cout << "Event ID: " << event_temp->E_ID << endl;
                    cout << "Event Date: " << event_temp->date << endl;
                    cout << "Event Time: " << event_temp->time << endl;
                    cout << "Description: " << event_temp->description << endl;
                    cout << "Organizer: " << event_temp->organizer << endl;
                    found = true;
                    break;
                }
                event_temp = event_temp->next;
            }
            category_temp = category_temp->next;
        }

        if (!found) {
            cout << "Event with name '" << searchName << "' not found.\n";
        }
    } else if (option == 2) {
        string input_date;
        cout << "Enter event date (YYYY-MM-DD): ";
        cin >> input_date;
        if (input_date.size() != 10 || input_date[4] != '-' || input_date[7] != '-') {
            cout << "Invalid date format! Please use YYYY-MM-DD.\n";
            return;
        }
        bool found = false;
        Category* category_temp = start;
        while (category_temp != NULL) {
            Event* event_temp = category_temp->events;
            while (event_temp != NULL) {
                if (input_date == event_temp->date) {
                    cout << "Event Name: " << event_temp->E_Name << " , " << "Event time: " << event_temp->time << endl;
                    found = true;
                }
                event_temp = event_temp->next;
            }
            category_temp = category_temp->next;
        }

        if (!found) {
            cout << "No events found on date " << input_date << ".\n";
        }
    } else {
        cout << "Invalid choice. Please select either 1 or 2.\n";
    }
}

void update() {
    if (start == NULL) {
        cout << "No categories available. Please add a category first.\n";
        return;
    }
    string category_name;
    int event_id;
    cout << "Enter category name where the event is located: ";
    cin >> category_name;
    cin.ignore();

    Category* category_curr = start;
    while (category_curr != NULL && category_curr->C_Name != category_name) {
        category_curr = category_curr->next;
    }

    if (category_curr == NULL) {
        cout << "Category not found.\n";
        return;
    }

    Event* event_temp = category_curr->events;
    if (event_temp == NULL) {
        cout << "No events under this category.\n";
        return;
    }

    cout << "Enter the event ID to update: ";
    cin >> event_id;

    while (event_temp != NULL && event_temp->E_ID != event_id) {
        event_temp = event_temp->next;
    }

    if (event_temp == NULL) {
        cout << "Event not found.\n";
        return;
    }

    // Update the event details
    cout << "Updating event " << event_temp->E_Name << " (ID: " << event_temp->E_ID << ")\n";

    cout << "Enter new event name: ";
    cin >> event_temp->E_Name;
    cout << "Enter new event date: ";
    cin >> event_temp->date;
    cout << "Enter new event time: ";
    cin >> event_temp->time;
    cout << "Enter new event description: ";
    cin >> event_temp->description;
    cout << "Enter new event organizer: ";
    cin >> event_temp->organizer;
    // Update in SQLite database
    string sql = "UPDATE Event SET E_Name = '" + string(event_temp->E_Name) + "', Description = '" + event_temp->description + "', Date = '" + event_temp->date + "', Time = '" + event_temp->time + "', Organizer = '" + event_temp->organizer + "' WHERE E_ID = " + to_string(event_id) + ";";
    execute_sql(sql);

    cout << "Event updated successfully!\n";
}

void Delete() {
    if (start == NULL) {
        cout << "No categories available. Please add a category first.\n";
        return;
    }
    string category_name;
    int event_id;
    cout << "Enter category name where the event is located: ";
    cin >> category_name;
    cin.ignore();
// Find the category
    Category* category_curr = start;
    while (category_curr != NULL && category_curr->C_Name != category_name) {
        category_curr = category_curr->next;
    }
    if (category_curr == NULL) {
        cout << "Category not found.\n";
        return;
    }
    // Find the event
    Event* event_temp = category_curr->events;
    if (event_temp == NULL) {
        cout << "No events under this category.\n";
        return;
    }

    cout << "Enter the event ID to delete: ";
    cin >> event_id;

    while (event_temp != NULL && event_temp->E_ID != event_id) {
        event_temp = event_temp->next;
    }
    if (event_temp == NULL) {
        cout << "Event not found.\n";
        return;
    }
    // Delete the event by adjusting pointers
    if (event_temp->prev != NULL) {
        event_temp->prev->next = event_temp->next;
    } else {
        // If it's the first event in the list
        category_curr->events = event_temp->next;
    }
    if (event_temp->next != NULL) {
        event_temp->next->prev = event_temp->prev;
    }
    // Delete from SQLite database
    string sql = "DELETE FROM Event WHERE E_ID = " + to_string(event_id) + ";";
    execute_sql(sql);

    delete event_temp;
    cout << "Event deleted successfully!\n";
}

void sort() {
    if (start == NULL) {
        cout << "No categories available to sort events.\n";
        return;
    }
    Category* category_temp = start;
    while (category_temp != NULL) {
        if (category_temp->events != NULL) {
            bool swapped;
            do {
                swapped = false;
                Event* current = category_temp->events;
                while (current != NULL && current->next != NULL) {
                    // Compare date first
                    if (current->date > current->next->date ||
                        (current->date == current->next->date && current->time > current->next->time)) {
                        // Swap events
                        swap(current->E_Name, current->next->E_Name);
                        swap(current->E_ID, current->next->E_ID);
                        swap(current->description, current->next->description);
                        swap(current->date, current->next->date);
                        swap(current->time, current->next->time);
                        swap(current->organizer, current->next->organizer);
                        swapped = true;
                    }
                    current = current->next;
                }
            } while (swapped);
        }
        category_temp = category_temp->next;
    }
    cout << "Events sorted by date and time within each category.\n";
}

void Statistics() {
    if (start == NULL) {
        cout << "No categories available. Please add a category first.\n";
        return;
    }
    int option;
    cout << "Statistics:\n";
    cout << "1. Total number of events\n";
    cout << "2. Number of events by specific Date\n";
    cout << "Enter your choice: ";
    cin >> option;
    if (option == 1) {
        int total_events = 0;
        int total_categories = 0;
        Category* category_curr = start;
        while (category_curr != NULL) {
            Event* event_temp = category_curr->events;
            while (event_temp != NULL) {
                total_events++;
                event_temp = event_temp->next;
            }
            total_categories++;
            category_curr = category_curr->next;
        }
        cout << "Total: " << total_events << " events and " << total_categories << " categories." << endl;
    } else if (option == 2) {
        string input_date;
        cout << "Enter the specific date to count events (YYYY-MM-DD): ";
        cin >> input_date;

        if (input_date.size() != 10 || input_date[4] != '-' || input_date[7] != '-') {
            cout << "Invalid date format! Please use YYYY-MM-DD.\n";
            return;
        }
        int total_events = 0;
        Category* category_temp = start;
        while (category_temp != NULL) {
            Event* event_temp = category_temp->events;
            while (event_temp != NULL) {
                if (event_temp->date == input_date) {
                    total_events++;  // Count the event if the date matches
                }
                event_temp = event_temp->next;
            }
            category_temp = category_temp->next;
        }

        cout << "Total events on " << input_date << ": " << total_events << "\n";
    }
}
int main() {
    initialize_db(); // Initialize the database
    int choice;
    do {
        cout << "\nEvent Management System \n";
        cout << "___________________________\n";
        cout << "1. Add Category\n";
        cout << "2. Add Event to Category\n";
        cout << "3. Display All Categories and Events\n";
        cout << "4. Search Event\n";
        cout << "5. Update the Event\n";
        cout << "6. Delete the Event\n";
        cout << "7. Sort Events\n";
        cout << "8. Display Statistics\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1:
                add_category();
                break;
            case 2:
                add_event();
                break;
            case 3:
                display();
                break;
            case 4:
                searchevent();
                break;
            case 5:
                update();
                break;
            case 6:
                Delete();
                break;
            case 7:
                sort();
                break;
            case 8:
                Statistics();
                break;
            case 9:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 9);
    close_db(); // Close the database
    return 0;
}
