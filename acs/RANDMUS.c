#library "RANDMUS"
#include "zcommon.acs"

#define MAX_SUPPORTED_SONGS 999
#define SERVER_MODE_CVAR "mus_servermode"
#define SONGSTR_PREFIX "JUKEBOX_"
#define SONGFILE_PREFIX "SONG"


global int 1:currentSongIndex;
global int 2:playedSongs[];
global bool 3:doneInit;

function int cstrcmp(int s1, int s2)
{
  int i = 0;
  int c1, c2;
  
  while(true) {
    c1 = GetChar(s1, i); c2 = GetChar(s2, i);
    
    if((!c1 && !c2) || (c1 != c2)) { break; }
    
    i++;
  }
  
  return c1 - c2;
}

function int getSongStr(int SongNumber, int PrefixString)
{
    // if the song amount ever goes over 1000, uncomment everything but this line ...
  int OneZero = "0"; int TwoZeros = "00"; // int ThreeZeros = "000";
  int OutputString;
  if(SongNumber <= 9)
  { // if it's less than 10 (1 digit, two zeros)
    OutputString = StrParam(s:PrefixString, s:TwoZeros, d:SongNumber); // concatenate the prefix, two zeros, and the number
  }
  else if(SongNumber <= 99)
  { // if it's less than 100 (2 digits, one zero)
    OutputString = StrParam(s:PrefixString, s:OneZero, d:SongNumber); // concatenate the prefix, one zero, and the number
  }
  else
  { // if it's over 100
    OutputString = StrParam(s:PrefixString, d:SongNumber); // concatenate the prefix and the number
  }
  
  return OutputString;
}

function int getSongInfo(int SongNumber)
{
  return getSongStr(SongNumber, SONGSTR_PREFIX);
}

function int getSongFile(int SongNumber)
{
  return getSongStr(SongNumber, SONGFILE_PREFIX);
}

function int getMaxRandSong(void)
{
  int temp;
  int i;
  
  for(i = 1; i < MAX_SUPPORTED_SONGS; i++)
  { // Please do not use more than 9999 songs.
    temp = getSongInfo(i); // Compare the string and the language text, if they're the same it's the last one.
    if(!CStrCmp(temp, StrParam(l:temp)))
    {
      return i - 1;
    }
  }
  
  return -1;
}

function void silence(bool serverMode)
{
  if(serverMode) {
    SetMusic("silence");
  }
  else {
    LocalSetMusic("silence");
  }
  
}

function int playSong(int songId, bool serverMode) 
{
  if(songId > 0) 
  {
    if (serverMode) {
      SetMusic(getSongFile(songId),0);
    }
    else {
      LocalSetMusic(getSongFile(songId),0);  
    }
    return 0;
  }
  else {
    if (serverMode) {
      SetMusic("*",0);
    }
    else {
      LocalSetMusic("*",0);  
    }
    return 0;
  }
  return 1;
}

function int getCurrentSong(void)
{
  return playedSongs[currentSongIndex];
}

function int getNextSong(void) 
{
  int songId;
  // Too much history, not bothering to shift array
  if(currentSongIndex+1 >= MAX_SUPPORTED_SONGS) 
  {
    //too much, i give up
    return getRandomSongId();
  }

  // First song case
  if(playedSongs[currentSongIndex] == 0) 
  {
    songId = getRandomSongId();
    playedSongs[currentSongIndex] = songId;
    return songId;
  }

  // Traversing a previously generated song
  if(playedSongs[currentSongIndex+1] > 0) 
  {
    currentSongIndex += 1;
    return playedSongs[currentSongIndex];
  }
  else // Need to generate a new song
  {
    songId = getRandomSongId();
    playedSongs[currentSongIndex+1] = songId;
    currentSongIndex += 1;
    return songId;
  }
  return 0;
}

function int getPreviousSong(void) {
  if(currentSongIndex-1 >= 0) {
    currentSongIndex -= 1;
    return playedSongs[currentSongIndex];
  }
  return 0;
}

function int showInfo(int songId, bool serverMode) 
{
  return showInfoCond(songId, true, serverMode);
}

function int showInfoCond(int songId, bool condition, bool serverMode) 
{
  if(condition)
  {
      int infoId = getSongInfo(songId);
      if(CStrCmp(infoId, StrParam(l:infoId)))
      {
        SetFont("SmallFont");
        if(serverMode)
        {
          HudMessageBold(l:infoId; HUDMSG_FADEINOUT | HUDMSG_LOG, 153, CR_WHITE, 0.1, 0.8, 3.0, 0.5, 1.0);
        }
        else {
          HudMessage(l:infoId; HUDMSG_FADEINOUT | HUDMSG_LOG, 153, CR_WHITE, 0.1, 0.8, 3.0, 0.5, 1.0);  
        }
        
      }
      return 0;
  }
    return 1;
}


function int getRandomSongId(void) {
  int randSong = 0;
  randSong = random(1, getMaxRandSong());
  return randSong;
}

function void defaultPlayedSongs(int start) {
  int i;
  for (i = start; i < MAX_SUPPORTED_SONGS; i++) {
      playedSongs[i] = 0;
  }
}

// START SONGINFO
Script 340 (void) NET
{
  int songId = getCurrentSong();
  if(GetCvar(SERVER_MODE_CVAR) == 1) {
    ACS_Execute(341, 0, songId, 1, 0);
  }
  else {
    ACS_Execute(341, 0, songId, 0, 0);  
  }
}
Script 341 (int songId, int serverMode) NET clientside
{
  if (serverMode) {
    showInfo(songId, false);  
  }
  else {
    showInfo(getCurrentSong(), false);
  }
}
// END SONGINFO

// Display the songinfo when the player enters
Script 342 ENTER clientside
{
  if(GetCvar(SERVER_MODE_CVAR) == 1) {
    RequestScriptPuke(340, 0, 0, 0);
  }
  else {
    ACS_Execute(340, 0, 0, 0, 0);  
  }
  
}

// START INIT
Script 344 OPEN
{
  if(GetCvar(SERVER_MODE_CVAR) == 1)
  {
    if(!doneInit) {
      defaultPlayedSongs(0);  
      doneInit = true;
    }
    
    silence(true);
    int songId = getNextSong();
    playSong(songId, true);
  }
}
Script 345 OPEN clientside
{
  if(GetCvar(SERVER_MODE_CVAR) != 1)
  {
    if(!doneInit) {
      defaultPlayedSongs(0);  
      doneInit = true;
    }
    
    silence(true);
    int songId = getNextSong();
    playSong(songId, true);
  }
}
// END INIT

// START CONTROL
script 346 (int mode, int serverMode)
{
  int songId;
  switch (mode)
  {
    case 0: // Next song
      silence(serverMode);
      LocalAmbientSound("music/shift",127);
      Delay(35);
      songId = getNextSong();
      playSong(songId, serverMode);
      Delay(35);
      showInfo(songId, serverMode);
      break;
    case 1: // Previous song
      songId = getPreviousSong();
      if(songId > 0) { // Don't stop the music if there are no previous songs
        silence(serverMode);
        LocalAmbientSound("music/shift",127);
        Delay(35);
        playSong(songId, serverMode);
        Delay(35);
        showInfo(songId, serverMode);
      }
      break;
    case 2: // Default Song
      silence(serverMode);
      LocalAmbientSound("music/shift",127);
      Delay(35);
      playSong(0,serverMode);
      Delay(35);
      showInfo(0,serverMode);
      
      break;
  }
}

script 347 (int mode)
{
  if(GetCvar(SERVER_MODE_CVAR) == 1) {
    ACS_Execute(346, 0, mode, 1, 0);  
  }
}
script 348 (int mode) NET clientside
{
  if(GetCvar(SERVER_MODE_CVAR) != 1) {
    ACS_Execute(346, 0, mode, 0, 0);  
  }
}
//END CONTROL

// START MANUAL SONG
script 349 (int songId, int serverMode)
{
  if(songId > 0){
    if(currentSongIndex+1 < MAX_SUPPORTED_SONGS) {
      currentSongIndex += 1;
      playedSongs[currentSongIndex] = songId;
      defaultPlayedSongs(currentSongIndex+1); // Empty out saved history for songs after this one
    }
  }
  playSong(songId, serverMode);
  showInfo(songId, serverMode);
}
script 350 (int songId) // Manually changing the song
{
  if(GetCvar(SERVER_MODE_CVAR) == 1) {
    ACS_Execute(349, 0, songId, 1, 0);  
  }
}

script 351 (int songId) NET clientside // Manually changing the song
{
  if(GetCvar(SERVER_MODE_CVAR) != 1) {
    ACS_Execute(349, 0, songId, 0, 0);  
  }
}

// END MANUAL SONG