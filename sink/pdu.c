#include "pdu.h"
#include "route.h"

/*  
    Function: parse received pdu
    Parameter:  pf:         pointer to pdu_format
                recvbuf:    pointer to received buffer 
    Return:     RETVAL_PDU_FORMAT_VALID:    valid pdu format
                RETVAL_PDU_FORMAT_INVALID:  invalid pdu format
*/
int parse_pdu(pdu_format* pf, char* recvbuf){
    /*initialize pf*/
    memset(pf->des_ip_num, 0, sizeof(pf->des_ip_num));
    memset(pf->des_ip, 0, sizeof(pf->des_ip));
    memset(pf->source_ip_num, 0, sizeof(pf->source_ip_num));
    memset(pf->source_ip, 0, sizeof(pf->source_ip));
    pf->length = 0;
    pf->type = 0;

    /*get source and destination ip address*/
    for(int i=0; i<4; i++){
        pf->source_ip_num[i] = recvbuf[i];
        pf->des_ip_num[i] = recvbuf[4+i];
    }
    /*cast source and destination ip from number to string*/
    IP32ToIPchar(pf->source_ip_num, pf->source_ip);
    IP32ToIPchar(pf->des_ip_num, pf->des_ip);
    fprintf(stdout, "PDU parse info: source_ip: %s\n", pf->source_ip);
    fprintf(stdout, "PDU parse info: des_ip: %s\n", pf->des_ip);

    /*get length*/
    pf->length = (unsigned char)recvbuf[8];
    if(pf->length == 0){
        fprintf(stderr, "PDU parse warning: the length field of PDU is zero!\n");
        return RETVAL_PDU_FORMAT_INVALID;
    }else
        fprintf(stdout, "PDU parse info: length: %d\n", pf->length);

    /*get type*/
    unsigned char temp_type = (unsigned char)recvbuf[9];
    pf->type = ((temp_type&0xf0)>>4);
    if(pf->type == PDU_TYPE_PERIODICALLY)
        fprintf(stdout, "PDU parse info: type: PDU_TYPE_PERIODICALLY\n");
    else if(pf->type == PDU_TYPE_M2M)
        fprintf(stdout, "PDU parse info: type: PDU_TYPE_M2M\n");
    else if(pf->type == PDU_TYPE_APPLICATION)
        fprintf(stdout, "PDU parse info: type: PDU_TYPE_APPLICATION\n");
    else{
        fprintf(stderr, "PDU parse warning: unknown pdu type: %d\n", pf->type);
        return RETVAL_PDU_FORMAT_INVALID;
    }

    /*get payload*/
    char* payload = (char*)malloc(sizeof(char)*(strlen(recvbuf)+1-PDU_HEADER_LENGTH));
    if(payload == NULL){
        fprintf(stderr, "Memory warning: Can't allocate memory when parse_pdu!\n");
        return RETVAL_PDU_FORMAT_ERROR;
    }
    memcpy(payload, recvbuf+sizeof(char)*PDU_HEADER_LENGTH, (strlen(recvbuf)+1-PDU_HEADER_LENGTH));
    pf->payload = payload;
    
    return RETVAL_PDU_FORMAT_VALID;
}

/*
    Function: make sendbuf
    Parameter:  
                sendbuf:                pointer to sendbuf
                source_ip:              source IP address (string format)
                des_ip:                 destination IP address (string format)
                length:                 length of pdu
                type:                   type of pdu
                payload_buf:            pointer to payload buf
                payload_buf_length:     length of payload buffer
    Return:     RETVAL_MAKE_SENDBUF_SUCCESSFULLY:   successfully make sendbuf
                RETVAL_MAKE_SENDBUF_FAILED:         failed to make sendbuf
*/
int make_sendbuf(char* sendbuf, char* source_ip, char* des_ip, char length, char type, char* payload_buf, int payload_buf_length){
    /*check whether all parameters are vaild*/
    if(strlen(source_ip) == 0 || strlen(des_ip) == 0){
        fprintf(stderr, "Make sendbuf warning: invaild source ip / destination ip field!\n");
        fprintf(stderr, "Make sendbuf warning: source ip: %s\n", source_ip);
        fprintf(stderr, "Make sendbuf warning: des ip: %s\n", des_ip);
        return RETVAL_MAKE_SENDBUF_FAILED;
    }
    else if(length <= 0){
        fprintf(stderr, "Make sendbuf warning: invaild length field: %d\n", length);
        return RETVAL_MAKE_SENDBUF_FAILED;
    }
    else if((type != PDU_TYPE_PERIODICALLY) && (type != PDU_TYPE_M2M) && (type != PDU_TYPE_APPLICATION)){
        fprintf(stderr, "Make sendbuf warning: invaild type field!: %d\n", type);
        return RETVAL_MAKE_SENDBUF_FAILED;
    }

    /*make source and destination ip address*/
    char source_ip_num[4], des_ip_num[4];
    IPcharToIP32(source_ip, source_ip_num);
    IPcharToIP32(des_ip, des_ip_num);
    for(int i=0; i<4; i++){
        sendbuf[i] = source_ip_num[i];
        sendbuf[i+4] = des_ip_num[i];
    }

    /*make length*/
    sendbuf[8] = length;

    /*make type*/
    sendbuf[9] = ((type<<4) & 0xf0);
    if(sendbuf[9] == 0)
        sendbuf[9] = 0x0f;
    
    /*make payload*/
    for(int i=0; i<payload_buf_length; i++){
        sendbuf[10+i] = payload_buf[i];
    }

    return RETVAL_MAKE_SENDBUF_SUCCESSFULLY;
}

/*  
    Function: parse resource pdu
    Parameter:  rpf:        pointer to resource_pdu_format
                payload:    pointer to payload of pdu_format
    Return:     RETVAL_RESOURCE_PDU_FORMAT_VALID:    valid resource pdu format
                RETVAL_RESOURCE_PDU_FORMAT_INVALID:  invalid resource pdu format
*/
int parse_resource_pdu(resource_pdu_format* rpf, char* payload)
{
    /*extract terminal ID*/ 
    rpf->termID = payload[0];
    
    /*extract sensor type*/ 
    rpf->sen_type = payload[1];
    
    /*extract executor type*/ 
    rpf->execute_type = payload[2];

    /*copy resource payload*/
    char* resource_payload = (char*)malloc(sizeof(char)*RESOURCE_PAYLOAD_BUF_SIZE);
    if(resource_payload == NULL){
        fprintf(stdout, "Memory warning: Can't allocate memory when data_frame_analyse_function!\n");
        return RETVAL_RESOURCE_PDU_FORMAT_ERROR;
    }
    strcpy(resource_payload, payload+sizeof(char)*3);
	
    rpf->resource_payload = resource_payload;

    /*printf info*/
    fprintf(stdout, "Resource PDU parse info: terminalID: %d\n", rpf->termID);
	fprintf(stdout, "Resource PDU parse info: sensor_type: %d\n", rpf->sen_type);
    fprintf(stdout, "Resource PDU parse info: execute_type: %d\n", rpf->execute_type);
	fprintf(stdout, "Resource PDU parse info: char data: %s\n", rpf->resource_payload);
    fprintf(stdout, "Resource PDU parse info: int data: %d\n", char_to_int(rpf->resource_payload));

    return RETVAL_RESOURCE_PDU_FORMAT_VALID;
}

/*  
    Function: convert a ip string into four byte unsigned char
    Parameter:  ip:     ip string
                ip32:   buffer that stores four byte unsigned char
    Return:     none
*/
void IPcharToIP32(char* ip, u8* ip32)
{
	u8 i, j, ipInt, idx = 0;
	u8 ipIndex = 0;
	char temp[4];
	for(i=0; i<=strlen(ip); i++){
		if(ip[i] == '.' || ip[i] == '\0'){
			for(j=0; ipIndex<i; j++){
				temp[j] = ip[ipIndex];
				ipIndex++;
			}
			temp[j] = '\0';
			ipIndex++;
			ipInt = atoi(temp);	
			ip32[idx] = ipInt;
			idx++;
		}
	}
}

/*  
    Function: convert a four byte unsigned char into ip string
    Parameter:  ip32: buffer that stores four byte unsigned char
                ip: ip string
    Return: none
*/
void IP32ToIPchar(u8* ip32, char* ip)
{
	u8 i,j;
	char temp[10];
	char* idx = ip;
	for(i=0; i<3; i++){
		Int_To_Str(ip32[i], temp);
		for(j=0; j<strlen(temp); j++){
			*idx = temp[j];
			idx++;
		}
		*idx = '.';
		idx++;
	}
	Int_To_Str(ip32[3], temp);
	for(j=0; j<strlen(temp); j++){
		*idx = temp[j];
		idx++;
	}
	*idx = '\0';
}

void Int_To_Str(int x,char *Str)
{
    int t;
    char *Ptr,Buf[5];
    int i = 0;              
    Ptr = Str;           
    if(x < 10){
        *Ptr ++ = x+0x30;
    }
    else{
        while(x > 0){
            t = x % 10;
            x = x / 10;
            Buf[i++] = t+0x30; 
        }
        i--;
        for(;i >= 0;i --)  {
        *(Ptr++) = Buf[i];
        }
    }
    *Ptr = '\0';
}

/*
 * 	Function: char_to_int
 * 	Parameter In:		
 *	*data: char type data 
 * 	Parameter Out:int type data
 */
int char_to_int(uint8_t *data)
{
    int i, j = 0, k = 1;
    for (i = 0; data[i]; i++)
        data[i] -= '0';
    i--;
    for (; i >= 0; i--)
    {
        j += data[i] * k;
        k *= 10;
    }
    return j;
}