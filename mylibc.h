/* size_t is unsigned and can hold any non-negative integer, i.e. it can 
   hold very small and very large values. When we use this, the compiler 
   is free to choose any unsigned int that is big enough to hold this value
   but not bigger than what's needed */

size_t strlen(const char* str){
	size_t len = 0;
	while(str[len])
		len++;
	return len;
}