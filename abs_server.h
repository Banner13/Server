// jackZ

#pragma once

class Server {
public:

	virtual ~Server() = default;


	virtual int Init() = 0;
    virtual int Start() = 0;
    virtual int Reboot() = 0;
    virtual int Stop() = 0;
    virtual int DeInit() = 0;
    
protected:
	Server() = default;
};

