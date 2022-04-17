#!/usr/bin/env python3
#
# Generate pre-crc'ed command payloads
#

import crccheck

sum_fn = crccheck.crc.Crc(width=8,
                          poly=0x07,
                          initvalue=0x00,
                          reflect_input=False,
                          reflect_output=False,
                          xor_output=0xe9,
                          check_result=0xf4)


def gen_pkt(label, payload):
    sum = sum_fn.calc(payload)
    pkt = b'\xc0\x08\x03' + payload + bytes([sum]) + b'\x01'
    print(f'{label}: {pkt.hex()}')


gen_pkt("off", b'\x00\x00\x0a\x00')
gen_pkt("on", b'\x03\x00\x0a\x00')
gen_pkt("set 1", b'\x03\x00\x01\x01')
gen_pkt("set 2", b'\x03\x00\x02\x01')
gen_pkt("set 3", b'\x03\x00\x03\x01')
gen_pkt("set 4", b'\x03\x00\x04\x01')
gen_pkt("set 5", b'\x03\x00\x05\x01')
gen_pkt("set 6", b'\x03\x00\x06\x01')
gen_pkt("set 7", b'\x03\x00\x07\x01')
gen_pkt("set 8", b'\x03\x00\x08\x01')
gen_pkt("set 9", b'\x03\x00\x09\x01')
gen_pkt("set 10", b'\x03\x00\x0a\x01')
