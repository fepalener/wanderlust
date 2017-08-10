#ifndef _CommandQueue_h_
#define _CommandQueue_h_

#include "Command.h"

#include <queue>

class CommandQueue
{
public:
	void					push(const Command& command);
	Command					pop();
	bool					isEmpty() const;
	
private:
	std::queue<Command>		mQueue;
};

#endif