#include "TomasuloComponents.h"
#include "TomasuloFunctions.h"


string convertString1(int int_value)
{
	ostringstream s;
	s << int_value;
	return s.str();
}

string trim(const string &s)
{
  int last = s.size() - 1;
  while (last >= 0 && s[last] == ' ')
    --last;
  return s.substr(0, last + 1);
}


TomasuloComponents::TomasuloComponents()
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
}


TomasuloComponents::~TomasuloComponents(){}

void TomasuloComponents::readFiles(deque<Instruction> *queue_Instruction, deque<RegisterFile> *queue_Register)
{
	char * nameFile = "";
	vector<string> values;
	Instruction ins_values;
	RegisterFile reg_values;
	Flag check = YES;
	int instructions_cont = 0;

	read:
		if(check)
			nameFile = "compressBenchMark.txt";
		else
			nameFile = "RegistersFile.txt";
		
		ifstream infile(nameFile);		
		try
		{
			while (infile.good())
			{
				string str_line = "";
				getline(infile,str_line);
				values = decodeInstruction(str_line,check);
					if(check)
					{
						if (NUM_QUEUE_INST != 0)
						{
							if(instructions_cont == NUM_QUEUE_INST)
							{
								cout<<"Your program has: "<<NUM_QUEUE_INST<<" instructions."<<endl;
								break;
							}
						}
						if(values.size() > 3)
						{
							ins_values = fillInstruction(values);
							ins_values.ins_number_instrucion=queue_Instruction->size()+1;
							string precise_line = trim(str_line);
							precise_line.erase(std::remove(precise_line.begin(), precise_line.end(), '\t'), precise_line.end());
							ins_values.ins_instruction_complete_long = precise_line;
							ins_values.ins_instruction_complete_short = precise_line.substr(18,str_line.length()-18);							
							queue_Instruction->push_back(ins_values);
							instructions_cont++;
						}
					}
					else
					{
						reg_values = fillRegister(values);
						queue_Register->push_back(reg_values);
					}					
			}
		}
		catch (runtime_error &e)
		{
			cout<<e.what()<< " readFile " <<endl;
		}
		catch(...)
		{
			cout<<"Unknown error in readFile"<<endl;
		}

	if(check)
	{
		check = NO;
		goto read;
	}
}


vector<string> TomasuloComponents::decodeInstruction(string newData, Flag fla_check)
{
	string item="";
	vector<string> values;
	int i=0;
	try
	{
		if(!fla_check)
		{
			stringstream ss(newData/*+' '*/);
			while(getline(ss, item, ' '))
			{
				values.push_back(item);
			}
		}
		else
		{
			stringstream ss(newData/*+' '*/);
			getline(ss, item, ' ');
			getline(ss, item, ' ');
			values.push_back(item);
			getline(ss, item, ' ');
			values.push_back(item);			
			getline(ss, item, ' ');
			item.erase(std::remove_if(item.begin(), item.end(), isspace), item.end());
			values.push_back(item);
			if((item.compare("NOP")!= 0) && (item.compare("SYSCALL")!= 0))
			{
				if((item.compare("JAL")!= 0) && (item.compare("J")!= 0))
				{
					getline(ss, item, ',');
					item.erase(std::remove_if(item.begin(), item.end(), isspace), item.end());
					values.push_back(item);
					getline(ss, item, ',');
					item.erase(std::remove_if(item.begin(), item.end(), isspace), item.end());
					values.push_back(item);
					getline(ss, item, ',');
					item.erase(std::remove_if(item.begin(), item.end(), isspace), item.end());
					values.push_back(item);
				}
				else 
				{
					getline(ss, item, ',');
					item.erase(std::remove_if(item.begin(), item.end(), isspace), item.end());			
					values.push_back(item);		
					values.push_back("31");		
					values.push_back("4");					
				}		
			}
		}
	}
	catch (runtime_error &e)
	{
		cout<<e.what()<< " split " <<endl;
	}
	catch(...)
	{
		cout<<"Unknown error in split"<<endl;
	}
	return values;
}


Instruction TomasuloComponents::fillInstruction(vector<string> ins_Instruction)
{
	Instruction result;	
	try
	{
		result.ins_address = ins_Instruction.at(0);
		result.ins_pipelined = YES;
		if((ins_Instruction.at(2).compare("JAL")!= 0) && (ins_Instruction.at(2).compare("J")!= 0) && (ins_Instruction.at(2).compare("SW") != 0) && (ins_Instruction.at(2).compare("SB") != 0) && (ins_Instruction.at(2).compare("SH") != 0) && (ins_Instruction.at(2).compare("SWR") != 0) && (ins_Instruction.at(2).compare("BNE") != 0) && (ins_Instruction.at(2).compare("BEQ") != 0) && (ins_Instruction.at(2).compare("BGEZ") != 0) && (ins_Instruction.at(2).compare("BGTZ") != 0) && (ins_Instruction.at(2).compare("BLEZ") != 0) && (ins_Instruction.at(2).compare("BLTZ") != 0))
		{
				result.ins_result = ins_Instruction.at(3);
				result.ins_operand1 = ins_Instruction.at(4);
				result.ins_operand2 = ins_Instruction.at(5);															
		}
		else if((ins_Instruction.at(2).compare("SW")== 0) || (ins_Instruction.at(2).compare("SB") == 0) || (ins_Instruction.at(2).compare("SH") == 0) || (ins_Instruction.at(2).compare("SWR") == 0))
		{			
				result.ins_result = ins_Instruction.at(5);
				result.ins_operand1 = ins_Instruction.at(3);
				result.ins_operand2 = ins_Instruction.at(4);
			
		}
		else if ((ins_Instruction.at(2).compare("BNE")== 0) || (ins_Instruction.at(2).compare("BEQ") == 0) || (ins_Instruction.at(2).compare("BGEZ") == 0) || (ins_Instruction.at(2).compare("BGTZ") == 0) || (ins_Instruction.at(2).compare("BLEZ") == 0) || (ins_Instruction.at(2).compare("BLTZ") == 0))
		{			
				result.ins_result = ins_Instruction.at(3);
				result.ins_operand1 = ins_Instruction.at(4);
				result.ins_operand2 = ins_Instruction.at(5);
			
		}
		else if(ins_Instruction.at(2).compare("JAL") == 0) 
		{
			result.ins_result = ins_Instruction.at(4);
			result.ins_operand1 = ins_Instruction.at(3);
			result.ins_operand2 = ins_Instruction.at(5);
		}
		else if(ins_Instruction.at(2).compare("J") == 0)
		{
			result.ins_result = ins_Instruction.at(3);
			result.ins_operand1 = ins_Instruction.at(3);
			result.ins_operand2 = ins_Instruction.at(5);
		}

		result.ins_pipelined = YES;
		if((ins_Instruction.at(2).compare("DADD") == 0) || (ins_Instruction.at(2).compare("DSUB") == 0))
			result.ins_number_cycles_execute = NUM_CYCLES_FP_ADDD_SUBD;		
		else if (ins_Instruction.at(2).compare("DMULT") == 0)
			result.ins_number_cycles_execute = NUM_CYCLES_FP_MULTD;		
		else if (ins_Instruction.at(2).compare("DDIV") == 0)
			result.ins_number_cycles_execute = NUM_CYCLES_FP_DIVD;
		else if ((ins_Instruction.at(2).compare("ADDIU") == 0) || (ins_Instruction.at(2).compare("ADDU") == 0) || (ins_Instruction.at(2).compare("ADDI") == 0) || (ins_Instruction.at(2).compare("SUBU") == 0) || (ins_Instruction.at(2).compare("DIV") == 0) || (ins_Instruction.at(2).compare("MULT") == 0))
			result.ins_number_cycles_execute = NUM_CYCLES_INT;
		else if ((ins_Instruction.at(2).compare("SLL") == 0) || (ins_Instruction.at(2).compare("SLLV") == 0) || (ins_Instruction.at(2).compare("SLT") == 0) || (ins_Instruction.at(2).compare("SLTI") == 0) || (ins_Instruction.at(2).compare("SLTIU") == 0) || (ins_Instruction.at(2).compare("SLTU") == 0) || (ins_Instruction.at(2).compare("SRA") == 0) || (ins_Instruction.at(2).compare("SRAV") == 0))
			result.ins_number_cycles_execute = NUM_CYCLES_SLTI;
		else if (ins_Instruction.at(2).compare("MFLO") == 0)
			result.ins_number_cycles_execute = NUM_CYCLES_INT;
		else if ((ins_Instruction.at(2).compare("AND") == 0) || (ins_Instruction.at(2).compare("ANDI") == 0) || (ins_Instruction.at(2).compare("ORI") == 0) || (ins_Instruction.at(2).compare("OR") == 0) || (ins_Instruction.at(2).compare("XOR") == 0))
			result.ins_number_cycles_execute = NUM_CYCLES_INT;
		else if ((ins_Instruction.at(2).compare("BNE") == 0) || (ins_Instruction.at(2).compare("BEQ") == 0) || (ins_Instruction.at(2).compare("BGEZ") == 0) || (ins_Instruction.at(2).compare("BGTZ") == 0) || (ins_Instruction.at(2).compare("BLEZ") == 0) || (ins_Instruction.at(2).compare("BLTZ") == 0))
			result.ins_number_cycles_execute = NUM_CYCLES_BNE;	   
		else if ((ins_Instruction.at(2).compare("J") == 0) || (ins_Instruction.at(2).compare("JR") == 0) || (ins_Instruction.at(2).compare("JAL") == 0))
			result.ins_number_cycles_execute = NUM_CYCLES_BNE;	   
		else if ((ins_Instruction.at(2).compare("LW") == 0) || (ins_Instruction.at(2).compare("LB") == 0) || (ins_Instruction.at(2).compare("LBU") == 0) || (ins_Instruction.at(2).compare("LH") == 0) || (ins_Instruction.at(2).compare("LWR") == 0) || (ins_Instruction.at(2).compare("LWL") == 0) || (ins_Instruction.at(2).compare("LUI") == 0))
			result.ins_number_cycles_execute = NUM_CYCLES_LD;	   
		else 
			result.ins_number_cycles_execute = NUM_CYCLES_SD;	   

		if (ins_Instruction.at(2).compare("LW") == 0)result.ins_operation = OP_LW;
		else if (ins_Instruction.at(2).compare("LB") == 0)result.ins_operation = OP_LB;
		else if (ins_Instruction.at(2).compare("LBU") == 0)result.ins_operation = OP_LBU;
		else if (ins_Instruction.at(2).compare("LH") == 0)result.ins_operation = OP_LH;
		else if (ins_Instruction.at(2).compare("LWR") == 0)result.ins_operation = OP_LWR;
		else if (ins_Instruction.at(2).compare("LWL") == 0)result.ins_operation = OP_LWL;
		else if (ins_Instruction.at(2).compare("LUI") == 0)result.ins_operation = OP_LUI;
		else if (ins_Instruction.at(2).compare("SW") == 0)result.ins_operation = OP_SW;
		else if (ins_Instruction.at(2).compare("SB") == 0)result.ins_operation = OP_SB;
		else if (ins_Instruction.at(2).compare("SH") == 0)result.ins_operation = OP_SH;
		else if (ins_Instruction.at(2).compare("SWR") == 0)result.ins_operation = OP_SWR;
		else if (ins_Instruction.at(2).compare("ADDIU") == 0)result.ins_operation = OP_ADDIU;
		else if (ins_Instruction.at(2).compare("ADDU") == 0)result.ins_operation = OP_ADDU;
		else if (ins_Instruction.at(2).compare("ADDI") == 0)result.ins_operation = OP_ADDI;
		else if (ins_Instruction.at(2).compare("SUBU") == 0)result.ins_operation = OP_SUBU;
		else if (ins_Instruction.at(2).compare("DIV") == 0)result.ins_operation = OP_DIV;
		else if (ins_Instruction.at(2).compare("MULT") == 0)result.ins_operation = OP_MULT;
		else if (ins_Instruction.at(2).compare("DADD") == 0)result.ins_operation = OP_DADD;
		else if (ins_Instruction.at(2).compare("DSUB") == 0)result.ins_operation = OP_DSUB;
		else if (ins_Instruction.at(2).compare("DDIV") == 0)result.ins_operation = OP_DDIV;
		else if (ins_Instruction.at(2).compare("DMULT") == 0)result.ins_operation = OP_DMULT;
		else if (ins_Instruction.at(2).compare("SLL") == 0)result.ins_operation = OP_SLL;
		else if (ins_Instruction.at(2).compare("SLLV") == 0)result.ins_operation = OP_SLLV;
		else if (ins_Instruction.at(2).compare("SLT") == 0)result.ins_operation = OP_SLTI;
		else if (ins_Instruction.at(2).compare("SLTI") == 0)result.ins_operation = OP_SLTI;
		else if (ins_Instruction.at(2).compare("SLTIU") == 0)result.ins_operation = OP_SLTIU;
		else if (ins_Instruction.at(2).compare("SLTU") == 0)result.ins_operation = OP_SLTU;
		else if (ins_Instruction.at(2).compare("SRA") == 0)result.ins_operation = OP_SRA;
		else if (ins_Instruction.at(2).compare("SRAV") == 0)result.ins_operation = OP_SRAV;
		else if (ins_Instruction.at(2).compare("MFLO") == 0)result.ins_operation = OP_MFLO;
		else if (ins_Instruction.at(2).compare("AND") == 0)result.ins_operation = OP_AND;
		else if (ins_Instruction.at(2).compare("ANDI") == 0)result.ins_operation = OP_ANDI;
		else if (ins_Instruction.at(2).compare("ORI") == 0)result.ins_operation = OP_ORI;
		else if (ins_Instruction.at(2).compare("OR") == 0)result.ins_operation = OP_OR;		
		else if (ins_Instruction.at(2).compare("XOR") == 0)result.ins_operation = OP_XOR;		
		else if (ins_Instruction.at(2).compare("BNE") == 0)result.ins_operation = OP_BNE;
		else if (ins_Instruction.at(2).compare("BEQ") == 0)result.ins_operation = OP_BEQ;
		else if (ins_Instruction.at(2).compare("BGEZ") == 0)result.ins_operation = OP_BGEZ;
		else if (ins_Instruction.at(2).compare("BGTZ") == 0)result.ins_operation = OP_BGTZ;
		else if (ins_Instruction.at(2).compare("BLEZ") == 0)result.ins_operation = OP_BLEZ;
		else if (ins_Instruction.at(2).compare("BLTZ") == 0)result.ins_operation = OP_BLTZ;
		else if (ins_Instruction.at(2).compare("J") == 0)result.ins_operation = OP_J;
		else if (ins_Instruction.at(2).compare("JR") == 0)result.ins_operation = OP_JR;		
		else result.ins_operation = OP_JAL;		
	}
	catch (runtime_error &e)
	{
		cout<<e.what()<< " fillInstruction " <<endl;
	}
	catch(...)
	{
		cout<<"Unknown error in fillInstruction"<<endl;
	}
	return result;
}


RegisterFile TomasuloComponents::fillRegister(vector<string> reg_Register)
{
	RegisterFile values;
	values.ref_register = reg_Register.at(0);
	values.ref_value = atoi(reg_Register.at(1).c_str());
	return values;
}


void TomasuloComponents::printFile(deque<Instruction> ins_instruction, deque<RegisterFile> reg_register, Flag fla_printAll)
{
	Instruction ins_values;
	RegisterFile reg_values;
	bool band = false;
	cout<<endl;
	cout<<string(55,'-')<<endl<<setw(32)<<"INSTRUCTIONS"<<endl<<string(55,'-')<<endl;
	try
	{
		for (int i = 0; i < ins_instruction.size(); i++)
		{
			ins_values = ins_instruction.at(i);
			cout<<left<<ins_values.ins_instruction_complete_long<<endl;
		}
		cout<<endl;
		if(fla_printAll)
		{
			cout<<string(55,'-')<<endl<<left<<setw(36)<<"REGISTERS"<<endl<<string(55,'-')<<endl;
			for (int i = 0; i < reg_register.size(); i++)
			{
				reg_values = reg_register.at(i);	
				if((reg_values.ref_register.substr(0,1).compare ("F")!=0)&&(band == false))
				{	
					cout<<endl<<endl;
					band = true;					
				}
				cout<<setw(4)<<reg_values.ref_register<<setw(8)<<reg_values.ref_value;
				if(reg_values.ref_register.substr(0,1).compare ("F")==0)
				{
					if((i+1)%5 == 0 && i != 0)
						cout<<endl;
				}
				else
				{
					if((i-1)%5 == 0 && i != 0)
						cout<<endl;
				}
			}
		}
	}
	catch (runtime_error &e)
	{
		cout<<e.what()<< " printFile " <<endl;
	}
	catch(...)
	{
		cout<<"Unknown error in printFile"<<endl;
	}
}


void TomasuloComponents::printROB(deque<ROB> rob_instruction,int Cycles)
{
	ROB rob_values;
	int i;
	string valueMem="";
	try
	{
		for (i = 0; i < rob_instruction.size(); i++)
		{
			rob_values = rob_instruction.at(i);
			if((rob_values.rob_operation == OP_SW) || (rob_values.rob_operation == OP_SB) || (rob_values.rob_operation == OP_SH) || (rob_values.rob_operation ==OP_SWR))  
			{
				char mem[10],mem1[10];
				int value=0,value1=0;
				size_t begin = rob_values.rob_instruction.find(",")+2;
				size_t end = rob_values.rob_instruction.rfind(",")+2;
				rob_values.rob_instruction.copy(mem,int(end-1),begin);
				rob_values.rob_instruction.copy(mem1,int(rob_values.rob_instruction.length()-end),end);
				value = atoi(mem);
				value1 = atoi(mem1);
				valueMem = "MEM[" + convertString1(value) + "+" + convertString1(value1) + "]";
			}
			if((rob_values.rob_operation == OP_J) || (rob_values.rob_operation == OP_BNE) || (rob_values.rob_operation == OP_BEQ) || (rob_values.rob_operation == OP_BGEZ) || (rob_values.rob_operation ==OP_BLEZ) || (rob_values.rob_operation ==OP_BLTZ))  
			{
				valueMem = "PC0";
			}
			
			cout<<setw(7)<<rob_values.rob_number_cycles_execute<<setw(6)<<rob_values.rob_entry<<setw(5)<<busy[rob_values.rob_busy]
            <<setw(20)<<rob_values.rob_instruction<<setw(13)<<state[rob_values.rob_state]
            <<setw(14)<<(((rob_values.rob_operation == OP_SW) || (rob_values.rob_operation == OP_SB) || (rob_values.rob_operation == OP_SH) || (rob_values.rob_operation ==OP_SWR) ||
			(rob_values.rob_operation == OP_J) || 
			(rob_values.rob_operation == OP_BNE) || (rob_values.rob_operation == OP_BEQ) || (rob_values.rob_operation == OP_BGEZ) || (rob_values.rob_operation ==OP_BLEZ) || (rob_values.rob_operation ==OP_BLTZ)) ? valueMem : rob_values.rob_destination)
            <<setw(8)<<rob_values.rob_value<<busy[rob_values.rob_done]<<endl;
		}
	}
	catch (runtime_error &e)
	{
	cout<<e.what()<< " printROB " <<endl;
	}
	catch(...)
	{
	cout<<"Unknown error in printROB"<<endl;
	}
	cout<<endl<<endl<<"CYCLE: "<<Cycles<<endl;
}


void TomasuloComponents::printRS(deque<ReservationStation> res_instruction)
{
	ReservationStation res_values;
	int i, size = res_instruction.size();
	try
	{
		for (i = 0; i < size; i++)
		{
			res_values = res_instruction.at(i);
			if(!res_values.res_availability)
				cout<<setw(6)<<left<<busy[res_values.res_busy]<<setw(10)<<res_values.res_functional_unit<<setw(9)<<operation[res_values.res_operation]<<setw(7)<<(res_values.res_vj != NON_VALUE ?
				res_values.res_vj:NON_VALUE)
				<<setw(7)<<(res_values.res_vk != NON_VALUE && res_values.res_operation != OP_SW && res_values.res_operation != OP_SLTIU ? res_values.res_vk:NON_VALUE)
				<<setw(10)<<(res_values.res_qj != NON_VALUE_NUMBER?"ROB"+convertString1(res_values.res_qj):"")
				<<setw(10)<<(res_values.res_qk != NON_VALUE_NUMBER?"ROB"+convertString1(res_values.res_qk):"")<<"ROB"<<res_values.res_destination<<endl;
		}
	}
	catch (runtime_error &e)
	{
		cout<<e.what()<< " printRS " <<endl;
	}
	catch(...)
	{
		cout<<"Unknown error in printRS"<<endl;
}
}


void TomasuloComponents::printRegStatus(deque<RegisterStatus> rst_instruction)
{
	RegisterStatus rst_values;
	int i=0, size = rst_instruction.size()-(REGISTERS_FP+(REGISTERS_INT/2)),limit=(REGISTERS_INT/2)-1;
	int flag = 0;
	int rowPrint=0;
	
	cout<<endl<<endl;
	cout<<string(75,'-')<<endl<<setw(32)<<"FP REGISTERS STATUS"<<endl<<string(75,'-')<<endl;
	back:
		try
		{
			for (; i < size; i++)
			{
				rst_values = rst_instruction.at(i);
				switch(rowPrint)
				{
					case 0: 
						if(i==0 || i==REGISTERS_INT/2 || i==REGISTERS_INT)
						{
							cout<<setw(10)<<left<<"FIELD"<< setw(4) <<
							rst_values.rst_field;
						}
						else
							cout<< setw(4) << rst_values.rst_field;
						if(i == limit) 
						{
							i = flag == 0 ? -1 : flag == 1 ? (REGISTERS_INT/2)-1 : flag == 2 ? (REGISTERS_INT)-1 : 0;
							rowPrint=1;
							cout<<endl;
						}						
					break;	
					case 1: 
						if(i==0 || i==REGISTERS_INT/2 || i==REGISTERS_INT)
							cout<<setw(6)<<left<<"ROB"<< setw(8) <<(rst_values.rst_numberROB == NON_VALUE_NUMBER ? "" : convertString1(rst_values.rst_numberROB));
						else
							cout<< setw(4) << (rst_values.rst_numberROB ==NON_VALUE_NUMBER ? "" : convertString1(rst_values.rst_numberROB));
						if(i == limit) 
						{
							i = flag == 0 ? -1 : flag == 1 ? (REGISTERS_INT/2)-1 : flag == 2 ? (REGISTERS_INT)-1 : 0;
							rowPrint=2;
							cout<<endl;
						}							
					break;
					case 2: 
						if(i==0 || i==REGISTERS_INT/2 || i==REGISTERS_INT)
							cout<<setw(10)<<left<<"BUSY"<< setw(4) <<(rst_values.rst_busy == YES ? busy[rst_values.rst_busy] : "");
						else
						{
							cout<< setw(4) << (rst_values.rst_busy == YES ? busy[rst_values.rst_busy] : "");
						}
					break;
				}
			}
			cout<<endl<<endl;
			flag = flag +1;
			if(flag == 1)
			{				
				cout<<endl;
				cout<<string(75,'-')<<endl<<setw(32)<<"INTEGER REGISTERS STATUS"<<endl<<string(75,'-')<<endl;
				size = REGISTERS_FP+REGISTERS_INT/2;
				rowPrint = 0;
				limit = REGISTERS_FP+(REGISTERS_INT/2)-1;
				goto back;
			}
			else if (flag == 2)
			{
				cout<<endl;
				size = REGISTERS_FP+REGISTERS_INT;
				rowPrint = 0;
				limit = REGISTERS_FP+(REGISTERS_INT)-1;
				goto back;
			}
		}
		catch (runtime_error &e)
		{
			cout<<e.what()<< " printRegStatus " <<endl;
		}
			catch(...)
		{
			cout<<"Unknown error in printRegStatus"<<endl;
		}
}