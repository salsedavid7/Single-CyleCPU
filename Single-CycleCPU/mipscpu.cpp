#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

using namespace std;
int pc = 0;
int alu_zero = 0;
int total_clock_cycles = 0;
int branch_target = 0;
int jump_target = 0;
int branch =0;
int jump = 0;
int registerfile[32] = {0}; 
int d_mem [32]= {0}; 
int regDST = 0;
int regWrite = 0;
int aluSRC = 0;
int memRead = 0;
int memWrite = 0;
int memToReg = 0;
int jumpREG = 0;

string alu(string code){
    //seperate function for ALU control
    //generates ALUOP code used by execute()

	string aluop;
    if (code == "100100"){ //AND
        aluop = "0000";
    }
    if (code == "100101"){ //OR
        aluop = "0001";
    }
    if (code == "100000" || code == "101011" || code == "100011"){ // SW and LW ADD                                                              
        aluop = "0010";
    }
    if (code == "100010" || code == "000100"){ //SUB and BEQ
        aluop = "0110";
    }
    if (code == "101010"){ //SLT
        aluop = "0111";
    }
    if (code == "100111"){ //NOR
        aluop = "1100";
    }
    return aluop;
}

string negativeVal(string immediate){
  //sign-extention, two's complement
    for (int i = 0; i < immediate.size(); i++){
        if (immediate[i] == '1'){
            immediate[i] = '0';
        }
        else{
            immediate[i] = '1';
        }
    }
    for (int i = immediate.size() - 1; i > 0; i--){
        if (immediate[i] == '1'){
            immediate[i] = '0';
        }
        else{
            immediate[i] = '1';
            break;
        }
    }
    return immediate;
}

int immCheck(string immediate){
  //checks if immediate is neg to see if it needs sign extention
	if (immediate[0] == '1'){
        immediate = negativeVal(immediate);
    }
    return stoi(immediate, nullptr, 2);
}

string rv(int rg){
    string val = "";
    //names of registers
    if(rg == 0){
         val = "zero";
        }
    if(rg == 1){
        val = "at";
        }
    if(rg == 2){
        val = "v0";
        }
    if(rg == 3){
        val = "v1";
        }
    if(rg == 4){
        val = "a0";
        }
    if(rg == 5){
        val = "a1";
        }
    if(rg == 6){
        val = "a2";
        }
    if(rg == 7){
        val = "a3";
        }
    if(rg == 8){
        val = "t0";
    }
    if(rg == 9){
        val = "t1";
        }
    if(rg == 10){
        val = "t2";
        }
    if(rg == 11){
        val = "t3";
        }
    if(rg == 12){
        val = "t4";
        }
    if(rg == 13){
        val = "t5";
        }
    if(rg == 14){
        val = "t6";
        }
    if(rg == 15){
        val = "t7";
        }
    if(rg == 16){
        val = "s0";
        }
    if(rg == 17){
        val = "s1";
        }
    if(rg == 18){
        val = "s2";
        }
    if(rg == 19){
        val = "s3";
        }
    if(rg == 20){
        val = "s4";
        }
    if(rg == 21){
        val = "s5";
        }
    if(rg == 22){
        val = "s6";
        }
    if(rg == 23){
        val = "s7";
        }
    if(rg == 24){
        val = "t8";
        }
    if(rg == 25){
        val = "t9";
        }
    if(rg == 26){
        val = "k0";
        }
    if(rg == 27){
        val = "k1";
        }
    if(rg == 28){
        val = "gp";
        }
    if(rg == 29){
        val = "sp";
        }
    if(rg == 30){
        val = "fp";
        }
    if(rg == 31){
        val = "ra";
        }
        return val;
}

int binToDec(string address){
  //binary to decimal
  int value = 0;
  int index = 0;
  for(int i = address.length() - 1;i >= 0;i--){
    if(address[i] == '1'){
      value += pow(2, index);
      }
      index++;
}
  return(value);
}

void ControlUnit(string op){
	// generates control signals using opcode
	// initialize w/ zeros

    if (op == "000000"){ // ADD, SUB, AND, OR, NOR, SLT
        regWrite = 1;
        regDST = 1;
        branch = 0;
        aluSRC = 0;
        memWrite = 0;
        memToReg = 0;
        memRead = 0;
        jump = 0;
    }
    if (op == "100011"){ // LW
        regWrite = 1;
        regDST = 0;
        branch = 0;
        aluSRC = 1;
        memWrite = 0;
        memToReg = 1;
        memRead = 1;
        jump = 0;
    }
    if (op == "101011"){ // SW
        regWrite = 0;
        branch = 0;
        aluSRC = 1;
        memWrite = 1;
        memRead = 0;
        jump = 0;
    }
    if (op == "000100"){ // BEQ
        regWrite = 0;
        branch = 1;
        aluSRC = 0;
        memWrite = 0;
        memRead = 0;
        jump = 0;
    }
    if (op == "000010"){ // J format
        regWrite = 0;
        branch = 0;
        memWrite = 0;
        memRead = 0;
        jump = 1;
    }

    if (op == "000011"){ // JAL
        regWrite = 1;
        regDST = 10;
        memWrite = 0;
        memRead = 0;
        memToReg = 10;
        jump = 1;
    }
    if (op == "001000"){ // JR
        regWrite = 0;
        branch = 0;
        memWrite = 0;
        jumpREG = 1;
        jump = 1;
    }

}

void Writeback(int rg, int result){
	// Get results from ALU and data from d_mem and updates destination register in registerfile

    if (regWrite == 0 && memWrite == 1){ // SW
        
        }else if (regWrite == 1){ // R AND LW
        registerfile[rg] = result;
        cout << "$" << rv(rg) << " is modified to 0x" << hex << result << endl;
    }
    total_clock_cycles = total_clock_cycles + 1;
}

void Mem(int rg, int address){
	// Each entry of d_mem array will be accessed w/ following addresses
  // updates d_mem
    int val = 0;
    if (memWrite == 1){ //SW
        d_mem[address] = val;
        cout << "memory 0x" << hex << address << " is modified to 0x" << hex << val << endl;
        val = registerfile[rg];
        Writeback(address, val);
    }else if (memRead == 1){ //lw
        val = d_mem[address/4]; // divide by 4 to place it in the memory by 4
        Writeback(rg, val);
    }else{ // R/I
        Writeback(rg,address);
    }

}

void execute(string alu_op, int rs, int rt, int rd, int shamt, int address){

	//uses alu_op to determine what kind of math we are doing

	//seperate code for jump and branch where branch/jump target address is updated

	//shift-left-2 sign-extended offset input

	//add shift-left-2 w/ PC+4 value

    int val = 0;
    
    if (alu_op == "0000"){ //AND
        val = registerfile[rs] & registerfile[rt]; 
    } 
    if (alu_op == "0001"){  //OR
        val = registerfile[rs] | registerfile[rt]; 
    }
    if (alu_op == "0110"){ //SUBTRACT AND BEQ
        val = registerfile[rs] - registerfile[rt]; 
    }
    if (alu_op == "1100"){ //NOR
        val = ~(registerfile[rs] | registerfile[rt]); 
    }
    if (alu_op == "0010"){ // LW AND SW
        if (aluSRC == 1){
            val = registerfile[rs] + address;
        }
        else{ // Else add
            val = registerfile[rs] + registerfile[rt];
        }
    }
    if (alu_op == "0111"){ // SLT
        if (registerfile[rs] < registerfile[rt]){
            val = 1;
        }
        else{
            val = 0;
        }
    }

    if(memToReg == 10){ //JAL
        cout << "JAL attempted" << endl;
        address = jump_target;
        jump_target = 31;
        Mem(jump_target, address);
    }

    //checking BEQ and setting alu_zero
    if (val == 0 && branch == 1){
        alu_zero = 1;
    }else{
        alu_zero = 0;
    }

    // check LW, SW, or R/I type
    if (memWrite == 1 || memRead == 1){ //LW and SW
        Mem(rt, val);
    }

    else{ // R or I type
        Mem(rd, val);
    }

}

void decode(string str){
  //decode sets rs rt rd and other necessary registers depending on the instruction and sends to execute
	int rs = 0;
	int rt = 0;
	int rd = 0;
	int shamt;
	int address;
	string funct;
	string immediate;
	string alu_op;
	string opcode = str.substr(0, 6); // get opcode


  // send opcode to control unit
	ControlUnit(opcode);

  //sign extention when necessary
	if(jump == 1){  //J OR JAL
		immediate = str.substr(6, 26);
		address = immCheck(immediate);
		address = address << 2;
        cout << "address :" << address << endl;
		jump_target = address;
	}
    else if(branch == 1){ //BRANCH
		rs = binToDec(str.substr(6, 5));
        rt = binToDec(str.substr(11, 5));
		alu_op = alu(opcode);
		immediate = str.substr(16,16);
		address = immCheck(immediate);
		address = address << 2;
		branch_target = address;
	}

    else{  // R and I
		rs = binToDec(str.substr(6, 5));
        if (regDST == 1 && regWrite == 1){
            rd = binToDec(str.substr(16, 5));
        }else{ //LW and SW
            rt = binToDec(str.substr(11, 5));
        }

        if (aluSRC == 0){ // R
            rt = binToDec(str.substr(11, 5));
            shamt = binToDec(str.substr(21,5));
            funct = str.substr(26, 6);
            alu_op = alu(funct); //alu_op determined by funct  
        }

        else{ // Immediate LW and SW
            immediate = str.substr(16, 16);
            alu_op = alu(opcode); //alu_op determined by opcode
            address = immCheck(immediate);
        }
	}
    //sends to execute no matter what
    execute(alu_op, rs, rt, rd, shamt, address);
	
}

void Fetch(vector<string> instruct){
  
  // Runs lines of instructions from the file. Goes until pc/4 is bigger than the number of lines in the file

	while(pc/4 < instruct.size()){
		cout << "total_clock_cycles " << dec << total_clock_cycles + 1 << ":" << endl;

		decode(instruct[pc/4]);
		
		int next_pc = pc + 4; // update pc

		//logic for copying values
		if (alu_zero == 1 && branch == 1){
            pc = next_pc + branch_target;
        }else if(jump == 1){
            pc = jump_target;
        }else{
            pc = next_pc;
        }
		cout << "pc is modified to 0x" << hex << pc << endl << endl;
	
	}
  //outputs clock cycles

	cout << "program terminated:" << endl;
	cout << "total execution time: " << total_clock_cycles << " cycles." << endl;

}

int main() {
    
	vector<string> instruction; // vector to store text file contents
	string line;      //line of code from text file
	string filename;  //hardcoded filenames
	  cout << "Enter file name to run:\n";
    cin >> filename;
    cout << "\n";

    //reg file and data mem presets

    if(filename == "sample_part1.txt"){
      registerfile[9] = 0x20;
        registerfile[10] = 0x5;
        registerfile[16] = 0x70;
        d_mem[28] = 0x5;
        d_mem[29] = 0x10;
    }
    else if(filename == "sample_part2.txt"){
        registerfile[16] = 0x20;
        registerfile[4] = 0x5;
        registerfile[5] = 0x2;
        registerfile[7] = 0xa;
    }

    // load text file into the vector
    ifstream file(filename); 
    while(getline(file, line)){
        instruction.push_back(line);
    }
    Fetch(instruction);

}
