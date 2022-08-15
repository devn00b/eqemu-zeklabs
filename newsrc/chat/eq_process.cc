void EQIRC_Connection::EQ_Decode(unsigned char *buffer, int size, int DecodeKey)
{
	int Key=DecodeKey;
	char *test=(char*)malloc(size);
        int i;
	for (i = 0 ; i+4 <= size ; i+=4)
	{
		int pt = (*(int*)&buffer[i])^(Key);
		Key = (*(int*)&buffer[i]);
		*(int*)&test[i]=pt;
	}
	unsigned char KC=Key&0xFF;
	for ( i ; i < size ; i++)
	{
		test[i]=buffer[i]^KC;
	}
	memcpy(&buffer[0],&test[0],size);	
	free(test);
}

int EQIRC_Connection::EQ_Process(char *data)
{
data out_data;
	if (data[0]==0x00) {
		switch(data[1]) {
			case OP_SessionInit: {
				sesreq=(session_request *)(data+2);
				Session=ntohl(sesreq->Session);
				MaxLen=ntohs(sesreq->MaxLength);

				out_data.buffer[1]=OP_SessionInitResp;
				sesres=(session_response *)(&out_data.buffer[2]);
				sesres->Session=htonl(Session);
				sesres->Key=htonl(key);
				sesres->MaxLength=htons(maxlen);
				out_data.length=sizeof(session_response)+2;

				EQData.push_back(out_data);
			}
			case OP_SequenceAckResp: { //0x15
				break;
			}
				
			case OP_Packet: { //0x09
				short Seq=htons(*(unsigned short *)(data+2));
				if (Seq>=NextInSeq) {
					if (Seq>NextInSeq) {
						printf("Client is a head of us! We expected %d, but got %d.  Adjusting\n",NextInSeq,Seq);
						NextInSeq=Seq;
					}
					EQ_Process_SubOP(data+4);
					NextInSeq++;
				} else if (Seq<NextInSeq) {
					printf("Duplicate paacket #%d\n",Seq);
				}
			}
		}
	} else {
		cout << "Got Message without leading 0x00" << endl;
	}

	return 1;
}

void EQIRC_Connection::EQ_Process_SubOP(char *data)
{
	switch(data[0]) {
		case OP_Packet_ClientLogin: { //0x001
			char *server, char *name, char *pass, char *lang;
			server=(data+2);
			name=strchr(server,'.');
			*(name++)=0;
			pass=name+strlen(name)+1;
			lang=pass+strlen(pass)+1;
			*(lang+2)=0;
			Me.Init(server,name,pass,lang);
			printf("Connecting user: %s,%s/%s using lang %s\n",server,name,pass,lang);

			out_data.buffer[1]=0x03;
			out_data.buffer[6]=(unsigned char)NextInSeq;
			out_data.buffer[7]=0x06; // Length;
			out_data.buffer[8]=0x00;
			out_data.buffer[9]=0x09;
			out_data.buffer[10]=0x00;
			out_data.buffer[11]=0x00;
			out_data.buffer[12]=0x01;
			out_data.buffer[13]=0x01;
			out_data.touched=true;
			out_data.length=14;

			EQData.push_back(out_data);
			irc_out_buffer+="PASS joe\r\n";
			irc_out_buffer+="NICK [";
			irc_out_buffer+=server;
			irc_out_buffer+="]";
			irc_out_buffer+=name;
			irc_out_buffer+="\r\n";
			irc_out_buffer+="USER ";
			irc_out_buffer+=name;
			irc_out_buffer+=" 0 *";
			irc_out_buffer+=name;
			irc_out_buffer+=" 0 *\r\n";

			// Do IRC Login
			break;
		}
		case OP_Packet_ClientCommand: { //0x02
			handle_EQ_command(data+1);
			break;
		}
	}
}
