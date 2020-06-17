template<class... Ts>
void run_function(void *function, const Ts&... args) {	
	reinterpret_cast<void(*)(Ts...)>(function)(args...);
}

template<typename T>
void inline delete_array_null(T * ptr){
	if(ptr != nullptr) {
		delete[] ptr;
		ptr = nullptr;
	}
}

template <typename T>
std::string m128i_to_string(const __m128i var) {
	std::stringstream sstr;
	const T* values = (const T*) &var;
	for (unsigned int i = 0; i < sizeof(__m128i) / sizeof(T); i++) {
		sstr <<"0x"<<std::hex<< values[i] << " ";
	}
	return sstr.str();
}

inline time_point<high_resolution_clock> clock_start() { 
	return high_resolution_clock::now();
}

inline double time_from(const time_point<high_resolution_clock>& s) {
	return std::chrono::duration_cast<std::chrono::microseconds>(high_resolution_clock::now() - s).count();
}

/*This migh be redundant, but at this point, it is easier to keep both timers*/  
class Timer{
	uint64_t c0;
	double t0;
	struct timeval tp;
	
	public:
  
	void start(){
		c0 = RDTSC;
		gettimeofday(&tp, NULL);
		t0 = ((double)(tp.tv_sec))*1000 + ((double)(tp.tv_usec))/1000;
	}
  
	void get(uint64_t &dc, double &dt){
		dc = RDTSC - c0;
		gettimeofday(&tp, NULL);
		dt = ((double)(tp.tv_sec))*1000 + ((double)(tp.tv_usec))/1000 - t0;
	}
  
	void print(string message){
		uint64_t dc;
		double dt;
		get(dc, dt);
		std::cout << message << ": ";
		std::cout << std::setprecision(2) << std::scientific;
		std::cout << (double)dc << "cc, " << std::fixed;
		
		uint64_t miliseconds	= (uint64_t)dt % 1000 ;
		uint64_t seconds 		= ((uint64_t)dt / 1000) % 60 ;
		uint64_t minutes 		= (((uint64_t)dt / (1000*60)) % 60);
		uint64_t hours   		= (((uint64_t)dt / (1000*60*60)) % 24);
		
		if (hours) std::cout << hours << ":";
		if (hours||minutes) std::cout << minutes << ":";
		if (hours||minutes||seconds) std::cout << seconds << ":";
		std::cout << miliseconds << "ms" << std::endl;
	}	
};

class memMeter{	
	struct rusage usage;
	double memory_usage;
public: 
	/*https://stackoverflow.com/questions/669438/how-to-get-memory-usage-at-runtime-using-c
	https://elinux.org/Runtime_Memory_Measurement*/
	void process_mem_usage(double& resident_set, double& resident_set_max){		
		getrusage(RUSAGE_SELF, &usage);
		resident_set_max = (double)usage.ru_maxrss/1024;
		
		using std::ios_base;
		using std::ifstream;
		using std::string;		
		
		long program_size, resident_set_size, shared_pages, text, stack , library , dirty_pages; 
		ifstream stat_stream("/proc/self/statm",ios_base::in);		
		stat_stream >> program_size >> resident_set_size >> shared_pages >> text >> stack  >> library  >> dirty_pages;		
		stat_stream.close();
		
		long page_size = sysconf(_SC_PAGE_SIZE); // in Bytes
		resident_set = (double)(resident_set_size*page_size)/1024/1024;		
	}
	
	void print(string message){
		double resident_set, resident_set_max;
		process_mem_usage(resident_set, resident_set_max);
		
		std::cout << message << ": ";
		if(resident_set < 1024) std::cout << resident_set << "MB, " << resident_set_max << "MB" << std::endl;
		else std::cout << resident_set/1024 << "GB, " << resident_set_max/1024 << "GB" << std::endl;
	}
};

inline string towsComplement(string num){
	string rnum(num);
	size_t bit_len = num.length();	
	bool invert = false;
	for (int64_t i = bit_len-1; i >= 0; i--){
		if(invert){
			if (num[i] == '1') rnum[i] = '0';
			else rnum[i] = '1';
		}
		if(num[i] == '1') invert = true;
	}	
	return rnum;
}

inline string bin_to_dec(const string& bin2) {
	if(bin2[0] == '0')
		return change_base(bin2, 2, 10);
	string bin = bin2;
	bool flip = false;
	for(int i = bin.size()-1; i>=0; --i) {
		if(flip)
			bin[i] = (bin[i] == '1' ? '0': '1');
		if(bin[i] == '1')
			flip = true;
	}
	return "-"+change_base(bin, 2, 10);
}

inline int64_t bin_to_dec(string bin, bool is_signed){
	bool neg = false;
	if (is_signed && (bin[0] == '1')) neg = true;
	if (neg) bin = towsComplement(bin);	
	int64_t dec = (int64_t)(std::bitset<64>(bin).to_ulong());
	if (neg) dec = -dec;	
    return dec;
}

inline string dec_to_bin(const string& dec) {
	string bin = change_base(dec, 10, 2);
	if(dec[0] != '-')
		return '0' + bin;
	bin[0] = '1';
	bool flip = false;
	for(int i = bin.size()-1; i>=1; --i) {
		if(flip)
			bin[i] = (bin[i] == '1' ? '0': '1');
		if(bin[i] == '1')
			flip = true;
	}
	return bin;
}

inline string dec_to_bin(int64_t dec, uint64_t bit_len){
	string bin = std::bitset<64>(dec).to_string(); 
    bin = bin.substr(64-bit_len, bit_len);	
	return bin;
}

inline string change_base(string str, int old_base, int new_base) {
	mpz_t tmp;
	mpz_init_set_str (tmp, str.c_str(), old_base);
	char * b = new char[mpz_sizeinbase(tmp, new_base) + 2];
	mpz_get_str(b, new_base, tmp);
	mpz_clear(tmp);
	string res(b);
	delete[]b;
	return res;
}

inline void error(const char * s, int line, const char * file) {
	fprintf(stderr, s, "\n");
	if(file != nullptr) {
		fprintf(stderr, "at %d, %s\n", line, file);
	}
	exit(1);
}

inline std::string hex_to_binary(std::string hex_){
	uint64_t len = hex_.length();
	string bin("");
	
	for (uint64_t i = 0; i < len; i++){
		char H = hex_.at(i);
		string B;
		if(H == '0') B = "0000";
		else if(H == '1') B = "0001";
		else if(H == '2') B = "0010";
		else if(H == '3') B = "0011";
		else if(H == '4') B = "0100";
		else if(H == '5') B = "0101";
		else if(H == '6') B = "0110";
		else if(H == '7') B = "0111";
		else if(H == '8') B = "1000";
		else if(H == '9') B = "1001";
		else if((H == 'a')||(H == 'A')) B = "1010";
		else if((H == 'b')||(H == 'B')) B = "1011";
		else if((H == 'c')||(H == 'C')) B = "1100";
		else if((H == 'd')||(H == 'D')) B = "1101";
		else if((H == 'e')||(H == 'E')) B = "1110";
		else if((H == 'f')||(H == 'F')) B = "1111";
		bin = bin + B;
	}
	return bin;
}

inline std::string binary_to_hex(std::string bin){
	uint64_t len = bin.length();
	if (len%4) bin.insert(0, 4-len%4, '0');
	len = bin.length();
	string hex_("");
	
	for (uint64_t i = 0; i < len; i+=4){
		string B = bin.substr(i, 4);
		string H;
		if(B == "0000") H = "0";
		else if(B == "0001") H = "1";
		else if(B == "0010") H = "2";
		else if(B == "0011") H = "3";
		else if(B == "0100") H = "4";
		else if(B == "0101") H = "5";
		else if(B == "0110") H = "6";
		else if(B == "0111") H = "7";
		else if(B == "1000") H = "8";
		else if(B == "1001") H = "9";
		else if(B == "1010") H = "A";
		else if(B == "1011") H = "B";
		else if(B == "1100") H = "C";
		else if(B == "1101") H = "D";
		else if(B == "1110") H = "E";
		else if(B == "1111") H = "F";
		hex_ = hex_ + H;
	}
	return hex_;
}

inline void parse_party_and_port(char ** arg, int argc, int * party, int * port) {
	if (argc == 1)
		error("ERROR: argc = 1, need two argsm party ID {1,2} and port.");
	*party = atoi (arg[1]);
	*port = atoi (arg[2]);
}

inline std::string Party(int p) {
	if (p == ALICE)	
		return "ALICE";
	else if (p == BOB)
		return "BOB";
	else return "PUBLIC";
}

template<typename t>
t bool_to_int(const bool * data, size_t len) {
	if (len != 0) len = (len > sizeof(t)*8 ? sizeof(t)*8 : len);
	else len = sizeof(t)*8;
	t res = 0;
	for(size_t i = 0; i < len-1; ++i) {
		if(data[i])
			res |= (1LL<<i);
	}
	if(data[len-1]) return -1*res;
	else return res;
}

inline uint64_t bool_to64(const bool * data) {
	uint64_t res = 0;
	for(int i = 0; i < 64; ++i) {
		if(data[i])
			res |= (1ULL<<i);
	}
	return res;
}
inline block bool_to128(const bool * data) {
	return makeBlock(bool_to64(data+64), bool_to64(data));
}

inline void int64_to_bool(bool * data, uint64_t input, int length) {
	for (int i = 0; i < length; ++i) {
		data[i] = (input & 1)==1;
		input >>= 1;
	}
}

inline void printBlock(block var){
    uint16_t *val = (uint16_t*) &var;
    std::cout << boost::format{"%04X%04X%04X%04X%04X%04X%04X%04X"}%val[7]%val[6]%val[5]%val[4]%val[3]%val[2]%val[1]%val[0] << std::endl;
}
