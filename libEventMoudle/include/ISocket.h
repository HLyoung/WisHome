

#ifndef _ISOCKET_H_
#define _ISOCKET_H_

class ISocket
{
public:
	virtual const CISocketOwner * getOwner() = 0;
	virtual void setOwner(CISocketOwner *serv_owner) = 0;
private:
    CISocketOwner *owner;  
};



#endif