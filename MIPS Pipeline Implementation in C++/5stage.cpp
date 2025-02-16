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
                    
				std::cout << std::dec << endl;
                // for (int i = 0; i < 32 ; ++i){
                //     std::cout << "  " <<data[i] ;}                
                // std::cout << std::dec << '\n';
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

struct IF{
    int PC;
    bool PCSrc;
    int PC_plus_4;
    int branch_PC;
    int nop;
};

struct ID{
    vector<string> Instr;
    int nop;
};

struct EX{
    int format;
    string op;
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
        else if(op=="slt"){return 7;} 
        else if(op=="mul"){return 8;}
        else if(op=="sw"){return 9;}
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
        else if( ALU_op==8){return (read_1*read_2);}
        else if( ALU_op==9){return (read_2+imm);}
    }
};

struct MEM{
    int format;
    int ALU_result;
    int store_data;
    string dest_r;
    bool RegDst; 
    bool ALU_Src; //Identifies I/R type
    bool Branch;
    bool Mem_Read;
    bool Mem_Write;
    bool Reg_Read;
    bool MemtoReg;
    int nop;
};

struct WB{
    int format;
    string dest_r;
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

struct state{
    IF IF_Stage;
    ID ID_Stage;
    EX EX_Stage;
    MEM MEM_Stage;
    WB WB_Stage;
};
int main(int argc, char *argv[])
{    
	if (argc != 2)
	{
		std::cerr << "Required argument: file_name\n./MIPS_interpreter <file name>\n";
		return 0;
	}
	std::ifstream file(argv[1]);
	MIPS_Architecture *mips;
    state default_state,new_state;
    
	if (file.is_open())
		mips = new MIPS_Architecture(file);
	else
	{
		std::cerr << "File could not be opened. Terminating...\n";
		return 0;
	}

    default_state.IF_Stage.PC=0;
    default_state.IF_Stage.nop=0;
    default_state.IF_Stage.branch_PC=0;
    default_state.IF_Stage.PC_plus_4=0;
    default_state.IF_Stage.PCSrc=false;

    default_state.ID_Stage.Instr;
    default_state.ID_Stage.nop=1;

    default_state.EX_Stage.format = 0;
    default_state.EX_Stage.op="";
    default_state.EX_Stage.read_1=0;
    default_state.EX_Stage.read_2=0;
    default_state.EX_Stage.imm=0;
    default_state.EX_Stage.rd="";
    default_state.EX_Stage.rt="";
    default_state.EX_Stage.rs="";
    default_state.EX_Stage.RegDst=false;
    default_state.EX_Stage.ALU_op=8;
    default_state.EX_Stage.ALU_Src=false;
    default_state.EX_Stage.Branch=false;
    default_state.EX_Stage.Mem_Read=false;
    default_state.EX_Stage.Mem_Write=false;
    default_state.EX_Stage.Reg_Read=false;
    default_state.EX_Stage.MemtoReg=false;
    default_state.EX_Stage.nop=1;

    default_state.MEM_Stage.format =0 ;
    default_state.MEM_Stage.ALU_result=0;
    default_state.MEM_Stage.store_data=0;
    default_state.MEM_Stage.dest_r="";
    default_state.MEM_Stage.RegDst=false;
    default_state.MEM_Stage.ALU_Src=false;
    default_state.MEM_Stage.Branch=false;
    default_state.MEM_Stage.Mem_Read=false;
    default_state.MEM_Stage.Mem_Write=false;
    default_state.MEM_Stage.Reg_Read=false;
    default_state.MEM_Stage.MemtoReg=false;
    default_state.MEM_Stage.nop=1;

    default_state.WB_Stage.format = 0;
    default_state.WB_Stage.dest_r="";
    default_state.WB_Stage.R_result=0;
    default_state.WB_Stage.I_result=0;
    default_state.WB_Stage.RegDst=false;
    default_state.WB_Stage.ALU_Src=false;
    default_state.WB_Stage.Branch=false;
    default_state.WB_Stage.Mem_Read=false;
    default_state.WB_Stage.Mem_Write=false;
    default_state.WB_Stage.Reg_Read=false;
    default_state.WB_Stage.MemtoReg=false;
    default_state.WB_Stage.nop=1;

    int cycle=0;
    int stall_marker_1=0;
    int stall_marker_2=0;
    bool stall = false;
    bool branch = false;
    bool bne_branch = false;
    bool set_IF_nop = false;
    bool jump = false;
    // while((default_state.IF_Stage.nop==0) || (default_state.ID_Stage.nop==0) || (default_state.EX_Stage.nop==0) || (default_state.MEM_Stage.nop==0) || (default_state.WB_Stage.nop==0)){
    while(true){
        mips->printRegisters(cycle);
        //-------------------------WB Stage-----------------------------
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
        }
        //-----------------------MEM Stage--------------------------------
        new_state.WB_Stage.nop = default_state.MEM_Stage.nop; 
        if(default_state.MEM_Stage.nop ==1){
        cout << "0 " << endl;
        
        }
        if(default_state.MEM_Stage.nop == 0){
            if(default_state.MEM_Stage.Mem_Write==true){
                
                mips->data[default_state.MEM_Stage.ALU_result]=default_state.MEM_Stage.store_data;
                cout << "1 " << default_state.MEM_Stage.ALU_result << " " << mips->data[default_state.MEM_Stage.ALU_result] << endl;
                
            }
            else if(default_state.MEM_Stage.Mem_Read==true){
                if(default_state.MEM_Stage.MemtoReg==true){
                    new_state.WB_Stage.I_result = mips->data[default_state.MEM_Stage.ALU_result];
                }
                cout << "0 " << endl;
                
            }
            else{
                if(default_state.MEM_Stage.format == 5){
                       new_state.WB_Stage.I_result = default_state.MEM_Stage.ALU_result; 
                    }
                else{new_state.WB_Stage.R_result = default_state.MEM_Stage.ALU_result;}
                cout << "0 " << endl;
            
            }
            new_state.WB_Stage.dest_r = default_state.MEM_Stage.dest_r;
            new_state.WB_Stage.Reg_Read = default_state.MEM_Stage.Reg_Read;
            new_state.WB_Stage.MemtoReg = default_state.MEM_Stage.MemtoReg;
            new_state.WB_Stage.RegDst = default_state.MEM_Stage.RegDst;
            new_state.WB_Stage.format = default_state.MEM_Stage.format;
        }
        // --------------------------EX Stage----------------------------------
        new_state.MEM_Stage.nop = default_state.EX_Stage.nop;

        if(default_state.EX_Stage.nop==0){
            if(default_state.EX_Stage.format == 4){
                if(new_state.EX_Stage.read_1==new_state.EX_Stage.read_2){
                   new_state.IF_Stage.nop = 0;
                   set_IF_nop = true;
                    new_state.IF_Stage.PC = mips->address[default_state.ID_Stage.Instr[3]]; 
                    branch = false; 
                }             
                else{
                    branch = false;
                    new_state.IF_Stage.nop = 0;
                    set_IF_nop = true;
                    new_state.IF_Stage.PC = default_state.IF_Stage.PC ;  
                }
                
            }
            else if(default_state.EX_Stage.format == 7){
                if(new_state.EX_Stage.read_1!=new_state.EX_Stage.read_2){
                   new_state.IF_Stage.nop = 0;
                   set_IF_nop = true;
                    new_state.IF_Stage.PC = mips->address[default_state.ID_Stage.Instr[3]]; 
                    bne_branch = false; 
                }             
                else{
                    bne_branch = false;
                    new_state.IF_Stage.nop = 0;
                    set_IF_nop = true;
                    new_state.IF_Stage.PC = default_state.IF_Stage.PC ;  
                }
            }
            default_state.IF_Stage.branch_PC =  mips->address[default_state.ID_Stage.Instr[1]];
            new_state.MEM_Stage.store_data = default_state.EX_Stage.read_1;
            default_state.EX_Stage.ALU_op = default_state.EX_Stage.ALU_Control();
            int result = default_state.EX_Stage.operation();
            int e = default_state.EX_Stage.ALU_op;
            new_state.MEM_Stage.ALU_result = result;
            new_state.MEM_Stage.format = default_state.EX_Stage.format;
            new_state.MEM_Stage.Mem_Read = default_state.EX_Stage.Mem_Read;
            new_state.MEM_Stage.Mem_Write = default_state.EX_Stage.Mem_Write;
            new_state.MEM_Stage.Reg_Read = default_state.EX_Stage.Reg_Read;
            new_state.MEM_Stage.MemtoReg = default_state.EX_Stage.MemtoReg;
            new_state.MEM_Stage.RegDst = default_state.EX_Stage.RegDst;
            if(default_state.EX_Stage.RegDst==true){new_state.MEM_Stage.dest_r = default_state.EX_Stage.rd;}
            else if(default_state.EX_Stage.format == 3){ new_state.MEM_Stage.dest_r = "";}
            
            else{new_state.MEM_Stage.dest_r = default_state.EX_Stage.rt;}
        }
        // ------------------------ID Stage---------------------------------------
            if(stall== true && ((cycle == stall_marker_1+2) || (cycle == stall_marker_2+1))){
                stall=false;
                default_state.ID_Stage.nop=0;
            }
        new_state.EX_Stage.nop = default_state.ID_Stage.nop;
        if(default_state.ID_Stage.nop==0){
            string ins = default_state.ID_Stage.Instr[0];
            if(compare(ins,"add") || compare(ins,"sub" ) || compare(ins,"and") || compare(ins,"or")  || compare(ins,"slt") || compare(ins,"mul")){
                new_state.EX_Stage.format = 1;
                new_state.EX_Stage.RegDst= true;
                new_state.EX_Stage.ALU_Src= false;
                new_state.EX_Stage.Mem_Read = false;
                new_state.EX_Stage.Mem_Write=false;
                new_state.EX_Stage.Reg_Read=true;
                new_state.EX_Stage.MemtoReg=false;
                new_state.EX_Stage.read_1 = mips->registers[mips->registerMap[default_state.ID_Stage.Instr[2]]];
                new_state.EX_Stage.read_2 = mips->registers[mips->registerMap[default_state.ID_Stage.Instr[3]]];
                new_state.EX_Stage.rd = default_state.ID_Stage.Instr[1];
                new_state.EX_Stage.rt = default_state.ID_Stage.Instr[3];
                new_state.EX_Stage.rs = default_state.ID_Stage.Instr[2];
            }
            else if(ins == "lw"){
                new_state.EX_Stage.format = 2;
                new_state.EX_Stage.RegDst= false;
                new_state.EX_Stage.ALU_Src= true;
                new_state.EX_Stage.Mem_Read = true;
                new_state.EX_Stage.Mem_Write=false;
                new_state.EX_Stage.Reg_Read=true;
                new_state.EX_Stage.MemtoReg=true;
                string str = default_state.ID_Stage.Instr[2];
                size_t start_pos = str.find("($");          // find the start of the substring to extract
                size_t end_pos = str.find(")", start_pos);  // find the end of the substring to extract
                string substri = str.substr(start_pos+1, end_pos-start_pos-1); 
                new_state.EX_Stage.read_1 = mips->registers[mips->registerMap[substri]];
                new_state.EX_Stage.rt = default_state.ID_Stage.Instr[1];
                string subst = str.substr(0,start_pos); 
                new_state.EX_Stage.imm = stoi(subst);
                new_state.EX_Stage.rs = substri;
            }
            else if(ins == "sw"){
                new_state.EX_Stage.format = 3;
                new_state.EX_Stage.ALU_Src= true;
                new_state.EX_Stage.Mem_Read = false;
                new_state.EX_Stage.Mem_Write=true;
                new_state.EX_Stage.Reg_Read=false;
                new_state.EX_Stage.RegDst = false;
                string str = default_state.ID_Stage.Instr[2];
                size_t start_pos = str.find("($");          // find the start of the substring to extract
                size_t end_pos = str.find(")", start_pos);  // find the end of the substring to extract
                string substri = str.substr(start_pos+1, end_pos-start_pos-1); 
                new_state.EX_Stage.read_1 = mips->registers[mips->registerMap[default_state.ID_Stage.Instr[1]]];
                new_state.EX_Stage.read_2 = mips->registers[mips->registerMap[substri]];
                new_state.EX_Stage.rt =substri;
                string subst = str.substr(0,start_pos); 
                new_state.EX_Stage.imm = stoi(subst);
                new_state.EX_Stage.rs = default_state.ID_Stage.Instr[1];
            }
            else if(ins == "beq" || ins == "bne"){
                if(ins == "beq"){
                    new_state.EX_Stage.format = 4;
                    branch = true;
                }
                else if(ins == "bne"){
                    new_state.EX_Stage.format = 7;
                    bne_branch = true;
                }
                new_state.EX_Stage.ALU_Src= false;
                new_state.EX_Stage.Mem_Read = false;
                new_state.EX_Stage.Mem_Write=false;
                new_state.EX_Stage.Reg_Read=false;
                new_state.EX_Stage.rs = default_state.ID_Stage.Instr[2];
                new_state.EX_Stage.rt = default_state.ID_Stage.Instr[1];
                new_state.EX_Stage.read_1 = mips->registers[mips->registerMap[default_state.ID_Stage.Instr[2]]];
                new_state.EX_Stage.read_2 = mips->registers[mips->registerMap[default_state.ID_Stage.Instr[1]]];
                default_state.IF_Stage.nop = 1;
                new_state.IF_Stage.PC = default_state.IF_Stage.PC;
                
            }
            else if(ins == "j"){
                new_state.EX_Stage.format = 6;
                new_state.EX_Stage.ALU_Src= false;
                new_state.EX_Stage.Mem_Read = false;
                new_state.EX_Stage.Mem_Write=false;
                new_state.EX_Stage.rs = "";
                new_state.EX_Stage.rt = "";
                new_state.EX_Stage.Reg_Read=false;
                jump = true;  
                default_state.IF_Stage.nop = 1;
                new_state.IF_Stage.PC = default_state.IF_Stage.PC;
            }
            else if(compare(ins,"addi") || compare(ins,"sll" )|| compare(ins,"srl")){
                new_state.EX_Stage.format = 5;
                new_state.EX_Stage.RegDst= false;
                new_state.EX_Stage.ALU_Src= true;
                new_state.EX_Stage.Mem_Read = false;
                new_state.EX_Stage.Mem_Write=false;
                new_state.EX_Stage.Reg_Read=true;
                new_state.EX_Stage.MemtoReg=false;
                new_state.EX_Stage.read_1 = mips->registers[mips->registerMap[default_state.ID_Stage.Instr[2]]];
                new_state.EX_Stage.imm = stoi(default_state.ID_Stage.Instr[3]);
                new_state.EX_Stage.rt = default_state.ID_Stage.Instr[1];
                new_state.EX_Stage.rs = default_state.ID_Stage.Instr[2];

            } 
            new_state.EX_Stage.op = default_state.ID_Stage.Instr[0];
            if(new_state.EX_Stage.format == 6){
                new_state.IF_Stage.nop = 0;
                new_state.IF_Stage.PC = mips->address[default_state.ID_Stage.Instr[1]];
                jump = false;
                set_IF_nop = true;
            }


            
            if(default_state.EX_Stage.nop == 0 && default_state.ID_Stage.nop == 0){
                if((new_state.MEM_Stage.dest_r == new_state.EX_Stage.rs) || (new_state.MEM_Stage.dest_r == new_state.EX_Stage.rt)){
                    new_state.EX_Stage.nop = 1;
                    stall_marker_1 = cycle;
                    stall=true;
                } 
            } 
            if(stall==false){
                if(default_state.MEM_Stage.nop==0 && default_state.ID_Stage.nop == 0){
                if((new_state.WB_Stage.dest_r == new_state.EX_Stage.rs) || (new_state.WB_Stage.dest_r == new_state.EX_Stage.rt)){
                    new_state.EX_Stage.nop = 1;
                    stall =true;
                    stall_marker_2 = cycle;

                }
            }
            }

        }
        // ------------------------------IF STAGE---------------------------

        if(default_state.IF_Stage.PC >= mips->commands.size()){
            default_state.IF_Stage.nop = 1;
        }
        new_state.ID_Stage.nop = default_state.IF_Stage.nop;
        if(default_state.IF_Stage.nop==0){

            if(stall == true){
                new_state.ID_Stage.Instr = default_state.ID_Stage.Instr;
            }
            else{
                new_state.ID_Stage.Instr = mips->commands[default_state.IF_Stage.PC];
            }
            if(branch == true || bne_branch == true){
                new_state.ID_Stage.nop =1;
                
            }
            if(stall==true){
                new_state.IF_Stage.PC = default_state.IF_Stage.PC;
                new_state.ID_Stage.nop = 1;
            }
            else if(default_state.IF_Stage.PCSrc==false){
                new_state.IF_Stage.PC = default_state.IF_Stage.PC + 1;
            }
            else{
                new_state.IF_Stage.PC = default_state.IF_Stage.branch_PC;
            }
        }
        if(set_IF_nop == false){
        if(branch == true || jump == true || bne_branch == true){
            new_state.IF_Stage.nop = 1;
        }
        else{
        new_state.IF_Stage.nop = default_state.IF_Stage.nop;

        }
        }
        else if(set_IF_nop == true){ set_IF_nop = false;}
                // cycle++;
        if(default_state.IF_Stage.nop==1 && default_state.ID_Stage.nop==1 && default_state.EX_Stage.nop==1 && default_state.MEM_Stage.nop==1 && default_state.WB_Stage.nop==1){
            break;
            
        }
        
        default_state = new_state;
        cycle++;
        
    }
    // mips->printRegisters(cycle);
    
	return cycle;
}
