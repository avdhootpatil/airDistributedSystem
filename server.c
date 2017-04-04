#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define MAX_FLIGHTS	10
#define MAX_SEATS	50
#define TRUE		1
#define FALSE		0

typedef struct reserve
{
	int reservation_no;
	int reserve_seats;
	struct reserve *next;
	struct reserve *previous;
} RESERVE;

typedef struct flight
{
	int flight_no;
	int num_seats;
	int ticket_issued;
	RESERVE *reservation_list;
} FLIGHT;

int reservation_counter = 0;
int ticket_issued = 0;
FLIGHT flight_buffer[MAX_FLIGHTS];

int main(int argc, char **argv)
{
	int job = 0, flight_no = 0, num_seats = 0, i = 0, reservation_no = 0;
	MPI_Status status;
	RESERVE *temp;
	char *message;
	char *tmp;
	int my_rank;

	sleep(3);
	message = (char *)malloc(1000*sizeof(char));
	tmp = (char *)malloc(100*sizeof(char));

	for(i = 0; i < MAX_FLIGHTS; i++)
	{
		flight_buffer[i].num_seats = MAX_SEATS;
		flight_buffer[i].flight_no = i;
		flight_buffer[i].reservation_list = NULL;
		flight_buffer[i].ticket_issued = 0;
	}


	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &job);

	i = 0;
	/*while(i <= job)
	{
		if(i == my_rank)
		{
			i++;
			continue;
		}
		MPI_Send(&my_rank, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
		i++;
	}*/

	MPI_Bcast(&my_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);

	while(TRUE)
	{
		MPI_Recv(&job, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

		switch (job)
		{
			case 1:
			{
				printf("OK\n");
				job = 1;
				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
	
				MPI_Recv(&flight_no, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				MPI_Recv(&num_seats, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				MPI_Recv(&job, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				reservation_counter++;

				flight_buffer[flight_no].num_seats -= num_seats;
				
				if(flight_buffer[flight_no].reservation_list == NULL)
				{
					flight_buffer[flight_no].reservation_list = (RESERVE *)malloc(1*sizeof(RESERVE));

					flight_buffer[flight_no].reservation_list->reservation_no = reservation_counter;
					flight_buffer[flight_no].reservation_list->reserve_seats = num_seats;
					flight_buffer[flight_no].reservation_list->next = NULL;
					flight_buffer[flight_no].reservation_list->previous = NULL;
				}
				else
				{
					temp = flight_buffer[flight_no].reservation_list;
	
					while(temp->next != NULL)
						temp = temp->next;
	
					temp->next = (RESERVE *)malloc(1*sizeof(RESERVE));
					temp->next->previous = temp;
					temp = temp->next;
					temp->reservation_no = reservation_counter;
					temp->reserve_seats = num_seats;
					temp->next = NULL;
				}

				MPI_Send(&reservation_counter, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				
				break;	
			}
			case 2:
			{
				job = 2;
				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

				MPI_Recv(&reservation_no, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				for(i = 0; i < MAX_FLIGHTS; i++)
				{
					temp = flight_buffer[i].reservation_list;

					if(temp == NULL)
						continue;
					else
					{
						while((temp->reservation_no != reservation_no) && temp->next != NULL)
							temp = temp->next;
							
						if(temp->reservation_no == reservation_no)
						{
							if(temp->previous != NULL)
								temp->previous->next = temp->next;
							if(temp->next != NULL)
								temp->next->previous = temp->previous;
							
							free(temp);
							job = TRUE;
							break;
						}
						else if(temp->reservation_no == reservation_no && temp->next == NULL)
						{
							if(temp->previous != NULL)
							{
								temp->previous->next == NULL;
								free(temp);
							}
							else
							{
								free(temp);
								flight_buffer[i].reservation_list = NULL;
							}
							job = TRUE;
							break;
						}
					}
				}

				
				if(job == 2)
				{
					job = 0;
				}

				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

				break;
			}
			case 3:
			{
				job = 3;
				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

				MPI_Recv(&reservation_no, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				
				job = FALSE;

				for(i = 0; i < MAX_FLIGHTS; i++)
				{
					temp = flight_buffer[i].reservation_list;

					if(temp == NULL)
						continue;
					else
					{
						while((temp->reservation_no != reservation_no) && temp->next != NULL)
							temp = temp->next;
							
						if(temp->reservation_no == reservation_no)
						{
							if(temp->previous != NULL)
								temp->previous->next = temp->next;
							if(temp->next != NULL)
								temp->next->previous = temp->previous;

							free(temp);
							job = TRUE;
							flight_buffer[i].ticket_issued++;

							break;
						}
						else if(temp->reservation_no == reservation_no && temp->next == NULL)
						{	
							if(temp->previous != NULL)
								temp->previous->next = NULL;
							else
								flight_buffer[i].reservation_list = NULL;
							free(temp);
							job = TRUE;
							flight_buffer[i].ticket_issued++;
							break;
						}
					}
				}

				if(job == TRUE)
				{
					ticket_issued++;
					MPI_Send(&ticket_issued, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				}
				else
					MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

				break;
			}
			case 4:
			{
				job = 4;
				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);

				MPI_Recv(message, 10, MPI_CHAR, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				MPI_Recv(&flight_no, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				MPI_Send(&job, 1, MPI_INT, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				MPI_Recv(&job, 1, MPI_INT, status.MPI_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
				
				if(strcmp(message, "BRIEF") == 0)
				{
					strcpy(message, "");
					strcat(message, "Free seats: ");
					sprintf(tmp, "%d", flight_buffer[flight_no].num_seats);
					strcat(message, tmp);
					strcat(message, "\n");
					sprintf(tmp, "%d", reservation_counter);
					strcat(message, "Reserve: ");
					strcat(message, tmp);
					strcat(message, "\n");
					sprintf(tmp, "%d", flight_buffer[flight_no].ticket_issued);
					strcat(message, "Ticket issued: ");
					strcat(message, tmp);
					strcat(message, "\n");
				}
				else if(strcmp(message, "FULL") == 0)
				{
					strcpy(message, "");
					strcat(message, "Tickets issued: ");
					sprintf(tmp, "%d", flight_buffer[flight_no].ticket_issued);
					strcat(message, tmp);
					strcat(message, "\n");
		
					temp = flight_buffer[flight_no].reservation_list;
		
					if(temp == NULL)
					{
						strcat(message, "No reservations\n");
					}
					else
					{		
						while(temp != NULL)
						{	
							strcat(message, "Reservation_no: ");
							sprintf(tmp, "%d", temp->reservation_no);
							strcat(message, tmp);
							strcat(message, "\n");
							sprintf(tmp, "%d", temp->reserve_seats);
							strcat(message, "Reserve seats: ");
							strcat(message, tmp);
							strcat(message, "\n");
							temp = temp->next;
						}
					}
				}
				MPI_Send(message, 1000, MPI_CHAR, status.MPI_SOURCE, 0, MPI_COMM_WORLD);
				
				break;
			}
			case 5:
			{
	//				exit(EXIT_SUCCESS);
			}
		}
	}

	MPI_Finalize();
	return 0;
}
