#include <stdint.h>
#include <limits.h>

typedef struct frame
{
    unsigned int fin:1;
    unsigned int rsv1:1;
    unsigned int rsv2:1;
    unsigned int rsv3:1;
    unsigned int opcode:4;
    unsigned int mask:1;
    uint64_t payload_len;
    unsigned char mask_key[4];
    char *payload_data;
} Frame;

Frame *parse_frame(unsigned char *buf, int n, int* opcode)
{
    Frame *res = (Frame *)malloc(sizeof(Frame));

    // FIN
    res->fin = (buf[0] & 0x80) >> 7;
    // RSV1
    res->rsv1 = (buf[0] & 0x40) >> 6;
    // RSV2
    res->rsv2 = (buf[0] & 0x20) >> 5;
    // RSV3
    res->rsv3 = (buf[0] & 0x10) >> 4;
    // opcode
    res->opcode = (buf[0] & 0x0F);
    *opcode = res->opcode;
    // mask
    res->mask = (buf[1] & 0x80) >> 7;
    // payload_len
    res->payload_len = (buf[1] & 0x7F);

    char *next;

    if (res->payload_len <= 125)
    {
        next = &buf[2];
    }
    else if (res->payload_len == 126)
    {
        res->payload_len = (buf[2] << 8) | buf[3];
        next = &buf[4];
    }
    else if (res->payload_len == 127)
    {
        res->payload_len = 0;

        for (int i = 2; i <= 9; i++)
        {
            res->payload_len =
                (res->payload_len << 8) | buf[i];
        }
        next = &buf[10];
    }

    if (res->mask)
    { // if masked, next points to mask key
        memcpy(res->mask_key, next, 4);
        next += 4;
    }

    // copy payload
    res->payload_data = (char *)malloc(res->payload_len * sizeof(char));
    memcpy(res->payload_data, next, res->payload_len);

    if (res->mask)
    {
        for (uint64_t i = 0; i < res->payload_len; i++)
        {
            res->payload_data[i] ^= res->mask_key[i % 4];
        }
    }

    return res;
}

Frame* deep_copy(Frame* f){
    Frame* copy = (Frame*)malloc(sizeof(Frame));
    copy->fin = f->fin;
    copy->rsv1 = f->rsv1;
    copy->rsv2 = f->rsv2;
    copy->rsv3 = f->rsv3;
    copy->opcode = f->opcode;
    copy->mask = f->mask;
    copy->payload_len = f->payload_len;
    memcpy(copy->mask_key,f->mask_key,sizeof(f->mask_key));
    copy->payload_data = (char*)malloc(f->payload_len*sizeof(char));
    memcpy(copy->payload_data,f->payload_data,f->payload_len);
    return copy;
}

void printFrame(Frame f)
{
    printf("FIN : %d\n", f.fin);
    printf("RSV1 : %d\n", f.rsv1);
    printf("RSV2 : %d\n", f.rsv2);
    printf("RSV3 : %d\n", f.rsv3);
    printf("OPCODE : %d\n", f.opcode);
    printf("MASK : %d\n", f.mask);
    printf("PAYLOAD LENGTH : %d\n", (int)f.payload_len);
    printf("PAYLOAD DATA : %.*s\n", (int)f.payload_len, f.payload_data);
}

void createClientFrame(Frame *f, uint8_t *buffer, int *len)
{

    // BYTE 0
    buffer[0] = (f->fin & 1) << 7 | (f->rsv1 & 1) << 6 | (f->rsv2 & 1) << 5 | (f->rsv3 & 1) << 4 | (f->opcode & 0x0F);
    // BYTE 1
    int pos = 1;
    if(f->payload_len<=125){
        buffer[pos++] = f->payload_len;    // no need for mask bit as it is 0
    }else if(f->payload_len>=126 && f->payload_len<=65,535){
        buffer[pos++] = 126;
        buffer[pos++] = (f->payload_len>>8) & 0xFF;
        buffer[pos++] = f->payload_len & 0xFF;
    }else{
        buffer[1] = 127;
        for(int i=7;i>=0;i--){
            buffer[pos++] = (f->payload_len>>8*i) && 0xFF;
        }
    }

    memcpy(&buffer[pos],f->payload_data,f->payload_len);
    *len = pos+f->payload_len;
    return;
}