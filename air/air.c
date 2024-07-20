#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    char name[50];
    char password[20];
} User;

typedef struct {
    int flight_id;
    char origin[30];
    char destination[30];
    char departure_time[20];
    int seats_available;
} Flight;

typedef struct {
    int booking_id;
    int user_id;
    int flight_id;
    int num_passengers;
    struct {
        char name[50];
        int age;
        char phone[15];
    } passengers[10]; // Assume a maximum of 10 passengers per booking for simplicity
} Booking;

void registerUser();
int loginUser();
void viewFlights();
void bookFlight(int userId);
void viewBookedFlights(int userId);
void mainMenu();
void userMenu(int userId);

int main() {
    mainMenu();
    return 0;
}

void mainMenu() {
    int choice, userId;
    while (1) {
        printf("1. Register\n2. Login\n3. Exit\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: registerUser(); break;
            case 2: userId = loginUser();
                    if (userId != -1) userMenu(userId);
                    break;
            case 3: exit(0);
            default: printf("Invalid choice. Try again.\n");
        }
    }
}

void registerUser() {
    FILE *fp = fopen("users.txt", "a");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }
    User user;
    printf("Enter User ID: ");
    scanf("%d", &user.id);
    printf("Enter Name: ");
    scanf("%s", user.name);
    printf("Enter Password: ");
    scanf("%s", user.password);
    fprintf(fp, "%d %s %s\n", user.id, user.name, user.password);
    fclose(fp);
    printf("Registration Successful!\n");
}

int loginUser() {
    FILE *fp = fopen("users.txt", "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return -1;
    }
    User user;
    int id;
    char password[20];
    printf("Enter User ID: ");
    scanf("%d", &id);
    printf("Enter Password: ");
    scanf("%s", password);
    while (fscanf(fp, "%d %s %s", &user.id, user.name, user.password) != EOF) {
        if (user.id == id && strcmp(user.password, password) == 0) {
            fclose(fp);
            printf("Login Successful!\n");
            return id;
        }
    }
    fclose(fp);
    printf("Invalid credentials. Try again.\n");
    return -1;
}

void userMenu(int userId) {
    int choice;
    while (1) {
        printf("1. View Flights\n2. Book Flight\n3. View Booked Flights\n4. Logout\nChoose an option: ");
        scanf("%d", &choice);
        switch (choice) {
            case 1: viewFlights(); break;
            case 2: bookFlight(userId); break;
            case 3: viewBookedFlights(userId); break;
            case 4: return;
            default: printf("Invalid choice. Try again.\n");
        }
    }
}

void viewFlights() {
    FILE *fp = fopen("flights.txt", "r");
    if (fp == NULL) {
        printf("Error opening file.\n");
        return;
    }
    Flight flight;
    int found = 0;
    while (fscanf(fp, "%d %s %s %s %d", &flight.flight_id, flight.origin, flight.destination, flight.departure_time, &flight.seats_available) != EOF) {
        found = 1;
        printf("Flight ID: %d\nOrigin: %s\nDestination: %s\nDeparture Time: %s\nSeats Available: %d\n\n",
               flight.flight_id, flight.origin, flight.destination, flight.departure_time, flight.seats_available);
    }
    if (!found) printf("No flights available.\n");
    fclose(fp);
}

void bookFlight(int userId) {
    FILE *fpFlights = fopen("flights.txt", "r");
    FILE *fpTemp = fopen("flights_temp.txt", "w");
    FILE *fpBookings = fopen("bookings.txt", "a");
    if (fpFlights == NULL || fpTemp == NULL || fpBookings == NULL) {
        printf("Error opening file.\n");
        if (fpFlights) fclose(fpFlights);
        if (fpTemp) fclose(fpTemp);
        if (fpBookings) fclose(fpBookings);
        return;
    }
    Flight flight;
    Booking booking;
    int flightId, found = 0, seatsRequested, i;
    printf("Enter Flight ID to book: ");
    scanf("%d", &flightId);
    printf("Enter number of seats to book: ");
    scanf("%d", &seatsRequested);
    while (fscanf(fpFlights, "%d %s %s %s %d", &flight.flight_id, flight.origin, flight.destination, flight.departure_time, &flight.seats_available) != EOF) {
        if (flight.flight_id == flightId && flight.seats_available >= seatsRequested) {
            found = 1;
            flight.seats_available -= seatsRequested;
            booking.booking_id = rand() % 10000;
            booking.user_id = userId;
            booking.flight_id = flightId;
            booking.num_passengers = seatsRequested;
            for (i = 0; i < seatsRequested; i++) {
                printf("Enter details for passenger %d:\n", i + 1);
                printf("Name: ");
                scanf("%s", booking.passengers[i].name);
                printf("Age: ");
                scanf("%d", &booking.passengers[i].age);
                printf("Phone: ");
                scanf("%s", booking.passengers[i].phone);
            }
            fprintf(fpBookings, "%d %d %d %d\n", booking.booking_id, booking.user_id, booking.flight_id, booking.num_passengers);
            for (i = 0; i < seatsRequested; i++) {
                fprintf(fpBookings, "%s %d %s\n", booking.passengers[i].name, booking.passengers[i].age, booking.passengers[i].phone);
            }
            printf("Booking Successful! Booking ID: %d\n", booking.booking_id);
        }
        fprintf(fpTemp, "%d %s %s %s %d\n", flight.flight_id, flight.origin, flight.destination, flight.departure_time, flight.seats_available);
    }
    if (!found) printf("Flight not found or not enough seats available.\n");
    fclose(fpFlights);
    fclose(fpTemp);
    fclose(fpBookings);
    remove("flights.txt");
    rename("flights_temp.txt", "flights.txt");
}

void viewBookedFlights(int userId) {
    FILE *fp = fopen("bookings.txt", "r");
    FILE *fpFlights = fopen("flights.txt", "r");
    if (fp == NULL || fpFlights == NULL) {
        printf("Error opening file.\n");
        if (fp) fclose(fp);
        if (fpFlights) fclose(fpFlights);
        return;
    }
    Booking booking;
    Flight flight;
    int found = 0, i;
    while (fscanf(fp, "%d %d %d %d", &booking.booking_id, &booking.user_id, &booking.flight_id, &booking.num_passengers) != EOF) {
        if (booking.user_id == userId) {
            rewind(fpFlights);
            while (fscanf(fpFlights, "%d %s %s %s %d", &flight.flight_id, flight.origin, flight.destination, flight.departure_time, &flight.seats_available) != EOF) {
                if (flight.flight_id == booking.flight_id) {
                    printf("Booking ID: %d\nFlight ID: %d\nOrigin: %s\nDestination: %s\nDeparture Time: %s\nNumber of Passengers: %d\n",
                           booking.booking_id, flight.flight_id, flight.origin, flight.destination, flight.departure_time, booking.num_passengers);
                    for (i = 0; i < booking.num_passengers; i++) {
                        fscanf(fp, "%s %d %s", booking.passengers[i].name, &booking.passengers[i].age, booking.passengers[i].phone);
                        printf("Passenger %d: Name: %s, Age: %d, Phone: %s\n", i + 1, booking.passengers[i].name, booking.passengers[i].age, booking.passengers[i].phone);
                    }
                    found = 1;
                    printf("\n");
                    break;
                }
            }
        }
    }
    if (!found) printf("No flights booked yet.\n");
    fclose(fp);
    fclose(fpFlights);
}
