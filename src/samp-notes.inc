/* 	
	samp-notes.inc by Leap
	For support send me a DM in Discord: leeeap
*/

#if defined _SampNotes_included
#endinput
#endif

#define _SampNotes_included

#if !defined MAX_NOTIFICATION_STACK
#define MAX_NOTIFICATION_STACK 5
#endif

#if !defined MAX_NOTIFICATION_BUFFER
#define MAX_NOTIFICATION_BUFFER 400
#endif

#define NOTIFICATION_ANCHOR_TOP 2
#define NOTIFICATION_ANCHOR_BOTTOM 1

enum E_NOTIFICATION_CONTAINER
{
	nUID,
	PlayerText:nTextDrawHandle,
	PlayerText:nIconHandle,
	nTextBuffer[MAX_NOTIFICATION_BUFFER],
	nIconBuffer[MAX_NOTIFICATION_BUFFER],
	nExpirationTimestamp,
	nTimerHandle,
	nClassificationID,
	nLineHeight,
	nBackgroundColor,
	nForegroundColor,
	nNotificationType
}

#define UNIX_EPOCH_YEAR 1970

new
	__NotificationRegistry[MAX_PLAYERS][MAX_NOTIFICATION_STACK][E_NOTIFICATION_CONTAINER],
	__GlobalNotificationUID = 0,
	Float:__NotificationAnchorX = 497.0,
	Float:__NotificationAnchorY = 111.0,
	__NotificationAnchor = NOTIFICATION_ANCHOR_BOTTOM,
	__NotificationAudioID = 1149,
    __NotificationThrottleMap[MAX_PLAYERS][1]
;

stock __FindNotificationByUID(playerid, uid)
{
	for(new i = 0; i < MAX_NOTIFICATION_STACK; i++)
	{
		if(__NotificationRegistry[playerid][i][nUID] == uid) return i;
	}
	return -1;
}

stock __AcquireNotificationSlot(playerid)
{
	for(new i = 0; i < MAX_NOTIFICATION_STACK; i++)
	{
		if(__NotificationRegistry[playerid][i][nUID] == 0) {
			return i;
		}
	}
	return -1;
}

stock __CalculateStringLineCount(const string[])
{
	new lines_count = 1;
	lines_count = strlen(string)/23;
	return lines_count;
}

stock __GetLines(const buffer[])
{
    new count = 0;
    new len = strlen(buffer);

    for (new i = 0; i < len; i++)
    {
        if (buffer[i] == '\n')
        {
            count++;
        }
    }
    return count;
}

stock SampNotes_SetAudioID(sound)
{
	__NotificationAudioID = sound;
	return 1;
}

stock SampNotes_SetAnchor(type)
{
	switch(type)
	{
		case NOTIFICATION_ANCHOR_TOP, NOTIFICATION_ANCHOR_BOTTOM:
		{
			__NotificationAnchor = type;
			return 1;
		}
	}
	return 0;
}

stock SampNotes_SetPosition(Float:pos_x, Float:pos_y)
{
	__NotificationAnchorX = pos_x;
	__NotificationAnchorY = pos_y;
	return 1;
}

static stock bool:__IsLeapYear(year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

static const __MONTH_DAYS[12] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

stock __GetUnixTimestamp(year = 0, month = 0, day = 0, hour = 0, minute = 0, second = 0, hourGMT = 0, minuteGMT = 0) {
    static const
		SECONDS_PER_MINUTE = 60,
		SECONDS_PER_HOUR = 3600,
		SECONDS_PER_DAY = 86400;

	new timestamp = 0;

	if (year == 0 || month == 0 || day == 0) {
	    getdate(year, month, day);
	    gettime(hour, minute, second);
	}

	for (new i = UNIX_EPOCH_YEAR; i < year; i++)
	    day += (__IsLeapYear(i) == true) ? 365 : 366;

	for (new i = 1; i < month; i++)
		day += __MONTH_DAYS[i - 1];

	if (__IsLeapYear(year) == true && month > 2)
	    day += 1;

    timestamp += ((day - 1) * SECONDS_PER_DAY);
    timestamp += ((hour + hourGMT) * SECONDS_PER_HOUR);
    timestamp += ((minute + minuteGMT) * SECONDS_PER_MINUTE);
    timestamp += second;

	return timestamp;
}

stock SampNotes_Push(playerid, const icon[], const text[], time = 0, color = -1, bg_color = 255, sound = 1, const size = sizeof(text), tipo = 0, noti = 90)
{
	if(noti < 10 && __NotificationThrottleMap[playerid][noti] != 0) return 1;

	#pragma unused size
	static
	slot_index,
	lines_count,
	Float:size_bg,
	Float:pos_y;

	slot_index = __AcquireNotificationSlot(playerid);		
	if(slot_index == -1) return 0;

    new TextBuffer[200];
	format(TextBuffer, sizeof(TextBuffer), "%s", icon);
	format(__NotificationRegistry[playerid][slot_index][nIconBuffer], MAX_NOTIFICATION_BUFFER, TextBuffer);
	
    format(TextBuffer, sizeof(TextBuffer), "%s", text);
	format(__NotificationRegistry[playerid][slot_index][nTextBuffer], MAX_NOTIFICATION_BUFFER, TextBuffer);
	
    lines_count = __CalculateStringLineCount(TextBuffer);

	if(tipo == 1) lines_count = 3;
	if(tipo == 2) lines_count = 3;
	if(tipo == 3) lines_count = 4;

	size_bg = 1.2 + (lines_count * 1.4);

	pos_y = __NotificationAnchorY;
	for(new i; i < slot_index; i++)
	{
		if(__NotificationAnchor == NOTIFICATION_ANCHOR_BOTTOM) {
			pos_y += 13.0 + (__NotificationRegistry[playerid][i][nLineHeight] * 16.0) + 6.0;
		} else {
			pos_y -= 13.0 + (__NotificationRegistry[playerid][i][nLineHeight] * 16.0) + 6.0;
		}
	}
	if(lines_count > 1 && __NotificationAnchor == NOTIFICATION_ANCHOR_TOP) {
		pos_y -= (lines_count - 1) * 16.0;
	}
	
	__NotificationRegistry[playerid][slot_index][nNotificationType] = tipo;
	__NotificationRegistry[playerid][slot_index][nLineHeight] = lines_count;
	__NotificationRegistry[playerid][slot_index][nBackgroundColor] = bg_color;
	__NotificationRegistry[playerid][slot_index][nForegroundColor] = color;
	
	new PlayerText:temp_td;

	if(strfind(__NotificationRegistry[playerid][slot_index][nIconBuffer], "none", true) != -1)
	{
		format(TextBuffer, sizeof(TextBuffer), "%s", __CharsetNormalize(__NotificationRegistry[playerid][slot_index][nTextBuffer]));	
	}
	else
	{
		format(TextBuffer, sizeof(TextBuffer), "____%s", __CharsetNormalize(TextBuffer));
	}

	temp_td = CreatePlayerTextDraw(playerid, __NotificationAnchorX, pos_y+lines_count, TextBuffer);
	PlayerTextDrawLetterSize(playerid, temp_td, 0.271250, 1.383465);
	PlayerTextDrawTextSize(playerid, temp_td, 607.500000, size_bg);
	PlayerTextDrawAlignment(playerid, temp_td, 1);
	PlayerTextDrawColor(playerid, temp_td, color);
	PlayerTextDrawUseBox(playerid, temp_td, true);
	PlayerTextDrawSetShadow(playerid, temp_td, 0);
	PlayerTextDrawSetOutline(playerid, temp_td, 0);
	PlayerTextDrawBackgroundColor(playerid, temp_td, 100996095);
	PlayerTextDrawBoxColor(playerid, temp_td, 0x1a1a1aFF);
	PlayerTextDrawFont(playerid, temp_td, 1);
	PlayerTextDrawSetProportional(playerid, temp_td, 1);
	PlayerTextDrawShow(playerid, temp_td);
	
	__GlobalNotificationUID++;
	__NotificationRegistry[playerid][slot_index][nTextDrawHandle] = temp_td;

	if(strfind(__NotificationRegistry[playerid][slot_index][nIconBuffer], "none", true) != -1)
	{
		//
	}
	else
	{
		format(TextBuffer, sizeof(TextBuffer), "%s", __CharsetNormalize(__NotificationRegistry[playerid][slot_index][nIconBuffer]));
		temp_td = CreatePlayerTextDraw(playerid, __NotificationAnchorX, pos_y+1.3, TextBuffer);
		PlayerTextDrawLetterSize(playerid, temp_td, 0.000000, 0.000000);
		PlayerTextDrawTextSize(playerid, temp_td, 13.125000, 13.125000);
		PlayerTextDrawAlignment(playerid, temp_td, 1);
		PlayerTextDrawColor(playerid, temp_td, -1);
		PlayerTextDrawSetShadow(playerid, temp_td, 0);
		PlayerTextDrawSetOutline(playerid, temp_td, 0);
		PlayerTextDrawFont(playerid, temp_td, 4);
		PlayerTextDrawShow(playerid, temp_td);
		__NotificationRegistry[playerid][slot_index][nIconHandle] = temp_td;
	}

	if(time == 0) {
		__NotificationRegistry[playerid][slot_index][nExpirationTimestamp] = 0;
		__NotificationRegistry[playerid][slot_index][nTimerHandle] = 0;
	} else {
		__NotificationRegistry[playerid][slot_index][nExpirationTimestamp] = time + __GetUnixTimestamp();
		__NotificationRegistry[playerid][slot_index][nClassificationID] = noti;
		if(noti < 10 && noti != 90) __NotificationThrottleMap[playerid][noti] = 6;
		__NotificationRegistry[playerid][slot_index][nTimerHandle] = SetTimerEx("__SampNotes_Destroy", time * 1000, false, "dd", playerid, __GlobalNotificationUID);
	}
	
	if(sound) {
		PlayerPlaySound(playerid, __NotificationAudioID, 0.0, 0.0, 0.0);
	}
	
	__NotificationRegistry[playerid][slot_index][nUID] = __GlobalNotificationUID;

	return __GlobalNotificationUID;
}

forward __SampNotes_Destroy(playerid, uid);
public __SampNotes_Destroy(playerid, uid)
{
	static pos;
	pos = __FindNotificationByUID(playerid, uid);
	if(pos == -1) return 1;

	if(__NotificationRegistry[playerid][pos][nTimerHandle] != 0) {
		KillTimer(__NotificationRegistry[playerid][pos][nTimerHandle]);
	}

	PlayerTextDrawHide(playerid, __NotificationRegistry[playerid][pos][nTextDrawHandle]);
	PlayerTextDrawDestroy(playerid, __NotificationRegistry[playerid][pos][nTextDrawHandle]);

	__NotificationRegistry[playerid][pos][nUID] = 0;

	if(strfind(__NotificationRegistry[playerid][pos][nIconBuffer], "none", true) != -1)
	{
		// nothing
	}
	else
	{
		PlayerTextDrawHide(playerid, __NotificationRegistry[playerid][pos][nIconHandle]);
		PlayerTextDrawDestroy(playerid, __NotificationRegistry[playerid][pos][nIconHandle]);
	}
 		
 	if(__NotificationRegistry[playerid][pos][nClassificationID] < 10) { 
		__NotificationThrottleMap[playerid][__NotificationRegistry[playerid][pos][nClassificationID]] = 0; 
	}
	
	__NotificationRegistry[playerid][pos][nClassificationID] = 90;
	
	if(pos + 1 >= MAX_NOTIFICATION_STACK) return 1;
	
	for(new j = pos + 1, time; j < MAX_NOTIFICATION_STACK; j++)
	{
		if(__NotificationRegistry[playerid][j][nUID] == 0) continue;
		
		if(__NotificationRegistry[playerid][j][nExpirationTimestamp] == 0) {
			SampNotes_Push(
				playerid,
				__NotificationRegistry[playerid][j][nIconBuffer], 
				__NotificationRegistry[playerid][j][nTextBuffer], 
				0,
				__NotificationRegistry[playerid][j][nForegroundColor], 
				__NotificationRegistry[playerid][j][nBackgroundColor], 
				0,
				300,
				__NotificationRegistry[playerid][j][nNotificationType],
				__NotificationRegistry[playerid][j][nClassificationID]
			);
		} else {
			time = __NotificationRegistry[playerid][j][nExpirationTimestamp] - __GetUnixTimestamp();
			if(time > 0) {
				SampNotes_Push(
					playerid,
					__NotificationRegistry[playerid][j][nIconBuffer], 
					__NotificationRegistry[playerid][j][nTextBuffer], 
					time,
					__NotificationRegistry[playerid][j][nForegroundColor], 
					__NotificationRegistry[playerid][j][nBackgroundColor], 
					0,
					300,
					__NotificationRegistry[playerid][j][nNotificationType],
					__NotificationRegistry[playerid][j][nClassificationID]
				);
			}
		}
		__SampNotes_Destroy(playerid, __NotificationRegistry[playerid][j][nUID]);
		return 1;
	}
	return 1;
}

// Spanish accents normalization for textdraws and notifications
stock __CharsetNormalize(const string[])
{
	new
	szFixed[1024],
	iPos,
	iLen;

	for (iLen = strlen(string); iPos < iLen; iPos++)
	{
		switch (string[iPos])
		{
			case 'à':   szFixed[iPos] = 151;
			case 'á':   szFixed[iPos] = 152;
			case 'â':   szFixed[iPos] = 153;
			case 'ä':   szFixed[iPos] = 154;
			case 'À':   szFixed[iPos] = 128;
			case 'Á':   szFixed[iPos] = 129;
			case 'Â':   szFixed[iPos] = 130;
			case 'Ä':   szFixed[iPos] = 131;
			case 'è':   szFixed[iPos] = 157;
			case 'é':   szFixed[iPos] = 158;
			case 'ê':   szFixed[iPos] = 159;
			case 'ë':   szFixed[iPos] = 160;
			case 'È':   szFixed[iPos] = 134;
			case 'É':   szFixed[iPos] = 135;
			case 'Ê':   szFixed[iPos] = 136;
			case 'Ë':   szFixed[iPos] = 137;
			case 'ì':   szFixed[iPos] = 161;
			case 'í':   szFixed[iPos] = 162;
			case 'î':   szFixed[iPos] = 163;
			case 'ï':   szFixed[iPos] = 164;
			case 'Ì':   szFixed[iPos] = 138;
			case 'Í':   szFixed[iPos] = 139;
			case 'Î':   szFixed[iPos] = 140;
			case 'Ï':   szFixed[iPos] = 141;
			case 'ò':   szFixed[iPos] = 165;
			case 'ó':   szFixed[iPos] = 166;
			case 'ô':   szFixed[iPos] = 167;
			case 'ö':   szFixed[iPos] = 168;
			case 'Ò':   szFixed[iPos] = 142;
			case 'Ó':   szFixed[iPos] = 143;
			case 'Ô':   szFixed[iPos] = 144;
			case 'Ö':   szFixed[iPos] = 145;
			case 'ù':   szFixed[iPos] = 169;
			case 'ú':   szFixed[iPos] = 170;
			case 'û':   szFixed[iPos] = 171;
			case 'ü':   szFixed[iPos] = 172;
			case 'Ù':   szFixed[iPos] = 146;
			case 'Ú':   szFixed[iPos] = 147;
			case 'Û':   szFixed[iPos] = 148;
			case 'Ü':   szFixed[iPos] = 149;
			case 'ñ':   szFixed[iPos] = 174;
			case 'Ñ':   szFixed[iPos] = 173;
			case '¡':   szFixed[iPos] = 64;
			case '¿':   szFixed[iPos] = 175;
			case '`':   szFixed[iPos] = 177;
			case '&':   szFixed[iPos] = 38;
			default:    szFixed[iPos] = string[iPos];
		}
	}
	return szFixed;
}


// Quick function to show a notification without icon and default settings
stock ShowPlayerNotification(playerid, const text_[])
{
	new mensaje[250];
	format(mensaje, sizeof(mensaje), "%s", __CharsetNormalize(text_));
	if(IsPlayerConnected(playerid) && !IsPlayerNPC(playerid)) 
		SampNotes_Push(playerid, "none", mensaje, 5, 0xFFFFFFDD, 0x000000FF);
	return 1;
}

stock SampNotes_PushFormatted(playerid, const text[], {Float, _}:...) {
	static
	    args,
	    str[192];

	if ((args = numargs()) <= 2) {
	    SampNotes_Display(playerid, text);
	} else {
		while (--args >= 2) {
			#emit LCTRL 5
			#emit LOAD.alt args
			#emit SHL.C.alt 2
			#emit ADD.C 12
			#emit ADD
			#emit LOAD.I
			#emit PUSH.pri
		}
		#emit PUSH.S text
		#emit PUSH.C 192
		#emit PUSH.C str
		#emit LOAD.S.pri 8
		#emit CONST.alt 4
		#emit ADD
		#emit PUSH.pri
		#emit SYSREQ.C format
		#emit LCTRL 5
		#emit SCTRL 4

        SampNotes_Push(playerid, "none", str, 10, 0xFFFFFFDD, 0x000000FF);

		#emit RETN
	}
	return 1;
}