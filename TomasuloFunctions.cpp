#include "TomasuloFunctions.h"


void printHeader(HeaderPrint hea_type)
{
	switch(hea_type)
	{
		case HP_ROB:
			cout<<endl;
			cout<<string(77,'-')<<endl<<setw(32)<<"REORDER BUFFER"<<endl<<string(77,'-')<<endl;
			cout<<setw(7)<<"EX_CYC"<<setw(6)<<"ENTRY"<<setw(5)<<"BUSY"
			<<setw(20)<<"INSTRUCTION"<<setw(13)<<"STATE"
			<<setw(14)<<"DEST"<<setw(8)<<"VALUE"<<"DONE"<<endl;
			cout<<string(77, '.')<<endl;
		break;
		case HP_RSTATION:
			cout<<endl;
			cout<<string(70,'-')<<endl<<setw(32)<<"RESERVATION STATION"<<endl<<string(70,'-')<<endl;
			cout<<setw(6)<<left<<"BUSY"<<setw(10)<<"FUN.UNIT"<<setw(9)<<"OP"<<setw(7)<<"VJ"
			<<setw(7)<<"VK"<<setw(10)<<"QJ"<<setw(10)<<"QK"<<"DEST"<<endl;
			cout<<string(70, '.')<<endl;
		break;
		case HP_RSTATUS:
		break;
	}
}

int convertIntReg(string str_value,Flag fla_flag)
{
	char c[7];
	int size_string=0;
	if(fla_flag)
		str_value.copy(c,2,0);
	else
		return atoi(str_value.c_str()); 
	return atoi(c);
}

float convertFloatReg(string str_value,Flag fla_flag)
{
	char c[7];
	float size_string=0;
	if(fla_flag)
		str_value.copy(c,2,0);
	else
		return atof(str_value.c_str()); 
	return atof(c);
}

string convertString(int int_value)
{
	ostringstream s;
	s << int_value;
	return s.str();
}

string convertStringFloat(float flo_value)
{
	ostringstream s;
	s << flo_value;
	return s.str();
}

TomasuloFunctions::TomasuloFunctions()
{
	operation[0] = "LW";
	operation[1] = "LB";	
	operation[2] = "LBU";
	operation[3] = "LH";
	operation[4] = "LWR";
	operation[5] = "LWL";
	operation[6] = "LUI";			   
	operation[7] = "SW";
	operation[8] = "SB";
	operation[9] = "SH";
	operation[10] = "SWR";			   
	operation[11] = "ADDIU";
	operation[12] = "ADDU";
	operation[13] = "ADDI";
	operation[14] = "SUBU";
	operation[15] = "DIV";
	operation[16] = "MULT";
	operation[17] = "DADD";
	operation[18] = "DSUB";
	operation[19] = "DDIV";
	operation[20] = "DMULT";
	operation[21] = "SLL";
	operation[22] = "SLLV";
	operation[23] = "SLT";
	operation[24] = "SLTI";
	operation[25] = "SLTIU";
	operation[26] = "SLTU";
	operation[27] = "SRA";
	operation[28] = "SRAV";
	operation[29] = "MFLO";
	operation[30] = "AND";
	operation[31] = "ANDI";
	operation[32] = "ORI";
	operation[33] = "OR";
	operation[34] = "XOR";
	operation[35] = "BNE";
	operation[36] = "BEQ";
	operation[37] = "BGEZ";
	operation[38] = "BGTZ";
	operation[39] = "BLEZ";
	operation[40] = "BLTZ";
	operation[41] = "J";
	operation[42] = "JR";
	operation[43] = "JAL";

	state[0] = "ISSUE";
	state[1] = "EXECUTE";
	state[2] = "WRITE_BACK";
	state[3] = "COMMIT";
	state[4] = "WAIT_EXECUTE";
	state[5] = "WAIT_WB";
	state[6] = "WAIT_COMMIT";

	busy[0] = "NO";
	busy[1] = "YES";
	
	functional_unit[0] = "ALUFP0"; //FP addition - substraction & multiplication - division & Integer
	functional_unit[1] = "ALUFP1"; //FP addition - substraction & multiplication - division & Integer
	functional_unit[2] = "ALUFP2"; //FP addition - substraction & multiplication - division & Integer
	functional_unit[3] = "LD0"; //Load 
	functional_unit[4] = "LD1"; //Load
	functional_unit[5] = "ST0"; //Store
	
	Cycles = 0;
	AvailabilityFU_ALUFP0 = YES;
	AvailabilityFU_ALUFP1 = YES;
	AvailabilityFU_ALUFP2 = YES;
	AvailabilityFU_LD0 = YES;
	AvailabilityFU_LD1 = YES;
	AvailabilityFU_ST0 = YES;
	cycle_noStall = YES;
	programCounter = 0;
	//rev_values = NULL;
}

TomasuloFunctions::~TomasuloFunctions()
{}

void TomasuloFunctions::simulateCycle()
{
	system("cls");
	Flag done = NO;
	deque<Instruction> ins_Values;
	deque<RegisterFile> ref_Values;
	deque<RegisterStatus> rst_Values(REGISTERS_FP+REGISTERS_INT);
	deque<ROB> rob_Values;
	ReservationStationCycle rsc_rsCycle;
	deque<ReservationStation> res_valuesRS(NUM_ENTRIES_RS);
	FunctionalUnit fu_values[NUM_FUNCTIONAL_UNITS] = {};
	initializeRST(&rst_Values);
	initializeRS(&res_valuesRS);
	initializeFU(fu_values);
	
	TomasuloComponents test;
	test.readFiles(&ins_Values,&ref_Values);
	test.printFile(ins_Values,ref_Values, YES);
	test.printROB(rob_Values,Cycles);
	//system("pause");
	int resSTALL=-1;
	Flag fla_ControlPipelined = YES;
	int count_ControlLD = 0; //CONTROLS ONE LOAD IN SUPERSCALAR
	int count_ControlST = 0;
	int count_ControlADD_D = 0;
	int count_ControlMULT_D = 0;
	int count_ControlINT = 0;
	int i=0, remain = 0, size = 0;
	
	while (done == NO){
		while(!ins_Values.empty() || !rob_Values.empty() && rob_Values.size() < NUM_ENTRIES_ROB)
		{
			i=0;
			remain = ins_Values.size()%INSTRUCTIONS_PER_CYCLE;
			if (remain == ins_Values.size())
				size = ins_Values.size();
			else
				size = INSTRUCTIONS_PER_CYCLE;

			for(;i<size;i++)//issue number of instructions you defined
			{
				system("cls");
				if(!ins_Values.empty())
					switch(ins_Values.front().ins_operation)
					{
						case OP_LW: case OP_LB: case OP_LBU: case OP_LH: case OP_LWR: case OP_LWL: case OP_LUI: 
							cycle_noStall = checkAvailabilityRS(res_valuesRS); 
							resSTALL = STALL_LOAD; 
							break; //LD
						case OP_SW: case OP_SB: case OP_SH: case OP_SWR:  
							cycle_noStall = checkAvailabilityRS(res_valuesRS); 
							resSTALL = STALL_STORE; 
							break; //SD
						case OP_ADDIU: case OP_ADDU: case OP_ADDI: case OP_SUBU: case OP_DIV: case OP_MULT:
						case OP_SLL: case OP_SLLV: case OP_SLT: case OP_SLTI: case OP_SLTIU: case OP_SLTU: case OP_SRA: case OP_SRAV:
						case OP_MFLO:
						case OP_AND: case OP_ANDI: case OP_ORI: case OP_OR: case OP_XOR:
						case OP_BNE: case OP_BEQ: case OP_BGEZ: case OP_BGTZ: case OP_BLEZ: case OP_BLTZ:
						case OP_J: case OP_JR: case OP_JAL:
							cycle_noStall = checkAvailabilityRS(res_valuesRS);
							resSTALL = STALL_INTEGER;
							break; //ADD//ADDDI//SUB//SUBI//BRANCH
						case OP_DADD: 	case OP_DSUB:
							cycle_noStall = checkAvailabilityRS(res_valuesRS);
							resSTALL = STALL_FP_ADDSUB;
							break;//ADD_D//SUB_D
						case OP_DMULT: case OP_DDIV:
							cycle_noStall = checkAvailabilityRS(res_valuesRS);
							resSTALL = STALL_FP_MULDIV;
							break; //MUL_D //DIV_D
					}
				if(!ins_Values.empty() && cycle_noStall && rob_Values.size() < NUM_ENTRIES_ROB)
				{
					if(!fla_ControlPipelined)
						fla_ControlPipelined = rob_Values.back().rob_number_cycles_execute == 0 ? YES : NO;
					if(i == 0)
					{ 
						count_ControlLD = count_ControlST = count_ControlADD_D = count_ControlMULT_D = count_ControlINT= 0;
					}
					if(fla_ControlPipelined)
					{
						fillROB(ins_Values.front()/*,&rst_Values*/,&rob_Values);
						switch(ins_Values.front().ins_operation)
						{
							case OP_LW: case OP_LB: case OP_LBU: case OP_LH: case OP_LWR: case OP_LWL: case OP_LUI:  
								fillRS(ins_Values.front(),ref_Values,&rob_Values.back(),&res_valuesRS,&rst_Values);
								count_ControlLD++;							
							break; //LD
						
							case OP_SW: case OP_SB: case OP_SH: case OP_SWR:  
								fillRS(ins_Values.front(),ref_Values,&rob_Values.back(),&res_valuesRS,&rst_Values);
								count_ControlST++;							
							break; //SD
						
							case OP_ADDIU: case OP_ADDU: case OP_ADDI: case OP_SUBU: case OP_DIV: case OP_MULT:
							case OP_SLL: case OP_SLLV: case OP_SLT: case OP_SLTI: case OP_SLTIU: case OP_SLTU: case OP_SRA: case OP_SRAV:
							case OP_MFLO:
							case OP_AND: case OP_ANDI: case OP_ORI: case OP_OR: case OP_XOR:
							case OP_BNE: case OP_BEQ: case OP_BGEZ: case OP_BGTZ: case OP_BLEZ: case OP_BLTZ:
							case OP_J: case OP_JR: case OP_JAL:
								fillRS(ins_Values.front(),ref_Values,&rob_Values.back(),&res_valuesRS,&rst_Values);
								count_ControlINT++;							
							break;
							case OP_DADD: //ADD_D 
							case OP_DSUB: //SUB_D
								fillRS(ins_Values.front(),ref_Values,&rob_Values.back(),&res_valuesRS,&rst_Values);
								count_ControlADD_D++;						
							break;
							case OP_DMULT: //MUL_D
							case OP_DDIV: //DIV_D
								fillRS(ins_Values.front(),ref_Values,&rob_Values.back(),&res_valuesRS,&rst_Values);
								count_ControlMULT_D++;							
							break;
						}					
						ins_Values.pop_front();
					}
				}
			else
				if(!cycle_noStall)
				{
					//cout<<"CYCLE STALL - RESERVATION STATION"<<functional_unit[resSTALL]<<endl<<string(60,'-')<<endl<<endl<<string(60,'-')<<endl;
					programCounter += i;
					break;
				}
			}
			cout<<endl<<endl<<"CYCLE: "<<Cycles<<endl;
			Cycles++;
			system("cls");
			test.printFile(ins_Values,ref_Values,NO);
			printHeader(HP_RSTATION);
			test.printRS(res_valuesRS);
			test.printRegStatus(rst_Values);
			printHeader(HP_ROB);
			test.printROB(rob_Values,Cycles);
			rsc_rsCycle.rsc_rsCycle[0] = res_valuesRS;
			modifyCycle(&rob_Values,&res_valuesRS,&rsc_rsCycle,&rst_Values,&ref_Values,&fla_ControlPipelined,fu_values);
			res_valuesRS = rsc_rsCycle.rsc_rsCycle[0];
			//if (Cycles >= 50)
				//system("pause");
		}
		system("cls");
		done = YES;
		cout<<endl<<endl<<"CYCLE: "<<Cycles<<endl;
		test.printFile(ins_Values,ref_Values,NO);
		printHeader(HP_RSTATION);
		test.printRS(res_valuesRS);
		test.printRegStatus(rst_Values);
		printHeader(HP_ROB);
		test.printROB(rob_Values,Cycles);
		rsc_rsCycle.rsc_rsCycle[0] = res_valuesRS;
		system("pause");
		rob_Values.~deque();
		res_valuesRS.~deque();
		ins_Values.~deque();
		ref_Values.~deque();
		rst_Values.~deque();
	}
}



void TomasuloFunctions::initializeRS(deque<ReservationStation> *res_value)
{
	int size = res_value->size();

	for(int i=0;i<size;i++)
	{
		res_value->at(i).res_functional_unit = "";
		res_value->at(i).res_busy = NO;
		res_value->at(i).res_dispatch = NO;
		res_value->at(i).res_destination = NON_VALUE_NUMBER;
		res_value->at(i).res_availability = YES;
		res_value->at(i).res_number_cycles_execute = NON_VALUE_NUMBER;
		res_value->at(i).res_operation_name = "";
		res_value->at(i).res_operation = NON_VALUE_NUMBER;
		res_value->at(i).res_qj = NON_VALUE_NUMBER;
		res_value->at(i).res_qk = NON_VALUE_NUMBER;
		res_value->at(i).res_vj = NON_VALUE;
		res_value->at(i).res_vk = NON_VALUE;
	}
}


void TomasuloFunctions::initializeFU( FunctionalUnit (&fu_values)[NUM_FUNCTIONAL_UNITS])
{
	fu_values[0].fu_name = "ALUFP0";
	fu_values[0].fu_availability = YES;
	fu_values[1].fu_name = "ALUFP1";
	fu_values[1].fu_availability = YES;
	fu_values[2].fu_name = "ALUFP2";
	fu_values[2].fu_availability = YES;
	fu_values[3].fu_name = "LD0";
	fu_values[3].fu_availability = YES;
	fu_values[4].fu_name = "LD1";
	fu_values[4].fu_availability = YES;
	fu_values[5].fu_name = "ST0";
	fu_values[5].fu_availability = YES;

}


void TomasuloFunctions::initializeRST(deque<RegisterStatus> *rst_values)
{
	int i = 0;
	try
	{
		for(;i<REGISTERS_FP;i++)
		{
			rst_values->at(i).rst_field = "F" + static_cast<ostringstream*>( &(ostringstream() << i*2) )->str();
			rst_values->at(i).rst_busy = NO;
			rst_values->at(i).rst_numberROB = NON_VALUE_NUMBER;
		}
		for(i=REGISTERS_FP;i<REGISTERS_INT+REGISTERS_FP;i++)
		{
			rst_values->at(i).rst_field = "R" + convertString(abs(i-REGISTERS_INT+REGISTERS_FP));
			rst_values->at(i).rst_busy = NO;
			rst_values->at(i).rst_numberROB = NON_VALUE_NUMBER;
		}
	}
	catch (runtime_error &e)
	{
		cout<<e.what()<< " initializeRST " <<endl;
	}
	catch(...)
	{
		cout<<"Unknown error in initializeRST"<<endl;
	}
}


void TomasuloFunctions::fillROB(Instruction ins_values, /*deque<RegisterStatus> *rst_Instructions,*/ deque<ROB> *rob_Instructions )
{
	ROB values;
	std::string result="";
	int size = rob_Instructions->size(), position_rstatus = 0;
	size_t found=0;

	if(size<NUM_ENTRIES_ROB)
	{		
		values.rob_entry = size == 0 ? 1 : rob_Instructions->at(size-1).rob_entry + 1;
		values.rob_busy = YES;
		values.rob_instruction = ins_values.ins_instruction_complete_short;
		values.rob_operation = ins_values.ins_operation;
		values.rob_state = ST_ISSUE;
		result = ins_values.ins_result;
		values.rob_destination = result;
		values.rob_value = NON_VALUE;
		values.rob_done = NO;
		values.rob_number_cycles_execute = ins_values.ins_number_cycles_execute;
		values.rob_pipelined = ins_values.ins_pipelined;		
		if((ins_values.ins_operation == OP_DADD) || (ins_values.ins_operation == OP_DSUB) || (ins_values.ins_operation == OP_DMULT) || (ins_values.ins_operation == OP_DDIV))//operations whose result is float
		{
			position_rstatus = convertFloatReg(result,YES)/2;
		}
		else
		{
			position_rstatus = convertIntReg(result,YES)+(REGISTERS_FP);
		}
	}
		values.rob_position_rstatus = position_rstatus;
		rob_Instructions->push_back(values);
}


void TomasuloFunctions::fillRS(Instruction ins_values, deque<RegisterFile> ref_values, ROB *rob_values, deque<ReservationStation>
*res_instruction, deque<RegisterStatus> *rst_Values)
{
	int i=0,j=0,size=res_instruction->size();	
	int index_operand1,index_operand2,rob1,rob2;
	Flag flag_RStoperand1=NO,flag_RStoperand2=NO;	
	for(;i<size;i++)
	{
		if(res_instruction->at(i).res_availability)
		break;
	}
	rob_values->rob_position_rstation = i;
	
	switch(ins_values.ins_operation)
	{
		case OP_LW: case OP_LB: case OP_LBU: case OP_LH: case OP_LWR: case OP_LWL: case OP_LUI:  
			res_instruction->at(i).res_number_cycles_execute = NUM_CYCLES_LD;
		break; //LD
		case OP_SW: case OP_SB: case OP_SH: case OP_SWR:  
			res_instruction->at(i).res_number_cycles_execute = NUM_CYCLES_SD;	
		break; //SD
		case OP_BNE: case OP_BEQ: case OP_BGEZ: case OP_BGTZ: case OP_BLEZ: case OP_BLTZ:
		case OP_J: case OP_JR: case OP_JAL:				
			res_instruction->at(i).res_number_cycles_execute = NUM_CYCLES_BNE;
		break; //BNEZ
		case OP_ADDIU: case OP_ADDU: case OP_ADDI: case OP_SUBU: case OP_DIV: case OP_MULT:
		case OP_SLL: case OP_SLLV: case OP_SLT: case OP_SLTI: case OP_SLTIU: case OP_SLTU: case OP_SRA: case OP_SRAV:
		case OP_MFLO:
		case OP_AND: case OP_ANDI: case OP_ORI: case OP_OR:	case OP_XOR:
			res_instruction->at(i).res_number_cycles_execute = NUM_CYCLES_INT;
		break;
		case OP_DADD: //ADD_D
		case OP_DSUB: //SUB_D
			res_instruction->at(i).res_number_cycles_execute = NUM_CYCLES_FP_ADDD_SUBD;
		break;
		case OP_DMULT: //MUL_D
			res_instruction->at(i).res_number_cycles_execute = NUM_CYCLES_FP_MULTD;
		break;
		case OP_DDIV: //DIV_D
			res_instruction->at(i).res_number_cycles_execute = NUM_CYCLES_FP_DIVD;
		break;
	}
	//res_instruction->at(i).res_functional_unit = functional_unit_name;
	res_instruction->at(i).res_busy = YES;
	res_instruction->at(i).res_availability = NO;
	res_instruction->at(i).res_destination = rob_values->rob_entry;
	res_instruction->at(i).res_operation_name = operation[ins_values.ins_operation];
	res_instruction->at(i).res_operation = ins_values.ins_operation;
	try
	{		
		if((res_instruction->at(i).res_operation != OP_JAL) && (res_instruction->at(i).res_operation != OP_J))
		{
			index_operand1 = ((stoi(ins_values.ins_operand1) >= 0) && (stoi(ins_values.ins_operand1) < 32)) ? convertIntReg(ins_values.ins_operand1,YES) : NON_VALUE_NUMBER ;			
			index_operand2 = ((stoi(ins_values.ins_operand2) >= 0) && (stoi(ins_values.ins_operand2) < 32)) ? convertIntReg(ins_values.ins_operand2,YES) : NON_VALUE_NUMBER;
			if((ins_values.ins_operation == OP_DADD) || (ins_values.ins_operation == OP_DSUB) || (ins_values.ins_operation == OP_DMULT) || (ins_values.ins_operation == OP_DDIV))//operations whose result is float
			{
				if ((stoi(ins_values.ins_operand1) >= 0) && (stoi(ins_values.ins_operand1) < 32))
					flag_RStoperand1 = rst_Values->at(index_operand1/2).rst_busy;
				if ((stoi(ins_values.ins_operand2) >= 0) && (stoi(ins_values.ins_operand2) < 32))
					flag_RStoperand2 = rst_Values->at(index_operand2/2).rst_busy;
			}else
				if ((stoi(ins_values.ins_operand1) >= 0) && (stoi(ins_values.ins_operand1) < 32))
					flag_RStoperand1 = rst_Values->at(index_operand1+REGISTERS_FP).rst_busy;
				if ((stoi(ins_values.ins_operand2) >= 0) && (stoi(ins_values.ins_operand2) < 32))
					flag_RStoperand2 = rst_Values->at(index_operand2+REGISTERS_FP).rst_busy;
			
			if(((stoi(ins_values.ins_operand1) >= 0) && (stoi(ins_values.ins_operand1) < 32)) && flag_RStoperand1)
			{
				if((ins_values.ins_operation == OP_DADD) || (ins_values.ins_operation == OP_DSUB) || (ins_values.ins_operation == OP_DMULT) || (ins_values.ins_operation == OP_DDIV))//operations whose result is float					
						rob1 = rst_Values->at(index_operand1/2).rst_numberROB;
				else				
						rob1 = rst_Values->at(index_operand1+REGISTERS_FP).rst_numberROB;
				/*if(ins_values.ins_operand1 == convertString(rob_values->rob_entry))
					res_instruction->at(i).res_qj = rst_Values->at(rob_values->rob_position_rstatus).rst_numberROB;
				else*/
					res_instruction->at(i).res_qj = rob1;
			}
			else
			{
				if(((stoi(ins_values.ins_operand1) >= 0) && (stoi(ins_values.ins_operand1) < 32)) && !flag_RStoperand1)
				{
					int regist = convertIntReg(ins_values.ins_operand1,YES);
					if((ins_values.ins_operation == OP_DADD) || (ins_values.ins_operation == OP_DSUB) || (ins_values.ins_operation == OP_DMULT) || (ins_values.ins_operation == OP_DDIV))//operations whose result is float						
							regist = ref_values.at(regist/2).ref_value;
					else
							regist = ref_values.at(regist+REGISTERS_FP).ref_value;
				
					res_instruction->at(i).res_vj = convertString(regist);
				}
				else
				{
					if((stoi(ins_values.ins_operand2) < 0) || (stoi(ins_values.ins_operand2) > 31))
						res_instruction->at(i).res_vj = ins_values.ins_operand1;
				}
			}
			
				if(((stoi(ins_values.ins_operand2) >= 0) && (stoi(ins_values.ins_operand2) < 32)) && flag_RStoperand2)
				{
					if((ins_values.ins_operation == OP_DADD) || (ins_values.ins_operation == OP_DSUB) || (ins_values.ins_operation == OP_DMULT) || (ins_values.ins_operation == OP_DDIV))//operations whose result is float					
							rob2 = rst_Values->at(index_operand2/2).rst_numberROB;
					else					
							rob2 = rst_Values->at(index_operand2+REGISTERS_FP).rst_numberROB;			
					/*if(ins_values.ins_operand2 == convertString(rob_values->rob_entry))
						res_instruction->at(i).res_qk = rst_Values->at(rob_values->rob_position_rstatus).rst_numberROB;
					else*/
						res_instruction->at(i).res_qk = rob2;
				}
				else
				{
					if(((stoi(ins_values.ins_operand2) >= 0) && (stoi(ins_values.ins_operand2) < 32)) && !flag_RStoperand2)
					{
						int regist = convertIntReg(ins_values.ins_operand2,YES);
						if((ins_values.ins_operation == OP_DADD) || (ins_values.ins_operation == OP_DSUB) || (ins_values.ins_operation == OP_DMULT) || (ins_values.ins_operation == OP_DDIV))//operations whose result is float
							regist = ref_values.at(regist/2).ref_value;
						else
							regist = ref_values.at(regist+REGISTERS_FP).ref_value;
						res_instruction->at(i).res_vk = convertString(regist);
					}
					else
					{
						if((stoi(ins_values.ins_operand2) < 0) || (stoi(ins_values.ins_operand2) > 31))
							res_instruction->at(i).res_vk = ins_values.ins_operand2;
					}
				}		
		}
		else 
		{
			res_instruction->at(i).res_vj = ins_values.ins_operand1;
			res_instruction->at(i).res_vk = ins_values.ins_operand2;
		}
	}
	catch (runtime_error &e)
	{
		cout<<e.what()<< " fillRS " <<endl;
	}
		catch(...)

	{
		cout<<"Unknown error in fillRS"<<endl;
	}
	if ((res_instruction->at(i).res_operation != OP_J) && (res_instruction->at(i).res_operation != OP_SW) && (res_instruction->at(i).res_operation != OP_SB) && (res_instruction->at(i).res_operation != OP_SH) && (res_instruction->at(i).res_operation != OP_SWR))
	{
		rst_Values->at(rob_values->rob_position_rstatus).rst_busy = YES;
		rst_Values->at(rob_values->rob_position_rstatus).rst_numberROB = rob_values->rob_entry;
	}	
}



Flag TomasuloFunctions::checkAvailabilityRS(deque<ReservationStation> res_Instructions)
{
	Flag fla_return = NO;
	int size = res_Instructions.size();
	for(int i=0;i<size;i++)
	{
		if(res_Instructions.at(i).res_availability)
		{
			fla_return = YES;
			break;
		}
	}
	return fla_return;
}


string TomasuloFunctions::checkAvailabilityFU(int rob_operation, FunctionalUnit (&fu_values)[NUM_FUNCTIONAL_UNITS])
{
	string name_return = "NO";
	switch(rob_operation)
		{
			case OP_LW: case OP_LB: case OP_LBU: case OP_LH: case OP_LWR: case OP_LWL: case OP_LUI:
				if (fu_values[3].fu_availability == YES)
				{
					name_return = fu_values[3].fu_name;
					fu_values[3].fu_availability = NO;
					break;
				}
				else if (fu_values[4].fu_availability == YES)
				{
					name_return = fu_values[4].fu_name;
					fu_values[4].fu_availability = NO;
					break;
				}
			break;
			case OP_SW: case OP_SB: case OP_SH: case OP_SWR:  
				if (fu_values[5].fu_availability == YES)
				{
					name_return = fu_values[5].fu_name;
					fu_values[5].fu_availability = NO;
					break;
				}
			break;
			case OP_BNE: case OP_BEQ: case OP_BGEZ: case OP_BGTZ: case OP_BLEZ: case OP_BLTZ:
			case OP_J: case OP_JR: case OP_JAL:				
				if (fu_values[0].fu_availability == YES)
				{
					name_return = fu_values[0].fu_name;
					fu_values[0].fu_availability = NO;
					break;
				}
				else if (fu_values[1].fu_availability == YES)
				{
					name_return = fu_values[1].fu_name;
					fu_values[1].fu_availability = NO;
					break;
				}
				else if (fu_values[2].fu_availability == YES)
				{
					name_return = fu_values[2].fu_name;
					fu_values[2].fu_availability = NO;
					break;
				}
			break;
			case OP_ADDIU: case OP_ADDU: case OP_ADDI: case OP_SUBU: case OP_DIV: case OP_MULT:
			case OP_SLL: case OP_SLLV: case OP_SLT: case OP_SLTI: case OP_SLTIU: case OP_SLTU: case OP_SRA: case OP_SRAV:
			case OP_MFLO:
			case OP_AND: case OP_ANDI: case OP_ORI: case OP_OR: case OP_XOR:	
			case OP_DADD: case OP_DSUB:
			case OP_DMULT: case OP_DDIV:															
				if (fu_values[0].fu_availability == YES)
				{
					name_return = fu_values[0].fu_name;
					fu_values[0].fu_availability = NO;
					break;
				}
				else if (fu_values[1].fu_availability == YES)
				{
					name_return = fu_values[1].fu_name;
					fu_values[1].fu_availability = NO;
					break;
				}
				else if (fu_values[2].fu_availability == YES)
				{
					name_return = fu_values[2].fu_name;
					fu_values[2].fu_availability = NO;
					break;
				}
			break;		
	}
	return name_return;
}


Flag TomasuloFunctions::modifyCycle(deque<ROB> *rob_Instructions,deque<ReservationStation> *res_Instructions, ReservationStationCycle *rsc_rsCycle, deque<RegisterStatus>
*rst_valuesRegister, deque<RegisterFile> *ref_values, Flag *fla_ControlPipelined, FunctionalUnit (&fu_values)[NUM_FUNCTIONAL_UNITS])
{
	int i = 0, size = rob_Instructions->size();
	int operation=NON_VALUE_NUMBER;
	State sta_previousState;
	int cycle_commit_counter = 0;	
	number_writeback_cycle = 0;

	try
	{
		if(!*fla_ControlPipelined)
		{
			int i=0;
			
			modifyState(&rob_Instructions->at(i),*rob_Instructions,&rsc_rsCycle->rsc_rsCycle[TYPE_RS_LOAD],*res_Instructions,rst_valuesRegister,ST_WRITE_BACK,ref_values,fla_ControlPipelined,fu_values);
			if(rob_Instructions->at(i).rob_number_cycles_execute == 0)
				*fla_ControlPipelined = YES;
		}
		else
		{			
			back:
			for(int i=0; i<rob_Instructions->size(); i++)
			{
				if(*fla_ControlPipelined)
				{
					if((rob_Instructions->at(i).rob_state == ST_COMMIT) && (cycle_commit_counter < INSTRUCTIONS_PER_CYCLE) && (i == 0))
					{
						rob_Instructions->pop_front();
						cycle_commit_counter++;
						goto back;						
					}//COMMIT IN ORDER
					sta_previousState = i == 0 ? ST_COMMIT : rob_Instructions->at(i-1).rob_state;
					switch(rob_Instructions->at(i).rob_operation)
					{
						case OP_LW: case OP_LB: case OP_LBU: case OP_LH: case OP_LWR: case OP_LWL: case OP_LUI:
							modifyState(&rob_Instructions->at(i),*rob_Instructions,&rsc_rsCycle->rsc_rsCycle[TYPE_RS_LOAD],*res_Instructions,rst_valuesRegister,sta_previousState,ref_values,fla_ControlPipelined,fu_values);
						break; //LD
						case OP_SW: case OP_SB: case OP_SH: case OP_SWR:  
							modifyState(&rob_Instructions->at(i),*rob_Instructions,&rsc_rsCycle->rsc_rsCycle[TYPE_RS_LOAD],*res_Instructions,rst_valuesRegister,sta_previousState,ref_values,fla_ControlPipelined,fu_values);
						break; //SD
						case OP_BNE: case OP_BEQ: case OP_BGEZ: case OP_BGTZ: case OP_BLEZ: case OP_BLTZ:
						case OP_J: case OP_JR: case OP_JAL:				
							modifyState(&rob_Instructions->at(i),*rob_Instructions,&rsc_rsCycle->rsc_rsCycle[TYPE_RS_LOAD],*res_Instructions,rst_valuesRegister,sta_previousState,ref_values,fla_ControlPipelined,fu_values);
						break; //BNEZ
						case OP_ADDIU: case OP_ADDU: case OP_ADDI: case OP_SUBU: case OP_DIV: case OP_MULT:
						case OP_SLL: case OP_SLLV: case OP_SLT: case OP_SLTI: case OP_SLTIU: case OP_SLTU: case OP_SRA: case OP_SRAV:
						case OP_MFLO:
						case OP_AND: case OP_ANDI: case OP_ORI: case OP_OR:	case OP_XOR:
							modifyState(&rob_Instructions->at(i),*rob_Instructions,&rsc_rsCycle->rsc_rsCycle[TYPE_RS_LOAD],*res_Instructions,rst_valuesRegister,sta_previousState,ref_values,fla_ControlPipelined,fu_values);
						break; //ADD//ADDDI//SUB//SUBI
						case OP_DADD: case OP_DSUB:
							modifyState(&rob_Instructions->at(i),*rob_Instructions,&rsc_rsCycle->rsc_rsCycle[TYPE_RS_LOAD],*res_Instructions,rst_valuesRegister,sta_previousState,ref_values,fla_ControlPipelined,fu_values);
						break;//ADD_D//SUB_D
						case OP_DMULT: case OP_DDIV:
							modifyState(&rob_Instructions->at(i),*rob_Instructions,&rsc_rsCycle->rsc_rsCycle[TYPE_RS_LOAD],*res_Instructions,rst_valuesRegister,sta_previousState,ref_values,fla_ControlPipelined,fu_values);
						break; //MUL_D //DIV_D*/
					}
				}
			}
		}
	}
	catch (runtime_error &e)
	{
	cout<<e.what()<< " modifyCycle " <<endl;
	}
	catch(...)
	{
	cout<<"Unknown error in modifyCycle "<<rob_Instructions->at(i).rob_instruction<<endl;
	}
	return YES;
}


Flag TomasuloFunctions::modifyState(ROB *rob_instruction,deque<ROB> rob_values, deque<ReservationStation> *res_instruction,deque<ReservationStation> res_values,
deque<RegisterStatus> *rst_values, State sta_previousState, deque<RegisterFile> *ref_values, Flag *fla_ControlPipelined,  FunctionalUnit (&fu_values)[NUM_FUNCTIONAL_UNITS])
{
	//TYPE_RS_LOAD, TYPE_RS_STORE, TYPE_RS_FP_ADDSUB, TYPE_RS_FP_MULDIV, TY_PERS_INTEGER
	State sta_state = rob_instruction->rob_state;
	int ope_operation = rob_instruction->rob_operation;
	string vj = res_instruction->at(rob_instruction->rob_position_rstation).res_vj;
	string vk = res_instruction->at(rob_instruction->rob_position_rstation).res_vk;
	int qj = res_instruction->at(rob_instruction->rob_position_rstation).res_qj;
	int qk = res_instruction->at(rob_instruction->rob_position_rstation).res_qk;
	string functional_unit_available = "";
	vector<int> rs_positions;
	string operation;
	size_t end = rob_instruction->rob_instruction.find(" ");													
	operation = rob_instruction->rob_instruction.substr(0,end);						

	try
	{
		
		if((rob_instruction->rob_entry == res_instruction->at(rob_instruction->rob_position_rstation).res_destination) || (sta_state != ST_COMMIT))
		{			
			if((sta_state == ST_ISSUE && vj != NON_VALUE && vk != NON_VALUE) || (sta_state == ST_WAIT_EXECUTE && vj != NON_VALUE && vk != NON_VALUE) || (((operation.compare("SW") == 0) || (operation.compare("SB") == 0) || (operation.compare("SH") == 0) || (operation.compare("SWR") == 0)) && (sta_state == ST_ISSUE) && (vk != NON_VALUE)))
			{
				functional_unit_available = checkAvailabilityFU(rob_instruction->rob_operation,fu_values);		
				res_instruction->at(rob_instruction->rob_position_rstation).res_dispatch = YES;
				if (functional_unit_available.compare("NO") != 0)
				{					
					res_instruction->at(rob_instruction->rob_position_rstation).res_functional_unit = functional_unit_available;
					rob_instruction->rob_state = ST_EXECUTE;
				}
				else
				{
					rob_instruction->rob_state = ST_WAIT_EXECUTE;
				}
			}
			else
				if(sta_state == ST_EXECUTE || (sta_state == ST_ISSUE && (vj != NON_VALUE && vk !=NON_VALUE)) || (sta_state == ST_WAIT_EXECUTE && (vj != NON_VALUE && vk != NON_VALUE)))
				{
					if(sta_state == ST_WAIT_EXECUTE)
						rob_instruction->rob_state = ST_EXECUTE;
					if(rob_instruction->rob_number_cycles_execute > 0)
						rob_instruction->rob_number_cycles_execute--;						
					if(rob_instruction->rob_number_cycles_execute == 0 && (vj != NON_VALUE && vk!= NON_VALUE))
					{
						if (number_writeback_cycle < INSTRUCTIONS_PER_CYCLE)
						{
								rob_instruction->rob_state = ST_WRITE_BACK;
								number_writeback_cycle++;
						}else
						{
							rob_instruction->rob_state = ST_WAIT_WB;
						}
							switch(rob_instruction->rob_operation)
							{
								case OP_LW: case OP_LB: case OP_LBU: case OP_LH: case OP_LWR: case OP_LWL: case OP_LUI:
									rob_instruction->rob_value = convertString(stoi(vj)+stoi(vk));
									//ref_values->at(rob_instruction->rob_position_rstatus).ref_value = convertFloatReg(rob_instruction->rob_value,NO);
									//cout << "Entro en 1";
								break; //LD
								case OP_SW: case OP_SB: case OP_SH: case OP_SWR:   									
								{	int temp = stoi(rob_instruction->rob_destination)+REGISTERS_FP;
									//rob_instruction->rob_value = convertString(stoi(vk) + ref_values->at(temp).ref_value);
									char mem[5];
									size_t begin = rob_instruction->rob_instruction.find(" ")+1;									
									size_t end = rob_instruction->rob_instruction.find(",");
									rob_instruction->rob_instruction.copy(mem,int(end-1),begin);																											
									if (vj != NON_VALUE)
									{	if ((rst_values->at(atoi(mem)+(REGISTERS_FP)).rst_busy == NO)||((rst_values->at(atoi(mem)+(REGISTERS_FP)).rst_busy == YES) && (rst_values->at(atoi(mem)+(REGISTERS_FP)).rst_numberROB > rob_instruction->rob_entry)))
										{
											rob_instruction->rob_state = ST_WRITE_BACK;
											rob_instruction->rob_value = convertString(ref_values->at(atoi(mem)+REGISTERS_FP).ref_value);
											//cout << "Entro en 2";
										}
										else
											rob_instruction->rob_state = ST_WAIT_WB;
									}else
										rob_instruction->rob_state = ST_WAIT_WB;}
								break; //SD
								case OP_BNE: case OP_BEQ: case OP_BGEZ: case OP_BGTZ: case OP_BLEZ: case OP_BLTZ:
								{	string temp = convertStringFloat(abs(convertFloatReg(vj,NO)-convertFloatReg(vk,NO)));									
									if (stoi(temp) == 0)
									{	rob_instruction->rob_value = rob_instruction->rob_destination;}
									else
									{rob_instruction->rob_value = "4";}}
									//ref_values->at(rob_instruction->rob_position_rstatus).ref_value = convertFloatReg(rob_instruction->rob_value,NO);
									//cout << "Entro en 3";}
								break; //BNEZ								
								case OP_J: case OP_JR: case OP_JAL:				
									rob_instruction->rob_value = convertStringFloat(convertFloatReg(vj,NO)+convertFloatReg(vk,NO));
									//ref_values->at(rob_instruction->rob_position_rstatus).ref_value = convertFloatReg(rob_instruction->rob_value,NO);
									//cout << "Entro en 4";
								break;
								case OP_ADDIU: case OP_ADDU: case OP_ADDI: 
									rob_instruction->rob_value = convertStringFloat(convertFloatReg(vj,NO)+convertFloatReg(vk,NO));
									//ref_values->at(rob_instruction->rob_position_rstatus).ref_value = convertFloatReg(rob_instruction->rob_value,NO);
									//cout << "Entro en 5";
								break; //ADD//ADDDI//ADD_D
								case OP_SUBU: 								
									rob_instruction->rob_value = convertStringFloat(convertFloatReg(vj,NO)-convertFloatReg(vk,NO));
									//ref_values->at(rob_instruction->rob_position_rstatus).ref_value = convertFloatReg(rob_instruction->rob_value,NO);
								break; //SUB//SUBI//SUB_D
								case OP_DMULT: case OP_MULT:
									rob_instruction->rob_value = convertStringFloat(convertFloatReg(vj,NO)*convertFloatReg(vk,NO));
									//ref_values->at(rob_instruction->rob_position_rstatus).ref_value = convertFloatReg(rob_instruction->rob_value,NO);
								break; //MUL_D
								case OP_DDIV: case OP_DIV:
									rob_instruction->rob_value = vk == "0" ?
									"DIV0" : convertStringFloat(convertFloatReg(vj,NO)/convertFloatReg(vk,NO));
									//ref_values->at(rob_instruction->rob_position_rstatus).ref_value = convertFloatReg(rob_instruction->rob_value,NO);
								break; //DIV_D
								case OP_SLL: case OP_SLLV: case OP_SLT: case OP_SLTI: case OP_SLTIU: case OP_SLTU: case OP_SRA: case OP_SRAV:
								case OP_MFLO:
								case OP_AND: case OP_ANDI: case OP_ORI: case OP_OR:	case OP_XOR:
									rob_instruction->rob_value = convertFloatReg(vj,NO) < convertFloatReg(vk,NO) ? "1" : "0";
									//cout << "Entro en 6";
							}							
							rob_instruction->rob_done = YES;																					
					}		
			}
		else
		{
			if((sta_state == ST_ISSUE) && (qj != NON_VALUE_NUMBER || qk !=NON_VALUE_NUMBER ))
				rob_instruction->rob_state = ST_WAIT_EXECUTE;
			if(sta_state == ST_WAIT_WB) 
			{
				if (number_writeback_cycle < INSTRUCTIONS_PER_CYCLE)
				{
					if ((rob_instruction->rob_operation == OP_SW) || (rob_instruction->rob_operation == OP_SB) || (rob_instruction->rob_operation == OP_SH) || (rob_instruction->rob_operation == OP_SWR))
					{
						if((vj != NON_VALUE) && (rst_values->at(rob_instruction->rob_position_rstatus).rst_busy == NO))
						{
							char mem[5];
							size_t begin = rob_instruction->rob_instruction.find(" ")+1;									
							size_t end = rob_instruction->rob_instruction.find(",");
							rob_instruction->rob_instruction.copy(mem,int(end-1),begin);	
							rob_instruction->rob_state = ST_WRITE_BACK;
							rob_instruction->rob_value = convertString(ref_values->at(atoi(mem)+REGISTERS_FP).ref_value);			
						}
					}
					else
					{
						rob_instruction->rob_state = ST_WRITE_BACK;
					}
				}
			}
			if((sta_state == ST_WRITE_BACK) )//&& (sta_previousState == ST_COMMIT))
			{				
				rs_positions = findRS(res_values,rob_instruction->rob_entry,1);
				for (int k=0;k<rs_positions.size();k++)
				{
					res_instruction->at(rs_positions.at(k)).res_vj = rob_instruction->rob_value;
					res_instruction->at(rs_positions.at(k)).res_qj = NON_VALUE_NUMBER;
				}
				rs_positions = findRS(res_values,rob_instruction->rob_entry,2);
				for (int k=0;k<rs_positions.size();k++)
				{
					res_instruction->at(rs_positions.at(k)).res_vk = rob_instruction->rob_value;
					res_instruction->at(rs_positions.at(k)).res_qk = NON_VALUE_NUMBER;
				}
				
				if ((operation.compare("J") != 0) && (operation.compare("SW") != 0) && (operation.compare("SB") != 0) && (operation.compare("SH") != 0) && (operation.compare("SWR") != 0))
				{
					if (rst_values->at(rob_instruction->rob_position_rstatus).rst_numberROB == rob_instruction->rob_entry)
					{
						rst_values->at(rob_instruction->rob_position_rstatus).rst_busy = NO;				
						rst_values->at(rob_instruction->rob_position_rstatus).rst_numberROB = NON_VALUE_NUMBER;
					}
				}
				res_instruction->at(rob_instruction->rob_position_rstation).res_availability = YES;
				res_instruction->at(rob_instruction->rob_position_rstation).res_destination = NON_VALUE_NUMBER;				
				res_instruction->at(rob_instruction->rob_position_rstation).res_number_cycles_execute = NON_VALUE_NUMBER;
				res_instruction->at(rob_instruction->rob_position_rstation).res_operation_name = "";				
				res_instruction->at(rob_instruction->rob_position_rstation).res_operation = NON_VALUE_NUMBER;
				res_instruction->at(rob_instruction->rob_position_rstation).res_qj = NON_VALUE_NUMBER;
				res_instruction->at(rob_instruction->rob_position_rstation).res_qk = NON_VALUE_NUMBER;
				res_instruction->at(rob_instruction->rob_position_rstation).res_vj = NON_VALUE;
				res_instruction->at(rob_instruction->rob_position_rstation).res_vk = NON_VALUE;
				
				string fu_name = res_instruction->at(rob_instruction->rob_position_rstation).res_functional_unit;
				res_instruction->at(rob_instruction->rob_position_rstation).res_functional_unit = "";
				for (int k=0;k<NUM_FUNCTIONAL_UNITS;k++)
				{
					if (fu_values[k].fu_name.compare(fu_name) == 0)
					{
						fu_values[k].fu_availability = YES;
						break;
					}
				}

				rob_instruction->rob_state = ST_COMMIT;
				rob_instruction->rob_busy = NO;									
			}			
			else
			if(sta_state == ST_COMMIT)
			{								
			}
			else
				if(sta_state == ST_WAIT_EXECUTE && rob_instruction->rob_pipelined == NO && rob_instruction->rob_number_cycles_execute > 0)
					rob_instruction->rob_number_cycles_execute --;
			}
		}
	}
	catch (runtime_error &e)
	{
	cout<<e.what()<< " modifyState " <<endl;
	}
	catch(...)
	{
	cout<<"Unknown error in modifyState"<<endl;
	}
	return YES;
}

	

vector<int> TomasuloFunctions::findRS(std::deque<ReservationStation> rs_Instructions, int entry,int option)
{
	vector<int> rs_positions;
	
		switch(option)
		{
			case 1:
				for(int i = 0; i<rs_Instructions.size();i++)
				{
					if(rs_Instructions.at(i).res_qj == entry)
						rs_positions.push_back(i);
				}
			break;
			case 2:
				for(int i = 0; i<rs_Instructions.size();i++)
				{
					if(rs_Instructions.at(i).res_qk == entry)
						rs_positions.push_back(i);
				}
			break;
		}		
		return rs_positions;
}


int TomasuloFunctions::findROB(std::deque<ROB> rob_Instructions, int entry)
{
	for(int i = 0; i<rob_Instructions.size();i++)
	{
		if(rob_Instructions.at(i).rob_entry == entry)
		return i;
	}
}
