#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <fstream>
#include <stdbool.h>
#include <exception>
#include <math.h>
#include <string.h>
#include <iostream>
#include <boost/tokenizer.hpp>
using namespace std;

bool compare(string s1,string s2){
    if(s1==s2){
        return true;
    }
    else{
        return false;
    }
}
struct MIPS_Architecture
{
	int registers[32] = {0}, PCcurr = 0, PCnext;
	std::unordered_map<std::string, std::function<int(MIPS_Architecture &, std::string, std::string, std::string)>> instructions;
	std::unordered_map<std::string, int> registerMap, address;
	static const int MAX = (1 << 20);
	int data[MAX >> 2] = {0};
	
    
	std::vector<std::vector<std::string>> commands;
	std::vector<int> commandCount;
	enum exit_code
	{
		SUCCESS = 0,
		INVALID_REGISTER,
		INVALID_LABEL,
		INVALID_ADDRESS,
		SYNTAX_ERROR,
		MEMORY_ERROR
	};

	// constructor to initialise the instruction set
	MIPS_Architecture(std::ifstream &file)
	{
		// instructions = {{"add", &MIPS_Architecture::add}, {"sub", &MIPS_Architecture::sub}, {"mul", &MIPS_Architecture::mul}, {"beq", &MIPS_Architecture::beq}, {"bne", &MIPS_Architecture::bne}, {"slt", &MIPS_Architecture::slt}, {"j", &MIPS_Architecture::j}, {"lw", &MIPS_Architecture::lw}, {"sw", &MIPS_Architecture::sw}, {"addi", &MIPS_Architecture::addi}};

		for (int i = 0; i < 32; ++i)
			registerMap["$" + std::to_string(i)] = i;
		registerMap["$zero"] = 0;
		registerMap["$at"] = 1;
		registerMap["$v0"] = 2;
		registerMap["$v1"] = 3;
		for (int i = 0; i < 4; ++i)
			registerMap["$a" + std::to_string(i)] = i + 4;
		for (int i = 0; i < 8; ++i)
			registerMap["$t" + std::to_string(i)] = i + 8, registerMap["$s" + std::to_string(i)] = i + 16;
		registerMap["$t8"] = 24;
		registerMap["$t9"] = 25;
		registerMap["$k0"] = 26;
		registerMap["$k1"] = 27;
		registerMap["$gp"] = 28;
		registerMap["$sp"] = 29;
		registerMap["$s8"] = 30;
		registerMap["$ra"] = 31;

		constructCommands(file);
		commandCount.assign(commands.size(), 0);
	};
    void printRegisters(int clockCycle)
			{
				// std::cout << "Cycle number: " << clockCycle << '\n';
						//   << std::hex;
				for (int i = 0; i < 32; ++i)
					std::cout << registers[i] << ' ';
                    
				std::cout << std::dec << '\n';
                // for (int i = 0; i < 32 ; ++i){
                //     std::cout << "  " <<data[i] ;}                
                // std::cout << std::dec << '\n';}
            }
    void parseCommand(std::string line)
	{
		// strip until before the comment begins
		line = line.substr(0, line.find('#'));
		std::vector<std::string> command;
		boost::tokenizer<boost::char_separator<char>> tokens(line, boost::char_separator<char>(", \t"));
		for (auto &s : tokens)
			command.push_back(s);
		// empty line or a comment only line
		if (command.empty())
			return;
		else if (command.size() == 1)
		{
			std::string label = command[0].back() == ':' ? command[0].substr(0, command[0].size() - 1) : "?";
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command.clear();
		}
		else if (command[0].back() == ':')
		{
			std::string label = command[0].substr(0, command[0].size() - 1);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command = std::vector<std::string>(command.begin() + 1, command.end());
		}
		else if (command[0].find(':') != std::string::npos)
		{
			int idx = command[0].find(':');
			std::string label = command[0].substr(0, idx);
			if (address.find(label) == address.end())
				address[label] = commands.size();
			else
				address[label] = -1;
			command[0] = command[0].substr(idx + 1);
		}
		else if (command[1][0] == ':')
		{
			if (address.find(command[0]) == address.end())
				address[command[0]] = commands.size();
			else
				address[command[0]] = -1;
			command[1] = command[1].substr(1);
			if (command[1] == "")
				command.erase(command.begin(), command.begin() + 2);
			else
				command.erase(command.begin(), command.begin() + 1);
		}
		if (command.empty())
			return;
		if (command.size() > 4)
			for (int i = 4; i < (int)command.size(); ++i)
				command[3] += " " + command[i];
		command.resize(4);
		commands.push_back(command);		
	};
    void constructCommands(std::ifstream &file)
	{
		std::string line;
		while (getline(file, line))
			parseCommand(line);
		file.close();
	};
};


struct IF_1{
    int PC;
    bool PCSrc;
    int PC_plus_4;
    int branch_PC;
    int nop;
};

struct IF_2{
    vector<string> Instr;
    int nop;
};

struct ID_1{
    vector<string> Instr;
    string rs;
    string rd;
    string rt;
    int nop;
};

struct ID_2{
    vector<string> Instr;
    int nop;
    string rs;
    string rt;
    string rd;   
};

struct RR{
    int format;
    vector<string> Instr;
    string op;
    string rd;
    string rt;
    string rs;
    string dest_r;
    int read_1;
    int read_2;
    int imm;
    bool RegDst; 
    int ALU_op;
    bool ALU_Src; //Identifies I/R type
    bool Branch;
    bool Mem_Read;
    bool Mem_Write;
    bool Reg_Read;
    bool MemtoReg;
    int nop;
};

struct EX{
    string op;
    int format;
    int read_1;
    int read_2;
    int imm;
    string rd;
    string rt;
    string rs;
    bool RegDst; 
    int ALU_op;
    bool ALU_Src; //Identifies I/R type
    bool Branch;
    bool Mem_Read;
    bool Mem_Write;
    bool Reg_Read;
    bool MemtoReg;
    int nop;

    int ALU_Control(){
        if(compare(op,"add")){return 0;}
        else if(op=="sub"){return 1;}
        else if(op=="addi" ||compare(op,"lw")){return 2;}
        else if(op=="sll"){return 3;}
        else if(op=="srl"){return 4;}
        else if(op=="and"){return 5;}
        else if(op=="or"){return 6;}
        else if(op=="slt"){return 7;} //lw,sw missing
        else if(op=="sw"){return 9;}
        else if(op=="mul"){return 10;}
    }

    int operation(){
        if(ALU_op ==0){ return (read_1+read_2); }
        else if( ALU_op==1){return (read_1-read_2);}
        else if( ALU_op==2){return (read_1+imm);}
        else if( ALU_op==3){return (read_1*pow(2,imm));}
        else if( ALU_op==4){return (int(read_1/pow(2,imm)));}
        else if( ALU_op==5){return (read_1 & read_2);}
        else if( ALU_op==6){return (read_1 | read_2);}
        else if( ALU_op==7){return (read_1<read_2);}
        else if( ALU_op==9){return (read_2+imm);}
        else if( ALU_op==10){return (read_1*read_2);}
    }
};


struct MEM_1{
    int format;
    int ALU_result;
    int store_data;
    string dest_r;
    string rt;
    string rs;
    string rd;
    bool RegDst; 
    bool Mem_Read;
    bool Mem_Write;
    bool Reg_Read;
    bool MemtoReg;
    int nop;
};

struct MEM_2{
    int format;
    int ALU_result;
    int store_data;
    string dest_r;
    string rs;
    string rt;
    string rd;
    bool RegDst; 
    bool Mem_Read;
    bool Mem_Write;
    bool Reg_Read;
    bool MemtoReg;
    int nop;
};

struct WB{
    string dest_r;
    int format;
    int R_result;
    int I_result; //load
    bool RegDst; 
    bool ALU_Src; //Identifies I/R type
    bool Branch;
    bool Mem_Read;
    bool Mem_Write;
    bool Reg_Read;
    bool MemtoReg;
    int nop;
};

struct BUFFER{
    int format;
    int I_Result;
    int R_Result;
    string dest_r;
    bool Reg_Read;
};


struct state{
    IF_1 IF_1_Stage;
    IF_2 IF_2_Stage;
    ID_1 ID_1_Stage;
    ID_2 ID_2_Stage;
    RR RR_Stage;
    EX EX_Stage;
    MEM_1 MEM_1_Stage;
    MEM_2 MEM_2_Stage;
    WB WB_Stage;
    BUFFER BF_Stage;
};

int main(int argc, char *argv[])
{
    // cout << "starting";
    // std::cerr << "main() called\n";
	if (argc != 2)
	{
		std::cerr << "Required argument: file_name\n./MIPS_interpreter <file name>\n";
		return 0;
	}
	std::ifstream file(argv[1]);
	MIPS_Architecture *mips;
    // mips->data[8] = 126;
    state default_state,new_state;
    // std::cerr << "states created\n";
    

	if (file.is_open())
		mips = new MIPS_Architecture(file);
	else
	{
		std::cerr << "File could not be opened. Terminating...\n";
		return 0;
	}

    default_state.IF_1_Stage.PC=0;
    default_state.IF_1_Stage.nop=0;
    default_state.IF_1_Stage.branch_PC=0;
    default_state.IF_1_Stage.PC_plus_4=0;
    default_state.IF_1_Stage.PCSrc=false;

    default_state.IF_2_Stage.Instr;
    default_state.IF_2_Stage.nop=1;

    default_state.ID_1_Stage.Instr;
    default_state.ID_1_Stage.nop=1;
    default_state.ID_1_Stage.rs = "";
    default_state.ID_1_Stage.rt = "";
    default_state.ID_1_Stage.rd = "";

    default_state.ID_2_Stage.Instr;
    default_state.ID_2_Stage.nop=1;
    default_state.ID_2_Stage.rs = "";
    default_state.ID_2_Stage.rt = "";
    default_state.ID_2_Stage.rd = "";



    default_state.RR_Stage.format=0;
    default_state.RR_Stage.op="";
    default_state.RR_Stage.rd="";
    default_state.RR_Stage.rt="";
    default_state.RR_Stage.rs="";
    default_state.RR_Stage.dest_r="";
    default_state.RR_Stage.imm=0;
    default_state.RR_Stage.ALU_Src=false;
    default_state.RR_Stage.RegDst=false;
    default_state.RR_Stage.Branch=false;
    default_state.RR_Stage.Mem_Read=false;
    default_state.RR_Stage.Mem_Write=false;
    default_state.RR_Stage.Reg_Read=false;
    default_state.RR_Stage.MemtoReg=false;
    default_state.RR_Stage.nop=1;

    default_state.EX_Stage.op="";
    default_state.EX_Stage.format=0;
    default_state.EX_Stage.read_1=0;
    default_state.EX_Stage.read_2=0;
    default_state.EX_Stage.imm=0;
    default_state.EX_Stage.rs = "";
    default_state.EX_Stage.rd="";
    default_state.EX_Stage.rt="";
    default_state.EX_Stage.RegDst=false;
    default_state.EX_Stage.ALU_op=8;
    default_state.EX_Stage.ALU_Src=false;
    default_state.EX_Stage.Branch=false;
    default_state.EX_Stage.Mem_Read=false;
    default_state.EX_Stage.Mem_Write=false;
    default_state.EX_Stage.Reg_Read=false;
    default_state.EX_Stage.MemtoReg=false;
    default_state.EX_Stage.nop=1;

    default_state.MEM_1_Stage.format=0;
    default_state.MEM_1_Stage.store_data=0;
    default_state.MEM_1_Stage.ALU_result=0;
    default_state.MEM_1_Stage.dest_r="";
    default_state.MEM_1_Stage.rd="";
    default_state.MEM_1_Stage.rs="";
    default_state.MEM_1_Stage.rt="";
    default_state.MEM_1_Stage.RegDst=false;
    default_state.MEM_1_Stage.Reg_Read=false;
    default_state.MEM_1_Stage.Mem_Read=false;
    default_state.MEM_1_Stage.Mem_Write=false;
    default_state.MEM_1_Stage.MemtoReg=false;
    default_state.MEM_1_Stage.nop=1;

    default_state.MEM_2_Stage.format=0;
    default_state.MEM_2_Stage.store_data=0;
    default_state.MEM_2_Stage.ALU_result=0;
    default_state.MEM_2_Stage.dest_r="";
    default_state.MEM_2_Stage.rd="";
    default_state.MEM_2_Stage.rs="";
    default_state.MEM_2_Stage.rt="";
    default_state.MEM_2_Stage.RegDst=false;
    default_state.MEM_2_Stage.Reg_Read=false;
    default_state.MEM_2_Stage.Mem_Read=false;
    default_state.MEM_2_Stage.Mem_Write=false;
    default_state.MEM_2_Stage.MemtoReg=false;
    default_state.MEM_2_Stage.nop=1;

    default_state.WB_Stage.dest_r="";
    default_state.WB_Stage.format=0;
    default_state.WB_Stage.R_result=0;
    default_state.WB_Stage.I_result=0;
    default_state.WB_Stage.RegDst=false;
    default_state.WB_Stage.ALU_Src=false;
    default_state.WB_Stage.Branch=false;
    default_state.WB_Stage.Mem_Read=false;
    default_state.WB_Stage.Mem_Write=false;
    default_state.WB_Stage.Reg_Read=false;
    default_state.WB_Stage.MemtoReg=false;
    default_state.WB_Stage.nop=true;

    default_state.BF_Stage.dest_r ="";
    default_state.BF_Stage.format = 0;
    default_state.BF_Stage.Reg_Read = false;
    default_state.BF_Stage.I_Result =0;
    default_state.BF_Stage.R_Result =0 ;

    int cycle=0;
    int stall_marker_1=0;
    int stall_marker_2=0;
    int stall_marker_3=0;
    int stall_marker_79 = 0;
    int stall_forward = 0;
    int forward_write = 0;
    int completed = -1;
    bool set_complete = false;
    bool stall = false;
    bool wait = false;
    bool branch = false;
    bool bne_branch = false;
    bool set_IF_nop = false;
    bool jump = false;
    bool stall79 = false;
    bool branch_entry = false;

    // while(stall==true || stall79 == true ||(default_state.IF_1_Stage.nop==0) || (default_state.IF_2_Stage.nop==0) || (default_state.ID_1_Stage.nop==0)  || (default_state.ID_2_Stage.nop==0) || (default_state.RR_Stage.nop==0) || (default_state.EX_Stage.nop==0) || (default_state.MEM_1_Stage.nop==0) || (default_state.MEM_2_Stage.nop==0) || (default_state.WB_Stage.nop==0)){
    while(true){
        mips->printRegisters(cycle);
    //-----------------------------------------WB Stage-----------------------------------------------------------------------
        if(default_state.WB_Stage.nop==0){
            if(default_state.WB_Stage.Reg_Read==true){
                int write_data;
                if(default_state.WB_Stage.MemtoReg==false){
                    if(default_state.WB_Stage.format == 1){
                    write_data = default_state.WB_Stage.R_result;}
                    else{write_data = default_state.WB_Stage.I_result;}
                    mips->registers[mips->registerMap[default_state.WB_Stage.dest_r]] =  write_data;
                }
                else{ int write_data = default_state.WB_Stage.I_result;
                mips->registers[mips->registerMap[default_state.WB_Stage.dest_r]] =  write_data;}
                
            }
            new_state.BF_Stage.dest_r = default_state.WB_Stage.dest_r;
            new_state.BF_Stage.format = default_state.WB_Stage.format;
            new_state.BF_Stage.I_Result = default_state.WB_Stage.I_result;
            new_state.BF_Stage.R_Result = default_state.WB_Stage.R_result;
            new_state.BF_Stage.Reg_Read = default_state.WB_Stage.Reg_Read;
      
        }
    //----------------------------------------MEM_2 Stage---------------------------------------------------------------------

        new_state.WB_Stage.nop = default_state.MEM_2_Stage.nop; 
        if(default_state.MEM_2_Stage.nop ==1){
        cout << "0 " << endl;
        // cout << " " << endl;
        }
        if(default_state.MEM_2_Stage.nop==0){
            if(default_state.MEM_2_Stage.Mem_Write){
                mips->data[default_state.MEM_2_Stage.ALU_result]=default_state.MEM_2_Stage.store_data;
                cout << "1 " << default_state.MEM_2_Stage.ALU_result << " " << mips->data[default_state.MEM_2_Stage.ALU_result] << endl;
                // cout << " " << endl;
            }
            else if(default_state.MEM_2_Stage.Mem_Read){
                if(default_state.MEM_2_Stage.MemtoReg){
                    if(default_state.MEM_2_Stage.format == 2){
                    new_state.WB_Stage.I_result = mips->data[default_state.MEM_2_Stage.ALU_result];}
                }
                 cout << "0 " << endl;
                // cout << " " << endl;
            }
            else{
                if(default_state.MEM_2_Stage.format == 5){
                       new_state.WB_Stage.I_result = default_state.MEM_2_Stage.ALU_result; 
                    }
                else{new_state.WB_Stage.R_result = default_state.MEM_2_Stage.ALU_result;}
                cout << "0 " << endl;
                // cout << " " << endl;
            }


            new_state.WB_Stage.dest_r = default_state.MEM_2_Stage.dest_r;
            new_state.WB_Stage.Reg_Read = default_state.MEM_2_Stage.Reg_Read;
            new_state.WB_Stage.format = default_state.MEM_2_Stage.format;
            new_state.WB_Stage.RegDst = default_state.MEM_2_Stage.RegDst;
            new_state.WB_Stage.MemtoReg = default_state.MEM_2_Stage.MemtoReg;




        }
    //------------------------------------------------MEM_1_Stage-----------------------------------------------------------
        new_state.MEM_2_Stage.nop = default_state.MEM_1_Stage.nop;
        if (default_state.MEM_1_Stage.nop==0){
            new_state.MEM_2_Stage.format = default_state.MEM_1_Stage.format;
            new_state.MEM_2_Stage.ALU_result = default_state.MEM_1_Stage.ALU_result;
            new_state.MEM_2_Stage.store_data = default_state.MEM_1_Stage.store_data;
            new_state.MEM_2_Stage.dest_r = default_state.MEM_1_Stage.dest_r;
            new_state.MEM_2_Stage.RegDst = default_state.MEM_1_Stage.RegDst;
            new_state.MEM_2_Stage.rd = default_state.MEM_1_Stage.rd;
            new_state.MEM_2_Stage.rs = default_state.MEM_1_Stage.rs;
            new_state.MEM_2_Stage.rt = default_state.MEM_1_Stage.rt;
            new_state.MEM_2_Stage.Mem_Read = default_state.MEM_1_Stage.Mem_Read;
            new_state.MEM_2_Stage.Mem_Write = default_state.MEM_1_Stage.Mem_Write;
            new_state.MEM_2_Stage.Reg_Read = default_state.MEM_1_Stage.Reg_Read;
            new_state.MEM_2_Stage.MemtoReg = default_state.MEM_1_Stage.MemtoReg;

        
    //-----------------------------------------Mem to Mem forwarding---------------------------------------------------------------
        if(default_state.MEM_2_Stage.nop == 0){
            if(new_state.MEM_2_Stage.Reg_Read == true && new_state.MEM_2_Stage.dest_r != "$zero"){
            // load store rs
            if( new_state.WB_Stage.format == 2 && new_state.MEM_2_Stage.format == 3){
                if( new_state.WB_Stage.dest_r == new_state.MEM_2_Stage.rs){
                    new_state.MEM_2_Stage.store_data = new_state.WB_Stage.I_result;
                }
            }
            //--------------------------------------WB Mem forwarding----------------------------------------------
            // R store
            if( default_state.WB_Stage.format == 1 && new_state.MEM_2_Stage.format == 3){
                if( default_state.WB_Stage.dest_r == new_state.MEM_2_Stage.rs){
                    new_state.MEM_2_Stage.store_data = default_state.WB_Stage.R_result;
                }
            }
            // I store
            if( default_state.WB_Stage.format == 5 && new_state.MEM_2_Stage.format == 3){
                if( default_state.WB_Stage.dest_r == new_state.MEM_2_Stage.rs){
                    new_state.MEM_2_Stage.store_data = default_state.WB_Stage.I_result;
                }}
            

            //1_3 I store
        }}
        }


    //---------------------------------------------EX----------------------------------------------------------------
        new_state.MEM_1_Stage.nop = default_state.EX_Stage.nop;
        
        if(!default_state.EX_Stage.nop){
            // cout << "in ex stage is " << default_state.EX_Stage.format << endl;
            // cout << "in wb stage is " << default_state.WB_Stage.format << endl;
            // cout << "in new wb stage is " << new_state.WB_Stage.format << endl;
            if(default_state.WB_Stage.Reg_Read == true && default_state.WB_Stage.dest_r!="$zero" && (default_state.WB_Stage.dest_r==
            default_state.EX_Stage.rs)){
            if(default_state.WB_Stage.format == 2 && (default_state.EX_Stage.format == 1 || default_state.EX_Stage.format==4 || 
                default_state.EX_Stage.format == 7 || default_state.EX_Stage.format == 5 || default_state.EX_Stage.format == 2)){
                    // cout << "In Load_R " << endl;
                    default_state.EX_Stage.read_1 = default_state.WB_Stage.I_result;
                    // cout << "data forwarded is " << default_state.WB_Stage.I_result << endl;
                }}

            if(default_state.EX_Stage.format == 4){
                if(new_state.EX_Stage.read_1==new_state.EX_Stage.read_2){
                   new_state.IF_1_Stage.nop = 0;
                   set_IF_nop = true;
                    new_state.IF_1_Stage.PC = mips->address[default_state.RR_Stage.Instr[3]]; 
                    branch_entry = false;
                    branch = false; 
                }             
                else{
                    branch = false;
                    // cout << "beq_not aken " << endl;
                    new_state.IF_1_Stage.nop = 0;
                    set_IF_nop = true;
                    new_state.IF_1_Stage.PC = default_state.IF_1_Stage.PC ;  
                    branch_entry = false;
                }
                
            }
            else if(default_state.EX_Stage.format == 7){
                if(new_state.EX_Stage.read_1!=new_state.EX_Stage.read_2){
                   new_state.IF_1_Stage.nop = 0;
                   set_IF_nop = true;
                    new_state.IF_1_Stage.PC = mips->address[default_state.RR_Stage.Instr[3]]; 
                    bne_branch = false; 
                    branch_entry = false;
                }             
                else{
                    bne_branch = false;
                    new_state.IF_1_Stage.nop = 0;
                    set_IF_nop = true;
                    new_state.IF_1_Stage.PC = default_state.IF_1_Stage.PC; 
                    branch_entry = false;
                }
            }
            default_state.IF_1_Stage.branch_PC =  mips->address[default_state.RR_Stage.Instr[1]];
            default_state.EX_Stage.ALU_op = default_state.EX_Stage.ALU_Control();
            int result = default_state.EX_Stage.operation();
            int e = default_state.EX_Stage.ALU_op;
            // cout << "read_1 is " << default_state.EX_Stage.read_1 << endl;
            // cout << "read_2 is " << default_state.EX_Stage.read_2 << endl;
            //  cout <<"result in ex is " << result << endl;
            // PASS lw/sw TO MEM_1_STAGE
            if(default_state.EX_Stage.format==2 || default_state.EX_Stage.format==3){
            new_state.MEM_1_Stage.ALU_result = result;
            new_state.MEM_1_Stage.store_data = default_state.EX_Stage.read_1;
            new_state.MEM_1_Stage.format = default_state.EX_Stage.format;
            new_state.MEM_1_Stage.rd = default_state.EX_Stage.rd;
            new_state.MEM_1_Stage.rt = default_state.EX_Stage.rt;
            new_state.MEM_1_Stage.rs = default_state.EX_Stage.rs;
            new_state.MEM_1_Stage.Mem_Read = default_state.EX_Stage.Mem_Read;
            new_state.MEM_1_Stage.Mem_Write = default_state.EX_Stage.Mem_Write;
            new_state.MEM_1_Stage.Reg_Read = default_state.EX_Stage.Reg_Read;
            new_state.MEM_1_Stage.MemtoReg = default_state.EX_Stage.MemtoReg;
            new_state.MEM_1_Stage.RegDst = default_state.EX_Stage.RegDst;
            if(default_state.EX_Stage.RegDst){new_state.MEM_1_Stage.dest_r = default_state.EX_Stage.rd;}
            else{new_state.MEM_1_Stage.dest_r = default_state.EX_Stage.rt;}
        }
        

            else if(default_state.EX_Stage.format!=2 && default_state.EX_Stage.format!=3){
                    wait = false;
                    new_state.WB_Stage.nop = 0;
                    new_state.MEM_1_Stage.nop = 1;
                    new_state.MEM_2_Stage.nop = 1;
                    if(default_state.EX_Stage.Reg_Read){
                        if(default_state.EX_Stage.format == 1){
                            new_state.WB_Stage.R_result = result;
                            new_state.WB_Stage.dest_r = default_state.EX_Stage.rd;
                        }
                        else if(default_state.EX_Stage.format == 5){
                            new_state.WB_Stage.I_result = result;
                            new_state.WB_Stage.dest_r = default_state.EX_Stage.rt;
                        }
                    }
                
                new_state.WB_Stage.Reg_Read = default_state.EX_Stage.Reg_Read;
                new_state.WB_Stage.format = default_state.EX_Stage.format;
                new_state.WB_Stage.RegDst = default_state.EX_Stage.RegDst;
                new_state.WB_Stage.MemtoReg = default_state.EX_Stage.MemtoReg;
            }
        }
        //---------------------------------------BF Mem forwarding -----------------------------------------------
            //1_3 R store
            if(default_state.BF_Stage.Reg_Read == true && default_state.BF_Stage.dest_r!="$zero" && not(default_state.WB_Stage.Reg_Read == true &&
            (default_state.WB_Stage.dest_r!="$zero") && (default_state.WB_Stage.dest_r==new_state.MEM_1_Stage.rs)) && (default_state.BF_Stage.dest_r==
            new_state.MEM_1_Stage.rs)){
                // R .... store
                if(default_state.BF_Stage.format == 1  && new_state.MEM_1_Stage.format == 3){
                    new_state.MEM_1_Stage.store_data = default_state.BF_Stage.R_Result;}
                //I .... store
                if(default_state.BF_Stage.format == 5  && new_state.MEM_1_Stage.format == 3){
                    new_state.MEM_1_Stage.store_data = default_state.BF_Stage.I_Result;}
            }
//----------------------------------------------RR Stage----------------------------------------------------------------------
    new_state.EX_Stage.nop = default_state.RR_Stage.nop;
    if(default_state.RR_Stage.nop==0){
            new_state.EX_Stage.op = default_state.RR_Stage.op;
            new_state.EX_Stage.format = default_state.RR_Stage.format;
            new_state.EX_Stage.read_1 = mips->registers[mips->registerMap[default_state.RR_Stage.rs]];
            new_state.EX_Stage.read_2 = mips->registers[mips->registerMap[default_state.RR_Stage.rt]];
            new_state.EX_Stage.rd = default_state.RR_Stage.rd;
            new_state.EX_Stage.rt = default_state.RR_Stage.rt;
            new_state.EX_Stage.rs = default_state.RR_Stage.rs;
            new_state.EX_Stage.imm = default_state.RR_Stage.imm;
            new_state.EX_Stage.RegDst = default_state.RR_Stage.RegDst;
            new_state.EX_Stage.ALU_Src = default_state.RR_Stage.ALU_Src;
            new_state.EX_Stage.Mem_Read = default_state.RR_Stage.Mem_Read;
            new_state.EX_Stage.Mem_Write = default_state.RR_Stage.Mem_Write;
            new_state.EX_Stage.Reg_Read = default_state.RR_Stage.Reg_Read;
            new_state.EX_Stage.MemtoReg = default_state.RR_Stage.MemtoReg; 
    
//----------------------------------------------MEM-EX -Forwarding--------------------------------------------------------------
        
            if(default_state.EX_Stage.nop == 0){
                if(new_state.MEM_2_Stage.Reg_Read == true && new_state.MEM_2_Stage.dest_r != "$zero"){
                //load R | load b
                if((new_state.WB_Stage.format == 2) && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format == 4 || new_state.EX_Stage.format == 7 )){
                    if(new_state.WB_Stage.dest_r ==  new_state.EX_Stage.rs){
                        new_state.EX_Stage.read_1 = new_state.WB_Stage.I_result;
                    }
                    if(new_state.WB_Stage.dest_r ==  new_state.EX_Stage.rt){
                        new_state.EX_Stage.read_2 = new_state.WB_Stage.I_result;
                    }
                }
                // load I
                if((new_state.WB_Stage.format == 2) && (new_state.EX_Stage.format == 5)){
                    if(new_state.WB_Stage.dest_r ==  new_state.EX_Stage.rs){
                        new_state.EX_Stage.read_1 = new_state.WB_Stage.I_result;
                    }
                }
                //load store rt
                if((new_state.WB_Stage.format == 2) && (new_state.EX_Stage.format == 3)){
                    if(new_state.WB_Stage.dest_r ==  new_state.EX_Stage.rt){
                        new_state.EX_Stage.read_1 = new_state.WB_Stage.I_result;
                    }
                }
                //load load rs
                if((new_state.WB_Stage.format == 2) && (new_state.EX_Stage.format == 2)){
                    if(new_state.WB_Stage.dest_r ==  new_state.EX_Stage.rs){
                        new_state.EX_Stage.read_1 = new_state.WB_Stage.I_result;
                    }
                }
            } }

//----------------------------------------------------EX EX Forwarding----------------------------------------------------------------------
            //RR,RB
            if(default_state.EX_Stage.nop==0){
                if(new_state.WB_Stage.Reg_Read == true && new_state.WB_Stage.dest_r != "$zero"){
                if((new_state.WB_Stage.format == 1) && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format == 4 || new_state.EX_Stage.format == 7 )){
                if(new_state.WB_Stage.dest_r == new_state.EX_Stage.rs){
                    new_state.EX_Stage.read_1 = new_state.WB_Stage.R_result;
                } 
                if(new_state.WB_Stage.dest_r == new_state.EX_Stage.rt){
                    new_state.EX_Stage.read_2 = new_state.WB_Stage.R_result;
                }
                }
            // IR,IB
                if((new_state.WB_Stage.format == 5) && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format == 4 || new_state.EX_Stage.format == 7 )){
                if(new_state.WB_Stage.dest_r == new_state.EX_Stage.rs){
                    new_state.EX_Stage.read_1 = new_state.WB_Stage.I_result;
                } 
                if(new_state.WB_Stage.dest_r == new_state.EX_Stage.rt){
                    new_state.EX_Stage.read_2 = new_state.WB_Stage.I_result;
                }
                }
            // II
                if((new_state.WB_Stage.format == 5) && new_state.EX_Stage.format == 5){
                if(new_state.WB_Stage.dest_r == new_state.EX_Stage.rs){
                    new_state.EX_Stage.read_1 = new_state.WB_Stage.I_result;
                } 
                }
            // R load ,R store rs 
                if((new_state.WB_Stage.format == 1) && (new_state.EX_Stage.format == 2 || new_state.EX_Stage.format == 3)){
                if(new_state.WB_Stage.dest_r == new_state.EX_Stage.rs){
                    new_state.EX_Stage.read_1 = new_state.WB_Stage.R_result;
                } 
                }
            // I load, I store rs
                if((new_state.WB_Stage.format == 5) && (new_state.EX_Stage.format == 2 || new_state.EX_Stage.format == 3)){
                if(new_state.WB_Stage.dest_r == new_state.EX_Stage.rs){
                    new_state.EX_Stage.read_1 = new_state.WB_Stage.I_result;
                } 
                }
            // RI
                if((new_state.WB_Stage.format == 1) && (new_state.EX_Stage.format == 5)){
                if(new_state.WB_Stage.dest_r == new_state.EX_Stage.rs){
                    new_state.EX_Stage.read_1 = new_state.WB_Stage.I_result;
                } 
                }
            }}}
        if(default_state.WB_Stage.Reg_Read == true && default_state.WB_Stage.dest_r!="$zero" && not(new_state.WB_Stage.Reg_Read == true &&
            (new_state.WB_Stage.dest_r!="$zero") && (new_state.WB_Stage.dest_r==new_state.EX_Stage.rs)) && (default_state.WB_Stage.dest_r==
            new_state.EX_Stage.rs)){
                // RR RB RI R_LOAD
                if(default_state.WB_Stage.format == 1 && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format==4 || 
                new_state.EX_Stage.format == 7 || new_state.EX_Stage.format == 5 || new_state.EX_Stage.format == 2)){
                    new_state.EX_Stage.read_1 = default_state.WB_Stage.R_result;
                }
                // IR IB II I_LOAD
                if(default_state.WB_Stage.format == 5 && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format==4 || 
                new_state.EX_Stage.format == 7 || new_state.EX_Stage.format == 5 || new_state.EX_Stage.format == 2)){
                    new_state.EX_Stage.read_1 = default_state.WB_Stage.I_result;
                }
                // LOAD_R LOAD_I LOAD_B LOAD_LOAD
                // if(default_state.WB_Stage.format == 2 && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format==4 || 
                // new_state.EX_Stage.format == 7 || new_state.EX_Stage.format == 5 || new_state.EX_Stage.format == 2)){
                //     cout << "In Load_R " << endl;
                //     new_state.EX_Stage.read_1 = default_state.WB_Stage.I_result;
                //     cout << "data forwarded is " << default_state.WB_Stage.I_result << endl;
                // }                
            }
        if(default_state.WB_Stage.Reg_Read == true && default_state.WB_Stage.dest_r!="$zero" && not(new_state.WB_Stage.Reg_Read == true &&
            (new_state.WB_Stage.dest_r!="$zero") && (new_state.WB_Stage.dest_r==new_state.EX_Stage.rt)) && (default_state.WB_Stage.dest_r==
            new_state.EX_Stage.rt)){
                // RR RB R_STORE
                if(default_state.WB_Stage.format == 1 && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format==4 || 
                new_state.EX_Stage.format == 7 || new_state.EX_Stage.format == 3)){
                    new_state.EX_Stage.read_2 = default_state.WB_Stage.R_result;
                }
                // IR IB I_STORE
                if(default_state.WB_Stage.format == 5 && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format==4 || 
                new_state.EX_Stage.format == 7 || new_state.EX_Stage.format == 3)){
                    new_state.EX_Stage.read_2 = default_state.WB_Stage.I_result;
                }
                // LOAD_R LOAD_B LOAD_STORE
                if(default_state.WB_Stage.format == 2 && (new_state.EX_Stage.format == 1 || new_state.EX_Stage.format==4 || 
                new_state.EX_Stage.format == 7 || new_state.EX_Stage.format == 3)){
                    // cout << "In load_R" << endl;
                    new_state.EX_Stage.read_2 = default_state.WB_Stage.I_result;
                }
            }

//--------------------------------------------ID_2 Stage----------------------------------------------------------------------
    new_state.RR_Stage.nop = default_state.ID_2_Stage.nop;
        if(default_state.ID_2_Stage.nop==0){
            string ins = default_state.ID_2_Stage.Instr[0];
            if(compare(ins,"add") || compare(ins,"sub" ) || compare(ins,"and") || compare(ins,"or")  || compare(ins,"slt") || compare(ins,"mul")){
                new_state.RR_Stage.format = 1;
                new_state.RR_Stage.Instr = default_state.ID_2_Stage.Instr;
                new_state.RR_Stage.rd = default_state.ID_2_Stage.Instr[1];
                new_state.RR_Stage.rt = default_state.ID_2_Stage.Instr[3];
                new_state.RR_Stage.rs = default_state.ID_2_Stage.Instr[2];
                new_state.RR_Stage.read_1 = mips->registers[mips->registerMap[default_state.ID_2_Stage.Instr[2]]];
                new_state.RR_Stage.read_2 = mips->registers[mips->registerMap[default_state.ID_2_Stage.Instr[3]]];
                new_state.RR_Stage.dest_r = default_state.ID_2_Stage.Instr[1];
                new_state.RR_Stage.RegDst= true;
                new_state.RR_Stage.ALU_Src= false;
                new_state.RR_Stage.Mem_Read = false;
                new_state.RR_Stage.Mem_Write=false;
                new_state.RR_Stage.Reg_Read=true;
                new_state.RR_Stage.MemtoReg=false;                
            }
            else if(ins == "lw"){
                new_state.RR_Stage.format = 2;
                new_state.RR_Stage.Instr = default_state.ID_2_Stage.Instr;
                string str = default_state.ID_2_Stage.Instr[2];
                size_t start_pos = str.find("($");          // find the start of the substring to extract
                size_t end_pos = str.find(")", start_pos);  // find the end of the substring to extract
                string substri = str.substr(start_pos+1, end_pos-start_pos-1); 
                string subst = str.substr(0,start_pos); 
                new_state.RR_Stage.imm = stoi(subst);
                new_state.RR_Stage.rt = default_state.ID_2_Stage.Instr[1];
                new_state.RR_Stage.dest_r = new_state.RR_Stage.rt;
                new_state.RR_Stage.rs = substri;
                new_state.RR_Stage.RegDst= false;
                new_state.RR_Stage.ALU_Src= true;
                new_state.RR_Stage.read_1 = mips->registers[mips->registerMap[substri]];
                new_state.RR_Stage.Mem_Read = true;
                new_state.RR_Stage.Mem_Write=false;
                new_state.RR_Stage.Reg_Read=true;
                new_state.RR_Stage.MemtoReg=true;
            }
            else if(ins == "sw"){
                new_state.RR_Stage.format = 3;
                new_state.RR_Stage.Instr = default_state.ID_2_Stage.Instr;
                string str = default_state.ID_2_Stage.Instr[2];
                size_t start_pos = str.find("($");          // find the start of the substring to extract
                size_t end_pos = str.find(")", start_pos);  // find the end of the substring to extract
                string substri = str.substr(start_pos+1, end_pos-start_pos-1); 
                new_state.RR_Stage.rt =substri;
                string subst = str.substr(0,start_pos); 
                new_state.RR_Stage.imm = stoi(subst);
                new_state.RR_Stage.dest_r = "";
                new_state.RR_Stage.rs = default_state.ID_2_Stage.Instr[1];
                // cout << "setting read 1 to ===== " << mips->registers[mips->registerMap[default_state.ID_2_Stage.Instr[1]]] << endl;
                // cout << default_state.ID_2_Stage.Instr[1] << endl;
                new_state.RR_Stage.read_1 = mips->registers[mips->registerMap[default_state.ID_2_Stage.Instr[1]]];
                new_state.RR_Stage.read_2 = mips->registers[mips->registerMap[substri]];
                new_state.RR_Stage.ALU_Src= true;
                new_state.RR_Stage.Mem_Read = false;
                new_state.RR_Stage.Mem_Write=true;
                new_state.RR_Stage.Reg_Read=false;
                new_state.RR_Stage.RegDst=false;
            }
            else if(ins == "beq" || ins == "bne"){
                if(ins == "beq"){
                    new_state.RR_Stage.format = 4;
                    branch = true;
                }
                else if(ins == "bne"){
                    new_state.RR_Stage.format = 7;
                    bne_branch = true;
                }
                new_state.RR_Stage.Instr = default_state.ID_2_Stage.Instr;
                new_state.RR_Stage.ALU_Src= false;
                new_state.RR_Stage.Mem_Read = false;
                new_state.RR_Stage.Mem_Write=false;
                new_state.RR_Stage.Reg_Read=false;
                new_state.RR_Stage.read_1 = mips->registers[mips->registerMap[default_state.ID_2_Stage.Instr[2]]];
                new_state.RR_Stage.read_2 = mips->registers[mips->registerMap[default_state.ID_2_Stage.Instr[1]]];
                default_state.IF_1_Stage.nop = 1;
                new_state.IF_1_Stage.PC = default_state.IF_1_Stage.PC;
                new_state.RR_Stage.rs = default_state.ID_2_Stage.Instr[2];
                new_state.RR_Stage.rt = default_state.ID_2_Stage.Instr[1];
            }
            else if(ins == "j"){
                new_state.RR_Stage.format = 6;
                new_state.RR_Stage.Instr = default_state.ID_2_Stage.Instr;
                new_state.RR_Stage.ALU_Src= false;
                new_state.RR_Stage.Mem_Read = false;
                new_state.RR_Stage.Mem_Write=false;
                new_state.RR_Stage.rs = "";
                new_state.RR_Stage.rt = "";
                new_state.RR_Stage.Reg_Read=false;
                jump = true;  
                default_state.IF_1_Stage.nop = 1;
                new_state.IF_1_Stage.PC = default_state.IF_1_Stage.PC;
            }
            else if(compare(ins,"addi") || compare(ins,"sll" )|| compare(ins,"srl")){
                new_state.RR_Stage.format = 5;
                new_state.RR_Stage.Instr = default_state.ID_2_Stage.Instr;
                new_state.RR_Stage.RegDst= false;
                new_state.RR_Stage.ALU_Src= true;
                new_state.RR_Stage.Mem_Read = false;
                new_state.RR_Stage.Mem_Write=false;
                new_state.RR_Stage.Reg_Read=true;
                new_state.RR_Stage.MemtoReg=false;
                new_state.RR_Stage.read_1 = mips->registers[mips->registerMap[default_state.ID_2_Stage.Instr[2]]];
                new_state.RR_Stage.imm = stoi(default_state.ID_2_Stage.Instr[3]);
                new_state.RR_Stage.rt = default_state.ID_2_Stage.Instr[1];
                new_state.RR_Stage.rs = default_state.ID_2_Stage.Instr[2];
                new_state.RR_Stage.dest_r = default_state.ID_2_Stage.Instr[1];

            } 
        
            new_state.RR_Stage.op = default_state.ID_2_Stage.Instr[0];
            if(new_state.RR_Stage.format == 6){
                new_state.IF_1_Stage.nop = 0;
                new_state.IF_2_Stage.nop = 0;
                new_state.ID_1_Stage.nop = 0;
                new_state.IF_1_Stage.PC = mips->address[default_state.ID_2_Stage.Instr[1]];
                jump = false;
                set_IF_nop = true;
            } 
        }
//--------------------------------------------------------------ID_1 Stage--------------------------------------------------
    new_state.ID_2_Stage.nop = default_state.ID_1_Stage.nop;
    if(default_state.ID_1_Stage.nop==0){
        new_state.ID_2_Stage.Instr = default_state.ID_1_Stage.Instr;
        string ins = default_state.ID_1_Stage.Instr[0];
        if(compare(ins,"add") || compare(ins,"sub" ) || compare(ins,"and") || compare(ins,"or")  || compare(ins,"slt") || compare(ins,"mul")){
            new_state.ID_2_Stage.rt = default_state.ID_1_Stage.Instr[3];
            new_state.ID_2_Stage.rs = default_state.ID_1_Stage.Instr[2];
            new_state.ID_2_Stage.rd = default_state.ID_1_Stage.Instr[1];
        }
        else if(ins == "lw"){
            new_state.ID_2_Stage.Instr = default_state.ID_1_Stage.Instr;
            string str = default_state.ID_1_Stage.Instr[2];
            size_t start_pos = str.find("($");          // find the start of the substring to extract
            size_t end_pos = str.find(")", start_pos);  // find the end of the substring to extract
            string substri = str.substr(start_pos+1, end_pos-start_pos-1); 
            string subst = str.substr(0,start_pos); 
            new_state.ID_2_Stage.rt = default_state.ID_1_Stage.Instr[1];
            new_state.ID_2_Stage.rd  = "";
            new_state.ID_2_Stage.rs = substri;
        }
        else if(ins == "sw"){
            string str = default_state.ID_1_Stage.Instr[2];
            size_t start_pos = str.find("($");          // find the start of the substring to extract
            size_t end_pos = str.find(")", start_pos);  // find the end of the substring to extract
            string substri = str.substr(start_pos+1, end_pos-start_pos-1); 
            new_state.ID_2_Stage.rt =substri;
            string subst = str.substr(0,start_pos); 
            new_state.ID_2_Stage.rd = "";
            new_state.ID_2_Stage.rs = default_state.ID_1_Stage.Instr[1];
        }
        else if(compare(ins,"addi") || compare(ins,"sll" )|| compare(ins,"srl") || compare(ins,"beq") || (compare(ins,"bne"))){
            new_state.ID_2_Stage.rt = default_state.ID_1_Stage.Instr[1];
            new_state.ID_2_Stage.rs = default_state.ID_1_Stage.Instr[2];
            new_state.ID_2_Stage.rd  = "";
        }
        else if(compare(ins,"j")){
            new_state.ID_2_Stage.rt = "";
            new_state.ID_2_Stage.rs = "";
            new_state.ID_2_Stage.rd  = "";
        }

        }

    

//--------------------------------------------------------------IF_2 Stage---------------------------------------------------
    if(stall== true && ((cycle == stall_marker_1+2) || (cycle == stall_marker_2+1) || (cycle == stall_marker_3 + 4) )){
            stall=false;
            default_state.IF_2_Stage.nop=0;            
        }
    
    if(stall79 == true && cycle == stall_marker_79+2){
        stall79=false;
        default_state.IF_2_Stage.nop=0;
    }
    new_state.ID_1_Stage.nop = default_state.IF_2_Stage.nop;
    if(default_state.IF_2_Stage.nop == 0){
        new_state.ID_1_Stage.Instr =default_state.IF_2_Stage.Instr;
        string ins = default_state.IF_2_Stage.Instr[0];
        string subst;
        if(ins == "lw"){
            string str = default_state.IF_2_Stage.Instr[2];
            size_t start_pos = str.find("($");          // find the start of the substring to extract
            size_t end_pos = str.find(")", start_pos);  // find the end of the substring to extract
            string substri = str.substr(start_pos+1, end_pos-start_pos-1); 
            subst = str.substr(0,start_pos); 
            new_state.ID_1_Stage.rt = default_state.IF_2_Stage.Instr[1];
            new_state.ID_1_Stage.rs = substri;
            }

        string sub_sw;
        if(ins == "sw"){
            string str = default_state.IF_2_Stage.Instr[2];
            size_t start_pos = str.find("($");          // find the start of the substring to extract
            size_t end_pos = str.find(")", start_pos);  // find the end of the substring to extract
            sub_sw = str.substr(start_pos+1, end_pos-start_pos-1); 
            string substorw = str.substr(0,start_pos); 
            new_state.ID_1_Stage.rs = default_state.IF_2_Stage.Instr[1];
            new_state.ID_1_Stage.rt = sub_sw;
        }
        if(compare(ins,"addi") || compare(ins,"sll" )|| compare(ins,"srl") || compare(ins,"beq") || (compare(ins,"bne"))){
            new_state.ID_1_Stage.rt = default_state.IF_2_Stage.Instr[1];
            new_state.ID_1_Stage.rs = default_state.IF_2_Stage.Instr[2];
            new_state.ID_1_Stage.rd  = "";
        }
        if(compare(ins,"add") || compare(ins,"sub" ) || compare(ins,"and") || compare(ins,"or")  || compare(ins,"slt") || compare(ins,"mul")){
            new_state.ID_1_Stage.rt = default_state.IF_2_Stage.Instr[3];
            new_state.ID_1_Stage.rs = default_state.IF_2_Stage.Instr[2];
            new_state.ID_1_Stage.rd = default_state.IF_2_Stage.Instr[1];
        }
        if(compare(ins,"j")){
            new_state.ID_1_Stage.rt = "";
            new_state.ID_1_Stage.rs = "";
            new_state.ID_1_Stage.rd  = "";
        }


        if(ins=="beq" || ins=="bne"){
            branch_entry = true;
            default_state.IF_1_Stage.nop =1;
            new_state.IF_1_Stage.PC = default_state.IF_1_Stage.PC;
        }
        if(default_state.ID_1_Stage.nop == 0 && default_state.IF_2_Stage.nop==0){    
            if(stall == false){
                // RR / R IMM / IMM R / IMM IMM
                if(default_state.ID_2_Stage.nop == 0 && default_state.IF_2_Stage.nop == 0){
                    if((new_state.RR_Stage.format == 1 || new_state.RR_Stage.format == 5) && 
                        (new_state.ID_2_Stage.Instr[0]== "addi" || new_state.ID_2_Stage.Instr[0]== "sll" || new_state.ID_2_Stage.Instr[0]== "srl" ||
                        (compare(new_state.ID_2_Stage.Instr[0],"mul") || compare(new_state.ID_2_Stage.Instr[0],"add") || new_state.ID_2_Stage.Instr[0]=="sub" || new_state.ID_2_Stage.Instr[0]=="and"|| new_state.ID_2_Stage.Instr[0]=="or" || new_state.ID_2_Stage.Instr[0]=="slt" )))
                    {if((new_state.RR_Stage.dest_r == new_state.ID_1_Stage.rs) || (new_state.RR_Stage.dest_r == new_state.ID_1_Stage.rt)){
                        new_state.ID_1_Stage.nop = 1;
                        stall = true;
                        stall_marker_2 = cycle;
                    } }
                }
                // R-LOAD-STORE / I-LOAD-STORE RS
                if(default_state.ID_2_Stage.nop == 0 && default_state.IF_2_Stage.nop == 0){
                    if((new_state.RR_Stage.format==1 || new_state.RR_Stage.format == 5) && (new_state.ID_1_Stage.Instr[0] == "lw" || new_state.ID_1_Stage.Instr[0]== "sw")){
                        // cout << new_state.RR_Stage.dest_r << endl;
                        // cout << new_state.ID_1_Stage.rs << endl;
                    if((new_state.RR_Stage.dest_r == new_state.ID_1_Stage.rs)){
                        new_state.ID_1_Stage.nop = 1;
                        stall = true;
                        stall_marker_2 = cycle;
                    } }
                }
                // R-STORE / I-STORE RT 
                if(default_state.ID_2_Stage.nop == 0 && default_state.IF_2_Stage.nop == 0){
                    if((new_state.RR_Stage.format==1 || new_state.RR_Stage.format == 5) && (new_state.ID_1_Stage.Instr[0]== "sw")){
                    if((new_state.RR_Stage.dest_r == new_state.ID_1_Stage.rt)){
                        new_state.ID_1_Stage.nop = 1;
                        stall = true;
                        stall_marker_2 = cycle;
                    } }
                }
                // LOAD-LOAD 
                if(default_state.ID_2_Stage.nop == 0 && default_state.IF_2_Stage.nop == 0){
                    if((new_state.RR_Stage.format==2) && (new_state.ID_1_Stage.Instr[0]== "lw" || new_state.ID_1_Stage.Instr[0] == "sw" )){
                    if((new_state.RR_Stage.rt == new_state.ID_1_Stage.rs)){
                        new_state.ID_1_Stage.nop = 1;
                        stall = true;
                        stall_marker_2 = cycle;
                    } }
                }
                //LOAD-STORE
                if(default_state.ID_2_Stage.nop == 0 && default_state.IF_2_Stage.nop == 0){
                    if((new_state.RR_Stage.format==2) && (new_state.ID_1_Stage.Instr[0] == "sw" )){
                    if((new_state.RR_Stage.rt == new_state.ID_1_Stage.rt)){
                        new_state.ID_1_Stage.nop = 1;
                        stall = true;
                        stall_marker_2 = cycle;
                    } }
                }
            }
            if(default_state.ID_1_Stage.nop == 0 && default_state.IF_2_Stage.nop == 0){
            if((ins != "lw" && ins != "sw") && (default_state.ID_1_Stage.Instr[0] == "lw" || default_state.ID_1_Stage.Instr[0] == "sw")){
                stall_marker_79 = cycle;
                stall79=true;
                
                new_state.ID_1_Stage.nop = 1;
            } }


        }        
    }
//--------------------------------------------------------------IF_1 Stage----------------------------------------------------
    if(default_state.IF_1_Stage.PC >= mips->commands.size()){
            default_state.IF_1_Stage.nop = 1;}
    new_state.IF_2_Stage.nop = default_state.IF_1_Stage.nop;
    if(default_state.IF_1_Stage.nop==0){
            completed = cycle;
            if(stall == true){
                new_state.IF_2_Stage.Instr = default_state.IF_2_Stage.Instr;
            }
            else{
                new_state.IF_2_Stage.Instr = mips->commands[default_state.IF_1_Stage.PC];
            }
            if(branch == true || bne_branch == true){
                new_state.IF_2_Stage.nop =1;
                
            }
            if(stall==true){
                new_state.IF_1_Stage.PC = default_state.IF_1_Stage.PC;
                new_state.IF_2_Stage.nop = 1;
            }
            else if(default_state.IF_1_Stage.PCSrc==false){
                new_state.IF_1_Stage.PC = default_state.IF_1_Stage.PC + 1;
            }
            if(stall79 == true){
                new_state.IF_1_Stage.PC = default_state.IF_1_Stage.PC;
                new_state.IF_2_Stage.nop = 1;
                new_state.IF_2_Stage.Instr = default_state.IF_2_Stage.Instr;
            }
        }
        if(set_IF_nop == false){
        if(branch == true || jump == true || bne_branch == true){
            new_state.IF_1_Stage.nop = 1;
        }
        else{
        new_state.IF_1_Stage.nop = default_state.IF_1_Stage.nop;

        }
        }
        else if(set_IF_nop == true){ set_IF_nop = false;}
        if(stall == false && stall79 == false && default_state.IF_1_Stage.nop==1 && default_state.IF_2_Stage.nop==1 && default_state.ID_1_Stage.nop==1 &&
        default_state.ID_2_Stage.nop==1 && default_state.RR_Stage.nop==1 && default_state.EX_Stage.nop==1 && default_state.MEM_1_Stage.nop==1 &&
        default_state.MEM_2_Stage.nop==1 && default_state.WB_Stage.nop==1){
            break;
        }
        // mips->printRegisters(cycle);
        default_state = new_state;

        cycle++;
        // mips->printRegisters(cycle);
        }
    return cycle;
}
