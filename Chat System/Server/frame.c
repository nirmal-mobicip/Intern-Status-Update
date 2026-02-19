#include <stdint.h>

typedef struct frame
{
    int fin ;
    int rsv1 ;
    int rsv2 ;
    int rsv3 ;
    int opcode ;
    int mask ;
    uint64_t payload_len;
    unsigned char mask_key[4];
    char *payload_data;
} Frame;

Frame *parse_frame(unsigned char *buf, int n)
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

    if(res->mask){
        for(uint64_t i = 0;i<res->payload_len;i++){
            res->payload_data[i] ^= res->mask_key[i%4];
        }
    }

    return res;
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