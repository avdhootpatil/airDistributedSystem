#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_FLIGHTS	10
#define MAX_SEATS	50
#define TRUE		1
#define FALSE		0

FILE *fout, *fin1, *fin2, *fin3, *fin4, *fin5;

/************		Function Prototypes		*****************************/

int menu();
int make_reservation(int flight_no, int num_seats);
int cancel_reservation(int reservation_no);
int issue_ticket(int reservation_no);
char *show_info(char *string, int flight_no);

/************************************************************************/

int my_rank, server;

int main(int argc, char **argv)
{
	int i, job = 0, reservation_no = 0, flight_no = 0, num_seats = 0, done = 0, ticket_no = 0;
	char *string, men[5], *buffer, *message;
	MPI_Status status;
	int tmp = 0;

	fout = fopen("output.txt", "w");
	fin1 = fopen("input1.txt", "r");
	fin2 = fopen("input2.txt", "r");
	fin3 = fopen("input3.txt", "r");
	fin4 = fopen("input4.txt", "r");
	fin5 = fopen("input5.txt", "r");


	buffer = (char *)malloc(20*sizeof(char));

	MPI_Init(&argc, &argv);					//Initializing MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	printf("tha kanei receive?\n");

//	MPI_Recv(&server, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	MPI_Bcast(&server, 1, MPI_INT, 0, MPI_COMM_WORLD);
	printf("Receice server: %d\n", server);
	while(TRUE)
	{
		printf("I am in\n");
		job = menu();

		switch (job)
		{
			case 1:
			{
				printf("case 1\n");
				fprintf(fout, "ID: %d |Flight number: ", my_rank);
				fscanf(fin1, "%d", &flight_no);
				fprintf(fout, "ID: %d |Number of seats: ", my_rank);
				fscanf(fin1, "%d", &num_seats);
		
				reservation_no = make_reservation(flight_no, num_seats);
				
				if(reservation_no == 0)
				{
					fprintf(fout, "ID: %d |Problem occured...\n", my_rank);
				}
				else
				{
					fprintf(fout, "ID: %d |Reservation number: %d\n", my_rank, reservation_no);
					fprintf(fout, "ID: %d |Please keep it safe until the ticket issue!\n", my_rank);
				}
	
				break;
			}
			case 2:
			{
				fprintf(fout, "ID: %d |Reservation number: ", my_rank);
				fscanf(fin2, "%d", &reservation_no);
		
				done = cancel_reservation(reservation_no);
		
				if(done == TRUE)
				{
					fprintf(fout, "ID: %d |Reservation canceled!\n", my_rank);
				}
				else
				{
					fprintf(fout, "ID: %d |Reservation cancel aborted...\n", my_rank);
				}
	
				break;
			}
			case 3:
			{
				fprintf(fout, "ID: %d |Reservation number: ", my_rank);
				fscanf(fin3, "%d", &reservation_no);
		
				ticket_no = issue_ticket(reservation_no);
		
				if(ticket_no == FALSE)
				{	
					fprintf(fout, "ID: %d |Error Occured! Try again!\n", my_rank);
				}
				else
				{
					fprintf(fout, "ID: %d |Ticket issue number: %d\n", my_rank, ticket_no);
				}
		
				break;
			}
			case 4:
			{
				string = (char *)malloc(10*sizeof(char));
		
				fprintf(fout, "ID: %d |Full or Brief view(FULL/BRIEF)?: ", my_rank);
				fscanf(fin4, "%s", string);
				fprintf(fout, "ID: %d |Flight number: ", my_rank);
				fscanf(fin4, "%d", &flight_no);
		
				message = show_info(string, flight_no);
		
				fprintf(fout, "ID: %d |%s\n", my_rank, message);
		
				break;
			}
			case 5:
			{
				return 0;
			}
		}
	}

	MPI_Finalize();					//Terminating MPI, no more use of it...

	return 0;
}

int menu()
{
	int temp = 0;

	while(temp < 1 || temp > 5)
	{
		//system("clear");
		fprintf(fout, "ID: %d |\n\n \t\tWelcome to Ticket Reservation System \n\n", my_rank);
		fprintf(fout, "ID: %d |MENU\n", my_rank);
		fprintf(fout, "ID: %d |----------------------------------------------------\n", my_rank);
		fprintf(fout, "ID: %d |1. Make a reservation\n", my_rank);
		fprintf(fout, "ID: %d |2. Cancel a reservation\n", my_rank);
		fprintf(fout, "ID: %d |3. Issue a tickets\n", my_rank);
		fprintf(fout, "ID: %d |4. View your reservation\n", my_rank);
		fprintf(fout, "ID: %d |5. Exit\n", my_rank);
		fprintf(fout, "ID: %d |Select between 1-5: ", my_rank);
		fscanf(fin5, "%d", &temp);

	}

	return temp;
}

int make_reservation(int flight_no, int num_seats)
{
	int i = 0, return_val = 0;
	MPI_Status status;

	i = 1;

	MPI_Send(&i, 1, MPI_INT, server, 0, MPI_COMM_WORLD);
	MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	if(i == 1)
	{

		MPI_Send(&flight_no, 1, MPI_INT, server, 0, MPI_COMM_WORLD);
		MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Send(&num_seats, 1, MPI_INT, server, 0, MPI_COMM_WORLD);
		MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Send(&i, 1, MPI_INT, server, 0, MPI_COMM_WORLD);
		MPI_Recv(&return_val, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		return return_val;
	}
	else
		return 0;
}

int cancel_reservation(int reservation_no)
{
	int i = 0, return_val = 0;
	MPI_Status status;

	i = 2;

	MPI_Send(&i, 1, MPI_INT, server, 0, MPI_COMM_WORLD);

	MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	if(i == 2)
	{
		MPI_Send(&reservation_no, 1, MPI_INT, server, 0, MPI_COMM_WORLD);

		MPI_Recv(&return_val, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		return return_val;
	}
	else
		return FALSE;
}

int issue_ticket(int reservation_no)
{
	int i = 0;
	MPI_Status status;

	i = 3;

	MPI_Send(&i, 1, MPI_INT, server, 0, MPI_COMM_WORLD);

	MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	if(i == 3)
	{
		MPI_Send(&reservation_no, 1, MPI_INT, server, 0, MPI_COMM_WORLD);

		MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		if(i == FALSE)
		{
			return FALSE;
		}

		return i;
	}

	return FALSE;
}

char *show_info(char *string, int flight_no)
{
	int i = 0;
	MPI_Status status;
	char *return_str;

	return_str = (char *)malloc(1000*sizeof(char));

	i = 4;

	MPI_Send(&i, 1, MPI_INT, server, 0, MPI_COMM_WORLD);

	MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

	if(i == 4)
	{
		MPI_Send(string, 10, MPI_CHAR, server, 0, MPI_COMM_WORLD);
		MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Send(&flight_no, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		MPI_Recv(&i, 1, MPI_INT, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
		MPI_Send(&i, 1, MPI_INT, server, 0, MPI_COMM_WORLD);
		MPI_Recv(return_str, 1000, MPI_CHAR, server, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
	}
	else
		strcpy(return_str, "Error Occured!\n");

	return return_str;
}		


