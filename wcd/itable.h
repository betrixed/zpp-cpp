#ifndef WCD_ITABLE_H
#define WCD_ITABLE_H

namespace wcd {

class ITable {
public:
	enum {
		ALL_TS = 3,
		UPDATE_TS = 2,
		CREATE_TS = 1,
		NO_TS = 0
	};

};

};

//itable.h
#endif