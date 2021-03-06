#include <fb/font.h>
static const uint8_t font_5x8_data[] = {
/* --- new character space (32) starting at offset 0x0000 --- */
	/*0000:*/	5, 5, 1, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*0005:*/	0x00,  /* ........ */
/* --- new character exclam (33) starting at offset 0x0006 --- */
	/*0006:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*000b:*/	0x20,  /* ..#..... */
	/*000c:*/	0x20,  /* ..#..... */
	/*000d:*/	0x20,  /* ..#..... */
	/*000e:*/	0x20,  /* ..#..... */
	/*000f:*/	0x00,  /* ........ */
	/*0010:*/	0x20,  /* ..#..... */
/* --- new character quotedbl (34) starting at offset 0x0011 --- */
	/*0011:*/	5, 5, 3, 0, 3, /* width and bbox (w,h,x,y) */ 
	/*0016:*/	0x50,  /* .#.#.... */
	/*0017:*/	0x50,  /* .#.#.... */
	/*0018:*/	0x50,  /* .#.#.... */
/* --- new character numbersign (35) starting at offset 0x0019 --- */
	/*0019:*/	5, 5, 7, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*001e:*/	0x50,  /* .#.#.... */
	/*001f:*/	0x50,  /* .#.#.... */
	/*0020:*/	0xf8,  /* #####... */
	/*0021:*/	0x50,  /* .#.#.... */
	/*0022:*/	0xf8,  /* #####... */
	/*0023:*/	0x50,  /* .#.#.... */
	/*0024:*/	0x50,  /* .#.#.... */
/* --- new character dollar (36) starting at offset 0x0025 --- */
	/*0025:*/	5, 5, 7, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*002a:*/	0x20,  /* ..#..... */
	/*002b:*/	0x70,  /* .###.... */
	/*002c:*/	0xa0,  /* #.#..... */
	/*002d:*/	0x70,  /* .###.... */
	/*002e:*/	0x28,  /* ..#.#... */
	/*002f:*/	0x70,  /* .###.... */
	/*0030:*/	0x20,  /* ..#..... */
/* --- new character percent (37) starting at offset 0x0031 --- */
	/*0031:*/	5, 5, 5, 0, 1, /* width and bbox (w,h,x,y) */ 
	/*0036:*/	0x40,  /* .#...... */
	/*0037:*/	0x50,  /* .#.#.... */
	/*0038:*/	0x20,  /* ..#..... */
	/*0039:*/	0x50,  /* .#.#.... */
	/*003a:*/	0x10,  /* ...#.... */
/* --- new character ampersand (38) starting at offset 0x003b --- */
	/*003b:*/	5, 5, 7, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0040:*/	0x40,  /* .#...... */
	/*0041:*/	0xa0,  /* #.#..... */
	/*0042:*/	0xa0,  /* #.#..... */
	/*0043:*/	0x40,  /* .#...... */
	/*0044:*/	0xa0,  /* #.#..... */
	/*0045:*/	0xa0,  /* #.#..... */
	/*0046:*/	0x50,  /* .#.#.... */
/* --- new character quotesingle (39) starting at offset 0x0047 --- */
	/*0047:*/	5, 5, 3, 0, 3, /* width and bbox (w,h,x,y) */ 
	/*004c:*/	0x20,  /* ..#..... */
	/*004d:*/	0x20,  /* ..#..... */
	/*004e:*/	0x20,  /* ..#..... */
/* --- new character parenleft (40) starting at offset 0x004f --- */
	/*004f:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0054:*/	0x20,  /* ..#..... */
	/*0055:*/	0x40,  /* .#...... */
	/*0056:*/	0x40,  /* .#...... */
	/*0057:*/	0x40,  /* .#...... */
	/*0058:*/	0x40,  /* .#...... */
	/*0059:*/	0x20,  /* ..#..... */
/* --- new character parenright (41) starting at offset 0x005a --- */
	/*005a:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*005f:*/	0x40,  /* .#...... */
	/*0060:*/	0x20,  /* ..#..... */
	/*0061:*/	0x20,  /* ..#..... */
	/*0062:*/	0x20,  /* ..#..... */
	/*0063:*/	0x20,  /* ..#..... */
	/*0064:*/	0x40,  /* .#...... */
/* --- new character asterisk (42) starting at offset 0x0065 --- */
	/*0065:*/	5, 5, 5, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*006a:*/	0x90,  /* #..#.... */
	/*006b:*/	0x60,  /* .##..... */
	/*006c:*/	0xf0,  /* ####.... */
	/*006d:*/	0x60,  /* .##..... */
	/*006e:*/	0x90,  /* #..#.... */
/* --- new character plus (43) starting at offset 0x006f --- */
	/*006f:*/	5, 5, 5, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0074:*/	0x20,  /* ..#..... */
	/*0075:*/	0x20,  /* ..#..... */
	/*0076:*/	0xf8,  /* #####... */
	/*0077:*/	0x20,  /* ..#..... */
	/*0078:*/	0x20,  /* ..#..... */
/* --- new character comma (44) starting at offset 0x0079 --- */
	/*0079:*/	5, 5, 3, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*007e:*/	0x30,  /* ..##.... */
	/*007f:*/	0x20,  /* ..#..... */
	/*0080:*/	0x40,  /* .#...... */
/* --- new character hyphen (45) starting at offset 0x0081 --- */
	/*0081:*/	5, 5, 1, 0, 2, /* width and bbox (w,h,x,y) */ 
	/*0086:*/	0x70,  /* .###.... */
/* --- new character period (46) starting at offset 0x0087 --- */
	/*0087:*/	5, 5, 3, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*008c:*/	0x20,  /* ..#..... */
	/*008d:*/	0x70,  /* .###.... */
	/*008e:*/	0x20,  /* ..#..... */
/* --- new character slash (47) starting at offset 0x008f --- */
	/*008f:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0094:*/	0x10,  /* ...#.... */
	/*0095:*/	0x10,  /* ...#.... */
	/*0096:*/	0x20,  /* ..#..... */
	/*0097:*/	0x40,  /* .#...... */
	/*0098:*/	0x80,  /* #....... */
	/*0099:*/	0x80,  /* #....... */
/* --- new character zero (48) starting at offset 0x009a --- */
	/*009a:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*009f:*/	0x20,  /* ..#..... */
	/*00a0:*/	0x50,  /* .#.#.... */
	/*00a1:*/	0x50,  /* .#.#.... */
	/*00a2:*/	0x50,  /* .#.#.... */
	/*00a3:*/	0x50,  /* .#.#.... */
	/*00a4:*/	0x20,  /* ..#..... */
/* --- new character one (49) starting at offset 0x00a5 --- */
	/*00a5:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*00aa:*/	0x20,  /* ..#..... */
	/*00ab:*/	0x60,  /* .##..... */
	/*00ac:*/	0x20,  /* ..#..... */
	/*00ad:*/	0x20,  /* ..#..... */
	/*00ae:*/	0x20,  /* ..#..... */
	/*00af:*/	0x70,  /* .###.... */
/* --- new character two (50) starting at offset 0x00b0 --- */
	/*00b0:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*00b5:*/	0x60,  /* .##..... */
	/*00b6:*/	0x90,  /* #..#.... */
	/*00b7:*/	0x10,  /* ...#.... */
	/*00b8:*/	0x60,  /* .##..... */
	/*00b9:*/	0x80,  /* #....... */
	/*00ba:*/	0xf0,  /* ####.... */
/* --- new character three (51) starting at offset 0x00bb --- */
	/*00bb:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*00c0:*/	0xf0,  /* ####.... */
	/*00c1:*/	0x20,  /* ..#..... */
	/*00c2:*/	0x60,  /* .##..... */
	/*00c3:*/	0x10,  /* ...#.... */
	/*00c4:*/	0x90,  /* #..#.... */
	/*00c5:*/	0x60,  /* .##..... */
/* --- new character four (52) starting at offset 0x00c6 --- */
	/*00c6:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*00cb:*/	0x20,  /* ..#..... */
	/*00cc:*/	0x60,  /* .##..... */
	/*00cd:*/	0xa0,  /* #.#..... */
	/*00ce:*/	0xf0,  /* ####.... */
	/*00cf:*/	0x20,  /* ..#..... */
	/*00d0:*/	0x20,  /* ..#..... */
/* --- new character five (53) starting at offset 0x00d1 --- */
	/*00d1:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*00d6:*/	0xf0,  /* ####.... */
	/*00d7:*/	0x80,  /* #....... */
	/*00d8:*/	0xe0,  /* ###..... */
	/*00d9:*/	0x10,  /* ...#.... */
	/*00da:*/	0x90,  /* #..#.... */
	/*00db:*/	0x60,  /* .##..... */
/* --- new character six (54) starting at offset 0x00dc --- */
	/*00dc:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*00e1:*/	0x60,  /* .##..... */
	/*00e2:*/	0x80,  /* #....... */
	/*00e3:*/	0xe0,  /* ###..... */
	/*00e4:*/	0x90,  /* #..#.... */
	/*00e5:*/	0x90,  /* #..#.... */
	/*00e6:*/	0x60,  /* .##..... */
/* --- new character seven (55) starting at offset 0x00e7 --- */
	/*00e7:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*00ec:*/	0xf0,  /* ####.... */
	/*00ed:*/	0x10,  /* ...#.... */
	/*00ee:*/	0x20,  /* ..#..... */
	/*00ef:*/	0x20,  /* ..#..... */
	/*00f0:*/	0x40,  /* .#...... */
	/*00f1:*/	0x40,  /* .#...... */
/* --- new character eight (56) starting at offset 0x00f2 --- */
	/*00f2:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*00f7:*/	0x60,  /* .##..... */
	/*00f8:*/	0x90,  /* #..#.... */
	/*00f9:*/	0x60,  /* .##..... */
	/*00fa:*/	0x90,  /* #..#.... */
	/*00fb:*/	0x90,  /* #..#.... */
	/*00fc:*/	0x60,  /* .##..... */
/* --- new character nine (57) starting at offset 0x00fd --- */
	/*00fd:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0102:*/	0x60,  /* .##..... */
	/*0103:*/	0x90,  /* #..#.... */
	/*0104:*/	0x90,  /* #..#.... */
	/*0105:*/	0x70,  /* .###.... */
	/*0106:*/	0x10,  /* ...#.... */
	/*0107:*/	0x60,  /* .##..... */
/* --- new character colon (58) starting at offset 0x0108 --- */
	/*0108:*/	5, 5, 5, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*010d:*/	0x60,  /* .##..... */
	/*010e:*/	0x60,  /* .##..... */
	/*010f:*/	0x00,  /* ........ */
	/*0110:*/	0x60,  /* .##..... */
	/*0111:*/	0x60,  /* .##..... */
/* --- new character semicolon (59) starting at offset 0x0112 --- */
	/*0112:*/	5, 5, 6, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*0117:*/	0x30,  /* ..##.... */
	/*0118:*/	0x30,  /* ..##.... */
	/*0119:*/	0x00,  /* ........ */
	/*011a:*/	0x30,  /* ..##.... */
	/*011b:*/	0x20,  /* ..#..... */
	/*011c:*/	0x40,  /* .#...... */
/* --- new character less (60) starting at offset 0x011d --- */
	/*011d:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0122:*/	0x10,  /* ...#.... */
	/*0123:*/	0x20,  /* ..#..... */
	/*0124:*/	0x40,  /* .#...... */
	/*0125:*/	0x40,  /* .#...... */
	/*0126:*/	0x20,  /* ..#..... */
	/*0127:*/	0x10,  /* ...#.... */
/* --- new character equal (61) starting at offset 0x0128 --- */
	/*0128:*/	5, 5, 3, 0, 1, /* width and bbox (w,h,x,y) */ 
	/*012d:*/	0xf0,  /* ####.... */
	/*012e:*/	0x00,  /* ........ */
	/*012f:*/	0xf0,  /* ####.... */
/* --- new character greater (62) starting at offset 0x0130 --- */
	/*0130:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0135:*/	0x40,  /* .#...... */
	/*0136:*/	0x20,  /* ..#..... */
	/*0137:*/	0x10,  /* ...#.... */
	/*0138:*/	0x10,  /* ...#.... */
	/*0139:*/	0x20,  /* ..#..... */
	/*013a:*/	0x40,  /* .#...... */
/* --- new character question (63) starting at offset 0x013b --- */
	/*013b:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0140:*/	0x20,  /* ..#..... */
	/*0141:*/	0x50,  /* .#.#.... */
	/*0142:*/	0x10,  /* ...#.... */
	/*0143:*/	0x20,  /* ..#..... */
	/*0144:*/	0x00,  /* ........ */
	/*0145:*/	0x20,  /* ..#..... */
/* --- new character at (64) starting at offset 0x0146 --- */
	/*0146:*/	5, 5, 8, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*014b:*/	0x30,  /* ..##.... */
	/*014c:*/	0x48,  /* .#..#... */
	/*014d:*/	0x98,  /* #..##... */
	/*014e:*/	0xa8,  /* #.#.#... */
	/*014f:*/	0xa8,  /* #.#.#... */
	/*0150:*/	0x90,  /* #..#.... */
	/*0151:*/	0x40,  /* .#...... */
	/*0152:*/	0x30,  /* ..##.... */
/* --- new character A (65) starting at offset 0x0153 --- */
	/*0153:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0158:*/	0x60,  /* .##..... */
	/*0159:*/	0x90,  /* #..#.... */
	/*015a:*/	0x90,  /* #..#.... */
	/*015b:*/	0xf0,  /* ####.... */
	/*015c:*/	0x90,  /* #..#.... */
	/*015d:*/	0x90,  /* #..#.... */
/* --- new character B (66) starting at offset 0x015e --- */
	/*015e:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0163:*/	0xe0,  /* ###..... */
	/*0164:*/	0x90,  /* #..#.... */
	/*0165:*/	0xe0,  /* ###..... */
	/*0166:*/	0x90,  /* #..#.... */
	/*0167:*/	0x90,  /* #..#.... */
	/*0168:*/	0xe0,  /* ###..... */
/* --- new character C (67) starting at offset 0x0169 --- */
	/*0169:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*016e:*/	0x60,  /* .##..... */
	/*016f:*/	0x90,  /* #..#.... */
	/*0170:*/	0x80,  /* #....... */
	/*0171:*/	0x80,  /* #....... */
	/*0172:*/	0x90,  /* #..#.... */
	/*0173:*/	0x60,  /* .##..... */
/* --- new character D (68) starting at offset 0x0174 --- */
	/*0174:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0179:*/	0xe0,  /* ###..... */
	/*017a:*/	0x90,  /* #..#.... */
	/*017b:*/	0x90,  /* #..#.... */
	/*017c:*/	0x90,  /* #..#.... */
	/*017d:*/	0x90,  /* #..#.... */
	/*017e:*/	0xe0,  /* ###..... */
/* --- new character E (69) starting at offset 0x017f --- */
	/*017f:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0184:*/	0xf0,  /* ####.... */
	/*0185:*/	0x80,  /* #....... */
	/*0186:*/	0xe0,  /* ###..... */
	/*0187:*/	0x80,  /* #....... */
	/*0188:*/	0x80,  /* #....... */
	/*0189:*/	0xf0,  /* ####.... */
/* --- new character F (70) starting at offset 0x018a --- */
	/*018a:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*018f:*/	0xf0,  /* ####.... */
	/*0190:*/	0x80,  /* #....... */
	/*0191:*/	0xe0,  /* ###..... */
	/*0192:*/	0x80,  /* #....... */
	/*0193:*/	0x80,  /* #....... */
	/*0194:*/	0x80,  /* #....... */
/* --- new character G (71) starting at offset 0x0195 --- */
	/*0195:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*019a:*/	0x60,  /* .##..... */
	/*019b:*/	0x90,  /* #..#.... */
	/*019c:*/	0x80,  /* #....... */
	/*019d:*/	0xb0,  /* #.##.... */
	/*019e:*/	0x90,  /* #..#.... */
	/*019f:*/	0x60,  /* .##..... */
/* --- new character H (72) starting at offset 0x01a0 --- */
	/*01a0:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01a5:*/	0x90,  /* #..#.... */
	/*01a6:*/	0x90,  /* #..#.... */
	/*01a7:*/	0xf0,  /* ####.... */
	/*01a8:*/	0x90,  /* #..#.... */
	/*01a9:*/	0x90,  /* #..#.... */
	/*01aa:*/	0x90,  /* #..#.... */
/* --- new character I (73) starting at offset 0x01ab --- */
	/*01ab:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01b0:*/	0x70,  /* .###.... */
	/*01b1:*/	0x20,  /* ..#..... */
	/*01b2:*/	0x20,  /* ..#..... */
	/*01b3:*/	0x20,  /* ..#..... */
	/*01b4:*/	0x20,  /* ..#..... */
	/*01b5:*/	0x70,  /* .###.... */
/* --- new character J (74) starting at offset 0x01b6 --- */
	/*01b6:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01bb:*/	0x70,  /* .###.... */
	/*01bc:*/	0x20,  /* ..#..... */
	/*01bd:*/	0x20,  /* ..#..... */
	/*01be:*/	0x20,  /* ..#..... */
	/*01bf:*/	0xa0,  /* #.#..... */
	/*01c0:*/	0x40,  /* .#...... */
/* --- new character K (75) starting at offset 0x01c1 --- */
	/*01c1:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01c6:*/	0x90,  /* #..#.... */
	/*01c7:*/	0xa0,  /* #.#..... */
	/*01c8:*/	0xc0,  /* ##...... */
	/*01c9:*/	0xa0,  /* #.#..... */
	/*01ca:*/	0xa0,  /* #.#..... */
	/*01cb:*/	0x90,  /* #..#.... */
/* --- new character L (76) starting at offset 0x01cc --- */
	/*01cc:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01d1:*/	0x80,  /* #....... */
	/*01d2:*/	0x80,  /* #....... */
	/*01d3:*/	0x80,  /* #....... */
	/*01d4:*/	0x80,  /* #....... */
	/*01d5:*/	0x80,  /* #....... */
	/*01d6:*/	0xf0,  /* ####.... */
/* --- new character M (77) starting at offset 0x01d7 --- */
	/*01d7:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01dc:*/	0x90,  /* #..#.... */
	/*01dd:*/	0xf0,  /* ####.... */
	/*01de:*/	0xf0,  /* ####.... */
	/*01df:*/	0x90,  /* #..#.... */
	/*01e0:*/	0x90,  /* #..#.... */
	/*01e1:*/	0x90,  /* #..#.... */
/* --- new character N (78) starting at offset 0x01e2 --- */
	/*01e2:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01e7:*/	0x90,  /* #..#.... */
	/*01e8:*/	0xd0,  /* ##.#.... */
	/*01e9:*/	0xf0,  /* ####.... */
	/*01ea:*/	0xb0,  /* #.##.... */
	/*01eb:*/	0xb0,  /* #.##.... */
	/*01ec:*/	0x90,  /* #..#.... */
/* --- new character O (79) starting at offset 0x01ed --- */
	/*01ed:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01f2:*/	0x60,  /* .##..... */
	/*01f3:*/	0x90,  /* #..#.... */
	/*01f4:*/	0x90,  /* #..#.... */
	/*01f5:*/	0x90,  /* #..#.... */
	/*01f6:*/	0x90,  /* #..#.... */
	/*01f7:*/	0x60,  /* .##..... */
/* --- new character P (80) starting at offset 0x01f8 --- */
	/*01f8:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*01fd:*/	0xe0,  /* ###..... */
	/*01fe:*/	0x90,  /* #..#.... */
	/*01ff:*/	0x90,  /* #..#.... */
	/*0200:*/	0xe0,  /* ###..... */
	/*0201:*/	0x80,  /* #....... */
	/*0202:*/	0x80,  /* #....... */
/* --- new character Q (81) starting at offset 0x0203 --- */
	/*0203:*/	5, 5, 7, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*0208:*/	0x60,  /* .##..... */
	/*0209:*/	0x90,  /* #..#.... */
	/*020a:*/	0x90,  /* #..#.... */
	/*020b:*/	0xd0,  /* ##.#.... */
	/*020c:*/	0xb0,  /* #.##.... */
	/*020d:*/	0x60,  /* .##..... */
	/*020e:*/	0x10,  /* ...#.... */
/* --- new character R (82) starting at offset 0x020f --- */
	/*020f:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0214:*/	0xe0,  /* ###..... */
	/*0215:*/	0x90,  /* #..#.... */
	/*0216:*/	0x90,  /* #..#.... */
	/*0217:*/	0xe0,  /* ###..... */
	/*0218:*/	0x90,  /* #..#.... */
	/*0219:*/	0x90,  /* #..#.... */
/* --- new character S (83) starting at offset 0x021a --- */
	/*021a:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*021f:*/	0x60,  /* .##..... */
	/*0220:*/	0x90,  /* #..#.... */
	/*0221:*/	0x40,  /* .#...... */
	/*0222:*/	0x20,  /* ..#..... */
	/*0223:*/	0x90,  /* #..#.... */
	/*0224:*/	0x60,  /* .##..... */
/* --- new character T (84) starting at offset 0x0225 --- */
	/*0225:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*022a:*/	0x70,  /* .###.... */
	/*022b:*/	0x20,  /* ..#..... */
	/*022c:*/	0x20,  /* ..#..... */
	/*022d:*/	0x20,  /* ..#..... */
	/*022e:*/	0x20,  /* ..#..... */
	/*022f:*/	0x20,  /* ..#..... */
/* --- new character U (85) starting at offset 0x0230 --- */
	/*0230:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0235:*/	0x90,  /* #..#.... */
	/*0236:*/	0x90,  /* #..#.... */
	/*0237:*/	0x90,  /* #..#.... */
	/*0238:*/	0x90,  /* #..#.... */
	/*0239:*/	0x90,  /* #..#.... */
	/*023a:*/	0x60,  /* .##..... */
/* --- new character V (86) starting at offset 0x023b --- */
	/*023b:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0240:*/	0x90,  /* #..#.... */
	/*0241:*/	0x90,  /* #..#.... */
	/*0242:*/	0x90,  /* #..#.... */
	/*0243:*/	0x90,  /* #..#.... */
	/*0244:*/	0x60,  /* .##..... */
	/*0245:*/	0x60,  /* .##..... */
/* --- new character W (87) starting at offset 0x0246 --- */
	/*0246:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*024b:*/	0x90,  /* #..#.... */
	/*024c:*/	0x90,  /* #..#.... */
	/*024d:*/	0x90,  /* #..#.... */
	/*024e:*/	0xf0,  /* ####.... */
	/*024f:*/	0xf0,  /* ####.... */
	/*0250:*/	0x90,  /* #..#.... */
/* --- new character X (88) starting at offset 0x0251 --- */
	/*0251:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0256:*/	0x90,  /* #..#.... */
	/*0257:*/	0x90,  /* #..#.... */
	/*0258:*/	0x60,  /* .##..... */
	/*0259:*/	0x60,  /* .##..... */
	/*025a:*/	0x90,  /* #..#.... */
	/*025b:*/	0x90,  /* #..#.... */
/* --- new character Y (89) starting at offset 0x025c --- */
	/*025c:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0261:*/	0x88,  /* #...#... */
	/*0262:*/	0x88,  /* #...#... */
	/*0263:*/	0x50,  /* .#.#.... */
	/*0264:*/	0x20,  /* ..#..... */
	/*0265:*/	0x20,  /* ..#..... */
	/*0266:*/	0x20,  /* ..#..... */
/* --- new character Z (90) starting at offset 0x0267 --- */
	/*0267:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*026c:*/	0xf0,  /* ####.... */
	/*026d:*/	0x10,  /* ...#.... */
	/*026e:*/	0x20,  /* ..#..... */
	/*026f:*/	0x40,  /* .#...... */
	/*0270:*/	0x80,  /* #....... */
	/*0271:*/	0xf0,  /* ####.... */
/* --- new character bracketleft (91) starting at offset 0x0272 --- */
	/*0272:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0277:*/	0x70,  /* .###.... */
	/*0278:*/	0x40,  /* .#...... */
	/*0279:*/	0x40,  /* .#...... */
	/*027a:*/	0x40,  /* .#...... */
	/*027b:*/	0x40,  /* .#...... */
	/*027c:*/	0x70,  /* .###.... */
/* --- new character backslash (92) starting at offset 0x027d --- */
	/*027d:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0282:*/	0x80,  /* #....... */
	/*0283:*/	0x80,  /* #....... */
	/*0284:*/	0x40,  /* .#...... */
	/*0285:*/	0x20,  /* ..#..... */
	/*0286:*/	0x10,  /* ...#.... */
	/*0287:*/	0x10,  /* ...#.... */
/* --- new character bracketright (93) starting at offset 0x0288 --- */
	/*0288:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*028d:*/	0x70,  /* .###.... */
	/*028e:*/	0x10,  /* ...#.... */
	/*028f:*/	0x10,  /* ...#.... */
	/*0290:*/	0x10,  /* ...#.... */
	/*0291:*/	0x10,  /* ...#.... */
	/*0292:*/	0x70,  /* .###.... */
/* --- new character asciicircum (94) starting at offset 0x0293 --- */
	/*0293:*/	5, 5, 2, 0, 4, /* width and bbox (w,h,x,y) */ 
	/*0298:*/	0x20,  /* ..#..... */
	/*0299:*/	0x50,  /* .#.#.... */
/* --- new character underscore (95) starting at offset 0x029a --- */
	/*029a:*/	5, 5, 1, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*029f:*/	0xf0,  /* ####.... */
/* --- new character grave (96) starting at offset 0x02a0 --- */
	/*02a0:*/	5, 5, 2, 0, 4, /* width and bbox (w,h,x,y) */ 
	/*02a5:*/	0x40,  /* .#...... */
	/*02a6:*/	0x20,  /* ..#..... */
/* --- new character a (97) starting at offset 0x02a7 --- */
	/*02a7:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*02ac:*/	0x70,  /* .###.... */
	/*02ad:*/	0x90,  /* #..#.... */
	/*02ae:*/	0x90,  /* #..#.... */
	/*02af:*/	0x70,  /* .###.... */
/* --- new character b (98) starting at offset 0x02b0 --- */
	/*02b0:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*02b5:*/	0x80,  /* #....... */
	/*02b6:*/	0x80,  /* #....... */
	/*02b7:*/	0xe0,  /* ###..... */
	/*02b8:*/	0x90,  /* #..#.... */
	/*02b9:*/	0x90,  /* #..#.... */
	/*02ba:*/	0xe0,  /* ###..... */
/* --- new character c (99) starting at offset 0x02bb --- */
	/*02bb:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*02c0:*/	0x30,  /* ..##.... */
	/*02c1:*/	0x40,  /* .#...... */
	/*02c2:*/	0x40,  /* .#...... */
	/*02c3:*/	0x30,  /* ..##.... */
/* --- new character d (100) starting at offset 0x02c4 --- */
	/*02c4:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*02c9:*/	0x10,  /* ...#.... */
	/*02ca:*/	0x10,  /* ...#.... */
	/*02cb:*/	0x70,  /* .###.... */
	/*02cc:*/	0x90,  /* #..#.... */
	/*02cd:*/	0x90,  /* #..#.... */
	/*02ce:*/	0x70,  /* .###.... */
/* --- new character e (101) starting at offset 0x02cf --- */
	/*02cf:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*02d4:*/	0x60,  /* .##..... */
	/*02d5:*/	0xb0,  /* #.##.... */
	/*02d6:*/	0xc0,  /* ##...... */
	/*02d7:*/	0x60,  /* .##..... */
/* --- new character f (102) starting at offset 0x02d8 --- */
	/*02d8:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*02dd:*/	0x20,  /* ..#..... */
	/*02de:*/	0x50,  /* .#.#.... */
	/*02df:*/	0x40,  /* .#...... */
	/*02e0:*/	0xe0,  /* ###..... */
	/*02e1:*/	0x40,  /* .#...... */
	/*02e2:*/	0x40,  /* .#...... */
/* --- new character g (103) starting at offset 0x02e3 --- */
	/*02e3:*/	5, 5, 5, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*02e8:*/	0x60,  /* .##..... */
	/*02e9:*/	0x90,  /* #..#.... */
	/*02ea:*/	0x70,  /* .###.... */
	/*02eb:*/	0x10,  /* ...#.... */
	/*02ec:*/	0x60,  /* .##..... */
/* --- new character h (104) starting at offset 0x02ed --- */
	/*02ed:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*02f2:*/	0x80,  /* #....... */
	/*02f3:*/	0x80,  /* #....... */
	/*02f4:*/	0xe0,  /* ###..... */
	/*02f5:*/	0x90,  /* #..#.... */
	/*02f6:*/	0x90,  /* #..#.... */
	/*02f7:*/	0x90,  /* #..#.... */
/* --- new character i (105) starting at offset 0x02f8 --- */
	/*02f8:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*02fd:*/	0x20,  /* ..#..... */
	/*02fe:*/	0x00,  /* ........ */
	/*02ff:*/	0x60,  /* .##..... */
	/*0300:*/	0x20,  /* ..#..... */
	/*0301:*/	0x20,  /* ..#..... */
	/*0302:*/	0x70,  /* .###.... */
/* --- new character j (106) starting at offset 0x0303 --- */
	/*0303:*/	5, 5, 7, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*0308:*/	0x10,  /* ...#.... */
	/*0309:*/	0x00,  /* ........ */
	/*030a:*/	0x10,  /* ...#.... */
	/*030b:*/	0x10,  /* ...#.... */
	/*030c:*/	0x10,  /* ...#.... */
	/*030d:*/	0x50,  /* .#.#.... */
	/*030e:*/	0x20,  /* ..#..... */
/* --- new character k (107) starting at offset 0x030f --- */
	/*030f:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0314:*/	0x80,  /* #....... */
	/*0315:*/	0x80,  /* #....... */
	/*0316:*/	0x90,  /* #..#.... */
	/*0317:*/	0xe0,  /* ###..... */
	/*0318:*/	0x90,  /* #..#.... */
	/*0319:*/	0x90,  /* #..#.... */
/* --- new character l (108) starting at offset 0x031a --- */
	/*031a:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*031f:*/	0x60,  /* .##..... */
	/*0320:*/	0x20,  /* ..#..... */
	/*0321:*/	0x20,  /* ..#..... */
	/*0322:*/	0x20,  /* ..#..... */
	/*0323:*/	0x20,  /* ..#..... */
	/*0324:*/	0x70,  /* .###.... */
/* --- new character m (109) starting at offset 0x0325 --- */
	/*0325:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*032a:*/	0xd0,  /* ##.#.... */
	/*032b:*/	0xa8,  /* #.#.#... */
	/*032c:*/	0xa8,  /* #.#.#... */
	/*032d:*/	0xa8,  /* #.#.#... */
/* --- new character n (110) starting at offset 0x032e --- */
	/*032e:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0333:*/	0xe0,  /* ###..... */
	/*0334:*/	0x90,  /* #..#.... */
	/*0335:*/	0x90,  /* #..#.... */
	/*0336:*/	0x90,  /* #..#.... */
/* --- new character o (111) starting at offset 0x0337 --- */
	/*0337:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*033c:*/	0x60,  /* .##..... */
	/*033d:*/	0x90,  /* #..#.... */
	/*033e:*/	0x90,  /* #..#.... */
	/*033f:*/	0x60,  /* .##..... */
/* --- new character p (112) starting at offset 0x0340 --- */
	/*0340:*/	5, 5, 5, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*0345:*/	0xe0,  /* ###..... */
	/*0346:*/	0x90,  /* #..#.... */
	/*0347:*/	0xe0,  /* ###..... */
	/*0348:*/	0x80,  /* #....... */
	/*0349:*/	0x80,  /* #....... */
/* --- new character q (113) starting at offset 0x034a --- */
	/*034a:*/	5, 5, 5, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*034f:*/	0x70,  /* .###.... */
	/*0350:*/	0x90,  /* #..#.... */
	/*0351:*/	0x70,  /* .###.... */
	/*0352:*/	0x10,  /* ...#.... */
	/*0353:*/	0x10,  /* ...#.... */
/* --- new character r (114) starting at offset 0x0354 --- */
	/*0354:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0359:*/	0xa0,  /* #.#..... */
	/*035a:*/	0xd0,  /* ##.#.... */
	/*035b:*/	0x80,  /* #....... */
	/*035c:*/	0x80,  /* #....... */
/* --- new character s (115) starting at offset 0x035d --- */
	/*035d:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0362:*/	0x30,  /* ..##.... */
	/*0363:*/	0x60,  /* .##..... */
	/*0364:*/	0x10,  /* ...#.... */
	/*0365:*/	0x60,  /* .##..... */
/* --- new character t (116) starting at offset 0x0366 --- */
	/*0366:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*036b:*/	0x40,  /* .#...... */
	/*036c:*/	0x40,  /* .#...... */
	/*036d:*/	0xe0,  /* ###..... */
	/*036e:*/	0x40,  /* .#...... */
	/*036f:*/	0x50,  /* .#.#.... */
	/*0370:*/	0x20,  /* ..#..... */
/* --- new character u (117) starting at offset 0x0371 --- */
	/*0371:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0376:*/	0x90,  /* #..#.... */
	/*0377:*/	0x90,  /* #..#.... */
	/*0378:*/	0x90,  /* #..#.... */
	/*0379:*/	0x70,  /* .###.... */
/* --- new character v (118) starting at offset 0x037a --- */
	/*037a:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*037f:*/	0x50,  /* .#.#.... */
	/*0380:*/	0x50,  /* .#.#.... */
	/*0381:*/	0x50,  /* .#.#.... */
	/*0382:*/	0x20,  /* ..#..... */
/* --- new character w (119) starting at offset 0x0383 --- */
	/*0383:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0388:*/	0x88,  /* #...#... */
	/*0389:*/	0xa8,  /* #.#.#... */
	/*038a:*/	0xa8,  /* #.#.#... */
	/*038b:*/	0x50,  /* .#.#.... */
/* --- new character x (120) starting at offset 0x038c --- */
	/*038c:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*0391:*/	0x90,  /* #..#.... */
	/*0392:*/	0x60,  /* .##..... */
	/*0393:*/	0x60,  /* .##..... */
	/*0394:*/	0x90,  /* #..#.... */
/* --- new character y (121) starting at offset 0x0395 --- */
	/*0395:*/	5, 5, 5, 0, -1, /* width and bbox (w,h,x,y) */ 
	/*039a:*/	0x90,  /* #..#.... */
	/*039b:*/	0x90,  /* #..#.... */
	/*039c:*/	0x70,  /* .###.... */
	/*039d:*/	0x90,  /* #..#.... */
	/*039e:*/	0x60,  /* .##..... */
/* --- new character z (122) starting at offset 0x039f --- */
	/*039f:*/	5, 5, 4, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*03a4:*/	0xf0,  /* ####.... */
	/*03a5:*/	0x20,  /* ..#..... */
	/*03a6:*/	0x40,  /* .#...... */
	/*03a7:*/	0xf0,  /* ####.... */
/* --- new character braceleft (123) starting at offset 0x03a8 --- */
	/*03a8:*/	5, 5, 7, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*03ad:*/	0x30,  /* ..##.... */
	/*03ae:*/	0x40,  /* .#...... */
	/*03af:*/	0x20,  /* ..#..... */
	/*03b0:*/	0xc0,  /* ##...... */
	/*03b1:*/	0x20,  /* ..#..... */
	/*03b2:*/	0x40,  /* .#...... */
	/*03b3:*/	0x30,  /* ..##.... */
/* --- new character bar (124) starting at offset 0x03b4 --- */
	/*03b4:*/	5, 5, 6, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*03b9:*/	0x20,  /* ..#..... */
	/*03ba:*/	0x20,  /* ..#..... */
	/*03bb:*/	0x20,  /* ..#..... */
	/*03bc:*/	0x20,  /* ..#..... */
	/*03bd:*/	0x20,  /* ..#..... */
	/*03be:*/	0x20,  /* ..#..... */
/* --- new character braceright (125) starting at offset 0x03bf --- */
	/*03bf:*/	5, 5, 7, 0, 0, /* width and bbox (w,h,x,y) */ 
	/*03c4:*/	0xc0,  /* ##...... */
	/*03c5:*/	0x20,  /* ..#..... */
	/*03c6:*/	0x40,  /* .#...... */
	/*03c7:*/	0x30,  /* ..##.... */
	/*03c8:*/	0x40,  /* .#...... */
	/*03c9:*/	0x20,  /* ..#..... */
	/*03ca:*/	0xc0,  /* ##...... */
/* --- new character asciitilde (126) starting at offset 0x03cb --- */
	/*03cb:*/	5, 5, 2, 0, 4, /* width and bbox (w,h,x,y) */ 
	/*03d0:*/	0x50,  /* .#.#.... */
	/*03d1:*/	0xa0,  /* #.#..... */
};
static const uint16_t font_5x8_offsets[] = {
0x0000 /* space */,
	0x0006 /* exclam */,
	0x0011 /* quotedbl */,
	0x0019 /* numbersign */,
	0x0025 /* dollar */,
	0x0031 /* percent */,
	0x003b /* ampersand */,
	0x0047 /* quotesingle */,
	0x004f /* parenleft */,
	0x005a /* parenright */,
	0x0065 /* asterisk */,
	0x006f /* plus */,
	0x0079 /* comma */,
	0x0081 /* hyphen */,
	0x0087 /* period */,
	0x008f /* slash */,
	0x009a /* zero */,
	0x00a5 /* one */,
	0x00b0 /* two */,
	0x00bb /* three */,
	0x00c6 /* four */,
	0x00d1 /* five */,
	0x00dc /* six */,
	0x00e7 /* seven */,
	0x00f2 /* eight */,
	0x00fd /* nine */,
	0x0108 /* colon */,
	0x0112 /* semicolon */,
	0x011d /* less */,
	0x0128 /* equal */,
	0x0130 /* greater */,
	0x013b /* question */,
	0x0146 /* at */,
	0x0153 /* A */,
	0x015e /* B */,
	0x0169 /* C */,
	0x0174 /* D */,
	0x017f /* E */,
	0x018a /* F */,
	0x0195 /* G */,
	0x01a0 /* H */,
	0x01ab /* I */,
	0x01b6 /* J */,
	0x01c1 /* K */,
	0x01cc /* L */,
	0x01d7 /* M */,
	0x01e2 /* N */,
	0x01ed /* O */,
	0x01f8 /* P */,
	0x0203 /* Q */,
	0x020f /* R */,
	0x021a /* S */,
	0x0225 /* T */,
	0x0230 /* U */,
	0x023b /* V */,
	0x0246 /* W */,
	0x0251 /* X */,
	0x025c /* Y */,
	0x0267 /* Z */,
	0x0272 /* bracketleft */,
	0x027d /* backslash */,
	0x0288 /* bracketright */,
	0x0293 /* asciicircum */,
	0x029a /* underscore */,
	0x02a0 /* grave */,
	0x02a7 /* a */,
	0x02b0 /* b */,
	0x02bb /* c */,
	0x02c4 /* d */,
	0x02cf /* e */,
	0x02d8 /* f */,
	0x02e3 /* g */,
	0x02ed /* h */,
	0x02f8 /* i */,
	0x0303 /* j */,
	0x030f /* k */,
	0x031a /* l */,
	0x0325 /* m */,
	0x032e /* n */,
	0x0337 /* o */,
	0x0340 /* p */,
	0x034a /* q */,
	0x0354 /* r */,
	0x035d /* s */,
	0x0366 /* t */,
	0x0371 /* u */,
	0x037a /* v */,
	0x0383 /* w */,
	0x038c /* x */,
	0x0395 /* y */,
	0x039f /* z */,
	0x03a8 /* braceleft */,
	0x03b4 /* bar */,
	0x03bf /* braceright */,
	0x03cb /* asciitilde */,
	0xffff /* (no glyph) */
};
const struct fb_font font_5x8 = {
	.height = 8,
	.ascent = 7,
	.firstchar = 32, /* space */
	.lastchar = 127, /* ? */
	.chardata = font_5x8_data,
	.charoffs = font_5x8_offsets,
};
