#ifndef CIRCUIT_FILE
#define CIRCUIT_FILE

#include "emp-tool/execution/circuit_execution.h"
#include "emp-tool/execution/protocol_execution.h"
#include "emp-tool/utils/block.h"
#include "emp-tool/circuits/bit.h"
#include <stdio.h>

#define MAX_CKT_PARAM 1048576 //2^20

template <typename T>
void CktParamBound(T param){
    if (param > MAX_CKT_PARAM){
		std::cout << "Circuit parameter exceeds allowed limit: " << MAX_CKT_PARAM << std::endl << "If you want to allow larger netlists, edit MAX_CKT_PARAM in 'emp-tool/circuits/circuit_file.h'" << std::endl;
		exit(-1);
	}
}
template<typename T, typename... Args>
void CktParamBound(T param, Args... args){ // recursive variadic function
	return CktParamBound(args...);
}

namespace emp {
#define NUM_CONST 2

#define AND_GATE 0
#define XOR_GATE 1
#define NOT_GATE 2
#define DFF_GATE 3

class CircuitFile { 
public:
	int num_gate, num_wire, n0, n0_0, n1, n1_0, n2, n2_0, n3;
	int *gates;
	block * wires;
	
	CircuitFile(const char * file, bool bin = false) {
		num_gate = 0;
		num_wire = 0;
		n0 = 0;
		n0_0 = 0;
		n1 = 0;
		n1_0 = 0;
		n2 = 0;
		n2_0 = 0;
		n3 = 0;

		FILE * f;	
		if (!bin) f = fopen(file, "r");
		else f = fopen(file, "rb");
		if (f == NULL) {
			perror(file);
			exit(-1);
		}
		
		if (!bin){
			int tmp = 0, tmp2 = 0;

			tmp2=fscanf(f, "%d%d\n", &num_gate, &num_wire);
			tmp2=fscanf(f, "%d%d%d%d%d%d%d\n", &n0, &n0_0, &n1, &n1_0, &n2, &n2_0, &n3);
			CktParamBound <int>(num_gate, num_wire, n0, n0_0, n1, n1_0, n2, n2_0, n3);
			tmp2=fscanf(f, "\n");
			char str[10];
			gates = new int[num_gate*4];
			wires = nullptr;
			for(int i = 0; i < num_gate; ++i) {
				tmp2=fscanf(f, "%d", &tmp);
				if (tmp == 2) {
					tmp2=fscanf(f, "%d%d%d%d%10s", &tmp, &gates[4*i], &gates[4*i+1], &gates[4*i+2], str);
					if (str[0] == 'A') gates[4*i+3] = AND_GATE;
					else if (str[0] == 'X') gates[4*i+3] = XOR_GATE;
					else if (str[0] == 'D') gates[4*i+3] = DFF_GATE;
				}
				else if (tmp == 1) {
					tmp2=fscanf(f, "%d%d%d%10s", &tmp, &gates[4*i], &gates[4*i+2], str);
					gates[4*i+3] = NOT_GATE;
				}
			}
		}
		else{			
			int circuit_params[9];	
			CHECK_EXPR(fread((char*)circuit_params, 9*sizeof(int), 1, f) == 1)
		
			num_gate  =  circuit_params[0]; 
			num_wire  =  circuit_params[1]; 
			n0        =  circuit_params[2]; 
			n0_0      =  circuit_params[3]; 
			n1        =  circuit_params[4]; 
			n1_0      =  circuit_params[5]; 
			n2        =  circuit_params[6]; 
			n2_0      =  circuit_params[7]; 
			n3        =  circuit_params[8]; 
			CktParamBound <int>(num_gate, num_wire, n0, n0_0, n1, n1_0, n2, n2_0, n3);
			
			gates = new int[num_gate*4];
			CHECK_EXPR(fread((char*)gates, num_gate*4*sizeof(int), 1, f) == 1)						
		}
		
		fclose(f);
	}

	CircuitFile(const CircuitFile& cf) {
		num_gate = cf.num_gate;
		num_wire = cf.num_wire;
		n1 = cf.n1;
		n2 = cf.n2;
		n3 = cf.n3;
		CktParamBound <int>(num_gate, num_wire, n0, n0_0, n1, n1_0, n2, n2_0, n3);
		gates = new int[num_gate*4];
		wires = new block[num_wire];
		memcpy(gates, cf.gates, num_gate*4*sizeof(int));
		memcpy(wires, cf.wires, num_wire*sizeof(block));	
	}
	
	~CircuitFile(){
		delete[] gates;
		//delete[] wires;
	}
	
	void toBinary(const char * file){				
		FILE * f= fopen(file, "wb");
		if (f == NULL) {
			perror(file);
			exit(-1);
		}
		
		int circuit_params[9];
		
		circuit_params[0] = num_gate; 
		circuit_params[1] = num_wire;
		circuit_params[2] = n0; 
		circuit_params[3] = n0_0; 
		circuit_params[4] = n1; 
		circuit_params[5] = n1_0; 
		circuit_params[6] = n2; 
		circuit_params[7] = n2_0; 
		circuit_params[8] = n3;		
		
		fwrite((char*)circuit_params, 9*sizeof(int), 1, f);
		fwrite((char*)gates, num_gate*4*sizeof(int), 1, f);
		
		fclose(f);
	}
	
	int table_size() const{
		return num_gate*4;
	}

	void compute(block * out, block * in1, block * in2) {
		wires = new block[num_wire];
		memcpy(wires, in1, n1*sizeof(block));
		memcpy(wires+n1, in2, n2*sizeof(block));
		for(int i = 0; i < num_gate; ++i) {
			if(gates[4*i+3] == AND_GATE) {
				wires[gates[4*i+2]] = CircuitExecution::circ_exec->and_gate(wires[gates[4*i]], wires[gates[4*i+1]]);
			}
			else if (gates[4*i+3] == XOR_GATE) {
				wires[gates[4*i+2]] = CircuitExecution::circ_exec->xor_gate(wires[gates[4*i]], wires[gates[4*i+1]]);
			}
			else  
				wires[gates[4*i+2]] = CircuitExecution::circ_exec->not_gate(wires[gates[4*i]]);
		}
		memcpy(out, &wires[num_wire-n3], n3*sizeof(block));
	}
};
}
#endif// CIRCUIT_FILE
