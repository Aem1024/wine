/*
 * Test winmm sound capture in each sound format
 *
 * Copyright (c) 2002 Francois Gouget
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "wine/test.h"
#include "windef.h"
#include "winbase.h"
#include "winnls.h"
#include "mmsystem.h"
#include "mmddk.h"

#include "winmm_test.h"

static const char * wave_in_error(MMRESULT error)
{
    static char msg[1024];
    static char long_msg[1100];
    MMRESULT rc;

    rc = waveInGetErrorText(error, msg, sizeof(msg));
    if (rc != MMSYSERR_NOERROR)
        sprintf(long_msg, "waveInGetErrorText(%x) failed with error %x", error, rc);
    else
        sprintf(long_msg, "%s(%s)", mmsys_error(error), msg);
    return long_msg;
}

static void wave_in_test_deviceIn(int device, LPWAVEFORMATEX pwfx, DWORD format, DWORD flags, LPWAVEINCAPS pcaps)
{
    HWAVEIN win;
    HANDLE hevent;
    WAVEHDR frag;
    MMRESULT rc;
    DWORD res;
    WORD nChannels = pwfx->nChannels;
    WORD wBitsPerSample = pwfx->wBitsPerSample;
    DWORD nSamplesPerSec = pwfx->nSamplesPerSec;

    hevent=CreateEvent(NULL,FALSE,FALSE,NULL);
    ok(hevent!=NULL,"CreateEvent: error=%ld\n",GetLastError());
    if (hevent==NULL)
        return;

    win=NULL;
    rc=waveInOpen(&win,device,pwfx,(DWORD)hevent,0,CALLBACK_EVENT|flags);
    /* Note: Win9x doesn't know WAVE_FORMAT_DIRECT */
    ok(rc==MMSYSERR_NOERROR || rc==MMSYSERR_BADDEVICEID ||
       ((rc==WAVERR_BADFORMAT || rc==MMSYSERR_NOTSUPPORTED) &&
       (flags & WAVE_FORMAT_DIRECT) && !(pcaps->dwFormats & format)) ||
       ((rc==WAVERR_BADFORMAT || rc==MMSYSERR_NOTSUPPORTED) &&
       (!(flags & WAVE_FORMAT_DIRECT) || (flags & WAVE_MAPPED)) && !(pcaps->dwFormats & format)) ||
       (rc==MMSYSERR_INVALFLAG && (flags & WAVE_FORMAT_DIRECT)),
       "waveInOpen: device=%d format=%ldx%2dx%d flags=%lx(%s) rc=%s\n",device,
       pwfx->nSamplesPerSec,pwfx->wBitsPerSample,pwfx->nChannels,CALLBACK_EVENT|flags,
       wave_open_flags(CALLBACK_EVENT|flags),wave_in_error(rc));
    if ((rc==WAVERR_BADFORMAT || rc==MMSYSERR_NOTSUPPORTED) &&
       (flags & WAVE_FORMAT_DIRECT) && (pcaps->dwFormats & format))
        trace(" Reason: The device lists this format as supported in it's capabilities but opening it failed.\n");
    if ((rc==WAVERR_BADFORMAT || rc==MMSYSERR_NOTSUPPORTED) &&
       !(pcaps->dwFormats & format))
        trace("waveInOpen: device=%d format=%ldx%2dx%d %s rc=%s failed but format not supported so OK.\n",
              device, pwfx->nSamplesPerSec,pwfx->wBitsPerSample,pwfx->nChannels,
              flags & WAVE_FORMAT_DIRECT ? "flags=WAVE_FORMAT_DIRECT" :
              flags & WAVE_MAPPED ? "flags=WAVE_MAPPED" : "", mmsys_error(rc));
    if (rc!=MMSYSERR_NOERROR) {
        CloseHandle(hevent);
        return;
    }
    res=WaitForSingleObject(hevent,1000);
    ok(res==WAIT_OBJECT_0,"WaitForSingleObject failed for open\n");

    ok(pwfx->nChannels==nChannels &&
       pwfx->wBitsPerSample==wBitsPerSample &&
       pwfx->nSamplesPerSec==nSamplesPerSec,
       "got the wrong format: %ldx%2dx%d instead of %ldx%2dx%d\n",
       pwfx->nSamplesPerSec, pwfx->wBitsPerSample,
       pwfx->nChannels, nSamplesPerSec, wBitsPerSample, nChannels);

    frag.lpData=malloc(pwfx->nAvgBytesPerSec);
    frag.dwBufferLength=pwfx->nAvgBytesPerSec;
    frag.dwBytesRecorded=0;
    frag.dwUser=0;
    frag.dwFlags=0;
    frag.dwLoops=0;
    frag.lpNext=0;

    rc=waveInPrepareHeader(win, &frag, sizeof(frag));
    ok(rc==MMSYSERR_NOERROR, "waveInPrepareHeader: device=%d rc=%s\n",device,wave_in_error(rc));
    ok(frag.dwFlags&WHDR_PREPARED,"waveInPrepareHeader: prepared flag not set\n");

    if (winetest_interactive && rc==MMSYSERR_NOERROR) {
        trace("Recording for 1 second at %5ldx%2dx%d %s\n",
              pwfx->nSamplesPerSec, pwfx->wBitsPerSample,pwfx->nChannels,
              flags & WAVE_FORMAT_DIRECT ? "WAVE_FORMAT_DIRECT" :
              flags & WAVE_MAPPED ? "WAVE_MAPPED" : "");
        rc=waveInAddBuffer(win, &frag, sizeof(frag));
        ok(rc==MMSYSERR_NOERROR,"waveInAddBuffer: device=%d rc=%s\n",device,wave_in_error(rc));

        rc=waveInStart(win);
        ok(rc==MMSYSERR_NOERROR,"waveInStart: device=%d rc=%s\n",device,wave_in_error(rc));

        res = WaitForSingleObject(hevent,1200);
        ok(res==WAIT_OBJECT_0,"WaitForSingleObject failed for header\n");
        ok(frag.dwFlags&WHDR_DONE,"WHDR_DONE not set in frag.dwFlags\n");
        ok(frag.dwBytesRecorded==pwfx->nAvgBytesPerSec,"frag.dwBytesRecorded=%ld, should=%ld\n",
           frag.dwBytesRecorded,pwfx->nAvgBytesPerSec);
        /* stop playing on error */
        if (res!=WAIT_OBJECT_0) {
            rc=waveInStop(win);
            ok(rc==MMSYSERR_NOERROR,
               "waveInStop: device=%d rc=%s\n",device,wave_in_error(rc));
        }
    }

    rc=waveInUnprepareHeader(win, &frag, sizeof(frag));
    ok(rc==MMSYSERR_NOERROR,
       "waveInUnprepareHeader: device=%d rc=%s\n",device,wave_in_error(rc));

    rc=waveInClose(win);
    ok(rc==MMSYSERR_NOERROR,
       "waveInClose: device=%d rc=%s\n",device,wave_in_error(rc));
    res=WaitForSingleObject(hevent,1000);
    ok(res==WAIT_OBJECT_0,"WaitForSingleObject failed for close\n");
    free(frag.lpData);
    CloseHandle(hevent);
}

static void wave_in_tests()
{
    WAVEINCAPS caps;
    WAVEFORMATEX format,oformat;
    HWAVEIN win;
    MMRESULT rc;
    UINT ndev,d,f;
    WCHAR * wname;
    CHAR * name;
    DWORD size;
    DWORD dwPageSize;
    BYTE * twoPages;
    SYSTEM_INFO sSysInfo;
    DWORD flOldProtect;
    BOOL res;

    GetSystemInfo(&sSysInfo);
    dwPageSize = sSysInfo.dwPageSize;

    ndev=waveInGetNumDevs();
    trace("found %d WaveIn devices\n",ndev);

    rc=waveInGetDevCapsA(ndev+1,&caps,sizeof(caps));
    ok(rc==MMSYSERR_BADDEVICEID,
       "waveInGetDevCapsA: MMSYSERR_BADDEVICEID expected, got %s\n",wave_in_error(rc));

    rc=waveInGetDevCapsA(WAVE_MAPPER,&caps,sizeof(caps));
    if (ndev>0)
        ok(rc==MMSYSERR_NOERROR,
           "waveInGetDevCapsA: MMSYSERR_NOERROR expected, got %s\n",wave_in_error(rc));
    else
        ok(rc==MMSYSERR_BADDEVICEID || rc==MMSYSERR_NODRIVER,
           "waveInGetDevCapsA: MMSYSERR_BADDEVICEID or MMSYSERR_NODRIVER expected, got %s\n",wave_in_error(rc));

    format.wFormatTag=WAVE_FORMAT_PCM;
    format.nChannels=2;
    format.wBitsPerSample=16;
    format.nSamplesPerSec=44100;
    format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;
    format.nAvgBytesPerSec=format.nSamplesPerSec*format.nBlockAlign;
    format.cbSize=0;
    rc=waveInOpen(&win,ndev+1,&format,0,0,CALLBACK_NULL);
    ok(rc==MMSYSERR_BADDEVICEID,
       "waveInOpen: MMSYSERR_BADDEVICEID expected, got %s\n",wave_in_error(rc));

    for (d=0;d<ndev;d++) {
        rc=waveInGetDevCapsA(d,&caps,sizeof(caps));
        ok(rc==MMSYSERR_NOERROR || rc==MMSYSERR_BADDEVICEID,
           "waveInGetDevCapsA: failed to get capabilities of device %d: rc=%s\n",d,wave_in_error(rc));
        if (rc==MMSYSERR_BADDEVICEID)
            continue;

        name=NULL;
        rc=waveInMessage((HWAVEIN)d, DRV_QUERYDEVICEINTERFACESIZE, (DWORD_PTR)&size, 0);
        ok(rc==MMSYSERR_NOERROR || rc==MMSYSERR_INVALPARAM || rc==MMSYSERR_NOTSUPPORTED,
           "waveInMessage: failed to get interface size for device: %d rc=%s\n",d,wave_in_error(rc));
        if (rc==MMSYSERR_NOERROR) {
            wname = (WCHAR *)malloc(size);
            rc=waveInMessage((HWAVEIN)d, DRV_QUERYDEVICEINTERFACE, (DWORD_PTR)wname, size);
            ok(rc==MMSYSERR_NOERROR,"waveInMessage: failed to get interface name for device: %d rc=%s\n",d,wave_in_error(rc));
            ok(lstrlenW(wname)+1==size/sizeof(WCHAR),"got an incorrect size: %ld instead of %d\n",size,(lstrlenW(wname)+1)*sizeof(WCHAR));
            if (rc==MMSYSERR_NOERROR) {
                name = malloc(size/sizeof(WCHAR));
                WideCharToMultiByte(CP_ACP, 0, wname, size/sizeof(WCHAR), name, size/sizeof(WCHAR), NULL, NULL);
            }
            free(wname);
        } else if (rc==MMSYSERR_NOTSUPPORTED) {
            name=strdup("not supported");
        }

        trace("  %d: \"%s\" (%s) %d.%d (%d:%d): channels=%d formats=%05lx\n",
              d,caps.szPname,(name?name:"failed"),caps.vDriverVersion >> 8,
              caps.vDriverVersion & 0xff,
              caps.wMid,caps.wPid,
              caps.wChannels,caps.dwFormats);

        free(name);

        for (f=0;f<NB_WIN_FORMATS;f++) {
            format.wFormatTag=WAVE_FORMAT_PCM;
            format.nChannels=win_formats[f][3];
            format.wBitsPerSample=win_formats[f][2];
            format.nSamplesPerSec=win_formats[f][1];
            format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;
            format.nAvgBytesPerSec=format.nSamplesPerSec*format.nBlockAlign;
            format.cbSize=0;
            wave_in_test_deviceIn(d,&format,win_formats[f][0],0, &caps);
            wave_in_test_deviceIn(d,&format,win_formats[f][0],WAVE_FORMAT_DIRECT, &caps);
            wave_in_test_deviceIn(d,&format,win_formats[f][0],WAVE_MAPPED, &caps);
        }

        /* Try a PCMWAVEFORMAT aligned next to an unaccessable page for bounds checking */
        twoPages = VirtualAlloc(NULL, 2 * dwPageSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
        ok(twoPages!=NULL,"Failed to allocate 2 pages of memory\n");
        if (twoPages) {
            res = VirtualProtect(twoPages + dwPageSize, dwPageSize, PAGE_NOACCESS, &flOldProtect);
            ok(res, "Failed to set memory access on second page\n");
            if (res) {
                LPWAVEFORMATEX pwfx = (LPWAVEFORMATEX)(twoPages + dwPageSize - sizeof(PCMWAVEFORMAT));
                pwfx->wFormatTag=WAVE_FORMAT_PCM;
                pwfx->nChannels=1;
                pwfx->wBitsPerSample=8;
                pwfx->nSamplesPerSec=22050;
                pwfx->nBlockAlign=pwfx->nChannels*pwfx->wBitsPerSample/8;
                pwfx->nAvgBytesPerSec=pwfx->nSamplesPerSec*pwfx->nBlockAlign;
                wave_in_test_deviceIn(d,pwfx,WAVE_FORMAT_2M08,0, &caps);
                wave_in_test_deviceIn(d,pwfx,WAVE_FORMAT_2M08,WAVE_FORMAT_DIRECT, &caps);
                wave_in_test_deviceIn(d,pwfx,WAVE_FORMAT_2M08,WAVE_MAPPED, &caps);
            }
            VirtualFree(twoPages, 2 * dwPageSize, MEM_RELEASE);
        }

        /* Try invalid formats to test error handling */
        trace("Testing invalid format: 11 bits per sample\n");
        format.wFormatTag=WAVE_FORMAT_PCM;
        format.nChannels=2;
        format.wBitsPerSample=11;
        format.nSamplesPerSec=22050;
        format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;
        format.nAvgBytesPerSec=format.nSamplesPerSec*format.nBlockAlign;
        format.cbSize=0;
        oformat=format;
        rc=waveInOpen(&win,d,&format,0,0,CALLBACK_NULL|WAVE_FORMAT_DIRECT);
        ok(rc==WAVERR_BADFORMAT || rc==MMSYSERR_INVALFLAG || rc==MMSYSERR_INVALPARAM,
           "waveInOpen: opening the device in 11 bit mode should fail %d: rc=%s\n",d,wave_in_error(rc));
        if (rc==MMSYSERR_NOERROR) {
            trace("     got %ldx%2dx%d for %ldx%2dx%d\n",
                  format.nSamplesPerSec, format.wBitsPerSample,
                  format.nChannels,
                  oformat.nSamplesPerSec, oformat.wBitsPerSample,
                  oformat.nChannels);
            waveInClose(win);
        }

        trace("Testing invalid format: 2 MHz sample rate\n");
        format.wFormatTag=WAVE_FORMAT_PCM;
        format.nChannels=2;
        format.wBitsPerSample=16;
        format.nSamplesPerSec=2000000;
        format.nBlockAlign=format.nChannels*format.wBitsPerSample/8;
        format.nAvgBytesPerSec=format.nSamplesPerSec*format.nBlockAlign;
        format.cbSize=0;
        oformat=format;
        rc=waveInOpen(&win,d,&format,0,0,CALLBACK_NULL|WAVE_FORMAT_DIRECT);
        ok(rc==WAVERR_BADFORMAT || rc==MMSYSERR_INVALFLAG || rc==MMSYSERR_INVALPARAM,
           "waveInOpen: opening the device with 2 MHz sample rate should fail %d: rc=%s\n",d,wave_in_error(rc));
        if (rc==MMSYSERR_NOERROR) {
            trace("     got %ldx%2dx%d for %ldx%2dx%d\n",
                  format.nSamplesPerSec, format.wBitsPerSample,
                  format.nChannels,
                  oformat.nSamplesPerSec, oformat.wBitsPerSample,
                  oformat.nChannels);
            waveInClose(win);
        }
    }
}

START_TEST(capture)
{
    wave_in_tests();
}
