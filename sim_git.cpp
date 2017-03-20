#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <cstdlib>

#define debug 0

using namespace std;

int SIZE = 32;
int k=0;

// unsigned int pc;
char pc[20];

int ROB_empty_space = 0;
int IQ_free_entries = 0;
int enough_space_in_ROB = 0;
int enough_space_in_IQ = 0;
int head = 0;
int tail = 0;
int sequence_no = 1;
int empty = 0;
int IQ_empty = 0;
int IQ_is_empty = 0;
int execute_list_first_invalid_position = 0;
int wb_first_invalid_position = 0;
int iq_first_invalid_position = 0;
int WB_empty = 0;
int WB_is_empty = 0;
int ROB_empty = 0;
int ROB_is_empty = 0;
int dest = 0;
int retirement = 0;
int cycle = 0;
int DE_loop = 0;
int RN_loop = 0;
int RR_loop = 0;
int DI_loop = 0;

int ROB_SIZE, IQ_SIZE, WIDTH, operation_type, destination, source1, source2, dst, init_dst, src1, src2, mini, mini_seq_no;

char *tracefile = (char *)malloc(100);

int **DE, **RN, **RR, **DI, **IQ, **execute_list, **WB, **ROB, **TS;

int rmt[67][2];

int COLUMNS = 11;
int COL_TS = 22;

/*

Column headers

seq_no = 0; 
valid = 1;
dst = 2;
src1 = 3;
src1_ready = 4;
src2 = 5;
src2_ready = 6;
type_no = 7;
latency = 8;
ready = 9;
init_dst = 10;
pc = 11;

COL_TS headers

op_type = 0;
dst = 1;
src1 = 2;
src2 = 3;
fe_b = 4;
fe_dur = 5;
de_b = 6;
de_dur = 7;
rn_b = 8;
rn_dur = 9;
rr_b = 10;
rr_dur = 11;
di_b = 12;
di_dur = 13;
is_b = 14;
is_dur = 15;
exe_b = 16;
exe_dur = 17;
wb_b = 18;
wb_dur = 19;
ret_be = 20;
ret_dur = 21;

*/

FILE *pFile;

unsigned int HexToInt(char *inAddress);
int IsEmpty(int **reg);
int Transfer(int **from, int **to);
int IncHead();
int IncTail();
int Wakeup();
int Fetch();
int Decode();
int Rename();
int RegRead();
int Dispatch();
int Issue();
int Execute();
int Writeback();
int Retire();
int AdvanceCycle();

int main(int argc, char* argv[])
{
	ROB_SIZE = atoi(argv[1]);
	IQ_SIZE = atoi(argv[2]);
	WIDTH = atoi(argv[3]);
	tracefile = argv[4];

	for (int i=0; i<67; i++) // Initialize Rename Map Table
	{
		rmt[i][0] = 0;
		rmt[i][1] = 0;
	}

/*************************************Pipeline Registers************************************************************************/

	// Decode Register
	// int **DE; 
	
	DE = (int **)malloc(WIDTH * sizeof(int *));
	if (DE == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < WIDTH; i++)
	{
		DE[i] = (int *)malloc(COLUMNS * sizeof(int));
		if (DE[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}
	
	// Rename Register
	// int **RN;
	
	RN = (int **)malloc(WIDTH * sizeof(int *));
	if (RN == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < WIDTH; i++)
	{
		RN[i] = (int *)malloc(COLUMNS * sizeof(int));
		if (RN[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}

	// Register Read Pipeline Register
	// int **RR;
	
	RR = (int **)malloc(WIDTH * sizeof(int *));
	if (RR == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < WIDTH; i++)
	{
		RR[i] = (int *)malloc(COLUMNS * sizeof(int));
		if (RR[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}

	// Dispatch Register
	// int **DI;

	DI = (int **)malloc(WIDTH * sizeof(int *));
	if (DI == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < WIDTH; i++)
	{
		DI[i] = (int *)malloc(COLUMNS * sizeof(int));
		if (DI[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}

	// Issue Queue Register
	// int **IQ;

	IQ = (int **)malloc(IQ_SIZE * sizeof(int *));
	if (IQ == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < IQ_SIZE; i++)
	{
		IQ[i] = (int *)malloc(COLUMNS * sizeof(int));
		if (IQ[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}

	// Execute list pipeline register
	// int **execute_list;
	
	execute_list = (int **)malloc(WIDTH * 5 * sizeof(int *));
	if (execute_list == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < WIDTH*5; i++)
	{
		execute_list[i] = (int *)malloc(COLUMNS * sizeof(int));
		if (execute_list[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}

	// Writeback Pipeline register
	// int **WB;
	
	WB = (int **)malloc(WIDTH*5 * sizeof(int *));
	if (WB == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < WIDTH*5; i++)
	{
		WB[i] = (int *)malloc(COLUMNS * sizeof(int));
		if (WB[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}

	// Reorder buffer
	// int **ROB;
	
	ROB = (int **)malloc(ROB_SIZE * sizeof(int *));
	if (ROB == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < ROB_SIZE; i++)
	{
		ROB[i] = (int *)malloc(COLUMNS * sizeof(int));
		if (ROB[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}

	//Time sequence array of 10,000 rows

	TS = (int **)malloc(20001 * sizeof(int *));
	if (TS == NULL)
	{
	  cout << "Out of memory" << endl;
	  exit;
	}
	for (int i=0; i < 20001; i++)
	{
		TS[i] = (int *)malloc(COL_TS * sizeof(int));
		if (TS[i] == NULL)
		{
			cout << "Out of memory" << endl;
		}
	}

/*******************************************************************************************************************/

ifstream myfile(tracefile);

pFile = fopen(tracefile, "r");

	do
	{
		Retire();
		Writeback();
	 	Execute();
	 	Issue();
	 	Dispatch();
	 	RegRead();
	 	Rename();
	 	Decode();
		Fetch();	
	}	
	while (AdvanceCycle());

	cycle = TS[9999][20] + TS[9999][21];

	for (int i=0; i < 10000; i++)
	{
		cout << i << "\t" << "fu{" << TS[i][0] << "}	src{" << TS[i][2] << "," << TS[i][3];
		cout << "}	dst{" << TS[i][1] << "}		FE{" << TS[i][4] << "," << TS[i][5];
		cout << "}	DE{" << TS[i][6] << "," << TS[i][7];
		cout << "}	RN{" << TS[i][8] << "," << TS[i][9];
		cout << "}	RR{" << TS[i][10] << "," << TS[i][11];
		cout << "}	DI{" << TS[i][12] << "," << TS[i][13];
		cout << "}	IS{" << TS[i][14] << "," << TS[i][15];
		cout << "}	EX{" << TS[i][16] << "," << TS[i][17];
		cout << "}	WB{" << TS[i][18] << "," << TS[i][19];
		cout << "}	RT{" << TS[i][20] << "," << TS[i][21] << "}" << endl;
	}
		
	cout << "# === Simulator Command =========" << endl;
	cout << "# ./sim 	" << ROB_SIZE << "\t" << IQ_SIZE << "\t" << WIDTH << "\t" << tracefile << endl;
	cout << "# === Processor Configuration ===" << endl;
	cout << "# ROB_SIZE = " << ROB_SIZE << endl;
	cout << "# IQ_SIZE  = " << IQ_SIZE << endl;
	cout << "# WIDTH    = " << WIDTH << endl;
	cout << "# === Simulation Results ========" << endl;
	cout << "# Dynamic Instruction Count    = " << 10000  << endl;
	cout << "# Cycles                       = " << cycle << endl;
	cout << "# Instructions Per Cycle (IPC) = " << fixed << setprecision(2) << (float)10000 / (float)cycle << endl;
}


unsigned int HexToInt(char *inAddress) 
{		
	return (unsigned int) strtol(inAddress, NULL, 16);
};

int IsEmpty(int **reg)
{
	for (int i=0; i < WIDTH; i++ )
	{
		if (reg[i][1] == 0)
			empty++;
	}

	if (empty >= WIDTH)
	{	
		empty = 0;
		return 1;
	}
	else
	{
		empty = 0;
		return 0;
	}
}

int Transfer(int **from, int **to)
{
	for (int i = 0; i < WIDTH; i++)
	{
		for (int j = 0; j < COLUMNS; j++)
		{
			to[i][j] = from[i][j];
		}

		from[i][1] = 0;
	}
	return 0;
}

int IncHead()
{
	head++;

	if (head == ROB_SIZE)
		head = 0;
	return 0;
}

int IncTail()
{
	tail++;

	if (tail == ROB_SIZE)
		tail = 0;
	return 0;
}

int Wakeup()
{
	for (int i=0; i < IQ_SIZE; i++)
	{
		if ((IQ[i][3] == WB[wb_first_invalid_position][2]) && (WB[wb_first_invalid_position][1] == 1))
			IQ[i][4] = 1;

		if ((IQ[i][5] == WB[wb_first_invalid_position][2]) && (WB[wb_first_invalid_position][1] == 1))
			IQ[i][6] = 1;
	}

	for (int i=0; i < WIDTH; i++)
	{
		if ((DI[i][3] == WB[wb_first_invalid_position][2]) && (WB[wb_first_invalid_position][1] == 1))
			DI[i][4] = 1;

		if ((DI[i][5] == WB[wb_first_invalid_position][2]) && (WB[wb_first_invalid_position][1] == 1))
			DI[i][6] = 1;
	}

	for (int i=0; i < WIDTH; i++)
	{
		if ((RR[i][3] == WB[wb_first_invalid_position][2]) && (WB[wb_first_invalid_position][1] == 1))
			RR[i][4] = 1;

		if ((RR[i][5] == WB[wb_first_invalid_position][2]) && (WB[wb_first_invalid_position][1] == 1))
			RR[i][6] = 1;
	}
 	return 0;
}

int Fetch()
{
	if (sequence_no < 10040)
	{
		if (IsEmpty(DE) == 1)
		{
			for(int i=0; i < WIDTH; i++)
			{	
			//myfile >> hex >> pc >> dec >> operation_type >> dec >> destination >> dec >> source1 >> dec >> source2;
				if (feof(pFile) != 1) 
				{
					fscanf (pFile, "%s", pc);
					// cout << "PC is = " << pc << endl;
					fscanf (pFile, "%d", &operation_type);
					// cout << "Op type is = " << operation_type << endl;
					fscanf (pFile, "%d", &destination);
					// cout << "destination is = " << destination << endl;
					fscanf (pFile, "%d", &source1);
					// cout << "source1 is = " << source1 << endl;
					fscanf (pFile, "%d", &source2);
					// cout << "Source2 is = " << source2 << endl;
					//cout << "Prog is = " << prog << endl;

					TS[sequence_no - 1][0] = operation_type;
					TS[sequence_no - 1][1] = destination;
					TS[sequence_no - 1][2] = source1;
					TS[sequence_no - 1][3] = source2;

					TS[sequence_no - 1][4] = cycle;
					TS[sequence_no - 1][5] = 1;
					TS[sequence_no - 1][6] = cycle + 1;

					
					DE[i][0] = sequence_no - 1;
					DE[i][1] = 1;
					DE[i][2] = destination;
					DE[i][10] = destination;
					DE[i][3] = source1;
					DE[i][5] = source2;
					DE[i][7] = operation_type;

					if (DE[i][7] == 0)
						DE[i][8] = 1;
					else if(DE[i][7] == 1)
						DE[i][8] = 2;
					else if(DE[i][7] == 2)
						DE[i][8] = 5;
					
					sequence_no++;
					DE_loop++;
				}
				else break;
			}
		}
	}
	else return 0;

	return 0;
}

int Decode()
{
	if (IsEmpty(DE) == 0 )
	{
		if (IsEmpty(RN) == 1)
		{
			for (int i=0; i < DE_loop; i++) // Decode end
			{
				TS[DE[i][0]][7] = cycle + 1 -  TS[DE[i][0]][6];
			}

			//Transfer(DE, RN);
			for (int i = 0; i < DE_loop; i++)
			{
				for (int j = 0; j < COLUMNS; j++)
				{
					RN[i][j] = DE[i][j];
				}
				DE[i][1] = 0;
			}
			
			for (int i=0; i < DE_loop; i++) //RN begin
			{
				TS[RN[i][0]][8] = cycle + 1;
			}
			//DE[sequence_no][1] = 0; //Invalidate DE once transferred
			RN_loop = DE_loop;
			DE_loop = 0;
		}	
	}	
	return 0;
}

int Rename()
{
	if (IsEmpty(RN) == 0)
	{	
		for (int i=0; i < ROB_SIZE; i++)
		{
			if (ROB[i][1] == 0)
				ROB_empty_space++;
		}
		
		if (ROB_empty_space >= RN_loop)
		 	enough_space_in_ROB = 1;
		else
		 	enough_space_in_ROB = 0;

		if ((IsEmpty(RR) == 1) && (enough_space_in_ROB == 1))
		{
			for (int i=0; i < RN_loop; i++) 
			{
				for (int j=0; j < COLUMNS; j++) //allocate an entry in the ROB
				{
					ROB[tail][j] = RN[i][j];
				}

				if (RN[i][5] == -1)
					(RN[i][6] = 1) ;
				else
				{ 
					if (rmt[RN[i][5]][0] == 1) //source registers renaming
						RN[i][5] = rmt[RN[i][5]][1];
					else
						RN[i][6] = 1;
				}	

				if (RN[i][3] == -1)// source register renaming
					(RN[i][4] = 1) ;
				else
				{
					if (rmt[RN[i][3]][0] == 1)
						RN[i][3] = rmt[RN[i][3]][1];
					else
						RN[i][4] = 1;
				}

				if (RN[i][2] != -1)
				rmt[RN[i][2]][0] = 1; //destination register renaming
				rmt[RN[i][2]][1] = tail;
				RN[i][2] = tail;
				ROB[tail][2] = tail;

				IncTail();
			}

			for (int i=0; i < RN_loop; i++) // RN end
			{
				TS[RN[i][0]][9] = cycle + 1 -  TS[RN[i][0]][8];
			}

			//Transfer (RN, RR);
			for (int i = 0; i < RN_loop; i++)
			{
				for (int j = 0; j < COLUMNS; j++)
				{
					RR[i][j] = RN[i][j];
				}

				RN[i][1] = 0;
			}


			for (int i=0; i < RN_loop; i++) //RR begin
			{
				TS[RR[i][0]][10] = cycle + 1;
			}

			RR_loop = RN_loop;
			RN_loop = 0;
			// for (int i=0; i<WIDTH; i++)
			// 	RN[i][1] = 0; //Invalidate once RN transferred
		}
	}

	ROB_empty_space = 0;
	return 0;
}

int RegRead()
{
	if (IsEmpty(RR) == 0)
	{
		if (IsEmpty(DI) == 1)
		{
			for (int i=0; i < RR_loop; i++)
			{
				if (RR[i][4] != 1)
				{
					if (ROB[RR[i][3]][9] == 1) // Check if source operands are ready
						RR[i][4] = 1;
				}	
				if (RR[i][6] != 1)
				{
					if (ROB[RR[i][5]][9] == 1)
						RR[i][6] = 1;
				}
			}

			for (int i=0; i < RR_loop; i++) // RR end
			{
				TS[RR[i][0]][11] = cycle + 1 -  TS[RR[i][0]][10];
			}

			//Transfer(RR, DI);
			for (int i = 0; i < RR_loop; i++)
			{
				for (int j = 0; j < COLUMNS; j++)
				{
					DI[i][j] = RR[i][j];
				}

				RR[i][1] = 0;
			}

			for (int i=0; i < RR_loop; i++) //DI begin
			{
				TS[DI[i][0]][12] = cycle + 1;
			}

			DI_loop = RR_loop;
			RR_loop = 0;
		}
	}
	return 0;
}

int Dispatch()
{
	if (IsEmpty(DI) == 0)
	{
		for(int i=0; i < IQ_SIZE; i++)
		{
			if (IQ[i][1] == 0)
				IQ_free_entries++;
		}

		if (IQ_free_entries >=  DI_loop)
			enough_space_in_IQ = 1;
		else
			enough_space_in_IQ = 0;

		if (enough_space_in_IQ == 1)
		{
			// Transfer(DI, IQ);
			for (int i=0; i < DI_loop; i++)
			{	
				for (int q=0; q < IQ_SIZE; q++)	//find an invalid position in IQ
				{
					if (IQ[q][1] == 0)
					{	
						iq_first_invalid_position = q;
						break;
					}
				}

				for (int j = 0; j < COLUMNS; j++) //transfer from DI to IQ
				{
					IQ[iq_first_invalid_position][j] = DI[i][j];

					TS[DI[i][0]][13] = cycle + 1 -  TS[DI[i][0]][12];
					//***************chng*******

					TS[DI[i][0]][14] = cycle + 1;
				}
				
				DI[i][1] = 0; //invalidate that instruction in execute_list
			}
			DI_loop = 0;
		}
	}
	
	IQ_free_entries = 0;
	return 0;
}

int Issue()
{	
	for (int i=0; i < IQ_SIZE; i++ ) // Check if IQ not empty
	{
		if (IQ[i][1] == 0)
			IQ_empty++;
	}

	if (IQ_empty >= IQ_SIZE)
		IQ_is_empty = 1;
	else
		IQ_is_empty = 0;

	if (IQ_is_empty == 0)	// Check if IQ not empty
	{
		for (int i=0; i < WIDTH; i++)
		{
			for (int g=0; g < IQ_SIZE; g++)
			{
				if ( (IQ[g][1] == 1) && (IQ[g][4] == 1) && (IQ[g][6] == 1) )
				{
					mini = g;
					mini_seq_no = IQ[g][0];
					break;
				}
			}

			for (int k=0; k < IQ_SIZE; k++)
			{	
				if ( (IQ[k][1] == 1) && (IQ[k][4] == 1) && (IQ[k][6] == 1) )
				{	
					if (mini_seq_no > IQ[k][0])
					{
						mini_seq_no = IQ[k][0];
						mini = k;					
					}						
				}
			}	
			
			//cout << "mini seq no is = " << mini_seq_no << endl;
			//cout << "mini is = " << mini << endl;
			if ( (IQ[mini][1] == 1) &&  (IQ[mini][4] == 1) && (IQ[mini][6] == 1) )
			{
				for (int p=0; p < WIDTH*5; p++)
				{
					if (execute_list[p][1] == 0)
					{	
						execute_list_first_invalid_position = p;
						break;
					}
				}

				for (int j = 0; j < COLUMNS; j++) //send it to execute_list
				{
					execute_list[execute_list_first_invalid_position][j] = IQ[mini][j];

					TS[IQ[mini][0]][15] = cycle + 1 -  TS[IQ[mini][0]][14];
					//***************chng**********

					TS[IQ[mini][0]][16] = cycle + 1;
				}
				IQ[mini][1] = 0; //invalidate that instruction
			}
		}
	}
	IQ_empty = 0;
	return 0; 
}

int Execute()
{
	for (int p=0; p < (WIDTH*5); p++)
	{
		if (execute_list[p][1] == 1)
		{
			execute_list[p][8]--;
		
			if (execute_list[p][8] == 0)
			{ 
				for (int q=0; q < WIDTH*5; q++)	//find an invalid position in WB
				{
					if (WB[q][1] == 0)
					{	
						wb_first_invalid_position = q;
						break;
					}
				}

				for (int j = 0; j < COLUMNS; j++) //transfer from execute_list to WB
				{
					WB[wb_first_invalid_position][j] = execute_list[p][j];

					TS[execute_list[p][0]][17] = cycle + 1 -  TS[execute_list[p][0]][16];
					TS[execute_list[p][0]][18] = cycle + 1;
				}
				
				execute_list[p][1] = 0; //invalidate that instruction in execute_list

			}

			//wakeup dependent instructions in IQ, DI and RR
			Wakeup();
		}
	}
	return 0;
}

int Writeback()
{
	for (int i=0; i < (WIDTH*5); i++ ) // Check if WB not empty
	{
		if (WB[i][1] == 0)
			WB_empty++;
	}

	if (WB_empty >= (WIDTH*5))
		WB_is_empty = 1;
	else
		WB_is_empty = 0;

	if (WB_is_empty == 0) // Check if WB not empty
	{
		for (int i=0; i < (WIDTH*5); i++)
		{
			if (WB[i][1] == 1)
			{
				dest = WB[i][2];

				ROB[dest][9] = 1; // set the ready bit of the corresponding rob entry 

				TS[WB[i][0]][19] = cycle + 1 -  TS[WB[i][0]][18];
				TS[WB[i][0]][20] = cycle + 1;


				WB[i][1] = 0; // invalidate that instruction in WB
			}
		}
	}
	WB_empty = 0;
	return 0;
}

int Retire()
{
	for (int i=0; i < ROB_SIZE; i++ ) // Check if ROB not empty
	{
		if (ROB[i][1] == 0)
			ROB_empty++;
	}

	if (ROB_empty >= ROB_SIZE)
		ROB_is_empty = 1;
	else
		ROB_is_empty = 0;

	if (ROB_is_empty == 0)	// Check if ROB not empty
	{
		for (int i=0; i < WIDTH; i++)
		{
			if ((ROB[head][9] == 1) && (ROB[head][1] == 1))
			{
				ROB[head][1] = 0;
				retirement++;

				TS[ROB[head][0]][21] = cycle + 1 -  TS[ROB[head][0]][20];

				if (ROB[head][10] != -1)
				{
					if (rmt[ROB[head][10]][1] == head)
					{
						rmt[ROB[head][10]][0] = 0;
					}
				}
				
				IncHead();
			}
		}
	}
	ROB_empty = 0;
	return 0;
}

int AdvanceCycle()
{
	cycle++;

	if (retirement == sequence_no - 1)
	{
		fclose(pFile);
		return 0;
	}
	else
		return 1;
}