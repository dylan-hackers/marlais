/*

   macintosh.c

   This software is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This software is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with this software; if not, write to the Free
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Original copyright notice follows:

   Dylan primitives for Macintosh.

   Copyright (C) 1994, Patrick C. Beard.  All Rights Reserved.

   Permission to use, copy, and modify this software and its
   documentation is hereby granted only under the following terms and
   conditions.  Both the above copyright notice and this permission
   notice must appear in all copies of the software, derivative works
   or modified version, and both notices must appear in supporting
   documentation.  Users of this software agree to the terms and
   conditions set forth in this notice.

 */

#include "macintosh.h"
#include "string.h"
#include "number.h"
#include "values.h"
#include "error.h"
#include "foreign_ptr.h"

#include <StandardFile.h>
#include <Strings.h>
#include <string.h>
#include <Files.h>

static Object beep (void);
static Object new_menu (Object title, Object id, Object items);
static Object dispose_menu (Object menu);
static Object insert_menu (Object menu);
static Object delete_menu (Object menu);
static Object draw_menus ();

static struct primitive mac_prims[] =
{
    {"beep", prim_0, beep},
    {"get-file", prim_0, get_file},
    {"put-file", prim_0_2, put_file},
    {"%new-menu", prim_3, new_menu},
    {"%dispose-menu", prim_1, dispose_menu},
    {"%insert-menu", prim_1, insert_menu},
    {"%delete-menu", prim_1, delete_menu},
    {"%draw-menus", prim_0, draw_menus},
};

void
init_mac_prims ()
{
    int num = sizeof (mac_prims) / sizeof (struct primitive);

    init_prims (num, mac_prims);
}

static Object
beep ()
{
    SysBeep (1);
    return unspecified_object;
}

Object
get_file ()
{
    StandardFileReply reply;
    SFTypeList types =
    {'TEXT'};

    StandardGetFile (nil, 1, types, &reply);
    if (reply.sfGood) {
	char path[256];

	return make_byte_string (FSSpecToPath (&reply.sfFile, path));
    }
    return false_object;
}

Object
put_file (Object defaultNameObj, Object promptObj)
{
    char defaultName[256], prompt[256];
    StandardFileReply reply;

    defaultName[0] = 0;
    if (defaultNameObj != NULL) {
	if (BYTESTRP (defaultNameObj))
	    strcpy (defaultName, BYTESTRVAL (defaultNameObj));
	else
	    error ("put-file: default name should be a <string> not", defaultNameObj, NULL);
    }
    prompt[0] = 0;
    if (promptObj != NULL) {
	if (BYTESTRP (promptObj))
	    strcpy (prompt, BYTESTRVAL (promptObj));
	else
	    error ("put-file: prompt should be a <string> not", promptObj, NULL);
    }
    StandardPutFile (c2pstr (prompt), c2pstr (defaultName), &reply);
    if (reply.sfGood) {
	FSSpec *file = &reply.sfFile;

	return make_byte_string (FSSpecToPath (&reply.sfFile, defaultName));
    }
    return false_object;
}

static char *
dir_path (char path[256], long dirID, short vRefNum)
{
    static CInfoPBRec info;
    Str32 dirName;
    OSErr result;

    info.dirInfo.ioNamePtr = dirName;
    info.dirInfo.ioVRefNum = vRefNum;
    info.dirInfo.ioFDirIndex = -1;
    info.dirInfo.ioDrDirID = dirID;
    result = PBGetCatInfoAsync (&info);

    // when we reach the root directory, we terminate the recursion.
    if (dirID == 2)
	return strcat (strcpy (path, p2cstr (dirName)), ":");
    else
	return strcat (strcat (dir_path (path, info.dirInfo.ioDrParID, vRefNum), p2cstr (dirName)), ":");
}

char *
FSSpecToPath (FSSpec * file, char path[256])
{
    return strcat (dir_path (path, file->parID, file->vRefNum), p2cstr (file->name));
}

/* to make error.c happy. */

char *sys_siglist[32] =
{
    "",
    "hangup",
    "interrupt",
    "quit",
    "illegal instruction (not reset when caught)",
    "trace trap (not reset when caught)",
    "IOT instruction",
    "EMT instruction",
    "floating point exception",
    "kill (cannot be caught or ignored)",
    "bus error",
    "segmentation violation",
    "bad argument to system call",
    "write on a pipe with no one to read it",
    "alarm clock",
    "software termination signal from kill",
    "urgent condition on IO channel",
    "sendable stop signal not from tty",
    "stop signal from tty",
    "continue a stopped process",
    "to parent on child stop or exit",
    "to readers pgrp upon background tty read",
    "like TTIN for output if (tp->t_local&LTOSTOP)",
    "input/output possible signal",
    "exceeded CPU time limit",
    "exceeded file size limit",
    "virtual time alarm",
    "profiling time alarm",
    "window changed",
    "resource lost (eg, record-lock lost)",
    "user defined signal 1",
    "user defined signal 2",
};

// menu manager functions.

static Object
new_menu (Object title, Object id, Object items)
{
    Str255 str;
    MenuHandle menu;

    if (!BYTESTRP (title)) {
	error ("%new-menu: first parameter must be a <string> not ", title, NULL);
    }
    memcpy (str + 1, BYTESTRVAL (title), BYTESTRSIZE (title));
    str[0] = BYTESTRSIZE (title);

    menu = NewMenu (INTVAL (id), str);
    if (!menu) {
	error ("%new-menu: NewMenu failed.", NULL);
    }
    // add all items (strings) in the list.
    while (!EMPTYLISTP (items)) {
	Object item = CAR (items);
	int length = BYTESTRSIZE (item);

	memcpy (str + 1, BYTESTRVAL (item), length);
	str[0] = length;
	AppendMenu (menu, str);
	items = CDR (items);
    }

    // wrap the menu handle in a foreign pointer object.
    return make_foreign_ptr (menu);
}

static Object
dispose_menu (Object menuObj)
{
    MenuHandle menu;

    if (!FOREIGNP (menuObj)) {
	error ("%dispose-menu: menu not a foreign pointer.", menuObj, NULL);
    }
    menu = (MenuHandle) FOREIGNPTR (menuObj);
    DisposeMenu (menu);

    return unspecified_object;
}

static Object
insert_menu (Object menuObj)
{
    MenuHandle menu;

    if (!FOREIGNP (menuObj)) {
	error ("%insert-menu: menu not a foreign pointer.", menuObj, NULL);
    }
    menu = (MenuHandle) FOREIGNPTR (menuObj);
    InsertMenu (menu, 0);

    return unspecified_object;
}

static Object
delete_menu (Object menuObj)
{
    MenuHandle menu;

    if (!FOREIGNP (menuObj)) {
	error ("%delete-menu: menu not a foreign pointer.", menuObj, NULL);
    }
    menu = (MenuHandle) FOREIGNPTR (menuObj);
    DeleteMenu ((**menu).menuID);

    return unspecified_object;
}

static Object
draw_menus ()
{
    DrawMenuBar ();
    return unspecified_object;
}
