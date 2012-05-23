/*
 * NMEA2000 over CAN.
 *
 * This file is Copyright (c) 2012 by the GPSD project
 * BSD terms apply: see the file COPYING in the distribution root for details.
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <termios.h>
#include <time.h>
#include <math.h>
#ifndef S_SPLINT_S
#include <unistd.h>
#include <sys/socket.h>
#endif /* S_SPLINT_S */

#include "gpsd.h"
#if defined(NMEA2000_ENABLE)
#include "bits.h"

#ifndef S_SPLINT_S
#include <linux/can.h>
#include <linux/can/raw.h>
#endif /* S_SPLINT_S */

#define LOG_FILE 1

typedef struct PGN
    {
    unsigned int  pgn;
    unsigned int  fast;
    unsigned int  type;
    gps_mask_t    (* func)(unsigned char *bu, int len, struct PGN *pgn, struct gps_device_t *session);
    const char    *name;
    } PGN;

/*@-nullassign@*/

#if LOG_FILE
FILE *logFile = NULL;
#endif /* of if LOG_FILE */

static int print_data(unsigned char *buffer, int len, PGN *pgn)
{
    /*@-bufferoverflowhigh@*/
    if ((libgps_debuglevel >= LOG_IO) != 0) {
	int   l1, l2, ptr;
	char  bu[128];

        ptr = 0;
        l2 = sprintf(&bu[ptr], "got data:%6u:%3d: ", pgn->pgn, len);
	ptr += l2;
        for (l1=0;l1<len;l1++) {
            if (((l1 % 20) == 0) && (l1 != 0)) {
	        gpsd_report(LOG_IO,"%s\n", bu);
		ptr = 0;
                l2 = sprintf(&bu[ptr], "                   : ");
		ptr += l2;
            }
            l2 = sprintf(&bu[ptr], "%02ux ", (unsigned int)buffer[l1]);
	    ptr += l2;
        }
        gpsd_report(LOG_IO,"%s\n", bu);
    }
    /*@+bufferoverflowhigh@*/
    return(0);
}

static gps_mask_t get_mode(struct gps_device_t *session)
{
    if (session->driver.nmea2000.mode_valid) {
        session->newdata.mode = session->driver.nmea2000.mode;
    } else {
        session->newdata.mode = MODE_NOT_SEEN;
    }

    return MODE_SET;
}


static gps_mask_t hnd_059392(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_060928(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_126208(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_126464(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_126996(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_129025(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);

    /*@-type@*//* splint has a bug here */
    session->newdata.latitude = getles32(bu, 0) * 1e-7;
    session->newdata.longitude = getles32(bu, 4) * 1e-7;
    /*@+type@*/

    (void)strlcpy(session->gpsdata.tag, "129025", sizeof(session->gpsdata.tag));

    return LATLON_SET | get_mode(session);
}


static gps_mask_t hnd_129026(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);

    session->driver.nmea2000.sid[0]  =  bu[0];

    /*@-type@*//* splint has a bug here */
    session->newdata.track           =  getleu16(bu, 2) * 1e-4 * RAD_2_DEG;
    session->newdata.speed           =  getleu16(bu, 4) * 1e-2;
    /*@+type@*/

    (void)strlcpy(session->gpsdata.tag, "129026", sizeof(session->gpsdata.tag));

    return SPEED_SET | TRACK_SET | get_mode(session);
}


static gps_mask_t hnd_126992(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    //uint8_t        sid;
    //uint8_t        source;

    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);

    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);

    //sid        = bu[0];
    //source     = bu[1] & 0x0f;

    /*@-type@*//* splint has a bug here */
    session->newdata.time = getleu16(bu, 2)*24*60*60 + getleu32(bu, 4)/1e4;
    /*@+type@*/

    (void)strlcpy(session->gpsdata.tag, "126992", sizeof(session->gpsdata.tag));

    return TIME_SET | get_mode(session);
}


static const int mode_tab[] = {MODE_NO_FIX, MODE_2D,  MODE_3D, MODE_NO_FIX,
			       MODE_NO_FIX, MODE_NO_FIX, MODE_NO_FIX, MODE_NO_FIX};

static gps_mask_t hnd_129539(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    gps_mask_t mask;

    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);

    mask                             = 0;
    session->driver.nmea2000.sid[1]  = bu[0];

    session->driver.nmea2000.mode_valid = 1;

    session->driver.nmea2000.mode    = mode_tab[(bu[1] >> 3) & 0x07];

    /*@-type@*//* splint has a bug here */
    session->gpsdata.dop.hdop        = getleu16(bu, 2) * 1e-2;
    session->gpsdata.dop.vdop        = getleu16(bu, 4) * 1e-2;
    session->gpsdata.dop.tdop        = getleu16(bu, 6) * 1e-2;
    /*@+type@*/
    mask                            |= DOP_SET;

    gpsd_report(LOG_DATA, "pgn %6d(%3d): sid:%02x hdop:%5.2f vdop:%5.2f tdop:%5.2f\n",
		pgn->pgn,
		session->driver.nmea2000.unit,
		session->driver.nmea2000.sid[1],
		session->gpsdata.dop.hdop,
		session->gpsdata.dop.vdop,
		session->gpsdata.dop.tdop);

    (void)strlcpy(session->gpsdata.tag, "129539", sizeof(session->gpsdata.tag));

    return mask | get_mode(session);
}


static gps_mask_t hnd_129540(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    int         l1, l2;

    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);

    session->driver.nmea2000.sid[2]           = bu[0];
    session->gpsdata.satellites_visible       = (int)bu[2];

    for (l2=0;l2<MAXCHANNELS;l2++) {
        session->gpsdata.used[l2] = 0;
    }
    l2 = 0;
    for (l1=0;l1<session->gpsdata.satellites_visible;l1++) {
        int    svt;
        double azi, elev, snr;

	/*@-type@*//* splint has a bug here */
        elev  = getles16(bu, 3+12*l1+1) * 1e-4 * RAD_2_DEG;
        azi   = getleu16(bu, 3+12*l1+3) * 1e-4 * RAD_2_DEG;
        snr   = getles16(bu, 3+12*l1+5) * 1e-2;
	/*@+type@*/

        svt   = (int)(bu[3+12*l1+11] & 0x0f);

        session->gpsdata.elevation[l1]  = (int) (round(elev));
	session->gpsdata.azimuth[l1]    = (int) (round(azi));
        session->gpsdata.ss[l1]         = snr;
        session->gpsdata.PRN[l1]        = (int)bu[3+12*l1+0];
	if ((svt == 2) || (svt == 5)) {
	    session->gpsdata.used[l2] = session->gpsdata.PRN[l1];
	    l2 += 1;
	}
    }
    return  SATELLITE_SET | USED_IS;
}


static gps_mask_t hnd_129029(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    gps_mask_t mask;

    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);

    mask                             = 0;
    session->driver.nmea2000.sid[3]  = bu[0];
 
    /*@-type@*//* splint has a bug here */
    session->newdata.time            = getleu16(bu,1) * 24*60*60 + getleu32(bu, 3)/1e4;
    /*@+type@*/
    mask                            |= TIME_SET;

    /*@-type@*//* splint has a bug here */
    session->newdata.latitude        = getles64(bu, 7) * 1e-16;
    session->newdata.longitude       = getles64(bu, 15) * 1e-16;
    /*@+type@*/
    mask                            |= LATLON_SET;

    /*@-type@*//* splint has a bug here */
    session->newdata.altitude        = getles64(bu, 23) * 1e-6;
    /*@+type@*/
    mask                            |= ALTITUDE_SET;

//  printf("mode %x %x\n", (bu[31] >> 4) & 0x0f, bu[31]);
    switch ((bu[31] >> 4) & 0x0f) {
    case 0:
        session->gpsdata.status      = STATUS_NO_FIX;
	break;
    case 1:
        session->gpsdata.status      = STATUS_FIX;
	break;
    case 2:
        session->gpsdata.status      = STATUS_DGPS_FIX;
	break;
    case 3:
    case 4:
    case 5:
        session->gpsdata.status      = STATUS_FIX; /* Is this correct ? */
	break;
    default:
        session->gpsdata.status      = STATUS_NO_FIX;
	break;
    }
    mask                            |= STATUS_SET;

    /*@-type@*//* splint has a bug here */
    session->gpsdata.separation      = getles32(bu, 38) / 100.0;
    /*@+type@*/
    session->newdata.altitude       -= session->gpsdata.separation;

    session->gpsdata.satellites_used = (int)bu[33];

    /*@-type@*//* splint has a bug here */
    session->gpsdata.dop.hdop        = getleu16(bu, 34) * 0.01;
    session->gpsdata.dop.pdop        = getleu16(bu, 36) * 0.01;
    /*@+type@*/
    mask                            |= DOP_SET;

    (void)strlcpy(session->gpsdata.tag, "129029", sizeof(session->gpsdata.tag));

    return mask | get_mode(session);
}


static gps_mask_t hnd_129038(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_129039(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_129040(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_129794(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_129798(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_129802(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_129809(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}


static gps_mask_t hnd_129810(unsigned char *bu, int len, PGN *pgn, struct gps_device_t *session)
{
    (void)print_data(bu, len, pgn);
    gpsd_report(LOG_DATA, "pgn %6d(%3d):\n", pgn->pgn, session->driver.nmea2000.unit);
    return(0);
}

/*@-usereleased@*/
static const char msg_059392[] = {"ISO  Acknowledgment"};
static const char msg_060928[] = {"ISO  Address Claim"};
static const char msg_126208[] = {"NMEA Command/Request/Acknowledge"};
static const char msg_126464[] = {"ISO  Transmit/Receive PGN List"};
static const char msg_126992[] = {"GNSS System Time"};
static const char msg_126996[] = {"ISO  Product Information"};
static const char msg_129025[] = {"GNSS Position Rapid Update"};
static const char msg_129026[] = {"GNSS COG and SOG Rapid Update"};
static const char msg_129029[] = {"GNSS Positition Data"};
static const char msg_129539[] = {"GNSS DOPs"};
static const char msg_129540[] = {"GNSS Satellites in View"};
static const char msg_129038[] = {"AIS  Class A Position Report"};
static const char msg_129039[] = {"AIS  Class B Position Report"};
static const char msg_129040[] = {"AIS  Class B Extended Position Report"};
static const char msg_129794[] = {"AIS  Class A Static and Voyage Related Data"};
static const char msg_129798[] = {"AIS  SAR Aircraft Position Report"};
static const char msg_129802[] = {"AIS  Safty Related Broadcast Message"};
static const char msg_129809[] = {"AIS  Class B CS Static Data Report, Part A"};
static const char msg_129810[] = {"AIS  Class B CS Static Data Report, Part B"};
static const char msg_error [] = {"**error**"};

static PGN gpspgn[] = {{ 59392, 0, 0, hnd_059392, &msg_059392[0]},
		       { 60928, 0, 0, hnd_060928, &msg_060928[0]},
		       {126208, 0, 0, hnd_126208, &msg_126208[0]},
		       {126464, 1, 0, hnd_126464, &msg_126464[0]},
		       {126992, 0, 0, hnd_126992, &msg_126992[0]},
		       {126996, 1, 0, hnd_126996, &msg_126996[0]},
		       {129025, 0, 1, hnd_129025, &msg_129025[0]},
		       {129026, 0, 1, hnd_129026, &msg_129026[0]},
		       {129029, 1, 1, hnd_129029, &msg_129029[0]},
		       {129539, 0, 1, hnd_129539, &msg_129539[0]},
		       {129540, 1, 1, hnd_129540, &msg_129540[0]},
		       {0     , 0, 0, NULL,       &msg_error [0]}};

static PGN aispgn[] = {{ 59392, 0, 0, hnd_059392, &msg_059392[0]},
		       { 60928, 0, 0, hnd_060928, &msg_060928[0]},
		       {126208, 0, 0, hnd_126208, &msg_126208[0]},
		       {126464, 1, 0, hnd_126464, &msg_126464[0]},
		       {126992, 0, 0, hnd_126992, &msg_126992[0]},
		       {126996, 1, 0, hnd_126996, &msg_126996[0]},
		       {129038, 1, 2, hnd_129038, &msg_129038[0]},
		       {129039, 1, 2, hnd_129039, &msg_129039[0]},
		       {129040, 1, 2, hnd_129040, &msg_129040[0]},
		       {129794, 1, 2, hnd_129794, &msg_129794[0]},
		       {129798, 1, 2, hnd_129798, &msg_129798[0]},
		       {129802, 1, 2, hnd_129802, &msg_129802[0]},
		       {129809, 1, 2, hnd_129809, &msg_129809[0]},
		       {129810, 1, 2, hnd_129810, &msg_129810[0]},
		       {0     , 0, 0, NULL,       &msg_error [0]}};
/*@+usereleased@*/

/*@-immediatetrans@*/
static /*@null@*/ PGN *search_pgnlist(unsigned int pgn, PGN *pgnlist)
{
    int l1;
    PGN *work;

    l1 = 0;
    work = NULL;
    while (pgnlist[l1].pgn != 0) {
        if (pgnlist[l1].pgn == pgn) {
	    work = &pgnlist[l1];
	    break;
	} else {
	    l1 = l1 + 1;
	    }
	}
    return work;
}
/*@+immediatetrans@*/

/*@-nullstate -branchstate -globstate -mustfreeonly@*/
static void find_pgn(struct can_frame *frame, struct gps_device_t *session)
{
    PGN *work;

    session->driver.nmea2000.workpgn = NULL;

    /*@ignore@*//* because the CAN include files choke splint */
#if LOG_FILE
    if (frame->can_id & 0x80000000) {
	// cppcheck-suppress unreadVariable
	unsigned int source_prio UNUSED;
	unsigned int source_pgn;
	unsigned int source_unit;
	// cppcheck-suppress unreadVariable
	unsigned int daddr UNUSED;

        if (logFile != NULL) {
	    struct timespec  msgTime;

	    clock_gettime(CLOCK_REALTIME, &msgTime);
	    fprintf(logFile,
		    "(%010d.%06d) can0 %08x#",
		    (unsigned int)msgTime.tv_sec,
		    (unsigned int)msgTime.tv_nsec/1000,
		    frame->can_id & 0x1ffffff);
	    if ((frame->can_dlc & 0x0f) > 0) {
		int l1;
	        for(l1=0;l1<(frame->can_dlc & 0x0f);l1++) {
		    fprintf(logFile, "%02x", frame->data[l1]);
		}
	    }
	    fprintf(logFile, "\n");
	}
#endif /* of if LOG_FILE */
	/*@end@*/
	session->driver.nmea2000.can_msgcnt += 1;
	/*@ignore@*//* because the CAN include files choke splint */
	source_pgn = (frame->can_id >> 8) & 0x1ffff;
	source_prio = (frame->can_id >> 26) & 0x7;
	source_unit = frame->can_id & 0x0ff;
	/*@end@*/

	if ((source_pgn >> 8) < 240) {
	    daddr  = source_pgn & 0x000ff;
	    source_pgn  = source_pgn & 0x1ff00;
	} else {
	    daddr = 0;
	}

	if (session->driver.nmea2000.unit_valid == 0) {
	    session->driver.nmea2000.unit = source_unit;
	    session->driver.nmea2000.unit_valid = 1;
	}

	if (source_unit == session->driver.nmea2000.unit) {
	    if (session->driver.nmea2000.pgnlist != NULL) {
	        work = search_pgnlist(source_pgn, session->driver.nmea2000.pgnlist);
	    } else {
	        PGN *pgnlist;
	    
		pgnlist = &gpspgn[0];
		work = search_pgnlist(source_pgn, pgnlist);
		if (work == NULL) {
		    pgnlist = &aispgn[0];
		    work = search_pgnlist(source_pgn, pgnlist);
		}
		if ((work != 0) && (work->type > 0)) {
		    session->driver.nmea2000.pgnlist = pgnlist;
		}
	    }
	    if (work != NULL) {
	        if (work->fast == 0) {
		    size_t l2;

		    gpsd_report(LOG_DATA, "pgn %6d:%s \n", work->pgn, work->name);
		    session->driver.nmea2000.workpgn = (void *) work;
		    session->driver.nmea2000.idx = 0;
		    session->driver.nmea2000.ptr = 0;
		    /*@i1@*/session->packet.outbuflen =  frame->can_dlc & 0x0f;
		    for (l2=0;l2<session->packet.outbuflen;l2++) {
		        /*@i3@*/session->packet.outbuffer[session->driver.nmea2000.ptr++]= frame->data[l2];
		    }
		}
		/*@i2@*/else if ((frame->data[0] & 0x1f) == 0) {
		    unsigned int l2;

		    /*@i2@*/session->driver.nmea2000.fast_packet_len = frame->data[1];
		    /*@i2@*/session->driver.nmea2000.idx = frame->data[0];
		    session->driver.nmea2000.ptr = 0;
		    session->driver.nmea2000.idx += 1;
		    for (l2=2;l2<8;l2++) {
		        /*@i3@*/session->packet.outbuffer[session->driver.nmea2000.ptr++]= frame->data[l2];
		    }
		    gpsd_report(LOG_DATA, "pgn %6d:%s \n", work->pgn, work->name);
		}
		/*@i2@*/else if (frame->data[0] == session->driver.nmea2000.idx) {
		    unsigned int l2;

		    for (l2=1;l2<8;l2++) {
		        if (session->driver.nmea2000.fast_packet_len > session->driver.nmea2000.ptr) {
			    /*@i3@*/session->packet.outbuffer[session->driver.nmea2000.ptr++] = frame->data[l2];
			}
		    }
		    if (session->driver.nmea2000.ptr == session->driver.nmea2000.fast_packet_len) {
		        session->driver.nmea2000.workpgn = (void *) work;
		        session->packet.outbuflen = session->driver.nmea2000.fast_packet_len;
			session->driver.nmea2000.fast_packet_len = 0;
		    } else {
		        session->driver.nmea2000.idx += 1;
		    }
		} else {
		    session->driver.nmea2000.idx = 0;
		    session->driver.nmea2000.fast_packet_len = 0;
		    gpsd_report(LOG_ERROR, "Fast error\n");
		}
	    } else {
	        // we got a unknown unit number
	    }
	} else {
	    // we got RTR or 2.0A CAN frame, not used
	}
    }
}
/*@+nullstate +branchstate +globstate +mustfreeonly@*/


static ssize_t nmea2000_get(struct gps_device_t *session)
{   
    struct can_frame frame;
    ssize_t          status;

//  printf("NMEA2000 get: enter\n");
    session->packet.outbuflen = 0;
    status = read(session->gpsdata.gps_fd, &frame, sizeof(frame));
    if (status == (ssize_t)sizeof(frame)) {
        session->packet.type = NMEA2000_PACKET;
	find_pgn(&frame, session);
//	printf("NMEA2000 get: exit(%d)\n", status);
	if (session->driver.nmea2000.workpgn == NULL) {
	    status = 0;
	}
        return frame.can_dlc & 0x0f;
    }
//  printf("NMEA2000 get: exit(0)\n");
    return 0;
}

/*@-mustfreeonly@*/
static gps_mask_t nmea2000_parse_input(struct gps_device_t *session)
{    
    gps_mask_t mask;
    PGN *work;
 
//  printf("NMEA2000 parse_input called\n");
    mask = 0;
    work = (PGN *) session->driver.nmea2000.workpgn;

    if (work != NULL) {
        mask = (work->func)(&session->packet.outbuffer[0], (int)session->packet.outbuflen, work, session);
        session->driver.nmea2000.workpgn = NULL;
    }
    session->packet.outbuflen = 0;

    return mask;
}
/*@+mustfreeonly@*/

/*@+nullassign@*/

/* *INDENT-OFF* */
const struct gps_type_t nmea2000 = {
    .type_name      = "NMEA2000",       /* full name of type */
    .packet_type    = NMEA2000_PACKET,	/* associated lexer packet type */
    .flags	    = DRIVER_NOFLAGS,	/* no rollover or other flags */
    .trigger	    = NULL,		/* detect their main sentence */
    .channels       = 12,		/* not an actual GPS at all */
    .probe_detect   = NULL,
    .get_packet     = nmea2000_get,	/* how to get a packet */
    .parse_packet   = nmea2000_parse_input,	/* how to interpret a packet */
    .rtcm_writer    = NULL,		/* Don't send RTCM to this */
    .event_hook     = NULL,
#ifdef RECONFIGURE_ENABLE
    .speed_switcher = NULL,		/* no speed switcher */
    .mode_switcher  = NULL,		/* no mode switcher */
    .rate_switcher  = NULL,		/* no rate switcher */
    .min_cycle      = 1,		/* nominal 1-per-second GPS cycle */
#endif /* RECONFIGURE_ENABLE */
#ifdef CONTROLSEND_ENABLE
    .control_send   = NULL,		/* how to send control strings */
#endif /* CONTROLSEND_ENABLE */
#ifdef NTPSHM_ENABLE
    .ntp_offset     = NULL,
#endif /* NTPSHM_ ENABLE */
};
/* *INDENT-ON* */

/* end */

#endif /* of  defined(NMEA2000_ENABLE) */
