#ifndef INPUTOUTPUT_H__
#define INPUTOUTPUT_H__
#include <string>
#include <algorithm>

using namespace emp;

class InputOutput{
	string input_bin_str, init_bin_str, output_bin_str;
	int input_bit_width, input_bit_width_1, init_bit_width, init_bit_width_1, output_bit_width, cid, cycles;
	
	public:
	
	InputOutput(int dummy){
		output_bin_str = "";
	}
	
	void init(string input_hex_str, int input_bit_width, int output_bit_width, string init_hex_str = "", int init_bit_width = 0, int cycles = 1){
		this->input_bit_width = input_bit_width;
		this->init_bit_width = init_bit_width;
		this->output_bit_width = output_bit_width;
		this->cycles = cycles;
		
		input_bin_str = hex_to_binary(input_hex_str);
		std::reverse(input_bin_str.begin(), input_bin_str.end()); 
		input_bit_width_1 = input_bin_str.length();
		
		init_bin_str = hex_to_binary(init_hex_str);
		std::reverse(init_bin_str.begin(), init_bin_str.end());  
		init_bit_width_1 = init_bin_str.length();	
		
		cid = 0;
	}
	
	void fill_input(bool *in){	
		if(cid%cycles == 0){
			for (int i = 0; i < init_bit_width; ++i){
				int index = i + (cid/cycles)*init_bit_width;
				if(index < init_bit_width_1) in[i] = (init_bin_str[index] == '1')? true:false;
				else in[i] = false;
			}
		}
		
		/*input_hex_str (therefore input_bin_str) does not include init_hex_str, but input_bit_width is the total number of bits in input including init*/
		for (int i = init_bit_width; i < input_bit_width; ++i){
			int index = cid*(input_bit_width-init_bit_width)+i-init_bit_width;
			if(index < input_bit_width_1) in[i] = (input_bin_str[index] == '1')? true:false; 
			else in[i] = false;
		}
		
		cid++;
	}
	
	void fill_output(bool *out){
		for(int i = 0; i < output_bit_width; ++i)
			output_bin_str += (out[i]?"1":"0");		
	}

	string read_output(){	
		std::reverse(output_bin_str.begin(), output_bin_str.end());
		string output_hex_str = binary_to_hex(output_bin_str);		
		return output_hex_str;	
	}
};

string formatGCInputString(vector<int64_t> input, uint64_t bit_len){
	string bin_input_str, input_str;
	for (uint64_t i = 0; i < input.size(); i++){
		bin_input_str = bin_input_str + dec_to_bin(input[i], bit_len);
	}
	input_str = binary_to_hex(bin_input_str);
	return input_str;
}

string formatGCInputString(vector<int64_t> input, vector<uint64_t> bit_len){
	string bin_input_str, input_str;
	for (uint64_t i = 0; i < input.size(); i++){
		bin_input_str = bin_input_str + dec_to_bin(input[i], bit_len[i]);
	}
	input_str = binary_to_hex(bin_input_str);
	return input_str;
}

void parseGCOutputString(vector<int64_t> &output, string output_str, uint64_t bit_len, uint64_t offset, bool is_signed){
	string bin_output_str = hex_to_binary(output_str);
	uint64_t no_of_outputs = ceil(bin_output_str.size()/bit_len);
	output.resize(no_of_outputs);
	uint64_t len = offset;
	uint64_t i;
	for (i = 0; i < no_of_outputs; i++){
		len += bit_len;
	}
	if (len > bin_output_str.length()) bin_output_str.insert(0, len-bin_output_str.length(), '0');
	else if (len < bin_output_str.length()) bin_output_str = bin_output_str.substr(bin_output_str.length()-len, len);
	
	uint64_t cur = 0;
	for (i = 0; i < no_of_outputs; i++){
		output[i] = bin_to_dec(bin_output_str.substr(cur, bit_len), is_signed);
		cur += bit_len;
	}	
}

void parseGCOutputString(vector<int64_t> &output, string output_str, vector<uint64_t> bit_len, uint64_t offset, bool is_signed){
	string bin_output_str = hex_to_binary(output_str);
	uint64_t no_of_outputs = bit_len.size();
	output.resize(no_of_outputs);
	uint64_t len = offset;
	uint64_t i;
	for (i = 0; i < no_of_outputs; i++){
		len += bit_len[i];
	}
	if (len > bin_output_str.length()) bin_output_str.insert(0, len-bin_output_str.length(), '0');
	else if (len < bin_output_str.length()) bin_output_str = bin_output_str.substr(bin_output_str.length()-len, len);
	
	uint64_t cur = 0;
	for (i = 0; i < no_of_outputs; i++){
		output[i] = bin_to_dec(bin_output_str.substr(cur, bit_len[i]), is_signed);
		cur += bit_len[i];
	}	
}

#endif// INPUTOUTPUT_H__