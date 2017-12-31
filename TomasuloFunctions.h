#ifndef TOMASULOFUNCTIONS_H_INCLUDED
#define TOMASULOFUNCTIONS_H_INCLUDED

#include "TomasuloComponents.h"

using namespace std;

class TomasuloFunctions
{
	public:
		TomasuloFunctions();
		~TomasuloFunctions();
		//void simulateCycle(deque<Instruction> ins_Instructions, deque<Register> reg_Register);
		void simulateCycle();
		void initializeRS(deque<ReservationStation> *res_values);
		void initializeRST(deque<RegisterStatus> *rst_values);
		void initializeFU( FunctionalUnit (&fu_values)[NUM_FUNCTIONAL_UNITS]);
		void fillROB(Instruction ins_values, /*deque<RegisterStatus> *rst_Instructions,*/ deque<ROB>
		*rob_Instructions);
		void fillRS(Instruction ins_values, deque<RegisterFile> reg_values, ROB *rob_values, deque<ReservationStation>
		*res_instruction, deque<RegisterStatus> *rst_Values);
		Flag checkAvailabilityRS(deque<ReservationStation> res_Instructions);
		string checkAvailabilityFU(int rob_operation, FunctionalUnit (&fu_values)[NUM_FUNCTIONAL_UNITS]);
		Flag modifyCycle(deque<ROB> *rob_Instructions,deque<ReservationStation> *res_Instructions, ReservationStationCycle *rsc_rsCycle, deque<RegisterStatus>
*rst_valuesRegister, deque<RegisterFile> *ref_values, Flag *fla_ControlPipelined,  FunctionalUnit (&fu_values)[NUM_FUNCTIONAL_UNITS]);
		Flag modifyState(ROB *rob_instruction,deque<ROB> rob_values, deque<ReservationStation> *res_instruction,
		deque<ReservationStation> res_values, deque<RegisterStatus> *rst_values, State sta_previousState, deque<RegisterFile> *ref_values, Flag *fla_ControlPipelined, FunctionalUnit (&fu_values)[NUM_FUNCTIONAL_UNITS]);
		int findROB(deque<ROB> rob_Instructions, int entry);
		vector<int> TomasuloFunctions::findRS(std::deque<ReservationStation> rs_Instructions, int entry, int option);
	
	private:
		string operation[44];
		string state[7];
		string busy[2];
		string functional_unit[6];
		int Cycles;
		Flag AvailabilityFU_ALUFP0;
		Flag AvailabilityFU_ALUFP1;
		Flag AvailabilityFU_ALUFP2;
		Flag AvailabilityFU_LD0;
		Flag AvailabilityFU_LD1;
		Flag AvailabilityFU_ST0;
		Flag cycle_noStall;
		deque<RealValues> rev_values;
		int programCounter;
		int number_writeback_cycle;
};
#endif
