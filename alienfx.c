// alienfx.c  -*- coding:utf-8 -*-

static const char *Version[] = {
	"@(#) alienfx.c Copyright (c) 2010 C. Alex. North-Keys",
	"$Group: Talisman $",
	"$Incept: 2010-10-19 07:05:53 GMT (Oct Tue) 1287471953 $",
	"$Compiled: "__DATE__" " __TIME__ " $",
	"$Source: /home/erlkonig/zoo/alienfx/alienfx.c $",
	"$State: $",
	"$Revision: $",
	"$Date: 2010-10-19 07:05:53 GMT (Oct Tue) 1287471953 $",
	"$Author: erlkonig $",
	(const char*)0
	};

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#include <libusb-1.0/libusb.h>

#include <fgetstr.h>
#include <stoss.h>

// The "#define"s below were exhumed from code at http://3d.benjamin-thaut.de/
#define MIN_SPEED   100
#define MAX_SPEED  1000
#define STEP_SPEED  100

#define SEND_REQUEST_TYPE 0x21
#define SEND_REQUEST      0x09
#define SEND_VALUE        0x202
#define SEND_INDEX        0x00
#define SEND_DATA_SIZE    9

#define READ_REQUEST_TYPE 0xa1
#define READ_REQUEST      0x01
#define READ_VALUE        0x101
#define READ_INDEX        0x0
#define READ_DATA_SIZE    9  //  (alienFXid == ALLPOWERFULL_ALIENFX ? 8 : 9)

// all of these matched in code for ALLPOWERFULL and AREA51 controllers
#define STATE_BUSY               	  0x11
#define STATE_READY              	  0x10
#define STATE_UNKNOWN_COMMAND    	  0x12

#define SUPPORTED_COMMANDS       	    15
#define COMMAND_END_STORAGE      	  0x00 // End storage block
#define COMMAND_SET_MORPH_COLOR  	  0x01
#define COMMAND_SET_BLINK_COLOR  	  0x02
#define COMMAND_SET_COLOR        	  0x03
#define COMMAND_LOOP_BLOCK_END   	  0x04
#define COMMAND_TRANSMIT_EXECUTE 	  0x05 // End transmition and execute
#define COMMAND_GET_STATUS       	  0x06 // Get device status
#define COMMAND_RESET            	  0x07
#define COMMAND_SAVE_NEXT        	  0x08 // Save next inst' in storage block
#define COMMAND_SAVE             	  0x09 // Save storage data
#define COMMAND_BATTERY_STATE    	  0x0F // Set batery state
#define COMMAND_SET_SPEED        	  0x0E // Set display speed

#define RESET_TOUCH_CONTROLS     	  0x01
#define RESET_SLEEP_LIGHTS_ON    	  0x02
#define RESET_ALL_LIGHTS_OFF     	  0x03
#define RESET_ALL_LIGHTS_ON      	  0x04

#define DATA_LENGTH                      9

#define START_BYTE               	  0x02  // shows 0x00 for Area51; bug?
#define FILL_BYTE                	  0x00

#define BLOCK_LOAD_ON_BOOT       	  0x01
#define BLOCK_STANDBY            	  0x02
#define BLOCK_AC_POWER           	  0x05
#define BLOCK_CHARGING           	  0x06
#define BLOCK_BAT_POWER          	  0x08

typedef struct _AlienFxLights_t
{
	unsigned int id;			/* bit-or-able */
	const char *name;
	unsigned char is_composite;	/* is this a composite of other ids */
} AlienFxLights_t;

// m11x lighting
AlienFxLights_t LightsM11x[] = {
	{ 0x0001, "keyboard"          ,0 },
	{ 0x0020, "speaker-left"      ,0 },
	{ 0x0040, "speaker-right"     ,0 },
	{ 0x0100, "logo"              ,0 },
	{ 0x0800, "media-bar"         ,0 },
	{ 0x2000, "power-button"      ,0 },
	{ 0x4000, "power-button-eyes" ,0 },
	{ 0x8000, "power-reset-state" ,0 },
	{ 0x0961, "all-but-power"     ,1 },
	{ 0xe961, "all"               ,1 },
};
unsigned int LightsM11xCount = (sizeof LightsM11x
								/ sizeof *LightsM11x);

// AllPowerfull lighting (is that really with two "L"s on the end?)
AlienFxLights_t LightsAllPowerful[] = {
	{ 0x0001, "keyboard-right"      ,0 },
	{ 0x0002, "keyboard-middleright",0 },
	{ 0x0004, "keyboard-left"       ,0 },
	{ 0x0008, "keyboard-middleleft" ,0 },
	{ 0x0010, "power-button-2"      ,0 },
	{ 0x0020, "speaker-right"       ,0 },
	{ 0x0040, "speaker-left"        ,0 },
	{ 0x0080, "head"                ,0 }, // alien's
	{ 0x0100, "name"                ,0 },
	{ 0x0200, "touchpad"            ,0 },
	{ 0x1c00, "madia-bar"           ,0 },
	{ 0x2000, "power-button"        ,0 },
	{ 0x4000, "power-button-eyes"   ,0 },
	{ 0x7fff, "all"                 ,1 },
};
unsigned int LightsAllPowerfulCount = (sizeof LightsAllPowerful
									   / sizeof *LightsAllPowerful);

// Alienware 13 R3 OLED lighting
AlienFxLights_t LightsAlienware13r3oled[] = {
        { 0x0001, "keyboard-right"      ,0 },
        { 0x0002, "keyboard-middleright",0 },
        { 0x0004, "keyboard-left"       ,0 },
        { 0x0008, "keyboard-middleleft" ,0 },
        { 0x0100, "power-button"        ,0 },
        { 0x0020, "head"                ,0 }, // alien's
        { 0x0040, "name"                ,0 },
        { 0x0200, "touchpad"            ,0 },
        { 0x7fff, "all"                 ,1 },
};
unsigned int LightsAlienware13r3oledCount = (sizeof LightsAlienware13r3oled
                                                                           / sizeof *LightsAlienware13r3oled);

// Alienware 15 R3 lighting.
AlienFxLights_t LightsAlienware15r3[] = {
	{ 0x00002, "keyboard-mid-right"       ,0 },
	{ 0x00004, "keyboard-mid-left"        ,0 },
	{ 0x00008, "keyboard-far-left"        ,0 }, // alien
	{ 0x00001, "keyboard-far-right"       ,0 }, // alien's
	{ 0x00100, "power-button"             ,0 },
	{ 0x00020, "head"                     ,0 }, // button
	{ 0x00040, "name"                     ,0 },
	{ 0x00800, "bottom-right-burner"      ,0 },
	{ 0x00400, "bottom-left-burner"       ,0 },
	{ 0x02000, "top-right-burner"         ,0 },
	{ 0x01000, "top-left-burner"          ,0 },
	{ 0x00080, "touchpad"                 ,0 },
	{ 0x04000, "special"                 ,0 },
	{ 0x0ffff, "all"                      ,1 },
};

unsigned int LightsAlienware15r3Count = (sizeof LightsAlienware15r3
                                             / sizeof *LightsAlienware15r3);

// Area51 lighting.
AlienFxLights_t LightsArea51[] = {
	{ 0x00001, "touchpad"   ,0 },
	{ 0x00020, "lightpipe"  ,0 },
	{ 0x00080, "logo"       ,0 }, // alien
	{ 0x00100, "head"       ,0 }, // alien's
	{ 0x00400, "keyboard"   ,0 },
	{ 0x08000, "power"      ,0 }, // button
	{ 0x10000, "touchpanel" ,0 },
	{ 0x185a1, "all"        ,1 },
};
unsigned int LightsArea51Count = (sizeof LightsArea51
								  / sizeof *LightsArea51);

// Aurora lighting.  Left/right from the alien head's persp,0 ective, facing you
AlienFxLights_t LightsAurora[] = {
	{ 0x001, "head"       ,0 }, // alien's (his perspective determines left/right)
	{ 0x002, "top-right"  ,0 }, // R4-ALX top lighting (one side) under air flaps
	{ 0x004, "top-left"   ,0 }, // R4-ALX top lighting (one side)  under air flaps
	{ 0x008, "front"      ,0 }, // below alien head
	{ 0x020, "door-right" ,0 }, // R4-ALX inside of door (one side)
	{ 0x040, "door-left"  ,0 }, // R4-ALX inside of door (one side)
	{ 0x080, "right"      ,0 }, // lower side panel
	{ 0x100, "left"       ,0 }, // lower side panel
	{ 0x180, "sides"      ,1 }, // lower side panels
	{ 0xffe, "panels"     ,1 }, // everything but the alien head
	{ 0xfff, "all"        ,1 }, // 0x189 for normal, 0x1ef for ALX
};
unsigned int LightsAuroraCount = (sizeof LightsAurora
								  / sizeof *LightsAurora);

typedef struct _AlienFxType_t {
	unsigned int idVendor;
	unsigned int idProduct;
	const char *name;
	useconds_t post_reset_delay;
	AlienFxLights_t *lights;
	unsigned int lightsCount;
} AlienFxType_t;

AlienFxType_t AlienFxTypes[] = {
	// vendor product names - these aren't consistent with iProduct,
	// since I only had one data point and no easy way to see if AlienWare
	// had just stamped "G2 Desktop" on all of them or something.
	{ 0x187c, 0x514, "m11x", 100000, /* from github:stefansaraev */
	  LightsM11x, sizeof LightsM11x / sizeof *LightsM11x },
	{ 0x187c, 0x511, "area51", 100000,  /* delay not yet verified */
	  LightsArea51, sizeof LightsArea51 / sizeof *LightsArea51 },
	{ 0x187c, 0x512, "allpowerful", 100000,  /* delay not yet verified */
	  LightsAllPowerful, sizeof LightsAllPowerful / sizeof *LightsAllPowerful },
	{ 0x187c, 0x513, "aurora", 2500, /* ~R2 0µs; R4-ALX ≥2265µs */
	  LightsAurora, sizeof LightsAurora / sizeof *LightsAurora },
	{ 0x187c, 0x518, "m18x", 2500, /* nothing much verified yet */
	  LightsM11x, sizeof LightsAurora / sizeof *LightsAurora },
	{ 0x187c, 0x524, "m17x", 2500, /* nothing much verified yet */
	  LightsAllPowerful, sizeof LightsAllPowerful / sizeof *LightsAllPowerful },
	{ 0x187c, 0x529, "13r3oled", 2500, /* 13 R3 OLED */
	  LightsAlienware13r3oled, sizeof LightsAlienware13r3oled / sizeof *LightsAlienware13r3oled },
	{ 0x187c, 0x530, "15r3", 2500, /* 15 R3 OLED */
	  LightsAlienware15r3, sizeof LightsAlienware15r3 / sizeof *LightsAlienware15r3},
};
int AlienFxTypesCount = sizeof AlienFxTypes / sizeof AlienFxTypes[0];

typedef struct _AlienFxHandle_t
{
	libusb_context       *usb_context;
	libusb_device_handle *usb_handle;
	AlienFxType_t        *info;	/* for device corresponding to the handle */
} AlienFxHandle_t;

void Syntax(FILE *out);

char *Progname = "<unknown>";
unsigned char debug = 0;
unsigned char verbose = 0;

unsigned int LightMask(AlienFxHandle_t *fx, char **names)
/* names should be an null-terminated array of (n-term') strings */
{
	unsigned int mask = 0;
	unsigned int i;
	for( /*prior*/ ; *names ; ++names) {
		char *name = *names;
		if(isdigit(name[0])) {
			unsigned int region = 0;
			if(1 == sscanf(name, "%x", & region)) {
				mask |= region;
			} else {
				fprintf(stderr, "error: could not convert \"%s\" to a region\n",
						name);
				break;
			}
		} else {
			int found = 0;
			for(i = 0 ; i < fx->info->lightsCount ; ++i)
				if( ! strcmp(fx->info->lights[i].name, name))
				{
					if(debug)
						printf("%#x | %#x = %#x\n",
							   mask,
							   fx->info->lights[i].id,
							   mask | fx->info->lights[i].id);
					mask |= fx->info->lights[i].id;
					found = 1;
				}
			if( ! found)
				fprintf(stderr, "error, no such light region \"%s\"\n", name);
		}
	}
	return mask;
}

void Detach(libusb_device_handle *device) {
	if( 1 == libusb_kernel_driver_active(device, 0))
		(void)libusb_detach_kernel_driver(device, 0);
}

void Attach(libusb_device_handle *device){
	libusb_attach_kernel_driver(device, 0);
}

int WriteDevice(libusb_device_handle *usb_handle, // return whether successful
				unsigned char *data,
				int data_bytes)
{
    unsigned char buf[SEND_DATA_SIZE];
    memset(&buf[0], FILL_BYTE, SEND_DATA_SIZE);
    memcpy(&buf[0], data, data_bytes);
    int written_bytes = libusb_control_transfer(usb_handle,
                                                SEND_REQUEST_TYPE,
                                                SEND_REQUEST,
                                                SEND_VALUE,
                                                SEND_INDEX,
                                                buf, SEND_DATA_SIZE, 0);
    if(written_bytes != SEND_DATA_SIZE)
        fprintf(stderr,
                "WriteDevice: wrote %d bytes instead of expected %d %s\n",
                written_bytes, data_bytes, strerror(errno));
    return (written_bytes == SEND_DATA_SIZE);
}

// return number of bytes read
int ReadDevice(libusb_device_handle *usb_device,
               unsigned char *data, // point to buffer to receive data
               int data_bytes)
{
    unsigned char buf[READ_DATA_SIZE];
    memset(&buf[0], FILL_BYTE, READ_DATA_SIZE);
	// returns can be negative for various errors, -6 is LIBUSB_ERROR_BUSY.
    int read_bytes = libusb_control_transfer(usb_device,
                                             READ_REQUEST_TYPE, READ_REQUEST,
                                             READ_VALUE, READ_INDEX,
                                             &buf[0], sizeof buf, 0);
    memcpy(data, &buf[0], read_bytes);
    if(read_bytes < data_bytes)
        memset(&data[read_bytes], '\0',
			   data_bytes - (read_bytes < 0 ? 0 : read_bytes));
    return read_bytes;
}

int InitDevice(AlienFxType_t *all, AlienFxHandle_t *fxh)
{
	int succp = 0;
	if(0 == libusb_init( & fxh->usb_context))	{
		libusb_set_debug(fxh->usb_context, 3);
		int i;
		for(i = 0 ; (i < AlienFxTypesCount) && ! succp ; ++i) {
			AlienFxType_t *fxtype = &all[i];
			if(verbose)
				printf("scanning for AlienFX type \"%s\"... ", fxtype->name);
			if(fxh->usb_handle =
			   libusb_open_device_with_vid_pid(fxh->usb_context,
											   fxtype->idVendor,
											   fxtype->idProduct))
			{
				fxh->info = fxtype;
				if(verbose)
					printf("found \"%s\".\n", fxh->info->name);
				succp = 1;
			} else if(verbose)
				puts("no.");
		}
		if(fxh->usb_handle) {
			Detach(fxh->usb_handle);
			if(0 > libusb_claim_interface(fxh->usb_handle, 0)) {
				perror("libusb_claim_interface");
				fxh->usb_handle = 0;
			}
		}
	} else perror("libusb_init");
	return succp;
}

void ReleaseDevice(AlienFxHandle_t *fx)
{
	libusb_release_interface(fx->usb_handle, 0);
	Attach(fx->usb_handle);
	libusb_close(fx->usb_handle);
	libusb_exit(fx->usb_context);
}

void ReleaseDeviceOld(libusb_device_handle *alienfx,
				   libusb_context *context)
{
	libusb_release_interface(alienfx, 0);
	Attach(alienfx);
	libusb_close(alienfx);
	libusb_exit(context);
}


int Reset(libusb_device_handle *alienfx, int reset_type)
{
    /* reset_type must be one of:
     *   RESET_CONTROLS_ON
     *   RESET_SLEEP_LIGHTS_ON
     *   RESET_ALL_LIGHTS_OFF
     *   RESET_ALL_LIGHTS_ON
     */
    unsigned char data[] = { START_BYTE, COMMAND_RESET,
                             (unsigned char)reset_type };
    return WriteDevice(alienfx, &data[0], sizeof data);
}

int SaveNext(libusb_device_handle *alienfx, int block)
{
    unsigned char data[] = { START_BYTE, COMMAND_SAVE_NEXT,
                             (unsigned char)block };
    return WriteDevice(alienfx, &data[0], sizeof data);
}

int SaveDone(libusb_device_handle *alienfx)
{
    unsigned char data[] = { START_BYTE, COMMAND_SAVE };
    return WriteDevice(alienfx, &data[0], sizeof data);
}

int ColorSet(libusb_device_handle *alienfx, int block, int region,
             int r, int g, int b)
{
    unsigned char green = (g >> 4) & 0x0f; // only used within red_green
    unsigned char red   = (r >> 0) & 0xf0; // only used within red_green
    unsigned char blue  = (b >> 0) & 0xf0;
    unsigned char red_green = red | green;

    unsigned char reg1 = (unsigned char)(((unsigned int)region >> 16) & 0xff);
    unsigned char reg2 = (unsigned char)(((unsigned int)region >>  8) & 0xff);
    unsigned char reg3 = (unsigned char)(((unsigned int)region >>  0) & 0xff);

    unsigned char data[] = { START_BYTE, COMMAND_SET_COLOR,
                             (block & 0xFF),
                             reg1, // 0xff for *all* regions
                             reg2, // 0xff for *all* regions
                             reg3, // 0xff for *all* regions
                             red_green,
                             blue,
                             0 };
    return WriteDevice(alienfx, &data[0], sizeof data);
}

int ColorSetAlien15r3(libusb_device_handle *alienfx, int block, int region,
             int r, int g, int b)
{
    unsigned char red = (char) (r & 0xff);
    unsigned char green = (char) (g & 0xff);
    unsigned char blue = (char) (b & 0xff);



    unsigned char reg1 = (unsigned char)(((unsigned int)region >> 16) & 0xff);
    unsigned char reg2 = (unsigned char)(((unsigned int)region >>  8) & 0xff);
    unsigned char reg3 = (unsigned char)(((unsigned int)region >>  0) & 0xff);

    unsigned char data[] = { START_BYTE, COMMAND_SET_COLOR,
                             (block & 0xFF),
                             reg1, // 0xff for *all* regions
                             reg2, // 0xff for *all* regions
                             reg3, // 0xff for *all* regions
                             red,
                             blue,
                             green};
    return WriteDevice(alienfx, &data[0], sizeof data);
}

int ColorSetAll(libusb_device_handle *alienfx, int block, int r, int g, int b, bool isAlien15r3)
{
     
    if(isAlien15r3){
      return ColorSetAlien15r3(alienfx, block, 0xffffff, r, g, b);
    } else{
      return ColorSet(alienfx, block, 0xffffff, r, g, b);
    }

}

int SendAndExec(libusb_device_handle *alienfx)
{
    unsigned char data[] = { START_BYTE, COMMAND_TRANSMIT_EXECUTE };
    return WriteDevice(alienfx, &data[0], sizeof data);
}

int Loop(libusb_device_handle *alienfx) // preceding commands are a loop
{
    unsigned char data[] = { START_BYTE, COMMAND_LOOP_BLOCK_END };
    return WriteDevice(alienfx, &data[0], sizeof data);
}

int IsBusy(libusb_device_handle *alienfx, int *is_busy_return)
/*:note: The is_busy_return should be set to 1 if the device is busy,
 *       but hasn't yet been implemented.
 */
{
    int succp = 0;
	int read_bytes = 0;
	unsigned char data[] = { START_BYTE, COMMAND_GET_STATUS };
    unsigned char data_input[READ_DATA_SIZE];

	read_bytes = ReadDevice(alienfx, &data_input[0], READ_DATA_SIZE);
    if(0 < read_bytes) {
        succp = (START_BYTE == data_input[0]);
    } else {
		if(LIBUSB_ERROR_BUSY == read_bytes)
			*is_busy_return = 1;
        fprintf(stderr, "ReadDevice: failed to read any bytes: %s\n",
                strerror(errno));
    }
    return succp;
}

int CageInt(int min, int num, int max)
{
    return ((num <= min)
            ? min
            : ((num >= max)
               ? max
               : num ));
}

int SetDelay(libusb_device_handle *alienfx, unsigned int delay)
// delay = [MIN_SPEED, MAX_SPEED], but it's a delay
{
    delay = CageInt(MIN_SPEED, delay, MAX_SPEED);
    delay = (delay / STEP_SPEED) * STEP_SPEED; // quantize to step multiple
    unsigned char b1 = (delay >> 8) & 0xff;
    unsigned char b2 = (delay >> 0) & 0xff;
 
    unsigned char data[] = { START_BYTE, COMMAND_SET_SPEED, b1, b2 };
    return WriteDevice(alienfx, &data[0], sizeof data);
}

void WhenReady(libusb_device_handle *alienfx)
{
    int is_busy = 0;
    do {
        IsBusy(alienfx, & is_busy);
        if(is_busy)
            usleep(1000);
    } while(is_busy);
}

int CommandCount(char **cmdv) { return Sscount(cmdv); }

int Command(AlienFxHandle_t *fx, char **cmdv)
{
    int cmdc = CommandCount(cmdv); // cmdv[cmdc] == (char*)0
	if(debug)
		fprintf(stderr, "command %s with cmdc %d\n", *cmdv, cmdc);
    if( ! strcmp("color", cmdv[0])) {
        if(cmdc < 4) {
            fputs("error: too few arguments to command \"color\"\n", stderr);
            return 0;
        } else {
            int block = BLOCK_AC_POWER; // int block = BLOCK_LOAD_ON_BOOT
			char *all[] = { "all", 0 };
			unsigned int region = LightMask(fx, all);
            int r = 0, g = 0, b = 0;
            if(   (1 == sscanf(cmdv[1], "%d", &r))
                  && (1 == sscanf(cmdv[2], "%d", &g))
                  && (1 == sscanf(cmdv[3], "%d", &b)))
              {
                if(cmdc > 4)
                  region = LightMask(fx, & cmdv[4]);
                if(debug)
                  fprintf(stderr, "command: color %d %d %d %x\n",
                          r,g,b, region);
                Reset(fx->usb_handle, RESET_ALL_LIGHTS_ON);
                /* apparently some types may need a brief pause here */
                usleep(fx->info->post_reset_delay);
		// both 13f3 and 15r3 have this changed API
                if (fx->info->idProduct == 0x530 || fx->info->idProduct == 0x529){
                  ColorSetAlien15r3(fx->usb_handle, block, region, r, g, b);
                } else{
                  ColorSet(fx->usb_handle, block, region, r, g, b);
                }
                Loop(fx->usb_handle);
                SendAndExec(fx->usb_handle);
              }
        }
	} else if( ! strcmp("info", cmdv[0])) {
		if(verbose) {
			printf("  idVendor  %#6x \n", fx->info->idVendor);
			printf("  idProduct %#6x %s\n",
				   fx->info->idProduct, fx->info->name);
		}
		unsigned int i;
		unsigned int primitives = 0;
		unsigned int composites = 0;
		for(i = 0 ; i < fx->info->lightsCount ; ++i)
			if(fx->info->lights[i].is_composite)
				++composites;
			else
				++primitives;
		printf("  light primitives %d\n", primitives);
		printf("  light composites %d\n", composites);
		printf("  region type      name\n");
		printf("  ------ --------- ----\n");
		for(i = 0 ; i < fx->info->lightsCount ; ++i)
			printf("  %#6x %s %s\n",
				   fx->info->lights[i].id,
				   fx->info->lights[i].is_composite ? "composite" : "primitive",
				   fx->info->lights[i].name);
		if(debug) {
			struct libusb_device *usb_dev = 0;
			struct libusb_device_descriptor usb_desc;
			if(   (usb_dev = libusb_get_device(fx->usb_handle))
			   && (0 == libusb_get_device_descriptor(usb_dev, & usb_desc)))
			{
				printf("  bLength %d\n",            usb_desc.bLength);
				printf("  bDescriptorType %d\n",    usb_desc.bDescriptorType);
				printf("  bcdUSB %d\n",             usb_desc.bcdUSB);
				printf("  bDeviceClass %d\n",       usb_desc.bDeviceClass);
				printf("  bDeviceSubClass %d\n",    usb_desc.bDeviceSubClass);
				printf("  bDeviceProtocol %d\n",    usb_desc.bDeviceProtocol);
				printf("  bMaxPacketSize0 %d\n",    usb_desc.bMaxPacketSize0);
				printf("  idVendor %#x\n",          usb_desc.idVendor);
				printf("  idProduct %#x\n",         usb_desc.idProduct);
				printf("  bcdDevice %d\n",          usb_desc.bcdDevice);
				printf("  iManufacturer %#x\n",     usb_desc.iManufacturer);
				printf("  iProduct %#x\n",          usb_desc.iProduct);
				printf("  iSerialNumber %#x\n",     usb_desc.iSerialNumber);
				printf("  bNumConfigurations %d\n", usb_desc.bNumConfigurations);
			}
		}
    } else {
		fprintf(stderr, "unrecognized command \"%s\"\n", cmdv[0]);
		Syntax(stderr);
	}
    return 1;
}

void Syntax(FILE *out)
{
	fprintf(out,
			"Syntax: %s [ -d|-v ] [ info | color <r> <g> <b> [ <region>... ]\n",
			Progname);
	fputs("     -v  verbose: show device scanning and such\n", out);
	fputs("     -d  debug: possibly show parts used during programming\n", out);
	fputs("   info  show the subsystem's USB dev IDs and color regions\n", out);
	fputs("  color  set the color to <r> <g> <b>, each range [0-255]\n", out);
	fputs(" region  set the target region by name or hexadecimal ID\n", out);
	fputs("All <region>s given are combined in a single target\n", out);
	fputs("The \"info\" command is augmented by -v and -d\n", out);
}

int main(int ac, char **av)
{
    int succp = 1;
	Progname = av[0];
	AlienFxHandle_t fx = {0,};  // relying on the compiler to obviate memset.
	int ai = 1;
	while(av[ai] && ('-' == av[ai][0]) && (1 < strlen(av[ai]))) {
		// a simple "-" is handled later
		if(      ! strcmp("-v", av[ai])) verbose = 1;
		else if( ! strcmp("-d", av[ai])) debug  = 1;
		else if( ! strcmp("-h", av[ai])) { Syntax(stdout); return 0; }
		else { Syntax(stderr); return -1; }
		++ai;
	}
	if(ai >= ac) { Syntax(stderr); return -1; }
	if(InitDevice(AlienFxTypes, &fx))
	{
        int block = BLOCK_AC_POWER; // int block = BLOCK_LOAD_ON_BOOT;
        SetDelay(fx.usb_handle, MIN_SPEED);
        if(av[ai] && ! strcmp("-", av[ai])) // read from stdin
        {
            char *line = 0;
            while(line = Fgetstr(stdin, "\n")) {
                char **cmdv = 0;
                if(cmdv = Stoss(" \t", line)) {
                    succp = Command( & fx, cmdv);
                    free(cmdv);
                }
                free(line);
            }
            return succp;
        } else {              // r g b hex-region...
            succp = Command( & fx, & av[ai]);
        }
        ReleaseDevice( & fx);
	} else
		fputs("No recognized AlienFX device found.\n", stderr);

    return succp ? 0 : -1;
}

// ------------------------------------------------------------- eof
