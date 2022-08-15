#ifndef _EQ_DATA_H

#define _EQ_DATA_H

#pragma pack(1)

struct eq_data {
	int length;
	unsigned char buffer[512];
	bool sequenced;
};

struct session_request {
	unsigned long UnknownA;
	unsigned long Session;
	unsigned short UnknownB;
	unsigned short MaxLength;
};

struct session_response {
        unsigned long Session;
	unsigned long Key;
	unsigned short UnknownA;
	unsigned short UnknownB;
	unsigned char UnknownC;
	unsigned long MaxLength;
};

#define OP_SessionInit		0x01
#define OP_SessionInitResp	0x02
#define OP_Disconnect		0x05
#define OP_KeepAlive		0x06
#define OP_Packet		0x09
#define OP_SequenceAckResp	0x15

#define SUBOP_ClientLogin	0x01
#define SUBOP_ClientCommand	0x02

#endif
