// the value of far_data is an address
// of course far_data itself (the location in memory containing the address) has
// an address
char *far_data = "apple";

// In memory it may be layed out like this:

// Address Label      Value
// 0x123:  far_data = 0x456
// ...
// 0x456:  string   = 'a'

// if PC was at 0x100
// LDI R0 0x023
// would load 'a' into R0
