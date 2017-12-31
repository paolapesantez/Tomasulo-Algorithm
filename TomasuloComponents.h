#ifndef TOMASULOCOMPONENTS_H_INCLUDED
#define TOMASULOCOMPONENTS_H_INCLUDED

#include "Header.h"

//Define the number of entries for: the ReOrder Buffer, Registers: FP & Int. Define number of reservation stations.
#define NUM_QUEUE_INST 0 //TEST
#define NUM_ENTRIES_ROB 128
#define NUM_ENTRIES_RS 32
#define NUM_DISPATCHED_INST 6
#define REGISTERS_FP 16
#define REGISTERS_INT 32
#define NUM_FUNCTIONAL_UNITS 6


//Define the number of cycles that an instruction takes to execute
#define NUM_CYCLES_FP_ADDD_SUBD 2
#define NUM_CYCLES_FP_MULTD 3
#define NUM_CYCLES_FP_DIVD 4
#define NUM_CYCLES_LD 1
#define NUM_CYCLES_SD 1
#define NUM_CYCLES_INT 1
#define NUM_CYCLES_BNE 1
#define NUM_CYCLES_SLTI 1

//Define properties like how many instructions per cycle Tomasulo's Algorith will issue
#define INSTRUCTIONS_PER_CYCLE 2
#define NON_VALUE ""
#define NON_VALUE_NUMBER -1
#define SIZE_MEM 32

//Define classes of operations, stages of a instruction, and so on.
enum Operation{OP_LW=0,OP_LB,OP_LBU,OP_LH,OP_LWR,OP_LWL,OP_LUI,
			   OP_SW,OP_SB,OP_SH,OP_SWR,
			   OP_ADDIU,OP_ADDU,OP_ADDI,OP_SUBU,OP_DIV,OP_MULT,
			   OP_DADD,OP_DSUB,OP_DDIV,OP_DMULT,
			   OP_SLL,OP_SLLV,OP_SLT,OP_SLTI,OP_SLTIU,OP_SLTU,OP_SRA,OP_SRAV,
			   OP_MFLO,
			   OP_AND,OP_ANDI,OP_ORI,OP_OR,OP_XOR,
			   OP_BNE,OP_BEQ,OP_BGEZ,OP_BGTZ,OP_BLEZ,OP_BLTZ,
			   OP_J,OP_JR,OP_JAL};
enum Flag{NO,YES};
enum State{ST_ISSUE,ST_EXECUTE,ST_WRITE_BACK,ST_COMMIT,ST_WAIT_EXECUTE,ST_WAIT_WB,ST_WAIT_COMMIT};
enum TypeRS{TYPE_RS_LOAD, TYPE_RS_STORE, TYPE_RS_FP_ADDSUB, TYPE_RS_FP_MULDIV, TYPE_RS_INTEGER, TYPE_RS_BRANCH};
enum RSStall{STALL_LOAD,STALL_STORE,STALL_FP_ADDSUB,STALL_FP_MULDIV,STALL_INTEGER,STALL_BRANCH};
enum HeaderPrint{HP_ROB,HP_RSTATION,HP_RSTATUS};

struct Instruction
{
	string ins_address;
	int ins_number_instrucion;
	string ins_instruction_complete_long;
	string ins_instruction_complete_short;
	int ins_operation;
	string ins_result;
	string ins_operand1;
	string ins_operand2;
	int ins_number_cycles_execute;
	Flag ins_pipelined;
};

struct ROB
{
	int rob_entry;
	Flag rob_busy;
	string rob_instruction;
	int rob_operation;
	State rob_state;
	string rob_destination;
	string rob_value;
	Flag rob_done;
	int rob_number_cycles_execute; //when the execution starts we print out
	int rob_position_rstation;
	int rob_position_rstatus;
	Flag rob_pipelined;
};

struct FunctionalUnit
{
	string fu_name;
	Flag fu_availability;
};


struct ReservationStation
{
	string res_functional_unit;
	Flag res_availability;
	Flag res_busy;
	Flag res_dispatch;
	int res_operation;
	string res_operation_name;
	string res_vj;
	string res_vk;
	int res_qj;
	int res_qk;
	int res_destination;
	int res_number_cycles_execute;	//keep track of the execution to free the entry in RS
};

struct ReservationStationCycle
{
	deque<ReservationStation> rsc_rsCycle[NUM_FUNCTIONAL_UNITS];
};

struct RegisterFile
{
	string ref_register;
	float ref_value;
};

struct RegisterStatus
{
	string rst_field;
	int rst_numberROB;
	Flag rst_busy;
};

struct RealValues
{
	string rev_address;
	string rev_value;
};

class TomasuloComponents
{
	public:
		TomasuloComponents();
		~TomasuloComponents();
		void readFiles(deque<Instruction> *queue_Instruction, deque<RegisterFile> *queue_Register);
		void printFile(deque<Instruction> ins_instruction, deque<RegisterFile> ref_Register, Flag fla_printAll);
		void printROB(deque<ROB> rob_instruction,int Cycles);
		void printRS(deque<ReservationStation> res_instruction);
		void printRegStatus(deque<RegisterStatus> rst_instruction);
		vector<string> decodeInstruction(string newData, Flag fla_check);
		Instruction fillInstruction(vector<string> ins_Instruction);
		RegisterFile fillRegister(vector<string> reg_Register);

	private:
		string operation[44];
		string state[7];
		string busy[2];
		string functional_unit[6];
};
#endif